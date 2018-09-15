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

#include <mini-os/2D24C20E.h>

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

void handle_backend_event(char* evstr) {
   domid_t domid;
   char model[16], path[16];
   int event;
   char *err;

   NNPBACK_DEBUG("Xenbus Event: %s\n", evstr);

   event = parse_eventstr(evstr, &domid, model);
   
   if (event == EV_NEWFE) {
      int i;
      float *page;
      grant_ref_t ring_ref;
      if (strcmp("squeezenet1_0", model) == 0) {
         for (i = 0; i < sizeof(P2D24C20E) / sizeof(float *); ++i) {
            /* Create shared page */
            page = (float *)alloc_page(divide_round_up(P2D24C20E[i] / sizeof(float)), 1000);
            if(page == NULL) {
               NNPBACK_ERR("Unable to allocate page for shared memory\n");
            }
            ring_ref = gnttab_grant_access(domid, virt_to_mfn(page), 0);
            NNPBACK_DEBUG("grant ref is %lu\n", (unsigned long) ring_ref);

            snprintf(path, 16, "ring-ref%d", i);
            if((err = xenbus_printf(XBT_NIL, evstr, path, "%u", (unsigned int) ring_ref))) {
               NNPBACK_ERR("Unable to write %s/ring-ref, error was %s\n", err);
               free(err);
            }
         }
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
   if ((err = xenbus_write(XBT_NIL, "/local/domain/backend-id", value)))
   {
      NNPBACK_ERR("Unable to write backend id: %s\n", err);
      free(err);
   }

   eventthread = create_thread("nnpback-listener", event_thread, NULL);

}
