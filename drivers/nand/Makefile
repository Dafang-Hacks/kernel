# Makefile for nand block
#

EXTRA_CFLAGS	+= -Idrivers/nand/inc/ \
		-Idrivers/nand/manager/inc/

obj-$(CONFIG_NAND)	+= manager/ nand_block.o nand_char.o nand_debug.o
obj-$(CONFIG_NAND)	+= driver/

ifndef CONFIG_ANDROID
	obj-$(CONFIG_NAND)	+= nandfs/
endif