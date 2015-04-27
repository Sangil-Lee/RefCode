/*****************************************************************************
 * pxi-6259-drv.c
 *
 * The NI PXI-6259 driver.
 * Author: Janez Golob (Cosylab)
 * Copyright (C) 2010-2013 ITER Organization
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of the Linux
 * distribution for more details.
 ****************************************************************************/

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h> /* printk() */
#include <linux/slab.h>   /* kmalloc() */
#include <linux/fs.h>     /* everything... */
#include <linux/errno.h>  /* error codes */
#include <linux/types.h>  /* size_t */
#include <linux/fcntl.h>
#include <linux/cdev.h>
#include <linux/tty.h>
#include <asm/atomic.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/pci.h>
#include <linux/kmod.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/version.h>  /* KERNEL_VERSION */

#include <asm/system.h>         /* cli(), *_flags */
#include <asm/uaccess.h>        /* copy_*_user */

#include "pxi-6259-drv.h"        /* local definitions */
#include "pxi6259.h"
#include "pxi6259-enums.h"

/*
 * Global definitions
 */
#define MODULE_CONTEXTS 4

#define PCI_DEVICE_ID_NI_PXI6259 0x70b2         // 0x71dc, 70b2,  727e,  NOT: 8360,
#define PCI_VENDOR_ID_NI 0x1093
#define PXI6259_TOTAL_SIZE 0x20
#define PXI6259_DMA_MASK 0xffffffff
#define DRV_NAME "pxi6259"
#define DEVICE_NAME "pxi6259"

/* TODO : YSW added to support PCIe6259. This information was got from Changseong Kim */
#ifndef PCI_DEVICE_ID_NI_PCIe6259	/* for PCIe6259 */
#define PCI_DEVICE_ID_NI_PCIe6259 0x717f
#endif

/* TODO : YSW added to support 3.2.33-rt50.66.el6rt.x86_64 */
#ifndef	DMA_32BIT_MASK	/* for 3.2.33-rt50.66.el6rt.x86_64 */
#define	DMA_32BIT_MASK	PXI6259_DMA_MASK
#endif

/*
 * container_of_array(ptr, type, member, arr_offset, elsize)
 *
 * cast a member of a structure out to the containing structure
 *
 * @ptr:        the pointer to the member.
 * @type:       the type of the container struct this is embedded in.
 * @member:     the name of the member within the struct.
 * @arr_offset: the index in the array
 * @elsize:   : element size in bytes
 */
#define container_of_array(ptr, type, member, arr_offset, elsize) ({                      \
        const typeof( ((type *)0)->member ) *__mptr = (void*)(ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) - arr_offset * elsize );})


static struct pci_device_id ids[] = {
        { PCI_DEVICE(PCI_VENDOR_ID_NI, PCI_DEVICE_ID_NI_PXI6259), },
		/* TODO : YSW added to allow searching PCIe6259 devices */
        { PCI_DEVICE(PCI_VENDOR_ID_NI, PCI_DEVICE_ID_NI_PCIe6259), },
        { 0, }
};
MODULE_DEVICE_TABLE(pci, ids);

static int pxi6259_create_devices(struct module_context *mcon);
static int pxi6259_delete_devices(struct module_context *mcon);
static int pxi6259_update_ai_devices(struct module_context *mcon, dev_t aodev);
static int pxi6259_update_ao_devices(struct module_context *mcon, dev_t aodev);
static int pxi6259_update_dio_devices(struct module_context *mcon, dev_t diodev);
static int pxi6259_update_gpc_devices(struct module_context *mcon, dev_t gpcdev);
static int pxi6259_reset(struct module_context *mcon);

static inline void pxi6259_irq_put_event(struct queue *queue, struct pxi6259_read_buffer rb);
static int pxi6259_raw_io(struct module_context *mcon, struct pxi6259_raw_io_block *riob, uint32_t flag);

static int readFromEeprom(struct module_context *mcon, uint32_t offset, uint8_t *eepromBuffer, uint32_t bufferSize);


/*
 * Analog Input
 */
static int pxi6259_reset_ai(struct module_context *mcon);
static int pxi6259_load_ai_conf(struct module_context *mcon, struct pxi6259_ai_conf *ai_conf);
static int pxi6259_start_ai(struct module_context *mcon);
static int pxi6259_stop_ai(struct module_context *mcon);
static int pxi6259_release_ai(struct module_context *mcon);
static int pxi6259_read_ai_fifo(struct module_context *mcon, struct pxi6259_rw_buffer *buffer);
static int pxi6259_ai_sample_on_demand(struct module_context *mcon, uint32_t num_puls);

/*
 * Analog Output
 */
static int pxi6259_reset_ao(struct module_context *mcon);
static int ao_reset(struct module_context *mcon);
static int pxi6259_load_ao_conf(struct module_context *mcon, struct pxi6259_ao_conf *ao_task);
static int pxi6259_start_ao(struct module_context *mcon);
static int pxi6259_is_ao_done(struct module_context *mcon, uint8_t *is_task_done);
static int pxi6259_stop_ao(struct module_context *mcon);
static int pxi6259_release_ao(struct module_context *mcon);
static int pxi6259_ao_write_to_channel(struct module_context *mcon,
                enum pxi6259_channel channel, uint32_t value);
static int pxi6259_ao_write_to_fifo(struct module_context *mcon, struct pxi6259_rw_buffer *buffer);

/*
 * GPC - General Purpose Counter functions
 */
static int pxi6259_reset_gpc(struct module_context *mcon);
static int pxi6259_load_gpc0_conf(struct module_context *mcon, struct pxi6259_gpc_conf *gpc_conf);
static int pxi6259_load_gpc1_conf(struct module_context *mcon, struct pxi6259_gpc_conf *gpc_conf);
static int pxi6259_start_gpc0(struct module_context *mcon);
static int pxi6259_start_gpc1(struct module_context *mcon);
static int pxi6259_stop_gpc0(struct module_context *mcon);
static int pxi6259_stop_gpc1(struct module_context *mcon);
static int pxi6259_get_gpc0_value(struct module_context *mcon, uint32_t *gpc_value);
static int pxi6259_get_gpc1_value(struct module_context *mcon, uint32_t *gpc_value);
static int gpc0_reset(struct module_context *mcon);
static int gpc1_reset(struct module_context *mcon);

/*
 * DIO
 */
static int pxi6259_reset_dio(struct module_context *mcon);
static int pxi6259_load_dio_conf(struct module_context *mcon, struct pxi6259_dio_conf *dio_conf);
static int pxi6259_read_dio_port(struct module_context *mcon, dio_port_select_t port, void *to);
static int pxi6259_write_to_dio_port (struct module_context *mcon, dio_port_select_t port, void *from);

/*
 * Scaling
 */
static int pxi6259_ai_get_scaling_coeficient(struct module_context *mcon, uint32_t intervalIDx,uint32_t modeIDx,ai_scaling_coefficients_t *scaling);
static int pxi6259_ao_get_scaling_coeficient(struct module_context *mcon, uint32_t intervalIDx,uint32_t modeIDx,ao_scaling_coefficients_t *scaling);
static int pxi6259_eeprom_read_M_series(struct module_context *mcon,uint8_t *eeprom_buffer,
                uint32_t buffer_size);

/*
 * DMA
 */
static void free_dma_buffer(struct module_context *mcon, struct dma_buffer *dma);
static int dma_channel_config(struct module_context *mcon, struct dma_channel *chan,
                u32 request_source, enum dma_mode mode, enum dma_data_direction direction,
                u32 size, u8 xfer_width, u32 mite_offset);
static int dma_channel_stop(struct module_context *mcon, struct dma_channel *chan);
static int dma_channel_start(struct module_context *mcon, struct dma_channel *chan);
static void dma_channel_configure(struct module_context *mcon, struct dma_channel *chan);
static int dma_channel_reset(struct module_context *mcon, struct dma_channel *chan);
static u32 bytes_in_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info * cli_info);
inline static int bytes_free_or_error(struct module_context *mcon, unsigned int chan_number);

static int pxi6259_read_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info *cli_info, struct pxi6259_rw_buffer *buffer);
static int pxi6259_write_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info *cli_info, struct pxi6259_rw_buffer *buffer);

/*
 * Signal management functions
 */
static int pxi6259_connect_trigger(struct module_context *mcon,struct pxi6259_trigger_connection *trig);
static int pxi6259_disconnect_trigger(struct module_context *mcon,struct pxi6259_trigger_connection *trig);
static int pxi6259_export_signal(struct module_context *mcon,enum pxi6259_signal source,
                enum pxi6259_terminal terminal,uint8_t enable);

/*
 * TODO: Parameters which can be set at load time.
 */
int device_major = 0;
int device_minor = 0;

struct class *pxi6259_class;

MODULE_AUTHOR("Janez Golob");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("National Instruments PXI-6259 driver");


/* The list of module contexts, and a lock to protect it */
static int32_t module_contexts = 0;

struct module_context_listitem {
        struct module_context module;
        struct pci_dev *key;
        struct list_head list;
};

static LIST_HEAD(module_context_list);
static DEFINE_SPINLOCK(module_context_lock);

/**
 * TODO
 *      - chec errors and remove module context...
 */
static int __devinit pxi6259_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
        int retval = 0, i;
        struct module_context_listitem *lptr;
        uint32_t bar1_value, device_io_window_addr;

        retval = pci_enable_device(pdev);
        if (retval < 0) {
                printk(KERN_ERR "failed to enable device %d \n", retval);
                return retval;
        }

        pci_set_master(pdev);

        retval = pci_set_dma_mask(pdev, DMA_32BIT_MASK);
        if (!retval)
                retval = pci_set_consistent_dma_mask(pdev, DMA_32BIT_MASK);
        if (retval) {
                printk(KERN_WARNING DRV_NAME ": No suitable DMA available.\n");
                goto out_pci_disable_device;
        }

        retval = pci_request_regions(pdev, DRV_NAME);
        if (retval < 0) {
                PDEBUG(DRV_NAME ": IO address range already allocated.\n");
                goto out_pci_release_regions;
        }

        /* Add new module to mudole context list */
        spin_lock(&module_context_lock);

        list_for_each_entry(lptr, &module_context_list, list) {
                if (lptr->key == pdev)
                        goto out_pci_release_regions;
        }

        lptr = kmalloc(sizeof(struct module_context_listitem), GFP_KERNEL);
        if (!lptr) {
               retval = -ENOMEM;
               goto out_pci_release_regions;  /* Make this more graceful */
        }
        memset(lptr, 0, sizeof(struct module_context_listitem));

        lptr->module.pci_dev = pdev;
        lptr->key = pdev;

        mutex_init(&lptr->module.ai_mutex);
        mutex_init(&lptr->module.ao_mutex);
        mutex_init(&lptr->module.dio_mutex);
        mutex_init(&lptr->module.gpc_mutex);

        INIT_LIST_HEAD(&lptr->module.aichan_list);
        INIT_LIST_HEAD(&lptr->module.aochan_list);

        for (i=0; i<PCI_NUM_RESOURCES && i < PXI6259_ADDRESS_SPACES; i++) {
                if (pci_resource_flags(pdev, i) & IORESOURCE_MEM) {
                        lptr->module.addresses.spaces[i].base_address
                                = pci_resource_start(pdev, i);
                        lptr->module.addresses.spaces[i].window
                                = pci_resource_len(pdev, i);
                        lptr->module.addresses.spaces[i].space
                                = ioremap(lptr->module.addresses.spaces[i].base_address,
                                                lptr->module.addresses.spaces[i].window);
                        if (!lptr->module.addresses.spaces[i].space) {
                                printk(KERN_WARNING
                                                " resource number %d cannot be maped to kernel space", i);
                        }
                } else {
                        printk(KERN_WARNING
                                        "resurce number %d is not mem resource\n", i);
                        lptr->module.addresses.spaces[i].base_address = 0;
                        lptr->module.addresses.spaces[i].window = 0;
                }
        }

        lptr->module.index = module_contexts;

        retval = pxi6259_create_devices(&lptr->module);
        if (retval) {
                printk(KERN_ERR
                                "error creating subdevices\n");
                goto out_release_irq;
        }

        /*
         * TODO: move this to reset function
         */
        if (lptr->module.addresses.spaces[0].base_address
                        && lptr->module.addresses.spaces[BAR1].base_address) {
                /* Enable the I/O Device Window */
                bar1_value = ioread32(lptr->module.addresses.spaces[BAR0].space + 0x314);
                /* ...mask out the lower 8-bits, which are reserved. */
                device_io_window_addr = (bar1_value & 0xffffff00) | 0x80;
                /* ...finally, enable the I/O Device window */
                iowrite32(device_io_window_addr, lptr->module.addresses.spaces[BAR0].space + 0x0C0);
                /* Enable I/O Device Interrupts */
                iowrite32((1 << 24), lptr->module.addresses.spaces[BAR0].space + 0x008);
        }

        /* Register IRQ handler
        retval = request_irq(dev->irq, pxi6259_irq_handler,
                IRQF_SHARED,
                DRV_NAME,
                (void *)&lptr->module);
        if (retval) {
                printk(KERN_ERR "Cannot install IRQ handler.\n");
                goto out;
        }
        printk(KERN_NOTICE "%s IRQ installed %d\n", pci_name(dev), dev->irq);*/

        /* Initialize AI DMA buffer */
        lptr->module.ai_chan.state = DMA_IDLE;
        lptr->module.ai_chan.mite_offset = AI_CHAN_MITE_OFFSET;
        retval = dma_channel_reset(&lptr->module, &lptr->module.ai_chan);
        if (retval) {
                printk(KERN_ERR "%s failed to reset AI DMA channel %d!\n",
                                pci_name(pdev), retval);
                goto out_release_irq;
        }

        retval = dma_channel_config(&lptr->module, &lptr->module.ai_chan, 0,
                        DMA_MODE_RING, DMA_FROM_DEVICE, 524288, AI_CHAN_MITE_DATA_WIDTH, AI_CHAN_MITE_OFFSET);
        if (retval) {
                printk(KERN_ERR "%s failed to configure AI DMA channel %d!\n",
                                pci_name(pdev), retval);
                goto out_release_irq;
        }

        /* Initialize AO DMA buffers */
        lptr->module.ao_chan.state = DMA_IDLE;
        lptr->module.ao_chan.mite_offset = AO_CHAN_MITE_OFFSET;
        retval = dma_channel_reset(&lptr->module, &lptr->module.ao_chan);
        if (retval) {
                printk(KERN_ERR "%s failed to reset AO DMA channel %d!\n",
                                pci_name(pdev), retval);
                goto out_release_irq;
        }

        retval = dma_channel_config(&lptr->module, &lptr->module.ao_chan, 1,
                        DMA_MODE_RING, DMA_TO_DEVICE, 4000, AO_CHAN_MITE_DATA_WIDTH, AO_CHAN_MITE_OFFSET);
        if (retval) {
                printk(KERN_ERR "%s failed to configure AO DMA channel %d!\n",
                                pci_name(pdev), retval);
                goto out_free_ai_dma;
        }

        pxi6259_reset(&lptr->module);

        /* Read EEPROM calibration values */
        PDEBUG(KERN_DEBUG "%s Read M Series EEPROM\n",pci_name(lptr->module.pci_dev));
        readFromEeprom(&lptr->module,PXI6259_EEPROM_CAL_OFFSET,lptr->module.cal_eeprom,PXI6259_EEPROM_SIZE);

        /* Add module context */
        list_add(&lptr->list, &module_context_list);
        module_contexts++;

        spin_unlock(&module_context_lock);
        return 0;

  out_free_ai_dma:
        free_dma_buffer(&lptr->module, lptr->module.ai_chan.buffer);

  out_release_irq:
        //free_irq(dev->irq, &lptr->module);
  out_iounmap:
        for (i=0; i<PCI_NUM_RESOURCES && i < PXI6259_ADDRESS_SPACES; i++) {
                if(lptr->module.addresses.spaces[i].space) {
                        iounmap((void *)lptr->module.addresses.spaces[i].base_address);
                }
        }
  out_free_module_context:
        kfree(lptr);
  out_pci_release_regions:
        pci_release_regions(pdev);
  out_pci_disable_device:
        pci_disable_device(pdev);
        pci_set_drvdata(pdev, NULL);

        spin_unlock(&module_context_lock);
        return retval;
}


static void __devexit pxi6259_remove(struct pci_dev *dev)
{
        struct module_context_listitem *lptr, *next;
        int i;

        /* Remove module from module context list */
        spin_lock(&module_context_lock);

        list_for_each_entry_safe(lptr, next, &module_context_list, list) {
                if (lptr->key == dev) {

                        /* Unmap all memory regions */
                        for (i=0; i<PCI_NUM_RESOURCES && i < PXI6259_ADDRESS_SPACES; i++) {
                                if(lptr->module.addresses.spaces[i].space) {
                                        iounmap((void *)lptr->module.addresses.spaces[i].base_address);
                                }
                        }

                        /* Free DMA buffers */
                        free_dma_buffer(&lptr->module, lptr->module.ai_chan.buffer);
                        free_dma_buffer(&lptr->module, lptr->module.ao_chan.buffer);

                        /* Disconnect irq handler */
                        //free_irq(dev->irq, &lptr->module);

                        pci_release_regions(dev);
                        pci_disable_device(dev);


                        pxi6259_delete_devices(&lptr->module);

                        list_del(&lptr->list);
                        module_contexts--;
                        kfree(lptr);
                }
        }

        spin_unlock(&module_context_lock);
}


static struct pci_driver sync_driver = {
        .name = "ni-pxi-6259",
        .id_table = ids,
        .probe = pxi6259_probe,
        .remove = pxi6259_remove,
};


/*
 * Open function
 *
 * Function returns EBUSY if the device is already opened.
 */
static int pxi6259_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon = container_of(inode->i_cdev, struct module_context, cdev);
        filp->private_data = mcon; /* for other methods */

        PDEBUG("%s Open...\n", pci_name(mcon->pci_dev));

        return 0;
}

/*
 * The ioctl() implementation
 */
static long pxi6259_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        struct pxi6259_raw_io_block raw_io_block;
        struct module_context *mcon = (struct module_context *)filp->private_data;
        struct pxi6259_trigger_connection trigger;
        int retval = 0, i;
        struct pxi6259_client_list clilist;
        struct aichan_listitem *lptr;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                retval = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                retval =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (retval) return -EFAULT;

        mutex_lock(&mcon->ai_mutex);
        mutex_lock(&mcon->ao_mutex);
        mutex_lock(&mcon->dio_mutex);
        mutex_lock(&mcon->gpc_mutex);

        switch(cmd) {
          case PXI6259_IOC_RAW_READ:
                  memset(&raw_io_block, 0, sizeof(raw_io_block));
                  retval = copy_from_user((void *) &raw_io_block, (void *)arg, sizeof(raw_io_block));
                  if (!retval) {
                          retval = pxi6259_raw_io(mcon, &raw_io_block, 0); //TODO lock
                  }
                  break;

          case PXI6259_IOC_RAW_WRITE:
                  memset(&raw_io_block, 0, sizeof(raw_io_block));
                  retval = copy_from_user((void *) &raw_io_block, (void *)arg, sizeof(raw_io_block));
                  if (!retval) {
                          retval = pxi6259_raw_io(mcon ,&raw_io_block, 1); //TODO lock
                  }
                  break;

          case PXI6259_IOC_GET_MODULE_ADDR:
                  retval = copy_to_user((void *)arg, &mcon->addresses, sizeof(mcon->addresses));
                  if (retval) {
                          retval = -EINVAL;
                          break;
                  }
                  break;

          case PXI6259_IOC_CONNECT_TRIGGER:
                  retval = copy_from_user((void *) &trigger, (void *)arg, sizeof(trigger));
                  if (retval){
                          retval = -EINVAL;
                          break;
                  }

                  retval = pxi6259_connect_trigger(mcon, &trigger);
                  break;

          case PXI6259_IOC_DISCONNECT_TRIGGER:
                  retval = copy_from_user((void *) &trigger, (void *)arg, sizeof(trigger));
                  if (retval){
                          retval = -EINVAL;
                          break;
                  }

                  retval = pxi6259_disconnect_trigger(mcon, &trigger);
                  break;

          case PXI6259_IOC_GET_AI_CLIENT_LIST:
                  i=0; clilist.size = 0;
                  list_for_each_entry(lptr, &mcon->aichan_list, list) {
                      if (i<PXI6259_MAX_CLIENT_CONNECTIONS) {
                              clilist.pids[i] = lptr->pid;
                              i++;
                              clilist.size = i;
                      }
                  }
                  retval = copy_to_user((void *) arg, (void *)&clilist, sizeof(clilist));
                  if (retval){
                          retval = -EINVAL;
                          break;
                  }
                  break;

          case PXI6259_IOC_RESET:
                  retval = pxi6259_reset(mcon);
                  break;

          case PXI6259_IOC_GET_DEVICE_ID:
                  retval = copy_to_user((void *)arg, &mcon->index, sizeof(u32));
                  break;

          default:  /* redundant, as cmd was checked against MAXNR */
                retval = -ENOTTY;
        }

  out:
        mutex_unlock(&mcon->ai_mutex);
        mutex_unlock(&mcon->ao_mutex);
        mutex_unlock(&mcon->dio_mutex);
        mutex_unlock(&mcon->gpc_mutex);

        return retval;
}

/* INITIALIZATION FUNCTIONS */

static int readFromEeprom(      struct module_context *mcon,
                                uint32_t offset,
                                uint8_t *eepromBuffer,
                                uint32_t bufferSize)
{
        uint32_t _iowcr1Initial;
        uint32_t _iowbsr1Initial;
        uint32_t _iodwbsrInitial;
        uint32_t _bar1Value;

        uint32_t i;

        void *bar0;
        void *bar1;

        bar0 = mcon->addresses.spaces[BAR0].space;
        if (!bar0) {
                printk(KERN_ERR "%s address space not mapped!\n",
                                pci_name(mcon->pci_dev));
                return -EFAULT;
        }
        bar1 = mcon->addresses.spaces[BAR1].space;
        if (!bar1) {
                printk(KERN_ERR "%s address space not mapped!\n",
                                pci_name(mcon->pci_dev));
                return -EFAULT;
        }

        //_iodwbsrInitial = bar0.read32 (0xC0);
        _iodwbsrInitial = ioread32((void *)bar0 + 0xC0);

        // bar0.write32 (0xC0, 0x00);
        iowrite32(0,(void *)bar0 + 0xC0);

        //  _iowbsr1Initial = bar0.read32 (0xC4);
        _iowbsr1Initial = ioread32((void *)bar0 + 0xC4);

        //  _bar1Value      = bar0.read32 (0x314);
        _bar1Value = ioread32((void *)bar0 + 0x314);

        //bar0.write32 (0xC4, (0x0000008B | _bar1Value));
        iowrite32((0x0000008B | _bar1Value),(void *)bar0 + 0xC4);

        //_iowcr1Initial = bar0.read32(0xF4);
        _iowcr1Initial = ioread32((void *)bar0 + 0xF4);

        //bar0.write32 (0xF4, 0x00000001 | _iowcr1Initial);
        iowrite32((0x00000001 | _iowcr1Initial),(void *)bar0 + 0xF4);

        //bar0.write32 (0x30, 0xF);
        iowrite32(0xF,(void *)bar0 + 0x30);

        // ---- Read EEPROM
        for(i = 0; i < bufferSize; ++i)
        {
                //eepromBuffer[i] = bar1.read8(i + offset);
                eepromBuffer[i] = ioread8(((void *)bar1 + offset + i));
                PVDEBUG (KERN_ERR "%s %x\n",pci_name(mcon->pci_dev),eepromBuffer[i]);
        }

        // ---- Close EEPROM

        //bar0.write32 (0xC4, _iowbsr1Initial);
        iowrite32(_iowbsr1Initial,(void *)bar0 + 0xC4);
        //bar0.write32 (0xC0, _iodwbsrInitial);
        iowrite32(_iodwbsrInitial,(void *)bar0 + 0xC0);
        //bar0.write32 (0xF4, _iowcr1Initial);
        iowrite32(_iowcr1Initial,(void *)bar0 + 0xF4);
        //bar0.write32 (0x30, 0x00);
        iowrite32(0x00,(void *)bar0 + 0x30);

        return 0;
}


/*
 * TODO:
 *      - check if we need memory barrier.
 */
static int flush_register(struct module_context *mcon, enum pxi6259_register reg)
{
        uint8_t dw = reg_width[reg];
        uint32_t offset = reg_offsets[reg];
        uint32_t mask = flush_mask[reg];
        uint8_t *u8val;
        uint16_t *u16val;
        uint32_t *u32val;

        void *mmap;

        mmap = mcon->addresses.spaces[BAR1].space;
        if (!mmap) {
                printk(KERN_ERR "%s address space not mapped!\n",
                                pci_name(mcon->pci_dev));
                return -EFAULT;
        }

        /* Add offset */
        mmap = (void *) (mmap + offset);

        wmb();

        if (dw & 16) {
                u16val = (uint16_t *) &mcon->register_space[offset];
                iowrite16(*u16val, mmap);
                //PVDEBUG("%s write register (16 bit) offset %x val %x\n", offset, *u16val);
                PVDEBUG("write16 %x %x\n",*u16val, offset);
                *u16val &= (uint16_t) mask;
        } else if (dw & 8) {
                u8val = (uint8_t *) &mcon->register_space[offset];
                iowrite8(*u8val, mmap);
                //PVDEBUG("%s write register (8 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, *u8val);
                PVDEBUG("write8 %x %x\n",*u8val, offset);
                *u8val &= (uint8_t) mask;
        } else if (dw & 32) {
                u32val = (uint32_t *) &mcon->register_space[offset];
                iowrite32(*u32val, mmap);
                //PVDEBUG("%s write register (32 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, *u32val);
                PVDEBUG("write32 %x %x\n",*u32val, offset);
                *u32val &= (uint32_t) mask;
        } else {
                printk(KERN_ERR "%s data width %x is not supported!\n",
                                pci_name(mcon->pci_dev), dw);
                return -EINVAL;
        }
        return 0;
}

/*
 * TODO:
 *      - check if we need memory barrier.
 */
static int write_register(struct module_context *mcon, enum pxi6259_register reg, uint32_t val)
{
        uint8_t dw = reg_width[reg];
        uint32_t offset = reg_offsets[reg];
        uint32_t mask = flush_mask[reg];
        uint8_t *u8val;
        uint16_t *u16val;
        uint32_t *u32val;

        void *mmap;

        mmap = mcon->addresses.spaces[BAR1].space;
        if (!mmap) {
                printk(KERN_ERR "%s address space not mapped!\n",
                                pci_name(mcon->pci_dev));
                return -EFAULT;
        }

         /* Add offset */
        mmap = (void *) (mmap + offset);

        wmb();

        if (dw & 16) {
                u16val = (uint16_t *) &mcon->register_space[offset];
                *u16val = (uint16_t) val;
                iowrite16(*u16val, mmap);
                //PVDEBUG("%s write register (16 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, val);
                PVDEBUG("write16 %x %x\n",val, offset);
                *u16val &= (uint16_t) mask;
        } else if (dw & 8) {
                u8val = (uint8_t *) &mcon->register_space[offset];
                *u8val = (uint8_t) val;
                iowrite8(*u8val, mmap);
                //PVDEBUG("%s write register (8 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, val);
                PVDEBUG("write8 %x %x\n",val, offset);
                *u8val &= (uint8_t) mask;
        } else if (dw & 32) {
                u32val = (uint32_t *) &mcon->register_space[offset];
                *u32val = (uint32_t) val;
                iowrite32(*u32val, mmap);
               //PVDEBUG("%s write register (32 bit) offset %x val %x\n",
               //                 pci_name(mcon->pci_dev), offset, val);
                PVDEBUG("write32 %x %x\n",val, offset);
                *u32val &= (uint32_t) mask;
        } else {
                printk(KERN_ERR "%s data width %x is not supported!\n",
                                pci_name(mcon->pci_dev), dw);
                return -EINVAL;
        }

        return 0;
}

static int set_register_bits(struct module_context *mcon, enum pxi6259_register reg,
                uint32_t bit_mask, uint32_t bit_values)
{
        uint8_t dw = reg_width[reg];
        uint32_t offset = reg_offsets[reg];
        uint8_t *u8val;
        uint16_t *u16val;
        uint32_t *u32val;

#ifdef PXI6259_VDEBUG
        uint32_t mask = flush_mask[reg];
#endif

        if (!bit_mask) {
                return 0;
        }

        bit_mask &= (0xFFFFFFFF >> (32-dw));
        bit_values &= (0xFFFFFFFF >> (32-dw));

        PVDEBUG("%s set register bits [DW: %x] offset %x, mask %x, bit values %x\n",
                        pci_name(mcon->pci_dev), dw, offset, mask, bit_values);

        if (dw & 16) {
                u16val = (uint16_t *) &mcon->register_space[offset];
                *u16val &= ~bit_mask;
                *u16val |= bit_values & bit_mask;
        } else if (dw & 8) {
                u8val = (uint8_t *) &mcon->register_space[offset];
                *u8val &= ~bit_mask;
                *u8val |= bit_values & bit_mask;
        } else if (dw & 32) {
                u32val = (uint32_t *) &mcon->register_space[offset];
                *u32val &= ~bit_mask;
                *u32val |= bit_values & bit_mask;
        } else {
                printk(KERN_ERR "%s data width %x is not supported!\n",
                                pci_name(mcon->pci_dev), dw);
                return -EINVAL;
        }

        return 0;
}

static int set_register_bits_on_off(struct module_context *mcon,
                enum pxi6259_register reg, uint32_t bit_mask, uint8_t on_off)
{
        return set_register_bits(mcon, reg, bit_mask, on_off ? bit_mask : 0);
}

/*
 * TODO:
 *      - check if we need memory barrier.
 */
static int read_register(struct module_context *mcon, enum pxi6259_register reg, void *value)
{
        uint8_t dw = reg_width[reg];
        uint32_t offset = reg_offsets[reg];

        uint8_t u8val;
        uint16_t u16val;
        uint32_t u32val;

        void *mmap;

        mmap = mcon->addresses.spaces[BAR1].space;
        if (!mmap) {
                printk(KERN_ERR "%s address space not mapped!\n",
                                pci_name(mcon->pci_dev));
                return -EFAULT;
        }

        mb();

        /* Add offset */
        mmap = (void *) (mmap + offset);

        if (dw & 16) {
                u16val = ioread16(mmap);
                //PVDEBUG("%s read register (16 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, u16val);
                PVDEBUG("read16 %x %x\n", u16val, offset);
                *((uint16_t *)value) = u16val;
        } else if (dw & 8) {
                u8val = ioread8(mmap);
                //PVDEBUG("%s read register (8 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, u8val);
                PVDEBUG("read8 %x %x\n", u8val, offset);
                *((uint8_t *)value) = u8val;
        } else if (dw & 32) {
                u32val = ioread32(mmap);
                //PVDEBUG("%s read register (32 bit) offset %x val %x\n",
                //                pci_name(mcon->pci_dev), offset, u32val);
                PVDEBUG("read32 %x %x\n", u32val, offset);
                *((uint32_t *)value) = u32val;
        } else {
                printk(KERN_ERR "%s data width %x is not supported!\n",
                                pci_name(mcon->pci_dev), dw);
                return -EINVAL;
        }

        return 0;
}

/* DMA specific functions */

static u32 bytes_in_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info * cli_info)
{
        u32 device_addr = 0;
        u32 fifo_count = 0;
        u32 bytes_in_buffer = 0;

        void *device_addr_ptr = mcon->addresses.spaces[BAR0].space
                        + DeviceAddress_Register + chan->mite_offset;
        void *fifo_count_ptr = mcon->addresses.spaces[BAR0].space
                        + FifoCount_Register + chan->mite_offset;

        if (chan->state == DMA_IDLE || chan->state == DMA_UNKNOWN)
                return 0;

        if (chan->direction == DMA_FROM_DEVICE) {
                device_addr = ioread32(device_addr_ptr);
                fifo_count = ioread32(fifo_count_ptr);
                fifo_count &= 0xff;

             /* Update and store 64-bit DAR values. This allows us to use 64-bit
              * write_idx and not worry about rollover
              */
             if(cli_info->last_dar > device_addr) {
                     cli_info->real_dar = (cli_info->real_dar & 0xFFFFFFFF00000000ULL) | device_addr;
                     cli_info->real_dar +=                  0x0000000100000000ULL; /* u32 maxint + 1 */
                     cli_info->last_dar = device_addr;
             } else {
                     cli_info->real_dar = (cli_info->real_dar & 0xFFFFFFFF00000000ULL) | device_addr;
                     cli_info->last_dar = device_addr;
             }

             /* Update write_idx value. real_dar will [practically] never roll over, so it is safe to simply
              * subtract fifo_count.
              */
             cli_info->write_idx = cli_info->real_dar - fifo_count;

             /* Check if write_idx went down from last time. If so, is means that we're headed for an erroneous
              * DATA_NOT_AVAILABLE. Reset write_idx to previous value if necessary. If not, update last_write_idx.
              */
             if ((cli_info->write_idx < cli_info->last_write_idx ) ||
                             ((cli_info->last_write_idx == 0) && (device_addr < fifo_count))) {
                     PVDEBUG("     reseting write idx  %llu %llu %u\n", cli_info->write_idx, cli_info->last_write_idx, fifo_count);
                     cli_info->write_idx = cli_info->last_write_idx;

             }
             else
                     cli_info->last_write_idx = cli_info->write_idx;

        } else {
                cli_info->read_idx = ioread32(device_addr_ptr);
        }

        if (cli_info->wait_write_idx) {
                if (cli_info->write_idx <= cli_info->read_idx) {
                        return 0;
        } else {
                        cli_info->wait_write_idx = 0;
        }
        }

        /* Calculate difference between read and write indexes checking for rollovers */
        if (cli_info->write_idx < cli_info->read_idx) {
                bytes_in_buffer = 0xffffffff - (u32)( cli_info->read_idx - cli_info->write_idx);
                ++bytes_in_buffer;
                PVDEBUG ("    rolower %llu %llu\n", cli_info->write_idx, cli_info->read_idx);
        } else {
                bytes_in_buffer = (u32)(cli_info->write_idx - cli_info->read_idx);
        }

        PDEBUG ("bytes_in_buffer %u last_dar %u real_dar %llu write_idx %llu read_idx %llu buffer_location %llu fifo count %d\n", bytes_in_buffer,
                        cli_info->last_dar, cli_info->real_dar, cli_info->write_idx, cli_info->read_idx, cli_info->location, fifo_count);

        return bytes_in_buffer;
}

/*
 * WARNING: this function copy buffer into user space...
 */
static int read_linear_dma_buffer(struct module_context *mcon, struct dma_channel *chan,
                struct dma_cli_info *cli_info, u32 req_bytes, void *buffer)
{
        int retval = 0;

        u64 curr = cli_info->location;
        u64 end = (curr + req_bytes) % chan->buffer->size;

        /* Kernel virtual address */
        u8 *address = (u8 *) chan->buffer->address.cpu;
        u64 offset = 0;

        //PDEBUG("%s read linear DMA buffer\n", pci_name(mcon->pci_dev));

        if (end <= curr) {
                offset = chan->buffer->size - curr;
                retval = copy_to_user(buffer, address + curr, offset);
                if (retval) {
                        printk(KERN_ERR "copy part 1 to user failed!\n");
                        return retval;
                }

                buffer = (void*) ((u8 *) buffer + offset);
                curr = 0;
        }

        retval = copy_to_user(buffer, address + curr, (end - curr));
        if (retval) {
                printk(KERN_ERR "copy part 2 to user failed !\n");
                return retval;
        }

        cli_info->location = end;

        return 0;
}

// TODO dma buffer can be 2^n aligned
static int read_ai_chanel_from_linear_dma_buffer(struct module_context *mcon,
                struct dma_cli_info *cli_info, u32 num_samples, u32 chan_total, void *buffer)
{
        int retval = 0, i;

        u64 curr = cli_info->location;
        u64 step = AI_CHAN_MITE_DATA_WIDTH * chan_total;
        u64 next = 0;
        s16 *ptr = (s16 *)buffer;

        /* Kernel virtual address */
        u8 *address = (u8 *) mcon->ai_chan.buffer->address.cpu;

        PDEBUG("%s read %d samples from linear DMA buffer\n",
                        pci_name(mcon->pci_dev), num_samples);

        for (i=0; i< num_samples; i++) {
                retval = copy_to_user(&ptr[i], address + curr, AI_CHAN_MITE_DATA_WIDTH);
                if (retval) return -EFAULT;
                next = (curr + step) % mcon->ai_chan.buffer->size;
                PVDEBUG("copy to user %x %x curr %llu next %llu totoalchan %d\n", *(address + curr), *(address + curr + 1), curr, next, chan_total);
                curr = next;
        }

        /* Next sample to be read */
        cli_info->location = next;
        return 0;
}

/*
 * WARNING: this function copy buffer from user space...
 */
static int write_linear_dma_buffer(struct module_context *mcon,
                struct dma_channel *chan, u32 req_bytes, void *buffer)
{
        int retval = 0;

        u64 curr = chan->buffer->location;
        u64 end = (curr + req_bytes) % chan->size;

        u8 *address = (u8 *) chan->buffer->address.cpu;
        u64 offset = 0;

        if (end <= curr)
        {
            offset = (u64)chan->size - curr;
            memcpy (address + curr , buffer, offset);
            retval = copy_from_user((void *)(address + curr), buffer, offset);
            if (retval) return -EINVAL;

            buffer = (void *) ((u8 *)buffer + offset);
            curr = 0;
        }

        retval = copy_from_user((void *)(address + curr), buffer, (end - curr));
        if (retval) return -EINVAL;

        chan->buffer->location = end;
        return 0;
}

// TODO dma buffer can be 2^n aligned
static int write_ao_chanel_to_linear_dma_buffer(struct module_context *mcon,
                struct dma_cli_info *cli_info, u32 num_samples, u32 chan_total, void *buffer)
{
        int retval = 0, i;

        u64 curr = cli_info->location;
        u64 next = 0;
        s16 *ptr = (s16 *)buffer;

        /* Kernel virtual address */
        u8 *address = (u8 *) mcon->ao_chan.buffer->address.cpu;

        PDEBUG("%s write %d samples to linear DMA buffer\n",
                        pci_name(mcon->pci_dev), num_samples);

        for (i=0; i< num_samples; i++) {
                retval = copy_from_user( address + curr, &ptr[i], AO_CHAN_MITE_DATA_WIDTH);
                if (retval) return -EFAULT;
                next = (curr + AI_CHAN_MITE_DATA_WIDTH * chan_total) % mcon->ai_chan.buffer->size;
                PVDEBUG("copy from user %x %x curr %llu next %llu totoalchan %d\n", *(address + curr), *(address + curr + 1), curr, next, chan_total);
                curr = next;
        }

        /* Next sample to be read */
        cli_info->location = next;
        return 0;
}

/*
 * WARNING: this function copy data directly into the user space!
 */
static int pxi6259_read_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info *cli_info, struct pxi6259_rw_buffer *buffer)
{
        u32 bytes_in_buffer;
        u32 bytes_read;
        u32 req_bytes = buffer->rw_bytes;
        int retval;

        PDEBUG("%s reading DMA channel... \n", pci_name(mcon->pci_dev));

        if (chan->state == DMA_IDLE || chan->state == DMA_UNKNOWN) {
                printk(KERN_ERR "%s DMA channel in wrong state!\n", pci_name(mcon->pci_dev));
                return -EINVAL;
        }

        if (req_bytes == 0) {
                bytes_read = 0;
                retval = copy_to_user((void *)buffer->rw_bytes_done, (void *)&bytes_read, sizeof(bytes_read));
                if (retval)  printk(KERN_ERR "%s failed to copy to user space!\n", pci_name(mcon->pci_dev));
                return retval;
        }

        bytes_in_buffer = bytes_in_dma_channel(mcon, chan, cli_info);

        /* If bytes transfered exceed the buffer size, DMA buffer overflowed. */
        if (bytes_in_buffer > chan->size) {
                printk(KERN_ERR "%s DMA buffer overflow %d %llu %llu last_dar %u real_dar %llu!\n",
                                pci_name(mcon->pci_dev), bytes_in_buffer, cli_info->read_idx,
                                cli_info->write_idx, cli_info->last_dar, cli_info->real_dar);
                return -EIO;
        }

        if (bytes_in_buffer == 0) return -EAGAIN;

        if (bytes_in_buffer < req_bytes) {
                req_bytes = bytes_in_buffer & ~((u32)0x1);
        }

        //printk("reading %d samples (%d)\n", bytes_in_buffer/2, req_bytes/2);

        retval = read_linear_dma_buffer(mcon, chan, cli_info, req_bytes, (void *)buffer->buffer);
        if (retval) {
                printk(KERN_ERR "%s failed to read DMA buffer!\n", pci_name(mcon->pci_dev));
                return retval;
        }

        /* Check for overflow again. Note that the read index is not updated. Check for an
           overflow while moving the data out of the DMA buffer. */
        bytes_in_buffer = bytes_in_dma_channel(mcon, chan, cli_info);
        if (bytes_in_buffer > chan->size) {
                printk(KERN_ERR "%s DMA buffer overflow while moving data !\n",
                                pci_name(mcon->pci_dev));
                return -EIO;
        }

        /* No overflow during data move - update read index. */
        cli_info->read_idx += req_bytes;

        retval = copy_to_user((void *)buffer->rw_bytes_done, (void *)&req_bytes, sizeof(buffer->rw_bytes_done));
        if (retval) {
                printk(KERN_ERR "%s failed to copy to user space!\n", pci_name(mcon->pci_dev));
        }

        return retval;
}

/*
 * WARNING: this function copy data directly into the user space!
 */
static int pxi6259_write_dma_channel(struct module_context *mcon,
                struct dma_channel *chan, struct dma_cli_info *cli_info, struct pxi6259_rw_buffer *buffer)
{
        u32 bytes_in_buffer, bytes_free;
        u32 *bytes_left;

        if (DMA_IDLE == chan->state || DMA_UNKNOWN == chan->state)
                return -EINVAL;

        /* Get number of bytes available/free in the buffer and validate */
        bytes_in_buffer = bytes_in_dma_channel(mcon, chan, cli_info);
        bytes_free = chan->size - bytes_in_buffer;

        // 1a. if bytes transfered exceed the buffer size, DMA buffer underflow
        //     this occurs when the read index passes the write index

        if (bytes_in_buffer > chan->size) {
                printk(KERN_ERR "%s DMA buffer underflow!", pci_name(mcon->pci_dev));
                return -EIO;
        }

        // 1b. If the user didn't pass a buffer, just return the bytes available
        //if (req_bytes == 0 || user_buffer == NULL) {
        //        *bytes_left = bytes_free;
        //        return 0;
        //}

        // 1c. Check if there's enough space to write user data
        if (buffer->rw_bytes > bytes_free ) {
                buffer->rw_bytes = bytes_free;
        }

        // 2. Move Data to DMA Buffer
        write_linear_dma_buffer(mcon, chan, buffer->rw_bytes, (void *)buffer->buffer);

        // 3. Check for overflow again
        //    Note that the write index is not updated.  Check for an underflow while
        //    moving the data into the DMA buffer.

        bytes_in_buffer = bytes_in_dma_channel(mcon, chan, cli_info);
        bytes_free = chan->size - bytes_in_buffer;

        if (bytes_in_buffer > chan->size) {
                printk(KERN_ERR "%s DMA buffer underflow while moving data!", pci_name(mcon->pci_dev));
                return -EIO;
        }

        // 4. No underflow during data move.  Update write index and recalculate
        //    Number of bytes in the buffer to return to the user.

        cli_info->write_idx += buffer->rw_bytes;

        bytes_in_buffer = bytes_in_dma_channel(mcon, chan, cli_info);
        bytes_free = chan->size - bytes_in_buffer;

        *bytes_left = bytes_free;

        return 0;
}


static struct dma_buffer *allocate_dma_buffer(struct module_context *mcon, size_t size)
{
        struct dma_buffer *dma;
        void       *cpu_addr;
        dma_addr_t  bus_addr;

        if (NULL == mcon)
                return NULL;

        /* Allocate the dma descriptor */

        dma = (struct dma_buffer *) kmalloc (sizeof (*dma), GFP_KERNEL);
        if (dma == NULL) {
                printk(KERN_ERR "%s could not kmalloc space for dma descriptor!\n", pci_name(mcon->pci_dev));
                return NULL;
        }

        PDEBUG("%s allocated dma descriptor.\n", pci_name(mcon->pci_dev));

        cpu_addr = pci_alloc_consistent(mcon->pci_dev, size, &bus_addr);
        if (NULL != cpu_addr) {
                INIT_LIST_HEAD(&dma->link);
                dma->address.bus   = bus_addr;
                dma->address.cpu   = cpu_addr;
                dma->size          = size;

                PDEBUG ("%s allocated dma buffer %lu bytes (descriptor: 0x%p)\n", pci_name(mcon->pci_dev), size, dma);
                PDEBUG ("    bus address 0x%p\n", (void *)bus_addr);
                PDEBUG ("    cpu address 0x%p\n", cpu_addr);
        } else {
                printk(KERN_ERR "%s could not allocate DMA buffer, size %lu.\n", pci_name(mcon->pci_dev), size);
                kfree(dma);
                return NULL;
        }

        return dma;
}

static void free_dma_buffer(struct module_context *mcon, struct dma_buffer *dma)
{
        if (NULL == mcon) {
                PDEBUG("%s attempted to release dma using NULL device\n", pci_name(mcon->pci_dev));
                return;
        }

        if (NULL == dma) {
                PDEBUG("%s attempted to release dma using NULL dma descriptor\n", pci_name(mcon->pci_dev));
                return;
        }

        pci_free_consistent(mcon->pci_dev, dma->size, dma->address.cpu, dma->address.bus);

        PDEBUG ("%s free dma buffer %lu bytes (descriptor: 0x%p)\n",pci_name(mcon->pci_dev), dma->size, dma);
        PDEBUG ("               bus address 0x%p\n", (void *)dma->address.bus);
        PDEBUG ("               cpu address 0x%p\n",         dma->address.cpu);

        kfree(dma);
        PDEBUG ("%s free dma descriptor (%p)\n",pci_name(mcon->pci_dev), dma);
}

static void dma_channel_configure(struct module_context *mcon, struct dma_channel *chan)
{
        u32 memory_config_reg = 0;
        u32 channel_control_reg = 0;
        u32 device_cnfig_reg = 0;
        u32 startAddress = 0;

        channel_control_reg = 1 << 0xe;
        channel_control_reg |= ((chan->direction == DMA_FROM_DEVICE ? 1 : 0) << 0x3);

        memory_config_reg = 0x00E00400;
        memory_config_reg |= (chan->xfer_width << 0x8);

        device_cnfig_reg = 0x00000440;
        device_cnfig_reg |= (chan->xfer_width << 0x8);
        device_cnfig_reg |= ((4 + chan->drq) << 0x10);

        PDEBUG("%s configuring DMA channel ...\n", pci_name(mcon->pci_dev));

        if (chan->mode == DMA_MODE_NORMAL) {
                startAddress = chan->buffer->address.bus;

                PDEBUG("Configuring dma channel addressees %x size %d\n", startAddress, chan->size);

                iowrite32(cpu_to_le32(startAddress), mcon->addresses.spaces[BAR0].space
                                + MemoryAddress_Register + chan->mite_offset);
                iowrite32(chan->size, mcon->addresses.spaces[BAR0].space
                                + TransferCount_Register + chan->mite_offset);

                iowrite32(cpu_to_le32(startAddress), mcon->addresses.spaces[BAR0].space
                                + BaseAddress_Register + chan->mite_offset);
                iowrite32(chan->size, mcon->addresses.spaces[BAR0].space
                                + BaseCount_Register + chan->mite_offset);

                PVDEBUG("write32 %x %x \n",cpu_to_le32(startAddress),
                                MemoryAddress_Register + chan->mite_offset);
                PVDEBUG("write32 %x %x \n", chan->size, TransferCount_Register + chan->mite_offset);

                PVDEBUG("write32 %x %x \n",startAddress,
                                 BaseAddress_Register + chan->mite_offset);
                PVDEBUG("write32 %x %x \n",chan->size,
                                 BaseCount_Register + chan->mite_offset);
        } else if (chan->mode == DMA_MODE_RING) {
                startAddress = chan->buffer->address.bus;

                channel_control_reg |= 0x2;

                iowrite32(cpu_to_le32(startAddress), mcon->addresses.spaces[BAR0].space
                                + MemoryAddress_Register + chan->mite_offset);
                iowrite32(chan->size, mcon->addresses.spaces[BAR0].space
                                + TransferCount_Register + chan->mite_offset);

                iowrite32(cpu_to_le32(startAddress), mcon->addresses.spaces[BAR0].space
                                + BaseAddress_Register + chan->mite_offset);
                iowrite32(chan->size, mcon->addresses.spaces[BAR0].space
                                + BaseCount_Register + chan->mite_offset);

                PVDEBUG("write32 %x %x \n",startAddress,
                                 MemoryAddress_Register + chan->mite_offset);
                PVDEBUG("write32 %x %x \n",chan->size,
                                 TransferCount_Register + chan->mite_offset);

                PVDEBUG("write32 %x %x \n",startAddress,
                                 BaseAddress_Register + chan->mite_offset);
                PVDEBUG("write32 %x %x \n",chan->size,
                                 BaseCount_Register + chan->mite_offset);
        }

       iowrite32(channel_control_reg, mcon->addresses.spaces[BAR0].space
                        + ChannelControl_Register + chan->mite_offset);
       iowrite32(memory_config_reg, mcon->addresses.spaces[BAR0].space
                        + MemoryConfig_Register + chan->mite_offset);
       iowrite32(device_cnfig_reg, mcon->addresses.spaces[BAR0].space
                        + DeviceConfig_Register + chan->mite_offset);

       iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + DeviceAddress_Register + chan->mite_offset);

       PVDEBUG("write32 %x %x \n",channel_control_reg,
                         ChannelControl_Register + chan->mite_offset);
       PVDEBUG("write32 %x %x \n",memory_config_reg,
                         MemoryConfig_Register + chan->mite_offset);
       PVDEBUG("write32 %x %x \n",device_cnfig_reg,
                       DeviceConfig_Register + chan->mite_offset);
       PVDEBUG("write32 %x %x \n",0,
                         DeviceAddress_Register + chan->mite_offset);

       chan->buffer->location = 0;

       chan->state = DMA_CONFIGURED;
}

static int dma_channel_start(struct module_context *mcon, struct dma_channel *chan)
{
        PDEBUG("%s starting DMA channel...\n", pci_name(mcon->pci_dev));
        if (chan->state == DMA_STARTED) {
                dma_channel_stop(mcon, chan);
        }

        if (chan->state == DMA_STOPPED) {
                dma_channel_reset(mcon, chan);
                dma_channel_configure(mcon, chan);
        }

        if (chan->state != DMA_CONFIGURED)
                return -EINVAL;

        smp_mb();
        iowrite32(1, mcon->addresses.spaces[BAR0].space
                        + ChannelOperation_Register + chan->mite_offset);
        mmiowb();

        PVDEBUG("write32 %x %x \n", 1,
                         ChannelOperation_Register + chan->mite_offset);

        chan->state = DMA_STARTED;

        return 0;
}

static int dma_channel_stop(struct module_context *mcon, struct dma_channel *chan)
{
        PDEBUG("%s stopping DMA channel...\n", pci_name(mcon->pci_dev));

        iowrite32(1 << 0x2, mcon->addresses.spaces[BAR0].space
                        + ChannelOperation_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x \n", 1 << 0x2,
                         ChannelOperation_Register + chan->mite_offset);

        chan->state = DMA_STOPPED;

        return 0;
}


static int dma_channel_reset(struct module_context *mcon, struct dma_channel *chan)
{
        if (chan->state == DMA_STARTED)
                dma_channel_stop(mcon, chan);

        PDEBUG ("%s reseting DMA channel... \n", pci_name(mcon->pci_dev));

        // TODO add 1<<0x2
        iowrite32(1 << 0x1f, mcon->addresses.spaces[BAR0].space
                        + ChannelOperation_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x\n", 1 << 0x1f,
                         ChannelOperation_Register + chan->mite_offset);

        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + ChannelOperation_Register + chan->mite_offset);
        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + ChannelControl_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x\n", 0,
                         ChannelOperation_Register + chan->mite_offset);
        PVDEBUG("write32 %x %x\n", 0,
                         ChannelControl_Register + chan->mite_offset);

        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + BaseCount_Register + chan->mite_offset);
        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + TransferCount_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x\n", 0,
                         BaseCount_Register + chan->mite_offset);
        PVDEBUG("write32 %x %x\n", 0,
                        TransferCount_Register + chan->mite_offset);

        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + MemoryConfig_Register + chan->mite_offset);
        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + DeviceConfig_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x\n", 0,
                        MemoryConfig_Register + chan->mite_offset);
        PVDEBUG("write32 %x %x\n", 0,
                        DeviceConfig_Register + chan->mite_offset);

        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + BaseAddress_Register + chan->mite_offset);
        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + MemoryAddress_Register + chan->mite_offset);
        iowrite32(0, mcon->addresses.spaces[BAR0].space
                        + DeviceAddress_Register + chan->mite_offset);

        PVDEBUG("write32 %x %x\n", 0,
                         BaseAddress_Register + chan->mite_offset);
        PVDEBUG("write32 %x %x\n", 0,
                         MemoryAddress_Register + chan->mite_offset);
        PVDEBUG("write32 %x %x\n", 0,
                         DeviceAddress_Register + chan->mite_offset);

        chan->state = DMA_IDLE;

        return 0;
}


static int dma_channel_config(struct module_context *mcon, struct dma_channel *chan,
                u32 request_source, enum dma_mode mode, enum dma_data_direction direction,
                u32 size, u8 xfer_width, u32 mite_offset)
{
        PDEBUG("dma_channel_config\n");

        if (chan->state != DMA_IDLE) {
                dma_channel_reset(mcon, chan);
        }

        if (chan->state != DMA_IDLE) {
                return -EINVAL;
        }

        /* 1. Store DMA Channel configuration */
        chan->mode = mode;
        chan->direction = direction;
        chan->drq = request_source;
        chan->xfer_width = xfer_width;
        chan->size = size;

        //TODO remove this
        chan->mite_offset = mite_offset;

        /* Free DMA buffer */
        if (chan->buffer != NULL) {
                free_dma_buffer(mcon, chan->buffer);
        }

        /* Allocate DMA buffer */
        if (chan->mode == DMA_MODE_NORMAL || chan->mode == DMA_MODE_RING) {
                chan->buffer = allocate_dma_buffer(mcon, size);
        }

        if (chan->buffer == NULL)
                return -ENOMEM;

        /* Configure DMA Channel */
        dma_channel_configure(mcon, chan);

        chan->state = DMA_CONFIGURED;

        return 0;
}


/* AI segment function definitions */

static void ai_clear_fifo(struct module_context *mcon)
{
        PDEBUG("%s clearing AI FIFO\n", pci_name(mcon->pci_dev));

        write_register(mcon, AI_FIFO_Clear_Register, 1);
}


static void ai_adc_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting AI ADC\n", pci_name(mcon->pci_dev));

        write_register(mcon, Static_AI_Control_0_Register, 1);

        write_register(mcon, AI_Command_1_Register, AI_CONVERT_Pulse);
        write_register(mcon, AI_Command_1_Register, AI_CONVERT_Pulse);
        write_register(mcon, AI_Command_1_Register, AI_CONVERT_Pulse);
}

static void ai_personalize(struct module_context *mcon,
                ai_convert_output_select_t aiConOutputSelect,
                ai_sip_output_select_t aiScanInProgOutputSelect, ai_convert_pulse_width_t aiConPulseWidth)
{
        PDEBUG("%s personalizing AI segment\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AI_Output_Control_Register,
                        AI_CONVERT_Output_Select_Mask,
                        AI_CONVERT_Output_Select(aiConOutputSelect));
        set_register_bits(mcon, AI_Output_Control_Register,
                        AI_SCAN_IN_PROG_Output_Select_Mask,
                        AI_SCAN_IN_PROG_Output_Select(aiScanInProgOutputSelect));
        flush_register(mcon, AI_Output_Control_Register);

        set_register_bits_on_off(mcon, AI_Personal_Register, AI_CONVERT_Pulse_Width, aiConPulseWidth);
        flush_register(mcon, AI_Personal_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}

static void ai_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting AI segment\n", pci_name(mcon->pci_dev));

        write_register(mcon, Joint_Reset_Register, AI_Reset);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, Interrupt_A_Enable_Register,
                AI_SC_TC_Interrupt_Enable | AI_START1_Interrupt_Enable
                | AI_START2_Interrupt_Enable | AI_START_Interrupt_Enable
                | AI_STOP_Interrupt_Enable | AI_Error_Interrupt_Enable
                | AI_FIFO_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register,
                AI_SC_TC_Error_Confirm | AI_SC_TC_Interrupt_Ack
                | AI_START1_Interrupt_Ack | AI_START2_Interrupt_Ack
                | AI_START_Interrupt_Ack | AI_STOP_Interrupt_Ack
                | AI_Error_Interrupt_Ack, 1);
        flush_register(mcon, Interrupt_A_Ack_Register);

        //write_register(mcon, Interrupt_Control_Register, Interrupt_A_Enable | Interrupt_B_Enable);

        set_register_bits_on_off(mcon, AI_Mode_1_Register, AI_Start_Stop, 1);
        flush_register(mcon, AI_Mode_1_Register);

        write_register(mcon, AI_Mode_2_Register, 0);
        write_register(mcon, AI_Mode_3_Register, 0);
        write_register(mcon, AI_Output_Control_Register, 0);
        write_register(mcon, AI_Personal_Register, 0);
        write_register(mcon, AI_START_STOP_Select_Register, 0);
        write_register(mcon, AI_Trigger_Select_Register, 0);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        ai_personalize(mcon, mcon->ai_conf.aiConOutputSelect,
                mcon->ai_conf.aiScanInProgOutputSelect, mcon->ai_conf.aiConPulseWidth);
        ai_clear_fifo(mcon);
        ai_adc_reset(mcon);

        mcon->ai_state = AI_STATE_UNKNOWN;
}

static void ai_disarm(struct module_context *mcon)
{
        PDEBUG("%s disarming AI segment\n", pci_name(mcon->pci_dev));

        //write_register(mcon, Joint_Reset_Register, AI_Reset);

        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_Disarm, 1);
        flush_register(mcon, AI_Command_1_Register);
}

static void ai_clear_configuration_memory(struct module_context *mcon)
{
        PDEBUG("%s clearing AI configuration memory\n", pci_name(mcon->pci_dev));

        write_register(mcon, Configuration_Memory_Clear_Register, 1);

        /* Use configuration FIFO */
        set_register_bits_on_off(mcon, AI_Config_FIFO_Bypass_Register, AI_Bypass_Config_FIFO, 0);
        flush_register(mcon, AI_Config_FIFO_Bypass_Register);
}

static void ai_environmentalize(struct module_context *mcon,
                ai_external_mux_present_t aiExtMUXpresent, ai_extmux_output_select_t aiExtMUXCLKoutputSelect)
{
        PDEBUG("%s environmentalizing AI segment\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AI_Mode_2_Register,
                        AI_External_MUX_Present, aiExtMUXpresent);
        flush_register(mcon, AI_Mode_2_Register);

        set_register_bits(mcon, AI_Output_Control_Register,
                        AI_EXTMUX_CLK_Output_Select_Mask,
                        AI_EXTMUX_CLK_Output_Select(aiExtMUXCLKoutputSelect));
        flush_register(mcon, AI_Output_Control_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return;
}

static void ai_set_fifo_request_mode(struct module_context *mcon)
{
        PDEBUG("%s setting AI FIFO request mode\n", pci_name(mcon->pci_dev));

        set_register_bits(mcon, AI_Mode_3_Register,
                        AI_FIFO_Mode_Mask, AI_FIFO_Mode_NE);
}


static void ai_set_hardware_gating(struct module_context *mcon,
                ai_external_gate_select_t aiExtGateSelect)
{
        PDEBUG("%s setting AI hardware gating\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AI_Mode_3_Register,
                        AI_External_Gate_Select_Mask,
                        AI_External_Gate_Select(aiExtGateSelect));
        flush_register(mcon, AI_Mode_3_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}

static void ai_set_trigger(struct module_context *mcon,
                ai_start_select_t start_source,
                ai_start_polarity_t start_polarity,
                ai_reference_select_t ref_source,
                ai_reference_polarity_t ref_polarity)
{
        PDEBUG("%s setting AI trigger\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AI_Mode_1_Register,
                        AI_Trigger_Once, AI_Trigger_Once);
        flush_register(mcon, AI_Mode_1_Register);

        /* START1 trigger signal */
        set_register_bits(mcon, AI_Trigger_Select_Register,
                        AI_START1_Select_Mask, AI_START1_Select(start_source));
        set_register_bits(mcon, AI_Trigger_Select_Register,
                        AI_START1_Polarity, start_polarity ? AI_START1_Polarity : 0);
        set_register_bits(mcon, AI_Trigger_Select_Register,
                        AI_START1_Edge, AI_START1_Edge);
        set_register_bits(mcon, AI_Trigger_Select_Register,
                        AI_START1_Sync, AI_START1_Sync);
        flush_register(mcon, AI_Trigger_Select_Register);

        /* Reference trigger signal */
        if( ref_source == AI_REFERENCE_SELECT_PULSE) {
                set_register_bits(mcon, AI_Mode_2_Register, AI_Pre_Trigger, 0);
        } else {
                set_register_bits(mcon, AI_Mode_2_Register, AI_Pre_Trigger, AI_Pre_Trigger);
        }
        flush_register(mcon, AI_Mode_2_Register);

        /* Start2 trigger signal */
        set_register_bits(mcon, AI_Trigger_Select_Register,
                        AI_START2_Select_Mask,
                        AI_START2_Select(ref_source));
        set_register_bits_on_off(mcon, AI_Trigger_Select_Register,
                        AI_START2_Polarity, ref_polarity);
        set_register_bits_on_off(mcon, AI_Trigger_Select_Register,
                        AI_START2_Edge, 1);
        set_register_bits_on_off(mcon, AI_Trigger_Select_Register,
                        AI_START2_Sync, 1);
        flush_register(mcon, AI_Trigger_Select_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}

static void ai_sample_stop(struct module_context *mcon, ai_stop_select_t stopSrc,
                ai_stop_polarity_t stopPolarity, uint8_t multiChannel, uint16_t num_of_chans_per_scan)
{
        PDEBUG("%s AI sample stop\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        /* Internal stop source */
        if (stopSrc == AI_STOP_SELECT_DIV_TC_OR_PULSE) {
                if (multiChannel) {
                        /*DIV counter is used as the STOP source*/
                        set_register_bits(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Select_Mask,
                                        AI_STOP_Select(stopSrc));
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Polarity, stopPolarity);
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Sync, 1);
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Edge, 0);
                        flush_register(mcon, AI_START_STOP_Select_Register);
                        write_register(mcon, AI_DIV_Load_A_Register, num_of_chans_per_scan-1);
                } else {
                        set_register_bits(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Select_Mask,
                                        AI_STOP_Select(stopSrc));
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Polarity, stopPolarity);
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Sync, 1);
                        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                        AI_STOP_Edge, 0);
                        flush_register(mcon, AI_START_STOP_Select_Register);
                }
        } else {
                set_register_bits(mcon, AI_START_STOP_Select_Register,
                                AI_STOP_Select_Mask,
                                AI_STOP_Select(stopSrc));
                set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                AI_STOP_Polarity, stopPolarity);
                set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                AI_STOP_Sync, 0);
                set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                AI_STOP_Edge, 0);
                flush_register(mcon, AI_START_STOP_Select_Register);
        }

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}


static void ai_number_of_samples(struct module_context *mcon, uint32_t post_trigger_samples,
                          uint32_t pre_trigger_samples, uint8_t continuous)
{
        PDEBUG("%s AI number of samples\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AI_Mode_1_Register,
                        AI_Continuous, continuous);
        flush_register(mcon, AI_Mode_1_Register);

        write_register(mcon, AI_SC_Load_A_Register, post_trigger_samples-1);

        if (!pre_trigger_samples)
        {
                set_register_bits_on_off(mcon, AI_SC_Load_B_Register, 0xffffffff, 0);

                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SC_Reload_Mode,
                                AI_SC_RELOAD_MODE_NO_CHANGE);
                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SC_Initial_Load_Source,
                                AI_SC_INITIAL_LOAD_SOURCE_LOAD_A);
        } else {
                set_register_bits(mcon, AI_SC_Load_B_Register, 0xffffffff, (pre_trigger_samples-1));

                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SC_Reload_Mode, AI_SC_RELOAD_MODE_SWITCH);
                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SC_Initial_Load_Source,
                                AI_SC_INITIAL_LOAD_SOURCE_LOAD_B);
        }
        flush_register(mcon, AI_Mode_2_Register);
        flush_register(mcon, AI_SC_Load_B_Register);

        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SC_Load, 1);
        flush_register(mcon, AI_Command_1_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}


static void ai_sample_start(struct module_context *mcon, uint32_t period_divisor,
                     uint32_t delay_divisor, ai_sample_select_t source,
                     ai_sample_polarity_t polarity, uint8_t is_convert_clk_ext)
{
        PDEBUG("%s AI sample start\n",pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AI_START_STOP_Select_Register,
                        AI_START_Select_Mask, AI_START_Select(source));
        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                        AI_START_Polarity, polarity);
        if (is_convert_clk_ext) {
                set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                AI_START_Sync, 0);
        } else {
                set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                                AI_START_Sync, 1);
        }
        set_register_bits_on_off(mcon, AI_START_STOP_Select_Register,
                        AI_START_Edge, 1);
        flush_register(mcon, AI_START_STOP_Select_Register);


        if (source == AI_SAMPLE_SELECT_SI_TC)
        {
                set_register_bits_on_off(mcon, AI_Mode_1_Register,
                                AI_SI_Source_Polarity,
                                AI_SI_SOURCE_POLARITY_RISING_EDGE);
                set_register_bits(mcon, AI_Mode_1_Register,
                                AI_SI_Source_select_Mask,
                                AI_SI_Source_select(AI_SI_SELECT_INTIMEBASE1));
                flush_register(mcon, AI_Mode_1_Register);

                write_register(mcon, AI_SI_Load_B_Register, (delay_divisor-1));
                write_register(mcon, AI_SI_Load_A_Register, (period_divisor-1));
                /* Load register B */
                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SI_Initial_Load_Source,
                                AI_SI_INITIAL_LOAD_SOURCE_LOAD_B);
                flush_register(mcon, AI_Mode_2_Register);

                set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SI_Load, 1);
                flush_register(mcon, AI_Command_1_Register);

                /* Alternate_First_Period_Every_SCTC */
                set_register_bits(mcon, AI_Mode_2_Register,
                                AI_SI_Reload_Mode_Mask,
                                AI_SI_Reload_Mode(AI_SI_RELOAD_MODE_ALTERNATE_FIRST_PERIOD_EVERY_SCTC));
                /* Load register A */
                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SI_Initial_Load_Source,
                                AI_SI_INITIAL_LOAD_SOURCE_LOAD_A);
                flush_register(mcon, AI_Mode_2_Register);
        }

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}


static void ai_convert(struct module_context *mcon, uint32_t period_divisor,
                uint32_t delay_divisor, ai_convert_select_t source,
                ai_convert_polarity_t polarity, uint8_t is_sample_clk_ext)
{
        PDEBUG("%s AI convert\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register,
                        AI_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AI_Mode_1_Register,
                        AI_CONVERT_Source_Select_Mask,
                        AI_CONVERT_Source_Select(source));

        if (source == AI_CONVERT_SELECT_SI2TC) {
                set_register_bits_on_off(mcon, AI_Mode_1_Register,
                                AI_CONVERT_Source_Polarity, 0);  /* Falling edge */
                flush_register(mcon, AI_Mode_1_Register);

                set_register_bits_on_off(mcon, AI_Mode_2_Register, AI_SC_Gate_Enable, 0);
                set_register_bits_on_off(mcon, AI_Mode_2_Register, AI_Start_Stop_Gate_Enable, 0);
                set_register_bits_on_off(mcon, AI_Mode_2_Register, AI_SI2_Reload_Mode, 1); /* Alternate first period on every STOP */
                flush_register(mcon, AI_Mode_2_Register);

                if (is_sample_clk_ext) {
                        set_register_bits_on_off(mcon, AI_Mode_3_Register,
                                        AI_SI2_Source_Select, 1);
                } else {
                        set_register_bits_on_off(mcon, AI_Mode_3_Register,
                                        AI_SI2_Source_Select, 0);
                }

                flush_register(mcon, AI_Mode_3_Register);

                write_register(mcon, AI_SI2_Load_A_Register, (delay_divisor-1));
                write_register(mcon, AI_SI2_Load_B_Register, (period_divisor-1));

                set_register_bits_on_off(mcon, AI_Mode_2_Register, AI_SI2_Initial_Load_Source, 0);
                flush_register(mcon, AI_Mode_2_Register);

                set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SI2_Load, 1);
                flush_register(mcon, AI_Command_1_Register);

                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SI2_Initial_Load_Source, 1);
                flush_register(mcon, AI_Mode_2_Register);
        } else {
                set_register_bits_on_off(mcon, AI_Mode_1_Register,
                                AI_CONVERT_Source_Polarity, polarity);
                flush_register(mcon, AI_Mode_1_Register);

                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_SC_Gate_Enable, 1);
                set_register_bits_on_off(mcon, AI_Mode_2_Register,
                                AI_Start_Stop_Gate_Enable, 1);
                flush_register(mcon, AI_Mode_2_Register);
        }

        set_register_bits_on_off(mcon, Joint_Reset_Register, AI_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);
}

static void ai_arm(struct module_context *mcon, uint8_t armSC, uint8_t armSI,
                uint8_t armSI2, uint8_t armDIV)
{
        PDEBUG("%s AI arm.\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SC_Arm, armSC);
        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SI_Arm, armSI);
        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_SI2_Arm, armSI2);
        set_register_bits_on_off(mcon, AI_Command_1_Register, AI_DIV_Arm, armDIV);

        flush_register(mcon, AI_Command_1_Register);
}

static void ai_start(struct module_context *mcon)
{
        PDEBUG("%s AI start\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AI_Command_2_Register, AI_START1_Pulse, 1);
        flush_register(mcon, AI_Command_2_Register);
}

static void ai_start_on_demand(struct module_context *mcon)
{
        PVDEBUG("%s AI start on demand\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AI_Command_2_Register, AI_START_Pulse, 1);
        flush_register(mcon, AI_Command_2_Register);
}

static void ai_configure_channel(struct module_context *mcon, uint16_t channel,
                           uint16_t gain, ai_polarity_t polarity,
                           ai_channel_type_t channel_type, uint8_t dither, uint8_t last_channel)
{
        PDEBUG("%s AI configure channel %d\n", pci_name(mcon->pci_dev), channel);

        set_register_bits_on_off(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Polarity, polarity);
        set_register_bits(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Gain_Mask,
                        AI_Config_Gain(gain));
        set_register_bits(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Channel_Mask,
                        AI_Config_Channel(channel));
        set_register_bits(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Bank_Mask,
                        AI_Config_Bank((channel & 0x30) >> 4));
        set_register_bits(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Channel_Type_Mask,
                        AI_Config_Channel_Type(channel_type));
        set_register_bits_on_off(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Dither, dither ? 1 : 0);
        set_register_bits_on_off(mcon, AI_Config_FIFO_Data_Register,
                        AI_Config_Last_Channel, last_channel ? 1 : 0);
        flush_register(mcon, AI_Config_FIFO_Data_Register);

        if (last_channel) {
                set_register_bits_on_off(mcon, AI_Command_1_Register,
                                AI_LOCALMUX_CLK_Pulse, 1);
                flush_register(mcon, AI_Command_1_Register);
        }
}

/* Common definitions */

static void configure_timebase(struct module_context *mcon)
{
        PDEBUG("%s configuring timebase\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Clock_and_FOUT_Register,
                        Slow_Internal_Timebase, 1);
        flush_register(mcon, Clock_and_FOUT_Register);
}

static void pll_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting PLL\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Clock_and_Fout2_Register, TB1_Select, 0);
        set_register_bits_on_off(mcon, Clock_and_Fout2_Register, TB3_Select, 0);
        flush_register(mcon, Clock_and_Fout2_Register);

        set_register_bits_on_off(mcon, PLL_Control_Register, PLL_Enable, 0);
        flush_register(mcon, PLL_Control_Register);
}

static void analog_trigger_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting analog trigger\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Analog_Trigger_Etc_Register,
                        Analog_Trigger_Reset, 1);
        set_register_bits(mcon, Analog_Trigger_Etc_Register,
                        Analog_Trigger_Mode_Mask,
                        Analog_Trigger_Mode(kAnalog_Trigger_ModeLow_Window));
        flush_register(mcon, Analog_Trigger_Etc_Register);

        set_register_bits(mcon, Analog_Trigger_Control_Register,
                        Analog_Trigger_Select_Mask,
                        Analog_Trigger_Select(kAnalog_Trigger_SelectGround));
        flush_register(mcon, Analog_Trigger_Control_Register);

        write_register(mcon, Gen_PWM_0_Register, 0);
        write_register(mcon, Gen_PWM_1_Register, 0);

        set_register_bits_on_off(mcon, Analog_Trigger_Etc_Register,
                        Analog_Trigger_Enable, 0);
        flush_register(mcon, Analog_Trigger_Etc_Register);
}

/* Reset AI segment */
static int pxi6259_reset_ai(struct module_context *mcon){
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s AI segment reset \n", pci_name(mcon->pci_dev));

        /* Check if AI segment is being used */
        if (!list_empty(&mcon->aichan_list)) {
                printk(KERN_INFO "%s Channels are being used by 1 or more clients, cannot load new configuration\n",
                                pci_name(mcon->pci_dev));
                return -EBUSY;
        }

        analog_trigger_reset(mcon);

        minor++;
        ai_reset(mcon);
        rc = pxi6259_update_ai_devices(mcon, MKDEV(major, minor));
        return rc;
}

/* Reset AO segment */
static int pxi6259_reset_ao(struct module_context *mcon){
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s AO segment reset \n", pci_name(mcon->pci_dev));

        /* Check if AI segment is being used */
        if (!list_empty(&mcon->aochan_list)) {
            printk(KERN_INFO "%s Channels are being used by 1 or more clients, cannot load new configuration\n",
                pci_name(mcon->pci_dev));
            return -EBUSY;
        }

        minor += 1 + (PXI6259_MAX_AI_CHANNEL + 1 + 1 );
        ao_reset(mcon);
        rc = pxi6259_update_ao_devices(mcon,MKDEV(major, minor));
        return rc;

}

/* Reset DIO segment */
static int pxi6259_reset_dio(struct module_context *mcon){
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s DIO segment reset \n", pci_name(mcon->pci_dev));

        /* Update segment devices */

        minor += 1 + (PXI6259_MAX_AI_CHANNEL + 1 + 1) +
                     (PXI6259_MAX_AO_CHANNEL + 1 + 1);

        mcon->dio_state = DIO_STATE_UNKNOWN;
        rc = pxi6259_update_dio_devices(mcon, MKDEV(major, minor));
        return rc;
}

/* Reset GPC segment */
static int pxi6259_reset_gpc(struct module_context *mcon){
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s GPC segment reset \n", pci_name(mcon->pci_dev));

        /* Update segment devices */

        minor += 1 + (PXI6259_MAX_AI_CHANNEL + 1 + 1)
                   + (PXI6259_MAX_AO_CHANNEL + 1 + 1)
                   + (PXI6259_MAX_DIO_PORT + 1 + 1);

        gpc0_reset(mcon);
        gpc1_reset(mcon);
        rc = pxi6259_update_gpc_devices(mcon, MKDEV(major, minor));
        return rc;
}




/* Reset the device */
static int pxi6259_reset(struct module_context *mcon)
{
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s global reset \n", pci_name(mcon->pci_dev));

        /* Reset pll and timebase  */

        configure_timebase(mcon);
        pll_reset(mcon);

        /* Reset segment devices */

        rc = pxi6259_reset_ai(mcon);
        if (rc) return rc;

        rc=pxi6259_reset_ao(mcon);
        if (rc) return rc;


        rc=pxi6259_reset_dio(mcon);
        if (rc) return rc;

        rc=pxi6259_reset_gpc(mcon);
        if (rc) return rc;

        return 0;
}


static int pxi6259_load_ai_conf(struct module_context *mcon, struct pxi6259_ai_conf *ai_conf)
{
        uint16_t last_chan_idx = 0;
        uint16_t chan_num = 0;
        int i, retval;

        PDEBUG("%s loading AI configuration...\n", pci_name(mcon->pci_dev));

        /* Check if AI segment is being used */
        if (!list_empty(&mcon->aichan_list)) {
                printk(KERN_INFO "%s AI channels are being used by 1 or more clients, cannot load new configuration\n",
                                pci_name(mcon->pci_dev));
                return -EBUSY;
        }

        /* Check if segment is started */
        if (mcon->ai_state != AI_STATE_UNKNOWN) {
                printk(KERN_INFO "%s AI segment is started, releasing it\n",
                                pci_name(mcon->pci_dev));
                pxi6259_release_ai(mcon);
        }

        mcon->ai_conf = *ai_conf;

        /* Apply the configuration */

        /* Get number of channels */
        for (i=0; i<=PXI6259_MAX_AI_CHANNEL; i++) {
                if (ai_conf->channel_mask & ((uint64_t)1<<i)) {
                        last_chan_idx = i;
                        chan_num++;
                }
        }

        /* reset AI segment */
        ai_reset(mcon);

        /* Start AI task */
        ai_disarm(mcon);
        ai_clear_configuration_memory(mcon);

        /* fill configuration FIFO
           Note: It is not necessary for the channel numbers to be ordered */
        for (i=0; i <= last_chan_idx; i++) {
                if (ai_conf->channel_mask & ((uint64_t)1<<i)) {
                        ai_configure_channel(mcon,
                                ai_conf->channel[i].terminal, /* physical channel number */
                                ai_conf->channel[i].gain,     /* gain */
                                ai_conf->channel[i].polarity, /* polarity */
                                ai_conf->channel[i].channelType,
                                ai_conf->channel[i].dither,   /* dither */
                                (last_chan_idx==i)?1:0); /* last channel? */
                }
        }

        ai_set_fifo_request_mode(mcon);
        ai_environmentalize(mcon, ai_conf->aiExtMUXpresent, ai_conf->aiExtMUXCLKoutputSelect);

        ai_set_hardware_gating(mcon, ai_conf->aiExtGateSelect);

        ai_set_trigger(mcon,
                ai_conf->aiStartSource,    /* start source (START_1 select pulse) */
                ai_conf->aiStartPolarity,  /* start polarity (START_1 rising edge) */
                ai_conf->aiRefSource,      /* ref source (START_2 select pulse) */
                ai_conf->aiRefPolarity);   /* ref polarity (START_2 rising edge) */

        ai_sample_stop(mcon,
                        ai_conf->aiStopSource,
                        ai_conf->aiStopPolarity,
                        (chan_num>1)?1:0, chan_num);      /* multi channel? */
       // if (ai_conf->aiStopSource == AI_STOP_SELECT_DIV_TC_OR_PULSE && chan_num <= 1)
       //         ai_conf->aiArmDIV = 0;

        ai_number_of_samples(mcon,
                ai_conf->aiNoOfPostTrigSamples,    /* posttrigger samples */
                ai_conf->aiNoOfPreTrigSamples,     /* pretrigger samples */
                ai_conf->aiContinuous);          /* continuous? */

        ai_sample_start(mcon,
                        ai_conf->aiSamplePeriodDivisor,    /* period divisor */
                        ai_conf->aiSampleDelayDivisor,     /* delay divisor */
                        ai_conf->aiSampleClkSource,
                        ai_conf->aiSampleClkPolarity,
                        ai_conf->aiConvertClkSource != AI_CONVERT_SELECT_SI2TC);

        ai_convert(mcon,
                        ai_conf->aiConvertPeriodDivisor,                   // convert period divisor
                        ai_conf->aiConvertDelayDivisor,                    // convert delay divisor
                        ai_conf->aiConvertClkSource,                       // source
                        ai_conf->aiConvertClkPolarity,                    // polarity
                        ai_conf->aiSampleClkSource != AI_SAMPLE_SELECT_PULSE
                        && ai_conf->aiSampleClkSource != AI_SAMPLE_SELECT_SI_TC); // external sample clock?
                                                                        // TODO check clocks

        /* If the PFI signals are used by the AI segment clear them */
        if (ai_conf->aiConvertClkSource > AI_CONVERT_SELECT_PFI0 &&
                        ai_conf->aiConvertClkSource < AI_CONVERT_SELECT_PFI9) {
                retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, ai_conf->aiConvertClkSource -
                                AI_CONVERT_SELECT_PFI0 + PXI6259_PFI0, 0);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_REFERENCE_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }
        if (ai_conf->aiConvertClkSource > AI_CONVERT_SELECT_PFI10 &&
                        ai_conf->aiConvertClkSource < AI_CONVERT_SELECT_PFI15) {
                retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, ai_conf->aiConvertClkSource -
                                AI_CONVERT_SELECT_PFI10 + PXI6259_PFI10, 0);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_REFERENCE_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }
        if (ai_conf->aiSampleClkSource >= AI_SAMPLE_SELECT_PFI0 && ai_conf->aiSampleClkSource <= AI_SAMPLE_SELECT_PFI9) {
                retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, ai_conf->aiSampleClkSource -
                                AI_SAMPLE_SELECT_PFI0 + PXI6259_PFI0, 0);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_REFERENCE_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }
        if (ai_conf->aiSampleClkSource >= AI_SAMPLE_SELECT_PFI10 && ai_conf->aiSampleClkSource <= AI_SAMPLE_SELECT_PFI15) {
                retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, ai_conf->aiSampleClkSource -
                                AI_SAMPLE_SELECT_PFI10 + PXI6259_PFI10, 0);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_REFERENCE_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }

        /* Export signals */
        if (ai_conf->aiExportSampleClockSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_AI_SAMPLE_CLOCK,
                                ai_conf->aiExportSampleClockSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_SAMPLE_CLOCK signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }

        if (ai_conf->aiExportConvertClockSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_AI_CONVERT_CLOCK,
                                ai_conf->aiExportConvertClockSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_CONVERT_CLOCK signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }

        if (ai_conf->aiExportStartTrigSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_AI_START_TRIGGER,
                                ai_conf->aiExportStartTrigSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_START_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }

        if (ai_conf->aiExportReferenceTrigSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_AI_REFERENCE_TRIGGER,
                                ai_conf->aiExportReferenceTrigSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_AI_REFERENCE_TRIGGER signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        goto error;
                }
        }

        ai_clear_fifo(mcon);

        mcon->ai_state = AI_STATE_CONFIGURED;

        retval = pxi6259_update_ai_devices(mcon, MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1));
        if (retval) {
                printk(KERN_ERR "%s update ai devices failed %d %d %d\n",
                                pci_name(mcon->pci_dev), retval, device_major, mcon->index * DEVICE_MINOR_STEP + 1);
                goto error;
        }

        PDEBUG("%s loading AI configuration done.\n", pci_name(mcon->pci_dev));

        return 0;

  error:
        mcon->ai_state = AI_STATE_UNKNOWN;

        return retval;
}

static int pxi6259_start_ai(struct module_context *mcon)
{
        struct pxi6259_ai_conf *ai_conf = &mcon->ai_conf;
        struct aichan_listitem *lptr;
        int retval;

        PDEBUG("%s starting AI segment ...\n", pci_name(mcon->pci_dev));

        /* segment can be started multiple times */
        if (mcon->ai_state == AI_STATE_UNKNOWN) {
                printk(KERN_ERR "%s AI segment is in unknown state!\n",
                                pci_name(mcon->pci_dev));
                return -EINVAL;
        }

        ai_clear_fifo(mcon);

        /* Reset read indexes */
        list_for_each_entry(lptr, &mcon->aichan_list, list) {
                memset(&lptr->cli.ai_dma_info, 0, sizeof(lptr->cli.ai_dma_info));
                lptr->cli.ai_dma_info.read_idx = lptr->dma_idx * AI_CHAN_MITE_DATA_WIDTH;
                lptr->cli.ai_dma_info.location = lptr->dma_idx * AI_CHAN_MITE_DATA_WIDTH;
                lptr->cli.ai_dma_info.wait_write_idx = 1;
        }

        /* Enable AI DMA channel */
        set_register_bits(mcon, AI_AO_Select_Register, 0x1, 1);
        flush_register(mcon, AI_AO_Select_Register);

        retval = dma_channel_start(mcon, &mcon->ai_chan);
        if (retval) {
                printk(KERN_ERR "%s start AI DMA channel failed %d!\n",
                                pci_name(mcon->pci_dev), retval);
                return -EIO;
        }

        ai_arm(mcon, ai_conf->aiArmSC, ai_conf->aiArmSI, ai_conf->aiArmSI2, ai_conf->aiArmDIV);
        ai_start(mcon);

        mcon->ai_state = AI_STATE_STARTED;
        PDEBUG("%s starting AI segment done.\n", pci_name(mcon->pci_dev));

        return 0;
}

static int pxi6259_ai_sample_on_demand(struct module_context *mcon, uint32_t num_puls)
{
        int i, rep;
        u16 scan_status;

        PDEBUG("%s generating %d AI on demand sample pulses ...\n", pci_name(mcon->pci_dev), num_puls);

        /* segment can be started multiple times */
        if (mcon->ai_state == AI_STATE_UNKNOWN) {
                printk(KERN_ERR "%s AI segment is in unknown state!\n",
                                pci_name(mcon->pci_dev));
                return -EIO;
        }

        if (mcon->ai_conf.aiSampleClkSource != AI_SAMPLE_SELECT_PULSE) {
                PDEBUG("%s AI segment is not configured for on demand mode!\n", pci_name(mcon->pci_dev));
                return -EINVAL;
        }


        for (i=0; i<num_puls; i++) {
                ai_start_on_demand(mcon);
                rep = 0;
                do {
                        read_register(mcon, Joint_Status_2_Register, (void *)&scan_status);
                        rep++;
                } while (scan_status & (1<<7) && rep < MAX_AI_SCAN_STATUS_REP);

                if (rep == MAX_AI_SCAN_STATUS_REP)
                        return -EIO;
        }

        PDEBUG("%s generating AI on demand sample pulses done.\n", pci_name(mcon->pci_dev));

        return 0;
}

static int pxi6259_stop_ai(struct module_context *mcon)
{
        if (mcon->ai_state == AI_STATE_UNKNOWN) {
                printk(KERN_ERR "%s AI segment is in unknown state!\n",
                                pci_name(mcon->pci_dev));
                return -EIO;
        }

        dma_channel_reset(mcon, &mcon->ai_chan);
        dma_channel_stop(mcon, &mcon->ai_chan);

        set_register_bits(mcon, AI_AO_Select_Register, 0x1, 0);
        flush_register(mcon, AI_AO_Select_Register);

        ai_disarm(mcon);

        mcon->ai_state = AI_STATE_CONFIGURED;

        return 0;
}

static int pxi6259_release_ai(struct module_context *mcon)
{
        int rc;

        ai_disarm(mcon);

        /* Reset some registers */
        set_register_bits_on_off(mcon, AI_Mode_1_Register, 0xFFFFFFFF, 0);
        set_register_bits_on_off(mcon, AI_Config_FIFO_Data_Register, 0xFFFFFFFF, 0);

        mcon->ai_state = AI_STATE_UNKNOWN;

        rc = pxi6259_update_ai_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1));
        if (rc) return rc;

        PDEBUG("%s AI segment task released\n", pci_name(mcon->pci_dev));

        return 0;
}

static int pxi6259_read_ai_fifo(struct module_context *mcon, struct pxi6259_rw_buffer *buffer)
{
        u32 i=0;
        u16 status;
        s32 item, scans_to_read = buffer->rw_bytes/2;
        u16 val;
        s16 *ptr = (s16 *)buffer->buffer;
        int retval = 0;

        if (mcon->ai_state != AI_STATE_STARTED) {
              //  printk(KERN_ERR "%s AI segment task is not started, cannot acquire samples (state is %d)!\n",
              //                  pci_name(mcon->pci_dev), mcon->ai_state);
                return -EIO;
        }

        i = 0;
        while (i < scans_to_read) {
                read_register(mcon, AI_Status_1_Register, &status);

                if(!(status & (1<<12))) {
                        read_register(mcon, AI_FIFO_Data_Register, &item);
                        val = (int16_t)item;
                        retval = copy_to_user((void *)&ptr[i], &val, sizeof(val));
                        if (retval) return retval;
                        i++;
                } else {
                        break;
                }
        }

        i*=2;

        retval = copy_to_user((void *)buffer->rw_bytes_done, &i, sizeof(*buffer->rw_bytes_done));

       // PVDEBUG("%s reading AI segment FIFO (scans read %d)\n",
       //                 pci_name(mcon->pci_dev), i);

        return retval;
}


/* Analog segment functions */

static int ao_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting AO\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Reset, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_Disarm, 1);
        flush_register(mcon, AO_Command_1_Register);

        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_BC_TC_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_START1_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_UPDATE_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_START_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_STOP_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_Error_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_UC_TC_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_B_Enable_Register, AO_FIFO_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_B_Enable_Register);

        set_register_bits_on_off(mcon, AO_Personal_Register, AO_BC_Source_Select, AO_BC_SOURCE_SELECT_UC_TC);
        flush_register(mcon, AO_Personal_Register);

        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_BC_TC_Trigger_Error_Confirm, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_BC_TC_Error_Confirm, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_UC_TC_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_BC_TC_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_START1_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_UPDATE_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_START_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_STOP_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_B_Ack_Register, AO_Error_Interrupt_Ack, 1);
        flush_register(mcon, Interrupt_B_Ack_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        pxi6259_ao_write_to_channel(mcon, 0, 0);
        pxi6259_ao_write_to_channel(mcon, 1, 0);
        pxi6259_ao_write_to_channel(mcon, 2, 0);
        pxi6259_ao_write_to_channel(mcon, 3, 0);

        mcon->ao_state = AO_STATE_UNKNOWN;

        return 0;
}


static int ao_clear_fifo(struct module_context *mcon)
{
        PDEBUG("%s AO Clear FIFO\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AO_FIFO_Clear_Register, AO_FIFO_Clear, 1);
        flush_register(mcon, AO_FIFO_Clear_Register);

        return 0;
}


static int ao_configure_dac(struct module_context *mcon)
{
        struct pxi6259_ao_conf aoTask = mcon->ao_task;
        u32 order = 0;

        PDEBUG("%s AO Configure DAC\n", pci_name(mcon->pci_dev));

//        set_register_bits_on_off(mcon, AO_FIFO_Clear_Register, AO_FIFO_Clear, 1);
//        flush_register(mcon, AO_FIFO_Clear_Register);

        if (aoTask.channel[0].enabled) {
                set_register_bits_on_off(mcon, AO_Config_Bank_0_Register, AO_DAC_Offset_Select_Mask,
                                AO_DAC_Offset_Select(aoTask.channel[0].offsetSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_0_Register,AO_DAC_Reference_Select_Mask,
                                AO_DAC_Reference_Select(aoTask.channel[0].referenceSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_0_Register,
                                AO_Update_Mode, aoTask.channel[0].updateMode);
                set_register_bits_on_off(mcon, AO_Config_Bank_0_Register,
                                AO_DAC_Polarity, aoTask.channel[0].polarity);
                flush_register(mcon, AO_Config_Bank_0_Register);

                write_register(mcon, Static_AI_Control_4_Register, 0);          //reference attenuation off

                write_register(mcon, AO_Waveform_Order_0_Register, order++);

        }

        if (aoTask.channel[1].enabled) {
                set_register_bits_on_off(mcon, AO_Config_Bank_1_Register, AO_DAC_Offset_Select_Mask,
                                AO_DAC_Offset_Select(aoTask.channel[1].offsetSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_1_Register,AO_DAC_Reference_Select_Mask,
                                AO_DAC_Reference_Select(aoTask.channel[1].referenceSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_1_Register,
                                AO_Update_Mode, aoTask.channel[1].updateMode);
                set_register_bits_on_off(mcon, AO_Config_Bank_1_Register,
                                AO_DAC_Polarity, aoTask.channel[1].polarity);
                flush_register(mcon, AO_Config_Bank_1_Register);

                write_register(mcon, Static_AI_Control_5_Register, 0);          //reference attenuation off

                write_register(mcon, AO_Waveform_Order_1_Register, order++);
        }

        if (aoTask.channel[2].enabled) {
                set_register_bits_on_off(mcon, AO_Config_Bank_2_Register, AO_DAC_Offset_Select_Mask,
                                AO_DAC_Offset_Select(aoTask.channel[2].offsetSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_2_Register,AO_DAC_Reference_Select_Mask,
                                AO_DAC_Reference_Select(aoTask.channel[2].referenceSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_2_Register,
                                AO_Update_Mode, aoTask.channel[2].updateMode);
                set_register_bits_on_off(mcon, AO_Config_Bank_2_Register,
                                AO_DAC_Polarity, aoTask.channel[2].polarity);
                flush_register(mcon, AO_Config_Bank_2_Register);

                write_register(mcon, Static_AI_Control_6_Register, 0);          //reference attenuation off

                write_register(mcon, AO_Waveform_Order_2_Register, order++);
        }

        if (aoTask.channel[3].enabled) {
                set_register_bits_on_off(mcon, AO_Config_Bank_3_Register, AO_DAC_Offset_Select_Mask,
                                AO_DAC_Offset_Select(aoTask.channel[3].offsetSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_3_Register, AO_DAC_Reference_Select_Mask,
                                AO_DAC_Reference_Select(aoTask.channel[3].referenceSelect));
                set_register_bits_on_off(mcon, AO_Config_Bank_3_Register,
                                AO_Update_Mode, aoTask.channel[3].updateMode);
                set_register_bits_on_off(mcon, AO_Config_Bank_3_Register,
                                AO_DAC_Polarity, aoTask.channel[3].polarity);
                flush_register(mcon, AO_Config_Bank_3_Register);

                write_register(mcon, Static_AI_Control_7_Register, 0);          //reference attenuation off

                write_register(mcon, AO_Waveform_Order_3_Register, order++);
        }

        return 0;
}


static int ao_personalize(struct module_context *mcon)
{
        PDEBUG("%s AO Personalize\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AO_Personal_Register, AO_UPDATE_Pulse_Timebase,
                        AO_UPDATE_PULSE_TIMEBASE_SELECT_BY_PULSE_WIDTH);
        set_register_bits_on_off(mcon, AO_Personal_Register, AO_UPDATE_Pulse_Width,
                        AO_UPDATE_PULSE_WIDTH_ABOUT_3_TIMEBASE_PERIODS);
        set_register_bits_on_off(mcon, AO_Personal_Register, AO_TMRDACWR_Pulse_Width,
                        AO_TMRDACWR_PULSE_WIDTH_ABOUT_2_TIMEBASE_PERIODS);
        set_register_bits_on_off(mcon, AO_Personal_Register, AO_Number_Of_DAC_Packages,
                        AO_NUMBER_OF_DAC_PACKAGES_SINGLE_DAC_MODE);
        flush_register(mcon, AO_Personal_Register);

        set_register_bits(mcon, AO_Output_Control_Register, AO_UPDATE_Output_Select_Mask,
                        AO_UPDATE_Output_Select (AO_UPDATE_OUTPUT_SELECT_HIGH_Z));
        flush_register(mcon, AO_Output_Control_Register);

        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Last_Gate_Disable,1);
        flush_register(mcon, AO_Mode_3_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}


static int ao_reset_waveform_channels (struct module_context *mcon)
{
        PDEBUG("%s resetting Waveform Channel Order\n", pci_name(mcon->pci_dev));

//        set_register_bits_on_off(mcon, AO_Config_Bank_0_Register, AO_Update_Mode, AO_UPDATE_MODE_IMMEDIATE);
//        flush_register(mcon, AO_Config_Bank_0_Register);

        write_register(mcon,AO_Config_Bank_0_Register,0);

//        set_register_bits_on_off(mcon, AO_Config_Bank_1_Register, AO_Update_Mode, AO_UPDATE_MODE_IMMEDIATE);
//        flush_register(mcon, AO_Config_Bank_1_Register);

        write_register(mcon,AO_Config_Bank_1_Register,0);

//        set_register_bits_on_off(mcon, AO_Config_Bank_2_Register, AO_Update_Mode, AO_UPDATE_MODE_IMMEDIATE);
//        flush_register(mcon, AO_Config_Bank_2_Register);

        write_register(mcon,AO_Config_Bank_2_Register,0);

//        set_register_bits_on_off(mcon, AO_Config_Bank_3_Register, AO_Update_Mode, AO_UPDATE_MODE_IMMEDIATE);
//        flush_register(mcon, AO_Config_Bank_3_Register);

        write_register(mcon,AO_Config_Bank_3_Register,0);

        write_register(mcon,AO_Waveform_Order_0_Register,0xF);
        write_register(mcon,AO_Waveform_Order_1_Register,0xF);
        write_register(mcon,AO_Waveform_Order_2_Register,0xF);
        write_register(mcon,AO_Waveform_Order_3_Register,0xF);
        write_register(mcon,AO_Waveform_Order_4_Register,0xF);
        write_register(mcon,AO_Waveform_Order_5_Register,0xF);
        write_register(mcon,AO_Waveform_Order_6_Register,0xF);
        write_register(mcon,AO_Waveform_Order_7_Register,0xF);
        write_register(mcon,AO_Waveform_Order_8_Register,0xF);
        write_register(mcon,AO_Waveform_Order_9_Register,0xF);
        write_register(mcon,AO_Waveform_Order_10_Register,0xF);
        write_register(mcon,AO_Waveform_Order_11_Register,0xF);
        write_register(mcon,AO_Waveform_Order_12_Register,0xF);
        write_register(mcon,AO_Waveform_Order_13_Register,0xF);
        write_register(mcon,AO_Waveform_Order_14_Register,0xF);
        write_register(mcon,AO_Waveform_Order_15_Register,0xF);

        return 0;
}


static int ao_channel_select (struct module_context *mcon, uint8_t multiChannel, uint8_t numberOfChannels)
{
        PDEBUG("%s selecting AO Channels\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        numberOfChannels--;

        // single channel
        set_register_bits_on_off(mcon, AO_Mode_1_Register, AO_Multiple_Channels, multiChannel);
        flush_register(mcon, AO_Mode_1_Register);

        set_register_bits(mcon, AO_Output_Control_Register, AO_Number_Of_Channels_Mask,
                        AO_Number_Of_Channels(numberOfChannels));
        flush_register(mcon, AO_Output_Control_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}


static int ao_trigger(struct module_context *mcon)
{
        struct pxi6259_ao_conf aoTask = mcon->ao_task;

        PDEBUG("%s selecting AO Trigger\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits(mcon, AO_Trigger_Select_Register, AO_START1_Select_Mask,
                        AO_START1_Select(aoTask.start1Source));
        set_register_bits(mcon, AO_Trigger_Select_Register, AO_START1_Polarity, aoTask.start1Polarity);
        set_register_bits_on_off(mcon, AO_Trigger_Select_Register, AO_START1_Edge, 1);
        set_register_bits_on_off(mcon, AO_Trigger_Select_Register, AO_START1_Sync, 1);
        flush_register(mcon, AO_Trigger_Select_Register);

        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Trigger_Length, AO_TRIGGER_LENGTH_DA_START1);
        flush_register(mcon, AO_Mode_3_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}


static int ao_count(struct module_context *mcon)
{
        struct pxi6259_ao_conf aoTask = mcon->ao_task;
        ao_continuous_t continuousMode;

        PDEBUG("%s configure AO Count %d\n", pci_name(mcon->pci_dev), aoTask.continuous);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        continuousMode = aoTask.continuous? AO_CONTINUOUS_IGNORE_BC_TC : AO_CONTINUOUS_STOP_ON_BC_TC;

        set_register_bits_on_off(mcon, AO_Mode_1_Register, AO_Continuous, continuousMode);
        set_register_bits_on_off(mcon, AO_Mode_1_Register, AO_Trigger_Once, aoTask.continuous? 0:1);
        flush_register(mcon, AO_Mode_1_Register);

        set_register_bits_on_off(mcon, AO_Mode_2_Register, AO_BC_Initial_Load_Source, AO_BC_INITIAL_LOAD_SOURCE_REG_A);
        set_register_bits_on_off(mcon, AO_Mode_2_Register, AO_UC_Initial_Load_Source, AO_UC_INITIAL_LOAD_SOURCE_REG_A);
        flush_register(mcon, AO_Mode_2_Register);

        write_register(mcon, AO_BC_Load_A_Register, aoTask.numberOfBuffers - 1);
        write_register(mcon, AO_UC_Load_A_Register, aoTask.numberOfSamples - 1);

        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_BC_Load, 1);
        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_UC_Load, 1);
        flush_register(mcon, AO_Command_1_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}

static int ao_update(struct module_context *mcon)
{
        struct pxi6259_ao_conf aoTask = mcon->ao_task;
        uint8_t internalUpdate;

        PDEBUG("%s Configure AO Update...\n", pci_name(mcon->pci_dev));

        internalUpdate = (aoTask.updateSource == AO_UPDATE_SOURCE_SELECT_UI_TC);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AO_Command_2_Register, AO_BC_Gate_Enable, internalUpdate ? 0 : 1);
        flush_register(mcon, AO_Command_2_Register);

        set_register_bits(mcon, AO_Mode_1_Register, AO_UPDATE_Source_Select_Mask,
                        AO_UPDATE_Source_Select(aoTask.updateSource));
        set_register_bits(mcon, AO_Mode_1_Register, AO_UPDATE_Source_Polarity, aoTask.updatePolarity);
        flush_register(mcon, AO_Mode_1_Register);

        if (internalUpdate)
        {
                write_register(mcon, AO_UI_Load_A_Register, 1);

                set_register_bits_on_off(mcon, AO_Command_1_Register, AO_UI_Load, 1);
                flush_register(mcon, AO_Command_1_Register);

                write_register(mcon, AO_UI_Load_A_Register, aoTask.updatePeriodDivisor);
        }

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}

static int ao_fifo_mode(struct module_context *mcon)
{
        struct pxi6259_ao_conf aoTask = mcon->ao_task;

        PDEBUG("%s configure AO FIFO Mode\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AO_Mode_2_Register, AO_FIFO_Retransmit_Enable, aoTask.fifoRetransmitEnable);
        set_register_bits(mcon, AO_Mode_2_Register, AO_FIFO_Mode_Mask,
                        AO_FIFO_Mode(AO_FIFO_MODE_LESS_THAN_FULL));
        flush_register(mcon, AO_Mode_2_Register);

        set_register_bits_on_off(mcon, AO_Personal_Register, AO_FIFO_Enable, 1);
        flush_register(mcon, AO_Personal_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}

static int ao_stop(struct module_context *mcon)
{
        PDEBUG("%s configure AO Stop\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_Start, 1);
        flush_register(mcon, Joint_Reset_Register);

        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Stop_On_BC_TC_Error, 0);
        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Stop_On_BC_TC_Trigger_Error, 1);
        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Stop_On_Overrun_Error, 1);
        flush_register(mcon, AO_Mode_3_Register);

        set_register_bits_on_off(mcon, Joint_Reset_Register, AO_Configuration_End, 1);
        flush_register(mcon, Joint_Reset_Register);

        return 0;
}

#define AO_JOINT_STATUS_MAX_RETRY 0xff

static int ao_arm(struct module_context *mcon)
{
        uint32_t jointStatus2;
        uint32_t i = 0;

        PDEBUG("%s arm AO Counters\n", pci_name(mcon->pci_dev));

//        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Not_An_UPDATE, 0);
//        flush_register(mcon, AO_Mode_3_Register);
        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Not_An_UPDATE, 1);
        flush_register(mcon, AO_Mode_3_Register);
        set_register_bits_on_off(mcon, AO_Mode_3_Register, AO_Not_An_UPDATE, 0);
        flush_register(mcon, AO_Mode_3_Register);

        for (i=0; i<AO_JOINT_STATUS_MAX_RETRY; i++) {
                read_register(mcon, Joint_Status_2_Register, &jointStatus2);
                if (!(jointStatus2 & AO_TMRDACWRs_In_Progress_St))
                        break;
        }
        if (i >= AO_JOINT_STATUS_MAX_RETRY) {
                PDEBUG("%s Error: AO Arm failed...\n", pci_name(mcon->pci_dev));
                return -PXI6259_ERROR_AO_COUNTER_BUSY;
        }

        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_UI_Arm, 1);
        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_UC_Arm, 1);
        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_BC_Arm, 1);
        flush_register(mcon, AO_Command_1_Register);

        return 0;
}


static int ao_start(struct module_context *mcon)
{
        PDEBUG("%s AO start\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AO_Command_2_Register, AO_START1_Pulse, 1);
        flush_register(mcon, AO_Command_2_Register);

        return 0;
}


static int ao_disarm(struct module_context *mcon)
{
        PDEBUG("%s disarming AO segment\n", pci_name(mcon->pci_dev));

        // TODO test if this two lines are necessary
        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_UI2_Arm_Disarm, 0);
        flush_register(mcon, AO_Command_1_Register);

        set_register_bits_on_off(mcon, AO_Command_1_Register, AO_Disarm, 1);
        flush_register(mcon, AO_Command_1_Register);

        return 0;
}

static int ao_calibration(struct module_context *mcon, uint8_t value)
{
        PDEBUG("%s AO calibration\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, AO_Calibration_Register, AO_RefGround, value);
        flush_register(mcon, AO_Calibration_Register);

        return 0;
}

/* AO segment function used within ioctl calls */

static int pxi6259_load_ao_conf(struct module_context *mcon, struct pxi6259_ao_conf *ao_task)
{
        uint8_t numberOfChannels = 0;
        uint8_t multiChannel = 0;
        int i = 0, idx;
        int retval;

        PDEBUG("%s loading AO configuration...\n", pci_name(mcon->pci_dev));

        /* Check if AI segment is being used */
        if (!list_empty(&mcon->aochan_list)) {
                printk(KERN_INFO "%s AO channels are being used by 1 or more clients, cannot load new configuration\n",
                                pci_name(mcon->pci_dev));
                return -EBUSY;
        }

        /* Check if segment is started */
        if (mcon->ao_state != AO_STATE_UNKNOWN) {
                printk(KERN_INFO "%s AO segment is started, stopping it!\n",
                                pci_name(mcon->pci_dev));
                pxi6259_release_ao(mcon);
        }

        mcon->ao_task = *ao_task;

      //  ao_disarm(mcon);
        ao_reset(mcon);
        ao_personalize(mcon);
        ao_reset_waveform_channels(mcon);
        ao_clear_fifo(mcon);
        ao_calibration(mcon,0);
        ao_configure_dac(mcon);

        if (ao_task->signalGeneration == AO_SIGNAL_GENERATION_WAVEFORM) {
                /* Count enabled channels */
                for (i=0; i<PXI6259_NUMBER_OF_AO_CHANNELS; i++){
                        if (ao_task->channel[i].enabled)
                                numberOfChannels++;
                }
                PDEBUG("%s channels in use %d\n",pci_name(mcon->pci_dev),numberOfChannels);

                if (numberOfChannels > 1) {
                        multiChannel = 1;
                        switch (numberOfChannels) {
                          case 1:
                                if (mcon->ao_task.updatePeriodDivisor < 7)
                                        mcon->ao_task.updatePeriodDivisor = 7;
                                break;
                          case 2:
                                if (mcon->ao_task.updatePeriodDivisor < 10)
                                        mcon->ao_task.updatePeriodDivisor = 10;
                                break;
                          case 3:
                                if (mcon->ao_task.updatePeriodDivisor < 13)
                                        mcon->ao_task.updatePeriodDivisor = 13;
                                break;
                          case 4:
                                if (mcon->ao_task.updatePeriodDivisor < 16)
                                        mcon->ao_task.updatePeriodDivisor = 16;
                                break;
                          default:
                                break;
                        }
                } else {
                        multiChannel = 0;
                }

                if (!numberOfChannels) {
                        printk (KERN_ERR "%s No AO Channels Selected!\n",
                                        pci_name(mcon->pci_dev));
                        return -PXI6259_ERROR_NO_AO_CHANNELS_IN_USE;
                }



                ao_channel_select(mcon,multiChannel, numberOfChannels);
                ao_trigger(mcon);
                ao_count(mcon);
                ao_update(mcon);
                ao_fifo_mode (mcon);
                ao_stop(mcon);
        }

        retval = dma_channel_config(mcon, &mcon->ao_chan, 1,
                        ao_task->fifoRetransmitEnable ? DMA_MODE_NORMAL : DMA_MODE_RING, //DMA MODE
                        DMA_TO_DEVICE, //DMA DIRECTION
                        ao_task->fifoRetransmitEnable ?
                                ao_task->numberOfSamples * ao_task->numberOfBuffers * AO_CHAN_MITE_DATA_WIDTH * numberOfChannels : //DMA SIZE NORMAL
                                1000000,                                                                                           //DMA SIZE RING ??
                        AO_CHAN_MITE_DATA_WIDTH, AO_CHAN_MITE_OFFSET);
        if (retval) {
                printk(KERN_ERR "%s failed to configure AO DMA channel %d!\n",
                                pci_name(mcon->pci_dev), retval);
                return retval;
        }

        /* Update current location - support for multiple clients */
        memset(mcon->ao_cli_info, 0, sizeof(mcon->ao_cli_info));

        for (i=0, idx=0; i<=PXI6259_MAX_AO_CHANNEL; i++) {
                if (mcon->ao_task.channel[i].enabled) {
                        mcon->ao_cli_info[i].location = idx * AO_CHAN_MITE_DATA_WIDTH;
                        mcon->ao_cli_info[i].write_idx = idx * AO_CHAN_MITE_DATA_WIDTH;
                        mcon->ao_cli_info[i].last_write_idx = idx * AO_CHAN_MITE_DATA_WIDTH;
                        mcon->ao_cli_info[i].number_of_samples=0;
                        idx++;
                }
        }

        mcon->ao_state = AO_STATE_CONFIGURED;

        retval = pxi6259_update_ao_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1 + PXI6259_MAX_AI_CHANNEL + 1 + 1));
        if (retval) {
                PDEBUG("%s failed to update AO devices\n", pci_name(mcon->pci_dev));
                mcon->ao_state = AO_STATE_UNKNOWN;
                return retval;
        }

        PDEBUG("%s AO configuration is loaded.\n", pci_name(mcon->pci_dev));
        return 0;
}

static int pxi6259_start_ao(struct module_context *mcon)
{
        int retval = 0, i;
        int dma_wait_max = 0xfffff;

        PDEBUG("%s starting AO segment...\n", pci_name(mcon->pci_dev));

        /* Check if segment is configured */
        if (mcon->ao_state == AO_STATE_UNKNOWN) {
                PDEBUG("%s AO segment is not configured, can not start!\n", pci_name(mcon->pci_dev));
                return -EINVAL;
        }

        /* Check if segment is started */
        if (mcon->ao_state == AO_STATE_STARTED) {
                printk(KERN_DEBUG "%s AO segment is already started!\n",
                                pci_name(mcon->pci_dev));
                return -EBUSY;
        }

        if (mcon->ao_task.signalGeneration == AO_SIGNAL_GENERATION_WAVEFORM) {

                if( mcon->ao_chan.state != DMA_STARTED ){
                   /* Select the DMA Channel and enable FIFO request */
                   set_register_bits(mcon, AO_Start_Select_Register, AO_AOFREQ_Enable, AO_AOFREQ_Enable);
                   flush_register(mcon, AO_Start_Select_Register);
                   set_register_bits_on_off(mcon, AI_AO_Select_Register, ((u8)2 << 0x4) & ~0xf, 1);
                   flush_register(mcon, AI_AO_Select_Register);

                   /* The DMA process will start moving data even if the timing engine is not started, it's
                      only controlled by the FIFO request flag, which is set to request to less-than-full in
                      aoFifoMode(). */
                   retval = dma_channel_start(mcon, &mcon->ao_chan);
                   if (retval) {
                           printk(KERN_ERR "%s start AO DMA channel failed %d!\n",
                                           pci_name(mcon->pci_dev), retval);
                           return retval;
                   }
                }

                /* Find the last enabled channel */
                for (i=PXI6259_MAX_AO_CHANNEL; i>=0; i--) {
                         if (mcon->ao_task.channel[i].enabled) {
                             break;
                         }
                 }

                /* If starting with FIFO retransmit ALL of the samples must be transferred */
                /* When the last enabled channel is done transferring all of the samples are transferred */
                /* This wait loop is intentionally not using wait_queue, since we are blocking while holding the ao_lock*/
                if(mcon->ao_task.fifoRetransmitEnable){
                    PDEBUG("%s Waiting for DMA to finish\n",pci_name(mcon->pci_dev));
                    while(bytes_in_dma_channel(mcon,&mcon->ao_chan,&mcon->ao_cli_info[i]) > 10){
                        PVDEBUG("%s waiting for DMA to finish\n");

                        if (signal_pending(current)) {
                            return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
                        }

                        dma_wait_max --;
                        if(dma_wait_max <= 0){
                            printk(KERN_ERR "%s DMA transfer timeout. Bytes still in buffer : %d\n",pci_name(mcon->pci_dev), bytes_in_dma_channel(mcon,&mcon->ao_chan,&mcon->ao_cli_info[i]) );
                            return -ERESTARTSYS;
                        }
                    }
                    PDEBUG("%s DMA flushed\n",pci_name(mcon->pci_dev));
                }

                retval = ao_arm(mcon);
                if (retval) {
                        PDEBUG("%s Error: Arming AO counters\n", pci_name(mcon->pci_dev));
                        return retval;
                }
                retval = ao_start(mcon);
                if (retval) {
                        PDEBUG("%s Error: Starting AO task\n", pci_name(mcon->pci_dev));
                        return retval;
                }

                mcon->ao_state = AO_STATE_STARTED;
        }

        return 0;
}


static int pxi6259_is_ao_done(struct module_context *mcon, uint8_t *is_task_done)
{
        return -EINVAL;
}


static int pxi6259_stop_ao(struct module_context *mcon)
{
        int i, idx;

        PDEBUG("%s stopping AO segment...\n", pci_name(mcon->pci_dev));


        if (mcon->ao_state == AO_STATE_UNKNOWN) {
                PDEBUG("%s AO segment is not configured, can not stop!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        if (mcon->ao_state == AO_STATE_STARTED) {

                PDEBUG(KERN_INFO"%s dma in buffer %d\n",pci_name(mcon->pci_dev),bytes_in_dma_channel(mcon,&mcon->ao_chan,&mcon->ao_cli_info));


                /* Disable the DMA Channel and disable FIFO request */
                dma_channel_stop(mcon, &mcon->ao_chan);
                /* And also reset and configure DMA channel so it is possible to write samples
                 * after new configuration is loaded.
                 */
                dma_channel_reset(mcon, &mcon->ao_chan);
                dma_channel_configure(mcon, &mcon->ao_chan);

                /* Update current location - support for multiple clients */
                memset(mcon->ao_cli_info, 0, sizeof(mcon->ao_cli_info));
                for (i=0, idx=0; i<=PXI6259_MAX_AO_CHANNEL; i++) {
                        if(mcon->ao_task.channel[i].enabled) {
                                mcon->ao_cli_info[i].location = idx * AO_CHAN_MITE_DATA_WIDTH;
                                mcon->ao_cli_info[i].number_of_samples = 0;
                                idx++;
                        }
                }

                ao_disarm(mcon);

                set_register_bits(mcon, AO_Start_Select_Register, AO_AOFREQ_Enable, 0);
                flush_register(mcon, AO_Start_Select_Register);
                set_register_bits_on_off(mcon, AI_AO_Select_Register, ((u8)2 << 0x4) & ~0xf, 0);
                flush_register(mcon, AI_AO_Select_Register);

                mcon->ao_state = AO_STATE_CONFIGURED;
        }

        return 0;
}

static int pxi6259_release_ao(struct module_context *mcon)
{
        int retval;

        if (mcon->ao_state == AO_STATE_STARTED) {
                pxi6259_stop_ao(mcon); /* We have to release segment anyway */
        }

        ao_disarm(mcon);

        /* Reset some registers */
        set_register_bits_on_off(mcon, AO_Mode_1_Register, 0xFFFFFFFF, 0);
        mcon->ao_state = AO_STATE_UNKNOWN;

        retval = pxi6259_update_ao_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1 + PXI6259_MAX_AI_CHANNEL + 1 + 1));
        if (retval) return retval;

        PDEBUG("%s AO segment released\n", pci_name(mcon->pci_dev));

        return 0;
}


static int pxi6259_ao_write_to_channel(struct module_context *mcon,
                enum pxi6259_channel channel, uint32_t value)
{
        if (mcon->ao_state == AO_STATE_UNKNOWN) {
                PDEBUG("%s AO segment is not configured, can not write!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        PDEBUG("%s write to channel %d raw: %x\n", pci_name(mcon->pci_dev), channel, value);

        switch (channel) {
          case 0:
                  write_register(mcon, DAC_Direct_Data_0_Register, value);
                  break;
          case 1:
                  write_register(mcon, DAC_Direct_Data_1_Register, value);
                  break;
          case 2:
                  write_register(mcon, DAC_Direct_Data_2_Register, value);
                  break;
          case 3:
                  write_register(mcon, DAC_Direct_Data_3_Register, value);
                  break;
          default:
                  break;
        }

        return 0;
}

#define WRITE_AO_FIFO_SIZE 1024
/*
 * WARNING: this function copies data from user space!
 */
static int pxi6259_ao_write_to_fifo(struct module_context *mcon,
                struct pxi6259_rw_buffer *buffer)
{
        int i = 0, j;
        uint32_t raw_buffer[WRITE_AO_FIFO_SIZE];
        uint32_t num_samples, tmp_length;
        uint32_t *ptr, *zeroptr;
        int retval;

        num_samples = (int)buffer->rw_bytes / (int)4;
        zeroptr = (uint32_t *)buffer->buffer;

        PDEBUG("Writing to AO FIFO num_samples %d %d\n", num_samples, buffer->rw_bytes);

        if (mcon->ao_state == AO_STATE_UNKNOWN) {
                PDEBUG("%s AO segment is not configured, can not write fifo!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        for(i = (num_samples / WRITE_AO_FIFO_SIZE); i >= 0; i--) {
                ptr = &zeroptr[i*WRITE_AO_FIFO_SIZE];
                tmp_length = (i > 0) ? WRITE_AO_FIFO_SIZE
                                : num_samples % WRITE_AO_FIFO_SIZE;

                retval = copy_from_user((void *)raw_buffer, (void *)ptr, tmp_length * 4);
                if (retval) return -EINVAL;

                for (j=0; j<tmp_length; j++) {
                        write_register(mcon, AO_FIFO_Data_Register, raw_buffer[j]);
                }
        }

        return 0;
}

/*
 * GENERAL PURPOSE COUNTERS SECTION
 */

static int gpc0_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting GPC0\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, G0_Reset, 1);
        flush_register(mcon, Joint_Reset_Register);

        write_register(mcon, G0_Mode_Register, 0);
        write_register(mcon, G0_Command_Register, 0);
        write_register(mcon, G0_Input_Select_Register, 0);

        write_register(mcon, G0_Autoincrement_Register, 0);

        set_register_bits_on_off(mcon, Interrupt_A_Enable_Register, G0_TC_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_A_Enable_Register, G0_Gate_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        set_register_bits_on_off(mcon, G0_Command_Register, G_Synchronized_Gate, GPC_SYNCHRONIZED_GATE_ENABLED);
        flush_register(mcon, G0_Command_Register);

        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G0_Gate_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G0_TC_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G0_TC_Error_Confirm, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G0_Gate_Error_Confirm, 1);
        flush_register(mcon, Interrupt_A_Ack_Register);

        write_register(mcon, G0_Autoincrement_Register, 0);

        write_register(mcon, G0_Counting_Mode_Register, 0);

        write_register(mcon, G0_Second_Gate_Register, 0);

        mcon->gpc0_state = GPC_STATE_UNKNOWN;

        return 0;
}

static int gpc1_reset(struct module_context *mcon)
{
        PDEBUG("%s resetting GPC1\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, Joint_Reset_Register, G1_Reset, 1);
        flush_register(mcon, Joint_Reset_Register);

        write_register(mcon, G1_Mode_Register, 0);
        write_register(mcon, G1_Command_Register, 0);
        write_register(mcon, G1_Input_Select_Register, 0);

        write_register(mcon, G1_Autoincrement_Register, 0);

        set_register_bits_on_off(mcon, Interrupt_A_Enable_Register, G1_TC_Interrupt_Enable, 0);
        set_register_bits_on_off(mcon, Interrupt_A_Enable_Register, G1_Gate_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        set_register_bits_on_off(mcon, G1_Command_Register, G_Synchronized_Gate, GPC_SYNCHRONIZED_GATE_ENABLED);
        flush_register(mcon, G1_Command_Register);

        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G1_Gate_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G1_TC_Interrupt_Ack, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G1_TC_Error_Confirm, 1);
        set_register_bits_on_off(mcon, Interrupt_A_Ack_Register, G1_Gate_Error_Confirm, 1);
        flush_register(mcon, Interrupt_A_Ack_Register);

        write_register(mcon, G1_Autoincrement_Register, 0);

        write_register(mcon, G1_Counting_Mode_Register, 0);

        write_register(mcon, G1_Second_Gate_Register, 0);

        mcon->gpc1_state = GPC_STATE_UNKNOWN;

        return 0;
}


static int gpc0_arm(struct module_context *mcon)
{
        PDEBUG("%s arming GPC0\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, G0_Command_Register, G_Arm, 1);
        flush_register(mcon, G0_Command_Register);

        return 0;
}

static int gpc1_arm(struct module_context *mcon) {
        PDEBUG("%s arming GPC1\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, G1_Command_Register, G_Arm, 1);
        flush_register(mcon, G1_Command_Register);

        return 0;
}

static int gpc0_disarm(struct module_context *mcon)
{
        PDEBUG("%s disarming GPC0\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, G0_Command_Register, G_Disarm, 1);
        flush_register(mcon, G0_Command_Register);

        return 0;
}

static int gpc1_disarm(struct module_context *mcon)
{
        PDEBUG("%s disarming GPC1\n", pci_name(mcon->pci_dev));

        set_register_bits_on_off(mcon, G1_Command_Register, G_Disarm, 1);
        flush_register(mcon, G1_Command_Register);

        return 0;
}

// preset for single pulse generation
static int gpc0_single_pulse_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc0Task = mcon->gpc0_task;

        PDEBUG("%s single pulse configuration for GPC0\n", pci_name(mcon->pci_dev));

        // select source A
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G0_Mode_Register);

        // load value of delay before pulse
        write_register(mcon, G0_Load_A_Register, gpc0Task.initialValue);

        // load the value to timer
        set_register_bits_on_off(mcon, G0_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G0_Command_Register);

        // load value of pulse lenght
        write_register(mcon, G0_Load_B_Register, gpc0Task.initialValue);

        // select source B
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_B);
        flush_register(mcon, G0_Mode_Register);

        // select G_IN_TIMEBASE1
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(GPC_SOURCE_SELECT_TIMEBASE1));
        // select polarity: rising edge
        set_register_bits(mcon, G0_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);
        // select: counter src
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(gpc0Task.source));
        // select output polarity: active high
        set_register_bits(mcon, G0_Input_Select_Register, G_Output_Polarity, 1);
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_Gate_Select_Load_Source, 0);
        flush_register(mcon, G0_Input_Select_Register);


        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_Polarity, 0);
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Output_Mode_Mask, G_Output_Mode(1));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_ALTERNATE);
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_NO_RELOAD);
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G0_Mode_Register,
                        G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_EDGE_GATING_ACTIVE_HIGH));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G0_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask,
                        G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_SECOND_TC));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        flush_register(mcon, G0_Mode_Register);

        set_register_bits(mcon, G0_Command_Register, G_Up_Down_Mask, G_Up_Down(GPC_COUNT_DIRECTION_SOFTWARE_DOWN));
        set_register_bits(mcon, G0_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits(mcon, G0_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G0_Command_Register);

        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_Gate_Interrupt_Enable, 0);
        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_TC_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        return 0;
}

// preset for single pulse generation
static int gpc1_single_pulse_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc1Task = mcon->gpc1_task;

        PDEBUG("%s single pulse configuration for GPC1\n", pci_name(mcon->pci_dev));

        // select source A
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G1_Mode_Register);

        // load value of delay before pulse
        write_register(mcon, G1_Load_A_Register, gpc1Task.initialValue);

        // load the value to timer
        set_register_bits_on_off(mcon, G1_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G1_Command_Register);

        // load value of pulse lenght
        write_register(mcon, G1_Load_B_Register, gpc1Task.initialValue);

        // select source B
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_B);
        flush_register(mcon, G1_Mode_Register);

        // select G_IN_TIMEBASE1
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(GPC_SOURCE_SELECT_TIMEBASE1));
        // select polarity: rising edge
        set_register_bits(mcon, G1_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);
        // select: counter src
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(gpc1Task.source));
        // select output polarity: active high
        set_register_bits(mcon, G1_Input_Select_Register, G_Output_Polarity, 1);
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_Gate_Select_Load_Source, 0);
        flush_register(mcon, G1_Input_Select_Register);


        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_Polarity, 0);
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Output_Mode_Mask, G_Output_Mode(1));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_ALTERNATE);
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_NO_RELOAD);
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G1_Mode_Register,
                        G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_EDGE_GATING_ACTIVE_HIGH));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G1_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask,
                        G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_SECOND_TC));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        flush_register(mcon, G1_Mode_Register);

        set_register_bits(mcon, G1_Command_Register, G_Up_Down_Mask, G_Up_Down(GPC_COUNT_DIRECTION_SOFTWARE_DOWN));
        set_register_bits(mcon, G1_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits(mcon, G1_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G1_Command_Register);


        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_Gate_Interrupt_Enable, 0);
        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_TC_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        return 0;
}


// preset for simple counting
static int gpc0_simple_count_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc0Task = mcon->gpc0_task;

        PDEBUG("%s Single count configuration for GPC0\n", pci_name(mcon->pci_dev));

        // select the source
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G0_Mode_Register);

        // load source into load register
        write_register(mcon, G0_Load_A_Register, gpc0Task.initialValue);

        // load value into timer
        set_register_bits_on_off(mcon, G0_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G0_Command_Register);

        // select source for the counter
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(gpc0Task.source));

        // select polarity
        set_register_bits(mcon, G0_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);

        // set gating
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(GPC_GATE_SELECT_LOGIC_LOW));
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_Gate_Select_Load_Source, 0); // missing
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_Output_Polarity, 0);         // missing
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_OR_Gate, 0);                 // missing
        flush_register(mcon, G0_Input_Select_Register);

        // set mode
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_Polarity, 1);           // invert = 1
        set_register_bits(mcon, G0_Mode_Register, G_Output_Mode_Mask, G_Output_Mode(GPC_OUTPUT_MODE_PULSE));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_NO_RELOAD);
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G0_Mode_Register, G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_LEVEL_GATING));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G0_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask, G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_SECOND_TC));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_USE_SAME);
        flush_register(mcon, G0_Mode_Register);

        // set commands for G0
        set_register_bits(mcon, G0_Command_Register, G_Up_Down_Mask, G_Up_Down(gpc0Task.countDirection));
        set_register_bits_on_off(mcon, G0_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits_on_off(mcon, G0_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G0_Command_Register);

        // disable interrupts
        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_Gate_Interrupt_Enable, 0);// missing
        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_TC_Interrupt_Enable, 0);// missing
        flush_register(mcon, Interrupt_A_Enable_Register);

        // setup DMA
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Enable, 0);// missing
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Int_Enable, 0);// missing
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Write, 0);// missing
        flush_register(mcon, G0_DMA_Config_Register);

        // reset DMA
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Reset, 1);// missing
        flush_register(mcon, G0_DMA_Config_Register);

        return 0;
}


// preset for simple counting
static int gpc1_simple_count_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc1Task = mcon->gpc1_task;

        PDEBUG("%s Single count configuration for GPC1\n", pci_name(mcon->pci_dev));

        // select source A
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G1_Mode_Register);

        // load initial counter value
        write_register(mcon, G1_Load_A_Register, gpc1Task.initialValue);
        write_register(mcon, G1_Load_B_Register, 0);

        // load the value to timer
        set_register_bits_on_off(mcon, G1_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G1_Command_Register);

        // select G_IN_TIMEBASE1
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(gpc1Task.source));
        // select polarity: rising edge
        set_register_bits(mcon, G1_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);

        // set input
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(GPC_GATE_SELECT_LOGIC_LOW));
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_Gate_Select_Load_Source, 0);
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_Output_Polarity, 0);
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_OR_Gate, 0);
        flush_register(mcon, G1_Input_Select_Register);

        // set mode
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_Polarity, 1);           // invert = 1
        set_register_bits(mcon, G1_Mode_Register, G_Output_Mode_Mask, G_Output_Mode(GPC_OUTPUT_MODE_PULSE));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_NO_RELOAD);
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G1_Mode_Register, G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_LEVEL_GATING));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G1_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask, G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_SECOND_TC));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_USE_SAME);
        flush_register(mcon, G1_Mode_Register);

        // set commands for G1
        set_register_bits(mcon, G1_Command_Register, G_Up_Down_Mask, G_Up_Down(gpc1Task.countDirection));
        set_register_bits_on_off(mcon, G1_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits_on_off(mcon, G1_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G1_Command_Register);

        // disable interrupts
        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_Gate_Interrupt_Enable, 0);
        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_TC_Interrupt_Enable, 0);
        flush_register(mcon, Interrupt_A_Enable_Register);

        // setup DMA
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Enable, 0);
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Int_Enable, 0);
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Write, 0);
        flush_register(mcon, G1_DMA_Config_Register);

        // reset DMA
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Reset, 1);
        flush_register(mcon, G1_DMA_Config_Register);

        return 0;
}


// preset for simple counting
static int gpc0_delay_input_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc0Task = mcon->gpc0_task;

        PDEBUG("%s Single count configuration for GPC0\n", pci_name(mcon->pci_dev));

        // select the source
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G0_Mode_Register);

        // load source into load register
        write_register(mcon, G0_Load_A_Register, gpc0Task.initialValue);

        // load value into timer
        set_register_bits_on_off(mcon, G0_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G0_Command_Register);

        // select source for the counter
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(gpc0Task.source));

        // select polarity
        set_register_bits(mcon, G0_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);

        // set gating
        set_register_bits(mcon, G0_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(gpc0Task.gateSource));
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_Gate_Select_Load_Source, 0); // missing
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_Output_Polarity, 0);         // missing
        set_register_bits_on_off(mcon, G0_Input_Select_Register, G_OR_Gate, 0);                 // missing
        flush_register(mcon, G0_Input_Select_Register);

        // set mode
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_Polarity, 0);
        set_register_bits(mcon, G0_Mode_Register, G_Output_Mode_Mask, G_Output_Mode(GPC_OUTPUT_MODE_PULSE));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_RELOAD_ON_STOP_GATE);
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G0_Mode_Register, G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_EDGE_GATING_ACTIVE_HIGH));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G0_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask,
                        G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_FIRST_TC));
        set_register_bits(mcon, G0_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        set_register_bits_on_off(mcon, G0_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_USE_SAME);
        flush_register(mcon, G0_Mode_Register);

        // set commands for G0
        set_register_bits(mcon, G0_Command_Register, G_Up_Down_Mask, G_Up_Down(gpc0Task.countDirection));
        set_register_bits_on_off(mcon, G0_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits_on_off(mcon, G0_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G0_Command_Register);

        // disable interrupts
        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_Gate_Interrupt_Enable, 0);// missing
        set_register_bits(mcon, Interrupt_A_Enable_Register, G0_TC_Interrupt_Enable, 0);// missing
        flush_register(mcon, Interrupt_A_Enable_Register);

        // setup DMA
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Enable, 0);// missing
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Int_Enable, 0);// missing
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Write, 0);// missing
        flush_register(mcon, G0_DMA_Config_Register);

        // reset DMA
        set_register_bits(mcon, G0_DMA_Config_Register, G_DMA_Reset, 1);// missing
        flush_register(mcon, G0_DMA_Config_Register);

        return 0;
}


// preset for delaying triggers
static int gpc1_delay_input_config(struct module_context *mcon)
{
        struct pxi6259_gpc_conf gpc1Task = mcon->gpc1_task;

        PDEBUG("%s Single count configuration for GPC0\n", pci_name(mcon->pci_dev));

        // select the source
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Load_Source_Select, GPC_LOAD_SOURCE_SELECT_LOAD_A);
        flush_register(mcon, G1_Mode_Register);

        // load source into load register
        write_register(mcon, G1_Load_A_Register, gpc1Task.initialValue);

        // load value into timer
        set_register_bits_on_off(mcon, G1_Command_Register, G_Load, GPC_LOAD_LOAD);
        flush_register(mcon, G1_Command_Register);

        // select source for the counter
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Source_Select_Mask, G_Source_Select(gpc1Task.source));

        // select polarity
        set_register_bits(mcon, G1_Input_Select_Register, G_Source_Polarity, GPC_SOURCE_POLARITY_RISING_EDGE);

        // set gating
        set_register_bits(mcon, G1_Input_Select_Register,
                        G_Gate_Select_Mask, G_Gate_Select(gpc1Task.gateSource));
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_Gate_Select_Load_Source, 0); // missing
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_Output_Polarity, 0);         // missing
        set_register_bits_on_off(mcon, G1_Input_Select_Register, G_OR_Gate, 0);                 // missing
        flush_register(mcon, G1_Input_Select_Register);

        // set mode
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_Polarity, 0);
        set_register_bits(mcon, G1_Mode_Register, G_Output_Mode_Mask, G_Output_Mode(GPC_OUTPUT_MODE_PULSE));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_Gate, GPC_LOADING_ON_GATE_RELOAD_ON_STOP_GATE);
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Loading_On_TC, GPC_LOADING_ON_TC_RELOAD_ON_TC);
        set_register_bits(mcon, G1_Mode_Register, G_Gating_Mode_Mask, G_Gating_Mode(GPC_GATING_MODE_EDGE_GATING_ACTIVE_HIGH));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Gate_On_Both_Edges, GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED);
        set_register_bits(mcon, G1_Mode_Register,
                        G_Trigger_Mode_For_Edge_Gate_Mask,
                        G_Trigger_Mode_For_Edge_Gate(GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Stop_Mode_Mask, G_Stop_Mode(GPC_STOP_MODE_STOP_ON_GATE_OR_FIRST_TC));
        set_register_bits(mcon, G1_Mode_Register,
                        G_Counting_Once_Mask, G_Counting_Once(GPC_COUNTING_ONCE_NO_HW_DISARM));
        set_register_bits_on_off(mcon, G1_Mode_Register, G_Reload_Source_Switching, GPC_RELOAD_SOURCE_USE_SAME);
        flush_register(mcon, G1_Mode_Register);

        // set commands for G1
        set_register_bits(mcon, G1_Command_Register, G_Up_Down_Mask, G_Up_Down(gpc1Task.countDirection));
        set_register_bits_on_off(mcon, G1_Command_Register, G_Bank_Switch_Enable, GPC_BANK_SWITCH_ENABLE_BANK_X);
        set_register_bits_on_off(mcon, G1_Command_Register, G_Bank_Switch_Mode, GPC_BANK_SWITCH_MODE_GATE);
        flush_register(mcon, G1_Command_Register);

        // disable interrupts
        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_Gate_Interrupt_Enable, 0);// missing
        set_register_bits(mcon, Interrupt_A_Enable_Register, G1_TC_Interrupt_Enable, 0);// missing
        flush_register(mcon, Interrupt_A_Enable_Register);

        // setup DMA
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Enable, 0);// missing
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Int_Enable, 0);// missing
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Write, 0);// missing
        flush_register(mcon, G1_DMA_Config_Register);

        // reset DMA
        set_register_bits(mcon, G1_DMA_Config_Register, G_DMA_Reset, 1);// missing
        flush_register(mcon, G1_DMA_Config_Register);

        return 0;
}

/* GPC functions used within ioctl calls */

static int pxi6259_load_gpc0_conf(struct module_context *mcon, struct pxi6259_gpc_conf *gpc_conf)
{
        uint16_t retval;

        PDEBUG("%s Loading GPC0 configuration...\n", pci_name(mcon->pci_dev));

        /* Check if segment is started */
        if (mcon->gpc0_state == GPC_STATE_STARTED) {
                printk(KERN_INFO "%s GPC0 segment is started, loading new configuration!\n",
                                pci_name(mcon->pci_dev));

                //TODO release segment
                mcon->gpc0_state = GPC_STATE_UNKNOWN;
        }

        /* Save the configuration */
        mcon->gpc0_task = *gpc_conf;

        // reset counter
        gpc0_reset (mcon);

        switch (gpc_conf->gpcInfo) {
          case GPC_TASK_TYPE_SIMPLE_COUNTER:
                  // load settings
                  gpc0_simple_count_config(mcon);
                  break;

          case GPC_TASK_TYPE_SINGLE_PULSE_GENERATION:
                  // load settings
                  gpc0_single_pulse_config(mcon);
                  break;

          case GPC_TASK_TYPE_DELAY_OUTPUT_TRIGGER:
                  // load delay settings
                  gpc0_delay_input_config(mcon);
                  break;

          default:
                  printk(KERN_ERR "%s GPC0 segment: Wrong GPC type\n",
                                  pci_name(mcon->pci_dev));
                  return -EINVAL;
        }

        /* Export signals */
        if (gpc_conf->gpcExportGPCOutSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_GPC0_OUTPUT,
                                gpc_conf->gpcExportGPCOutSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_GPC_OUTPUT signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        return retval;
                }
        }

        mcon->gpc0_state = GPC_STATE_CONFIGURED;

        retval = pxi6259_update_gpc_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1
                                        + (PXI6259_MAX_AI_CHANNEL + 1 + 1)
                                        + (PXI6259_MAX_AO_CHANNEL + 1 + 1)
                                        + (PXI6259_MAX_DIO_PORT + 1 + 1)));
        if(retval) {
                printk(KERN_ERR "%s failed to update gpc0 devices\n", pci_name(mcon->pci_dev));
                return retval;
        }

        return 0;
}

static int pxi6259_load_gpc1_conf(struct module_context *mcon, struct pxi6259_gpc_conf *gpc_conf)
{
        int retval;

        PDEBUG("%s Loading GPC1 configuration...\n", pci_name(mcon->pci_dev));

        /* Check if segment is started */
        if (mcon->gpc1_state == GPC_STATE_STARTED) {
                printk(KERN_INFO "%s GPC1 segment is started, loading new conf!\n",
                                pci_name(mcon->pci_dev));

                //TODO release segment
                mcon->gpc1_state = GPC_STATE_UNKNOWN;
        }

        /* Save the configuration */
        mcon->gpc1_task = *gpc_conf;

        // reset counter
        gpc1_reset (mcon);

        switch (gpc_conf->gpcInfo) {
          case GPC_TASK_TYPE_SIMPLE_COUNTER:
                  // load settings
                  gpc1_simple_count_config(mcon);
                  break;

          case GPC_TASK_TYPE_SINGLE_PULSE_GENERATION:
                  // load settings
                  gpc1_single_pulse_config(mcon);
                  break;

          case GPC_TASK_TYPE_DELAY_OUTPUT_TRIGGER:
                  // load delay settings
                  gpc1_delay_input_config(mcon);
                  break;

          default:
                  printk(KERN_ERR "%s GPC1 segment: Wrong GPC type\n",
                                  pci_name(mcon->pci_dev));
                  return -EINVAL;
        }

        /* Export signals */
        if (gpc_conf->gpcExportGPCOutSig) {
                retval = pxi6259_export_signal(mcon, PXI6259_GPC1_OUTPUT,
                                gpc_conf->gpcExportGPCOutSig, 1);
                if (retval) {
                        printk(KERN_ERR "%s exporting PXI6259_GPC_OUTPUT signal failed!\n",
                                        pci_name(mcon->pci_dev));
                        return retval;
                }
        }

        mcon->gpc1_state = GPC_STATE_CONFIGURED;

        retval = pxi6259_update_gpc_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1
                                        + (PXI6259_MAX_AI_CHANNEL + 1 + 1)
                                        + (PXI6259_MAX_AO_CHANNEL + 1 + 1)
                                        + (PXI6259_MAX_DIO_PORT + 1 + 1)));
        if(retval) {
                printk(KERN_ERR "%s failed to update gpc1 devices\n", pci_name(mcon->pci_dev));
                return retval;
        }

        return 0;
}

static int pxi6259_start_gpc0(struct module_context *mcon)
{
        int retval = 0;

        PDEBUG("%s start GPC0 segment\n", pci_name(mcon->pci_dev));

        if (mcon->gpc0_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC0 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EINVAL;
        }

        if (mcon->gpc0_state == GPC_STATE_STARTED) {
                PDEBUG("%s GPC0 segment is already started!\n", pci_name(mcon->pci_dev));
                return 0;
        }

        retval = gpc0_arm(mcon);
        if (!retval) {
                mcon->gpc0_state = GPC_STATE_STARTED;
        }

        return retval;
}


static int pxi6259_start_gpc1(struct module_context *mcon)
{
        uint16_t retval = 0;

        PDEBUG("%s start GPC1 segment\n", pci_name(mcon->pci_dev));

        if (mcon->gpc1_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC1 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EINVAL;
        }

        if (mcon->gpc1_state == GPC_STATE_STARTED) {
                PDEBUG("%s GPC1 segment is already started!\n", pci_name(mcon->pci_dev));
                return 0;
        }

        retval = gpc1_arm(mcon);
        if (!retval) {
                mcon->gpc1_state = GPC_STATE_STARTED;
        }

        return retval;
}

static int pxi6259_stop_gpc0(struct module_context *mcon)
{
        PDEBUG("%s stop GPC0 segment\n", pci_name(mcon->pci_dev));

        if (mcon->gpc0_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC0 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        if (mcon->gpc0_state != GPC_STATE_STARTED) {
                printk(KERN_DEBUG "%s GPC0 segment task is not running!\n",
                                pci_name(mcon->pci_dev));
                return 0;
        }

        gpc0_disarm(mcon);
        mcon->gpc0_state = GPC_STATE_CONFIGURED;

        return 0;
}

static int pxi6259_stop_gpc1(struct module_context *mcon)
{
        PDEBUG("%s stop GPC1 segment.\n", pci_name(mcon->pci_dev));

        if (mcon->gpc1_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC0 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        if (mcon->gpc1_state != GPC_STATE_STARTED) {
                printk(KERN_NOTICE "%s GPC0 segment task is not running!\n",
                                pci_name(mcon->pci_dev));
                return 0;
        }

        gpc1_disarm(mcon);
        mcon->gpc1_state = GPC_STATE_CONFIGURED;

        return 0;
}

static int pxi6259_get_gpc0_value(struct module_context *mcon, uint32_t *gpc_value)
{
        if (mcon->gpc0_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC0 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        set_register_bits_on_off(mcon, G0_Command_Register, G_Save_Trace, 0);
        flush_register(mcon,G0_Command_Register);

        set_register_bits_on_off(mcon, G0_Command_Register, G_Save_Trace, 1);
        flush_register(mcon,G0_Command_Register);

        read_register(mcon, G0_Save_Register, gpc_value);

        return 0;
}

static int pxi6259_get_gpc1_value(struct module_context *mcon, uint32_t *gpc_value)
{
        if (mcon->gpc1_state == GPC_STATE_UNKNOWN) {
                PDEBUG("%s GPC1 segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        set_register_bits_on_off(mcon, G1_Command_Register, G_Save_Trace, 0);
        flush_register(mcon,G1_Command_Register);

        set_register_bits_on_off(mcon, G1_Command_Register, G_Save_Trace, 1);
        flush_register(mcon,G1_Command_Register);

        read_register(mcon, G1_Save_Register, gpc_value);

        return 0;
}

/*
 * DIO part
 */
static int pxi6259_load_dio_conf(struct module_context *mcon, struct pxi6259_dio_conf *dio_conf)
{
        uint16_t i = 0, retval = 0;

        PDEBUG("%s Load DIO configuration.\n", pci_name(mcon->pci_dev));

        //TODO release dio devices

        if (dio_conf->channel[0].enabled)
                write_register(mcon, DIO_Direction_Register, dio_conf->channel[0].mask);

        if (dio_conf->channel[1].enabled) {
                for (i=0; i<8; i++) {
                        retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, PXI6259_PFI0 + i, 0);
                        if (retval) {
                                printk(KERN_ERR "%s exporting PXI6259_PFI_DO signal to PFI failed!\n",
                                                pci_name(mcon->pci_dev));
                                return retval;
                        }
                        if (dio_conf->channel[1].mask & (1<<i)) {
                                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                        DIO_Pin_Dir(i), 1);
                        } else {
                                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                        DIO_Pin_Dir(i), 0);
                        }
                }
        }

        if (dio_conf->channel[2].enabled) {
                for (i=0; i<8; i++) {
                        retval = pxi6259_export_signal(mcon, PXI6259_PFI_DO, PXI6259_PFI8 + i, 0);
                        if (retval) {
                                printk(KERN_ERR "%s exporting PXI6259_PFI_DO signal to PFI failed!\n",
                                                pci_name(mcon->pci_dev));
                                return retval;
                        }
                        if (dio_conf->channel[2].mask & (1<<i)) {
                                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                        DIO_Pin_Dir(i+8), 1);
                        } else {
                                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                        DIO_Pin_Dir(i+8), 0);
                        }
                }
        }
        flush_register(mcon,IO_Bidirection_Pin_Register);

        /* This registers are never cleared automatically */
        write_register(mcon, Static_Digital_Output_Register, 0);
        write_register(mcon, PFI_DO_Register, 0);

        mcon->dio_task = *dio_conf;
        mcon->dio_state = DIO_STATE_CONFIGURED;

        retval = pxi6259_update_dio_devices(mcon,
                        MKDEV(device_major, mcon->index * DEVICE_MINOR_STEP + 1
                                        + (PXI6259_MAX_AI_CHANNEL + 1 + 1) + (PXI6259_MAX_AO_CHANNEL + 1 + 1)));
        if (retval) return retval;

        PDEBUG("%s loading DIO configuration done.\n", pci_name(mcon->pci_dev));

        return 0;
}

/**
 * WARNING: this function copies data directly into user space!
 */
static int pxi6259_read_dio_port(struct module_context *mcon,
                dio_port_select_t port, void *to)
{
        uint16_t dio_value;
        uint32_t port_value;
        int retval;

        if (mcon->dio_state == DIO_STATE_UNKNOWN) {
                PDEBUG("%s DIO segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        switch (port) {
          case DIO_PORT_SELECT_PORT0:
                  read_register(mcon,Static_Digital_Input_Register, &port_value);
                  break;

          case DIO_PORT_SELECT_PORT1:
                  read_register(mcon,PFI_DI_Register, &dio_value);
                  port_value = dio_value & 0xFF;
                  break;

          case DIO_PORT_SELECT_PORT2:
                  read_register(mcon,PFI_DI_Register, &dio_value);
                  port_value = (dio_value >> 8) & 0xFF;
                  break;

          default:
                  return -EINVAL;
        }

        retval = copy_to_user(to, &port_value, sizeof(port_value));
        return retval;
}

/**
 * WARNING: this function copies data directly from user space.
 */
static int pxi6259_write_to_dio_port (struct module_context *mcon,
                dio_port_select_t port, void *from)
{
        uint32_t port_value;
        int retval;

        if (mcon->dio_state == DIO_STATE_UNKNOWN) {
                PDEBUG("%s DIO segment is not configured!\n", pci_name(mcon->pci_dev));
                return -EIO;
        }

        retval = copy_from_user((void *)&port_value, from, sizeof(port_value));
        if (retval) return retval;

        switch (port) {
          case DIO_PORT_SELECT_PORT0 :
                write_register(mcon,Static_Digital_Output_Register,port_value);
                break;
          case DIO_PORT_SELECT_PORT1 :
                set_register_bits(mcon, PFI_DO_Register,
                                mcon->dio_task.channel[1].mask,
                                port_value & mcon->dio_task.channel[1].mask);
                flush_register(mcon,PFI_DO_Register);
                break;
          case DIO_PORT_SELECT_PORT2 :
                set_register_bits(mcon, PFI_DO_Register,
                                (mcon->dio_task.channel[2].mask << 8),
                                ((port_value & mcon->dio_task.channel[2].mask) << 8));
                flush_register(mcon,PFI_DO_Register);
                break;
          default:
               return -EINVAL;
        }

        return 0;
}

/*
 * Scaling TODO this part need to be refactored
 */
inline static void pxi6259_get_float_from_EEPROM(uint8_t *eeprom_mem, uint32_t offset, uint8_t *ptr)
{
        ptr[3] = eeprom_mem[offset++];
        ptr[2] = eeprom_mem[offset++];
        ptr[1] = eeprom_mem[offset++];
        ptr[0] = eeprom_mem[offset++];
}

inline static uint32_t pxi6259_get_calibration_area_offset(uint8_t *eeprom_mem)
{
        return (eeprom_mem[CALIBRATION_AREA_OFFSET] << 8) | eeprom_mem[CALIBRATION_AREA_OFFSET+1];
}

/*
 * pxi6259_ai_get_scaling_coeficient
 *
 * modeIDx
 *      0 -> default
 *
 * intervalIDx
 *      0 -> +/- 10V
 *      1 -> +/- 5V
 *      2 -> +/- 2V
 *      3 -> +/- 1V
 *      4 -> +/- 500mV
 *      5 -> +/- 200mV
 *      6 -> +/- 100mV
 *
 * channel
 *      ignored - all channels use the same ADC
 */
static int pxi6259_ai_get_scaling_coeficient(struct module_context *mcon, uint32_t intervalIDx, uint32_t modeIDx, ai_scaling_coefficients_t *scaling)
{
        uint8_t* eeprom_mem = mcon->cal_eeprom;
        uint32_t cal_offset = 0;
        uint32_t mode_offset = 0;
        uint32_t interval_offset = 0;

        PDEBUG("%s Getting AI Scaling Coeficient\n",pci_name(mcon->pci_dev));

        cal_offset = pxi6259_get_calibration_area_offset(eeprom_mem);

        mode_offset = cal_offset + AI_MODE_BLOCK_START + (modeIDx*AI_MODE_BLOCK_SIZE);

        PDEBUG(KERN_ERR "%s modeOffset: %d\n",pci_name(mcon->pci_dev),mode_offset);

        scaling->mode.order = eeprom_mem[mode_offset++];
        pxi6259_get_float_from_EEPROM (eeprom_mem, mode_offset, scaling->mode.value[0].b);
        mode_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, mode_offset, scaling->mode.value[1].b);
        mode_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, mode_offset, scaling->mode.value[2].b);
        mode_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, mode_offset, scaling->mode.value[3].b);

        interval_offset = cal_offset + AI_INTERVAL_BLOCK_START + (intervalIDx*AI_INTERVAL_BLOCK_SIZE);
        PDEBUG(KERN_ERR "%s intervalOffset: %d\n",pci_name(mcon->pci_dev),interval_offset);

        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval.gain.b);
        interval_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval.offset.b);

        scaling->order = scaling->mode.order;

        PDEBUG(KERN_ERR "%s %x %x %x %x\n",pci_name(mcon->pci_dev),scaling->mode.value[2].b[0],
                        scaling->mode.value[2].b[1],scaling->mode.value[2].b[2],scaling->mode.value[2].b[3]);

        return 0;
}

/*
 * modeIdx
 *      ignored - AO does not use the mode constants
 *
 * intervalIdx
 *      0 -> 10V reference
 *      1 -> 5V reference
 *      2 -> 2V reference
 *      3 -> 1V reference
 *
 * channel
 *      dac number
 */
static int pxi6259_ao_get_scaling_coeficient(struct module_context *mcon, uint32_t intervalIDx, uint32_t modeIDx, ao_scaling_coefficients_t *scaling)
{
        uint8_t* eeprom_mem = mcon->cal_eeprom;
        uint32_t cal_offset = 0;
        uint32_t interval_offset = 0;

        PDEBUG("%s Getting AO Scaling Coeficient\n",pci_name(mcon->pci_dev));

        cal_offset = pxi6259_get_calibration_area_offset(eeprom_mem);

        /* Get coefficients for AO0 */
        interval_offset = cal_offset + AO_INTERVAL_BLOCK_START + (intervalIDx*AO_INTERVAL_BLOCK_SIZE) +
                        (SCALE_CONVERTER_SELECT_AO0*AO_CHANNEL_BLOCK_SIZE);
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[0].gain.b);
        interval_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[0].offset.b);
        scaling->order[0] = 1;

        /* Get coefficients for AO1 */
        interval_offset = cal_offset + AO_INTERVAL_BLOCK_START + (intervalIDx*AO_INTERVAL_BLOCK_SIZE) +
                        (SCALE_CONVERTER_SELECT_AO1*AO_CHANNEL_BLOCK_SIZE);
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[1].gain.b);
        interval_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[1].offset.b);
        scaling->order[1] = 1;

        /* Get coefficients for AO2 */
        interval_offset = cal_offset + AO_INTERVAL_BLOCK_START + (intervalIDx*AO_INTERVAL_BLOCK_SIZE) +
                        (SCALE_CONVERTER_SELECT_AO2*AO_CHANNEL_BLOCK_SIZE);
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[2].gain.b);
        interval_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[2].offset.b);
        scaling->order[2] = 1;

        /* get coefficients for AO3 */
        interval_offset = cal_offset + AO_INTERVAL_BLOCK_START + (intervalIDx*AO_INTERVAL_BLOCK_SIZE) +
                        (SCALE_CONVERTER_SELECT_AO3*AO_CHANNEL_BLOCK_SIZE);
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[3].gain.b);
        interval_offset += 4;
        pxi6259_get_float_from_EEPROM (eeprom_mem, interval_offset, scaling->interval[3].offset.b);
        scaling->order[3] = 1;

        return 0;
}

/*
 * Read serial number from eeprom
 */
static int pxi6259_serial_number_read(struct module_context *mcon, uint32_t *serial_number)
{
        uint8_t partial_val0;
        uint8_t partial_val1;
        uint8_t partial_val2;
        uint8_t partial_val3;

        PDEBUG(KERN_ERR "%s read serial number\n",pci_name(mcon->pci_dev));

        readFromEeprom(mcon, 4, (uint8_t *)serial_number, 4);

        partial_val0 = (uint8_t)*serial_number;
        partial_val1 = (uint8_t)(*serial_number >> 8);
        partial_val2 = (uint8_t)(*serial_number >> 16);
        partial_val3 = (uint8_t)(*serial_number >> 24);

        /* Serial number is returned as big-endian u32 */
        *serial_number = (partial_val0 << 24) | (partial_val1 << 16) | (partial_val2 << 8) | partial_val3;

        return 0;
}

/*
 * Exporting signals
 */
static int pxi6259_export_signal(struct module_context *mcon,
                    enum pxi6259_signal source, enum pxi6259_terminal terminal, uint8_t enable)
{
        pfi_output_select_t             pfi_out_sel = PFI_DEFAULT_OUT;
        rtsi_output_select_t            rtsi_out_sel = RTSI_OUTPUT_SELECT_ADR_START1;
        rtsi_shared_mux_output_select_t rtsi_shar_mux_out_sel = RTSI_SHARED_MUX_OUTPUT_SELECT_PFI0;

        uint8_t is_rtsi_terminal = (terminal < PXI6259_PFI_TERMINAL);

        printk(KERN_ERR "export signal: source %d terminal %d enable %d\n",source,terminal, enable);

        /* Configure RTSI and PFI according to the source signal */
        switch (source) {
          case PXI6259_PFI_DO:
                pfi_out_sel = PFI_DO;
                is_rtsi_terminal = 0;
                break;

          case PXI6259_AI_SAMPLE_CLOCK:
                pfi_out_sel = AI_START_PULSE;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_RTSI_BRD_0;
                rtsi_shar_mux_out_sel = RTSI_SHARED_MUX_OUTPUT_SELECT_AI_START_PULSE;
                break;

          case PXI6259_AI_CONVERT_CLOCK:
                pfi_out_sel = AI_CONVERT;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_SCLKG;
                break;

          case PXI6259_AI_START_TRIGGER:
                pfi_out_sel = AI_START1_PFI;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_ADR_START1;
                break;

          case PXI6259_AI_REFERENCE_TRIGGER:
                pfi_out_sel = AI_START2_PFI;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_ADR_START2;
                break;

          case PXI6259_AO_SAMPLE_CLOCK:
                pfi_out_sel = AO_UPDATE_N;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_DACUPDN;
                break;

          case PXI6259_AO_START_TRIGGER:
                pfi_out_sel = AO_START1_PFI;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_DA_START1;
                break;

          case PXI6259_GPC0_OUTPUT:
                pfi_out_sel = G0_OUT;
                break;

          case PXI6259_GPC1_OUTPUT:
                pfi_out_sel = G1_OUT;
                rtsi_out_sel = RTSI_OUTPUT_SELECT_RTSI_BRD_1;
                rtsi_shar_mux_out_sel = RTSI_SHARED_MUX_OUTPUT_SELECT_G1_OUT;
                break;

          default:
                return -EINVAL;
        }

        /* Commit to hardware */
        switch (terminal) {
          case PXI6259_RTSI0:
          case PXI6259_RTSI1:
          case PXI6259_RTSI2:
          case PXI6259_RTSI3:
                set_register_bits(mcon, RTSI_Trig_A_Output_Register,
                                RTSI_Trig_Output_Mask(terminal - PXI6259_RTSI0),
                                RTSI_Trig_Output(terminal - PXI6259_RTSI0, rtsi_out_sel));
                set_register_bits_on_off(mcon, RTSI_Trig_Direction_Register,
                                RTSI_Pin_Dir(terminal - PXI6259_RTSI0), enable);
                flush_register(mcon, RTSI_Trig_A_Output_Register);
                flush_register(mcon, RTSI_Trig_Direction_Register);
                break;
          case PXI6259_RTSI4:
          case PXI6259_RTSI5:
          case PXI6259_RTSI6:
          case PXI6259_RTSI7:
                set_register_bits(mcon, RTSI_Trig_B_Output_Register,
                                RTSI_Trig_Output_Mask(terminal - PXI6259_RTSI0),
                                RTSI_Trig_Output(terminal - PXI6259_RTSI0, rtsi_out_sel));
                set_register_bits_on_off(mcon, RTSI_Trig_Direction_Register,
                                RTSI_Pin_Dir(terminal - PXI6259_RTSI0), enable);
                flush_register(mcon, RTSI_Trig_B_Output_Register);
                flush_register(mcon, RTSI_Trig_Direction_Register);
                break;

          case PXI6259_PFI0:
          case PXI6259_PFI1:
          case PXI6259_PFI2:
                set_register_bits(mcon, PFI_Output_Select_1_Register,
                                PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                printk(KERN_ERR "pfi 0-2 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                flush_register(mcon, PFI_Output_Select_1_Register);
                flush_register(mcon, IO_Bidirection_Pin_Register);
                break;
          case PXI6259_PFI3:
          case PXI6259_PFI4:
          case PXI6259_PFI5:
                 set_register_bits(mcon, PFI_Output_Select_2_Register,
                                 PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                 PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                 printk(KERN_ERR "pfi 3-5 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 flush_register(mcon, PFI_Output_Select_2_Register);
                 flush_register(mcon, IO_Bidirection_Pin_Register);
                 break;
          case PXI6259_PFI6:
          case PXI6259_PFI7:
          case PXI6259_PFI8:
                set_register_bits(mcon, PFI_Output_Select_3_Register,
                                PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                printk(KERN_ERR "pfi 6-8 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                flush_register(mcon, PFI_Output_Select_3_Register);
                flush_register(mcon, IO_Bidirection_Pin_Register);
                break;
          case PXI6259_PFI9:
          case PXI6259_PFI10:
          case PXI6259_PFI11:
                 set_register_bits(mcon, PFI_Output_Select_4_Register,
                                 PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                 PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                 printk(KERN_ERR "pfi 9-11 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 flush_register(mcon, PFI_Output_Select_4_Register);
                 flush_register(mcon, IO_Bidirection_Pin_Register);
                 break;
          case PXI6259_PFI12:
          case PXI6259_PFI13:
          case PXI6259_PFI14:
                 set_register_bits(mcon, PFI_Output_Select_5_Register,
                                 PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                 PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                 printk(KERN_ERR "pfi 12-14 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 flush_register(mcon, PFI_Output_Select_5_Register);
                 flush_register(mcon, IO_Bidirection_Pin_Register);
                 break;
          case PXI6259_PFI15:
                 set_register_bits(mcon, PFI_Output_Select_6_Register,
                                 PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 set_register_bits_on_off(mcon, IO_Bidirection_Pin_Register,
                                 PFI_Pin_Dir(terminal - PXI6259_PFI0), enable);
                 printk(KERN_ERR "pfi 15 export mask: %d value %d\n",PFI_Output_Select_Mask(terminal - PXI6259_PFI0),
                                 PFI_Output_Select(terminal - PXI6259_PFI0, pfi_out_sel));
                 flush_register(mcon, PFI_Output_Select_6_Register);
                 flush_register(mcon, IO_Bidirection_Pin_Register);
                 break;

          default:
                return -EINVAL;
        }

        /* If this is a RTSI terminal, check if the RTSI MUX need to be set */
        if (is_rtsi_terminal) {
                switch (rtsi_out_sel)
                {
                  case RTSI_OUTPUT_SELECT_RTSI_BRD_0:
                  case RTSI_OUTPUT_SELECT_RTSI_BRD_1:
                  case RTSI_OUTPUT_SELECT_RTSI_BRD_2:
                  case RTSI_OUTPUT_SELECT_RTSI_BRD_3:
                          set_register_bits(mcon, RTSI_Shared_MUX_Register,
                                          RTSI_Shared_MUX_Output_Select_Mask(
                                                          rtsi_out_sel - RTSI_OUTPUT_SELECT_RTSI_BRD_0),
                                          RTSI_Shared_MUX_Output_Select(
                                                          rtsi_out_sel - RTSI_OUTPUT_SELECT_RTSI_BRD_0,
                                                          rtsi_shar_mux_out_sel));
                          flush_register(mcon, RTSI_Shared_MUX_Register);
                          break;
                  default: /* Do nothing */
                        break;
                }
        }

        PDEBUG("%s signal %d exported to terminal %d.\n",
                        pci_name(mcon->pci_dev), source, terminal);

        return 0;
}

static int pxi6259_connect_trigger(struct module_context *mcon,
                struct pxi6259_trigger_connection *trig)
{
        return -EINVAL;
}

static int pxi6259_disconnect_trigger(struct module_context *mcon,
                struct pxi6259_trigger_connection *trig)
{
        return -EINVAL;
}

static int pxi6259_raw_io(struct module_context *mcon, struct pxi6259_raw_io_block *riob,
                  uint32_t flag) /* Read=0,Write=1 flag */
{
        int retval;
        void *mmap;
        u8 u8data; /* Users IO location */
        u16 u16data;
        u32 u32data;

        if (riob->bar < 0
                        || riob->bar > (PXI6259_ADDRESS_SPACES - 1)) {
                printk(KERN_WARNING "wrong address space selected %d \n",
                                riob->bar);
                return -EINVAL;
        }

        mmap = mcon->addresses.spaces[riob->bar].space;
        if (!mmap) {
                printk(KERN_WARNING "address space %d not maped \n",
                                riob->bar);
                return -EINVAL;
        }

        if (riob->offset >= (mcon->addresses.spaces[riob->bar].window
                        - riob->dw)) {
                printk(KERN_WARNING "address offset %d too big \n",
                                                riob->offset);
                return -EINVAL;
        }

        /* Add offset */
        mmap = (void *) (mmap + riob->offset);

        if (flag) {
                switch (riob->dw) {
                  case PXI6259_DW08:
                       retval = copy_from_user(&u8data, riob->data, riob->dw);
                       if (retval) {
                               return retval;
                       }
                       iowrite8((u8) u8data, mmap);
                       break;
                  case PXI6259_DW16:
                       retval = copy_from_user(&u16data, riob->data, riob->dw);
                       if (retval) {
                               return retval;
                       }
                       iowrite16((u16) u16data, mmap);
                       break;
                  case PXI6259_DW32:
                       retval = copy_from_user(&u32data, riob->data, riob->dw);
                       if (retval) {
                               return retval;
                       }
                       iowrite32((u32) u32data, mmap);
                       break;
                  default:
                       retval = -ERESTARTSYS;
                       break;
                }
        } else {
                switch (riob->dw) {
                  case PXI6259_DW08:
                          u8data = ioread8(mmap);
                          retval = copy_to_user(riob->data, &u8data, riob->dw);
                          if (retval) {
                                  return retval;
                          }
                          break;
                  case PXI6259_DW16:
                          u16data = ioread16(mmap);
                          retval = copy_to_user(riob->data, &u16data, riob->dw);
                          if (retval) {
                                  return retval;
                          }
                          break;

                  case PXI6259_DW32:
                          u32data = ioread32(mmap);
                          retval = copy_to_user(riob->data, &u32data, riob->dw);
                          if (retval) {
                                  return retval;
                          }
                          break;

                  default:
                          retval = -ERESTARTSYS;
                          break;
                }
        }

   return retval;
}

/*
 * The other operations for the device come from the bare device
 */
struct file_operations pxi6259_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =  pxi6259_ioctl,
        .open =     pxi6259_open,
        .release =  NULL,
};

long pxi6259_ai_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct module_context *mcon = (struct module_context *)filp->private_data;
        struct pxi6259_ai_conf ai_conf;
        uint32_t num_pulses;

        ai_scaling_coefficients_t aiScalingCoefficient;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->ai_mutex);

        switch(cmd) {
          /* ANALOG INPUT SEGMENT IOCTL CALLS */
          case PXI6259_IOC_LOAD_AI_CONF:
                  rc = copy_from_user((void *) &ai_conf, (void *)arg, sizeof(ai_conf));
                  if (rc) break;

                  rc = pxi6259_load_ai_conf(mcon, &ai_conf);
                  break;

          case PXI6259_IOC_READ_AI_CONF:
                  if (mcon->ai_state == AI_STATE_UNKNOWN) {
                          rc = -EINVAL;
                  } else {
                          rc = copy_to_user((void *)arg, (void *)&mcon->ai_conf, sizeof(mcon->ai_conf));
                  }
                  break;

          case PXI6259_IOC_START_AI:
                  rc = pxi6259_start_ai(mcon);
                  break;

          case PXI6259_IOC_STOP_AI:
                  rc = pxi6259_stop_ai(mcon);
                  break;

          case PXI6259_IOC_GENERATE_PULSE:
                  rc = copy_from_user((void *) &num_pulses, (void *)arg, sizeof(num_pulses));
                  if (rc){
                          rc = -EINVAL;
                          break;
                  }
                  rc = pxi6259_ai_sample_on_demand(mcon, num_pulses);
                  break;

          case PXI6259_IOC_GET_AI_SCALING_COEF:
                  rc = pxi6259_ai_get_scaling_coeficient(mcon, 0, 0, &aiScalingCoefficient);
                  if (rc == 0) {
                      rc = copy_to_user((void *)arg, (void *)&aiScalingCoefficient, sizeof(aiScalingCoefficient));
                  }

                  break;

          case PXI6259_IOC_RESET_AI:
                  rc=pxi6259_reset_ai(mcon);
                  break;

          case PXI6259_IOC_GET_DEVICE_ID:
                  rc = copy_to_user((void *)arg, &mcon->index, sizeof(u32));
                  break;

          default:  /* redundant, as cmd was checked against MAXNR */
                  rc = -ENOTTY;
        }

        mutex_unlock(&mcon->ai_mutex);
        return rc;
}

static int pxi6259_ai_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon = container_of(inode->i_cdev, struct module_context, aicdev);
        filp->private_data = mcon;      /* for other methods */

        PDEBUG("%s AI open\n", pci_name(mcon->pci_dev));

        return 0;                       /* success */
}

/* Everybody can open and release file descriptor */
struct file_operations pxi6259_ai_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =    pxi6259_ai_ioctl,
        .open =     pxi6259_ai_open,
        .release =  NULL,
};

static long pxi6259_aichan_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct aichan_listitem *aichan_item = ( struct aichan_listitem *)filp->private_data;
        struct module_context *mcon = aichan_item->mcon;

        ai_scaling_coefficients_t aiScalingCoefficient;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->ai_mutex);

        switch(cmd) {
          /* ANALOG INPUT SEGMENT IOCTL CALLS */

          case PXI6259_IOC_READ_AI_CONF:
                  if (mcon->ai_state == AI_STATE_UNKNOWN) {
                          rc = -EINVAL;
                  } else {
                          rc = copy_to_user((void *)arg, (void *)&mcon->ai_conf, sizeof(mcon->ai_conf));
                  }
                  break;
          case PXI6259_IOC_GET_AI_SCALING_COEF:
                  rc = pxi6259_ai_get_scaling_coeficient(mcon, 0, 0, &aiScalingCoefficient);
                  if (rc == 0) {
                          rc = copy_to_user((void *) arg, (void *) &aiScalingCoefficient, sizeof(aiScalingCoefficient));
                  }

                  break;

          default:  /* redundant, as cmd was checked against MAXNR */
                  rc = -ENOTTY;
                  break;
        }

        mutex_unlock(&mcon->ai_mutex);
        return rc;
}

/*
 * Read function.
 *
 * This function reads samples from the AI channel.
 *
 * This function reads samples only and only from the coherent
 * DMA buffer and it is also assumed that the configuration will not change
 * until at least one client holds the reference to the channel file descriptor.
 * Because of this assumptions there is no need for locking. This also means that
 * the DMA buffer is at least configured if not started.

 */
static ssize_t pxi6259_aichan_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
        struct aichan_listitem *aichan_item = ( struct aichan_listitem *)filp->private_data;
        struct module_context *mcon = aichan_item->mcon;
        struct aichan_client *cli= &aichan_item->cli;

        ssize_t retval = 0;
        u32 num_bytes, num_samples;
        u32 chan_samples, read_bytes;

        while (bytes_in_dma_channel(mcon, &mcon->ai_chan, &cli->ai_dma_info)
                        < AI_CHAN_MITE_DATA_WIDTH) {

                if (filp->f_flags & O_NONBLOCK) {
                        return -EAGAIN;
                }

                if (wait_event_interruptible(cli->wait_queue,
                                bytes_in_dma_channel(mcon, &mcon->ai_chan, &cli->ai_dma_info)
                                >= AI_CHAN_MITE_DATA_WIDTH)) {
                        return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
                }

        }

        num_bytes = bytes_in_dma_channel(mcon, &mcon->ai_chan, &cli->ai_dma_info);
        /* If bytes transfered exceed the buffer size, DMA buffer overflowed. */
        if (num_bytes > mcon->ai_chan.size) {
                // Bug 2979: Requesting a read of more than the channel size.
                printk(KERN_ERR "%s DMA buffer overflow, cannot read data for chan %d a:[%d > %d]\n",
                                pci_name(mcon->pci_dev), aichan_item->chan_number, num_bytes,  mcon->ai_chan.size);

                printk(KERN_ERR "       read_idx %llu write_idx %llu last_write_idx %llu wait %d location %llu\n",
                                cli->ai_dma_info.read_idx, cli->ai_dma_info.write_idx,
                                cli->ai_dma_info.last_write_idx, cli->ai_dma_info.wait_write_idx, cli->ai_dma_info.location);

                // Reset read_idx to current sample position - see bug 2286
                cli->ai_dma_info.read_idx = cli->ai_dma_info.last_write_idx;

                retval = -EOVERFLOW;
                goto out;
        }

        /* Calculate how many samples can be read from DMA buffer */
        num_samples = num_bytes >> 1;
        /* The first index is always chan_number */
        chan_samples = num_samples/aichan_item->chan_total
                        + (num_samples % aichan_item->chan_total > 0 ? 1 : 0);

      //  printk(" bytes %d num_samples %d count %d\n", num_bytes, num_samples, count);
        if (chan_samples > count>>1)
                chan_samples = count>>1;

        /* Read samples */
        retval = read_ai_chanel_from_linear_dma_buffer(mcon, &cli->ai_dma_info,
                        chan_samples, aichan_item->chan_total, buf);
        if (retval) goto out;

        //printk("%s read %d samples from linear DMA buffer for channel %d\n",
        //                pci_name(mcon->pci_dev), chan_samples, aichan_item->chan_number);

        /* Check for overflow again. Note that the read index is not updated.
         * Check for an overflow while moving the data out of the DMA buffer.
         */
        num_bytes = bytes_in_dma_channel(mcon, &mcon->ai_chan, &cli->ai_dma_info);
        if (num_bytes > mcon->ai_chan.size) {
                printk(KERN_ERR "%s DMA buffer overflow during read for channel %d b:[%d > %d]\n",
                                pci_name(mcon->pci_dev), aichan_item->chan_number, num_bytes, mcon->ai_chan.size);
                printk(KERN_ERR "       read_idx %llu write_idx %llu last_write_idx %llu wait %d location %llu",
                                cli->ai_dma_info.read_idx, cli->ai_dma_info.write_idx,
                                cli->ai_dma_info.last_write_idx, cli->ai_dma_info.wait_write_idx,
                        cli->ai_dma_info.location);

                // Reset read_idx to current sample position - see bug 2286
                cli->ai_dma_info.read_idx = cli->ai_dma_info.last_write_idx;

                retval = -EOVERFLOW;
                goto out;
        }

        /* No overflow during data move - update read index. */
        read_bytes = chan_samples * (aichan_item->chan_total<<1);
        if (num_bytes < read_bytes)
                cli->ai_dma_info.wait_write_idx = 1;

        cli->ai_dma_info.read_idx += read_bytes;
        count = (chan_samples << 1);
        *f_pos += count;
        retval = count;

  out:
        return retval;
}

static int pxi6259_aichan_open(struct inode *inode, struct file *filp)
{
        int minor = MINOR(inode->i_cdev->dev);
        struct module_context *mcon;
        struct aichan_listitem *lptr;
        unsigned int chan_number = 0, chan_total, dma_idx;
        int i;

        /* AICHAN minor devices are 2,3,...,33 */
        int arr_offset = (minor % DEVICE_MINOR_STEP) - 2;

        mcon = container_of_array(inode->i_cdev,
                        struct module_context, aichancdev, arr_offset, sizeof(struct cdev));

        PDEBUG("%s AI channel open\n", pci_name(mcon->pci_dev) );

        /* Create new AI client */
        /* Allocate new aichan context */
        lptr = kmalloc(sizeof(struct aichan_listitem), GFP_KERNEL);
        if (!lptr) {
                return -ENOMEM;
        }
        memset(lptr, 0, sizeof(struct aichan_listitem));

        mutex_lock(&mcon->ai_mutex);

        /* Find out which channel it belongs to */
        for (i=0, chan_total=0, dma_idx=0; i<=PXI6259_MAX_AI_CHANNEL; i++) {
                if (&mcon->aichancdev[i] == inode->i_cdev) {
                        chan_number = i;
                        dma_idx = chan_total;
                }
                if (mcon->aichandev[i] != NULL)
                        chan_total++;
        }

        /* Save keys */
        lptr->key = lptr;
        lptr->cli.mcon = mcon;
        lptr->pid = get_current()->pid;
        lptr->mcon = mcon;
        lptr->chan_number = chan_number;
        lptr->chan_total = chan_total;
        lptr->dma_idx = dma_idx;

        /* Initialize wait queue */
        init_waitqueue_head(&lptr->cli.wait_queue);

        /* Set DMA buffer initial location */
        lptr->cli.ai_dma_info.location = dma_idx * AI_CHAN_MITE_DATA_WIDTH;
        lptr->cli.ai_dma_info.read_idx = dma_idx * AI_CHAN_MITE_DATA_WIDTH;
        lptr->cli.ai_dma_info.wait_write_idx = 1;

        PDEBUG("%s aichan opened [pid %d mconidx %d aichan %d dma_idx %d]\n",
                        pci_name(mcon->pci_dev), lptr->pid, mcon->index, lptr->chan_number, lptr->dma_idx);
        mcon->aichanclients++;

        /* Add it to the list */
        list_add(&lptr->list, &mcon->aichan_list);
        mutex_unlock(&mcon->ai_mutex);

        /* for other methods */
        filp->private_data = lptr;

        return 0;
}

/*
 * Delete aichan context.
 */
static int pxi6259_aichan_release(struct inode *inode, struct file *filp)
{
        struct aichan_listitem *aichan_item = ( struct aichan_listitem *)filp->private_data;
        struct module_context *mcon = aichan_item->mcon;
        struct aichan_listitem *lptr, *next;

        mutex_lock(&mcon->ai_mutex);

        list_for_each_entry_safe(lptr, next, &mcon->aichan_list, list) {
                if (lptr->key == aichan_item) {
                        list_del(&lptr->list);
                        mcon->aichanclients--;
                        PDEBUG("%s aichan released (pid %d mconidx %d aichan %d)\n",
                                        pci_name(mcon->pci_dev), aichan_item->pid,
                                        mcon->index, aichan_item->chan_number);
                        kfree(lptr);
                }
        }

        mutex_unlock(&mcon->ai_mutex);

        return 0;
}

static unsigned int pxi6259_aichan_poll(struct file *filp, poll_table *wait)
{
        struct aichan_listitem *aichan_item = ( struct aichan_listitem *)filp->private_data;
        struct module_context *mcon = aichan_item->mcon;
        struct aichan_client *cli= &aichan_item->cli;

        unsigned int mask = 0;
        u32 num_bytes;

        /* Check if channel still valid */
        if (mcon->aichandev[aichan_item->chan_number] == NULL) {
                mask = POLLERR;
                goto out;
        }

        /* Loop until there is something to read */
        num_bytes = bytes_in_dma_channel(mcon, &mcon->ai_chan, &cli->ai_dma_info);
        if (num_bytes > mcon->ai_chan.size) {
                printk(KERN_ERR "%s DMA buffer overflow, cannot read data for chan %d c:[%d > %d]\n",
                                pci_name(mcon->pci_dev), aichan_item->chan_number, num_bytes, mcon->ai_chan.size);
                mask = POLLERR;
                goto out;
        } else {
                mask = POLLIN | POLLRDNORM;
        }

    out:

        return mask;
}

struct file_operations pxi6259_aichan_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =   pxi6259_aichan_ioctl,
        .read =     pxi6259_aichan_read,
        .open =     pxi6259_aichan_open,
        .release =  pxi6259_aichan_release,
        .poll =     pxi6259_aichan_poll,
};

long pxi6259_ao_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct module_context *mcon = (struct module_context *)filp->private_data;
        struct pxi6259_ao_conf ao_task;
        ao_scaling_coefficients_t aoScalingCoefficient;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->ao_mutex);

        switch(cmd) {
                /* ANALOG OUTPUT SEGMENT IOCTL CALLS */
          case PXI6259_IOC_LOAD_AO_CONF:
                rc = copy_from_user((void *)&ao_task, (void *)arg, sizeof(ao_task));
                if (rc) {
                        rc = -EINVAL;
                        break;
                }
                rc = pxi6259_load_ao_conf(mcon, &ao_task);
                break;

          case PXI6259_IOC_READ_AO_CONF:
                if (mcon->ao_state == AO_STATE_UNKNOWN)
                        rc = -EINVAL;
                else
                        rc = copy_to_user((void *)arg, &mcon->ao_task, sizeof(mcon->ao_task));
                break;

          case PXI6259_IOC_START_AO:
                rc = pxi6259_start_ao(mcon);
                break;

          case PXI6259_IOC_STOP_AO:
                rc = pxi6259_stop_ao(mcon);
                break;

          case PXI6259_IOC_GET_AO_SCALING_COEF:

                rc = pxi6259_ao_get_scaling_coeficient(mcon,0,0,&aoScalingCoefficient);
                if (rc == 0) {
                          rc = copy_to_user((void *)arg, (void *)&aoScalingCoefficient, sizeof(aoScalingCoefficient));
                }

                break;

          case PXI6259_IOC_RESET_AO:
                rc = pxi6259_reset_ao(mcon);
                break;

          case PXI6259_IOC_GET_DEVICE_ID:
                rc = copy_to_user((void *)arg, &mcon->index, sizeof(u32));
                break;

          default:  /* redundant, as cmd was checked against MAXNR */
                rc = -ENOTTY;
        }

        mutex_unlock(&mcon->ao_mutex);
        return rc;
}

static int pxi6259_ao_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon = container_of(inode->i_cdev, struct module_context, aocdev);
        filp->private_data = mcon;      /* for other methods */

        PDEBUG("%s AO open\n", pci_name(mcon->pci_dev));

        return 0;                       /* success */
}

/* Everybody can open and release file descriptor */
struct file_operations pxi6259_ao_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =    pxi6259_ao_ioctl,
        .open =     pxi6259_ao_open,
        .release =  NULL,
};

long pxi6259_aochan_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct aochan_listitem *aochan_item = (struct aochan_listitem *)filp->private_data;
        struct module_context *mcon = aochan_item->mcon;
        ao_scaling_coefficients_t aoScalingCoefficient;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->ao_mutex);

        switch(cmd) {
                /* ANALOG OUTPUT SEGMENT IOCTL CALLS */
          case PXI6259_IOC_READ_AO_CONF:
                if (mcon->ao_state == AO_STATE_UNKNOWN)
                        rc = -EINVAL;
                else
                        rc = copy_to_user((void *)arg, &mcon->ao_task, sizeof(mcon->ao_task));
                break;

          case PXI6259_IOC_GET_AO_SCALING_COEF:

                rc = pxi6259_ao_get_scaling_coeficient(mcon,0,0,&aoScalingCoefficient);
                if (rc == 0) {
                        rc = copy_to_user((void *)arg, (void *)&aoScalingCoefficient, sizeof(aoScalingCoefficient));
                }

                break;

          default:  /* redundant, as cmd was checked against MAXNR */
                rc = -ENOTTY;
        }

        mutex_unlock(&mcon->ao_mutex);
        return rc;
}

inline static int bytes_free_or_error(struct module_context *mcon, unsigned int chan_number)
{
        u32 bytes_in_buffer, bytes_free;

        bytes_in_buffer = bytes_in_dma_channel(mcon, &mcon->ao_chan, &mcon->ao_cli_info[chan_number]);
        bytes_free = mcon->ao_chan.size - bytes_in_buffer;
        if (bytes_in_buffer > mcon->ao_chan.size) {
                printk(KERN_ERR "%s DMA buffer underflow at channel %d bytes_in_buffer: %d > %d!\n", pci_name(mcon->pci_dev), chan_number,bytes_in_buffer, mcon->ao_chan.size);
                return -EIO;
        }

        return bytes_free;
}


/* FIXME locking is not OK */
ssize_t pxi6259_aochan_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
        struct aochan_listitem *aochan_item = (struct aochan_listitem *)filp->private_data;
        struct module_context *mcon = aochan_item->mcon;
        unsigned int chan_number = aochan_item->chan_number;
        unsigned int chan_total = aochan_item->chan_total;
        struct aochan_client *cli = &aochan_item->cli;
        ssize_t retval = 0;
        u16 sample, num_samples,i;
        int bytes_free, samples_in_buffer;

        mutex_lock(&mcon->ao_mutex);

        /* Check if channel still valid */
        if (mcon->aochandev[chan_number] == NULL) {
                retval = -EBADFD;
                goto out;
        }

        /* pxi6259_write only one value to the channel*/
        if (mcon->ao_task.signalGeneration == AO_SIGNAL_GENERATION_STATIC) {
                if (count < sizeof(sample)) {
                        retval = 0;
                        goto out;
                }
                count = sizeof(sample);
                if (copy_from_user(&sample, buf, count)) {
                        retval = -EFAULT;
                        goto out;
                }

                retval = pxi6259_ao_write_to_channel(mcon, chan_number, (uint32_t)sample);
                if (retval) goto out;
        } else { /* Try to write all samples to the AO fifo */
                if (DMA_IDLE == mcon->ao_chan.state || DMA_UNKNOWN == mcon->ao_chan.state)
                              return -EFAULT;

                if (count < AO_CHAN_MITE_DATA_WIDTH) {
                        retval = 0;
                        goto out;
                }

                /* Loop until there is something to write */
                bytes_free = bytes_free_or_error(mcon, chan_number);
                if (bytes_free < 0) {
                        retval = bytes_free;
                        goto out;
                }

                /* If retransmit check if any space available */
                if (mcon->ao_task.fifoRetransmitEnable && bytes_free == 0) {
                        printk(KERN_INFO "%s AO is in retransmit mode and there is no more "
                                        "space in DMA buffer\n", pci_name(mcon->pci_dev));
                        goto out;
                }

                /* In FIFO retransmit we might need to clear fifo buffer.
                /* This is needed when restarting ao_segment with new samples
                /* without it, samples would just get appended to the end of buffer */
                if(mcon->ao_task.fifoRetransmitEnable && mcon->ao_cli_info[chan_number].number_of_samples == 0){
                        PDEBUG("%s Clearing FIFO to accept new data\n",pci_name(mcon->pci_dev));
                        ao_clear_fifo(mcon);
                }

                /* Wait until we have enough free space for at least one sample? */
                while (bytes_free < AO_CHAN_MITE_DATA_WIDTH) {
                        mutex_unlock(&mcon->ao_mutex);
                        if (filp->f_flags & O_NONBLOCK) {
                                retval = -EAGAIN;
                                goto out;
                        }

                        if (wait_event_interruptible(cli->wait_queue, bytes_free_or_error(mcon, chan_number))) {
                                return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
                        }

                        /* otherwise loop, but first reacquire the lock */
                        mutex_lock(&mcon->ao_mutex);
                }

                bytes_free = bytes_free_or_error(mcon, chan_number);
                if (bytes_free < 0) {
                        retval = bytes_free;
                        goto out;
                }

                /* Calculate how many samples fit into DMA buffer */
                num_samples = (bytes_free / AO_CHAN_MITE_DATA_WIDTH);
                /* The first index is always chan_number */
                num_samples = num_samples/chan_total
                                + (num_samples % chan_total > 0 ? 1 : 0);

                if (num_samples > count/AO_CHAN_MITE_DATA_WIDTH)
                        num_samples = count/AO_CHAN_MITE_DATA_WIDTH;

                /* Move data to DMA buffer */
                retval = write_ao_chanel_to_linear_dma_buffer(mcon,
                                &mcon->ao_cli_info[chan_number], num_samples, chan_total, (void *)buf);
                if (retval) goto out;

                /* Check for overflow again. Note that the write index is not updated. Check for an
                 * underflow while moving the data into the DMA buffer
                 */
                bytes_free = bytes_free_or_error(mcon, chan_number);
                if (bytes_free < 0) {
                        retval = bytes_free;
                        goto out;
                }

                /* No underflow during data move - update write index. Note that the correct position
                 * needs to be calculated so that it points to next channel sample;
                 */
                mcon->ao_cli_info[chan_number].write_idx += (num_samples * chan_total * AO_CHAN_MITE_DATA_WIDTH);

                // update number of samples in the current channel
                mcon->ao_cli_info[chan_number].number_of_samples += num_samples;


                /* Update count */
                count = num_samples * AO_CHAN_MITE_DATA_WIDTH;

                PDEBUG("%s written %d samples into DMA buffer channel %d\n", pci_name(mcon->pci_dev), num_samples,chan_number);


                /* Start DMA early if we have all of the samples*/
                /* This saves us about 3-10ms in startAO */

                /* Count the samples */
                bytes_free = 0; samples_in_buffer =0;
                for(i=0;i<=PXI6259_MAX_AO_CHANNEL;i++){
                    if(mcon->ao_task.channel[i].enabled){
                        samples_in_buffer += mcon->ao_cli_info[i].number_of_samples;
                        bytes_free += mcon->ao_task.numberOfSamples - mcon->ao_cli_info[i].number_of_samples;
                    }
                }

                /* Check if we can start DMA */
                //TODO: Fix the NON fifo regeneration case for starting DMA
                /** This might prove to be a problem in the future if buffers are not aligned (e.g.) there is 8192 samples in first buffer and no samples in other buffers **/
                if(     (samples_in_buffer > 8191 && !mcon->ao_task.fifoRetransmitEnable) || //non FIFO retransmit mode,
                                                                                             //we can start transfer once we have more samples
                                                                                             //than size of FIFO buffer
                        (bytes_free == 0 && mcon->ao_task.fifoRetransmitEnable)             //FIFO retransmit mode, start the transfer when we have all of the samples
                                                                                   ){

                    /* The DMA process will start moving data even if the timing engine is not started, it's
                       only controlled by the FIFO request flag, which is set to request to less-than-full in
                       aoFifoMode(). */
                    if( mcon->ao_chan.state != DMA_STARTED ){

                        PDEBUG("%s pre starting DMA channel\n",pci_name(mcon->pci_dev));

                        /* Select the DMA Channel and enable FIFO request */
                        set_register_bits(mcon, AO_Start_Select_Register, AO_AOFREQ_Enable, AO_AOFREQ_Enable);
                        flush_register(mcon, AO_Start_Select_Register);
                        set_register_bits_on_off(mcon, AI_AO_Select_Register, ((u8)2 << 0x4) & ~0xf, 1);
                        flush_register(mcon, AI_AO_Select_Register);

                        /* Select the DMA Channel and enable FIFO request */
                        set_register_bits(mcon, AO_Start_Select_Register, AO_AOFREQ_Enable, AO_AOFREQ_Enable);
                        flush_register(mcon, AO_Start_Select_Register);
                        set_register_bits_on_off(mcon, AI_AO_Select_Register, ((u8)2 << 0x4) & ~0xf, 1);
                        flush_register(mcon, AI_AO_Select_Register);

                        retval = dma_channel_start(mcon, &mcon->ao_chan);
                        if (retval) {
                               PDEBUG("%s start AO DMA channel failed %d!\n", pci_name(mcon->pci_dev), retval);
                               return retval;
                        }
                    }
                }

        }

        *f_pos += count;
        retval = count;

  out:
        mutex_unlock(&mcon->ao_mutex);
        return retval;
}

/*
 * For each client (user space process) the new aochan context is created. The same
 * structure is stored in private_data and used in other functions.
 */
static int pxi6259_aochan_open(struct inode *inode, struct file *filp)
{
        int minor = MINOR(inode->i_cdev->dev);
        struct module_context *mcon;
        struct aochan_listitem *lptr;
        unsigned int chan_number = 0, chan_total, dma_idx;
        int i;

        /* aochan minor devices are 2,3,...,33 */
        int arr_offset = (minor % DEVICE_MINOR_STEP) - ((PXI6259_MAX_AI_CHANNEL + 1) + 1 + 1) - 1;

        mcon = container_of_array(inode->i_cdev,
                        struct module_context, aochancdev, arr_offset, sizeof(struct cdev));

        PDEBUG("%s AO channel open %d %lx %d\n", pci_name(mcon->pci_dev), arr_offset, (long int)mcon, (minor % DEVICE_MINOR_STEP));


        mutex_lock(&mcon->ao_mutex);
        /* Find out which channel it belongs to */
        for (i=0, chan_total=0, dma_idx=0; i<=PXI6259_MAX_AO_CHANNEL; i++) {
                if (&mcon->aochancdev[i] == inode->i_cdev) {
                        chan_number = i;
                        dma_idx = chan_total; /* chan_total is not jet updated */
                }
                if (mcon->aochandev[i] != NULL)
                        chan_total++;
        }

        /* Allocate new aochan context */
        lptr = kmalloc(sizeof(struct aochan_listitem), GFP_KERNEL);
        if (!lptr) {
                return -ENOMEM;
        }
        memset(lptr, 0, sizeof(struct aochan_listitem));

        /* Save keys */
        lptr->key = lptr;
        lptr->cli.mcon = mcon;
        lptr->pid = get_current()->pid;
        lptr->mcon = mcon;
        lptr->chan_number = chan_number;
        lptr->chan_total = chan_total;
        lptr->dma_idx = dma_idx;

        /* Initialize wait queue */
        init_waitqueue_head(&lptr->cli.wait_queue);

        PDEBUG("%s aochan opened (pid %d mconidx %d aochan %d)\n",
                        pci_name(mcon->pci_dev), lptr->pid, mcon->index, lptr->chan_number);
        /* Add it to the list */
        list_add(&lptr->list, &mcon->aochan_list);
        mcon->aochanclients++;

        mutex_unlock(&mcon->ao_mutex); //fixed to prevent data inconsistency (chan_total,dma_idx)


        /* for other methods */
        filp->private_data = lptr;

        return 0;
}

/*
 * Delete aochan context.
 */
static int pxi6259_aochan_release(struct inode *inode, struct file *filp)
{
        struct aochan_listitem *aochan_item = (struct aochan_listitem *)filp->private_data;
        struct module_context *mcon = aochan_item->mcon;
        struct aochan_listitem *lptr, *next;

        mutex_lock(&mcon->ao_mutex);

        list_for_each_entry_safe(lptr, next, &mcon->aochan_list, list) {
                if (lptr->key == aochan_item) {
                        list_del(&lptr->list);
                        mcon->aochanclients--;
                        PDEBUG("%s aochan release (pid %d mconidx %d aochan %d)\n",
                                        pci_name(mcon->pci_dev), aochan_item->pid,
                                        mcon->index, aochan_item->chan_number);
                        kfree(lptr);
                }
        }

        mutex_unlock(&mcon->ao_mutex);

        return 0;
}

static unsigned int pxi6259_aochan_poll(struct file *filp, poll_table *wait)
{
        struct aochan_listitem *aochan_item = (struct aochan_listitem *)filp->private_data;
        struct module_context *mcon = aochan_item->mcon;
        unsigned int chan_number = aochan_item->chan_number;
        unsigned int chan_total = aochan_item->chan_total;
        int bytes_free, num_samples;
        unsigned int mask = 0;

        mutex_lock(&mcon->ao_mutex);

        /* Check if channel still valid */
        if (mcon->aochandev[chan_number] == NULL) {
                mask = POLLERR;
                goto out;
        }

        /* pxi6259_write only one value to the channel*/
        if (mcon->ao_task.signalGeneration == AO_SIGNAL_GENERATION_STATIC) {
                mask = (POLLOUT | POLLWRNORM);
        } else {
                if (DMA_IDLE == mcon->ao_chan.state || DMA_UNKNOWN == mcon->ao_chan.state) {
                        mask = POLLERR;
                        goto out;
                }

                /* Loop until there is something to write */
                bytes_free = bytes_free_or_error(mcon, chan_number);
                if (bytes_free < 0) {
                        mask = POLLERR;
                        goto out;
                } else {
                        /* Calculate how many samples fit into DMA buffer */
                        num_samples = (bytes_free / AO_CHAN_MITE_DATA_WIDTH);
                        /* The first index is always chan_number */
                        num_samples = num_samples/chan_total
                                        + (num_samples % chan_total > 0 ? 1 : 0);
                        if (num_samples > 0)
                                mask = (POLLOUT | POLLWRNORM);
                }
        }
  out:
        mutex_unlock(&mcon->ao_mutex);
        return mask;
}

struct file_operations pxi6259_aochan_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =    pxi6259_aochan_ioctl,
        .read =     NULL,
        .write =    pxi6259_aochan_write,
        .open =     pxi6259_aochan_open,
        .release =  pxi6259_aochan_release,
        .poll =     pxi6259_aochan_poll,
};

long pxi6259_dio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct module_context *mcon = (struct module_context *)filp->private_data;
        struct pxi6259_dio_conf dio_conf;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->dio_mutex);

        switch(cmd) {
        /* DIO SEGMENT IOCTL CALLS */
          case PXI6259_IOC_LOAD_DIO_CONF:
                rc = copy_from_user((void *)&dio_conf, (void *)arg, sizeof(dio_conf));
                if (rc){
                        rc = -EINVAL;
                        break;
                }
                rc = pxi6259_load_dio_conf(mcon, &dio_conf);
                break;

          case PXI6259_IOC_READ_DIO_CONF:
                if (mcon->dio_state == DIO_STATE_UNKNOWN) {
                        rc = -EINVAL;
                } else {
                        rc = copy_to_user((void *)arg, (void *)&mcon->dio_task, sizeof(mcon->dio_task));
                }
                break;

          case PXI6259_IOC_RESET_DIO:
              rc = pxi6259_reset_dio(mcon);
              break;

          case PXI6259_IOC_GET_DEVICE_ID:
                rc = copy_to_user((void *)arg, &mcon->index, sizeof(u32));
                break;

          default:  /* redundant, as cmd was checked against MAXNR */
                rc = -ENOTTY;
        }

        mutex_unlock(&mcon->dio_mutex);
        return rc;
}

static int pxi6259_dio_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon = container_of(inode->i_cdev, struct module_context, diocdev);
        filp->private_data = mcon;      /* for other methods */

        PDEBUG("%s DIO open\n", pci_name(mcon->pci_dev));

        return 0;                       /* success */
}

/* Everybody can open and release file descriptor */
struct file_operations pxi6259_dio_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =    pxi6259_dio_ioctl,
        .open =     pxi6259_dio_open,
        .release =  NULL,
};

ssize_t pxi6259_diochan_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
        struct diochan_private *priv = (struct diochan_private *)filp->private_data;
        struct module_context *mcon = priv->mcon;
        unsigned int chan_number = priv->chan_number;
        ssize_t retval = 0;

        mutex_lock(&mcon->dio_mutex);

        if (count < sizeof(u32)) {
                retval = 0;
                goto out;
        }

        /* Read only one value from the port*/
        retval = pxi6259_read_dio_port(mcon, chan_number, buf);
        if (retval) goto out;

        count = sizeof(u32);
        *f_pos += count;
        retval = count;

  out:
        mutex_unlock(&mcon->dio_mutex);
        return retval;
}

ssize_t pxi6259_diochan_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
        struct diochan_private *priv = (struct diochan_private *)filp->private_data;
        struct module_context *mcon = priv->mcon;
        unsigned int chan_number = priv->chan_number;
        ssize_t retval = 0;

        mutex_lock(&mcon->dio_mutex);

        if (count < sizeof(u32)) {
                retval = 0;
                goto out;
        }

        count = sizeof(u32);

        /* Write only one value to the port*/
        retval = pxi6259_write_to_dio_port(mcon, chan_number, (void *)buf);
        if (retval) goto out;

        *f_pos += count;
        retval = count;

  out:
        mutex_unlock(&mcon->dio_mutex);
        return retval;
}


static int pxi6259_diochan_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon;
        unsigned int chan_number = 0, chan_total;
        struct diochan_private *priv;
        int i, minor = MINOR(inode->i_cdev->dev);

        /* aochan minor devices are 2,3,...,33 */
        int arr_offset = (minor % DEVICE_MINOR_STEP) -
                        ((PXI6259_MAX_AI_CHANNEL + 1 + 1) + (PXI6259_MAX_AO_CHANNEL + 1 + 1)) - 1 - 1;

        mcon = container_of_array(inode->i_cdev,
                        struct module_context, diochancdev, arr_offset, sizeof(struct cdev));


        /* Find out which channel it belongs to */
        for (i=0, chan_total=0; i<=PXI6259_MAX_DIO_PORT; i++) {
                if (&mcon->diochancdev[i] == inode->i_cdev)
                        chan_number = i;
                if (mcon->diochandev[i] != NULL)
                        chan_total++;
        }

        priv = kmalloc(sizeof(struct diochan_private), GFP_KERNEL);
        if (!priv) {
                return -ENOMEM;
        }
        memset(priv, 0, sizeof(struct diochan_private));

        priv->mcon = mcon;
        priv->chan_number = chan_number;
        filp->private_data = priv; /* For other methods */

        PDEBUG("%s DIO port %d open\n", pci_name(mcon->pci_dev), chan_number);

        return 0;                       /* success */
}

static int pxi6259_diochan_release(struct inode *inode, struct file *filp)
{
        struct diochan_private *priv = (struct diochan_private *)filp->private_data;
        kfree(priv);

        return 0;
}

struct file_operations pxi6259_diochan_fops = {
        .owner =    THIS_MODULE,
        .read =     pxi6259_diochan_read,
        .write =    pxi6259_diochan_write,
        .open =     pxi6259_diochan_open,
        .release =  pxi6259_diochan_release,
};

long pxi6259_gpc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
        int rc = 0;
        struct gpcchan_private *priv = (struct gpcchan_private *)filp->private_data;
        struct module_context *mcon = priv->mcon;
        struct pxi6259_gpc_conf gpc_task;

        /*
         * extract the type and number bitfields, and don't decode
         * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
         */
        if (_IOC_TYPE(cmd) != PXI6259_IOC_MAGIC) return -ENOTTY;
        if (_IOC_NR(cmd) > PXI6259_IOC_MAX_NUM) return -ENOTTY;

        /*
         * the direction is a bitmask, and VERIFY_WRITE catches R/W
         * transfers. `Type' is user-oriented, while
         * access_ok is kernel-oriented, so the concept of "pxi6259_read" and
         * "pxi6259_write" is reversed
         */
        if (_IOC_DIR(cmd) & _IOC_READ)
                rc = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
        else if (_IOC_DIR(cmd) & _IOC_WRITE)
                rc =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
        if (rc) return -EFAULT;

        mutex_lock(&mcon->gpc_mutex);

        switch(cmd) {
          /* GENERAL PURPOSE COUNTER SEGMENTS IOCTL CALLS */
          case PXI6259_IOC_LOAD_GPC0_CONF:
                  rc = copy_from_user((void *) &gpc_task, (void *)arg, sizeof(gpc_task));
                  if (rc){
                          rc = -EINVAL;
                          break;
                  }

                  rc = pxi6259_load_gpc0_conf(mcon, &gpc_task);
                  break;

          case PXI6259_IOC_READ_GPC0_CONF:
                  if (mcon->gpc0_state == GPC_STATE_UNKNOWN)
                          rc =-EINVAL;
                  else
                          rc = copy_to_user((void *)arg, (void *)&mcon->gpc0_task, sizeof(mcon->gpc0_task));
                  break;

          case PXI6259_IOC_LOAD_GPC1_CONF:
                  rc = copy_from_user((void *) &gpc_task, (void *)arg, sizeof(gpc_task));
                  if (rc){
                          rc = -EINVAL;
                          break;
                  }
                  rc = pxi6259_load_gpc1_conf(mcon, &gpc_task);
                  break;

          case PXI6259_IOC_READ_GPC1_CONF:
                  if (mcon->gpc1_state == GPC_STATE_UNKNOWN)
                          rc =-EINVAL;
                  else
                          rc = copy_to_user((void *)arg, (void *)&mcon->gpc1_task, sizeof(mcon->gpc1_task));
                  break;

          case PXI6259_IOC_START_GPC0:
          case PXI6259_IOC_START_GPC1:
                  if (cmd == PXI6259_IOC_START_GPC0)
                          rc = pxi6259_start_gpc0(mcon);
                  else
                          rc = pxi6259_start_gpc1(mcon);
                  break;

          case PXI6259_IOC_STOP_GPC0:
          case PXI6259_IOC_STOP_GPC1:
                  if (cmd == PXI6259_IOC_STOP_GPC0)
                          rc = pxi6259_stop_gpc0(mcon);
                  else
                          rc = pxi6259_stop_gpc1(mcon);

                  break;

          case PXI6259_IOC_RESET_GPC:
              rc = pxi6259_reset_gpc(mcon);
              break;

          /* Bug 3893 */
          case PXI6259_IOC_GET_DEVICE_ID:
                  rc = copy_to_user((void *)arg, &mcon->index, sizeof(u32));
                  break;

          default:  /* redundant, as cmd was checked against MAXNR */
                rc = -ENOTTY;
        }

        mutex_unlock(&mcon->gpc_mutex);
        return rc;
}


static int pxi6259_gpc_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon = container_of(inode->i_cdev, struct module_context, gpccdev);
        unsigned int chan_number;
        struct gpcchan_private *priv;

        if (&mcon->gpcchancdev[0] == inode->i_cdev)
                chan_number = 0;
        else
                chan_number = 1;

        priv = kmalloc(sizeof(struct gpcchan_private), GFP_KERNEL);
        if (!priv) {
                return -ENOMEM;
        }
        memset(priv, 0, sizeof(struct gpcchan_private));

        priv->mcon = mcon;
        priv->chan_number = chan_number;
        filp->private_data = priv; /* For other methods */

        PDEBUG("%s GPC%d open\n", pci_name(mcon->pci_dev), chan_number);

        return 0;                       /* success */
}

static int pxi6259_gpc_release(struct inode *inode, struct file *filp)
{
        struct gpcchan_private *priv = (struct gpcchan_private *)filp->private_data;
        kfree(priv);

        PDEBUG("%s GPC%d release\n", pci_name(priv->mcon->pci_dev), priv->chan_number);

        return 0;
}

struct file_operations pxi6259_gpc_fops = {
        .owner =    THIS_MODULE,
        .unlocked_ioctl =    pxi6259_gpc_ioctl,
        .open =     pxi6259_gpc_open,
        .release =  pxi6259_gpc_release,
};

ssize_t pxi6259_gpcchan_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
        struct gpcchan_private *priv = (struct gpcchan_private *)filp->private_data;
        struct module_context *mcon = priv->mcon;
        unsigned int chan_number = priv->chan_number;
        u32 gpc_value;
        ssize_t retval = 0;

        mutex_lock(&mcon->gpc_mutex);

        if (count < sizeof(u32)) {
                retval = 0;
                goto out;
        }

        /* Read only one value from the port*/
        if (chan_number == 0)
                retval = pxi6259_get_gpc0_value(mcon, &gpc_value);
        else
                retval = pxi6259_get_gpc1_value(mcon, &gpc_value);
        if (retval) goto out;

        retval = copy_to_user((void *)buf, (void *)&gpc_value, sizeof(gpc_value));
        if (retval) {
                retval = -ENOMEM;
                goto out;
        }

        count = sizeof(u32);
        *f_pos += count;
        retval = count;

  out:
        mutex_unlock(&mcon->gpc_mutex);
        return retval;
}

static int pxi6259_gpcchan_open(struct inode *inode, struct file *filp)
{
        struct module_context *mcon;
        unsigned int chan_number;
        struct gpcchan_private *priv;
        int minor, arr_offset;

        minor = MINOR(inode->i_cdev->dev);

        /* aochan minor devices are 2,3,...,33 */
        arr_offset = (minor % DEVICE_MINOR_STEP) -
                        ((PXI6259_MAX_AI_CHANNEL + 1 + 1) + (PXI6259_MAX_AO_CHANNEL + 1 + 1)
                        + (PXI6259_MAX_DIO_PORT + 1 + 1)) - 1 - 1;

        mcon = container_of_array(inode->i_cdev,
                        struct module_context, gpcchancdev, arr_offset, sizeof(struct cdev));

        if (&mcon->gpcchancdev[0] == inode->i_cdev)
                chan_number = 0;
        else
                chan_number = 1;

        priv = kmalloc(sizeof(struct gpcchan_private), GFP_KERNEL);
        if (!priv) {
                return -ENOMEM;
        }
        memset(priv, 0, sizeof(struct gpcchan_private));

        priv->mcon = mcon;
        priv->chan_number = chan_number;
        filp->private_data = priv; /* For other methods */

        PDEBUG("%s GPC%d open\n", pci_name(mcon->pci_dev), chan_number);

        return 0;                       /* success */
}

struct file_operations pxi6259_gpcchan_fops = {
        .owner =    THIS_MODULE,
        .read =     pxi6259_gpcchan_read,
        .write =    NULL,
        .open =     pxi6259_gpcchan_open,
        .release =  pxi6259_gpc_release, /* Release function is shared */
};


/*
 * This function creates ai and aichan devices if not jet and publis its cdev structures.
 * It should be noted the the channel devices are deleted in any case and then recreated.
 * This behavior is needed because we must notify user space somehow.
 */
static int pxi6259_update_ai_devices(struct module_context *mcon, dev_t aidev)
{
        int i, minor = MINOR(aidev), major = MAJOR(aidev);

        if (MINOR(aidev) >= MINOR_NUMBERS - PXI6259_MAX_AI_CHANNEL - 1) {
                return -EINVAL;
        }

        PDEBUG("%s update AI devices %lx\n", pci_name(mcon->pci_dev), (long int)mcon);

        /* Create aidev if needed */
        if (mcon->aidev == NULL) {
                cdev_init(&mcon->aicdev, &pxi6259_ai_fops);
                mcon->aicdev.owner = THIS_MODULE;
                mcon->aicdev.ops = &pxi6259_ai_fops;
                cdev_add(&mcon->aicdev, MKDEV(major, minor), 1);
                // TODO check retval

                mcon->aidev = device_create(pxi6259_class,
                                mcon->dev, aidev,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.ai", mcon->index);
                if (IS_ERR(mcon->aidev)) {
                        cdev_del(&mcon->aicdev);
                        return PTR_ERR(mcon->aidev);
                }
        }

        minor++;

        /* Delete all channel devices ... */
        for (i=0 ; i<=PXI6259_MAX_AI_CHANNEL; i++) {
                if (mcon->aichandev[i] != NULL) {
                        cdev_del(&mcon->aichancdev[i]);
                        device_destroy(pxi6259_class, MKDEV(major, minor + i));
                        mcon->aichandev[i] = NULL;
                }
        }

        /* and create them back if needed */
        for (i=0 ; i<=PXI6259_MAX_AI_CHANNEL; i++) {
                if (mcon->ai_state != AI_STATE_UNKNOWN
                                && (mcon->ai_conf.channel_mask & 1<<i)
                                && mcon->aichandev[i] == NULL) {
                        cdev_init(&mcon->aichancdev[i], &pxi6259_aichan_fops);
                        mcon->aichancdev[i].owner = THIS_MODULE;
                        mcon->aichancdev[i].ops = &pxi6259_aichan_fops;
                        cdev_add(&mcon->aichancdev[i], MKDEV(major, minor + i), 1);
                        // TODO check retval

                        mcon->aichandev[i] = device_create(pxi6259_class,
                                        mcon->aidev, MKDEV(major, minor + i),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                        NULL,
#endif
                                        "pxi6259.%d.ai.%d", mcon->index, i);



                        if (IS_ERR(mcon->aichandev[i])) {
                                cdev_del(&mcon->aichancdev[i]);
                                return PTR_ERR(mcon->aidev);
                        }
                }
        }

        return 0;
}

static int pxi6259_update_ao_devices(struct module_context *mcon, dev_t aodev)
{
        int i, minor = MINOR(aodev), major = MAJOR(aodev);

        if (MINOR(aodev) >= MINOR_NUMBERS - PXI6259_MAX_AO_CHANNEL) {
                return -EINVAL;
        }

        PDEBUG("%s update AO devices %lx\n", pci_name(mcon->pci_dev), (long int)mcon);

        /* Create aidev if needed */
        if (mcon->aodev == NULL) {
                cdev_init(&mcon->aocdev, &pxi6259_ao_fops);
                mcon->aocdev.owner = THIS_MODULE;
                mcon->aocdev.ops = &pxi6259_ao_fops;
                cdev_add(&mcon->aocdev, MKDEV(major, minor), 1);
                // TODO check retval

                mcon->aodev = device_create(pxi6259_class,
                                mcon->dev, aodev,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.ao", mcon->index);
                if (IS_ERR(mcon->aodev)) {
                        cdev_del(&mcon->aocdev);
                        return PTR_ERR(mcon->aodev);
                }
        }

        minor++;

        /* Delete devices */
        for (i=0 ; i<=PXI6259_MAX_AO_CHANNEL; i++) {
                if (mcon->aochandev[i] != NULL) {
                        cdev_del(&mcon->aochancdev[i]);
                        device_destroy(pxi6259_class, MKDEV(major, minor + i));
                        mcon->aochandev[i] = NULL;
                }
        }

        /* and create them back */
        for (i=0 ; i<=PXI6259_MAX_AO_CHANNEL; i++) {
                if (mcon->ao_state != AO_STATE_UNKNOWN
                                && (mcon->ao_task.channel[i].enabled)
                                && mcon->aochandev[i] == NULL) {
                        cdev_init(&mcon->aochancdev[i], &pxi6259_aochan_fops);
                        mcon->aochancdev[i].owner = THIS_MODULE;
                        mcon->aochancdev[i].ops = &pxi6259_aochan_fops;
                        cdev_add(&mcon->aochancdev[i], MKDEV(major, minor + i), 1);
                        // TODO check retval

                        mcon->aochandev[i] = device_create(pxi6259_class,
                                        mcon->aodev, MKDEV(major, minor + i),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                        NULL,
#endif
                                        "pxi6259.%d.ao.%d", mcon->index, i);
                        if (IS_ERR(mcon->aochandev[i])) {
                                cdev_del(&mcon->aochancdev[i]);
                                return PTR_ERR(mcon->aodev);
                        }
                }
        }

        return 0;
}

static int pxi6259_update_dio_devices(struct module_context *mcon, dev_t diodev)
{
        int i, minor = MINOR(diodev), major = MAJOR(diodev);

        if (MINOR(diodev) >= MINOR_NUMBERS - PXI6259_MAX_DIO_PORT - 1 - 1) {
                return -EINVAL;
        }

        PDEBUG("%s update DIO devices %lx\n", pci_name(mcon->pci_dev), (long int)mcon);

        /* Create aidev if needed */
        if (mcon->diodev == NULL) {
                cdev_init(&mcon->diocdev, &pxi6259_dio_fops);
                mcon->diocdev.owner = THIS_MODULE;
                mcon->diocdev.ops = &pxi6259_dio_fops;
                cdev_add(&mcon->diocdev, MKDEV(major, minor), 1);
                // TODO check retval

                mcon->diodev = device_create(pxi6259_class,
                                mcon->dev, diodev,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.dio", mcon->index);
                if (IS_ERR(mcon->diodev)) {
                        cdev_del(&mcon->diocdev);
                        return PTR_ERR(mcon->diodev);
                }
        }

        minor++;

        /* Delete devices */
        for (i=0 ; i<=PXI6259_MAX_DIO_PORT; i++) {
                if (mcon->diochandev[i] != NULL) {
                        cdev_del(&mcon->diochancdev[i]);
                        device_destroy(pxi6259_class, MKDEV(major, minor + i));
                        mcon->diochandev[i] = NULL;
                }
        }

        /* and create them back */
        for (i=0 ; i<=PXI6259_MAX_DIO_PORT; i++) {
                if (mcon->dio_state != DIO_STATE_UNKNOWN
                                && (mcon->dio_task.channel[i].enabled)
                                && mcon->diochandev[i] == NULL) {
                        cdev_init(&mcon->diochancdev[i], &pxi6259_diochan_fops);
                        mcon->diochancdev[i].owner = THIS_MODULE;
                        mcon->diochancdev[i].ops = &pxi6259_diochan_fops;
                        cdev_add(&mcon->diochancdev[i], MKDEV(major, minor + i), 1);
                        // TODO check retval

                        mcon->diochandev[i] = device_create(pxi6259_class,
                                        mcon->diodev, MKDEV(major, minor + i),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                        NULL,
#endif
                                        "pxi6259.%d.dio.%d", mcon->index, i);
                        if (IS_ERR(mcon->diochandev[i])) {
                                cdev_del(&mcon->diochancdev[i]);
                                return PTR_ERR(mcon->diodev);
                        }
                }
        }

        return 0;
}

static int pxi6259_update_gpc_devices(struct module_context *mcon, dev_t gpcdev)
{
        int i, minor = MINOR(gpcdev), major = MAJOR(gpcdev);

        if (MINOR(gpcdev) >= MINOR_NUMBERS - PXI6259_MAX_GPC - 1 - 1) {
                return -EINVAL;
        }

        PDEBUG("%s update gpc devices %lx\n", pci_name(mcon->pci_dev), (long int)mcon);

        /* Create gpcdev if needed */
        if (mcon->gpcdev == NULL) {
                cdev_init(&mcon->gpccdev, &pxi6259_gpc_fops);
                mcon->gpccdev.owner = THIS_MODULE;
                mcon->gpccdev.ops = &pxi6259_gpc_fops;
                cdev_add(&mcon->gpccdev, MKDEV(major, minor), 1);
                // TODO check retval

                mcon->gpcdev = device_create(pxi6259_class,
                                mcon->dev, gpcdev,
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.gpc", mcon->index);
                if (IS_ERR(mcon->gpcdev)) {
                        cdev_del(&mcon->gpccdev);
                        return PTR_ERR(mcon->gpcdev);
                }
        }

        minor++;

        /* Delete devices */
        for (i=0 ; i<=PXI6259_MAX_GPC; i++) {
                if (mcon->gpcchandev[i] != NULL) {
                        cdev_del(&mcon->gpcchancdev[i]);
                        device_destroy(pxi6259_class, MKDEV(major, minor + i));
                        mcon->gpcchandev[i] = NULL;
                }
        }

        /* and create them back */
        i = 0;
        if (mcon->gpc0_state != GPC_STATE_UNKNOWN
                        && mcon->gpcchandev[i] == NULL) {
                cdev_init(&mcon->gpcchancdev[i], &pxi6259_gpcchan_fops);
                mcon->gpcchancdev[i].owner = THIS_MODULE;
                mcon->gpcchancdev[i].ops = &pxi6259_gpcchan_fops;
                cdev_add(&mcon->gpcchancdev[i], MKDEV(major, minor + i), 1);
                // TODO check retval

                mcon->gpcchandev[i] = device_create(pxi6259_class,
                                mcon->gpcdev, MKDEV(major, minor + i),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.gpc.%d", mcon->index, i);
                if (IS_ERR(mcon->gpcchandev[i])) {
                        cdev_del(&mcon->gpcchancdev[i]);
                        return PTR_ERR(mcon->gpcdev);
                }
        }

        i++;
        if (mcon->gpc1_state != GPC_STATE_UNKNOWN
                        && mcon->gpcchandev[i] == NULL) {
                cdev_init(&mcon->gpcchancdev[i], &pxi6259_gpcchan_fops);
                mcon->gpcchancdev[i].owner = THIS_MODULE;
                mcon->gpcchancdev[i].ops = &pxi6259_gpcchan_fops;
                cdev_add(&mcon->gpcchancdev[i], MKDEV(major, minor + i), 1);
                // TODO check retval

                mcon->gpcchandev[i] = device_create(pxi6259_class,
                                mcon->gpcdev, MKDEV(major, minor + i),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                                NULL,
#endif
                                "pxi6259.%d.gpc.%d", mcon->index, i);
                if (IS_ERR(mcon->gpcchandev[i])) {
                        cdev_del(&mcon->gpcchancdev[i]);
                        return PTR_ERR(mcon->gpcdev);
                }
        }

        return 0;
}

static int pxi6259_create_devices(struct module_context *mcon)
{
        int rc;
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        PDEBUG("%s create devices\n", pci_name(mcon->pci_dev));

        /* Create main device */
        cdev_init(&mcon->cdev, &pxi6259_fops);
        mcon->cdev.owner = THIS_MODULE;
        mcon->cdev.ops = &pxi6259_fops;
        cdev_add(&mcon->cdev, MKDEV(major, minor), 1);
        // TODO check retval

        mcon->dev = device_create(pxi6259_class, NULL,
                        MKDEV(major, minor),
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
                        NULL,
#endif
                        "pxi6259.%d", mcon->index);
        if (IS_ERR(mcon->dev)) {
                cdev_del(&mcon->cdev);
                return PTR_ERR(mcon->dev);
        }

        /* Update segment devices */
        minor++;
        rc = pxi6259_update_ai_devices(mcon, MKDEV(major, minor));
        if (rc) return rc;

        minor += (PXI6259_MAX_AI_CHANNEL + 1 + 1);
        rc = pxi6259_update_ao_devices(mcon,MKDEV(major, minor));
        if (rc) return rc;

        minor += (PXI6259_MAX_AO_CHANNEL + 1 + 1);
        rc = pxi6259_update_dio_devices(mcon, MKDEV(major, minor));
        if (rc) return rc;

        minor += (PXI6259_MAX_DIO_PORT + 1 + 1);
        rc = pxi6259_update_gpc_devices(mcon, MKDEV(major, minor));
        if (rc) return rc;

        return 0;
}

/*
 * WARNING, this function will delete all the devices and because of this it
 * should be called only in release function.
 */
static int pxi6259_delete_devices(struct module_context *mcon)
{
        int minor = mcon->index * DEVICE_MINOR_STEP;
        int major = device_major;

        minor++;
        /* Delete all channel subdevices */
        mcon->ai_state = AI_STATE_UNKNOWN;
        pxi6259_update_ai_devices(mcon, MKDEV(major, minor));
        /* Explicitly remove ai subdevice */
        cdev_del(&mcon->aicdev);
        device_destroy(pxi6259_class, MKDEV(major, minor));

        minor += (PXI6259_MAX_AI_CHANNEL + 1 + 1);
        mcon->ao_state = AO_STATE_UNKNOWN;
        pxi6259_update_ao_devices(mcon, MKDEV(major, minor));
        /* Explicitly remove ao subdevice */
        cdev_del(&mcon->aocdev);
        device_destroy(pxi6259_class, MKDEV(major, minor));

        minor += (PXI6259_MAX_AO_CHANNEL + 1 + 1);
        mcon->dio_state = DIO_STATE_UNKNOWN;
        pxi6259_update_dio_devices(mcon, MKDEV(major, minor));
        /* Explicitly remove dio subdevice */
        cdev_del(&mcon->diocdev);
        device_destroy(pxi6259_class, MKDEV(major, minor));

        minor += (PXI6259_MAX_DIO_PORT + 1 + 1);
        mcon->gpc0_state = GPC_STATE_UNKNOWN;
        mcon->gpc1_state = GPC_STATE_UNKNOWN;
        pxi6259_update_gpc_devices(mcon, MKDEV(major, minor));
        /* Explicitly remove dio subdevice */
        cdev_del(&mcon->gpccdev);
        device_destroy(pxi6259_class, MKDEV(major, minor));

        /* Delete main devices */
        cdev_del(&mcon->cdev);
        device_destroy(pxi6259_class, MKDEV(major, mcon->index * DEVICE_MINOR_STEP));

        return 0;
}


/*
 * Puts event into queue.
 *
 * This function should be used only in interrupt handler. It does not
 * provide any locking capabilities because it is assumed that it runs at
 * interrupt time. If this is not true please implement locking outside of
 * this function.
 *
 */
static inline void pxi6259_irq_put_event(struct queue *queue, struct pxi6259_read_buffer rb)
{

        if (queue->queue_off) {
                if (queue->size > 0)
                        queue->missed += queue->size;
                queue->entries[queue->write_index] = rb;
                queue->size = 1;
        } else {
                queue->entries[queue->write_index] = rb;
                queue->write_index = (queue->write_index + 1) % MAX_QUEUE_SIZE;
                if (queue->size < MAX_QUEUE_SIZE) {
                        queue->size++;
                } else {
                        queue->missed++;
                        queue->read_index = (queue->read_index + 1) % MAX_QUEUE_SIZE;
                }
        }

        /* signal pxi6259_read function */
        wake_up_interruptible(&queue->outq);
}


void pxi6259_cleanup_driver(void)
{
        /* Remove modules first so that connections can be cleaned up */
        pci_unregister_driver(&sync_driver);

        class_unregister(pxi6259_class);
        unregister_chrdev_region(MKDEV(device_major,0), MINOR_NUMBERS);
}

int pxi6259_init_driver(void)
{
        int result;
        dev_t devno = 0;

        devno = MKDEV(0, 0);
        result = alloc_chrdev_region(&devno, 0, MINOR_NUMBERS, DEVICE_NAME);
        if (result) {
                printk ("Failed to register device %s with error %d\n", DEVICE_NAME, result);
                goto fail;
        }
        device_major = MAJOR(devno);

        pxi6259_class = class_create(THIS_MODULE, DRV_NAME);
        if (IS_ERR(pxi6259_class)) {
               printk(KERN_ERR "Unable to allocate class\n");
               result = PTR_ERR(pxi6259_class);
               goto unreg_chrdev;
        }

        result = pci_register_driver(&sync_driver);
        if (result) {
                goto unreg_class;
        }

        printk("Loaded pxi6259 driver\n");

        return 0; /* succeed */

  unreg_class:
        class_unregister(pxi6259_class);
        class_destroy(pxi6259_class);
  unreg_chrdev:
        unregister_chrdev_region(MKDEV(device_major,0), MINOR_NUMBERS);
  fail:
        return result;
}

module_init(pxi6259_init_driver);
module_exit(pxi6259_cleanup_driver);
