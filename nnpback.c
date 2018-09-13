#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/events.h>
#include <errno.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <xen/io/protocols.h>
#include <mini-os/xmalloc.h>
#include <time.h>
#include <mini-os/nnpback.h>
#include <mini-os/lib.h>
#include <mini-os/sched.h>
#include <fcntl.h>
#include <mini-os/mm.h>
#include <mini-os/posix/sys/mman.h>

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
   unsigned int handle;

   char* fe_path;
   char* fe_state_path;

   /* Shared page */
   nnpif_shared_page_t *page;

   enum xenbus_state state;
   enum { DISCONNECTED, DISCONNECTING, CONNECTED } status;

   /* state flags */
   int flags;
};
typedef struct nnpif nnpif_t;

struct nnpback_dev {

   struct gntmap map;

   /* True if at least one nnpif has a request to be handled */
   int flags;

   xenbus_event_queue events;

};
typedef struct nnpback_dev nnpback_dev_t;

enum { EV_NONE, EV_NEWFE, EV_STCHNG } nnp_ev_enum;

/* Global objects */
static struct thread* eventthread = NULL;
static nnpback_dev_t gnnpdev;

void handle_backend_event(char* evstr) {
   domid_t domid;
   unsigned int handle;
   int event;

   NNPBACK_DEBUG("Xenbus Event: %s\n", evstr);
}

static void event_listener(void)
{
   const char* bepath = "backend/vnnp";
   char **path;
   char* err;

   /* Setup the backend device watch */
   if((err = xenbus_watch_path_token(XBT_NIL, bepath, bepath, &gnnpdev.events)) != NULL) {
      NNPBACK_ERR("xenbus_watch_path_token(%s) failed with error %s!\n", bepath, err);
      free(err);
      goto egress;
   }

   /* Wait and listen for changes in frontend connections */
   while(1) {
      path = xenbus_wait_for_watch_return(&gnnpdev.events);

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

   eventthread = create_thread("nnpback-listener", event_thread, NULL);

}
