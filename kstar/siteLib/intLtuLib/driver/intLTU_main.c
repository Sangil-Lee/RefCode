/******************************************************************************
 *  Copyright (c) 2010 ~ by NFRI, Woong. All Rights Reserved.                     *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE


2010. 3. 10
made by woong ryol, Lee


*/



/*#include <linux/config.h> */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/mm.h>

#include <linux/fs.h>
#include "linux/version.h"


#include <linux/interrupt.h>
#include <linux/wait.h>


#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include "asm/io.h"        /* ioremap, writel, readl, .. */
#include "asm/uaccess.h"   /* copy_from/to_user, .. */

#include "asm/pgtable.h" /* pgprot_noncached(prot) */

#include "asm/irq.h"
/* don't need in SL6, MRG-R 2013. 5. 27 */
/*#include "asm/system.h" */




#include "ioctl_intLTU.h"
#include "intLTU_main.h"
#include "intLTU_debug.h"
#include "intLTU_Def.h"

#if 0
#include "drv_intLTU.h"
#endif


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
#include <linux/list.h>
#include <linux/kthread.h>
#include <linux/version.h>
#endif

#include "kernel_2_6.h"



#define USE_ISR_TRG1_HIGH    0
#define USE_ISR_TRG1_LOW     0
#define USE_READ_OUT_HIGH    0
#define USE_READ_OUT_LOW     0


//DECLARE_WAIT_QUEUE_HEAD(WaitQueue_Read);

//wait_queue_head_t ewaitq;


/*#define CLTU_DUAL_PORT_SIZE 0x8000 */
/*static u32 CLTU_DUAL_PORT_ADDR; */



#define MAXDEV 4

int intLTU_use_interrupts = 1;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
module_param(intLTU_use_interrupts, int, 0644);
MODULE_PARM_DESC(intLTU_use_interrupts, "use interrupts [0 = OFF]");

#endif


/* Globals */
/*
static int       intLTU_major = 0;
intLTU_device_t     intLTU_device[MAX_CLTU_CARDS+1];
intLTU_device_t*    intLTU_device_irq_cmp[MAX_DEVICE_IRQ_CMP];
int           intLTU_number_of_cards = 0;
*/
/* Forward declaration */
/*static void intLTU_exit(void); */



//int prev_msb=0, prev_nsb=0;


/** Globals .. keep to a minimum! */
char intLTU_driver_name[] = "CLTU";
char intLTU__driver_string[] = "NFRI timing synchronization device";
char intLTU__copyright[] = "Copyright (c) 2010 NFRI, woong";
static CLTU_Device* devices[MAXDEV];
static int idx=0;

/*struct class* intLTU_device_class; */
/*void *buff;*/

/*
void simple_vma_open(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "Simple VMA open, virt %lx, phys %lx\n",
	vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}
void simple_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "Simple VMA close.\n");
}

static struct vm_operations_struct simple_remap_vm_ops = {
.open = simple_vma_open,
.close = simple_vma_close,
};
*/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
CLTU_Device* intLTU_lookupDevice(struct device *dev)
{
	int id;

	for (id = 0; id < idx; ++id){
		CLTU_Device* intLTU_device = devices[id];
		if (dev == &devices[id]->pci_dev->dev){
			return intLTU_device;
		}
	}
	
	return 0;
}
#else
CLTU_Device* intLTU_lookupDevice(struct pci_dev *dev)
{
	int id;

	for (id = 0; id < idx; ++id){
		CLTU_Device* intLTU_device = devices[id];
		if (dev == devices[id]->pci_dev){
			return intLTU_device;
		}
	}
	return 0;
}
#endif

static CLTU_Device *intLTU_lookup_device(struct inode* inode)
{
	int major = MAJOR(inode->i_rdev);
	int isearch;

	for (isearch = 0; isearch < idx; ++isearch){
		CLTU_Device* device = devices[isearch];

		assert(device != 0);

		if (device->ldev.major == major){
/*			info("\"%s\" major : %d", device->ldev.drv_name, major); */
			return device;
		}
	}

	assert(0);
	return 0;
}




/************************************************/
/* it happen after intLTU_exit() or  ctrl+C pressed or remod...................       */

int intLTU_release(struct inode *inode, struct file *filp)
{
/*	CLTU_Device  *device;
	int  idev;
*/
/*	info("called intLTU_release" ); */
/*
	if( buff != NULL)
	{
		info(" free vmalloc buffer");
		vfree(buff);
	}
*/
#if 0
	
	for (idev = 0; idev <= idx; idev++) 
	{
		device = devices[idev];
		
		info ("CLTU: Device #%d: iounmap CLTU PCI control base", device->idx);
		/* Free interrupt handler */
/*		if (device->interrupt_line)
		{
			writel(0x0, device->virt_addr0 + 0x68);

			free_irq (device->interrupt_line, device);
			intLTU_device_irq_cmp[device->interrupt_line] = 0;
			device->interrupt_line = 0;
			printk ("CLTU: Device #%d: Removing IRQ #%d\n",
				device->devno, device->pcidev->irq);
		}
*/
		/* Unmap CLTU control base */
		if (device->virt_addr0) 
		{
			info ("CLTU: Device #%d: iounmap CLTU PCI control base", device->idx);
			iounmap (device->virt_addr0);
			device->virt_addr0 = NULL;
		}

	}
#endif
	return 0;
}


int intLTU_mmap(struct file *filp, struct vm_area_struct *vma)
{
	CLTU_Device *device;
	unsigned long virtaddr  = 0;
	unsigned long physaddr  = 0;
	unsigned long size      = 0;
/*	unsigned int pageNumber = 0;*/
	
#if 0
	info("vm_pgoff   0x%x", (unsigned int)vma->vm_pgoff << PAGE_SHIFT);
	info("vm_start   0x%x", (unsigned int)vma->vm_start );
	info("vm_end     0x%x", (unsigned int)vma->vm_end );
	info("vm_flags   0x%x", (unsigned int)vma->vm_flags );
#endif

	/* Get intLTU data struct pionter */
	if (!filp->private_data) 
	{
		err ("intLTU_mmap() : filp->private_data = NULL pointer\n");
		return -EAGAIN;
	}
	else
		device = (CLTU_Device*)filp->private_data;	

	vma->vm_flags |= VM_RESERVED;
	vma->vm_flags |= VM_IO;

	
	virtaddr = vma->vm_start;
	size     = vma->vm_end - vma->vm_start;
/*
	info("vmalloc test.");
	buff = vmalloc(4*PAGE_SIZE);
	if( buff == NULL) info("vmalloc... failed!");
	else {
		*((volatile unsigned int*)buff) = 0xabcd1234;
		}
*/

	if( device->card_ver == VERSION_R1 ) {
		physaddr = device->phys_pci_bar2;   /* interrupt LTU assign control register here  bar2*/
/*		physaddr = device->virt_addr2; */  /* not working.*/
	}
	else if ( device->card_ver == VERSION_R2 ) {
/*		physaddr = device->phys_pci_bar0 ; */

/*		info("physaddr addr: 0x%x", (unsigned int)device->phys_addr_bar0 ); */
		physaddr = device->phys_addr_bar0; 
/*
		pageNumber = physaddr/PAGE_SIZE;
*/
/*		info("page>>PAGE_SHIFT: %d, pgN0: %d", physaddr>>PAGE_SHIFT, pageNumber);  same result */

/*
		physaddr = virt_to_phys(device->virt_addr0);
		info("virtual addr: 0x%x", (unsigned int)device->virt_addr0 );
		info("physaddr addr: 0x%x", (unsigned int)physaddr);
*/
/*		physaddr = (unsigned long)buff ; */

	}
	else {
		err ("intLTU_mmap() : Wrong version! (%d)", device->card_ver);
		return -EAGAIN;
	}

#if 0
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
	if( remap_pfn_range ( vma, vma->vm_start, physaddr>>PAGE_SHIFT, size, pgprot_noncached(vma->vm_page_prot) ) ) 
/*	if( remap_pfn_range ( vma, vma->vm_start, pageNumber, size, pgprot_noncached(vma->vm_page_prot) ) ) */
#else 
	if( remap_page_range( vma, vma->vm_start, physaddr, size, pgprot_noncached(vma->vm_page_prot) ) )
#endif
	{
		return -EAGAIN;
	}
#else


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
	if( REMAP_PAGE_RANGE(vma, virtaddr, physaddr, size, pgprot_noncached(vma->vm_page_prot) ) )
		return -EAGAIN;
#else
	if( REMAP_PAGE_RANGE(vma, virtaddr, physaddr, size, vma->vm_page_prot ) )
		return -EAGAIN;
#endif

#endif

/*
	vma->vm_ops = &simple_remap_vm_ops;
	simple_vma_open(vma);
*/
	return 0;

}


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
int intLTU_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
#else
int intLTU_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
#endif
{
	int ret=0;
//	int test=0;
	unsigned int argVal, nval;

	CLTU_Device *device;


	/* Get intLTU data struct pionter */
	if (!filp->private_data) 
	{
		err ("filp->private_data = NULL pointer\n");
		return -ENODEV;

	}
	else {
		device = (CLTU_Device*)filp->private_data;
/*		test	= down_interruptible(&device->sem);
		if (test)
		{
			// We didn't get the semaphore. 
			err("We didn't get the semaphore.");
			ret	= -ERESTARTSYS;
			return ret;
		}
*/	
	}
	

	if( _IOC_TYPE(cmd) != IOCTL_CLTU_MAGIC ){
//		up(&device->sem);
		return -EINVAL; /* The IOCTL code isn't ours.*/
	}
	if( _IOC_NR(cmd) >= IOCTL_CLTU_MAXNR ) {
//		up(&device->sem);
		return -EINVAL; /* The IOCTL service is unrecognized. */
	}

	
#if 0
	size = _IOC_SIZE(cmd);
	if( size )
	{
		ret = 0;
		/* This function may sleep */
		if( _IOC_DIR(cmd) & _IOC_READ ) ret = access_ok(VERIFY_WRITE, (void *) arg, size);
		else if( _IOC_DIR(cmd) & _IOC_WRITE) ret = access_ok(VERIFY_READ, (void *) arg, size);

		if( ret)  {
			err (" access_ok ... error.");
			return ret;
		}
	}
	else ret = -EINVAL;
#endif

	

	switch(cmd) {

	case IOCTL_CLTU_READ: 
		info("virt_addr00[0]:    0x%x, addr: 0x%x", *((volatile unsigned int*)device->virt_addr0), *((unsigned int *)device->virt_addr0) );
		info("virt_addr00[7c]:    0x%x", *((volatile unsigned int*)(device->virt_addr0 +0x7c)) );
		argVal = *((volatile unsigned int*)device->virt_addr0);
		ret = copy_to_user ((void*)arg, (void*)&argVal, sizeof(unsigned int)); 
		break;
	case IOCTL_CLTU_WRITE: break;
	case IOCTL_CLTU_INTERRUPT_ATTACH:
#if 0
		err =  copy_from_user(&IntCallback, (CLTU_INTERRUPT_CALLBACK*)arg, sizeof(CLTU_INTERRUPT_CALLBACK)) ;
		if (err != WR_OK) {
			printk("copy_from_user failed\n");
			return err;
		}
		
/*		err = copy_to_user ((void*)arg, (void*)&kio, sizeof(dpio2_kio_t)); */


/*			IntCallback = (CLTU_INTERRUPT_CALLBACK*) arg; */
			intLTU_device.callback_sem = IntCallback.sem_cb;
/*			callback_sem =  (intLTU_device )(arg); 
			printk(KERN_ALERT "callback_sem: %d\n", callback_sem); */
/*			pci_read_config_byte(device->pcidev, PCI_INTERRUPT_LINE, &device->interrupt_line); */
#endif
			break;
	case IOCTL_CLTU_INTERRUPT_ENABLE:
/*		info("interrupt enable, base: 0x%x", *((unsigned int *)device->virt_addr0)); */
		/* Copy user buffer */
		ret = copy_from_user (&argVal, (int *)arg, sizeof(unsigned int));
		if (ret != 0) 
		{
			err("copy_from_user failed");
//			up(&device->sem);
//			return ret;
		}

		writel(0xffffffff, device->virt_addr2 + 0x1c); 
		 
//		info("virt_addr00(68): before,  0x%08x", *((volatile unsigned int*)(device->virt_addr0+0x68)) );
		/* Write enable interrupt   */
		nval = 0x10900;
//		info("will write 0x%08x to 0x68\n", nval); 
/*		writel(0x9010f, device->virt_addr0 + 0x68);*/ 
		writel(nval, device->virt_addr0 + 0x68); 
//		info("virt_addr00(68): current,   0x%08x", *((volatile unsigned int*)(device->virt_addr0+0x68)) );


#if 0
		writel(0x0, device->virt_addr0 + 0x68);
#endif
		break;

	case IOCTL_CLTU_INTERRUPT_DISABLE:
		/* Copy user buffer */
		ret = copy_from_user (&argVal, (int *)arg, sizeof(unsigned int));
		if (ret != 0) 
		{
			err("copy_from_user failed");
//			up(&device->sem);
//			return ret;
		}

/*		info("Disable INT. arg : %d", argVal);  */

		nval = readl( device->virt_addr0+ 0x68 );
//		info("virt_addr00(68): before,    0x%08x\n", nval); 
		/* Write disable interrupt   */
		nval &= ~0x10900;
//		info("will write 0x%08x to 0x68\n", nval); 
		writel(nval, device->virt_addr0 + 0x68);
//		nval = readl( device->virt_addr0+ 0x68 );
//		info("virt_addr00(68): current,    0x%08x\n", nval); 
		break;

		
	default:
		ret = -ENOTTY;
		break;
	}

	if( ret != 0 ) err("ret: %d", ret);
	
//	up(&device->sem);
	return ret;
}

ssize_t intLTU_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	CLTU_Device* device;
	ssize_t			ret;
	WAIT_QUEUE_ENTRY_T	wait;

	/* Get intLTU data struct pionter */
	if (!filp->private_data) 
	{
		err ("intLTU_read() : filp->private_data = NULL pointer");
		return ( -EFAULT );
	}
	else
		device = (CLTU_Device*)filp->private_data;

	if (device == NULL)
	{
		/* The referenced device doesn't exist. */
		ret	= -ENODEV;
		return ret;
	}

	memset(&wait, 0, sizeof(wait));
	WAIT_QUEUE_ENTRY_INIT(&wait, current);	/* NOTES */
	SET_CURRENT_STATE(TASK_INTERRUPTIBLE);	/* NOTES */
	add_wait_queue(&device->queue, &wait);		/* NOTES */

//	info("goto sleep..  cnt:%d",device->interrupt_count[0]);
	

	EVENT_WAIT_IRQ_TO(device->queue, device->condition, HZ*30);
	remove_wait_queue(&device->queue, &wait);	/* NOTES */
	SET_CURRENT_STATE(TASK_RUNNING);	/* NOTES */

//	info("return to run.  cnt:%d",device->interrupt_count[0]);

	

/*
	if( device->block_flag == 1 ) {
		err ("device->block_flag == 1");
		return -1;
	}	
*/

//	ret = copy_to_user ((void*)buf, (const void *)device->read_cnt, count);

#if USE_READ_OUT_HIGH
	writel(0x1, device->virt_addr2 + 0x4);
#endif
#if USE_READ_OUT_LOW
	writel(0x0, device->virt_addr2 + 0x4); /* Trig #1 output low*/
#endif


	return count;
}


#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
irqreturn_t intLTU_isr(int irq, void* dev_id)
{
/*	int t_msb; */
	
	CLTU_Device* device = (CLTU_Device*)dev_id;

//	info("catch first, %d",device->interrupt_count[0]);

	/* Check for NULL pointer */
	if ( device->virt_addr0 == NULL) 
	{
		err("NULL pointer!, device->virt_addr0");
		return IRQ_NONE;
	}


//	device->read_cnt = readl( device->virt_addr2 + 0x8 );

	writel(0xffffffff, device->virt_addr2 + 0x1c); /* disable INTA */

#if USE_ISR_TRG1_HIGH
	writel(0x1, device->virt_addr2 + 0x4); /* Trig #1 output high*/
#endif

//	device->interrupt_count[0]++;

//	info("resume task, %d",device->interrupt_count[0]);
	EVENT_RESUME_IRQ(&device->queue, device->condition);



//	prev_msb = device->read_cnt ;
#if USE_ISR_TRG1_LOW	
	writel(0x0, device->virt_addr2 + 0x4);  /* Trig #1 output low*/
#endif

	return IRQ_HANDLED;
}
#endif

int intLTU_open(struct inode *inode, struct file *filp)
{
	int err = 0;
	CLTU_Device* device = intLTU_lookup_device(inode);

	if (device == 0){
		return -ENODEV;
	}

	/* Set filp->private_data  */
	filp->private_data = device;

	device->block_flag = 0 ;

	return err;
}

static int intLTU_map_pci_memory (CLTU_Device  *device)
{
	int erret=0;
	int i;
	u32 flag0, flag2;

	/* get Memory address and resigstration ...... */
	device->phys_pci_bar0 = pci_resource_start( device->pci_dev, 0);
	device->phys_pci_bar1 = pci_resource_start( device->pci_dev, 1);
	device->phys_pci_bar2 = pci_resource_start( device->pci_dev, 2);

	device->phys_pci_bar0_len= pci_resource_len( device->pci_dev, 0);
	device->phys_pci_bar2_len= pci_resource_len( device->pci_dev, 2);

	flag0 = pci_resource_flags(device->pci_dev, 0);
	flag2 = pci_resource_flags(device->pci_dev, 2);

	
	

			if (flag0  & IORESOURCE_IO)
			{ 
				err("BAR0 is I/O mapped and MUST be memory mapped\n");
				return -1;
			}
			if (flag2  & IORESOURCE_IO)
			{ 
				err("BAR2 is I/O mapped and MUST be memory mapped\n");
				return -1;
			}
			i = check_mem_region(device->phys_pci_bar0, device->phys_pci_bar0_len);
			if (i != 0) {
				err("BAR0 check_mem_region().... failed!");
				return -1;
			}
			i = check_mem_region(device->phys_pci_bar2, device->phys_pci_bar2_len);
			if (i != 0) {
				err("BAR2 check_mem_region().... failed!");
				return -1;
			}
			/*	skip request_mem_region */
					
	
			device->virt_addr0= ioremap_nocache(device->phys_pci_bar0, device->phys_pci_bar0_len);
			device->virt_addr2= ioremap_nocache(device->phys_pci_bar2, device->phys_pci_bar2_len);
#if 0
			if( device->virt_addr0!= NULL) {
				info("virt_addr00[0]:    0x%x, addr: 0x%x", *((volatile unsigned int*)device->virt_addr0), (unsigned int)device->virt_addr0 );
				info("virt_addr00(4):    0x%x", *((volatile unsigned int*)(device->virt_addr0+0x4)) );
				info("virt_addr00(8):    0x%x", *((volatile unsigned int*)(device->virt_addr0+0x8)) );
				info("virt_addr00(c):    0x%x", *((volatile unsigned int*)(device->virt_addr0+0xc)) );
				info("virt_addr00(64):    0x%x", *((volatile unsigned int*)(device->virt_addr0+0x64)) );
				info("virt_addr00(68):    0x%x", *((volatile unsigned int*)(device->virt_addr0+0x68)) );

			} 
			else {
				err("Get base0 address failed!" );
				return -1;
			}
#endif


	return erret;

}


static void intLTU_device_create(CLTU_Device* device, struct pci_dev *pci_dev)
{
	devices[idx] = device;
	device->pci_dev = pci_dev;
	device->idx = idx++;
	sprintf(device->ldev.drv_name, "intLTU.%d", device->idx );

#if 1
	pci_set_master(pci_dev);
#endif

	
	intLTU_map_pci_memory(device);


//	info("call sema_init()");
	sema_init(&device->sem, 1);
//	info("call init_waitqueue_head()");
	WAIT_QUEUE_HEAD_INIT( &device->queue);

//	info("good.");

/*	return device; */
}


static void intLTU_set_dev_type(
	CLTU_Device* device, const struct pci_device_id *ent)
{
	if ( ent->vendor == CLTU_VENDOR_ID )
	{
		if( ent->device == CLTU_DEVICE_ID ) 
		{
			device->card_ver = VERSION_R1;
			sprintf(device->model_name, "%s", "intLTU_R1");
			info("Old (first version)");
		} 
		else if( ent->device == CLTU_DEVICE_ID_2 ) 
		{
			device->card_ver = VERSION_R2;
			sprintf(device->model_name, "%s", "intLTU_R2");
		}
		else
			err( "It's not correct device! 0x%x\n", ent->device);
	} 
	else if ( ent->vendor == CLTU_VENDOR_ID_2  &&  ent->device == CLTU_DEVICE_ID_2 )
	{
		device->card_ver = VERSION_R2;
		sprintf(device->model_name, "%s", "intLTU_R2");
		info("New (second version).");
	} 
	else
	{
		err( "It's not correct vendor! 0x%x\n", ent->vendor);
	}

}

static int __devinit
intLTU_device_init(CLTU_Device *device)
{
	static struct file_operations intLTU_proc_ops = 
	{
		 .owner   = THIS_MODULE,
		 .open     = intLTU_open,
		 .release = intLTU_release,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,36))
		 .unlocked_ioctl = intLTU_ioctl,
#else
		 .ioctl = intLTU_ioctl,
#endif
		 
		 .mmap = intLTU_mmap,
		 .read = intLTU_read
	};
	int rc;
	u32 addr;         /* Temp for addr value                   */
	char result;


	rc = pci_enable_device(device->pci_dev);
	if (rc != 0){
		err("pci_enable_device FAILED returning %d", rc);
		return rc;
	}

	rc = register_chrdev(device->ldev.major = 0, 
			     device->ldev.drv_name, 
			     &intLTU_proc_ops);

	if (rc < 0){
		err( "can't get major %d", device->ldev.major );
		return rc;
	}else{
		device->ldev.major = rc;
/*		info( "device major set %d", device->ldev.major ); */
	}

/*
 * if interrupts are requested globally, AND the board is wanting an interrupt,
 * ask Linux for the use thereof
 */
 #if 1
	if (intLTU_use_interrupts != 0 && device->pci_dev->irq != 0)
	{

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,19))
		rc = request_irq(device->pci_dev->irq, (void *)intLTU_isr, SA_SHIRQ, device->ldev.drv_name, device); 
#else
		rc = request_irq( 
			device->pci_dev->irq, 
			(void *)intLTU_isr, 
			IRQF_SHARED, /* IRQF_DISABLED | IRQF_SHARED */
			device->ldev.drv_name, 
			device
		);
#endif

		if (rc != 0){
			err("request_irq %d FAILED", device->pci_dev->irq);
			return rc;
		}

	}
#endif
/*
	if (device->mbox_sync.using_interrupt){
		rc = acq200_sendCommand(device, 
			   BP_SET_FUNCODE(BP_FC_SET_INTERRUPTS)+BP_SET_A1(3));
		if (rc != 0){
			err("BP_FC_SET_INTERRUPTS failed %d", rc);
			return rc;
		}
	}

	if (device->completed_onetime_setups == 0){
		unsigned funcode = 
			BP_SET_FUNCODE(BP_FC_SET_HOST_DMABUF)|
			BP_SET_A1(NMAPPINGS);
		rc = acq200_sendCommandA3A4(device,
			funcode,
			(u32)device->dma_buf.pa, 
			dma_buf_len(&device->dma_buf));
		device->completed_onetime_setups = 1;

		if (rc != 0){	
			err("BP_FC_SET_HOST_DMABUF failed %d", rc);
			return rc;
		}

		rc = acq200_getAvailableChannels(device);
	}
*/
#if 0
	acq200_create_sysfs_device(&device->pci_dev->dev);

	device->class_dev = CLASS_DEVICE_CREATE(
		acq200_device_class,			/* cls */
		NULL,					/* cls_parent */
		device->idx,				/* "devt" */
		&device->pci_dev->dev,			/* device */
		"acq200.%d", device->idx);		 /* fmt, idx */

	acq200_create_sysfs_class(device->class_dev);
#endif



	result = pci_read_config_dword( device->pci_dev, PCI_BASE_ADDRESS_0, &addr );
	if( result != PCIBIOS_SUCCESSFUL )
	{
		err( "%s%d PCI Config Mem Address read error.\n",
			device->model_name, device->card_ver );
		return( result );
	}
	device->phys_addr_bar0 = addr & PCI_BASE_ADDRESS_MEM_MASK;

	info( "phys_pci_bar0:    0x%x", device->phys_pci_bar0);
	info( "phys_addr_bar0:    0x%x", device->phys_addr_bar0);



	return rc;	
}





static int __devinit
intLTU_probe(struct pci_dev *dev, const struct pci_device_id *ent)
{
	CLTU_Device* device;
	info("Found CLTU device!");

	device = kmalloc(sizeof(CLTU_Device), GFP_KERNEL);
	if( device == NULL)
	{
		err("kmalloc() failure.");
		return 1;
	}
		
	
	assert(device);
	memset(device, 0, sizeof(CLTU_Device));
	assert(idx < MAXDEV);

	intLTU_set_dev_type(device, ent); 

	intLTU_device_create(device, dev);

	return intLTU_device_init(device);
}


static void intLTU_remove (struct pci_dev *dev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))
	CLTU_Device *device = intLTU_lookupDevice(&dev->dev);
#else
	CLTU_Device *device = intLTU_lookupDevice(dev);
#endif
	
	if (device != 0)
	{
/*		int used_interrupts = device->mbox_sync.using_interrupt != 0; */
		if( intLTU_use_interrupts )
		{
			/* Write disable interrupt   */
/*			writel(0x0, device->virt_addr0 + 0x68);  */
			
			free_irq(device->pci_dev->irq, device);
/*			device->mbox_sync.using_interrupt = 0; */

			info ("\"%s\" free_irq() ok!", device->ldev.drv_name);

		}

		unregister_chrdev(device->ldev.major, device->ldev.drv_name);

		if (device->virt_addr0) 
		{
			iounmap (device->virt_addr0);
			info ("\"%s\" iounmap ok!", device->ldev.drv_name);
			device->virt_addr0 = NULL;
		}

/*		dbg(1, "free_irq %s", used_interrupts? "YES": "NO"); */
/*
		acq200_remove_sysfs_class(device->class_dev);
		acq200_remove_sysfs_device(&device->pci_dev->dev);
*/

		if (device->pci_dev)
			pci_disable_device(device->pci_dev);


		kfree(device);

		info("device free OK.");
	}
	
}



/*
 *
 * { Vendor ID, Device ID, SubVendor ID, SubDevice ID,
 *   Class, Class Mask, String Index }
 */
static struct pci_device_id intLTU_pci_tbl[] __devinitdata = {
	{ CLTU_VENDOR_ID, CLTU_DEVICE_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{ CLTU_VENDOR_ID_2, CLTU_DEVICE_ID_2, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{  }
};
MODULE_DEVICE_TABLE(pci, intLTU_pci_tbl);

static struct pci_driver intLTU_pci_driver = {
	.name     = intLTU_driver_name,
	.id_table = intLTU_pci_tbl,
	.probe    = intLTU_probe,
	.remove   = __devexit_p(intLTU_remove)
/*	
#ifdef PGMCOMOUT
	// Power Managment Hooks 
#ifdef CONFIG_PM
	.suspend  = intLTU_suspend,
	.resume   = intLTU_resume
#endif
#endif
*/
};


/*static int __init intLTU_init(void) */
static int intLTU_init_module(void)
{
	int rc;

	printk(KI "\n");
	info("%s\n%s", 
	     intLTU__driver_string, intLTU__copyright);
	printk(KI "Driver built %s\n", __DATE__ );
	
/*	intLTU_device_class = class_create(THIS_MODULE, "intLTUR1"); */
	rc = pci_register_driver( &intLTU_pci_driver);
	return 0;
}

void intLTU_exit_module(void)
{
	if (!idx) return;

	
	pci_unregister_driver(&intLTU_pci_driver);
/*	class_destroy(intLTU_device_class); */
	info("Driver unloaded!");
}


module_init(intLTU_init_module);
module_exit(intLTU_exit_module);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("WoongR.Lee, NFRI");
MODULE_DESCRIPTION("Driver for interrupt LTU");


