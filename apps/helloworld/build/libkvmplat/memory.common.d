cmd_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o := gcc -I/home/cheng/unikraft/apps/helloworld/build/include -nostdinc -nostdlib -I/home/cheng/unikraft/unikraft/include -I/home/cheng/unikraft/libs/newlib/include -I/home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include -I/home/cheng/unikraft/unikraft/lib/ukboot/include -I/home/cheng/unikraft/unikraft/lib/ukdebug/include -I/home/cheng/unikraft/unikraft/lib/ukargparse/include -I/home/cheng/unikraft/unikraft/lib/uktimeconv/include -I/home/cheng/unikraft/unikraft/lib/ukalloc/include -I/home/cheng/unikraft/unikraft/lib/ukallocbbuddy/include -I/home/cheng/unikraft/unikraft/lib/vfscore/include -I/home/cheng/unikraft/unikraft/lib/ukbus/include -I/home/cheng/unikraft/unikraft/plat/kvm/include -I/home/cheng/unikraft/unikraft/plat/common/include -DCC_VERSION=5.4 -D__X86_64__ -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -U __linux__ -U __FreeBSD__ -U __sun__ -no-pie -fno-stack-protector -fno-omit-frame-pointer -fno-tree-sra -Wall -Wextra -g0 -DUK_VERSION=0.2 -DUK_FULLVERSION=0.2~cdb5bbc -DUK_CODENAME="Titan" -mtune=generic -O2 -D__LIBNAME__=libkvmplat -D__BASENAME__=memory.c -c /home/cheng/unikraft/unikraft/plat/common/memory.c -o /home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o -Wp,-MD,/home/cheng/unikraft/apps/helloworld/build/libkvmplat/.memory.common.o.d

source_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o := /home/cheng/unikraft/unikraft/plat/common/memory.c

deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o := \
  /home/cheng/unikraft/unikraft/include/uk/plat/memory.h \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/ukplat/memrname.h) \
  /home/cheng/unikraft/unikraft/include/uk/arch/types.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/intsizes.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/types.h \
  /home/cheng/unikraft/unikraft/lib/ukalloc/include/uk/alloc.h \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukalloc/ifpages.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukalloc/ifstats.h) \
  /home/cheng/unikraft/libs/newlib/include/stddef.h \
  /home/cheng/unikraft/libs/newlib/include/__stddef_max_align_t.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/stdint.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/_default_types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/features.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/_newlib_version.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_intsup.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_stdint.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/_ansi.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/newlib.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/config.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/ieeefp.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/cdefs.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/_types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/lock.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/endian.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/_endian.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/select.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_sigset.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_timeval.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/timespec.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_timespec.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_pthreadtypes.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/sched.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/errno.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/errno.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/reent.h \
  /home/cheng/unikraft/unikraft/include/uk/config.h \
  /home/cheng/unikraft/unikraft/lib/ukdebug/include/uk/assert.h \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/enable/assert.h) \
  /home/cheng/unikraft/unikraft/include/uk/plat/bootstrap.h \
  /home/cheng/unikraft/unikraft/include/uk/essentials.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/lcpu.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/lcpu.h \
  /home/cheng/unikraft/unikraft/lib/ukdebug/include/uk/print.h \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printk.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd/crit.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd/err.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd/warn.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd/info.h) \
    $(wildcard /home/cheng/unikraft/apps/helloworld/build/kconfig/depinclude/libukdebug/printd/extra.h) \
  /home/cheng/unikraft/libs/newlib/include/stdarg.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/memory.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/string.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/_ansi.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/xlocale.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/strings.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/string.h \

/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o: $(deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o)

$(deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/memory.common.o):
