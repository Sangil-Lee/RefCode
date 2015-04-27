#ifndef  devMDSplus_H
#define  devMDSplus_H



#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#define OPMODE_REMOTE		1
#define OPMODE_CALIBRATION	2
#define OPMODE_TEST		3



#define ADDR_MODEL_TREE		"192.168.1.92:8000" 
/*#define ADDR_MODEL_TREE		"192.168.1.10:8000" */

#define FILE_NAME_RAWDATA		"/data/rawdata.dat"
#define FILE_NAME_NETDATA		"/data/netdata.dat"

/* my_tree
raw
*/
#define TREE_NAME	"raw"

/*
TAG1
FL1
*/
#define NODE_NAME	"\\FL1"

/* Useful definition for handling returned status values */
#define statusOk(status) ((status) & 1)
/*Internal function prototypes */
int getSize(char *sigName);
int readModelTree(int num);
/* int sendEDtoTreeFromChannelData(int shot, int mode);  Old Function - WY.Lee Delete at 20080930 */
/* Blip Time Infomation Transports Function Modify at 20080930 TG.Lee 
int sendEDtoTreeFromChannelData(int shot, int mode, float blip); */
int sendEDtoTreeFromChannelData(int shot, int mode);
int dataChannelizationAndSendData(int shot, int mode);
/*int sendEDtoTreeFromBigFile(int shot); */
int swap32(int *n);
int dataChannelization();
int checkDataMiss();
int completeDataPut();
int channelAverageForECE();

int createNewShot(int shot);


#if 0
#define NODE_NAME	":SUB1.SUB2:MYSIGNAL"
#elif 0
#define NODE_NAME	"\\MYTAG"
#elif 0
#define NODE_NAME	"\\TAG1"
#endif

/* unsigned int g_sampleDataCnt;  100 Sec Over Time .. Error ... Maybe   TG.Lee 20081202*/
unsigned long long g_sampleDataCnt;

char	 channelTagName[256][40];
unsigned short channelOnOff[256];



#endif
