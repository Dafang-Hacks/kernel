#!/usr/bin/env bash
HOST=192.168.0.43
cp arch/mips/boot/uImage.lzma kernel.bin
cp drivers/net/wireless/rtl818x/rtl8188eu/wlan.ko rtl8189es.bin
ftp-upload -h ${HOST} -u root --password ismart12 -d /system/sdcard/ kernel.bin
ftp-upload -h ${HOST} -u root --password ismart12 -d /driver rtl8189es.bin
