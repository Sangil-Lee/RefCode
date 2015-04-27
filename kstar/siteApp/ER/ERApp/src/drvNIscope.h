#ifndef drvNIscope_H
#define drvNIscope_H



#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */
#include<unistd.h> /* for read(), close(), write() */

#include "sfwDriver.h"
#include "sfwTree.h" /* segmented archiving */

#include "niLocalDef.h"

/*#include <semaphore.h> */

#include "niScope.h"



#define  USE_NORMAL_ARCHIVING        0
#define  USE_SEGMENTED_ARCHIVING     1


/*FILE *fpRaw; */
/*
typedef	signed char			int8;
typedef	unsigned char		uint8;
typedef	signed short		int16;
typedef	unsigned short		uint16;
typedef	signed int			int32;
typedef	unsigned int		uint32;
typedef	float				float32;
typedef	double				float64;
*/
/*sem_t DAQsemaphore; */


/******* make for RING buffer   ************************************/
#define MAX_RING_BUF_NUM            12
#define MAX_CUTOFF_BUF_NODE         4
#define BUFLEN	0x400000	/* @@todo read from device */

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

	unsigned int myStatusDev;
	

/* NI scope *************************/
	ViChar		cResource[256];
	ViSession	vi;
	
	ViInt32 n32ActualRecordLength;
	ViReal64 f64ActualSampleRate;
	
	ViReal64 f64minSampleRate;
	ViInt32 n32minRecordLength;
	ViReal64 f64refPosition;
	ViInt32 n32numRecords;
	ViBoolean b8enforceRealtime;

	ViChar	triggerSource[32];   
	ViChar   useMultiTrigger;
/* NI scope *************************/
	
	
	
	DBADDR db_SOFT_SYS_RUN;
	DBADDR db_SOFT_DATA_PROCESS;
/*
	DBADDR db_ER_LTU_mT0_sec0;
	DBADDR db_ER_LTU_mT0_sec1;
	DBADDR db_ER_LTU_mT0_sec2;
	DBADDR db_ER_LTU_mT1_sec0;
	DBADDR db_ER_LTU_mT1_sec1;
	DBADDR db_ER_LTU_mT1_sec2;
*/

	/* segmented Archiving */
	sfwTreePtr		pSfwTree;

}
ST_NISCOPE_dev;


typedef struct {
	
	unsigned int myStatusCh;
	
	ViInt32 n32ActualNumWaveform;

	ViReal64 *f64WaveformPtr;
	void* binaryWfmPtr;
	struct niScope_wfmInfo *wfmInfoPtr;
	ViReal64 f64Timeout;
	/* for Vertical property */
	ViReal64 f64VRange;
	ViReal64 f64VOffset;
	ViInt32  n32VCoupling;
	ViReal64 f64probeAttenuation;
	char cstrWFfileName[256];

	ViReal64 f64InputImpedance;
	ViReal64 f64maxInputFrequency;

	ViInt32 filterType;
	ViReal64 cutoffFrequency;
	ViReal64 centerFrequency;
	ViReal64 bandpassWidth;
	
/*	char cstr_tagName[64]; */


	DBADDR db_SYS_FETCH_DATA;

	/* segmented Archiving */
	sfwTreeNodePtr	pSfwTreeNode;
	sfwTreeNodePtr	pSfwTreeScaleNode;

}
ST_NISCOPE_ch;



int niScope_error(ST_STD_device *, ViStatus error);

int create_ni5122_taskConfig( ST_STD_device *pSTDdev);
ST_NISCOPE_ch* create_ni5122_channel();

void threadFunc_ni5122_DAQ(void *param);



/**************************************************************/
void func_ni5122_SYS_ARMING(void *pArg, double arg1, double arg2);
void func_ni5122_SYS_RUN(void *pArg, double arg1, double arg2);
void func_ni5122_OP_MODE(void *pArg, double arg1, double arg2);


/**************************************************************/


int drvNIscope_get_RecordLength(ST_STD_device *);
int drvNIscope_get_SampleRate(ST_STD_device *);
int drvNIscope_get_NumOfWfm(ST_STD_device *, ST_STD_channel *);
int drvNIscope_auto_setup(ST_STD_device *);
int drvNIscope_get_readData(ST_STD_device *, ST_STD_channel *);

int drvNIscope_v_Cfg_board(ST_STD_device *, ST_STD_channel *);
int drvNIscope_h_Cfg_board(ST_STD_device *);
int drvNIscope_v_Cfg_ChanCharacteristics(ST_STD_device *, ST_STD_channel *);

int drvNIscope_set_trigImmediate(ST_STD_device *);
int drvNIscope_start_acquisition(ST_STD_device *);
int drvNIscope_abort_acquition(ST_STD_device *);
int drvNIscope_get_fetchData(ST_STD_device *, ST_STD_channel *);
int drvNIscope_get_fetchBinary(ST_STD_device *, ST_STD_channel *);
int drvNIscope_get_AcquisitionStatus(ST_STD_device *);

int drvNIscope_cfg_triggerEdge(ST_STD_device *);

int drvNIscope_set_filterType(ST_STD_device *, ST_STD_channel *);
int drvNIscope_set_filterCutoffFreq(ST_STD_device *, ST_STD_channel *);
int drvNIscope_set_filterCenterFreq(ST_STD_device *, ST_STD_channel *);
int drvNIscope_set_filterBandWidth(ST_STD_device *, ST_STD_channel *);

int drvNIscope_set_multi_trigger(ST_STD_device *);
int drvNIscope_is_multi_trigger(ST_STD_device *);





void clear_niscope_device(void *pSTDdev, double arg1, double arg2);
/*void clear_niscope_channel(ST_STD_channel *);	*/
void clear_niscope_channel(void *pArg, double arg1, double arg2);



void drvNIscope_status_reset( ST_STD_device *);
int drvNIscope_check_DataPutFinish();

int drvNIscope_memCheck_Fech(ST_STD_device *, ST_STD_channel *);

int drvNIscope_get_Multi_Trig_Info(void *ptr);



#endif /* drvNIscope_H */

