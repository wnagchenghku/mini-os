cmd_/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o := gcc -I/home/cheng/unikraft/apps/helloworld/build/include -nostdinc -nostdlib -I/home/cheng/unikraft/unikraft/include -I/home/cheng/unikraft/libs/newlib/include -I/home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include -I/home/cheng/unikraft/unikraft/lib/ukboot/include -I/home/cheng/unikraft/unikraft/lib/ukdebug/include -I/home/cheng/unikraft/unikraft/lib/ukargparse/include -I/home/cheng/unikraft/unikraft/lib/uktimeconv/include -I/home/cheng/unikraft/unikraft/lib/ukalloc/include -I/home/cheng/unikraft/unikraft/lib/ukallocbbuddy/include -I/home/cheng/unikraft/unikraft/lib/vfscore/include -I/home/cheng/unikraft/unikraft/lib/ukbus/include -DCC_VERSION=5.4 -D__X86_64__ -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -U __linux__ -U __FreeBSD__ -U __sun__ -no-pie -fno-stack-protector -fno-omit-frame-pointer -fno-tree-sra -Wall -Wextra -g0 -DUK_VERSION=0.2 -DUK_FULLVERSION=0.2~cdb5bbc -DUK_CODENAME="Titan" -mtune=generic -O2 -D__LIBNAME__=libukargparse -D__BASENAME__=argparse.c -c /home/cheng/unikraft/unikraft/lib/ukargparse/argparse.c -o /home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o -Wp,-MD,/home/cheng/unikraft/apps/helloworld/build/libukargparse/.argparse.o.d

source_/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o := /home/cheng/unikraft/unikraft/lib/ukargparse/argparse.c

deps_/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o := \
  /home/cheng/unikraft/unikraft/lib/ukargparse/include/uk/argparse.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/types.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/intsizes.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/types.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/limits.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/limits.h \
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
  /home/cheng/unikraft/unikraft/include/uk/config.h \

/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o: $(deps_/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o)

$(deps_/home/cheng/unikraft/apps/helloworld/build/libukargparse/argparse.o):
