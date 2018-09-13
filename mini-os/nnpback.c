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
#include <fcntl.h>
#include <mini-os/mm.h>
#include <mini-os/posix/sys/mman.h>


//#define NNPBACK_PRINT_DEBUG
#ifdef NNPBACK_PRINT_DEBUG
#define NNPBACK_DEBUG(fmt,...) printk("Nnpback:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#else
#define NNPBACK_DEBUG(fmt,...)
#endif
#define NNPBACK_ERR(fmt,...) printk("Nnpback:Error " fmt, ##__VA_ARGS__)
#define NNPBACK_LOG(fmt,...) printk("Nnpback:Info " fmt, ##__VA_ARGS__)

struct nnpif {
   domid_t domid;

   char* fe_path;

   /* Shared page */
   tmpif_shared_page_t *page;
};
typedef struct nnpif nnpif_t;

struct tpmback_dev {
   struct gntmap map;
};
typedef struct tpmback_dev tpmback_dev_t;

static tpmback_dev_t gtpmdev;

/* Connect to frontend */
int connect_fe(nnpif_t* nnpif)
{
   // char path[512];
   // char* err, *value;
   uint32_t domid;
   grant_ref_t ringref;

   /* Fetch the grant reference */
   // snprintf(path, 512, "%s/ring-ref", nnpif->fe_path);
   // if((err = xenbus_read(XBT_NIL, path, &value))) {
   //    NNPBACK_ERR("Error creating new tpm instance xenbus_read(%s) Error = %s", path, err);
   //    free(err);
   //    return -1;
   // }
   // if(sscanf(value, "%d", &ringref) != 1) {
   //    NNPBACK_ERR("Non integer value (%s) in %s ??\n", value, path);
   //    free(value);
   //    return -1;
   // }
   // free(value);

   domid = nnpif->domid;
   if((nnpif->page = gntmap_map_grant_refs(&gtpmdev.map, 1, &domid, 0, &ringref, PROT_READ | PROT_WRITE)) == NULL) {
      NNPBACK_ERR("Failed to map grant reference %u\n", (unsigned int) nnpif->domid);
      return -1;
   }

   NNPBACK_LOG("Frontend %u connected\n", (unsigned int) nnpif->domid);

   return 0;
}

void init_nnpback(void)
{
   nnpif_t nnpif;
   connect_fe(&nnpif);
}