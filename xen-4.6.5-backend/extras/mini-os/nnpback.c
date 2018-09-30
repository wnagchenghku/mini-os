#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/events.h>
#include <errno.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <xen/io/protocols.h>
#include <mini-os/xmalloc.h>
#include <time.h>
#include <mini-os/lib.h>
#include <fcntl.h>
#include <mini-os/mm.h>
#include <mini-os/posix/sys/mman.h>

#include <mini-os/nnpback.h>
#include <mini-os/utlist.h>

#define NNPBACK_PRINT_DEBUG
#ifdef NNPBACK_PRINT_DEBUG
#define NNPBACK_DEBUG(fmt,...) printk("Nnpback:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#define NNPBACK_DEBUG_MORE(fmt,...) printk(fmt, ##__VA_ARGS__)
#else
#define NNPBACK_DEBUG(fmt,...)
#endif
#define NNPBACK_ERR(fmt,...) printk("Nnpback:Error " fmt, ##__VA_ARGS__)
#define NNPBACK_LOG(fmt,...) printk("Nnpback:Info " fmt, ##__VA_ARGS__)

typedef struct el {
    domid_t domid;
    grant_ref_t *grant_ref;
    struct el *next, *prev;
} el;

int namecmp(el *a, el *b) {
    return a->domid == b->domid ? 0 : -1;
}

struct nnpback_dev {

   struct gntmap map;

   xenbus_event_queue events;
};
typedef struct nnpback_dev nnpback_dev_t;

enum { EV_NONE, EV_NEWFE, EV_CLOSEFE } tpm_ev_enum;

/* Global objects */
static struct thread* eventthread = NULL;
static nnpback_dev_t gtpmdev = {
   .events = NULL,
};

/* parses the string that comes out of xenbus_watch_wait_return. */
static int parse_eventstr(const char* evstr, domid_t* domid, char* model)
{
   char* err;
   char* value;
   unsigned int udomid = 0;

  if (sscanf(evstr, "/local/domain/frontend/%u", &udomid) == 1) {
      *domid = udomid;
      if((err = xenbus_read(XBT_NIL, evstr, &value))) {
         free(err);
         return EV_NONE;
      }

      sscanf(value, "%s", model);
      free(value);
      if (strcmp(model, "close") == 0) {
         return EV_CLOSEFE;
      }
      return EV_NEWFE;
   }
   return EV_NONE;
}

int log2(int v)
{
   if (v == 1)
      return 0;

   return 1 + log2(v >> 1);
}

unsigned int round_up_power_of_two(unsigned int v) // compute the next highest power of 2 of 32-bit v
{
   v--;
   v |= v >> 1;
   v |= v >> 2;
   v |= v >> 4;
   v |= v >> 8;
   v |= v >> 16;
   v++;

   return v;
}

el *head = NULL; /* important- initialize to NULL! */
static void *page = NULL;

#define TOTAL_PAGE 100

float model_param[TOTAL_PAGE][1024];

void handle_backend_event(char* evstr) {
   domid_t domid;
   int event;
   char *err;
   int i, j, k = 0;
   char model[16], frontend_path[32];
   char entry_path[64], entry_value[1024];
   char state_path[64], state_value[8];
   grant_ref_t *grant_ref;

   struct timeval start, end;
   unsigned long e_usec;
   el *name, *elt, etmp;

   NNPBACK_DEBUG("Xenbus Event: %s\n", evstr);

   event = parse_eventstr(evstr, &domid, model);
   
   if (event == EV_NEWFE) {
      snprintf(frontend_path, 32, "/local/domain/backend/%d", domid);
      if((err = xenbus_write(XBT_NIL, frontend_path, "0"))) {
         NNPBACK_ERR("Unable to write frontend domain id, error was %s\n", err);
         free(err);
      }

      if (page == NULL) {
            page = (void*)alloc_pages(log2(round_up_power_of_two(TOTAL_PAGE)));
            for (i = 0; i < TOTAL_PAGE; ++i) {
               for (j = 0; j < 1024; ++j) {
                  *((float*)page + k++) = model_param[i][j];
               }
            }
      }

      grant_ref = (grant_ref_t*)malloc(sizeof(grant_ref_t) * TOTAL_PAGE);

      gettimeofday(&start, 0);
      for (i = 0; i < TOTAL_PAGE; ++i) {
         grant_ref[i] = gnttab_grant_access(domid, virt_to_mfn((uintptr_t)page + i * PAGE_SIZE), 0);
      }
      gettimeofday(&end, 0);
      e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);
      NNPBACK_LOG("Publishing grant references takes %lu microseconds\n", e_usec);     
 
      snprintf(entry_value, 1024, "%s", "");
      for (i = 0; i < TOTAL_PAGE; ++i) {
            snprintf(entry_value + strlen(entry_value), 1024 - strlen(entry_value), "%lu ", (unsigned long)grant_ref[i]);
      }

      snprintf(entry_path, 64, "%s/grant-ref", frontend_path);
      if((err = xenbus_write(XBT_NIL, entry_path, entry_value))) {
         NNPBACK_ERR("Unable to write ring-ref, error was %s\n", err);
         free(err);
      }

      snprintf(state_path, 64, "%s/state", frontend_path);
      snprintf(state_value, 8, "%d", 1);
      if((err = xenbus_write(XBT_NIL, state_path, state_value))) {
          NNPBACK_ERR("Unable to write state path, error was %s\n", err);
          free(err);
      }

      name = (el *)malloc(sizeof *name);
      name->domid = domid;
      name->grant_ref = grant_ref;
      DL_APPEND(head, name);
   } else if (event == EV_CLOSEFE) {
      etmp.domid = domid;
      DL_SEARCH(head, elt, &etmp, namecmp);
      for (i = 0; i < TOTAL_PAGE; ++i) {
         gnttab_end_access(elt->grant_ref[i]);
      }
      free(elt->grant_ref);
      DL_DELETE(head, elt);
      free(elt);
   }
}

static void event_listener(void)
{
   const char* bepath = "/local/domain/frontend";
   char **path;
   char* err;

   /* Setup the backend device watch */
   if((err = xenbus_watch_path_token(XBT_NIL, bepath, bepath, &gtpmdev.events)) != NULL) {
      NNPBACK_ERR("xenbus_watch_path_token(%s) failed with error %s!\n", bepath, err);
      free(err);
      goto egress;
   }

   /* Wait and listen for changes in frontend connections */
   while(1) {
      path = xenbus_wait_for_watch_return(&gtpmdev.events);

      handle_backend_event(*path);
      free(path);
   }

   if((err = xenbus_unwatch_path_token(XBT_NIL, bepath, bepath)) != NULL) {
      free(err);
   }
egress:
   return;
}

void event_thread(void* p) {
   event_listener();
}

void init_nnpback(void)
{
   char* err;
   char value[16];
   int i, j;

   printk("============= Init NNP BACK ================\n");

   for (i = 0; i < TOTAL_PAGE; ++i) {
      for (j = 0; j < 1024; ++j) {
         model_param[i][j] = 0.1 + j + i;
      }
   }

   gnttab_reset_model();

   snprintf(value, 16, "%d", xenbus_get_self_id());
   if ((err = xenbus_write(XBT_NIL, "/local/domain/backend", value)))
   {
      NNPBACK_ERR("Unable to write backend id: %s\n", err);
      free(err);
   }

   eventthread = create_thread("nnpback-listener", event_thread, NULL);

}