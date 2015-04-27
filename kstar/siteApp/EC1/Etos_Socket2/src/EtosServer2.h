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

const int iMode = 1;	//용도 : 0 : 둘다, 1 : LabView만, 2 : DG645만

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

const char *VacTagData2[2] = {
"EC1_MAXI2_STS_NO1","EC1_MAXI2_STS_NO2"};
chid VacTagDatachid2[2];

const char *TmpStsTagData[2] = {
"EC1_TMP3_STS_OPR","EC1_TMP4_STS_OPR"
};
chid TmpStsTagDatachid[2];

const char *TmpAmpTagData[2] = {
"EC1_TMP3_STS_AMP","EC1_TMP4_STS_AMP"
};
chid TmpAmpTagDatachid[2];

const char *TmpRpmTagData[2] = {
"EC1_TMP3_STS_RPM","EC1_TMP4_STS_RPM"
};
chid TmpRpmTagDatachid[2];

const char *TmpTempTagData[2] = {
"EC1_TMP3_STS_TEMP","EC1_TMP4_STS_TEMP"
};
chid TmpTempTagDatachid[2];


int VacpacketParsing();
int VacDataRecv();
void writetag(cUINT utype, chid searchID);
int readtag(cUINT utype, chid searchID);
void SearchTag();
