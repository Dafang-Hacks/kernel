/*
 * This modules maps additional mtd partions of the Motorola Milestone
 *
 * Copyright (C) 2010 Janne Grunau
 * Copyright (C) 2010 Mike Baker
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */


#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#define mtd_hack_part(pname, psize)	.name       = pname,              \
					.size       = psize * 1024,       \
					.offset     = MTDPART_OFS_APPEND, \
					.mask_flags = MTD_WRITEABLE,

#define mtd_hack_partw(pname, psize)     .name       = pname,              \
					.size       = psize * 1024,       \
					.offset     = MTDPART_OFS_APPEND, \


/* MTD partition layout:
*/


struct mtd_partition part[] = {
	{	mtd_hack_part("bootw", 256)},

};

static int create_missing_flash_parts(struct device *dev, void *data)
{
int ret;
	struct mtd_info *mtd = NULL;

	printk(KERN_INFO "mtd-hack: device %s\n", dev->init_name);

	mtd = dev_get_drvdata(dev);

	if (!mtd)
		return -1;

        printk(KERN_INFO "mtd-hack: mtd name %s, type %d, size %llu\n",
                mtd->name, mtd->type, mtd->size);

        /*
        if (mtd->read) {
                size_t ret = 0;
                u_char buf[520];
                int i;
//  int (*read) (struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf);
                mtd->read(mtd, 7995392, 512, &ret, buf);

                printk(KERN_INFO "flash contents: ");
                for (i=0; i < ret; i++)
                        printk(KERN_INFO "0x%x ", buf[i]);
        }
        */
    ret = mtd_del_partition(mtd, 0);
    printk(KERN_INFO "mtd_del_partition returned : %d",ret);
    ret = mtd_del_partition(mtd, 1);
    printk(KERN_INFO "mtd_del_partition returned : %d",ret);

	mtd_add_partition(mtd, "uboot1",0,128*1024);
	printk(KERN_INFO "mtd_add_partition returned : %d",ret);

    mtd_add_partition(mtd, "uboot2",128*1024,128*1024);
    printk(KERN_INFO "mtd_add_partition returned : %d",ret);

	return 0;
}

static int __init mtd_init(void)
{
	struct device_driver *devdrv;
	int err = 0;

    printk(KERN_INFO "mtd hack loaded");
	//	struct device_driver *driver_find(const char *name, struct bus_type *bus);
	devdrv = driver_find("jz-sfc", &platform_bus_type);

	printk(KERN_INFO "mtd-hack: found driver %s modname %s\n", devdrv->name, devdrv->mod_name);
	//	int driver_for_each_device(struct device_driver *drv, struct device *start,
	//	                           void *data, int (*fn)(struct device *, void *))
	err = driver_for_each_device(devdrv, NULL, NULL, create_missing_flash_parts);


	printk(KERN_INFO "mtd hack loaded");

	return 0;
}

module_init(mtd_init);

MODULE_LICENSE("GPL");
