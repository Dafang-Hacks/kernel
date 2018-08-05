#!/usr/bin/env bash
CROSS_COMPILE="/home/eko/Dokumente/projects/Xiaomi-Dafang-Software/mips-gcc472-glibc216-64bit/bin/mips-linux-gnu-"
export CROSS_COMPILE=${CROSS_COMPILE}
export CC=${CROSS_COMPILE}gcc
export LD=${CROSS_COMPILE}ld
export CFLAGS="-muclibc -O2 -DDEBUG_TRACE -DFAKE_ROOT "
export CPPFLAGS="-muclibc -O2"
export LDFLAGS="-muclibc -O2"
make clean
make