#ifndef NNPFRONT_H
#define NNPFRONT_H

#include <mini-os/types.h>
#include <xen/xen.h>

struct nnpfront_dev {
   grant_ref_t ring_ref;

   tpmif_shared_page_t *page;

   domid_t bedomid;
};

#endif