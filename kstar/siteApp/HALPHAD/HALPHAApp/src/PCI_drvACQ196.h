#ifndef drvACQ196_H
#define drvACQ196_H


#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */

/*#include<unistd.h>*/ /* for read(), close(), write() */


/*#include <semaphore.h> */


#include "local.h"
//#include "acq200_ioctl.h"

#include "sfwDriver.h"


#define BUFLEN	0x400000	/* @@todo read from device */
#define TIMEOUT	5000		/* @@todo ticks. should be msec. */
				/*         but that needs dvr chg. */
#define INITIAL_TIMEOUT (24*3600*100)
#define REVID "$Revision: 1.5 $ B1000"
#define MAXTO		60			/* max timeout retries */


#define INIT_GAIN			1
#define INIT_CH_MASK		96


#define STR_DEVICE_TYPE_1   "ACQ196"
#define STR_DEVICE_TYPE_2   "user Device"

/*FILE *fpRaw; */


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
#define STR_DEV1_RAW_DIR       "/root/rawData"
#define STR_DEV2_RAW_DIR       "/media/data2"
#define STR_DEV3_RAW_DIR       "/media/data3"
#define STR_DEV4_RAW_DIR       "/media/data1"

#define STR_CH_MAPPING_FILE    "/usr/local/epics/siteApp/config/acq196_ch_mapping"
#define STR_CHANNEL_DATA_DIR   "/media/data2"





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










/*sem_t DAQsemaphore; */


/******* make for RING buffer   ************************************/
typedef struct {
	ELLNODE			node;
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* IMPORTANT...!!!!    should be adjusted your local device!!!!!!!!! */
	char data[ BUFLEN ];  
} ST_buf_node;

typedef struct {
	ST_buf_node	*pNode;
/*	unsigned long int		u32Cnt; */
	unsigned int		size;
	char				opcode;

} ST_User_Buff_node;
/****************************************************/


typedef struct {
	int fd;
	void *pbuf;
	char id[8];
}DeviceMapping;

#if 0
typedef DeviceMapping {
	[BUFFERA] = {
		.id = "bufA"
	},
	[BUFFERB] = {
		.id = "bufB"
	}
}BufferMapping;
#endif


typedef struct {

	uint16			slot;
	char			dataFileName[SIZE_RAW_DATA_FULL_NAME];
	unsigned int		boardConfigure;

	int	nChannelMask;
	char		clockSource;
	int clockDivider;
	int nCardMode;
	char   strTagName[NUM_CH_PER_BOARD][SIZE_TAG_NAME];
	char cTouchTag[NUM_CH_PER_BOARD];
	char channelOnOff[NUM_CH_PER_BOARD];
	
	DeviceMapping drv_bufAB[2]; 
//	struct BufferAB_Ops drv_ops;
	int n32_maxdma;
	
#if USE_FILE_POINTER
	FILE *fpRaw;
#else
	int R_file;
#endif

	unsigned long int rawDataSize;
	unsigned long int needDataSize;
	char gotDAQstopMsg;
	
	int parsing_remained_cnt;
	int mdsplus_snd_cnt;
	int daqing_cnt;

	char pvName_DEV_ARMING[SIZE_PV_NAME];
	char pvName_DEV_RUN[SIZE_PV_NAME];

} 
ST_ACQ196;

/*
typedef struct {
	ELLNODE		node;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex;

	int		gain;
	
	ST_ACQ196     *pAcq196;
	
	double		conversionTime_usec;

} ST_ACQ196_channel;
*/

int create_acq196_taskConfig( ST_STD_device *pSTDdev);
ST_threadCfg*  make_controlThreadConfig(ST_ACQ196 *pAcq196);


void func_acq196_SYS_ARMING(void *pArg, double arg1, double arg2);
void func_acq196_SYS_RUN(void *pArg, double arg1, double arg2);
void func_acq196_OP_MODE(void *pArg, double arg1, double arg2);
void func_acq196_SHOT_NUMBER(void *pArg, double arg1, double arg2);
void func_acq196_CREATE_LOCAL_SHOT(void *pArg, double arg1, double arg2);




int drvACQ196_is_Master(ST_STD_device *);
int drvACQ196_notify_InitCondition(ST_STD_device *);

void drvACQ196_set_chanGain(ST_ACQ196 *pAcq196, int channel, double gain);
void drvACQ196_set_samplingRate(ST_STD_device *pSTDdev, int samplingRate);
void drvACQ196_set_channelMask(ST_STD_device *, int );
int drvACQ196_set_cardMode(ST_STD_device *);

int drvACQ196_ARM_enable(ST_STD_device *);
int drvACQ196_ARM_disable(ST_STD_device *);
int drvACQ196_RUN_start(ST_STD_device *);
int drvACQ196_RUN_stop(ST_STD_device *);

int drvACQ196_wait_RUN_stop_complete(ST_STD_device *);



int drvACQ196_status_reset(ST_STD_device *);

int drvACQ196_check_AllReadyToSave();
int drvACQ196_check_AllStorageFinished();

int drvACQ196_check_Run_condition(ST_STD_device *);
int drvACQ196_check_Stop_condition(ST_STD_device *);


int drvACQ196_set_data_parsing(ST_STD_device *);
int drvACQ196_set_send_data(ST_STD_device *);

int drvACQ196_openFile(ST_STD_device *);

int drvACQ196_assign_pvname(ST_STD_device *);
int drvACQ196_reset_cnt(ST_STD_device *);





void threadFunc_user_DAQ(void *param);
void threadFunc_user_RingBuf(void *param);
void threadFunc_user_CatchEnd(void *param);






#endif /* drvACQ196_H */

