#!/bin/bash

image=build/main.img

if [ -ge $# 1 ] ; then
    image="$1"
fi

qemu-system-i386 -serial stdio -fda $image
