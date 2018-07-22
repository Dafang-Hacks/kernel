#!/usr/bin/env bash
HOST=192.168.0.99
cp arch/mips/boot/uImage.lzma kernel.bin
ftp-upload -h ${HOST} -u root --password ismart12 -d / kernel.bin



## Load with
# ext4load mmc 0:1 0x80600000 kernel-new.bin;bootm 0x80600000