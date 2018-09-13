#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/xmalloc.h>
#include <mini-os/events.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <mini-os/nnpfront.h>
#include <mini-os/lib.h>
#include <fcntl.h>

#define NNPFRONT_PRINT_DEBUG
#ifdef NNPFRONT_PRINT_DEBUG
#define NNPFRONT_DEBUG(fmt,...) printk("Nnpfront:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#define NNPFRONT_DEBUG_MORE(fmt,...) printk(fmt, ##__VA_ARGS__)
#else
#define NNPFRONT_DEBUG(fmt,...)
#endif
#define NNPFRONT_ERR(fmt,...) printk("Nnpfront:Error " fmt, ##__VA_ARGS__)
#define NNPFRONT_LOG(fmt,...) printk("Nnpfront:Info " fmt, ##__VA_ARGS__)


struct nnpfront_dev* init_nnpfront(const char* _nodename)
{
   struct nnpfront_dev* dev;
   const char* nodename;
   char path[512];
   char* value, *err;
   unsigned long long ival;

   printk("============= Init NNP Front ================\n");

   dev = malloc(sizeof(struct nnpfront_dev));
   memset(dev, 0, sizeof(struct nnpfront_dev));

   nodename = _nodename ? _nodename : "device/vnnp/0";
   dev->nodename = strdup(nodename);

   /* Get backend domid */
   snprintf(path, 512, "%s/backend-id", dev->nodename);
   if((err = xenbus_read(XBT_NIL, path, &value))) {
      NNPFRONT_ERR("Unable to read %s during nnpfront initialization! error = %s\n", path, err);
      free(err);
      goto error;
   }
   if(sscanf(value, "%llu", &ival) != 1) {
      NNPFRONT_ERR("%s has non-integer value (%s)\n", path, value);
      free(value);
      goto error;
   }
   free(value);
   dev->bedomid = ival;

   /* Get backend xenstore path */
   snprintf(path, 512, "%s/backend", dev->nodename);
   if((err = xenbus_read(XBT_NIL, path, &dev->bepath))) {
      NNPFRONT_ERR("Unable to read %s during nnpfront initialization! error = %s\n", path, err);
      free(err);
      goto error;
   }

   NNPFRONT_LOG("Initialization Completed successfully\n");

   return dev;

error:
   return NULL;
}