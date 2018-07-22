cmd_/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o := gcc -I/home/cheng/unikraft/apps/helloworld/build/include -nostdinc -nostdlib -I/home/cheng/unikraft/unikraft/include -I/home/cheng/unikraft/libs/newlib/include -I/home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include -I/home/cheng/unikraft/unikraft/lib/ukboot/include -I/home/cheng/unikraft/unikraft/lib/ukdebug/include -I/home/cheng/unikraft/unikraft/lib/ukargparse/include -I/home/cheng/unikraft/unikraft/lib/uktimeconv/include -I/home/cheng/unikraft/unikraft/lib/ukalloc/include -I/home/cheng/unikraft/unikraft/lib/ukallocbbuddy/include -I/home/cheng/unikraft/unikraft/lib/vfscore/include -I/home/cheng/unikraft/unikraft/lib/ukbus/include -DCC_VERSION=5.4 -D__X86_64__ -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -U __linux__ -U __FreeBSD__ -U __sun__ -no-pie -fno-stack-protector -fno-omit-frame-pointer -fno-tree-sra -Wall -Wextra -g0 -DUK_VERSION=0.2 -DUK_FULLVERSION=0.2~cdb5bbc -DUK_CODENAME="Titan" -mtune=generic -O2 -D__LIBNAME__=libnewlibglue -D__BASENAME__=pty.c -c /home/cheng/unikraft/libs/newlib/pty.c -o /home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o -Wp,-MD,/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/.pty.o.d

source_/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o := /home/cheng/unikraft/libs/newlib/pty.c

deps_/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o := \
  /home/cheng/unikraft/libs/newlib/include/pty.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/termios.h \
  /home/cheng/unikraft/libs/newlib/include/sys/termios.h \
  /home/cheng/unikraft/libs/newlib/include/sys/ioctl.h \
  /home/cheng/unikraft/unikraft/include/uk/essentials.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/types.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/intsizes.h \
  /home/cheng/unikraft/unikraft/include/uk/arch/x86_64/types.h \

/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o: $(deps_/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o)

$(deps_/home/cheng/unikraft/apps/helloworld/build/libnewlibglue/pty.o):
