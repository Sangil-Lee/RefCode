#ifndef drvPXISCXI_H
#define drvPXISCXI_H

#include <NIDAQmx.h>

#include "epicsRingBytes.h"
#include "epicsMessageQueue.h"
#include "epicsThread.h"

#include "ellLib.h"
#include "callback.h"
#include "dbScan.h"

#define CERNOX_CHANNEL			(0x0001<<0)
#define HALL_CHANNEL			(0x0001<<1)
#define STRAIN_CHANNEL			(0x0001<<2)
#define DISPLACEMENT_CHANNEL	(0x0001<<3)
#define RELAY_CHANNEL			(0x0001<<4)   /* 20070504 Modify */
#define DO8_CHANNEL  	        (0x0001<<5)     /* Modify 2007067 */
#define DI8_CHANNEL				(0x0001<<6)

#define TASK_NOT_INIT			0x0000
#define TASK_INITIALIZED		(0x0001<<0)
#define TASK_STARTED			(0x0001<<1)
#define TASK_STOPED				(0x0001<<2)
#define TASK_TIMING				(0x0001<<3)
#define TASK_CALLBACK			(0x0001<<4)
#define TASK_CONTHREAD          (0x0001<<5)
#define TASK_CONTQUEUE          (0x0001<<6)
#define TASK_DESTROIED          (0x0001<<10)

#define CHANNEL_INITIALIZED		(0x0001<<0)

#define INIT_GAIN_CERNOX        1000.    /* 1000 -> 100 changed by TGLee 08421, 100 -> 1000 changed at 080722*/
#define FINAL_GAIN_CERNOX		50.
#define INIT_MINRANGE_CERNOX    0.
#define INIT_MAXRANGE_CERNOX    /* 10000. */ 0.1
#define INIT_EXCITATIONCURR_CERNOX 10.E-6

#define INIT_MINRANGE_STRAIN             	 0.
#define INIT_MAXRANGE_STRAIN               	 1000.
#define INIT_UNIT_STRAIN			 DAQmx_Val_Strain
#define INIT_BRIDGETYPE_STRAIN            	 DAQmx_Val_FullBridgeI
#define INIT_EXCITATIONSOURCE_STRAIN     	 DAQmx_Val_Internal
#define INIT_EXCITATIONVOLTAGE_STRAIN    	 5.
#define INIT_INITIALBRIDGEVOLTAGE_STRAIN 	 0.
#define INIT_NOMINALGAGERESISTANCE_STRAIN	 10.
#define INIT_LEADWIRERESISTANCE_STRAIN   	 0.

#define INIT_TERMINAL_DISPLACEMENT     		 DAQmx_Val_InputTermCfg_Diff   /* 20070325 Modify */
#define INIT_MINRANGE_DISPLACEMENT	 	 -0.02
#define INIT_MAXRANGE_DISPLACEMENT	 	 0.02
#define INIT_UNIT_DISPLACEMENT         		 DAQmx_Val_Volts         /* 20070325 Modify */
#define INIT_BRIDGETYPE_DISPLACEMENT	 	 DAQmx_Val_FullBridge   /* 20070325 Modify */
#define INIT_EXCITATIONSOURCE_DISPLACEMENT	 DAQmx_Val_Internal
#define INIT_EXCITATIONVOLTAGE_DISPLACEMENT	 5.
#define INIT_EXCITATIONSCALING_DISPLACEMENT   	 TRUE

/* #define INIT_GAIN_HALL			  1. /20071212 Old TG.Lee*/
#define INIT_GAIN_HALL			  1000.
#define INIT_CUTOFFFREQ_HALL	          4.
#define INIT_MINRANGE_HALL	         -0.1
#define INIT_MAXRANGE_HALL		  0.1

#define INIT_CUTOFF		    	  1.
/* #define INIT_SAMPLING  		  100.  Default 10.  100Hz Test 2010 01 11 TG.Lee  */
#define INIT_SAMPLING  			  10.
#define INIT_DATAPOOLSIZE      		  4

/* #define INIT_DO8				0x00		 Add DO8 20070608 */ 
/* #define INIT_DI8				0x00		 Add DI8 20070617 */


/* 20070504 Modify */
#define TOPOLOGY_1127_1_Wire_64         "1127/1-Wire 64x1 Mux"
#define TOPOLOGY_1127_2_Wire_32         "1127/2-Wire 32x1 Mux"
#define TOPOLOGY_1127_2_Wire_4          "1127/2-Wire 4x8 Matrix"
#define TOPOLOGY_1127_4_Wire_16         "1127/4-Wire 16x1 Mux"
#define TOPOLOGY_1127_Independent       "1127/Independent"
#define TOPOLOGY_2564_16_SPST           "2564/16-SPST"
#define TOPOLOGY_2565_16_SPST           "2565/16-SPST"
#define TOPOLOGY_2566_16_SPDT           "2566/16-SPDT"
#define TOPOLOGY_2567_Independent       "2567/Independent"
#define TOPOLOGY_2568_31_SPST           "2568/31-SPST"
#define TOPOLOGY_2569_100_SPST          "2569/100-SPST"
#define TOPOLOGY_2570_40_SPDT           "2570/40-SPDT"

/* 20070325 Modify for displacement */
#define TERMINAL_CFGDEFAULT   		"Terminal Default"
#define TERMINAL_CFGRSE  		"Terminal RSE"
#define TERMINAL_CFGNRSE 		"Terminal NRSE"
#define TERMINAL_CFGDIFF 		"Terminal DIFF"
#define TERMINAL_CFGPDIFF  		"Terminal PseudoDiff"
/* End of 20070325 Modification  */


#define BRIDGE_FULLBRIDGE     		"Full Bridge"     /* 20070325 Modify */
#define BRIDGE_FULLBRIDGE_I		"Full Bridge I"
#define BRIDGE_FULLBRIDGE_II		"Full Bridge II"
#define BRIDGE_FULLBRIDGE_III		"Full Bridge III"
#define BRIDGE_HALFBRIDGE       	"Half Bridge"     /* 20070325 Modify */
#define BRIDGE_HALFBRIDGE_I		"Half Bridge I"
#define BRIDGE_HALFBRIDGE_II		"Half Bridge II"
#define BRIDGE_QUARTERBRIDGE    	"Quarter Bridge"      /* 20070325 Modify */
#define BRIDGE_QUARTERBRIDGE_I		"Quarter Bridge I"
#define BRIDGE_QUARTERBRIDGE_II		"Quarter Bridge II"
#define BRIDGE_NOBRIDGE          	"No Bridge"       /* 20070325 Modify */

typedef struct {
	char 		*bridgeStr;
	int32		bridgeType;
} bridgeConfig;

/* 20070325 Modify */
typedef struct {
  char    *terminalStr;
  int32    terminalType;
} terminalConfig;

/* 20070504 Modify */
typedef struct {
        char    *topologyStr;
        char	*topologyType;
} topologyConfig;


/* Modify 20070617 DO8 Channel add */
typedef struct {
	void             *ptaskConfig;
	void             *precord;
	double           setValue;
	void		 *pdio8ChannelConfig;
	unsigned char 	 setData;	
} execParam;

typedef void (*EXECFUNCQUEUE) (execParam *pParam);

typedef struct {
	EXECFUNCQUEUE     pFunc;
	execParam         param;
} controlThreadQueueData;


typedef struct {
	char                          threadName[60];
	epicsThreadId                 threadId;
	unsigned int                  threadPriority;
	unsigned int                  threadStackSize;
	EPICSTHREADFUNC               threadFunc;
	void                          *threadParam;
	
	epicsMessageQueueId           threadQueueId;
	
} drvPXISCXI_controlThreadConfig;



typedef struct {
	        ELLLIST         *ptaskList;
		ELLLIST		*pdestroiedList;
		ELLLIST		*pcernoxDataList;

		unsigned long long int one_sec_interval;
} drvPXISCXIConfig;





typedef struct {
	ELLNODE         node;
        char            taskName[60];
	TaskHandle      taskHandle;
        epicsMutexId    taskLock;
        epicsMutexId    bufferLock;
        IOSCANPVT       ioScanPvt;

	unsigned short	taskStatus;

	float64		samplingRate;

	int32 CVICALLBACK* cviCallback;

	ELLLIST		*pdataPoolList;
	ELLLIST		*pchannelConfig;

	float64		*pdata;

	drvPXISCXI_controlThreadConfig    *pcontrolThreadConfig;

	double          callbackTimeUsec;

	double          adjTime_smplRate_Usec;
	double          adjTime_cernoxGain_Usec;
	double          adjTime_lowPass_Usec;

	unsigned        adjCount_smplRate;
	unsigned        adjCount_cernoxGain;
	unsigned        adjCount_lowPass;
} drvPXISCXI_taskConfig;

typedef struct {
	ELLNODE		node;
	unsigned long	used_count;
	int32		num_read_data;
	float64		*pdata;
} dataPoolConfig;



typedef struct {
        ELLNODE         node;
	unsigned short	channelType;
	unsigned short  channelStatus;
        char            chanName[60];
        unsigned        chanIndex;

        float64         gain;
	float64		cutOffFreq;

        float64         minRange;
        float64         maxRange;
        float64         excitationCurrent;

        drvPXISCXI_taskConfig     *ptaskConfig;
	
	double		conversionTime_usec;

} drvPXISCXI_cernoxChannelConfig;

typedef struct {
	ELLNODE		node;
	unsigned short	channelType;
	unsigned short	channelStatus;
	char		chanName[60];
	unsigned	chanIndex;

	float64		gain;
	float64		cutOffFreq;

	float64		minRange;
	float64		maxRange;
	int32		units;
	char            bridgeStr[60];
	int32		bridgeType;
	int32		excitationSource;
	float64		excitationVoltage;
	float64		gageFactor;
	float64         initialBridgeVoltage;
	float64         nominalGageResistance;
	float64		poissonRate;
	float64		leadWireResistance;

	drvPXISCXI_taskConfig	*ptaskConfig;

} drvPXISCXI_strainChannelConfig;

/* 20070325 Modify */
typedef struct { 
	ELLNODE		node;
	unsigned short  channelType;
	unsigned short  channelStatus;
	char		chanName[60];
	unsigned	chanIndex;
	
	float64 	gain;
	float64		cutOffFreq;
  
  	char 	     	terminalStr[60];
  	int32   	terminalType;
	float64		minRange;
	float64		maxRange;
	int32		units;
	char    	bridgeStr[60];
	int32 		bridgeType;
	int32		excitationSource;
	float64		excitationVoltage;

	drvPXISCXI_taskConfig	*ptaskConfig;
	
}drvPXISCXI_displacementChannelConfig;


typedef struct {
	ELLNODE		node;
	unsigned short	channelType;
	unsigned short	channelStatus;
	char		chanName[60];
	unsigned	chanIndex;

	float64		gain;
	float64		cutOffFreq;

	float64		minRange;
	float64		maxRange;

	drvPXISCXI_taskConfig	*ptaskConfig;
	
} drvPXISCXI_hallChannelConfig;

/* 20070504 Modify*/
typedef struct {
	ELLNODE         node;
        unsigned short  channelType;
        unsigned short  channelStatus;
	char		chanName[60];
        unsigned        chanIndex;
                
	float64		gain;
	float64  	cutOffFreq;

	double          adjTime_relayConnect_Usec;
        double          adjTime_relayDisconnect_Usec;
        unsigned        adjCount_relayConnect;
        unsigned        adjCount_relayDisconnect;
	
	char            topologyStr[60];
        char*           topologyType;
        char            chanSecondName[60];
        char            relayName[60];
        unsigned long   relayCount;
        unsigned long   relayPos;
	
        drvPXISCXI_taskConfig   *ptaskConfig;
} drvPXISCXI_relayChannelConfig;

/* Add DO8 chanName is Port */
typedef struct {
        ELLNODE         node;
        unsigned short  channelType;
        unsigned short  channelStatus;
        char            chanName[60];
        unsigned        chanIndex;

        float64         gain;
        float64         cutOffFreq;
	
	double          adjTime_do8_Usec;
        unsigned        adjCount_do8;

        unsigned char   data;
        int32           written;
	int32		read;

        drvPXISCXI_taskConfig   *ptaskConfig;
} drvPXI6514_dio8ChannelConfig;


typedef struct {
	ELLNODE		node;
	unsigned short channelType;
	unsigned short channelStatus;
	char		chanName[60];
	unsigned  	chanIndex;

	float64		gain;
	float64  	cutOffFreq;

} drvPXISCXI_channelConfig;



typedef struct {
	double		zl, zu;
	int		index[10];
	double		coeff[10];
} cernoxSubData;

typedef struct {
	ELLNODE		node;
	char serial[40];
	cernoxSubData *pLowTemp;
	cernoxSubData *pMidTemp;
	cernoxSubData *pHighTemp;
} cernoxDataConfig;



typedef void (*EPICSEXITFUNC)(void *arg);


drvPXISCXI_taskConfig* drvPXISCXI_find_taskConfig(char *taskName);
drvPXISCXI_channelConfig* drvPXISCXI_find_channelConfig(char *taskName, char *chanName);
cernoxDataConfig* drvPXISCXI_find_cernoxDataConfig(char *serial);

void drvPXISCXI_set_cernoxChanGain(drvPXISCXI_taskConfig *ptaskConfig, double gain);
void drvPXISCXI_set_cernoxCurrent(drvPXISCXI_taskConfig *ptaskConfig, float64 current);
void drvPXISCXI_set_lowpassFilter(drvPXISCXI_taskConfig *ptaskConfig, double cutOffFreq);
void drvPXISCXI_set_samplingRate(drvPXISCXI_taskConfig *ptaskConfig, double samplingRate);

/* 20070504 Modify*/
void drvPXISCXI_set_relayChanConnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig);
void drvPXISCXI_set_relayChanDisconnect(drvPXISCXI_relayChannelConfig *prelayChannelConfig); 

/* Add DO8 20070607 */
void drvPXI6514_write_do8Channel(drvPXI6514_dio8ChannelConfig *pdio8ChannelConfig, unsigned char data);
unsigned long long int drvPXISCXI_rdtsc(void);
double                 drvPXISCXI_intervalUSec(unsigned long long int start, unsigned long long int stop);



#endif /* drvPXISCXI_H */

