#bin/sh
#4.2.1

cp ~/xen/xen-4.2.1-i386/tools/libxc/Makefile ~/xen-4.2.1/tools/libxc
cp ~/xen/xen-4.2.1-i386/tools/libxc/xc_dom_core.c ~/xen-4.2.1/tools/libxc
cp ~/xen/xen-4.2.1-i386/tools/libxc/dev_xc_dom_elfdyn.c ~/xen-4.2.1/tools/libxc
cp ~/xen/xen-4.2.1-i386/tools/libxc/dev_xc_dom_elfdyn.h ~/xen-4.2.1/tools/libxc

cp ~/xen/xen-4.2.1-i386/tools/libxl/xl_cmdimpl.c ~/xen-4.2.1/tools/libxl

# stack:      00029440-08029440
cp ~/xen/xen-4.2.1-i386/extras/mini-os/include/x86/arch_limits.h ~/xen-4.2.1/extras/mini-os/include/x86

cp ~/xen/xen-4.2.1-i386/extras/mini-os/Makefile ~/xen-4.2.1/extras/mini-os
cp ~/xen/xen-4.2.1-i386/extras/mini-os/minios.mk ~/xen-4.2.1/extras/mini-os
cp ~/xen/xen-4.2.1-i386/extras/mini-os/test.c ~/xen-4.2.1/extras/mini-os


# build shared library
# gcc -nostdlib -Wl,-hash-style=sysv -shared -fPIC -o libnnpack.so addvec.c
# so similarly
# gcc -I /home/cheng/NNPACK/include/ -I /home/cheng/NNPACK/deps/pthreadpool/include/ -nostdlib -Wl,-hash-style=sysv -shared -fPIC -o conv_1.so conv_1.c

# remember to change mini-os/Makefile -rpath=/home/cheng/xen-4.2.1/extras/mini-os/ 

# To change to x86_64:
# dl-machine.h: The x86-64 never uses Elf64_Rel relocations.
# dynamic-link.h -> do-rel.h -> dl-machine.h


# verified in i386 that chain .so works

# to change minios stack size
# extras/mini-os/include/x86/arch_limits.h 
# #define __STACK_SIZE_PAGE_ORDER  14

# to enable sse and avx
# 1. fpu_init and sse_init in minios
# 2. cpu_detection in libhermit


# if we change the stack to 14 in minios x86-64
# it reports "cannot handle page request order 14"
# then just increase domain memory to e.g., 2048
