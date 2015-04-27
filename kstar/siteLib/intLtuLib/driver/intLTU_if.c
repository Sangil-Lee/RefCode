/* ------------------------------------------------------------------------- */
/* intLTUif.c interface layer for intLTU peripheral                          */
/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2006 Peter Milne, D-TACQ Solutions Ltd
 *                      <Peter dot Milne at D hyphen TACQ dot com>
                                                                               
    This program is free software; you can redistribute it and/or modify
    it under the terms of Version 2 of the GNU General Public License
    as published by the Free Software Foundation;
                                                                               
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                                                                               
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/* ------------------------------------------------------------------------- */

/** @file intLTU_if.c interface layer for intLTU peripheral. */

#include <linux/kernel.h>
/*#include <linux/interrupt.h> */
#include <linux/pci.h>
#include <linux/time.h>
#include <linux/interrupt.h>       
#include <linux/init.h>
#include <linux/timex.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/moduleparam.h>

#include <linux/device.h>

#include <linux/proc_fs.h>

#include <asm/uaccess.h>  /* VERIFY_READ|WRITE */

#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#include <linux/module.h>
#endif



#include "intLTU_debug.h"
#include "intLTU_driver.h"



static ssize_t show_interrupt_count(
	struct device *dev,
	struct device_attribute *attr,
	char *buf)
{
	CLTU_Device *device = intLTU_lookupDevice(dev);

	if (device != 0){
		return sprintf(buf, "%d %d \n", 
			       device->interrupt_count[0],
			       device->interrupt_count[1]
			);
	}else{
		return -ENODEV;
	}
}

static DEVICE_ATTR(interrupt_count, S_IRUGO, show_interrupt_count, 0);



void intLTU_device_create_file(
	struct device * dev, struct device_attribute * attr,
	const char *file, int line)
{
	if (device_create_file(dev, attr)){
		err("%s:%d device_create_file", file, line);
	}
}


void intLTU_create_sysfs_device(struct device *dev)
{
	dbg(1, "01");
	DEVICE_CREATE_FILE(dev, &dev_attr_host_pa);
	dbg(1, "99");
}

void intLTU_remove_sysfs_device(struct device *dev)
{
	device_remove_file(dev, &dev_attr_host_pa);
}


#ifdef ORIGINAL_CLASS_DEVICE_INTERFACE
void intLTU_class_device_create_file(
        struct class_device * dev, struct class_device_attribute * attr,
        const char *file, int line)
{
        if (class_device_create_file(dev, attr)){
                err("%s:%d device_create_file", file, line);
        }
}
/*
#define CLASS_DEVICE_CREATE_FILE(dev, attr) \
        intLTU_class_device_create_file(dev, attr, __FILE__, __LINE__)

void intLTU_create_sysfs_class(struct class_device *dev)
{
        dbg(1, "01");
        CLASS_DEVICE_CREATE_FILE(dev, &class_device_attr_imask);
        CLASS_DEVICE_CREATE_FILE(dev, &class_device_attr_dev);
        dbg(1, "9");
}

void intLTU_remove_sysfs_class(struct class_device *dev)
{
        class_device_remove_file(dev, &class_device_attr_imask);
        class_device_remove_file(dev, &class_device_attr_dev);
}


#else
void intLTU_create_sysfs_class(struct device *dev)
{
	dbg(1, "01");
	DEVICE_CREATE_FILE(dev, &dev_attr_imask);
	DEVICE_CREATE_FILE(dev, &dev_attr_dev);
	dbg(1, "9");
}

void intLTU_remove_sysfs_class(struct device *dev)
{
	device_remove_file(dev, &dev_attr_imask);
	device_remove_file(dev, &dev_attr_dev);
}

#endif
*/


