#include "hdr.h"
#include "cadef.h"
#include <stdio.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

typedef unsigned char cBOOL;
typedef unsigned int  cUINT;

typedef enum{false,true} bool;

const int iMode = 2;	//용도 : 0 : 둘다, 1 : LabView만, 2 : DG645만

double dblWriteDATA=0;
float  fltWriteDATA=0;
int    iWriteDATA=0;
int    bWriteDATA=0;

double dblDATA;
float  fltDATA=0;
int    iDATA=0;
int    bDATA=0;
int    istatus;
int    iTAG;
float  fltRead=0;

int skDG645   = 0;	//DG645 socket connection
int iTest   = 0;

char buf[MAX_LINE];
char dataBuf[MAX_LINE];

char bufDG645[MAX_LINE];
char dataBufDG645[MAX_LINE];

char tempbuf[1024];
int iErr = 0;
int iRead = 0;
int iTM = -1; //Trigger Mode

int ichkclock;
int ichkclockdg645;

const int PREPACKET = 6;

const int BUFF_SIZE = 4096;

typedef enum {
    T_CIP_BOOL = 0x00C1,
    T_CIP_SINT = 0x00C2,
    T_CIP_INT  = 0x00C3,
    T_CIP_DINT = 0x00C4,
    T_CIP_REAL = 0x00CA,
    T_CIP_BITS = 0x00D3,
    T_CIP_STRT = 0x02A0
} CIP_Type;

const char *DG645Tag[24] = {
	"EC1_DG_SET_NO","EC1_DG_MOD_INT","EC1_DG_MOD_EXT","EC1_DG_MOD_SNGL","EC1_DG_SNGL_SHOT",
	"EC1_DG_REP_READ","EC1_DG_REP_SET","EC1_DG_ERR_NO","EC1_DG_TSTA_READ","EC1_DG_TSTA_SET",
	"EC1_DG_TSTB_READ","EC1_DG_TSTB_SET","EC1_DG_TSTC_READ","EC1_DG_TSTC_SET","EC1_DG_TSTD_READ",
	"EC1_DG_TSTD_SET","EC1_DG_TSTE_READ","EC1_DG_TSTE_SET","EC1_DG_TSTF_READ","EC1_DG_TSTF_SET",
	"EC1_DG_TSTG_READ","EC1_DG_TSTG_SET","EC1_DG_TSTH_READ","EC1_DG_TSTH_SET"
};

chid DG645chid[24];

void initsocket();

void dg_write();
int dg_read();

void DG645Check();
void DG645SubCheck();
void TriggerMode();
int TriggerModeSet();
void Trigger_DB_Set();
void SingleShot();
int ReadTag();
void writetag();
void Set_Error();
void Set_Zero();
int TimeSet_chk();
int ReadTS();
int DG645_ErrChk();
int dgTM_read();
int Reprate(void);
int dgRR_read();
