# Makefile for the xburst sound oss driver
#
# 24 APR 2012, <jbbi: jbbi@ingernic.cn>
#

ifeq ($(CONFIG_COMPILE_JZSOUND_INTO_KO),y)
obj-n 		+= devices/
obj-n 		+= interface/ xb_snd_card.o
else
obj-y 		+= devices/
obj-y 		+= interface/ xb_snd_card.o
endif

obj-$(CONFIG_SOUND_XBURST_DETECT)	+= xb_snd_detect.o

