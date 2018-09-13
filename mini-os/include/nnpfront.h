#ifndef NNPFRONT_H
#define NNPFRONT_H

#include <mini-os/types.h>
#include <xen/xen.h>

struct nnpfront_dev {
   grant_ref_t ring_ref;

   tpmif_shared_page_t *page;

   domid_t bedomid;
};


/*Initialize frontend */
struct nnpfront_dev* init_nnpfront(const char* nodename);
/*Shutdown frontend */
void shutdown_nnpfront(struct nnpfront_dev* dev);

#endif