#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <dbEvent.h>
#include <dbDefs.h>
#include <dbCommon.h>
#include <dbAccess.h>
#include <dbAddr.h>
#include <dbFldTypes.h>
#include <registryFunction.h>
#include <epicsExport.h>
#include <recSup.h>
#include <genSubRecord.h>
#include <recGbl.h>
#include <mdslib.h>

int waveformGenSubDebug=0;
epicsExportAddress(int, waveformGenSubDebug);

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

    if(genSubDebug >0)
        printf("Start I/Q Precessing. \n");

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

    if(genSubDebug > 5)
        printf("I/Q Precessed. \n");
  return(nelm);
}

char mpActivePV[40]="icrf:mp:active.VAL";
epicsEnum16 enum16Val = 0;

DBLINK *shotIdOutLink;
long shotId;

void mdsPlusShotID(int param)
{
    int status;
    char buf[40];
    DBADDR    *paddr;

    paddr = (DBADDR *)dbCalloc(1, sizeof(struct dbAddr));

    /* sleep */
    epicsThreadSleep(2.0);
    /*status = dbPutLink(shotIdOutLink, DBR_LONG, &shotId, 1);*/
    /* Put next pulse number */
    sprintf(buf, "icrf:pulseid.VAL");
    status = dbNameToAddr(buf, paddr);
    status = dbPutField(paddr, DBR_LONG, &shotId, 1);

    free(paddr);
    if(genSubDebug > 0)
         printf("genSub: mdsPlusShotID()  next shot number [%ld]. Status=%d\n",shotId,status);

}

#define status_ok(status)  ( ((status) & 1) ==1 )

int mdsPlusManInit_first = 1;

long mdsPlusFindInit()
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

    char buf[40];
    char            treeID[20];
    char            serverID[20];
    int             initShotID = 0;
    long            currentShotID = 0;

    if(mdsPlusManInit_first == 0) return(0);
    mdsPlusManInit_first = 0;

    sprintf(buf,"172.17.101.180:8000 adata");
    i=sscanf(buf,"%s %s",serverID, treeID);

    if(genSubDebug > 5)
            printf("genSub: mdsPlusManInit() Trying MdsConnect[%s,%s]\n",serverID,treeID);
    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
    } else {
        if(genSubDebug > 5)
            printf("genSub: mdsPlusManInit() MdsConnected[%s]\n",serverID);
        status=MdsOpen(treeID, &initShotID);
        if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,initShotID );
        } else {
            if(genSubDebug > 5)
               printf("genSub: mdsPlusManInit() MdsOpened [%s] shot number [%d].\n",
                            treeID,initShotID);
                    /* put data */
            sprintf(mbuf, "TCL(\"SHOW CURRENT %s\",_output)", treeID);
            status = MdsValue(mbuf,&idesc,&null,&len);
            if( !status_ok(status) ) {
                    printf("genSub: Error with %s.\n", mbuf);
            }
            if(genSubDebug > 5)
               printf("genSub: mdsPlusManInit() MdsValue with %s ok.\n",mbuf);

            if( status_ok(tstat) ) {
                status = MdsValue("_output",&sdesc, &null, &len);
                if( !status_ok(status) ) {
                        printf("genSub: Error getting output with SHOW CURRENT.");
                }
                sscanf(mbuf,"Current shot is %ld", &currentShotID);
                if(genSubDebug > 0)
                    printf("genSub: mdsPlusManInit() MdsValue SHOW CURRENT gets shot number [%ld].\n",
                            currentShotID);

                shotId = currentShotID + 1;

                epicsThreadCreate("mdsPlusShotID",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackSmall),
                      (EPICSTHREADFUNC) mdsPlusShotID,
                      0);
            }
        }
        status=MdsClose(treeID, &initShotID);
    }
    return(currentShotID+1);
}

typedef struct {
    long  putFlag;
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
        /* pv for mdsplus active */
        /*sprintf(mpActivePV, "icrf:mp:active.VAL");*/

	mdsPlusFindInit();
        isFirst = 0;
    
        if(genSubDebug > 0)
            printf("genSub: Initialized: mdsPlusInit()\n");
    }
  return(0);
}

long mdsPlusCreatePulse()
{
    int socket;
    int null=0;
    int dtype_long = DTYPE_LONG;
    int dtype_cstring = DTYPE_CSTRING;
    int dtype_float = DTYPE_FLOAT;

    int mdsstat, len;
    int idesc;
    int sdesc;
    int fdesc;
    int status=1,i;

    int bufsize=40;
    char buf[bufsize];
    char            treeID[20];
    char            serverID[20];
    int             shotID = 0;
    int 	    tmpShotID = 0;
    int             nextShotID;
    int             tok_pulse;
    float           coef=1.0;

    DBADDR  *paddr;
    long options, nRequest;

    paddr = (DBADDR *)dbCalloc(1, sizeof(struct dbAddr)); 
    /* Get server information */
    sprintf(buf, "icrf:daq:mptree:i.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGetField(paddr, DBR_STRING, buf, &options, &nRequest, NULL);
    i=sscanf(buf,"%s %s",serverID, treeID);
    /* Connect server */
/*    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
        return(-1);
    }
    if(genSubDebug > 5)
            printf("genSub: mdsPlusCreatePulse() MdsConnected[%s]\n",serverID);
*/
    /* Get pulse id */
/*    sprintf(buf, "icrf:pulseid.VAL");*/
/*    sprintf(buf, "icrf:daq:mcont:pulseid.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGetField(paddr, DBR_LONG, &nextShotID, &options, &nRequest, NULL); 
*/    /* open MDSPlus */
    /* should be opened with shot -1 */
/*    shotID = -1;
    status=MdsOpen(treeID, &shotID);
    if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,shotID );
        return(-1);
    }
    if(genSubDebug > 5)
               printf("genSub: mdsPlusCreatePulse() MdsOpened [%s] shot number [%d].\n",
                            treeID,shotID);
*/    /* create new pulse */
    /* shot increment */
/*    sprintf(buf, "TCL(\"SET CURRENT %s /INCREMENT\")", treeID);
    idesc = descr(&dtype_long, &mdsstat, &null);
    status = MdsValue(buf,&idesc,&null,&len);
    if(genSubDebug > 0)
            printf("genSub: mdsPlusCreatePulse() MdsValue %s\n", buf);
*/
    /* get increased current shot id. increased shot ID should be same to the nextShotID */
/*    sprintf(buf, "TCL(\"SHOW CURRENT %s\",_output)", treeID);
    idesc = descr(&dtype_long, &mdsstat, &null);
    status = MdsValue(buf,&idesc,&null,&len);
    if( !status_ok(status) ) {
        printf("genSub: Error with %s.", buf);
        return (-1);
    }
    if(genSubDebug > 3)
            printf("genSub: mdsPlusCreatePulse() MdsValue %s\n",buf);

    if( status_ok(mdsstat) ) {
        sdesc = descr(&dtype_cstring, buf, &null, &bufsize);
        status = MdsValue("_output",&sdesc, &null, &len);
        if( !status_ok(status) ) {
            printf("genSub: Error getting output with %s.",buf);
        }
        sscanf(buf,"Current shot is %d", &tmpShotID);
        if(tmpShotID != nextShotID ) {
            printf("genSub: Shot ID is incorrect.\n");
            return(-1);
        }
        if(genSubDebug > 0)
                    printf("genSub: mdsPlusCreatePulse() MdsValue SHOW CURRENT gets shot number [%d].\n",
                            tmpShotID);
*/        /* create current pulse */
/*        sprintf(buf, "TCL(\"CREATE PULSE %d\")", tmpShotID);
        status = MdsValue(buf,&idesc,&null,&len);
        if(genSubDebug > 3)
            printf("genSub: mdsPlusCreatePulse() MdsValue %s\n",buf);
    }
*/    /* close and reopen with increased shot id*/
/*    status=MdsClose(treeID, &shotID);*/
    /* shotID=nextShotID;*/
    /* Connect server */
    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
        return(-1);
    }
    if(genSubDebug > 5)
            printf("genSub: mdsPlusCreatePulse() MdsConnected[%s]\n",serverID);

    /* open MDSPlus */
    shotID = 0;
    status=MdsOpen(treeID, &shotID);
    if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,shotID );
        return(-1);
    }
    if(genSubDebug > 5)
               printf("genSub: mdsPlusCreatePulse() MdsOpened [%s] shot number [%d].\n",
                            treeID,shotID);

    /* write data at current(increased) shot id*/
    /* Get tokamak shot ID */
/*    sprintf(buf, "icrf:shotid.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGetField(paddr, DBR_LONG, &tok_pulse, &options, &nRequest, NULL);
*/    /* write tokamak shot id */
/*    idesc = descr(&dtype_long, &tok_pulse, &null);
    status = MdsPut("\\TOK_SHOT", "$", &idesc, &null);
    if(genSubDebug > 3)
            printf("genSub: mdsPlusCreatePulse() MdsPut [\\TOK_SHOT=%d]\n",tok_pulse);
*/
    /* Get current time string */
/*    sprintf(buf, "icrf:ioc:time.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGetField(paddr, DBR_STRING, buf, &options, &nRequest, NULL);
*/    /* write time */
/*    sdesc = descr(&dtype_cstring, buf, &null, &bufsize);
    status = MdsPut("\\PULSE_TIME", "$", &sdesc, &null);
    if(genSubDebug > 3)
            printf("genSub: mdsPlusCreatePulse() MdsPut [\\PULSE_TIME=%s]\n",buf);
*/    /* write coefficient */
    sprintf(buf, "icrf:daq:rfd:coef0.VAL");
    status = dbNameToAddr(buf, paddr);
    status = dbGetField(paddr, DBR_FLOAT, &coef, &options, &nRequest, NULL);
    fdesc = descr(&dtype_float, &coef, &null);
    status = MdsPut("\\ICRF_0C", "$", &fdesc, &null);

    sprintf(buf, "icrf:daq:rfd:coef1.VAL");
    status = dbNameToAddr(buf, paddr);
    status = dbGetField(paddr, DBR_FLOAT, &coef, &options, &nRequest, NULL);
    fdesc = descr(&dtype_float, &coef, &null);
    status = MdsPut("\\ICRF_1C", "$", &fdesc, &null);

    sprintf(buf, "icrf:daq:rfd:coef2.VAL");
    status = dbNameToAddr(buf, paddr);
    status = dbGetField(paddr, DBR_FLOAT, &coef, &options, &nRequest, NULL);
    fdesc = descr(&dtype_float, &coef, &null);
    status = MdsPut("\\ICRF_2C", "$", &fdesc, &null);

    sprintf(buf, "icrf:daq:rfd:coef3.VAL");
    status = dbNameToAddr(buf, paddr);
    status = dbGetField(paddr, DBR_FLOAT, &coef, &options, &nRequest, NULL);
    fdesc = descr(&dtype_float, &coef, &null);
    status = MdsPut("\\ICRF_3C", "$", &fdesc, &null);

    status=MdsClose(treeID, &shotID);
    free(paddr);
    return(0);
}

long mdsPlusPrepNext()
{
    int socket;
    int null=0;
    int dtype_long = DTYPE_LONG;
    int mdsstat, len;
    int idesc;
    int status=1,i;

    int bufsize=40;
    char buf[bufsize];
    char            treeID[20];
    char            serverID[20];
    int             shotID = 0;

    DBADDR  *paddr;
    long options, nRequest;

    paddr = (DBADDR *)dbCalloc(1, sizeof(struct dbAddr));
    /* Get server information */
    sprintf(buf, "icrf:daq:mptree:i.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGet(paddr, DBR_STRING, buf, &options, &nRequest, NULL);
    i=sscanf(buf,"%s %s",serverID, treeID);
    /* Connect server */
    socket=MdsConnect(serverID);
    if(socket == -1) {
        printf("genSub: Error connecting to mdsip server[%s].\n",serverID);
        return(-1);
    }
    if(genSubDebug > 5)
            printf("genSub: mdsPlusPrepNext() MdsConnected[%s]\n",serverID);

    /* Get pulse id */
    sprintf(buf, "icrf:pulseid.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    status = dbGetField(paddr, DBR_LONG, &shotID, &options, &nRequest, NULL);
    /* open MDSPlus*/
    status=MdsOpen(treeID, &shotID);
    if( !status_ok(status) ) {
            printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            treeID,shotID );
        return(-1);
    }
    if(genSubDebug > 5)
               printf("genSub: mdsPlusPrepNext() MdsOpened [%s] shot number [%d].\n",
                            treeID,shotID);

    /* generate MDSPlus event*/
    sprintf(buf, "TCL(\"SETEVENT cmpl\")");
    idesc = descr(&dtype_long, &mdsstat, &null);
    status = MdsValue(buf,&idesc,&null,&len);
    if( !status_ok(status) ) {
            printf("genSub: Error generating event for tree [%s].\n", treeID);
        return(-1);
    }
    if(genSubDebug > 0)
            printf("genSub: mdsPlusPrepNext() MdsValue %s\n", buf);

    /* Put next pulse number */
    sprintf(buf, "icrf:pulseid.VAL");
    status = dbNameToAddr(buf, paddr);
    options = 0;
    nRequest = 1;
    shotID++;
    status = dbPutField(paddr, DBR_LONG, &shotID, 1);

    status=MdsClose(treeID, &shotID);
    free(paddr);
    return(0);
}


/*#define status_ok(status)  ( ((status) & 1) ==1 )*/
int taskPos = 0;
int notifyNewPulse = 1;
int startMdsPut = 0;

void mdsPlusPut_Task(int param)
{
    unsigned long i;
    int socket;
    int null=0;
    int dtypeFloat = DTYPE_FLOAT;
    int dataDesc,timeDesc;
    int status=1, j;

    float          tdT;
    float          toffT;

    int            tnoRd;
    int            tshotID;
    char            ttagName[20];
    char            ttreeID[20];
    char            tserverID[20];
    float           *timeBase;

    DBADDR          *paddr;
    paddr = (DBADDR *)dbCalloc(1, sizeof(struct dbAddr));

    if(genSubDebug > 0)
        printf("Start MdsPut. ##############.\n");
    status = dbNameToAddr(mpActivePV, paddr);
    enum16Val = 1;
    status = dbPutField(paddr, DBR_ENUM, &enum16Val, 1);
    status = mdsPlusCreatePulse();
    /* loop numNode times */
    for(j = 0;j < numNode; j++) {
        if(genSubDebug > 5)
            printf("genSub: mdsPlusPut_Task() started. Task Position=%d, Flag=%ld\n",j,pmdsPutData[j].putFlag);
        if(pmdsPutData[j].putFlag == 0)
            goto endloop;

        epicsThreadSleep(0.01);

        tnoRd=(int)(pmdsPutData[j].noRd);
        tshotID=(int)(pmdsPutData[j].shotID);
        strcpy(ttagName,pmdsPutData[j].tagName);
        strcpy(ttreeID,pmdsPutData[j].treeID);
        strcpy(tserverID,pmdsPutData[j].serverID);

        tdT=pmdsPutData[j].dT;
        toffT=pmdsPutData[j].offT;

        if(genSubDebug > 3)
            printf("genSub: mdsPlusPut_Task() MdsConnect Ready. ServerID=[%s] TreeID=[%s] tagName=[%s] dt=[%f] off=[%f] noRd=[%d]\n",tserverID, ttreeID, ttagName, tdT, toffT, tnoRd);

        /* Connect server and open tree*/
        socket=MdsConnect(tserverID);
        if(socket == -1) {
            printf("genSub: Error connecting to mdsip server[%s].\n",tserverID);
            break;
        }
        if(genSubDebug > 5)
             printf("genSub: mdsPlusPut_Task() MdsConnected[%s]\n",tserverID);

        status=MdsOpen(ttreeID, &tshotID);
        if( !status_ok(status) ) {
             printf("genSub: Error opening tree [%s] for shot [%d].\n",
                            ttreeID,tshotID );
             break;
        }                
        if(genSubDebug > 5)
             printf("genSub: mdsPlusPut_Task() MdsOpened [%s] shot number [%d].\n",
                       ttreeID,tshotID);

        /* put data */
        timeBase = (float *)malloc(tnoRd*sizeof(float));
        for(i=0;i<tnoRd;i++)
            *(timeBase + i) = ((float)i)*tdT + toffT;

        dataDesc=descr(&dtypeFloat,pmdsPutData[j].dataArray,&tnoRd, &null);
        timeDesc=descr(&dtypeFloat,timeBase, &tnoRd, &null);
        status=MdsPut(ttagName,"BUILD_SIGNAL($1,,$2)",&dataDesc,&timeDesc,&null);
        if( !status_ok(status) ) {
            printf("genSub: Error writing signal.\n");
            break;
        }

        if(genSubDebug > 5)
            printf("genSub: mdsPlusPut_Task() MdsPutted to tag [%s]. shot number [%d], noRd=[%d].\n",
                                ttagName,tshotID,tnoRd);
        if(genSubDebug > 10)
            for(i=0;i<tnoRd;i++) {
                    printf("timeBase=%f,data=%f\n",
                                        *((float *)timeBase+i),*((float *)pmdsPutData[j].dataArray+i));
            }
        free(timeBase);
        status=MdsClose(ttreeID, &tshotID);
        if( !status_ok(status) ) {
            printf("genSub: Error closing tree for shot [%d].\n",tshotID );
            break;
        }
        if(genSubDebug > 5)
            printf("genSub: mdsPlusPut_Task() MdsClosed [%s] shot number [%d]\n",ttreeID,tshotID);

        endloop:
        if(genSubDebug > 5)
            printf("genSub: mdsPlusPut_Task() Data discarded for taskPos[%d].\n",j);
    }
    status = mdsPlusPrepNext();
    /* end of mdsput */
    startMdsPut = 0;
    status = dbNameToAddr(mpActivePV, paddr);
    enum16Val = 0;
    status = dbPutField(paddr, DBR_ENUM, &enum16Val, 1);
    free(paddr);
}

long mdsPlusPut( genSubRecord *pgsub )
{
    /*char *buf = (char *)calloc(40, sizeof(char));*/
    int i;
    int nextPos = 0;

    /* if notified new pulse set taskPos = 0 and notify next access will not be new */
    if(notifyNewPulse) {
        taskPos = 0;
        notifyNewPulse = 0;
    }
    /* if startMdsPut is active, do nothing */
    if(startMdsPut) {
        return(0);
    }
 
    nextPos = taskPos;
    pmdsPutData[nextPos].plinkout = pgsub->outa;

    /*buf = (char *)pgsub->a;
    sscanf(buf,"%s %s",pmdsPutData[nextPos].serverID, pmdsPutData[nextPos].treeID);
    buf = (char *)pgsub->g;
    sscanf(buf,"%s",pmdsPutData[nextPos].tagName);*/
    sscanf(pgsub->a,"%s %s",pmdsPutData[nextPos].serverID, pmdsPutData[nextPos].treeID);
    sscanf(pgsub->g,"%s",pmdsPutData[nextPos].tagName);
    /* check shot number */
    pmdsPutData[nextPos].shotID = *((unsigned long *)pgsub->b);
    pmdsPutData[nextPos].noRd = *((unsigned long *)pgsub->c);
    /* check dT and offT */
    pmdsPutData[nextPos].dT = *((float *)pgsub->d);
    pmdsPutData[nextPos].offT = *((float *)pgsub->e);
    /* check the node is valid */
    pmdsPutData[nextPos].putFlag = *((unsigned long *)pgsub->h);

    if(genSubDebug > 0)
        printf("genSub: Ready mdsPlusPut() shotID=%lu,treeID=%s,serverID=%s,tagName=%s, noRd=%lu\n",
                     pmdsPutData[nextPos].shotID, pmdsPutData[nextPos].treeID, pmdsPutData[nextPos].serverID, 
                        pmdsPutData[nextPos].tagName, pmdsPutData[nextPos].noRd);
    /* alocate data memory by noRd */
    /*pmdsPutData[nextPos].dataArray = (float *)calloc(pmdsPutData[nextPos].noRd, sizeofTypes[pgsub->ftf]);*/
    pmdsPutData[nextPos].dataArray = (float *)realloc(pmdsPutData[nextPos].dataArray, 
                                                        pmdsPutData[nextPos].noRd * sizeofTypes[pgsub->ftf]);
    /* put data */
    memcpy(pmdsPutData[nextPos].dataArray, pgsub->f, pmdsPutData[nextPos].noRd*sizeof(sizeofTypes[pgsub->ftf]));

    /* if taskPos is equal to the numNode-1, start mdsput and notify new pulse for next access.*/
    if(taskPos == numNode - 1 ) {
        notifyNewPulse = 1;
        startMdsPut = 1;
        epicsThreadCreate("mdsPlusPutTask",
                      epicsThreadPriorityLow,
                      epicsThreadGetStackSize(epicsThreadStackMedium),
                      (EPICSTHREADFUNC) mdsPlusPut_Task,
                      0);
    } else {
        taskPos = (taskPos+1)&0xf;
    }
    /*free(buf);*/
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


