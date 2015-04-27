#ifndef _CLTU_HEADER_H_
#define _CLTU_HEADER_H_

#ifdef __cplusplus
	extern "C" {
#endif

#ifndef KNAME
#define KNAME	32
#endif

#include <semaphore.h>

#include "cltu_Def.h"

/** device descriptor, one per device */
typedef struct st_CLTU_Device {
	int idx;				/**< card index in system. */
	int card_ver;		/* VERSION_R1,   VERSION_R2 */
	char model_name[KNAME];

	struct pci_dev *pci_dev;		/**< linux pci generic.    */
	struct CLASS_DEVICE *class_dev;		/**< class generic.	   */

	struct semaphore		sem;
	
	struct LogicalDev {
		int major;
		char drv_name[KNAME];
	} ldev;

	unsigned char curPort;
	unsigned int curHLI;
	unsigned int curLLI;
	unsigned int node_id;
/*	unsigned int u32Type; */

/****************************************************************** 2013. 3. 7 ***/
/*	unsigned int irigb_sel;
	unsigned int mode;
	unsigned int tlkRxClk_en;
	unsigned int tlkTxClk_sel;
	unsigned int mclk_sel; */
	ozXTU_MODE_T xtuMode;
	ozPCONFIG_T xtu_pcfg[NUM_PORT_CNT];
	unsigned int shot_delay_ns;
	unsigned int read_offset;
/* 2013. 3. 7 ********************************************************************/	

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
	void *virt_addr;

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


#ifdef __cplusplus
}
#endif 
#endif /* _IOCTL_CLTU_H_ */

