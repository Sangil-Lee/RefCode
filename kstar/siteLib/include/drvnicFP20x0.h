


#ifndef drvnicFP20x0_H
#define drvnicFP20x0_H

#include "epicsRingBytes.h"
#include "ellLib.h"
#include "callback.h"
#include "iocsh.h"
#include "asynDriver.h"

#include "drvSup.h"
#include "devSup.h"
#include "recSup.h"
#include "dbScan.h"


#define EOS_STR "\r\n"

#define cFP20x0_MSGBUFSIZE 2048
#define cFP20x0_DUMBUFSIZE 1024
#define cFP20x0_CALLBACK_QUEUE_SIZE 8


#define cFP20x0_MODTYPE_TERM   -1
#define cFP20x0_MODTYPE_DI8    0
#define cFP20x0_MODTYPE_DI16   1
#define cFP20x0_MODTYPE_DI32   2
#define cFP20x0_MODTYPE_DO8    3
#define cFP20x0_MODTYPE_DO16   4
#define cFP20x0_MODTYPE_DO32   5
#define cFP20x0_MODTYPE_AI8    6
#define cFP20x0_MODTYPE_AI16   7
#define cFP20x0_MODTYPE_AI32   8
#define cFP20x0_MODTYPE_AO8    9
#define cFP20x0_MODTYPE_AO16   10
#define cFP20x0_MODTYPE_AO32   11

#define cFP20x0_CMD_CHECKMODULE 0
#define cFP20x0_CMD_READ        1
#define cFP20x0_CMD_SETDELAY    2
#define cFP20x0_CMD_WRITE       3
#define cFP20x0_CMD_STR_CHECKMODULE    "CHECK"

#define cFP20x0_STR_MODTYPE_DI16_SHORT  "DI"
#define cFP20x0_STR_MODTYPE_DI8        "DI(8)"
#define cFP20x0_STR_MODTYPE_DI16       "DI(16)"
#define cFP20x0_STR_MODTYPE_DI32       "DI(32)"

#define cFP20x0_STR_MODTYPE_DO16_SHORT  "DO"
#define cFP20x0_STR_MODTYPE_DO8        "DO(8)"
#define cFP20x0_STR_MODTYPE_DO16       "DO(16)"
#define cFP20x0_STR_MODTYPE_DO32       "DO(32)"

#define cFP20x0_STR_MODTYPE_AI8_SHORT  "AI"
#define cFP20x0_STR_MODTYPE_AI2        "AI(2)"
#define cFP20x0_STR_MODTYPE_AI8        "AI(8)"
#define cFP20x0_STR_MODTYPE_AI16       "AI(16)"
#define cFP20x0_STR_MODTYPE_AI32       "AI(32)"

#define cFP20x0_STR_MODTYPE_AO8_SHORT  "AO"
#define cFP20x0_STR_MODTYPE_AO2        "AO(2)"
#define cFP20x0_STR_MODTYPE_AO8        "AO(8)"
#define cFP20x0_STR_MODTYPE_AO16       "AO(16)"
#define cFP20x0_STR_MODTYPE_AO32       "AO(32)"



typedef enum {
    cFP20x0_modType_term = -1,

    cFP20x0_modType_DI8,
    cFP20x0_modType_DI16,
    cFP20x0_modType_DI32,

    cFP20x0_modType_DO8,
    cFP20x0_modType_DO16,
    cFP20x0_modType_DO32,

    cFP20x0_modType_AI2,
    cFP20x0_modType_AI8,
    cFP20x0_modType_AI16,
    cFP20x0_modType_AI32,

    cFP20x0_modType_AO2,
    cFP20x0_modType_AO8,
    cFP20x0_modType_AO16,
    cFP20x0_modType_AO32
} cFP20x0_modType;


typedef struct {
    epicsUInt8          data;
    epicsUInt8          o_data;

    unsigned long       scanCnt[8];
    unsigned long       scanCntMbbiLi;
    IOSCANPVT           ioScanPvt[8];
    IOSCANPVT           ioScanMbbiLiPvt;
} cFP20x0_modConfig_DI8;

typedef struct {
    epicsUInt16         data;
    epicsUInt16         o_data;

    unsigned long       scanCnt[16];
    unsigned long       scanCntMbbiLi;
    IOSCANPVT           ioScanPvt[16];
    IOSCANPVT           ioScanMbbiLiPvt;
} cFP20x0_modConfig_DI16;

typedef struct {
    epicsUInt32         data;
    epicsUInt32         o_data;

    unsigned long       scanCnt[32];
    unsigned long       scanCntMbbiLi;
    IOSCANPVT           ioScanPvt[32];
    IOSCANPVT           ioScanMbbiLiPvt;
} cFP20x0_modConfig_DI32;

typedef struct {
    epicsUInt8          data;
    epicsUInt8          o_data;

    unsigned long       scanCnt[8];
    unsigned long       scanCntMbboLo;
    IOSCANPVT           ioScanPvt[8];
    IOSCANPVT           ioScanMbboLoPvt;
} cFP20x0_modConfig_DO8;

typedef struct {
    epicsUInt16         data;
    epicsUInt16         o_data;

    unsigned long       scanCnt[16];
    unsigned long       scanCntMbboLo;
    IOSCANPVT           ioScanPvt[16];
    IOSCANPVT           ioScanMbboLoPvt;
} cFP20x0_modConfig_DO16;

typedef struct {
    epicsUInt32         data;
    epicsUInt32         o_data;

    unsigned long       scanCnt[32];
    unsigned long       scanCntMbboLo;
    IOSCANPVT           ioScanPvt[32];
    IOSCANPVT           ioScanMbboLoPvt;
} cFP20x0_modConfig_DO32;

typedef struct {
    epicsFloat32        data[2];
    epicsFloat32        o_data[2];

    unsigned long       scanCnt[2];
    IOSCANPVT           ioScanPvt[2];
} cFP20x0_modConfig_AI2;

typedef struct {
    epicsFloat32        data[8];
    epicsFloat32        o_data[8];

    unsigned long       scanCnt[8];
    IOSCANPVT           ioScanPvt[8];
} cFP20x0_modConfig_AI8;

typedef struct {
    epicsFloat32        data[16];
    epicsFloat32        o_data[16];

    unsigned long       scanCnt[16];
    IOSCANPVT           ioScanPvt[16];
} cFP20x0_modConfig_AI16;

typedef struct {
    epicsFloat32        data[32];
    epicsFloat32        o_data[32];

    unsigned long       scanCnt[32];
    IOSCANPVT           ioScanPvt[32];
} cFP20x0_modConfig_AI32;

typedef struct {
    epicsFloat32        data[2];
    epicsFloat32        o_data[2];
  
    unsigned long       scanCnt[2];
    IOSCANPVT           ioScanPvt[2];
} cFP20x0_modConfig_AO2;

typedef struct {
    epicsFloat32        data[8];
    epicsFloat32        o_data[8];

    unsigned long       scanCnt[8];
    IOSCANPVT           ioScanPvt[8];
} cFP20x0_modConfig_AO8;

typedef struct {
    epicsFloat32        data[16];
    epicsFloat32        o_data[16];

    unsigned long       scanCnt[16];
    IOSCANPVT           ioScanPvt[16];
} cFP20x0_modConfig_AO16;

typedef struct {
    epicsFloat32        data[32];
    epicsFloat32        o_data[32];

    unsigned long       scanCnt[32];
    IOSCANPVT           ioScanPvt[32];
} cFP20x0_modConfig_AO32;


typedef union {
    cFP20x0_modConfig_DI8 vcFP20x0_modConfig_DI8;
    cFP20x0_modConfig_DI16 vcFP20x0_modConfig_DI16;
    cFP20x0_modConfig_DI32 vcFP20x0_modConfig_DI32;

    cFP20x0_modConfig_DO8 vcFP20x0_modConfig_DO8;
    cFP20x0_modConfig_DO16 vcFP20x0_modConfig_DO16;
    cFP20x0_modConfig_DO32 vcFP20x0_modConfig_DO32;

    cFP20x0_modConfig_AI2 vcFP20x0_modConfig_AI2;
    cFP20x0_modConfig_AI8 vcFP20x0_modConfig_AI8;
    cFP20x0_modConfig_AI16 vcFP20x0_modConfig_AI16;
    cFP20x0_modConfig_AI32 vcFP20x0_modConfig_AI32;

    cFP20x0_modConfig_AO2 vcFP20x0_modConfig_AO2;
    cFP20x0_modConfig_AO8 vcFP20x0_modConfig_AO8;
    cFP20x0_modConfig_AO16 vcFP20x0_modConfig_AO16;
    cFP20x0_modConfig_AO32 vcFP20x0_modConfig_AO32;
} cFP20x0_modConfig;


typedef struct {
    ELLNODE           node;
    char              modLabel[40];

    cFP20x0_modType   modType;
    char              *modStr;
    char              numCh;
    epicsMutexId      lock;

    cFP20x0_modConfig vcFP20x0_modConfig;

    void            *pdrvcFP20x0Config;
} cFP20x0Module;



typedef struct {
    asynOctet       *pasyncFP20x0Octet; 
    void            *pdrvPvt;
    void            *pdrvcFP20x0Config; 
    char            msgBuf[cFP20x0_MSGBUFSIZE];
    char            dumBuf[cFP20x0_DUMBUFSIZE];
} asyncFP20x0UserData;


typedef struct {
    ELLNODE         node;
    char            msgBuf[cFP20x0_MSGBUFSIZE];
    char            safePad[10];
    unsigned        cnt;
} drvcFP20x0MsgBufNode;

typedef struct {
    char            command;
    char            command_from_postCallback;
    void            *userPvt;
    struct dbCommon *precord;
    char            *msgBuf;
    drvcFP20x0MsgBufNode *pdrvcFP20x0MsgBufNode;
} drvcFP20x0CallbackQueueBuf;
    


typedef struct {
    ELLNODE         node;
    char            port_name[40];
    double          devTimeout;
    double          cbTimeout;
    double          scanInterval;
    int             monitorInterval;

    ELLLIST         *pcFP20x0Module_ellList;
    char            flg_moduleConstruction;
    char            flg_first;
    
    asynUser            *pasyncFP20x0User;
    asyncFP20x0UserData *pasyncFP20x0UserData;

    unsigned char   cbCount;
    unsigned int    timeoutCount;
    unsigned int    reconnCount;
    unsigned int    postReadFailCount;
    unsigned long   asynScanLostCount;
    unsigned long   asynRequestLostCount;
    IOSCANPVT       scanIoScanPvt;

    unsigned short  scanCnt;
    double          scanRate;
    double          scanTime;

    epicsTimeStamp  stamp;
    IOSCANPVT       stampIoScanPvt;

    epicsMutexId    lock;

    CALLBACK        *pPostCallback;

    epicsRingBytesId    queueCallbackRingBytesId;
    epicsRingBytesId    postCallbackRingBytesId;

} drvcFP20x0Config;

#endif /* drvnicFP20x0_H */
