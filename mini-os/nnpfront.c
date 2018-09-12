#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/xmalloc.h>
#include <mini-os/events.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <mini-os/tpmfront.h>
#include <mini-os/lib.h>

//#define NNPFRONT_PRINT_DEBUG
#ifdef NNPFRONT_PRINT_DEBUG
#define NNPFRONT_DEBUG(fmt,...) printk("Nnpfront:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#else
#define NNPFRONT_DEBUG(fmt,...)
#endif
#define NNPFRONT_ERR(fmt,...) printk("Nnpfront:Error " fmt, ##__VA_ARGS__)
#define NNPFRONT_LOG(fmt,...) printk("Nnpfront:Info " fmt, ##__VA_ARGS__)

static int publish_xenbus(struct nnpfront_dev* dev) {
   xenbus_transaction_t xbt;
   int retry;
   char* err;
   /* Write the grant reference to xenstore */
again:
   if((err = xenbus_transaction_start(&xbt))) {
      TPMFRONT_ERR("Unable to start xenbus transaction, error was %s\n", err);
      free(err);
      return -1;
   }

   if((err = xenbus_printf(xbt, dev->nodename, "ring-ref", "%u", (unsigned int) dev->ring_ref))) {
      TPMFRONT_ERR("Unable to write %s/ring-ref, error was %s\n", dev->nodename, err);
      free(err);
      goto abort_transaction;
   }

   if((err = xenbus_transaction_end(xbt, 0, &retry))) {
      TPMFRONT_ERR("Unable to complete xenbus transaction, error was %s\n", err);
      free(err);
      return -1;
   }
   if(retry) {
      goto again;
   }

   return 0;
abort_transaction:
   if((err = xenbus_transaction_end(xbt, 1, &retry))) {
      free(err);
   }
   return -1;
}

static int nnpfront_connect(struct nnpfront_dev* dev)
{
   char* err;
   /* Create shared page */
   dev->page = (tpmif_shared_page_t *)alloc_page();
   if(dev->page == NULL) {
      NNPFRONT_ERR("Unable to allocate page for shared memory\n");
      goto error;
   }
   memset(dev->page, 0, PAGE_SIZE);
   dev->ring_ref = gnttab_grant_access(dev->bedomid, virt_to_mfn(dev->page), 0);
   NNPFRONT_DEBUG("grant ref is %lu\n", (unsigned long) dev->ring_ref);

   return 0;
error:
   return -1;
}

struct nnpfront_dev* init_nnpfront(void)
{
   struct nnpfront_dev* dev;

   dev = malloc(sizeof(struct nnpfront_dev));
   memset(dev, 0, sizeof(struct nnpfront_dev));

   /* Create and publish grant reference and event channel */
   if (nnpfront_connect(dev)) {
      goto error;
   }

   NNPFRONT_LOG("Initialization Completed successfully\n");

   return dev;

error:
   shutdown_nnpfront(dev);
   return NULL;
}

void shutdown_nnpfront(struct nnpfront_dev* dev)
{
   char* err;
   char path[512];
   if(dev == NULL) {
      return;
   }
   NNPFRONT_LOG("Shutting down tpmfront\n");

   /* unmap shared page */
   gnttab_end_access(dev->ring_ref);

   free_page(dev->page);

   free(dev);
}

