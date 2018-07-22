cmd_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o := gcc -I/home/cheng/unikraft/apps/helloworld/build/include -nostdinc -nostdlib -I/home/cheng/unikraft/unikraft/include -I/home/cheng/unikraft/unikraft/plat/kvm/include -I/home/cheng/unikraft/unikraft/plat/common/include -DCC_VERSION=5.4 -D__X86_64__ -m64 -U __linux__ -U __FreeBSD__ -U __sun__ -D__ASSEMBLY__ -no-pie -DUK_VERSION=0.2 -DUK_FULLVERSION=0.2~cdb5bbc -DUK_CODENAME="Titan" -D__LIBNAME__=libkvmplat -D__BASENAME__=entry64.S -c /home/cheng/unikraft/unikraft/plat/kvm/x86/entry64.S -o /home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o -Wp,-MD,/home/cheng/unikraft/apps/helloworld/build/libkvmplat/.entry64.o.d

source_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o := /home/cheng/unikraft/unikraft/plat/kvm/x86/entry64.S

deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o := \
  /home/cheng/unikraft/unikraft/plat/common/include/x86/cpu_defs.h \
  /home/cheng/unikraft/unikraft/plat/kvm/include/kvm-x86/traps.h \
  /home/cheng/unikraft/unikraft/plat/common/include/x86/traps.h \
  /home/cheng/unikraft/unikraft/plat/common/include/x86/regs.h \
  /home/cheng/unikraft/unikraft/plat/kvm/include/kvm-x86/multiboot_defs.h \
  /home/cheng/unikraft/unikraft/plat/kvm/x86/pagetable.S \

/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o: $(deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o)

$(deps_/home/cheng/unikraft/apps/helloworld/build/libkvmplat/entry64.o):
