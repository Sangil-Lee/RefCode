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

const int iMode = 1;	//Mode : 0 : LabView DG645, 1 : LabView 2 : DG645Œ

double dblWriteDATA=0;
float  fltWriteDATA=0;
int    iWriteDATA=0;
int    bWriteDATA=0;
int	iLabRoopCnt=0;
double dblDATA;
float  fltDATA=0;
int    iDATA=0;
int    bDATA=0;
int    istatus;
int    iTAG;
float  fltRead=0;

int skLabVIEW = 0;	//LabView socket connection
int iTest   = 0;

char buf[MAX_LINE];
char dataBuf[MAX_LINE];

char bufDG645[MAX_LINE];
char dataBufDG645[MAX_LINE];

char tempbuf[1024];
int iErr = 0;
int iRead = 0;

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

const char *RTDTagData[32] = {
"EC1_WCS_RTD_S03","EC1_WCS_RTD_S05","EC1_WCS_RTD_S09","EC1_WCS_RTD_S10","EC1_WCS_RTD_S11",
"EC1_WCS_RTD_S12","EC1_WCS_RTD_S17","EC1_WCS_RTD_S18","EC1_WCS_RTD_S19","EC1_WCS_RTD_S20",
"EC1_WCS_RTD_S21","EC1_WCS_RTD_S22","EC1_FCS_RTD_SSW","EC1_FCS_RTD_SDB","EC1_WCS_RTD_S07",
"EC1_WCS_RTD_S08","EC1_WCS_RTD_R03","EC1_WCS_RTD_R05","EC1_WCS_RTD_R09","EC1_WCS_RTD_R10",
"EC1_WCS_RTD_R11","EC1_WCS_RTD_R12","EC1_WCS_RTD_R17","EC1_WCS_RTD_R18","EC1_WCS_RTD_R19",
"EC1_WCS_RTD_R20","EC1_WCS_RTD_R21","EC1_WCS_RTD_R22","EC1_FCS_RTD_RSW","EC1_FCS_RTD_RDB",
"EC1_WCS_RTD_R07","EC1_WCS_RTD_R08"
};

const char *RTDTagStat[16] = {
"EC1_WCS_RTD_STAT03","EC1_WCS_RTD_STAT05","EC1_WCS_RTD_STAT09",
"EC1_WCS_RTD_STAT10","EC1_WCS_RTD_STAT11","EC1_WCS_RTD_STAT12","EC1_WCS_RTD_STAT17","EC1_WCS_RTD_STAT18",
"EC1_WCS_RTD_STAT19","EC1_WCS_RTD_STAT20","EC1_WCS_RTD_STAT21","EC1_WCS_RTD_STAT22","EC1_FCS_RTD_STATSW",
"EC1_FCS_RTD_STATDB","EC1_WCS_RTD_STAT07","EC1_WCS_RTD_STAT08"
};

const char *RTDTag[50];
chid RTDchid[50];

const char *FRTag[36];
chid FRchid[36];

const char *TTTag[52];
chid TTchid[52];

const char *SETag[84];
chid SEchid[84];
float fltSetVal[84];

const char *EtcTag[7];
chid Etcchid[7];

const char *TDTag[43];
chid TDchid[41];

void initsocket();


int ReadTag();
void writetag();
int Reprate(void);

void LabVIEWCheck();
int LVDataRecv();
int RTDpacketParsing();
int FRpacketParsing();
int TTpacketParsing();
int SEpacketParsing();
int TDpacketParsing();
int SocketAsk();
void SendSet(void);
int SendINTG(void);
int StopINTG(void);

int ReadRTDFile();
int ReadFRFile();
int ReadTTFile();
int ReadSEFile();
int ReadTDFile();
int ReadEtcFile();
