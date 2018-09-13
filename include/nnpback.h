#include <xen/io/xenbus.h>
#include <mini-os/types.h>
#include <xen/xen.h>
#include <mini-os/nnpif.h>
#ifndef NNPBACK_H
#define NNPBACK_H

void init_nnpback(void);

void shutdown_nnpback(void);
#endif
