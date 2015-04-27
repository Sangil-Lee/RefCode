#ifndef __NIDAQMX_DATA__
#define __NIDAQMX_DATA__

#include <epicsThread.h>
#include <epicsMessageQueue.h>
#include <ellLib.h>
#include <errlog.h>

#include <NIDAQmx.h>

#include "rfctrl.h"
#include "rfant.h"

#define DATAFILE_6254  "/tmp/nidaq_6254.dat"
#define DATAFILE_6220  "/tmp/nidaq_6220.dat"

#define LOCAL_MDS			0
#define KSTAR_MDS			1

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

#define MAX_INT		4

#define FINITE_MODE	DAQmx_Val_FiniteSamps
#define CONT_MODE	DAQmx_Val_ContSamps

typedef void (*EPICSEXITFUNC)(void *arg);

typedef struct DaqAiPvt {
    char           portName[30];
    char	   devName[30];
    char	   trgName[30];
    char	   fileName[100];
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

    int32	   totalRead;

    int            trigMode;
    int            extMode;

    int		   smp_mode; /* sampleMode. FiniteSampes, ContSamps, HWTimedSignlePoint */

    int		   gain_mode[16];
    int		   offset_mode[16];

    float	   t0;
    float	   t1;
    float	   fBlipTime;

    float	   gain[16];
    float	   offset[16];
    float	   val[16];

    int            mdsput[16];
    char           mdsnode[16][50];

    int		   int_mode[16]; /* integral calc. */

    FILE	   *fp[16];
} DaqAiPvt;

typedef struct DaqQueueData {
    int		opcode;
} DaqQueueData;

typedef struct DaqIntegralPvt {
    float	lpm[MAX_INT];
    double	pw[MAX_INT];
    float	integral_t[MAX_INT];
    float	beam_pw;
    float	integral_s;
    float	integral_val[MAX_INT];
} DaqIntegralPvt;

int mds_createNewShot(int shot, char *tree);
int acqMdsPutData(DaqAiPvt *daqpvt, int mode);
int rfMdsPutData(RFCTRL_INFO *lhcd_info, int mode);
int rfantMdsPutData(RFANT_INFO *lhcd_info, int mode);

extern int mdsShot;
extern int Mds_LocalShot;
extern int Mds_KstarShot;
extern int KstarMode;

extern DaqAiPvt daq6254pvt;
extern DaqAiPvt daq6220pvt;

extern DaqIntegralPvt daqIntegral;

int SetDaqStart(DaqAiPvt *daqpvt);
int SetDaqStop(DaqAiPvt *daqpvt);
int SetDaqAbort(DaqAiPvt *daqpvt);
int SetDaqFinshStop(DaqAiPvt *daqpvt);

void DeltaT_Integral(DaqAiPvt *daqpvt, double *rawData, int ch);

#endif
