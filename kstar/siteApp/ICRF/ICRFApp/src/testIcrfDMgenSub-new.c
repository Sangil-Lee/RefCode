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

int genSubDebug=10;

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
            /* *(pha+i) = atan( (I-2047) / (float)(Q-2047) ); */
            *(pha+i) = (float)atan2( (double)(I-2047),  (double)(Q-2047) );
        } else {
            *(pha+i) = 3.14159/2.;
        } 
    }
    memcpy(pgsub->vala, amp, nelm*sizeof(float));
    memcpy(pgsub->valb, pha, nelm*sizeof(float));
    pgsub->vald = &nelm;
    free(amp);
    free(pha);

    if(genSubDebug > 3)
        printf("I/Q Precessed. \n");
  return(nelm);
}

DBLINK *shotIdOutLink;
long shotId;

void mdsPlusShotID(int param)
{
    unsigned long i;
    int socket,status;
    /* sleep */
    epicsThreadSleep(5.0);
    status = dbPutLink(shotIdOutLink, DBR_LONG, &shotId, 1);
    if(genSubDebug > 3)
         printf("genSub: mdsPlusShotID()  shot number [%ld]. Status=%d\n",shotId,status);

}

#define status_ok(status)  ( ((status) & 1) ==1 )

int mdsPlusManInit_first = 1;

long mdsPlusManInit( genSubRecord *pgsub)
{
    int socket;
    int null=0;
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int mbufsize = 1024;
    char mbuf[1024];
    int tstat, len;
    int idesc = descr(&dtype_long, &tstat, &null);
    int sdesc = descr(&dtype_cstring, mbuf, &null, &mbufsize);
    int status=1,i;

    char *buf;
    char            treeID[20];
    char            serverID[20];
    int             initShotID = 0;
    long             currentShotID = 0;

    if(mdsPlusManInit_first == 0) return(0);
    mdsPlusManInit_first = 0;

    buf = (char *) calloc(40, sizeof(char));
    /*buf = (char *)pgsub->a;*/
    sprintf(buf,"172.17.101.180:8000 adata");
    i=sscanf(buf,"%s %s",serverID, treeID);

    if(genSubDebug > 3)
            printf("genSub: mdsPlusManInit() Trying MdsConnect[%s,%s]\n",serverID,treeID);
    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
    } else {
        if(genSubDebug > 3)
            printf("genSub: mdsPlusManInit() MdsConnected[%s]\n",serverID);
        status=MdsOpen(treeID, &initShotID);
        if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,initShotID );
        } else {
            if(genSubDebug > 3)
               printf("genSub: mdsPlusManInit() MdsOpened [%s] shot number [%d].\n",
                            treeID,initShotID);
                    /* put data */
            sprintf(mbuf, "TCL(\"SHOW CURRENT %s\",_output)", treeID);
            status = MdsValue(mbuf,&idesc,&null,&len);
            if( !status_ok(status) ) {
                    printf("genSub: Error with %s.\n", mbuf);
            }
            if(genSubDebug > 3)
               printf("genSub: mdsPlusManInit() MdsValue with %s ok.\n",mbuf);

            if( status_ok(tstat) ) {
                status = MdsValue("_output",&sdesc, &null, &len);
                if( !status_ok(status) ) {
                        printf("genSub: Error getting output with SHOW CURRENT.");
                }
                sscanf(mbuf,"Current shot is %ld", &currentShotID);
                if(genSubDebug > 3)
                    printf("genSub: mdsPlusManInit() MdsValue SHOW CURRENT gets shot number [%ld].\n",
                            currentShotID);
                /*pgsub->vala = &currentShotID;*/
                shotId = currentShotID;
                shotIdOutLink = &pgsub->outa;
                epicsThreadCreate("mdsPlusShotID",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackSmall),
                      (EPICSTHREADFUNC) mdsPlusShotID,
                      0);
            }
        }
        status=MdsClose(treeID, &initShotID);
    }
    return(currentShotID);
}

long mdsPlusMan( genSubRecord *pgsub)
{
    int socket;
    int null=0;
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int mbufsize = 1024;
    char mbuf[1024];
    int tstat, len;
    int idesc = descr(&dtype_long, &tstat, &null);
    int sdesc = descr(&dtype_cstring, mbuf, &null, &mbufsize);
    int status=1,i;

    char *buf;
    char            treeID[20];
    char            serverID[20];
    int             initShotID = 0;
    long             currentShotID = 0;
    int             tok_pulse;
    char            pulse_time[120];

    buf = (char *) calloc(40, sizeof(char));

    buf = (char *)pgsub->a;
    i=sscanf(buf,"%s %s",serverID, treeID);

    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
	return(-1);
    }
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsConnected[%s]\n",serverID);
    status=MdsOpen(treeID, &initShotID);
    if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,initShotID );
        return(-1);
    }
    if(genSubDebug > 2)
               printf("genSub: mdsPlusMan() MdsOpened [%s] shot number [%d].\n",
                            treeID,initShotID);
    /* write data at current shot id*/
    /* write tokamak shot id */
    tok_pulse = *((int *)pgsub->b);
    tstat = tok_pulse;
    status = MdsPut("\\TOK_SHOT", "$", &idesc, &null);
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsPut [\\TOK_SHOT=%d]\n",tstat);

    /* write time */    
    buf = (char *)pgsub->c;
    sprintf(mbuf,"%s",buf);
    status = MdsPut("\\PULSE_TIME", "$", &sdesc, &null);
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsPut [\\PULSE_TIME=%s]\n",mbuf);

    /* shot increment */
    sprintf(mbuf, "TCL(\"SET CURRENT %s /INCREMENT\")", treeID);
    status = MdsValue(mbuf,&idesc,&null,&len);
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsValue %s\n", mbuf);

    /* get increased current shot id */
    sprintf(mbuf, "TCL(\"SHOW CURRENT %s\",_output)", treeID);
    status = MdsValue(mbuf,&idesc,&null,&len);
    if( !status_ok(status) ) {
        printf("genSub: Error with %s.", mbuf);
    }
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsValue %s\n",mbuf);

    if( status_ok(tstat) ) {
        status = MdsValue("_output",&sdesc, &null, &len);
        if( !status_ok(status) ) {
            printf("genSub: Error getting output with %s.",mbuf);
        }
        sscanf(mbuf,"Current shot is %ld", &currentShotID);
        if(genSubDebug > 2)
                    printf("genSub: mdsPlusMan() MdsValue SHOW CURRENT gets shot number [%ld].\n",
                            currentShotID);
        /*shotId = currentShotID;
        shotIdOutLink = &pgsub->outa;
        status = dbPutLink(shotIdOutLink, DBR_LONG, &shotId, 1);*/
        pgsub->vala = &currentShotID;
        if(genSubDebug > 2)
             printf("genSub: mdsPlusMan()  shot number [%ld]. Status=%d\n",currentShotID,status);

    }

    /* create current pulse */
    sprintf(mbuf, "TCL(\"CREATE PULSE %ld\")", currentShotID);
    status = MdsValue(mbuf,&idesc,&null,&len);
    if(genSubDebug > 2)
            printf("genSub: mdsPlusMan() MdsValue %s\n",mbuf);

    status=MdsClose(treeID, &initShotID);

    return(currentShotID);
}

typedef struct {
    unsigned short  putFlag;
    float            *dataArray;
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

int isFirst = 1,i;
int numNode = 0;

long mdsPlusInit( genSubRecord *pgsub )
{
    /* increase number of MDSPlus nodes */ 
    numNode++;

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
        for(i=0;i<16;i++) {
            pmdsPutData[i].dataArray = (float *)calloc(2047, sizeof(float));
        }

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

/*#define status_ok(status)  ( ((status) & 1) ==1 )*/
int taskPos = 0;
int notifyNewPulse = 1;
int startMdsPut = 0;

void mdsPlusPut_Task(int param)
{
    unsigned long i;
    int socket, mdsOpen;
    int null=0;
    int dtypeFloat = DTYPE_FLOAT;
    int dataDesc,timeDesc;
    int status=1, i;

    float           *tdataArray;
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
    ttimeBase = (float *)calloc(2047, sizeof(float));
    while(1) {
        /* sleep */
        epicsThreadSleep(0.01);
        if(startMdsPut) {
            /* trigger icrf:daq:next for mpman */
            /* dbput to icrf:daq:next */
            status=ca_put(DBR_LONG, "icrf:daq:next", 1);
            epicsThreadSleep(0.2);
            status=ca_put(DBR_LONG, "icrf:daq:next", 0);
            ca_flush_io(); 
            /* loop numNode times */
            for(j = 0;j < numNode; j++) {
                if(genSubDebug > 8)
                    printf("genSub: mdsPlusPut_Task() started. Task Position=%d\n",j);
                epicsThreadSleep(0.01);

                tnoRd=(int)(pmdsPutData[j].noRd);
                tshotID=(int)(pmdsPutData[j].shotID);
                strcpy(ttagName,pmdsPutData[j].tagName);
                strcpy(ttreeID,pmdsPutData[j].treeID);
                strcpy(tserverID,pmdsPutData[j].serverID);

                tdT=pmdsPutData[j].dT;
                toffT=pmdsPutData[j].offT;

                if(genSubDebug > 8)
                    printf("genSub: mdsPlusPut_Task() MdsConnect Ready. ServerID=[%s] TreeID=[%s] tagName=[%s]\n dt=[%f] off=[%f] noRd=[%d]\n",tserverID, ttreeID, ttagName, tdT, toffT, tnoRd);
                /* Connect server and open tree*/
                socket=MdsConnect(tserverID);
                if(socket == -1) {
                    printf("genSub: Error connecting to mdsip server[%s].\n",tserverID);
                    break;
                }
                if(genSubDebug > 3)
                     printf("genSub: mdsPlusPut_Task() MdsConnected[%s]\n",tserverID);
                status=MdsOpen(ttreeID, &tshotID);
                if( !status_ok(status) ) {
                    printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            ttreeID,tshotID );
                    break;
                }

                ttimeBase = (float *)realloc(ttimeBase, tnoRd*sizeof(float));
                for(i=0;i<tnoRd;i++)
                    *(ttimeBase + i) = ((float)i)*tdT + toffT;
                if(genSubDebug > 3)
                         printf("genSub: mdsPlusPut_Task() MdsConnected[%s]\n",tserverID);
                status=MdsOpen(ttreeID, &tshotID);
                if( !status_ok(status) ) {
                    printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            ttreeID,tshotID );
                    break;
                }                
                if(genSubDebug > 3)
                    printf("genSub: mdsPlusPut_Task() MdsOpened [%s] shot number [%d].\n",
                            ttreeID,tshotID);
                /* put data */
                dataDesc=descr(&dtypeFloat,pmdsPutData[j].dataArray,&tnoRd, &null);
                timeDesc=descr(&dtypeFloat,ttimeBase, &tnoRd, &null);
                status=MdsPut(ttagName,"BUILD_SIGNAL($1,,$2)",&dataDesc,&timeDesc,&null);
                if( !status_ok(status) ) {
                    printf("genSub: Error writing signal.\n");
                    break;
                }
                if(genSubDebug > 3)
                    printf("genSub: mdsPlusPut_Task() MdsPutted to tag [%s]. shot number [%d], noRd=[%d].\n",
                                ttagName,tshotID,tnoRd);
                if(genSubDebug > 11)
                    for(i=0;i<tnoRd;i++) {
                            printf("timeBase=%f,data=%f\n",
                                        *((float *)ttimeBase+i),*((float *)pmdsPutData[j].dataArray+i));
                    }
                }
                pmdsPutData[j].putFlag = 0;
                /* notify ending mdsPut */
                dbPutLink(&(pmdsPutData[j].plinkout), DBR_DOUBLE, &(pmdsPutData[j].putFlag), 1);

                status=MdsClose(ttreeID, &tshotID);
                if( !status_ok(status) ) {
                    printf("genSub: Error closing tree for shot [%d].\n",tshotID );
                    break;
                }
                if(genSubDebug > 3)
                    printf("genSub: mdsPlusPut_Task() MdsClosed [%s] shot number [%d]\n",ttreeID,tshotID);
                }
            }
            
            /* end of mdsput */
            startMdsPut = 0;
        }
    }
}

long mdsPlusPut( genSubRecord *pgsub )
{
    char *buf;
    int i;
    int nextPos = 0;

    buf = (char *) calloc(40, sizeof(char));

    /* if notified new pulse set taskPos = 0 and notify next access will not be new */
    if(notifyNewPulse) {
        taskPos = 0;
        notifyNewPulse = 0;
    }
    /* if startMdsPut is active, do nothing */
    if(startMdsPut) {
        return(0);
    } 
/*    while(pmdsPutData[nextPos].putFlag) {
        nextPos = (nextPos+1)&0xf;
    }
*/
    nextPos = taskPos;
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
        
    if(genSubDebug > 8)
        printf("genSub: Ready mdsPlusPut() shotID=%lu,treeID=%s,serverID=%s,tagName=%s, noRd=%lu\n",
                     pmdsPutData[nextPos].shotID, pmdsPutData[nextPos].treeID, pmdsPutData[nextPos].serverID, 
                        pmdsPutData[nextPos].tagName, pmdsPutData[nextPos].noRd);
    /* alocate data memory by noRd */
    /*pmdsPutData[nextPos].dataArray = (float *)calloc(pmdsPutData[nextPos].noRd, sizeofTypes[pgsub->ftf]);*/
    pmdsPutData[nextPos].dataArray = (float *)realloc(pmdsPutData[nextPos].dataArray, 
                                                        pmdsPutData[nextPos].noRd * sizeofTypes[pgsub->ftf]);
    /* put data */
    memcpy(pmdsPutData[nextPos].dataArray, pgsub->f, pmdsPutData[nextPos].noRd*sizeof(sizeofTypes[pgsub->ftf]));
    pmdsPutData[nextPos].putFlag = 1;
    /* notify starting mdsPut */
    pgsub->vala=&(pmdsPutData[nextPos].putFlag);

    /* if taskPos is equal to the numNode-1, start mdsput and notify new pulse for next access.*/
    if(taskPos == numNode - 1 ) {
        notifyNewPulse = 1;
        startMdsPut = 1;
    } else {
        taskPos = (taskPos+1)&0xf;
    }

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
    {"mdsPlusManInit",(REGISTRYFUNCTION)mdsPlusManInit},
    {"mdsPlusMan",(REGISTRYFUNCTION)mdsPlusMan},
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


