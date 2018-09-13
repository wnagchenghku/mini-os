#include <stdint.h>

#ifndef NNPIF_H__
#define NNPIF_H__

struct nnpif_shared_page {
    uint32_t length;         /* request/response length in bytes */

    uint8_t state;           /* enum tpmif_state */
    uint8_t locality;        /* for the current request */
    uint8_t pad;             /* should be zero */

    uint8_t nr_extra_pages;  /* extra pages for long packets; may be zero */
    uint32_t extra_pages[0]; /* grant IDs; length is actually nr_extra_pages */
};
typedef struct nnpif_shared_page nnpif_shared_page_t;

#endif