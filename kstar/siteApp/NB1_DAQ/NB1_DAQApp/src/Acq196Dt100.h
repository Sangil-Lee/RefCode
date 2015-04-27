#ifndef __DT100_ACQ196__
#define __DT100_ACQ196__

#include "local.h"
#include "acq200_ioctl.h"

#include "epicsMessageQueue.h"
#include "ellLib.h"

#define MAX_CARD	2
#define DATA_FILE1 "/media/data1/acq196.2.dat"
#define DATA_FILE2 "/media/data1/acq196.3.dat"
#define STR_CH_MAPPING_FILE "/usr/local/epics/siteApp/config/acq196_ch_mapping.dat"

#define DEF_ACQ_SETMODE		"acqcmd setModeTriggeredContinuous 0 100000\r\n"
#define DEF_ACQ_FIFERR		"set.dtacq_drv FIFERR 0x00000000\r\n"
#define DEF_ACQ_SETROUTE_D3	"set.route d3 in lemo out pxi\r\n"
#define DEF_ACQ_SETROUTE_D0	"set.route d0 in lemo out fpga\r\n"
#define DEF_ACQ_MAXDMA		"set.sys /sys/module/acq200_mu/parameters/max_dma 4000\r\n"
#define DEF_ACQ_CLIP		"set.sys /sys/module/acq200_mu/parameters/maxchain_clip 16\r\n"
#define DEF_ACQ_HITIDE		"set.dtacq_drv hitide 4\r\n"
#define DEF_ACQ_SETARM		"acqcmd setArm\r\n"
#define DEF_ACQ_SETABORT	"acqcmd setAbort\r\n"
#define DEF_ACQ_GETSTATE	"acqcmd getState\r\n"

#define DEF_ACQ_CLOCK		"acqcmd setInternalClock %d\r\n"
#define DEF_ACQ_INIT_CLOCK      "acqcmd setInternalClock 100000\r\n"
#define DEF_ACQ_TRGRISING	"set.trig DI3 rising\r\n"
#define DEF_ACQ_TRGFALLING	"set.trig DI3 falling\r\n"

#define DEF_ACQ_EVTRISING	"set.event0 DI3 rising\r\n"
#define DEF_ACQ_EVTFALLING	"set.event0 DI3 falling\r\n"

#define DEF_ACQ_SETDIO		"acqcmd setDIO ------\r\n"
#define DEF_ACQ_SETCH		"set.channelBlockMask 111\r\n"

#if 0
	#define BUFLEN  0x400000        /* @@todo read from device */
#else
	#define BUFLEN  0x300000        /* @@todo read from device */
#endif

#define INITIAL_TIMEOUT (24*3600*100)

#define LOCAL_MDS		0
#define KSTAR_MDS		1

#define ACQ_ABORT		1
#define ACQ_ARM			2
#define ACQ_TRG_RISING		3
#define ACQ_TRG_FALLING		4
#define ACQ_INT_CLOCK		5
#define ACQ_GET_DATA_TIME	6

#define ACQ_KSTAR_MDSPLUS	10
#define ACQ_KSTAR_RDY		11
#define ACQ_KSTAR_FORCE		12

#define ACQ_ST_STOP	0
#define ACQ_ST_RUN	1

#define FAULT_LIST	0
#define SOFT_TRG	1
#define DTACQ_ST	2

/* D-TACQ Queue Data Operation Code */
#define OP_CODE_DAQ_RUN		1
#define OP_CODE_DAQ_WRITE	2
#define OP_CODE_DAQ_CLOSE	3

#define MDS_PUT_RDY			1
#define MDS_PUT_OK			2
#define MDS_PUT_NONE		0

typedef struct {
    int fd;
    void *pbuf;
    char id[8];
}DeviceMapping;

typedef struct __TagAcq196Drv {
    int sockfd;           /* DT100 socket */
	int card;
	int slot;
    int clock;            /* command --> Internal Clock */
    int arm;              /* command --> setArm, setAbort */
    char state[100];      /* Read State */
    unsigned long long totalNum;
    unsigned long long needDataLen;
    int length;           /* Get Data length */
    float t0;
    float t1;
    float BlipTime;
    int isWrite;
    int shot;              /* Local Shot number */
    int shot_local;        /* Local, KSTAR shot number */
    int set_fault;
    int softTrg;
    int st;                 /* Status */
    int kstar;              /* KSTAR MODE */
    int kstarrdy;           /* KSTAR RDY */
    int kstarshot;          /* KSTAR Shot Number */
    int ca_kstarshot;       /* KSTAR Shot Number CA Link */
    int kstarput;           /* KSTAR MODE Force */
    int kstardaq;           /* KSTAR Daq Start */

	int mdsput_rdy;
	int channelMapping[96];

	DeviceMapping	drv_bufAB[2];
	 struct BufferAB_Ops ops;

    epicsMessageQueueId DaqQueueId;
    ELLLIST *pNodeList;
} Acq196Drv;

typedef struct __TagDtacqDrv {
	int mdsput;            /* Mdsput mode , 0 = NOT, 1 = PUT, 2 = Gain/Offset */
	float gain;
	float offset;
	char node_name[10];
} DtacqDrv;

struct _PsNodeTag {
    int ch;
    char *node;
};

struct _PsNodeGainTag {
    float val;
    char *node;
};

typedef struct __TagEllNode {
    ELLNODE node;
    char data[BUFLEN];
}tagEllNode;

typedef struct __TagQueueData {
    tagEllNode *pNode;
    char opcode;
    int  size;
} tagQueueData;

typedef struct __TagShotInfo {
	int shot_local;
	int ca_kstarshot;
	int kstarshot;
	int kstar;
	int create_shot;
	int slot_count;
	int mdsput_ok[MAX_CARD];
} ShotInfo;

extern ShotInfo shotinfo;

extern Acq196Drv acq196drv[MAX_CARD];
extern DtacqDrv dtacqDrv[MAX_CARD][96];

extern float	nbiPNB_A[2];
extern float	nbiPNB_B[2];

int acqRunThread(Acq196Drv *acqDrv, int num);
int action(Acq196Drv *acqDrv);

int mds_createNewShot(int shot);
int acqMdsPutData(Acq196Drv *acqDrv, int mode);

#endif
