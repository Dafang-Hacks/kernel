#!/usr/bin/env bash
HOST=192.168.0.46
cp arch/mips/boot/uImage.lzma kernel.bin
ftp-upload -h ${HOST} -u root --password ismart12 -d /system/sdcard/ kernel.bin