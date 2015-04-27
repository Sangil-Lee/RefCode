#ifndef _LOCAL_DEF_H_
#define _LOCAL_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************/
/* Standard Local PVs for KSTAR sequence ***********/

#define SYS_ARMING       "SXR_SYS_ARMING"
#define SYS_RUN          "SXR_SYS_RUN"
#define CA_SHOT_NUMBER   "SXR_CCS_SHOT_NUMBER"
#define CA_BLIP_TIME     "SXR_CCS_BLIP_TIME"

#define STR_DEVICE_TYPE_1   "NI6123"
#define STR_DEVICE_TYPE_2   "NI6259"
#define STR_DEVICE_TYPE_3   "IT6322"



#define PRINT_PHASE_INFO         0
#define PRINT_DEV_SUPPORT_ARG    0
#define PRINT_MDS_CONNECTION     0
#define PRINT_ERROR              1  /* 0: don't print on terminal, 1: print msg on terminal,  notify_admin_error_info()  */

#define USE_AUTO_SAVE          1 
#define USE_MDS_EVENT_NOTIFY   0
#define USE_LOCAL_DATA_POOL    0
#define USE_THREAD_CATCHEND    0
/*
 Standard Local PVs for KSTAR sequence ..... here!!!
#####################################################
*/


#define INIT_SAMPLING   10000  /* unit:Hz,  must under 390K */
#define INIT_BLIP_TIME  (16.0)  /* unit:sec,   */

#define PRINT_ACQCMD            0
#define PRINT_DMA_NUM           0
#define DEBUG_PRINT_HTSTREAM	0
#define PRINT_DAQ_DEBUG         0

#define CARD_MODE_SINGLE        1
#define CARD_MODE_MASTER        2
#define CARD_MODE_SLAVE         3




#define MAX_ACQ196         6



#define TEST_BLOCK_ADC_DAQ_RUN    0


#define USE_FILE_POINTER        1
#define MASTER_DEV_NAME        "B1"
#define STR_DEV1_RAW_DIR       "/media/data1"
#define STR_DEV2_RAW_DIR       "/media/data2"
#define STR_DEV3_RAW_DIR       "/media/data3"
#define STR_DEV4_RAW_DIR       "/root/data4"

#define STR_CH_MAPPING_FILE    "/usr/local/epics/siteApp/config/acq196_ch_mapping"
//#define STR_LOCAL_DATA_DIR   "/home/sxr_local_data/" 
//#define STR_MDS_PUT_DATA_DIR   "/home/sxr_mds_put_data/"
#define STR_LOCAL_DATA_DIR   "/data/sxr_local_data/"
#define STR_MDS_PUT_DATA_DIR   "/data/sxr_mds_put_data/"





#define INIT_ACQ196_MAXDMA          8000

#define MAX_RING_BUF_NUM            12
#define MAX_CUTOFF_BUF_NODE         4

#define NUM_CH_PER_BOARD            96
#define SIZE_RAW_DATA_FULL_NAME     64
#define TIME_GAP_ARMING             (1.0)




#define DEV_CHANNELIZATION_FAIL	(0x0001<<10)
#define DEV_STORAGE_FAIL        (0x0001<<11)
#define DEV_IN_LOOP_HTSTREAM    (0x0001<<12)
#define DEV_READY_TO_SAVE       (0x0001<<13)
#define DEV_STORAGE_FINISH      (0x0001<<14)
#define DEV_CMD_DAQ_STOPED      (0x0001<<15)
/* #define TASK_DAQ_DONE_OK     (0x001<<16) */



#define QUEUE_OPCODE_NORMAL        0x0
#define QUEUE_OPCODE_CLOSE_FILE   (0x1<<0)
#define QUEUE_OPCODE_DAQ_STOP     (0x1<<1)


#define CHANNEL_INITIALIZED     (0x0001<<0)



#define CLOCK_INTERNAL   0
#define CLOCK_EXTERNAL   1

#define ACQ196_SET_CLOCK_RATE   (0x01<<0)
#define ACQ196_SET_R_BITMASK    (0x01<<1)
#define ACQ196_SET_M_BITMASK    (0x01<<2)
#define ACQ196_SET_L_BITMASK    (0x01<<3)
#define ACQ196_SET_T0           (0x01<<4)
#define ACQ196_SET_T1           (0x01<<5)
#define ACQ196_SET_TAG          (0x01<<6)
#define ACQ196_SET_SHOT_NUM     (0x01<<7)



#ifdef __cplusplus
}
#endif

#endif



