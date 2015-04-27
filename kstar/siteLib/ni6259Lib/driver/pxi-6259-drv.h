/*****************************************************************************
 * pxi-6259-drv.h
 *
 * Definitions for the NI PXI-6259 driver.
 * Author: Janez Golob (Cosylab)
 * Copyright (C) 2010-2013 ITER Organization
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of the Linux
 * distribution for more details.
 ****************************************************************************/

#ifndef _PXI_6259_DRV_H_
#define _PXI_6259_DRV_H_

#include <linux/spinlock.h>
#include <linux/wait.h>
#include "pxi6259.h"
#include "pxi-6259-reg.h"

#define MAX_QUEUE_SIZE 32

/* Number of registers to emulate */
#ifndef REGISTERS
#define REGISTERS 256
#endif

/* Analog input channel mite offset */
#define AI_CHAN_MITE_OFFSET 0x500
/* Analog input data width */
#define AI_CHAN_MITE_DATA_WIDTH 0x2
/* Analog output channel mite offset */
#define AO_CHAN_MITE_OFFSET 0x600
/* Analog output data width */
#define AO_CHAN_MITE_DATA_WIDTH 0x2

/* How many times it is allowed to check status */
#define MAX_AI_SCAN_STATUS_REP 0xfff

/*
 * TODO: replace custum queue with kfifo when we will
 * use kernel version > 2.6.33. kfifo will contain a lot of
 * helper functions like copy to user and from user - also
 * all locking is working correctly.
 *
 * Flush queue after release.
 */
struct queue {
        uint8_t queue_off;
        uint32_t size;
        uint32_t missed;
        uint16_t read_index;
        uint16_t write_index;
        uint8_t timeout_flag;                           /* This flag is set by timeout handler */
        spinlock_t lock;                                /* Spinlock used by queue */
        wait_queue_head_t outq;                         /* Read queue */
        struct pxi6259_read_buffer entries[MAX_QUEUE_SIZE];
};


/* DMA related definitions */

enum dma_mode {
        DMA_MODE_NORMAL = 0,
        DMA_MODE_RING,
        DMA_MODE_LINK_CHAIN,
        DMA_MODE_LINK_CHAIN_RING,
        DMA_MODE_NONE,
};

enum dma_state {
        DMA_UNKNOWN,
        DMA_IDLE,
        DMA_CONFIGURED,
        DMA_STARTED,
        DMA_STOPPED
};

/*
    nNIRLP_tDMA

    Holds information on a DMA buffer.  Use tPCIDevice factory functions to create/destroy
    DMA buffers.

    The tDMA descriptor holds two addesses: cpu and bus:
    - cpu is the virtual address usable by the kernel to access the buffer.
    - bus is the address passed to the device.  Note that this address may not be the
      physical address as seen by the CPU.  If the system has an IOMMU, the kernel may
      map a 32-bit bus address to a 64-bit physical CPU address.

      If you need the physical cpu address use __pa(address.cpu)
*/

struct dma_buffer
{
        struct list_head link;
        size_t size;
        struct {
                void       *cpu;
                dma_addr_t  bus;
        } address;

        u64 location; // used only for writing and it is common to all clients
};

struct dma_channel {
        u8            mite_channel;
        u32           mite_offset;
        enum dma_data_direction direction;
        enum dma_mode mode;
        enum dma_state state;
        u32           drq;
        u8            xfer_width;
        u32           size;
        struct dma_buffer *buffer;
};

struct dma_cli_info {
        u64 write_idx;     /* Index point to the position where DMA writes/reads (points to the next free space)*/
        u64 last_write_idx;
        u64 read_idx;
        u8  wait_write_idx; /* This indicates we are waiting for the write index */
        u64 location;

        u64 real_dar; //64-bit "real" DAR value
        u32 last_dar; //DAR value from previous snapshot

        u32 number_of_samples; //Number in buffer, used for AO write
};

enum ai_state {
        AI_STATE_UNKNOWN,       /* The AI segment is not jet initialized */
        AI_STATE_CONFIGURED,    /* The configuration has been loaded into AI segment */
        AI_STATE_STARTED,       /* The AI segment has been started */
};

enum ao_state {
        AO_STATE_UNKNOWN,       /* The AO segment is not jet initialized */
        AO_STATE_CONFIGURED,    /* The configuration has been loaded into AI segment */
        AO_STATE_STARTED,       /* The AI segment has been started */
};

enum dio_state {
        DIO_STATE_UNKNOWN,       /* The DIO segment is not jet initialized */
        DIO_STATE_CONFIGURED,    /* The configuration has been loaded into AI segment */
};

enum gpc_state {
        GPC_STATE_UNKNOWN,       /* The AO segment is not jet initialized */
        GPC_STATE_CONFIGURED,    /* The configuration has been loaded into AI segment */
        GPC_STATE_STARTED,       /* The AI segment has been started */
};

struct aichan_client {
        pid_t pid;
        struct module_context *mcon;     /* Pointer to module context */
        struct dma_cli_info ai_dma_info; /* Info neded to read/write dma buffer */
        wait_queue_head_t wait_queue;
};

struct aochan_client {
        pid_t pid;
        struct module_context *mcon;     /* Pointer to module context */
        wait_queue_head_t wait_queue;
};

struct diochan_private {
        struct module_context *mcon;     /* Pointer to module context */
        unsigned int chan_number; /* AO channel number */
};

struct gpcchan_private {
        struct module_context *mcon;     /* Pointer to module context */
        unsigned int chan_number; /* AO channel number */
};

struct aichan_listitem {
        struct aichan_listitem *key;
        struct aichan_client cli;
        struct module_context *mcon;
        pid_t pid;
        unsigned int chan_number;
        unsigned int chan_total;
        unsigned int dma_idx;
        struct list_head list;
};

struct aochan_listitem {
        struct aochan_listitem *key;
        struct aochan_client cli;
        struct module_context *mcon;
        pid_t pid;
        unsigned int chan_number;
        unsigned int chan_total;
        unsigned int dma_idx;
        struct list_head list;
};

/*
 * Module context.
 */
struct module_context {
        int32_t index;

        uint32_t client_connection_mask;       /* Mask representing connected clients */

        uint32_t registers[REGISTERS];         /* Copy of module registers, Emulation and Reset */

        enum pxi6259_debug_flag debug;         /* Global debug options          */
        enum pxi6259_standard_status status;   /* Standard status               */

        struct mutex ai_mutex;                 /* Lock to protect ai module */
        struct mutex ao_mutex;                 /* Lock to protect ao module */
        struct mutex dio_mutex;                /* Lock to protect dio module */
        struct mutex gpc_mutex;                /* Lock to protect gpc module */

        struct pxi6259_module_address addresses;/* Addresses representing PCI bars */

        uint8_t register_space[MAX_M_OFFSET];   /* Register space used to write/flush/set_bits functions */

        struct pxi6259_ai_conf ai_conf;
        struct pxi6259_ao_conf ao_task;
        struct pxi6259_gpc_conf gpc0_task;
        struct pxi6259_gpc_conf gpc1_task;
        struct pxi6259_dio_conf dio_task;

        enum ai_state ai_state;
        enum ao_state ao_state;
        enum gpc_state gpc0_state;
        enum gpc_state gpc1_state;
        enum dio_state dio_state;

        /* Segment connection masks */
        uint64_t ai_cli_mask;
        uint64_t dio_cli_mask;
        uint64_t ao_cli_mask;
        uint64_t gpc0_cli_mask;
        uint64_t gpc1_cli_mask;

        /* DMA Channel configuration */
        struct dma_channel ai_chan;
        struct dma_channel ao_chan;
        struct dma_cli_info ao_cli_info[PXI6259_MAX_AO_CHANNEL + 1];

        struct pci_dev *pci_dev;
        struct cdev cdev;               /* Char device structure */
        struct device *dev;

        /* Subdevices */
        struct cdev aicdev;
        struct device *aidev;
        struct cdev aichancdev[PXI6259_MAX_AI_CHANNEL + 1];
        struct device *aichandev[PXI6259_MAX_AI_CHANNEL + 1];
        struct cdev aocdev;
        struct device *aodev;
        struct cdev aochancdev[PXI6259_MAX_AO_CHANNEL + 1];
        struct device *aochandev[PXI6259_MAX_AO_CHANNEL + 1];
        struct cdev diocdev;
        struct device *diodev;
        struct cdev diochancdev[PXI6259_MAX_DIO_PORT + 1];
        struct device *diochandev[PXI6259_MAX_DIO_PORT + 1];
        struct cdev gpccdev;
        struct device *gpcdev;
        struct cdev gpcchancdev[PXI6259_MAX_GPC + 1];
        struct device *gpcchandev[PXI6259_MAX_GPC + 1];

        /* Channel context lists */
        struct list_head aichan_list;
        int32_t aichanclients;
        struct list_head aochan_list;
        int32_t aochanclients;

        /* EEPROM values */
        uint8_t cal_eeprom[PXI6259_EEPROM_SIZE]; //calibration EEPROM

};


#define DRIVER_NAME_SIZE 30

/*
 * Driver working area
 */
struct driver_working_area {
        char name[DRIVER_NAME_SIZE];                    /* Driver name */
        uint32_t installed_modules;                     /* Number of installed modules */
        struct module_context *modules;                 /* Module contexts */
};

/*
 * Split minors in two parts
 */
#define TYPE(minor)     (((minor) >> 4) & 0xf)  /* high nibble */
#define NUM(minor)      ((minor) & 0xf)         /* low  nibble */

#define BAR0 0
#define BAR1 1
#define BAR2 2


#endif /* _PXI_6259_H_ */
