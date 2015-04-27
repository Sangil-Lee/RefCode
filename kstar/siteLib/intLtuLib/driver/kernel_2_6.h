// This software is covered by the GNU GENERAL PUBLIC LICENSE (GPL).
// $URL: http://subversion:8080/svn/gsc/trunk/drivers/LINUX/16AISS8AO4/driver/kernel_2_6.h $
// $Rev: 2991 $
// $Date: 2009-08-25 08:48:25 -0500 (Tue, 25 Aug 2009) $

/*
*	Provide special handling for things specific to the 2.6 kernel.
*/

#include <linux/moduleparam.h>
#include <linux/slab.h>



/* #defines	**************************************************************/

#define	CURRENT_PROCESS_ID			current->tgid
#define	EVENT_RESUME_IRQ(q,c)		c = 1; wake_up_interruptible(q)
#define	EVENT_WAIT_IRQ_TO(q,c,t)	(c)=0;wait_event_interruptible_timeout((q),(c),(t))
#define	GET_PAGE(page)				get_page((page))
#define	KILL_FASYNC(q,s,b)			kill_fasync(&(q),(s),(b))
#define	KMALLOC_LIMIT				(128L * 1024)
#define	MEM_ALLOC_LIMIT				(2L * 1024L * 1024)
#undef	MOD_DEC_USE_COUNT
#define	MOD_DEC_USE_COUNT
#undef	MOD_INC_USE_COUNT
#define	MOD_INC_USE_COUNT
#define	PAGE_RESERVE(vpa)			SetPageReserved(virt_to_page((vpa)))
#define	PAGE_UNRESERVE(vpa)			ClearPageReserved(virt_to_page((vpa)))

#define	PCI_DEVICE_LOOP(p)	\
	for ((p) = NULL; ((p) = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, (p))); )

#define	PCI_BAR_ADDRESS(p,b)		pci_resource_start((p),(b))
#define	PCI_BAR_FLAGS(p,b)			(pci_resource_flags((p),(b)))
#define	PCI_BAR_SIZE(p,b)			pci_resource_len((p),(b))
#define	PCI_DEVICE_SUB_SYSTEM(p)	((p)->subsystem_device)
#define	PCI_DEVICE_SUB_VENDOR(p)	((p)->subsystem_vendor)
#define	PCI_ENABLE_DEVICE(pci)		pci_enable_device(pci)
#define	PCI_DISABLE_DEVICE(p)		pci_disable_device((p))
#define	REGION_IO_CHECK(a,s)		0
#define	REGION_IO_RELEASE(a,s)		release_region(a,s)
#define	REGION_IO_REQUEST(a,s,n)	request_region(a,s,n)
#define	REGION_MEM_CHECK(a,s)		check_mem_region(a,s)
#define	REGION_MEM_RELEASE(a,s)		release_mem_region(a,s)
#define	REGION_MEM_REQUEST(a,s,n)	request_mem_region(a,s,n)
#define	REGION_TYPE_IO_BIT			IORESOURCE_IO
#define	SET_CURRENT_STATE(s)		current->state = (s)
#define	SET_MODULE_OWNER(p)
#define	VADDR_T						void*
#define	VIRT_TO_PAGE(p)				virt_to_page((p))
#define	VM_OFFSET_BYTES(vma)		((vma)->vm_pgoff * PAGE_SIZE)
#define	VM_OPS_UNMAP(vmo,fun)
#define	WAIT_QUEUE_ENTRY_INIT(w,c)	init_waitqueue_entry((w),(c))
#define	WAIT_QUEUE_ENTRY_T			wait_queue_t
#define	WAIT_QUEUE_HEAD_INIT(q)		init_waitqueue_head((q))
#define	WAIT_QUEUE_HEAD_T			wait_queue_head_t
/*
#if	(LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11))
	#define	REMAP_PAGE_RANGE(vma,vm_adrs,ph_adrs,size,prot)		\
			remap_pfn_range( vma,vm_adrs,ph_adrs >> PAGE_SHIFT,	\
								BYTES_TO_PAGE_BYTES((size)),prot)
#else
	#define	REMAP_PAGE_RANGE(v,f,t,s,p)	remap_page_range(v,f,t,s,p)
#endif
*/






/* prototypes	**************************************************************/


