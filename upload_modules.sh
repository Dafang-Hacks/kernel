#!/usr/bin/env bash
HOST=192.168.0.99
cp drivers/net/wireless/rtl818x/rtl8188eu/rtl8189es.ko rtl8189es.ko
cp sound/sound_firmware.ko audio.ko

ftp-upload -h ${HOST} -u root --password ismart12 -d /lib/modules/3.10.108/ rtl8189es.ko