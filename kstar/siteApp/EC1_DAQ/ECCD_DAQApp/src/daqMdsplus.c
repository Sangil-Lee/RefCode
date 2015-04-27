#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "NIDAQmx.h"
#include "nidaq.h"

#define STATUS_OK(status) (((status) & 1) == 1)

int mds_createNewShot(int shot)
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
        int tstat, len;
        char buf[128];
        int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an      integer */
        int idesc = descr(&dtype_Long, &tstat, &null);

        sprintf(buf, "TCL(\"set tree eccd1/shot=-1\")");
        status = MdsValue(buf, &idesc, &null, &len);
        if ( !((status) & 1) )
        {
            printf("-- TCL Set tree Error --\n");
            return -1;
        }
        sprintf(buf, "TCL(\"create pulse %d\")", shot);
        status = MdsValue(buf, &idesc, &null, &len);
        if ( !((status) & 1) )
        {
            printf("-- Create Pulse Error --\n");
            return -1;
        }
        status = MdsValue("TCL(\"close\")", &idesc, &null, &len);

        printf("Local Mdsplus Tree Create [%d]\n", shot);
        return 0;
}

int acqMdsPutData(DaqAiPvt *daqpvt)
{
    int _null = 0;
    int status;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Double = DTYPE_DOUBLE;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;

    int i, j;
    int nIndex = 0;
    int nNumSampleCnt = daqpvt->sampsPerChan;

    float fStartTime;
    float fEndTime;
    float fRealTimeGap;

    int   mdsSock;
    int   shot = mdsShot;
    char  tempNode[256];
    char  szTemp[256];

    float64 *getData;

    getData = (float64 *)malloc(sizeof(float64)*nNumSampleCnt);

    if(getData == NULL) {
        printf("--- malloc error ---\n");
        return -1;
    }

    printf("--- MdsConnect() ---\n");
    mdsSock = MdsConnect("172.17.121.251:8000");

    if(mdsSock == -1) {
		free (getData);
        printf("--- MDS Connect Faile ---\n");
        return -1;
    }

    printf("--- MdsOpen() ---\n");
    status = MdsOpen("eccd1", &shot);

    if ( !STATUS_OK(status) )
    {
		free (getData);
		MdsDisconnect();
      fprintf(stderr,"Error opening tree for shot %d.\n",shot);
      printf("%s\n", MdsGetMsg(status));
      return -1;
    }

/*
    fStartTime = (daqpvt->t0 + daqpvt->fBlipTime) * daqpvt->rate;
*/
    fStartTime = daqpvt->t0;
    fEndTime = daqpvt->sampsPerChan + fStartTime - 1.0;

    fRealTimeGap = 1.0 / (float64)daqpvt->rate;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

    printf("--- MdsPut ---\n");
    for(i=0; i < 16; i++) {

        for(j=0; j < daqpvt->sampsPerChan; j++) {
            getData[j] = daqpvt->data[j+nIndex];
        }
        nIndex += daqpvt->sampsPerChan;

        rawDataDesc = descr(&dtype_Double, getData, &nNumSampleCnt, &_null);

        if(daqpvt->mdsput[i] != 0) {
            printf("MdsPut : Ch %d\n", i);
            sprintf(tempNode, "\\%s:FOO", daqpvt->mdsnode[i]);
      	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);
          }

        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqpvt->mdsnode[i]);
        }
    }

    for(i=0; i < MAX_GAIN; i++) {
        sprintf(szTemp, "%f", daqGainPvt.gain[i]);
        sprintf(tempNode, "\\%sP_GAIN", daqGainPvt.mdsnode[i]);
        status = MdsPut(tempNode, szTemp, &_null);
    
        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqGainPvt.mdsnode[i]);
        }
    
        sprintf(szTemp, "%f", daqGainPvt.offset[i]);
        sprintf(tempNode, "\\%sP_OFFSET", daqGainPvt.mdsnode[i]);
        status = MdsPut(tempNode, szTemp, &_null);

        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqGainPvt.mdsnode[i]);
        }
    }
    status = MdsClose("eccd1", &shot);
	MdsDisconnect();

    free(getData);
    getData = NULL;

    printf("--- MDSPlus Data Put OK ---\n");

    return 0;
}

int acqMdsPutDataKSTAR(DaqAiPvt *daqpvt)
{
    int _null = 0;
    int status;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Double = DTYPE_DOUBLE;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;

    int i, j;
    int nIndex = 0;
    int nNumSampleCnt = daqpvt->sampsPerChan;

    float fStartTime;
    float fEndTime;
    float fRealTimeGap;

    int   mdsSock;
    int   shot = mdsShot;
    char  szTemp[256];
    char  tempNode[256];

    float64 *getData;

    getData = (float64 *)malloc(sizeof(float64)*nNumSampleCnt);

    if(getData == NULL) {
        printf("--- malloc error ---\n");
        return -1;
    }

    printf("--- KSTAR MdsConnect() ---\n");
    mdsSock = MdsConnect("172.17.100.202:8000");

    if(mdsSock == -1) {
		free (getData);
        printf("--- KSTAR MDS Connect Faile ---\n");
        return -1;
    }

    printf("--- KSTAR MdsOpen() ---\n");
    status = MdsOpen("HEATING", &shot);

    if ( !STATUS_OK(status) )
    {
		MdsDisconnect();
		free (getData);
      fprintf(stderr,"Error opening tree for shot %d.\n",shot);
      printf("%s\n", MdsGetMsg(status));
      return -1;
    }

    fStartTime = (daqpvt->t0 + daqpvt->fBlipTime) * daqpvt->rate;
    fEndTime = daqpvt->sampsPerChan + fStartTime - 1.0;

    fRealTimeGap = 1.0 / (float64)daqpvt->rate;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

    printf("--- KSTAR MdsPut ---\n");
    for(i=0; i < 16; i++) {

        for(j=0; j < daqpvt->sampsPerChan; j++) {
            getData[j] = daqpvt->data[j+nIndex];
        }
        nIndex += daqpvt->sampsPerChan;

        rawDataDesc = descr(&dtype_Double, getData, &nNumSampleCnt, &_null);
       
        if(daqpvt->mdsput[i] != 0) {
            /*sprintf(szTemp, "ECCD.ECCD1.%s:FOO", daqpvt->mdsnode[i]);*/
            sprintf(szTemp, "\\%s:FOO", daqpvt->mdsnode[i]);
      	    status = MdsPut(szTemp , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);
          }

        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqpvt->mdsnode[i]);
        }
    }

    for(i=0; i < MAX_GAIN; i++) {
        sprintf(szTemp, "%f", daqGainPvt.gain[i]);
        /*sprintf(tempNode, "ECCD.ECCD1.%sP_GAIN", daqGainPvt.mdsnode[i]);*/
        sprintf(tempNode, "\\%sP_GAIN", daqGainPvt.mdsnode[i]);
        status = MdsPut(tempNode, szTemp, &_null);

        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqGainPvt.mdsnode[i]);
        }

        sprintf(szTemp, "%f", daqGainPvt.offset[i]);
        /*sprintf(tempNode, "ECCD.ECCD1.%sP_OFFSET", daqGainPvt.mdsnode[i]);*/
        sprintf(tempNode, "\\%sP_OFFSET", daqGainPvt.mdsnode[i]);
        status = MdsPut(tempNode, szTemp, &_null);

        if(!((status) & 1)) {
            printf("MdsPut Error [%d] --> node [%s]\n", i, daqGainPvt.mdsnode[i]);
        }
    }

    status = MdsClose("HEATING", &shot);
	MdsDisconnect();

    free(getData);
    getData = NULL;

    printf("--- KSTAR MDSPlus Data Put OK ---\n");

    return 0;
}

#if 1
int acqMdsPoloPutData(PoloScan *scan)
{
    int _null = 0;
    int status;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Double = DTYPE_DOUBLE;
    int i;
    int   mdsSock;
    int   shot = mdsShot;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;
    float fStartTime;
    float fEndTime;
    float fRealTimeGap;

    float *getData;

    getData = (float *)malloc(sizeof(float)*scan->sample);
    printf("--- MdsConnect() ---\n");
    mdsSock = MdsConnect("172.17.121.251:8000");

    if(mdsSock == -1) {
		free (getData);
        printf("--- MDS Connect Faile ---\n");
        return -1;
    }

    status = MdsOpen("eccd1", &shot);

    if ( !STATUS_OK(status) )
    {
		free (getData);
      fprintf(stderr,"Error opening tree for shot %d.\n",shot);
      printf("%s\n", MdsGetMsg(status));
      return -1;
    }

    fStartTime = scan->t0 * 10.0;
    fEndTime = scan->sample + fStartTime - 1.0;
    fRealTimeGap = 1.0 / 10.0;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

    for(i=0; i < scan->sample; i++)
        getData[i] = scan->mdsval[i];

    rawDataDesc = descr(&dtype_Float, getData, &scan->sample, &_null);
    status = MdsPut("EC1_POLO", "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

    status = MdsClose("eccd1", &shot);

    printf("--- MDSPlus Poloidal Put OK ---\n");

    free(getData);
    getData = NULL;

    return 0;
}
#endif
