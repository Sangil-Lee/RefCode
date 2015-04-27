#ifndef __NIDAQMX_DATA__
#define __NIDAQMX_DATA__

#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <ellLib.h>

#define DATAFILE_6254  "/tmp/nidaq_6254.dat"
#define DATAFILE_6220  "/tmp/nidaq_6220.dat"

#define OP_CODE_DAQ_RUN		1
#define OP_CODE_DAQ_STOP	2
#define OP_CODE_DAQ_CONFIG	3
#define OP_CODE_DAQ_ABORT	4
#define OP_CODE_DAQ_MDSPLUS	5

#define DAQ_STATUS_STOP		0
#define DAQ_STATUS_RUN		1
#define DAQ_STATUS_DONE		2
#define DAQ_STATUS_MDSPLUS	3

#define DAQ_MAX_CH		16
#define DAQ_MAX_DATA		128000000

#define DAQ_START       0
#define DAQ_STOP        1
#define DAQ_ABORT       2
#define DAQ_CONFIG      3
#define DAQ_SAMPCLK     4
#define DAQ_OPRMODE     5

#define MAX_GAIN	11
#define MAX_INT		8

#define POLOSCAN_DONE	0
#define POLOSCAN_START	1
#define POLOSCAN_END	2
#define POLOSCAN_MDS	3

typedef struct DaqAiPvt {
    epicsThreadId  threadId;
    epicsMessageQueueId DaqQueueId;

    TaskHandle     taskHandle;
    float64        rate;
    uInt64         sampsPerChan;
    int32          activeEdge;
    int32          terminalConfig;
    uInt32         pretriggerSamples;
    int32          triggerEdge;
    int32          status;

    int            trigMode;
    int            extMode;
    float64        data[DAQ_MAX_DATA];

#if 0
    int		   localshot;
    int            operation; /* Local,KSTAR MDSPlus */
#endif
    float	   t0;
    float	   t1;
    float	   fBlipTime;

    int            mdsput[16];
    char           mdsnode[16][50];
} DaqAiPvt;

typedef struct PoloScan {
    int		   msg;
    int		   daqReady;
    float	   t0;
    float	   t1;
    int		   sample;
    float	   val;
    float	   mdsval[300];
} PoloScan;

typedef struct DaqQueueData {
    int		opcode;
} DaqQueueData;

typedef struct DaqGainPvt {
    float 	gain[MAX_GAIN];
    float	offset[MAX_GAIN];
    char	mdsnode[MAX_GAIN][50];
} DaqGainPvt;

typedef struct DaqIntegralPvt {
    float	lpm[MAX_INT];
    double	pw[MAX_INT];
    int		integral_t[MAX_INT];
    float	beam_pw;
    int		integral_s;
} DaqIntegralPvt;

#ifdef __cplusplus
extern "C" {
int mds_createNewShot(int shot);
int acqMdsPutData(DaqAiPvt *daqpvt);
int acqMdsPutDataKSTAR(DaqAiPvt *daqpvt);
int acqMdsPoloPutData(PoloScan *scan);
#if 0
int daqMdsPutGain(DaqGainPvt *gainPvt);
#endif
}
#endif

extern int mdsShot;
extern int Mds_LocalShot;
extern int Mds_KstarShot;
extern int KstarMode;

extern DaqAiPvt daq6254pvt;
extern DaqAiPvt daq6220pvt;

extern DaqGainPvt daqGainPvt;
extern DaqIntegralPvt daqIntegral;

extern PoloScan	poloScan;

int SetDaqStart(DaqAiPvt *daqpvt);
int SetDaqStop(DaqAiPvt *daqpvt);
int SetDaqAbort(DaqAiPvt *daqpvt);

#endif
