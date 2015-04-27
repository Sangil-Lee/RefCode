/*****************************************************************************
 * pxi6259.h
 *
 * Common definitions for NI PXI-6259 driver and its library.
 * Author: Janez Golob (Cosylab)
 * Copyright (C) 2010-2013 ITER Organization
 ****************************************************************************/

/**
 * @file
 * This file contains lost of important structure definitions and other defines,
 * that are used in driver and library and can also be used in user programs.
 */

#ifndef PXI6259_H_
#define PXI6259_H_


#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#ifdef __KERNEL__
#include <linux/time.h>  /* timespec */
#else
#include<time.h>
#include<stdint.h>
#endif

#include "pxi6259-enums.h"

#undef PDEBUG            /* undef it, just in case */
#ifdef PXI6259_DEBUG
#  ifdef __KERNEL__
    /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG fmt, ## args)
#  else
    /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PVDEBUG            /* undef it, just in case */
#ifdef PXI6259_VDEBUG
#  ifdef __KERNEL__
    /* This one if debugging is on, and kernel space */
#    define PVDEBUG(fmt, args...) printk( KERN_DEBUG fmt, ## args)
#  else
    /* This one for user space */
#    define PVDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PVDEBUG(fmt, args...) /* not debugging: nothing */
#endif

/* GCC specific definitions */
#  ifndef  __KERNEL__
# if __GNUC__ >= 3
#  define __must_check __attribute__ ((warn_unused_result))
#  define __deprecated __attribute__ ((deprecated))
#  define __packed      __attribute__ ((packed))
#  define likely(x)     __builtin_expect (!!(x), 1)
#  define unlikely(x)   __builtin_expect (!!(x), 0)
# else
#  define __must_check /* no  warn_unused_result */
#  define __deprecated /* no  deprecated */
#  define __packed     /* no  packed */
#  define likely(x)     (x)
#  define unlikely(x)   (x)
# endif
#endif

/* Number of client contexts */
#define PXI6259_CLIENT_CONTEXTS 4
/* Number of client connections */
#define PXI6259_MAX_CLIENT_CONNECTIONS 16
/* Maximum name size */
#define PXI6259_MAX_NAME_SIZE 100
/* Number of address spaces supported by HW (Nr of PCI bars). */
#define PXI6259_ADDRESS_SPACES 2
/* Max number of channels used at once */
#define PXI6259_MAX_CHANNELS 64
/* Number of ports available */
#define PXI6259_NUMBER_OF_PORTS 3

#define PXI6259_NUMBER_OF_AO_CHANNELS   4

#define PXI6259_AI_BUFFER_LENGHT 1024

#define PXI6259_AO_BUFFER_LENGHT 1024

#define PXI6259_DIO_BUFFER_LENGHT 1024

#define PXI6259_EEPROM_SIZE 1024

#define PXI6259_EEPROM_CAL_OFFSET 1024

/* The number of available minor numbers */
#define MINOR_NUMBERS 0xffff

#define DEVICE_MINOR_STEP 50

#define PXI6259_MAX_NUMBER_OF_AO_SAMPLES_PER_SECOND 2860000
#define PXI6259_MAX_NUMBER_OF_AI_SAMPLES_PER_SECOND 1250000            // This is true for single channel, for multichannel is 1.0M

/* Segment signatures - random generated */
#define PXI6259_HANDLE_SIGNATURE 0x57D34A80

#define PXI6259_AI_SIGNATURE 0xA9F34C3D
#define PXI6259_AO_SIGNATURE 0x3A4B5C6D
#define PXI6259_GPC_SIGNATURE 0xB5D7F1C9
#define PXI6259_DIO_SIGNATURE 0x9375BFDA


/* max value of file descriptor, defined by OS */
#define MAX_FILE_DESCRIPTOR 1024

/*
 * EEPROM offsets (Scaling constants)
 */
#define CALIBRATION_AREA_OFFSET         24
#define HEADER_SIZE                     16

#define AI_MODE_BLOCK_START             HEADER_SIZE
#define AI_MODE_BLOCK_SIZE              17
#define AI_MAX_NUMBER_OF_MODES          4
#define AI_MODE_BLOCK_END               (AI_MODE_BLOCK_START + (AI_MODE_BLOCK_SIZE * AI_MAX_NUMBER_OF_MODES))

#define AI_INTERVAL_BLOCK_START         AI_MODE_BLOCK_END
#define AI_INTERVAL_BLOCK_SIZE          8
#define AI_MAX_NUMBER_OF_INTERVALS      7
#define AI_INTERVAL_BLOCK_END           (AI_INTERVAL_BLOCK_START + (AI_INTERVAL_BLOCK_SIZE * AI_MAX_NUMBER_OF_INTERVALS))

#define AO_MODE_BLOCK_START             AI_INTERVAL_BLOCK_END
#define AO_MODE_BLOCK_SIZE              17
#define AO_MAX_NUMBER_OF_MODES          1
#define AO_MODE_BLOCK_END               (AO_MODE_BLOCK_START + (AO_MODE_BLOCK_SIZE * AO_MAX_NUMBER_OF_MODES))

#define AO_INTERVAL_BLOCK_START         AO_MODE_BLOCK_END
#define AO_INTERVAL_BLOCK_SIZE          8
#define AO_MAX_NUMBER_OF_INTERVALS      4
#define AO_INTERVAL_BLOCK_END           (AO_INTERVAL_BLOCK_START + (AO_INTERVAL_BLOCK_SIZE * AO_MAX_NUMBER_OF_INTERVALS))

#define AI_CHANNEL_BLOCK_SIZE           ((AI_MODE_BLOCK_SIZE*AI_MAX_NUMBER_OF_MODES) + (AI_INTERVAL_BLOCK_SIZE*AI_MAX_NUMBER_OF_INTERVALS))
#define AI_MAX_NUMBER_OF_CHANNELS       1

#define AO_CHANNEL_BLOCK_SIZE           ((AO_MODE_BLOCK_SIZE*AO_MAX_NUMBER_OF_MODES) + (AO_INTERVAL_BLOCK_SIZE*AO_MAX_NUMBER_OF_INTERVALS))
#define AO_MAX_NUMBER_OF_CHANNELS       4


enum pxi6259_data_width {
        PXI6259_DW08 = 1,  /* Data width 1 byte */
        PXI6259_DW16 = 2,  /* Data width 2 byte */
        PXI6259_DW32 = 4  /* Data width 4 byte */
};

enum pxi6259_endianness {
        PXI6259_NONE   = 0, /* Not specified, use default for address space */
        PXI6259_BIG    = 1, /* Big Endian hardware module */
        PXI6259_LITTLE = 2  /* Little Endian hardware module */
};

struct pxi6259_standard_address_space {
        unsigned long base_address;     /* Physical address - VME / PCI leaf */
        uint32_t window;                /* Address window size */
        enum pxi6259_data_width  dw;            /* Data width */
        enum pxi6259_endianness en;             /* Endian-ness of hardware */
        void *space;                    /* Address space */
};

struct pxi6259_module_address {
        uint32_t interrupt_vector;                      /* Interrupt vector number */
        uint32_t interrupt_level;                       /* Interrupt level (2 usually) */
        struct pxi6259_standard_address_space spaces[PXI6259_ADDRESS_SPACES];    /* Address spaces */
};

/*
 * Raw IO structure.
 *
 * It is used e.g. with RAW_WRITE and RAW_READ ioctl call.
 */
struct pxi6259_raw_io_block {
        uint32_t bar;          /* Bar to pxi6259_read from   1...6 on PCI */
        uint32_t offset;       /* Hardware address byte offset    */
        void *data;            /* Pointer to callers IO data area */
        enum pxi6259_data_width dw;    /* Data width to be copied in bytes */
};

/*
 * Structure contains client connected to driver (not to module
 * context).
 *
 * It is used e.g. with GET_CLIENT_LIST ioctl call.
 */
struct pxi6259_client_list {
        uint32_t size;                  /* Pids array size */
        pid_t pids[PXI6259_MAX_CLIENT_CONNECTIONS];    /* Array of pids */
};

/*
 * Structure represents connection to the interrupt vector and not
 * connection to module itself.
 *
 * It is used e.g. with CONNECT ioctl call.
 */
struct pxi6259_connection {
        int32_t module_index;       /* The module number 0..n; -1 means disconnected  */
        int32_t client_index;       /* The client number 0..n */
        uint32_t irq_mask;          /* Connection mask bits or index */

        uint8_t bar;                /* PCI bar to use */
        enum pxi6259_data_width dw;         /* 8, 16, 32  I/O or MMIO */

        uint8_t read_flag;          /* If true pxi6259_read */
        uint32_t read_offset;       /* Into bar  -1 if no need to pxi6259_read*/
        uint32_t read_value;        /* Read value */

        uint8_t write_flag;         /* If pxi6259_write 1 */
        uint32_t write_offset;      /* Into bar; -1 if no need to pxi6259_read */
        uint32_t write_value;       /* Value to check against/set */
};

/*
 * Structure represents client connections. As said connection structure
 * represents connection to one or more interrupts.
 *
 * It is used e.g. with GET_CLIENT_CONNECTIONS ioctl call.
 */
struct pxi6259_client_connections {
        pid_t pid;                                              /* Client pid */
        uint32_t size;                                          /* connections array size */
        struct pxi6259_connection connections[PXI6259_MAX_CLIENT_CONNECTIONS];  /* Interrupt connections */
};

/*
 * Structure represents pxi6259_read buffer.
 *
 * It contains interrupted connection. Returned from pxi6259_read() system call.
 *
 * TODO: array?
 */
struct pxi6259_read_buffer {
        uint8_t timeout_flag;           /* If true then the timeout occurred */
        uint16_t group_a_mask;
        uint16_t group_b_mask;
        uint32_t ai_bytes_to_read;
};


/*
 * Global debug tracing options.
 */
enum pxi6259_debug_flag {
        PXI6259_ASSERTION   = 0x01,  /* Assertion violations */
        PXI6259_TRACE       = 0x02,  /* Trace all IOCTL calls */
        PXI6259_WARNING     = 0x04,  /* Warning messages */
        PXI6259_MODULE      = 0x08,  /* Module specific debug warnings */
        PXI6259_INFORMATION = 0x10,  /* All debug information is printed */
        PXI6259_EMULATION   = 0x100  /* Driver emulation ON no hardware */
};

/*
 *  Standard status definitions: A value of ZERO is the BAD state.
 *
 *  It represents module context status.
 */
enum pxi6259_standard_status {
        NO_ISR        = 0x001,   /* No ISR installed */
        BUS_ERROR     = 0x002,   /* Bus error detected */
        HARDWARE_DISABLED = 0x004,   /* Hardware is not enabled */
        HARDWARE_FAIL = 0x008,   /* Hardware has failed */
        WATCH_DOG     = 0x010,   /* Hardware watch dog time out */
        BUS_FAULT     = 0x020,   /* Bus error(s) detected */
        EMULATION     = 0x080,   /* Hardware emulation ON */
        NO_HARDWARE   = 0x100    /* Hardware has not been installed */
};


struct pxi6259_module_status {
        enum pxi6259_standard_status standard_status;
        uint32_t hardware_status;
};

/*
 * Structures needed by HW specific ioctl calls
 */

struct pxi6259_device_name {
        uint32_t module_index;
        uint8_t  device_name[PXI6259_MAX_NAME_SIZE];
};

struct pxi6259_rw_buffer {
        uint32_t rw_bytes;
        uint32_t *rw_bytes_done;
        uint8_t *buffer;
};

struct pxi6259_trigger_connection {
	uint8_t source;
	uint8_t destination;
	uint8_t signal_modifiers;
};

struct pxi6259_ao_fifo_buffer_info {
        uint32_t numberOfSamples;
        uint32_t *ptrToBuffer;
};

typedef union {
    uint8_t     b[4];
    float       f;
} eeprom_float_t;

typedef struct
{
        uint32_t order;
        eeprom_float_t value[4];
} scale_mode_t;

typedef struct
{
        eeprom_float_t gain;
        eeprom_float_t offset;
}interval_t;

typedef struct
{
    uint32_t                    order;
    scale_converter_select_t    converter;
    eeprom_float_t              value[4];
    scale_mode_t                mode;
    interval_t                  interval;
} ai_scaling_coefficients_t;


typedef struct
{
        interval_t              interval[4];
        uint32_t                order[4];
} ao_scaling_coefficients_t;


#define AI_CHANNELS 32

// channel values
enum pxi6259_channel {


        /* Digital Input Output */
        DIO_PORT0 = 0,
        DIO_PORT1 = 1,
        DIO_PORT2 = 2,

        /* General Purpose Counter */
        GPC0 = 0,
        GPC1 = 1,
};

#define PXI6259_MAX_AI_CHANNEL 31
#define PXI6259_MAX_AO_CHANNEL 3
#define PXI6259_MAX_DIO_PORT   2
#define PXI6259_MAX_GPC        1


// terminals
enum pxi6259_terminal {
        PXI6259_RTSI_TERMINAL = 1,
        PXI6259_RTSI0,
        PXI6259_RTSI1,
        PXI6259_RTSI2,
        PXI6259_RTSI3,
        PXI6259_RTSI4,
        PXI6259_RTSI5,
        PXI6259_RTSI6,
        PXI6259_RTSI7,

        PXI6259_PFI_TERMINAL,
        PXI6259_PFI0,
        PXI6259_PFI1,
        PXI6259_PFI2,
        PXI6259_PFI3,
        PXI6259_PFI4,
        PXI6259_PFI5,
        PXI6259_PFI6,
        PXI6259_PFI7,
        PXI6259_PFI8,
        PXI6259_PFI9,
        PXI6259_PFI10,
        PXI6259_PFI11,
        PXI6259_PFI12,
        PXI6259_PFI13,
        PXI6259_PFI14,
        PXI6259_PFI15
};

enum pxi6259_signal {
        PXI6259_AI_SAMPLE_CLOCK = 1,
        PXI6259_AI_CONVERT_CLOCK,
        PXI6259_AI_START_TRIGGER,
        PXI6259_AI_REFERENCE_TRIGGER,
        PXI6259_AO_SAMPLE_CLOCK,
        PXI6259_AO_START_TRIGGER,
        PXI6259_GPC0_OUTPUT,
        PXI6259_GPC1_OUTPUT,
        PXI6259_PFI_DO,
};


// attributes

enum pxi6259_channel_attribute {
        /* Analog Input Channel Settings */
        AI_POLARITY,
        AI_GAIN,
        AI_CHANNEL_TYPE,
        AI_DITHER,

        /* Analog Output Channel Settings */
        AO_POLARITY,
        AO_REFERENCE_SELECT,
        AO_OFFSET_SELECT,
        AO_UPDATE_MODE,
        AO_WAVEFORM_ORDER,

        /* Digital Input Output Channel Settings */
        DIO_PORT_MASK,
        DIO_PIN_INPUT,
        DIO_PIN_OUTPUT,
};

enum pxi6259_segment_attribute {
        AI_NUMBER_OF_CHANNELS,

        AI_NO_OF_PRE_TRIG_SAMPLES,
        AI_NO_OF_POST_TRIG_SAMPLES,

        AI_CONTINUOUS,
        AI_EXTERNAL_CONVERT_CLOCK,
        AI_SIP_OUTPUT_SELECT,

        AI_EXTERNAL_MUX_PRESENT,
        AI_EXTMUX_OUTPUT_SELECT,
        AI_EXTERNAL_GATE_SELECT,

        /* Start trigger settings */
        AI_START_SELECT,
        AI_START_POLARITY,
        /* Reference trigger settings */
        AI_REFERENCE_SELECT,
        AI_REFERENCE_POLARITY,
        /* Stop trigger settings */
        AI_STOP_SELECT,
        AI_STOP_POLARITY,
        /* Sample clock settings */
        AI_SAMPLE_SELECT,
        AI_SAMPLE_POLARITY,
        AI_SAMPLE_PERIOD_DEVISOR,
        AI_SAMPLE_DELAY_DEVISOR,
        /* Convert clock settings */
        AI_CONVERT_SELECT,
        AI_CONVERT_POLARITY,
        AI_CONVERT_PERIOD_DEVISOR,
        AI_CONVERT_DELAY_DEVISOR,
        AI_CONVERT_OUTPUT_SELECT,
        AI_CONVERT_PULSE_WIDTH,

        AI_SI_SOURCE_SELECT,
        AI_ARM_SC,
        AI_ARM_SI,
        AI_ARM_SI2,
        AI_ARM_DIV,

        /* AO Segment */
        AO_CONTINUOUS,
        AO_SIGNAL_GENERATION,
        AO_START1_SOURCE_SELECT,
        AO_START1_POLARITY_SELECT,
        AO_UPDATE_SOURCE_SELECT,
        AO_UPDATE_POLARITY_SELECT,
        AO_UPDATE_PERIOD_DIVISOR,
        AO_UI_SOURCE_SELECT,
        AO_UI_SOURCE_POLARITY_SELECT,
        AO_NUMBER_OF_SAMPLES,
        AO_NUMBER_OF_BUFFERS,
        AO_FIFO_RETRANSMIT,


        /* GPC segment */
        GPC_INITIAL_VALUE,
        GPC_SOURCE_SELECT,
        GPC_GATE_SELECT,
        GPC_COUNT_DIRECTION,
        GPC_SELECT_TYPE,

        /* DIO segment */
        DIO_PORT0_PIN_INPUT,
        DIO_PORT0_PIN_OUTPUT,
        DIO_PORT0_INPUT,
        DIO_PORT0_OUTPUT,

        DIO_PORT1_PIN_INPUT,
        DIO_PORT1_PIN_OUTPUT,
        DIO_PORT1_INPUT,
        DIO_PORT1_OUTPUT,

        DIO_PORT2_PIN_INPUT,
        DIO_PORT2_PIN_OUTPUT,
        DIO_PORT2_INPUT,
        DIO_PORT2_OUTPUT,

        DIO_PIN_DIRECTION,
        DIO_PORT0_MASK,
        DIO_PORT1_MASK,
        DIO_PORT2_MASK,
};


struct pxi6259_ai_channel {
        uint8_t enabled;
        uint8_t terminal;       /* physical terminal */
        uint8_t dither;
        uint8_t gain;
        ai_polarity_t polarity;
        ai_channel_type_t channelType;
};


struct pxi6259_dio_channel {
        uint8_t enabled;
        uint32_t mask;
};


enum pxi6259_segment {
        PXI6259_SEGMENT_NONE = 0,
        PXI6259_SEGMENT_AI = 1,         /* Analog input segment */
        PXI6259_SEGMENT_AO = 2,         /* Analog output segment */
        PXI6259_SEGMENT_DIO = 3,        /* Digital input/output segment */
        PXI6259_SEGMENT_GPC0 = 4,       /* Counter segment */
        PXI6259_SEGMENT_GPC1 = 5,       /* Counter segment */
};

/**
 * The first three task states makes sense only in kernel space and
 * should be obtained with appropriate ioctl call.
 *
 * The task structure is initialized in driver support library. When at least
 * one channel is added the task state transit to READY state. Then the task can
 * be loaded into hardware.
 */
enum pxi6259_task_status {
        PXI6259_TASK_NOT_LOADED = 1,
        PXI6259_TASK_LOADED = 2,
        PXI6259_TASK_RUNNING = 4,

        PXI6259_TASK_READY = 16,
        PXI6259_TASK_INITIALIZED = 32,
        PXI6259_TASK_DESTROYED = 64
};

struct pxi6259_ai_conf {

        struct pxi6259_ai_channel channel[PXI6259_MAX_CHANNELS];
        uint64_t channel_mask;

        uint8_t                         FOUTtimebaseSelect;
        uint8_t                         FOUTdivider;
        uint8_t                         SlowInternalTimebase;
        uint8_t                         SIsource;

        uint8_t                         aiContinuous;
        uint8_t                         aiExternalConvertClock;
        uint8_t                         aiExternalSampleClock;
        tAI_ARM_SC                      aiArmSC;
        tAI_ARM_SI                      aiArmSI;
        tAI_ARM_SI2                     aiArmSI2;
        uint8_t                         aiArmDIV;

        uint32_t                        aiSamplePeriodDivisor;
        uint32_t                        aiSampleDelayDivisor;
        uint32_t                        aiConvertPeriodDivisor;
        uint32_t                        aiConvertDelayDivisor;

        uint32_t                        aiNoOfPreTrigSamples;
        uint32_t                        aiNoOfPostTrigSamples;
        uint32_t                        aiNoOfSamples;

        ai_convert_output_select_t      aiConOutputSelect;
        ai_sip_output_select_t          aiScanInProgOutputSelect;
        ai_convert_pulse_width_t        aiConPulseWidth;
        ai_external_mux_present_t       aiExtMUXpresent;
        ai_extmux_output_select_t       aiExtMUXCLKoutputSelect;
        ai_external_gate_select_t       aiExtGateSelect;
        ai_start_select_t               aiStartSource;
        ai_start_polarity_t             aiStartPolarity;
        ai_reference_select_t           aiRefSource;
        ai_reference_polarity_t         aiRefPolarity;
        ai_stop_select_t                aiStopSource;
        ai_stop_polarity_t              aiStopPolarity;
        ai_sample_select_t              aiSampleClkSource;
        ai_sample_polarity_t            aiSampleClkPolarity;
        ai_convert_select_t      	    aiConvertClkSource;
        ai_convert_polarity_t    	    aiConvertClkPolarity;
        ai_end_on_sc_t                  aiEndOnSCTC;
        ai_end_on_end_of_scan_t         aiEndOnEndOfScan;

        enum pxi6259_terminal           aiExportSampleClockSig;
        enum pxi6259_terminal           aiExportConvertClockSig;
        enum pxi6259_terminal           aiExportStartTrigSig;
        enum pxi6259_terminal           aiExportReferenceTrigSig;

        uint32_t key;
};


struct pxi6259_ao_channel {
        uint8_t                         enabled;
        uint8_t                         offsetSelect;
        uint8_t                         referenceSelect;
        ao_update_mode_t                updateMode;
       /* TODO ao_waveform_order_t             waveformOrder; */
        ao_dac_polarity_t               polarity;
};

struct pxi6259_ao_conf {
        uint8_t                         continuous;
        ao_signal_generation_t          signalGeneration;

        ao_start1_select_t              start1Source;
        ao_start1_polarity_t            start1Polarity;

        ao_update_source_select_t       updateSource;
        ao_update_source_polarity_t     updatePolarity;
        uint32_t                        updatePeriodDivisor;

        ao_ui_source_select_t           uiSourceSelect;
        ao_ui_source_polarity_t         uiSourcePolarity;

        struct pxi6259_ao_channel       channel[PXI6259_NUMBER_OF_AO_CHANNELS];

        uint32_t                        numberOfSamples;
        uint32_t                        numberOfBuffers;
        uint8_t                         fifoRetransmitEnable;

        uint32_t key;
};


struct pxi6259_gpc_conf {
        // status values
        uint8_t                         gpcInfo;

        // simple settings
        uint32_t                        initialValue;
        gpc_source_select_t             source;
        gpc_gate_select_t               gateSource;
        gpc_count_direction_t           countDirection;

        enum pxi6259_terminal           gpcExportGPCOutSig;

        uint32_t key;
};


struct pxi6259_dio_conf {
        struct pxi6259_dio_channel channel[PXI6259_NUMBER_OF_PORTS];

        uint32_t key;
};




/*
 * Standard driver IOCTL call definitions - use 's' as magic number.
 *
 * SET means "Set" through a ptr,
 * TTELL means "Tell" directly with the argument value
 * GET means "Get": reply by setting through a pointer
 * QUERY means "Query": response is on the return value
 * EXCHANGE means "eXchange": switch G and S atomically
 * SHIFT means "sHift": switch T and Q atomically
 *
 * _IO    an ioctl with no parameters
 * _IOW   an ioctl with pxi6259_write parameters (copy_from_user)
 * _IOR   an ioctl with pxi6259_read parameters  (copy_to_user)
 * _IOWR  an ioctl with both pxi6259_write and pxi6259_read parameters.
 *
 * 'Write' and 'pxi6259_read' are from the user's point of view!!!
 */
#define PXI6259_IOC_MAGIC  's'

#define PXI6259_IOC_GET_VERSION            _IOR(PXI6259_IOC_MAGIC, 2, long int *)                  /* Get version date (TODO: struct drv_version) */

#define PXI6259_IOC_GET_AI_CLIENT_LIST     _IOR(PXI6259_IOC_MAGIC, 13, struct pxi6259_client_list *)       /* Get list of driver client PIDs */

#define PXI6259_IOC_RESET                  _IO(PXI6259_IOC_MAGIC, 18)                 /* Reset the module, initialise, re-establish all connections (NULL) */

#define PXI6259_IOC_GET_STATUS             _IOR(PXI6259_IOC_MAGIC, 19, struct pxi6259_module_status *)     /* Read standard module status */
#define PXI6259_IOC_GET_CLEAR_STATUS       _IOR(PXI6259_IOC_MAGIC, 20, struct pxi6259_module_status *)     /* Read and clear standard module status */

#define PXI6259_IOC_RAW_READ               _IOWR(PXI6259_IOC_MAGIC, 21, struct pxi6259_raw_io_block *)      /* Raw pxi6259_read  access to card for debug */
#define PXI6259_IOC_RAW_WRITE              _IOWR(PXI6259_IOC_MAGIC, 22, struct pxi6259_raw_io_block *)      /* Raw pxi6259_write access to card for debug */

#define PXI6259_IOC_SET_HWEMULATION        _IOW(PXI6259_IOC_MAGIC, 23, uint8_t *)                  /* Set/Get HW emulation on of off */
#define PXI6259_IOC_GET_HWEMULATION        _IOR(PXI6259_IOC_MAGIC, 24, uint8_t *)                  /* TRUE=on FALSE=off */

/*
 * NI PXI-6259 ioctl definitions.
 *
 * This are actually HW dependent ioctl calls.
 */

#define PXI6259_IOC_GET_MODULE_ADDR        _IOR(PXI6259_IOC_MAGIC, 25, struct pxi6259_module_address *) 	/* Read module address spaces*/
#define PXI6259_IOC_GET_DEVICE_ID          _IOR(PXI6259_IOC_MAGIC, 26, uint32_t *)              	/* Get device id - used in device file names*/

/* Analog Input */
#define PXI6259_IOC_LOAD_AI_CONF	   _IOW(PXI6259_IOC_MAGIC, 27, struct pxi6259_ai_conf *)
#define PXI6259_IOC_READ_AI_CONF           _IOR(PXI6259_IOC_MAGIC, 28, struct pxi6259_ai_conf *)
#define PXI6259_IOC_CONNECT_AI_CLI         _IO(PXI6259_IOC_MAGIC, 29)
#define PXI6259_IOC_DISCONNECT_AI_CLI      _IO(PXI6259_IOC_MAGIC, 30)
#define PXI6259_IOC_START_AI               _IO(PXI6259_IOC_MAGIC, 31)
#define PXI6259_IOC_STOP_AI                _IO(PXI6259_IOC_MAGIC, 32)
#define PXI6259_IOC_IS_AI_DONE	           _IOWR(PXI6259_IOC_MAGIC, 33, uint8_t *)
#define PXI6259_IOC_READ_AI_SCANS	   _IOWR(PXI6259_IOC_MAGIC, 34, struct pxi6259_rw_buffer *)
#define PXI6259_IOC_GENERATE_PULSE         _IOW(PXI6259_IOC_MAGIC, 35, uint32_t *)
#define PXI6259_IOC_GET_AI_SCALING_COEF    _IOR(PXI6259_IOC_MAGIC, 36, ai_scaling_coefficients_t *)



/* General Purpose Counter */
#define PXI6259_IOC_LOAD_GPC0_CONF         _IOW(PXI6259_IOC_MAGIC, 37, struct pxi6259_gpc_conf *)
#define PXI6259_IOC_LOAD_GPC1_CONF         _IOW(PXI6259_IOC_MAGIC, 38, struct pxi6259_gpc_conf *)
#define PXI6259_IOC_READ_GPC0_CONF         _IOW(PXI6259_IOC_MAGIC, 39, struct pxi6259_gpc_conf *)
#define PXI6259_IOC_READ_GPC1_CONF         _IOW(PXI6259_IOC_MAGIC, 40, struct pxi6259_gpc_conf *)
#define PXI6259_IOC_CONNECT_GPC0_CLI       _IO(PXI6259_IOC_MAGIC, 41)
#define PXI6259_IOC_CONNECT_GPC1_CLI       _IO(PXI6259_IOC_MAGIC, 42)
#define PXI6259_IOC_DISCONNECT_GPC0_CLI    _IO(PXI6259_IOC_MAGIC, 43)
#define PXI6259_IOC_DISCONNECT_GPC1_CLI    _IO(PXI6259_IOC_MAGIC, 44)
#define PXI6259_IOC_START_GPC0             _IO(PXI6259_IOC_MAGIC, 45)
#define PXI6259_IOC_START_GPC1             _IO(PXI6259_IOC_MAGIC, 46)
#define PXI6259_IOC_STOP_GPC0              _IO(PXI6259_IOC_MAGIC, 47)
#define PXI6259_IOC_STOP_GPC1              _IO(PXI6259_IOC_MAGIC, 48)
#define PXI6259_IOC_GET_GPC0_COUNT_VALUE   _IOR(PXI6259_IOC_MAGIC, 49, uint32_t *)
#define PXI6259_IOC_GET_GPC1_COUNT_VALUE   _IOR(PXI6259_IOC_MAGIC, 50, uint32_t *)

/* DIO */
#define PXI6259_IOC_LOAD_DIO_CONF          _IOW(PXI6259_IOC_MAGIC, 51, struct pxi6259_dio_conf *)
#define PXI6259_IOC_READ_DIO_CONF          _IOR(PXI6259_IOC_MAGIC, 52, struct pxi6259_dio_conf *)
#define PXI6259_IOC_CONNECT_DIO_CLI        _IO(PXI6259_IOC_MAGIC, 53)
#define PXI6259_IOC_DISCONNECT_DIO_CLI     _IO(PXI6259_IOC_MAGIC, 54)
#define PXI6259_IOC_READ_DIO_PORT0         _IOR(PXI6259_IOC_MAGIC, 55, uint32_t *)
#define PXI6259_IOC_READ_DIO_PORT1         _IOR(PXI6259_IOC_MAGIC, 56, uint32_t *)
#define PXI6259_IOC_READ_DIO_PORT2         _IOR(PXI6259_IOC_MAGIC, 57, uint32_t *)
#define PXI6259_IOC_WRITE_TO_DIO_PORT0     _IOW(PXI6259_IOC_MAGIC, 58, uint32_t *)
#define PXI6259_IOC_WRITE_TO_DIO_PORT1     _IOW(PXI6259_IOC_MAGIC, 59, uint32_t *)
#define PXI6259_IOC_WRITE_TO_DIO_PORT2     _IOW(PXI6259_IOC_MAGIC, 60, uint32_t *)

/* Analog Output */
#define PXI6259_IOC_LOAD_AO_CONF           _IOW(PXI6259_IOC_MAGIC, 70, struct pxi6259_ao_conf *)
#define PXI6259_IOC_READ_AO_CONF           _IOR(PXI6259_IOC_MAGIC, 71, struct pxi6259_ao_conf *)
#define PXI6259_IOC_CONNECT_AO_CLI         _IO(PXI6259_IOC_MAGIC, 72)
#define PXI6259_IOC_DISCONNECT_AO_CLI      _IO(PXI6259_IOC_MAGIC, 73)
#define PXI6259_IOC_START_AO               _IO(PXI6259_IOC_MAGIC, 74)
#define PXI6259_IOC_STOP_AO                _IO(PXI6259_IOC_MAGIC, 75)
#define PXI6259_IOC_IS_AO_DONE             _IOWR(PXI6259_IOC_MAGIC, 76, uint8_t *)
#define PXI6259_IOC_WRITE_TO_AO0           _IOW(PXI6259_IOC_MAGIC, 77, uint32_t *)
#define PXI6259_IOC_WRITE_TO_AO1           _IOW(PXI6259_IOC_MAGIC, 78, uint32_t *)
#define PXI6259_IOC_WRITE_TO_AO2           _IOW(PXI6259_IOC_MAGIC, 79, uint32_t *)
#define PXI6259_IOC_WRITE_TO_AO3           _IOW(PXI6259_IOC_MAGIC, 80, uint32_t *)
#define PXI6259_IOC_WRITE_TO_AO_FIFO       _IOW(PXI6259_IOC_MAGIC, 81, struct pxi6259_rw_buffer *)
#define PXI6259_IOC_GET_AO_SCALING_COEF    _IOR(PXI6259_IOC_MAGIC, 62, ao_scaling_coefficients_t *)

#define PXI6259_IOC_SET_READ_FLAWOR        _IOW(PXI6259_IOC_MAGIC, 83, enum pxi6259_segment *)

/* Triggers */
#define PXI6259_IOC_CONNECT_TRIGGER        _IOWR(PXI6259_IOC_MAGIC, 84, struct pxi6259_trigger_connection *)
#define PXI6259_IOC_DISCONNECT_TRIGGER     _IOWR(PXI6259_IOC_MAGIC, 85, struct pxi6259_trigger_connection *)

/* Segment level resets */
#define PXI6259_IOC_RESET_AI               _IO(PXI6259_IOC_MAGIC, 86)
#define PXI6259_IOC_RESET_AO               _IO(PXI6259_IOC_MAGIC, 87)
#define PXI6259_IOC_RESET_DIO              _IO(PXI6259_IOC_MAGIC, 88)
#define PXI6259_IOC_RESET_GPC              _IO(PXI6259_IOC_MAGIC, 89)




#define PXI6259_IOC_MAX_NUM 100


/* Error And Completion Codes */
#define PXI6259_ERROR_BASE                              0x0FFA4000

#define PXI6259_ERROR_INVALID_DESCRIPTOR                (PXI6259_ERROR_BASE + 1)
#define PXI6259_ERROR_INVALID_MODE                      (PXI6259_ERROR_BASE + 2)
#define PXI6259_ERROR_FEATURE_NOT_SUPPORTED             (PXI6259_ERROR_BASE + 3)
#define PXI6259_ERROR_VERSION_MISMATCH                  (PXI6259_ERROR_BASE + 4)
#define PXI6259_ERROR_INTERNAL_SOFTWARE                 (PXI6259_ERROR_BASE + 5)
#define PXI6259_ERROR_FILE_IO                           (PXI6259_ERROR_BASE + 6)
#define PXI6259_ERROR_INVALID_ARGUMENT                  (PXI6259_ERROR_BASE + 7)
#define PXI6259_ERROR_TIMEOUT                           (PXI6259_ERROR_BASE + 8)

#define PXI6259_ERROR_DRIVER_INITIALIZATION             (PXI6259_ERROR_BASE + 20)
#define PXI6259_ERROR_DRIVER_TIMEOUT                    (PXI6259_ERROR_BASE + 21)

#define PXI6259_ERROR_READ_FAILURE                      (PXI6259_ERROR_BASE + 30)
#define PXI6259_ERROR_WRITE_FAILURE                     (PXI6259_ERROR_BASE + 31)
#define PXI6259_ERROR_DEVICE_NOT_FOUND                  (PXI6259_ERROR_BASE + 32)
#define PXI6259_ERROR_DEVICE_NOT_READY                  (PXI6259_ERROR_BASE + 33)
#define PXI6259_ERROR_DEVICE_NOT_INITIALIZED            (PXI6259_ERROR_BASE + 34)
#define PXI6259_ERROR_INTERNAL_HARDWARE                 (PXI6259_ERROR_BASE + 35)
#define PXI6259_ERROR_OVERFLOW                          (PXI6259_ERROR_BASE + 36)
#define PXI6259_ERROR_STATUS_NOT_AVAILABLE              (PXI6259_ERROR_BASE + 37)

/* Task errors */
#define PXI6259_ERROR_AO_NOT_CONNECTED                  (PXI6259_ERROR_BASE + 50)
#define PXI6259_ERROR_CONNECTING_AO                     (PXI6259_ERROR_BASE + 51)
#define PXI6259_ERROR_AI_NOT_CONNECTED                  (PXI6259_ERROR_BASE + 52)
#define PXI6259_ERROR_CONNECTING_AI                     (PXI6259_ERROR_BASE + 53)
#define PXI6259_ERROR_DIO_NOT_CONNECTED                 (PXI6259_ERROR_BASE + 54)
#define PXI6259_ERROR_CONNECTING_DIO                    (PXI6259_ERROR_BASE + 55)
#define PXI6259_ERROR_GPC_NOT_CONNECTED                 (PXI6259_ERROR_BASE + 56)
#define PXI6259_ERROR_CONNECTING_GPC                    (PXI6259_ERROR_BASE + 57)

#define PXI6259_ERROR_DISCONNECTING_AI                  (PXI6259_ERROR_BASE + 130)
#define PXI6259_ERROR_DISCONNECTING_AO                  (PXI6259_ERROR_BASE + 131)
#define PXI6259_ERROR_DISCONNECTING_GPC                 (PXI6259_ERROR_BASE + 132)
#define PXI6259_ERROR_DISCONNECTING_DIO                 (PXI6259_ERROR_BASE + 133)


#define PXI6259_ERROR_AI_SEGMENT_BUSY                   (PXI6259_ERROR_BASE + 60)
#define PXI6259_ERROR_GPC0_SEGMENT_BUSY                 (PXI6259_ERROR_BASE + 61)
#define PXI6259_ERROR_GPC1_SEGMENT_BUSY                 (PXI6259_ERROR_BASE + 62)
#define PXI6259_ERROR_DIO_SEGMENT_BUSY                  (PXI6259_ERROR_BASE + 63)
#define PXI6259_ERROR_AO_SEGMENT_BUSY                   (PXI6259_ERROR_BASE + 64)
#define PXI6259_ERROR_AO_COUNTER_BUSY                   (PXI6259_ERROR_BASE + 65)

#define PXI6259_ERROR_AI_NOT_CONFIGURED                 (PXI6259_ERROR_BASE + 66)
#define PXI6259_ERROR_GPC0_NOT_CONFIGURED               (PXI6259_ERROR_BASE + 67)
#define PXI6259_ERROR_GPC1_NOT_CONFIGURED               (PXI6259_ERROR_BASE + 68)
#define PXI6259_ERROR_DIO_NOT_CONFIGURED                (PXI6259_ERROR_BASE + 69)
#define PXI6259_ERROR_AO_NOT_CONFIGURED                 (PXI6259_ERROR_BASE + 70)

/* Channel errors */
#define PXI6259_ERROR_CHANNEL_EXISTS                    (PXI6259_ERROR_BASE + 120)
#define PXI6259_ERROR_PORT_EXISTS                       (PXI6259_ERROR_BASE + 120)
#define PXI6259_ERROR_CHANNEL_DOES_NOT_EXISTS           (PXI6259_ERROR_BASE + 121)
#define PXI6259_ERROR_PORT_DOES_NOT_EXISTS              (PXI6259_ERROR_BASE + 121)
#define PXI6259_ERROR_INVALID_SEGMENT_FOR_CHANNEL       (PXI6259_ERROR_BASE + 122)
#define PXI6259_ERROR_INVALID_CHANNEL_FOR_SEGMENT       (PXI6259_ERROR_BASE + 123)
#define PXI6259_ERROR_CHANNEL_IN_CONFLICT               (PXI6259_ERROR_BASE + 123)

/* SW buffer errors */
#define PXI6259_ERROR_BUFFER_EMPTY                      (PXI6259_ERROR_BASE + 80)

/* Trigger errors */
#define PXI6259_ERROR_CONNECTING_TRIGGER                (PXI6259_ERROR_BASE + 90)
#define PXI6259_ERROR_DISCONNECTING_TRIGGER             (PXI6259_ERROR_BASE + 91)

/* Scaling errors */
#define PXI6259_ERROR_GETTING_AI_SCALING_COEFFICIENT    (PXI6259_ERROR_BASE + 100)
#define PXI6259_ERROR_GETTING_AO_SCALING_COEFFICIENT    (PXI6259_ERROR_BASE + 101)
#define PXI6259_ERROR_INVALID_SCALING_ORDER_VALUE       (PXI6259_ERROR_BASE + 102)
#define PXI6259_ERROR_SCALING_FAILED                    (PXI6259_ERROR_BASE + 103)

#define PXI6259_ERROR_WRITTING_TO_DIO_PORT              (PXI6259_ERROR_BASE + 110)
#define PXI6259_ERROR_WRONG_AO_CHANNEL_ORDER            (PXI6259_ERROR_BASE + 111)
#define PXI6259_ERROR_NO_AO_CHANNELS_IN_USE             (PXI6259_ERROR_BASE + 112)
#define PXI6259_ERROR_NO_GPC_USE_SELECTED               (PXI6259_ERROR_BASE + 113)
#define PXI6259_ERROR_OPPERATION_NOT_SAMPLE_ON_PULSE    (PXI6259_ERROR_BASE + 114)

#define PXI6259_ERROR_MUTEX                             (PXI6259_ERROR_BASE + 115)

#endif /* PXI6259_H_ */
