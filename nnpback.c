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

#include <mini-os/P4C8732DB-backend.h>

#define NNPBACK_PRINT_DEBUG
#ifdef NNPBACK_PRINT_DEBUG
#define NNPBACK_DEBUG(fmt,...) printk("Nnpback:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#define NNPBACK_DEBUG_MORE(fmt,...) printk(fmt, ##__VA_ARGS__)
#else
#define NNPBACK_DEBUG(fmt,...)
#endif
#define NNPBACK_ERR(fmt,...) printk("Nnpback:Error " fmt, ##__VA_ARGS__)
#define NNPBACK_LOG(fmt,...) printk("Nnpback:Info " fmt, ##__VA_ARGS__)

struct nnpif {
   domid_t domid;

   /* Shared page */
   void *page;
};
typedef struct nnpif nnpif_t;

struct nnpback_dev {

   struct gntmap map;

   xenbus_event_queue events;
};
typedef struct nnpback_dev nnpback_dev_t;

enum { EV_NONE, EV_NEWFE } tpm_ev_enum;

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
      return EV_NEWFE;
   }
   return EV_NONE;
}

static inline size_t divide_round_up(size_t dividend, size_t divisor) {
   if (dividend % divisor == 0) {
      return dividend / divisor;
   } else {
      return dividend / divisor + 1;
   }
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
}

void handle_backend_event(char* evstr) {
   domid_t domid;
   char model[16], frontend_path[32];
   int event;
   char *err;
   char grant_ref_value[1000];
   char grant_ref_entry[64];
   snprintf(grant_ref_value, 1000, "%s", "");

   NNPBACK_DEBUG("Xenbus Event: %s\n", evstr);

   event = parse_eventstr(evstr, &domid, model);
   
   if (event == EV_NEWFE) {
      snprintf(frontend_path, 32, "/local/domain/backend/%d", domid);
      if((err = xenbus_write(XBT_NIL, frontend_path, "0"))) {
         NNPBACK_ERR("Unable to write frontend domain id, error was %s\n", err);
         free(err);
      }

      int grant_entry_sum = 0, outer;
      size_t inner;
      float *page;
      grant_ref_t grant_ref;
      if (strcmp("squeezenet1_0", model) == 0) {
         int total_item = sizeof(P4C8732DB) / sizeof(struct backend_param), total_bytes = 0;
         int i, j;
         for (i = 0; i < total_item; ++i) {
            total_bytes += P4C8732DB[i].param_size;

         float* page = (float*)alloc_pages(round_up_power_of_two(total_bytes));

         for (i = 0; i < divide_round_up(total_bytes, PAGE_SIZE); ++i) {
            grant_ref = gnttab_grant_access(domid, virt_to_mfn((uintptr_t)(void*)page + i * PAGE_SIZE), 0);
         }

         for (i = 0; i < total_item; ++i) {
            for (j = 0; j < P4C8732DB[i].param_size; ++j)
               *(page++) = (P4C8732DB[i].param_ptr + j);

      //    for (outer = 0; outer < sizeof(P2D24C20E) / sizeof(struct param); ++outer) {
      //       for (inner = 0; inner < divide_round_up(P2D24C20E[outer].param_size, 1024); ++inner) {
      //          /* Create shared page */
      //          page = (float *)alloc_page();
      //          if(page == NULL) {
      //             NNPBACK_ERR("Unable to allocate page for shared memory\n");
      //          }
      //          grant_ref = gnttab_grant_access(domid, virt_to_mfn(page), 0);
      //          NNPBACK_DEBUG("grant ref is %lu\n", (unsigned long) grant_ref);

      //          snprintf(grant_ref_value + strlen(grant_ref_value), 1000 - strlen(grant_ref_value), "%lu ", (unsigned long) grant_ref);
      //          if (strlen(grant_ref_value) > 950) { // XENSTORE_RING_SIZE is 1024
      //             snprintf(grant_ref_entry, 64, "%s/grant-ref%d", frontend_path, grant_entry_sum);
      //             if((err = xenbus_write(XBT_NIL, grant_ref_entry, grant_ref_value))) {
      //                NNPBACK_ERR("Unable to write ring-ref, error was %s\n", err);
      //                free(err);
      //             }
      //             grant_entry_sum++;
      //             snprintf(grant_ref_value, 1000, "%s", "");
      //          }
      //       }
      //    }
      // }
      // char state_path[64];
      // snprintf(state_path, 64, "%s/state", frontend_path);
      // char value[8];
      // snprintf(value, 8, "%d", 1);
      // if((err = xenbus_write(XBT_NIL, state_path, value))) {
      //    NNPBACK_ERR("Unable to write state path, error was %s\n", err);
      }
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
   printk("============= Init NNP BACK ================\n");
   char* err;
   char value[16];

   snprintf(value, 16, "%d", xenbus_get_self_id());
   if ((err = xenbus_write(XBT_NIL, "/local/domain/backend", value)))
   {
      NNPBACK_ERR("Unable to write backend id: %s\n", err);
      free(err);
   }

   eventthread = create_thread("nnpback-listener", event_thread, NULL);

}
