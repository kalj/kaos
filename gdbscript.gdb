set disassembly-flavor intel

target remote | qemu-system-i386 -S -gdb stdio -m 32 -fda build/main.img

layout asm
layout reg

set architecture i386
break *0x7c00