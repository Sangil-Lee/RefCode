#ifndef drvZT530_H
#define drvZT530_H


#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */

#include<unistd.h> /* for read(), close(), write() */


#include "sfwDriver.h"
#include "niLocalDef.h"

#include "niFgen.h"

#include "ztwaveC.h"
#include "ztypes.h"
#include "zerr.h"
#include "zt530.h"

static ZT_ERROR error;

typedef struct {

	unsigned int		boardConfigure;

	ZT_HANDLE instr_handle;
	char		Resource[256];
	
	epicsInt16	outputMode;
	epicsInt16 ch0Enable, ch1Enable; /* will be delete in near future */


	

	ViSession		vi;
	
	
	ViUInt16 sampleClockSource;
	ViReal64 sampleRate;
	ViInt32 clockMode;
	ViInt32 trigMode;
	ViInt32	trigType;
	ViChar  triggerSource[256];
	ViInt32 segmentPoints;

	ViChar strExportSigTO[256];
	ViUInt16 exportSigIn;
	ViUInt16  exportSigTO;
/********************************/
	
}
ST_ZT530;


typedef struct {
/*	
	ELLNODE		node;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex; */
	epicsInt16 u16ChannelID;  /*ZT530_CH1, ZT530_CH2 */
	epicsInt16 u16OutputState;  /*ZT530_OUTPUT_ACTIVE : ZT530_OUTPUT_INACTIVE; */



	

	epicsFloat64 f64Frequency; /* 1E+8 */
	epicsFloat64 f64Amplitude; /* 1 */
	epicsFloat64 f64StartPhase; /* 0.0 */
	epicsFloat64 f64DCoffset; /* 0 */
	epicsFloat64 f64Gain;
	epicsFloat64 f64Impedance; /* 1MHz*/
	
	epicsInt16 u16FilterState;  /* ZT530_FILT_10MHZ */
	epicsInt16 u16Clock_interpolate; /* ZT530_INTERP_AUTO */

	epicsInt32 u32Segment_points;
	epicsInt32 wfmType; /* ZT530_FUNC_SINE_WAVE */

	
	ViInt32 wfmHandle;
	ViChar wfmFileName[256];

	
	double		conversionTime_usec;

}
ST_ZT530_channel;



int create_zt530_taskConfig(ST_STD_device *);
ST_ZT530_channel* create_zt530_channel(ST_STD_channel *);
/*void clear_zt530_Tasks(ST_STD_device *pSTDdev); */
void clear_zt530_Tasks(void  *pArg, double arg1, double arg2);




/**************************************************************/
void func_zt530_SYS_ARMING(void *pArg, double arg1, double arg2);
void func_zt530_SYS_RUN(void *pArg, double arg1, double arg2);

/**************************************************************/


int zt530_error(ST_STD_device *, ViUInt16 count, ViInt16 errcode[],char *);

//int drvZT530_cfg_ouputMode(ST_STD_device *);
int drvZT530_set_clockSource(ST_STD_device *);
int drvZT530_arb_sampleRate(ST_STD_device *);
int drvZT530_set_clockMode(ST_STD_device *);

int drvZT530_set_function_generator(ST_STD_device *);
int drvZT530_reset_init(ST_STD_device *);



int drvZT530_cfg_StandardWF(ST_STD_device *, ST_STD_channel *);
int drvZT530_set_outputEnable(ST_STD_device *, ST_STD_channel *);

int drvZT530_init_Generation(ST_STD_device *);
int drvZT530_abort_Generation(ST_STD_device *);
int drvZT530_create_WFdownload(ST_STD_device *, ST_STD_channel *);
int drvZT530_cfg_arbitraryWF(ST_STD_device *, ST_STD_channel *);

int drvZT530_cfg_trigMode(ST_STD_device *);
int drvZT530_cfg_trigType(ST_STD_device *);

int drvZT530_ExportSignal(ST_STD_device *);


int drvZT530_stop_all_Generation();


#endif /* drvZT530_H */

