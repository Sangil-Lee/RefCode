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

const int iMode = 1;	

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

const char *VacTagData[6] = {
"EC1_MAXI_STS_NO1","EC1_MAXI_STS_NO2","EC1_MAXI_STS_NO3","EC1_MAXI_STS_NO4","EC1_MAXI_STS_NO5"
,"EC1_MAXI_STS_NO6"};
chid VacTagDatachid[6];

const char *TmpStsTagData[4] = {
"EC1_TMP1_STS_OPR","EC1_TMP2_STS_OPR","EC1_TMP3_STS_OPR","EC1_TMP4_STS_OPR"
};
chid TmpStsTagDatachid[4];

const char *TmpStsStrData[4] = {
"EC1_TMP1_STS_STRING","EC1_TMP2_STS_STRING","EC1_TMP3_STS_STRING","EC1_TMP4_STS_STRING"
};
chid TmpStsStrDatachid[4];

const char *TmpAmpTagData[4] = {
"EC1_TMP1_STS_AMP","EC1_TMP2_STS_AMP","EC1_TMP3_STS_AMP","EC1_TMP4_STS_AMP"
};
chid TmpAmpTagDatachid[4];

const char *TmpRpmTagData[4] = {
"EC1_TMP1_STS_RPM","EC1_TMP2_STS_RPM","EC1_TMP3_STS_RPM","EC1_TMP4_STS_RPM"
};
chid TmpRpmTagDatachid[4];

const char *TmpTempTagData[4] = {
"EC1_TMP1_STS_TEMP","EC1_TMP2_STS_TEMP","EC1_TMP3_STS_TEMP","EC1_TMP4_STS_TEMP"
};
chid TmpTempTagDatachid[4];


const char *VionAmpTagData[2] = {
"EC1_VACION1_STS_AMP","EC1_VACION2_STS_AMP"
};
chid VionAmpTagDatachid[2];

const char *VionFltTagData[2] = {
"EC1_VACION1_STS_FLT","EC1_VACION2_STS_FLT"
};
chid VionFltTagDatachid[2];



int VacpacketParsing();
int VacDataRecv();
void writetag(cUINT utype, chid searchID);
int readtag(cUINT utype, chid searchID);
void SearchTag();
