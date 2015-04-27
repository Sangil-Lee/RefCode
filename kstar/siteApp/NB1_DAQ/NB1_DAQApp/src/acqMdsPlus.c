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

#include "epicsThread.h"

#include "Acq196Dt100.h"
#include "drvAcq196.h"

#include "tc_save.h"

#define STATUS_OK(status) (((status) & 1) == 1)

int mds_createNewShot(int shot)
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
        int tstat, len;
        char buf[128];
        int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an      integer */
        int idesc = descr(&dtype_Long, &tstat, &null);

                sprintf(buf, "TCL(\"set tree nbi1/shot=-1\")");
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

        return 0;
}

int acqMdsPutData(Acq196Drv *acqDrv, int mode)
{
    int _null = 0;
    int status;
    int dtype_Double = DTYPE_DOUBLE;
    int dtype_Short = DTYPE_SHORT;
    int dtype_ULong = DTYPE_ULONG;

    short int16TempPack[96];

    int rawDataDesc;
    int rateDesc, startDesc, endDesc, voltageTDesc;
    int   id_end_Desc;

    int i, j;

    FILE *fpRaw = NULL;
    int  nTrueSampleCnt = 0;
    int  nTrueSampleCnt_1;
    double fVpp10_16bit;

    double fStartTime;
    double fEndTime;
    double fRealTimeGap;

    int   mdsSock;
    int   shot = acqDrv->shot;

    long   totalCnt;

    FILE *fp[96];
    char szTemp[256];
    short *getData = NULL;

    int   reConn = 0;
	int   slot = acqDrv->slot;
	int   card = acqDrv->card;

	float  blipTime = -16;

	char  tempNode[200];

    totalCnt = (int)(acqDrv->totalNum/(96 * 2));

    nTrueSampleCnt = (acqDrv->t1 - acqDrv->t0) * acqDrv->clock;

    printf("--- Total Count : %d, %d---\n", acqDrv->totalNum, totalCnt);
    printf("--- Sample Count : %d ---\n", nTrueSampleCnt);

    getData = (short *)malloc(sizeof(short)*nTrueSampleCnt);

    if(getData == NULL) {
        printf("--- malloc error ---\n");
        return -1;
    }

    printf("--- MdsConnect() ---\n");
    while(1) {
		if(mode == KSTAR_MDS) {
            mdsSock = MdsConnect("172.17.100.202:8000");
		}
		else if(mode == LOCAL_MDS) {
            mdsSock = MdsConnect("172.17.121.244:8000");
		}

        if(mdsSock == -1) {
            printf("--- MDS Connect Faile Reconnect [%d] ---\n", reConn);
            if(reConn >= 3) return -1;
            else reConn++;
        }
        else {
            break;
        }
    }

    reConn = 0;

    while(1) {
		if(mode == KSTAR_MDS)
			status = MdsOpen("HEATING", &shot);
		else if(mode == LOCAL_MDS)
            status = MdsOpen("nbi1", &shot);

        if ( !STATUS_OK(status) )
        {
            epicsThreadSleep(0.1);
            fprintf(stderr,"Error opening tree for shot %d.\n",shot);
            printf("%s\n", MdsGetMsg(status));
            if(reConn >= 3) return -1;
            else reConn++;
        }
        else {
            break;
        }
    }

	if(mode == KSTAR_MDS) {
	    fStartTime = (acqDrv->t0 - acqDrv->BlipTime) * acqDrv->clock;
        fEndTime = nTrueSampleCnt + fStartTime - 1.0;
	}
	else {
        fStartTime = acqDrv->t0;
        fEndTime = nTrueSampleCnt + fStartTime;
	}

    fRealTimeGap = 1.0 / (double)acqDrv->clock;
    fVpp10_16bit = 10.0 / 32768.0;
    nTrueSampleCnt_1 = nTrueSampleCnt - 1;

    rateDesc = descr(&dtype_Double, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Double, &fStartTime, &_null);
    endDesc = descr(&dtype_Double, &fEndTime, &_null);
    voltageTDesc = descr(&dtype_Double, &fVpp10_16bit, &_null);
    id_end_Desc = descr(&dtype_ULong, &nTrueSampleCnt_1, &_null);

	sprintf(szTemp, "/media/data1/acq196.%d.dat", slot);
    fpRaw = fopen(szTemp, "rb");
    if(fpRaw == NULL) {
		printf("file open error [%d]\n", slot);
        MdsClose("nbi1", (int *)shot);
        MdsDisconnect(mdsSock);
        free(getData);
        return 0;
    }

	sprintf(szTemp, "rm -f /tmp/data_%d_*_dat", slot);
    system(szTemp);

    for(i=0; i<96; i++) {
        sprintf(szTemp,"/tmp/data_%d_%d_dat", slot, i+1);
        fp[i] = fopen(szTemp, "wb");
        
        if(fp[i] == NULL) printf("Cannot open file : [%d]\n", i+1);
    }

    for (i=0; i < totalCnt-1; i++)
    {
        /* read one bulk data  */
        fread( &int16TempPack, 192, 1, fpRaw );
        for( j=0; j< 96; j++)
        {
            fwrite( &int16TempPack[j], 2 , 1, fp[acqDrv->channelMapping[j]] );
        } /* for( j=0; j< ch; j++)  */
    }

    printf("--- totalCnt [%d] : %d, i : %d ---\n", slot, totalCnt, i);

    for(i=0; i<96; i++) {
        fclose(fp[i]);
    }

    fclose(fpRaw);

    printf("--- MdsPut [%d] ---\n", slot);
    for(i=0; i<96; i++) {
        sprintf(szTemp,"/tmp/data_%d_%d_dat", slot, i+1);
        fp[0] = fopen(szTemp, "rb");
        fread( getData, 2, nTrueSampleCnt, fp[0]);
        fclose(fp[0]);

        rawDataDesc = descr(&dtype_Short, getData, &nTrueSampleCnt, &_null);

        if(dtacqDrv[card][i].mdsput != 0) {
			if(mode == KSTAR_MDS) {
				sprintf(tempNode, "\\%s:FOO", dtacqDrv[card][i].node_name);
         	    status = MdsPut(tempNode, 
					"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					&rawDataDesc, &voltageTDesc,  &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);
			}
			else if(mode == LOCAL_MDS) {
				sprintf(tempNode, "\\%s:FOO", dtacqDrv[card][i].node_name);
                status = MdsPut(tempNode ,
				    "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				    &rawDataDesc, &voltageTDesc,  &id_end_Desc, &startDesc, &rateDesc, &_null);
			}

            if(!((status) & 1)) {
                printf("MdsPut Error [%d] --> node [%s]\n", i, dtacqDrv[card][i].node_name);
            }

			if(dtacqDrv[card][i].mdsput == 2) {
                sprintf(szTemp, "%f", dtacqDrv[card][i].gain);
				sprintf(tempNode, "\\%s:P_GAIN", dtacqDrv[card][i].node_name);
				status = MdsPut(tempNode, szTemp, &_null);

                if(!((status) & 1)) {
                    printf("MdsPut Gain Error [%d] --> node [%s]\n", i, dtacqDrv[card][i].node_name);
                }

                sprintf(szTemp, "%f", dtacqDrv[card][i].offset);
				sprintf(tempNode, "\\%s:P_OFFSET", dtacqDrv[card][i].node_name);
				status = MdsPut(tempNode, szTemp, &_null);

                if(!((status) & 1)) {
                    printf("MdsPut Gain Error [%d] --> node [%s]\n", i, dtacqDrv[card][i].node_name);
                }
			}
        }
    }

    sprintf(szTemp, "%f", nbiPNB_A[0]);
	sprintf(tempNode, "\\NB11_PNB:P_GAIN");
	status = MdsPut(tempNode, szTemp, &_null);

    if(!((status) & 1)) {
        printf("MdsPut Gain Error NB11_PNB\n");
    }

    sprintf(szTemp, "%f", nbiPNB_A[1]);
	sprintf(tempNode, "\\NB12_PNB:P_GAIN");
	status = MdsPut(tempNode, szTemp, &_null);

    if(!((status) & 1)) {
        printf("MdsPut Gain Error NB12_PNB\n");
    }

    sprintf(szTemp, "%f", nbiPNB_B[0]);
	sprintf(tempNode, "\\NB11_PNB:P_OFFSET");
	status = MdsPut(tempNode, szTemp, &_null);

    if(!((status) & 1)) {
        printf("MdsPut OFFSET Error NB11_PNB\n");
	}

    sprintf(szTemp, "%f", nbiPNB_B[1]);
	sprintf(tempNode, "\\NB12_PNB:P_OFFSET");
	status = MdsPut(tempNode, szTemp, &_null);

    if(!((status) & 1)) {
        printf("MdsPut OFFSET Error NB11_PNB\n");
	}

	if(mode == KSTAR_MDS)
        status = MdsClose("HEATING", &shot);
	else if(mode == LOCAL_MDS)
		status = MdsClose("nbi1", &shot);
    MdsDisconnect(mdsSock);

    free(getData);
    getData = NULL;

    printf("--- MDSPlus Data Put OK ---\n");

    return 0;
}
