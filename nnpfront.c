#include <mini-os/os.h>
#include <mini-os/xenbus.h>
#include <mini-os/xmalloc.h>
#include <mini-os/events.h>
#include <mini-os/gnttab.h>
#include <xen/io/xenbus.h>
#include <mini-os/lib.h>
#include <fcntl.h>
#include <mini-os/posix/sys/mman.h>
#include <mini-os/P4C8732DB-frontend.h>

#define NNPFRONT_PRINT_DEBUG
#ifdef NNPFRONT_PRINT_DEBUG
#define NNPFRONT_DEBUG(fmt,...) printk("Nnpfront:Debug("__FILE__":%d) " fmt, __LINE__, ##__VA_ARGS__)
#define NNPFRONT_DEBUG_MORE(fmt,...) printk(fmt, ##__VA_ARGS__)
#else
#define NNPFRONT_DEBUG(fmt,...)
#endif
#define NNPFRONT_ERR(fmt,...) printk("Nnpfront:Error " fmt, ##__VA_ARGS__)
#define NNPFRONT_LOG(fmt,...) printk("Nnpfront:Info " fmt, ##__VA_ARGS__)

struct nnpfront_dev {
   struct gntmap map;
};
typedef struct nnpfront_dev nnpfront_dev_t;

static nnpfront_dev_t gtpmdev;

static inline size_t divide_round_up(size_t dividend, size_t divisor) {
   if (dividend % divisor == 0) {
      return dividend / divisor;
   } else {
      return dividend / divisor + 1;
   }
}

void init_nnpfront(void)
{
   char path[512];
   char* value, *err;
   unsigned long long ival;

   printk("============= Init NNP Front ================\n");

   /* Get backend domid */
   if((err = xenbus_read(XBT_NIL, "/local/domain/backend", &value))) {
      NNPFRONT_ERR("Unable to read %s during nnpfront initialization! error = %s\n", path, err);
      free(err);
   }
   if(sscanf(value, "%llu", &ival) != 1) {
      NNPFRONT_ERR("%s has non-integer value (%s)\n", path, value);
      free(value);
   }
   free(value);
   domid_t bedomid = ival;

   snprintf(path, 512, "%u", xenbus_get_self_id());
   char *model = "squeezenet1_0";

   if((err = xenbus_printf(XBT_NIL, "/local/domain/frontend", path, "%s", model))) {
      NNPFRONT_ERR("Unable to write to xenstore frontend id\n");
      free(err);
   }

   xenbus_event_queue events = NULL;

   snprintf(path, 512, "/local/domain/backend/%d/state", xenbus_get_self_id());
   /*Setup the watch to wait for the backend */
   if((err = xenbus_watch_path_token(XBT_NIL, path, path, &events))) {
      NNPFRONT_ERR("Could not set a watch on %s, error was %s\n", path, err);
      free(err);
   }

   NNPFRONT_LOG("Waiting for backend to publish references..\n");
   while(1) {
      int state = xenbus_read_integer(path);
      if(state == 1)
         break;
       xenbus_wait_for_watch(&events);
   }

   // int grant_entry_sum = 0;
   // int grant_ref_sum = 0;
   // char grant_ref_value[1000];
   // for (outer = 0; outer < sizeof(P2D24C20E) / sizeof(struct param); ++outer) {
   //    for (inner = 0; inner < divide_round_up(P2D24C20E[outer].param_size, 1024); ++inner) {
   //       snprintf(grant_ref_value + strlen(grant_ref_value), 1024 - strlen(grant_ref_value), "%lu ", (unsigned long) grant_ref);
   //       grant_ref_sum++;
   //       if (strlen(grant_ref_value) > 950) {
   //          grant_entry_sum++;
   //          snprintf(grant_ref_value, 1000, "%s", "");
   //       }
   //    }
   // }

   // grant_ref_t *grant_ref_arr = (grant_ref_t*)malloc(sizeof(int) * grant_ref_sum);
   // int grant_ref_it = 0, i;
   // for (i = 0; i < grant_entry_sum; ++i) {
   //    char *value;
   //    snprintf(grant_ref_entry, 64, "/local/domain/backend/%d/grant-ref%d", xenbus_get_self_id(), i);
   //    if((err = xenbus_read(XBT_NIL, grant_ref_entry, &value))) {
   //       NNPFRONT_ERR("Unable to read %s during tpmfront initialization! error = %s\n", grant_ref_entry, err);
   //       free(err);
   //    }
   //    int c, bytesread;
   //    while(sscanf(value, "%d%n", &c, &bytesread) > 0) {
   //      *(grant_ref_arr + grant_ref_it) = c;
   //      grant_ref_it++;
   //      value += bytesread;
   //   }
   //   free(value);
   // }
   
   // grant_ref_it = 0;

   // for (i = 0; i < sizeof(P2D24C20E) / sizeof(struct param); ++i) {
   //    size_t page_num = divide_round_up(P2D24C20E[i].param_size, 1024);
   //    grant_ref_t *grant_ref = malloc(sizeof(grant_ref_t) * page_num);
   //    int j;
   //    for (j = 0; j < page_num; ++j) {
   //       *(grant_ref + j) = *(grant_ref_arr + grant_ref_it);
   //       grant_ref_it++;
   //    }
   //    if((P2D24C20E[i].param_ptr = gntmap_map_grant_refs(&gtpmdev.map, page_num, &bedomid, 0, grant_ref, PROT_READ)) == NULL) {
   //       NNPBACK_ERR("Failed to map grant reference %u\n", (unsigned int) bedomid);
   //    }
   //    free(grant_ref);
   // }
   // free(grant_ref_arr);

   int total_item = sizeof(P4C8732DB) / sizeof(struct frontend_param), total_bytes = 0;
   int i, j;
   for (i = 0; i < total_item; ++i)
      total_bytes += P4C8732DB[i].param_size;

   int total_page = divide_round_up(total_bytes, PAGE_SIZE);
   grant_ref_t *ringref = (grant_ref_t*)malloc(sizeof(grant_ref_t) * total_page);
   for (i = 0; i < total_page; ++i) {
      //ringref[i] = ;
   }
   
   if(gntmap_map_grant_refs_batch(&gtpmdev.map, total_page, &bedomid, 0, ringref, PROT_READ) == NULL) {
      NNPBACK_ERR("Failed to map grant reference %u\n", (unsigned int) bedomid);
   }
   free(ringref);

   NNPFRONT_LOG("Initialization Completed successfully\n");
}
