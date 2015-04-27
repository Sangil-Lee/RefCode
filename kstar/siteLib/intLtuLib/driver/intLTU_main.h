#ifndef _CLTU_LINUX_KERNEL_H_
#define _CLTU_LINUX_KERNEL_H_

#ifdef __cplusplus
	extern "C" {
#endif




#include "intLTU_Def.h"
#include "intLTU_header.h"


/*
#ifndef DRV_OK
#define DRV_OK      (0)
#define DRV_ERROR   (-1)
#endif
*/

#define CLTU_VENDOR_ID (0x10B5)
#define CLTU_DEVICE_ID (0x9601)

#if 1
#define CLTU_VENDOR_ID_2 (0x10EE)
#define CLTU_DEVICE_ID_2 (0x0007)
#endif



/* kernel compatibility ugliness */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))

#define CLASS_DEVICE		device
#define CLASS_DEVICE_CREATE	device_create
#define CLASS_DEVICE_ATTR	DEVICE_ATTR
#define CLASS_DEVICE_CREATE_FILE DEVICE_CREATE_FILE
#else
#define ORIGINAL_CLASS_DEVICE_INTERFACE	1
#define CLASS_DEVICE		class_device
#define CLASS_DEVICE_CREATE	class_device_create

#endif



#define VERSION_R1  0x0100
#define VERSION_R2  0x0200

#define RDWR_DEV_NAME "intLTU"
#define RDWR_DEV_MAJOR 240





#if	(LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11))
	#define	REMAP_PAGE_RANGE(vma,vm_adrs,ph_adrs,size,prot)		\
			remap_pfn_range( vma,vm_adrs,ph_adrs >> PAGE_SHIFT,	\
								size,prot)
#else
	#define	REMAP_PAGE_RANGE(v,f,t,s,p)  remap_page_range(v,f,t,s,p)
#endif




#if 0
/** device descriptor, one per device */
typedef struct st_CLTU_Device {
	int idx;				/**< card index in system. */
	int card_ver;		/* VERSION_R1,   VERSION_R2 */
	char model_name[KNAME];

	struct pci_dev *pci_dev;		/**< linux pci generic.    */
	struct CLASS_DEVICE *class_dev;		/**< class generic.	   */
	
	struct LogicalDev {
		int major;
		char drv_name[KNAME];
	} ldev;

	unsigned char curPort;
/*
	struct DmaBuf dma_buf;
	struct DmaBuf AB[2];
	struct BigBufPool* pool;
*/

	u32 phys_pci_bar0;
	u32 phys_pci_bar1;
	u32 phys_pci_bar2;

	u32 phys_pci_bar0_len;
	u32 phys_pci_bar2_len;

	u32 phys_addr_bar0;
	
/* unsigned long   phys_pci_bar0; */
	void *virt_addr0;

	unsigned int interrupt_count[2];
	int block_flag;
		
/*
	struct IoMapping {
		u32* va;
		unsigned long pa;
		int len;
		char name[KNAME];
	}
		csr, ram, rom;

	u32 imask;

	struct MailboxSync {
		long using_interrupt;
		struct semaphore mutex;
		int response_received;

		wait_queue_head_t waitq;
	} mbox_sync;

	struct Clients {
		struct semaphore mutex;
		int count;
	} clients;

	struct CHANNEL_INFO {
		unsigned nchannels;        // #channels available
		short vmax10;              // vmax (volts*10)
		short vmin10;              // vmin (volts*10)
	}
		input_channels, output_channels;
*/

#if 0
	struct DevicePrivateData {
		int i2o_packets_incoming;
		int i2o_packets_returned;
		int i2o_packets_discarded;
		void* i2o_last_in;
		
		int i2o_packets_outgoing;       // # command packets sent
		short i2o_last_id_out;              // last id code

		struct rch {
			spinlock_t lock;
			struct list_head list;
		} rch;

		int streaming_enabled;

		struct tasklet_struct streaming_isr_tasklet;

		unsigned aux_mfa;
		wait_queue_head_t aux_waitq; 
		int aux_task_waiting;

		unsigned bad_mfa_count;
	} m_dpd;

	struct StreamBuf streambuf;
	struct task_struct *i2o_backstop;
#endif
}
CLTU_Device;

#endif


/*
typedef struct int_arg_t
{
	struct semaphore sem_cb;
	int nVal;
	
} CLTU_INTERRUPT_CALLBACK;
*/

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,1))

CLTU_Device* intLTU_lookupDevice(struct device *dev);
/*CLTU_Device* intLTU_lookupDeviceFromClass(struct CLASS_DEVICE *dev); */
/*
void intLTU_create_sysfs_device(struct device *dev);
void intLTU_remove_sysfs_device(struct device *dev);
void intLTU_create_sysfs_class(struct CLASS_DEVICE *dev);
void intLTU_remove_sysfs_class(struct CLASS_DEVICE *dev);
*/
#else

CLTU_Device* intLTU_lookupDevice(struct pci_dev *dev);

#endif

#if 0
extern void intLTU_device_create_file(
	struct device * dev, struct device_attribute * attr,
	const char *file, int line);

#define DEVICE_CREATE_FILE(dev, attr) \
	intLTU_device_create_file(dev, attr, __FILE__, __LINE__)
#endif

#ifdef __cplusplus
}
#endif 
#endif /* _IOCTL_CLTU_H_ */

