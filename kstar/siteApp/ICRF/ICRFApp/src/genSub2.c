#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <dbAccess.h>
#include <dbFldTypes.h>
#include <registryFunction.h>
#include <epicsExport.h>
#include <recSup.h>
#include <genSubRecord.h>
#include <recGbl.h>

#include <mdslib.h>

int genSubDebug=5;

static int sizeofTypes[]={0, 1, 1, 2, 2, 4, 4, 4, 8, 2};

void mdsPlusPut_Task(int param);

long procIQInit( genSubRecord *pgsub )
{
  return(0);
}

long procIQ( genSubRecord *pgsub )
{
    unsigned short  I;
    unsigned short  Q;
    float *amp;
    float *pha;
    unsigned long nelm,i;

    nelm = *((unsigned long *)pgsub->d);
    /* allocate memory */
    amp = (float *)malloc(nelm*sizeof(float));
    pha = (float *)malloc(nelm*sizeof(float));

    for(i=0;i<nelm;i++) {
        I = *((unsigned short *)pgsub->a+i);
        Q = *((unsigned short *)pgsub->b+i);
        
        *(amp+i) = sqrt((I-2047)*(I-2047)+(Q-2047)*(Q-2047));
        if(Q!=0) {                                      
            *(pha+i) = atan( (I-2047) / (float)(Q-2047) );
        } else {
            *(pha+i) = 3.14159/2.;
        } 
    }
    memcpy(pgsub->vala, amp, nelm*sizeof(float));
    memcpy(pgsub->valb, pha, nelm*sizeof(float));
    pgsub->vald = &nelm;
    if(genSubDebug > 3)
        printf("I/Q Precessed. \n");
  return(nelm);
}

long mdsPlusManInit( genSubRecord *pgsub)
{
    int socket;
    int null=0;
    int dtypeFloat = DTYPE_FLOAT;
    int dataDesc,timeDesc;
    int status=1;

    char *buf;
    unsigned long   shotID;
    char            tagName[20];
    char            treeID[20];
    char            serverID[20];
    int             currentShotID = 0;

    buf = (char *) calloc(40, sizeof(char));
    buf = (char *)pgsub->a;
    i=sscanf(buf,"%s %s",serverID, treeID);
    free(buf);

    socket=MdsConnect(tserverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
    } else {
        if(genSubDebug > 3)
            printf("genSub: mdsPlusPut_Task() MdsConnected[%s]\n",serverID);
            status=MdsOpen(treeID, &currentShotID);
            if( !status_ok(status) ) {
                printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,shotID );
            } else {
                if(genSubDebug > 3)
                   printf("genSub: mdsPlusPut_Task() MdsOpened [%s] shot number [%d].\n",
                            treeID,currentShotID);
                    /* put data */
                    dataDesc=descr(&dtypeFloat,tdataArray,&tnoRd, &null);
                    timeDesc=descr(&dtypeFloat,ttimeBase, &tnoRd, &null);
                    status=MdsPut(ttagName,"BUILD_SIGNAL($1,,$2)",&dataDesc,&timeDesc,&null);
                    if( !status_ok(status) ) {

    return();
}

long mdsPlusMan( genSubRecord *pgsub)
{
    return();
}

typedef struct {
    unsigned short  putFlag;
    void            *dataArray;
    float          dT;
    float          offT;

    unsigned long   noRd;
    unsigned long   shotID;
    char            tagName[20];
    char            treeID[20];
    char            serverID[20];

    DBLINK plinkout; /* link for notifying end of data putting */
} mdsPutData;

mdsPutData pmdsPutData[16];

int isFirst = 1;

long mdsPlusInit( genSubRecord *pgsub )
{
    /* verify input fields */
    if(pgsub->fta != DBF_STRING) { /* mdsPlus IDs*/ 
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTA field");
        return(0);
    }
    if(pgsub->ftb != DBF_ULONG) {/* node name */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTB field");
        return(0);
    }
    if(pgsub->ftc != DBF_ULONG) {/* number of elements */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTC field");
        return(0);
    }
    if(pgsub->ftd != DBF_FLOAT) {/* dT */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTD field");
        return(0);
    }
    if(pgsub->fte != DBF_FLOAT) { /* offT */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTE field");
        return(0);
    }
    if(pgsub->ftf != DBF_FLOAT) { /* data */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTF field");
        return(0);
    }
    if(pgsub->ftg != DBF_STRING) { /* tagname  */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal FTG field");
        return(0);
    }
    if(pgsub->noa != 1) {/* mdsPlus IDs */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal NOA field");
        return(0);
    }
    if(pgsub->nob != 1) {/* shotID */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal NOB field");
        return(0);
    }
    if(pgsub->noc != 1) {/* number of elements */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal NOC field");
        return(0);
    }
    if(pgsub->nod != 1) {/* dT */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal NOD field");
        return(0);
    }
    if(pgsub->noe != 1) {/* offT */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Illegal NOE field");
        return(0);
    }
    if(pgsub->nof < 100000) {/* data array */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Small NOF field");
        return(0);
    }
    if(pgsub->nog !=  1) {/* Tag name */
        recGblRecordError(S_db_badField, (void*)pgsub,
                            "mdsplusInit. Small NOG field");
        return(0);
    }

    if(isFirst) {
        epicsThreadCreate("mdsPlusPutTask",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC) mdsPlusPut_Task,
                      0);
        isFirst = 0;
    }
    if(genSubDebug > 3)
        printf("genSub: Initialized: mdsPlusInit()\n");

  return(0);
}

#define status_ok(status)  ( ((status) & 1) ==1 )
int taskPos = 0;

void mdsPlusPut_Task(int param)
{
    unsigned long i;
    int socket;
    int null=0;
    int dtypeFloat = DTYPE_FLOAT;
    int dataDesc,timeDesc;
    int status=1;

    void            *tdataArray;
    float           *ttimeBase;
    float          tdT;
    float          toffT;

    int            tnoRd;
    int            tshotID;
    char            ttagName[20];
    char            ttreeID[20];
    char            tserverID[20];


    if(genSubDebug > 3)
        printf("genSub: Thread created: mdsPlusPut_Task()\n"); 

    while(1) {
        if(genSubDebug > 5)
            printf("genSub taskPos=[%d], flag=[%hu]\n",taskPos,pmdsPutData[taskPos].putFlag);
        if(pmdsPutData[taskPos].putFlag == 1) {
            tdataArray=pmdsPutData[taskPos].dataArray;
            tdT=pmdsPutData[taskPos].dT;
            toffT=pmdsPutData[taskPos].offT;

            tnoRd=(int)(pmdsPutData[taskPos].noRd);
            tshotID=(int)(pmdsPutData[taskPos].shotID);
            strcpy(ttagName,pmdsPutData[taskPos].tagName);
            strcpy(ttreeID,pmdsPutData[taskPos].treeID);
            strcpy(tserverID,pmdsPutData[taskPos].serverID);
            if(genSubDebug > 3)
                    printf("genSub: mdsPlusPut_Task() MdsConnect Ready,\n ServerID=[%s] TreeID=[%s] tagName=[%s]\n dt=[%f] off=[%f] noRd=[%d]\n",tserverID, ttreeID, ttagName, tdT, toffT, tnoRd);
            ttimeBase = (float *)calloc(tnoRd, sizeof(float));
            for(i=0;i<tnoRd;i++)
                *(ttimeBase + i) = ((float)i)*tdT + toffT;
            /* connect mdsPlus server */
            socket=MdsConnect(tserverID);
            if(socket == -1) {
                printf("genSub: Error connecting to mdsip server[%s].\n",tserverID);
            } else {
                if(genSubDebug > 3)
                    printf("genSub: mdsPlusPut_Task() MdsConnected[%s]\n",tserverID);
                status=MdsOpen(ttreeID, &tshotID);
                if( !status_ok(status) ) {
                        printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            ttreeID,tshotID );
                } else {                
                    if(genSubDebug > 3)
                       printf("genSub: mdsPlusPut_Task() MdsOpened [%s] shot number [%d].\n",
                            ttreeID,tshotID);
                    /* put data */
                    dataDesc=descr(&dtypeFloat,tdataArray,&tnoRd, &null);
                    timeDesc=descr(&dtypeFloat,ttimeBase, &tnoRd, &null);
                    status=MdsPut(ttagName,"BUILD_SIGNAL($1,,$2)",&dataDesc,&timeDesc,&null);
                    if( !status_ok(status) ) {
                        printf("genSub: Error writing signal.\n");
                    } else {
                      if(genSubDebug > 3)
                            printf("genSub: mdsPlusPut_Task() MdsPutted to tag [%s]. shot number [%d], noRd=[%d].\n",
                                ttagName,tshotID,tnoRd);
                      if(genSubDebug > 8)
                        for(i=0;i<tnoRd;i++) {
                            printf("timeBase=%f,data=%f\n",
                                        *((float *)ttimeBase+i),*((float *)tdataArray+i));
                        }
                    }
                    /* disconnect */
                    status=MdsClose(ttreeID, &tshotID);
                    if( !status_ok(status) ) {
                    printf("genSub: Error closing tree for shot [%d].\n",tshotID );
                    } else {
                        if(genSubDebug > 3)
                            printf("genSub: mdsPlusPut_Task() MdsClosed [%s] shot number [%d]\n",ttreeID,tshotID);
                    }
                }
            }
            /* free allocated memory */
            free(pmdsPutData[taskPos].dataArray);
            free(ttimeBase);
            /* unset flag */
            pmdsPutData[taskPos].putFlag = 0;
            /* notify ending mdsPut */
            dbPutLink(&(pmdsPutData[taskPos].plinkout), DBR_DOUBLE, &(pmdsPutData[taskPos].putFlag), 1);
        }
        /* sleep */
        epicsThreadSleep(0.1);
        taskPos = (taskPos+1)&0xf;
    }
}

long mdsPlusPut( genSubRecord *pgsub )
{
    char *buf;
    int i;
    int nextPos = 0;

    buf = (char *) calloc(40, sizeof(char));

    nextPos = taskPos;
    while(pmdsPutData[nextPos].putFlag) {
        nextPos = (nextPos+1)&0xf;
    }
    pmdsPutData[nextPos].plinkout = pgsub->outa;

    buf = (char *)pgsub->a;
    i=sscanf(buf,"%s %s",pmdsPutData[nextPos].serverID, pmdsPutData[nextPos].treeID);
    buf = (char *)pgsub->g;
    i=sscanf(buf,"%s",pmdsPutData[nextPos].tagName);
    /* check shot number */
    pmdsPutData[nextPos].shotID = *((unsigned long *)pgsub->b);
    pmdsPutData[nextPos].noRd = *((unsigned long *)pgsub->c);
    /* check dT and offT */
    pmdsPutData[nextPos].dT = *((float *)pgsub->d);
    pmdsPutData[nextPos].offT = *((float *)pgsub->e);
        
    if(genSubDebug > 3)
        printf("genSub: Ready for mdsPut: mdsPlusPut()\n shotID=%lu, treeID=%s, serverID=%s, tagName=%s\n noRd=%lu\n",
                     pmdsPutData[nextPos].shotID, pmdsPutData[nextPos].treeID, pmdsPutData[nextPos].serverID, 
                        pmdsPutData[nextPos].tagName, pmdsPutData[nextPos].noRd);
    /* alocate data memory by noRd */
    pmdsPutData[nextPos].dataArray = (char *)calloc(pmdsPutData[nextPos].noRd, sizeofTypes[pgsub->ftf]);
    /* put data */
    memcpy(pmdsPutData[nextPos].dataArray, pgsub->f, pmdsPutData[nextPos].noRd*sizeof(sizeofTypes[pgsub->ftf]));
    pmdsPutData[nextPos].putFlag = 1;
    /* notify starting mdsPut */
    pgsub->vala=&(pmdsPutData[nextPos].putFlag);

    return(pmdsPutData[nextPos].noRd);
}


/* Register these symbols for use by IOC code */
/*
epicsRegisterFunction( procIQInit );
epicsRegisterFunction( procIQ );
epicsRegisterFunction( mdsPlusInit );
epicsRegisterFunction( mdsPlusPut );
*/

static registryFunctionRef icrfSubRef[] = {
    {"mdsPlusInit",(REGISTRYFUNCTION)mdsPlusInit},
    {"mdsPlusPut",(REGISTRYFUNCTION)mdsPlusPut},
    {"procIQInit",(REGISTRYFUNCTION)procIQInit},
    {"procIQ",(REGISTRYFUNCTION)procIQ}
};

static void icrfSub(void)
{
    registryFunctionRefAdd(icrfSubRef,NELEMENTS(icrfSubRef));
}
epicsExportRegistrar(icrfSub);


