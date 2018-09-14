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

};
typedef struct nnpback_dev nnpback_dev_t;

/* Global objects */
static struct thread* eventthread = NULL;
static nnpback_dev_t gnnpdev;

void init_nnpback()
{
   printk("============= Init NNP BACK ================\n");

   eventthread = create_thread("nnpback-listener", event_thread, NULL);

}
