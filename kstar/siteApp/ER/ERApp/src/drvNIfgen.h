#ifndef drvNIfgen_H
#define drvNIfgen_H


#include <sys/stat.h> /* for open() */
#include <fcntl.h> /* for open() */

#include<unistd.h> /* for read(), close(), write() */


#include "sfwDriver.h"
#include "niLocalDef.h"

#include "niFgen.h"



typedef struct {

	unsigned int		boardConfigure;

/********* NI fgen **************/
	ViChar		Resource[256];
	ViSession		vi;
	epicsInt32	outputMode;
	
	ViChar sampleClockSource[256];
	ViReal64 sampleRate;
	ViInt32 clockMode;
	ViInt32 trigMode;
	ViInt32	trigType;
	ViChar  triggerSource[256];

	ViInt32 exportSigIn;
	ViChar  strExportSigTO[256];
/********************************/
	
}
ST_NIFGEN;


typedef struct {
/*	
	ELLNODE		node;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex; */

	
/********* NI fgen **************/
	epicsInt32 wfmType;
	epicsFloat64 f64Frequency, f64Amplitude, f64StartPhase, f64DCoffset, f64gain;
	ViInt32 wfmHandle;
	ViChar wfmFileName[256];

/********* NI fgen **************/
	
	double		conversionTime_usec;

}
ST_NIFGEN_channel;



int create_ni5412_taskConfig(ST_STD_device *);
ST_NIFGEN_channel* create_ni5412_channel();
/*void clear_ni5412_Tasks(ST_STD_device *pSTDdev); */
void clear_ni5412_Tasks(void  *pArg, double arg1, double arg2);




/**************************************************************/
void func_ni5412_SYS_ARMING(void *pArg, double arg1, double arg2);
void func_ni5412_SYS_RUN(void *pArg, double arg1, double arg2);

/**************************************************************/


int niFgen_error(ST_STD_device *, ViStatus error);

int drvNIfgen_cfg_ouputMode(ST_STD_device *);
int drvNIfgen_set_clockSource(ST_STD_device *);
int drvNIfgen_arb_sampleRate(ST_STD_device *);
int drvNIfgen_set_clockMode(ST_STD_device *);

int drvNIfgen_cfg_StandardWF(ST_STD_device *, ST_STD_channel *);
int drvNIfgen_set_outputEnable(ST_STD_device *, char *channelName, ViBoolean onoff); /* ViUInt16 = ViBoolean */
int drvNIfgen_init_Generation(ST_STD_device *);
int drvNIfgen_abort_Generation(ST_STD_device *);
int drvNIfgen_create_WFdownload(ST_STD_device *, ST_STD_channel *);
int drvNIfgen_cfg_arbitraryWF(ST_STD_device *, ST_STD_channel *);

int drvNIfgen_cfg_trigMode(ST_STD_device *);
int drvNIfgen_cfg_trigType(ST_STD_device *);

int drvNIfgen_ExportSignal(ST_STD_device *);


int drvNIfgen_stop_all_Generation();


#endif /* drvNIfgen_H */

