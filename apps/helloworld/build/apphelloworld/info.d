cmd_/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o := gcc -I/home/cheng/unikraft/apps/helloworld/build/include -nostdinc -nostdlib -I/home/cheng/unikraft/unikraft/include -I/home/cheng/unikraft/libs/newlib/include -I/home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include -I/home/cheng/unikraft/unikraft/lib/ukboot/include -I/home/cheng/unikraft/unikraft/lib/ukdebug/include -I/home/cheng/unikraft/unikraft/lib/ukargparse/include -I/home/cheng/unikraft/unikraft/lib/uktimeconv/include -I/home/cheng/unikraft/unikraft/lib/ukalloc/include -I/home/cheng/unikraft/unikraft/lib/ukallocbbuddy/include -I/home/cheng/unikraft/unikraft/lib/vfscore/include -I/home/cheng/unikraft/unikraft/lib/ukbus/include -I/home/cheng/unikraft/apps/helloworld/include -I/home/cheng/unikraft/apps/helloworld/deps/fxdiv/include -I/home/cheng/unikraft/apps/helloworld/deps/pthreadpool/include -I/home/cheng/unikraft/apps/helloworld/deps/cpuinfo/include -I/home/cheng/unikraft/apps/helloworld/deps/cpuinfo/deps/clog/include -I/home/cheng/unikraft/apps/helloworld/deps/cpuinfo/src -I/home/cheng/unikraft/apps/helloworld/deps/cpuinfo/src/x86 -DCC_VERSION=5.4 -D__X86_64__ -m64 -mno-red-zone -fno-reorder-blocks -fno-asynchronous-unwind-tables -U __linux__ -U __FreeBSD__ -U __sun__ -no-pie -fno-stack-protector -fno-omit-frame-pointer -fno-tree-sra -Wall -Wextra -g0 -DUK_VERSION=0.2 -DUK_FULLVERSION=0.2~cdb5bbc -DUK_CODENAME="Titan" -mtune=generic -O2 -D__linux__ -D_POSIX_THREADS -D_POSIX_MONOTONIC_CLOCK -DNNP_INFERENCE_ONLY=1 -D__USE_GNU -D__LIBNAME__=apphelloworld -D__BASENAME__=info.c -c /home/cheng/unikraft/apps/helloworld/deps/cpuinfo/src/x86/info.c -o /home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o -Wp,-MD,/home/cheng/unikraft/apps/helloworld/build/apphelloworld/.info.o.d

source_/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o := /home/cheng/unikraft/apps/helloworld/deps/cpuinfo/src/x86/info.c

deps_/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o := \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/stdint.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/machine/_default_types.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/features.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/_newlib_version.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_intsup.h \
  /home/cheng/unikraft/apps/helloworld/build/libnewlibc/origin/newlib-2.5.0.20170922/newlib/libc/include/sys/_stdint.h \
  /home/cheng/unikraft/apps/helloworld/deps/cpuinfo/include/cpuinfo.h \
  /home/cheng/unikraft/libs/newlib/include/stdbool.h \
  /home/cheng/unikraft/apps/helloworld/deps/cpuinfo/src/x86/api.h \

/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o: $(deps_/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o)

$(deps_/home/cheng/unikraft/apps/helloworld/build/apphelloworld/info.o):
