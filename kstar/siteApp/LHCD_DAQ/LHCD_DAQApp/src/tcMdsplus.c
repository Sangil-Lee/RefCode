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

#include "tc_save.h"

#define STATUS_OK(status) (((status) & 1) == 1)

int acqMdsPutTCData(TCMDS_INFO *tcmds, int mode)
{
    int _null = 0;
    int status = 0;
    int dtype_Double = DTYPE_DOUBLE;
    int dtype_Short = DTYPE_SHORT;
    int dtype_ULong = DTYPE_ULONG;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;
    int   id_end_Desc;

    int i, j, k;
	int slot;

    int  nTrueSampleCnt = 0;
    int  nTrueSampleCnt_1 = 0;

    double fStartTime;
    double fEndTime;
    double fRealTimeGap;

    int   mdsSock;
    int   shot = tcmds->shot;

    long   totalCnt;

    FILE *fp;
    char szTemp[256];

    int   reConn = 0;
	char  tempNode[200];

	double *getData = NULL;

    nTrueSampleCnt = (tcmds->t1 - tcmds->t0) * 10;

	getData = (double *)malloc(sizeof(double)*nTrueSampleCnt);

    while(1) {
		if(mode == 1) { /* KSTAR MDS */
            mdsSock = MdsConnect("172.17.100.202:8000");
		}
		else if(mode == 0) { /* LOCAL MDS */
            mdsSock = MdsConnect("172.17.121.218:8000");
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
		if(mode == 1) /* KSTAR MDS */
			status = MdsOpen("HEATING", &shot);
		else if(mode == 0) /* LOCAL MDS */
            status = MdsOpen("lhcd1", &shot);

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

    fStartTime = tcmds->t0;
    fEndTime = nTrueSampleCnt + fStartTime;

    fRealTimeGap = 1.0 / 10.0;
    nTrueSampleCnt_1 = nTrueSampleCnt - 1;

    rateDesc = descr(&dtype_Double, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Double, &fStartTime, &_null);
    endDesc = descr(&dtype_Double, &fEndTime, &_null);
    id_end_Desc = descr(&dtype_ULong, &nTrueSampleCnt_1, &_null);


	double read_data[nTrueSampleCnt*MAX_CARD_NUM];
	double raw_data[TC_MAX_CH][nTrueSampleCnt];

    for(slot=0; slot < USE_CARD_NUM; slot++) {
		int ind = 0;
		int inc = 0;

		sprintf(szTemp, "/tmp/tc_%d_%d.dat", tcmds->card, slot);
		fp = fopen(szTemp, "r");
		fread(&read_data[0],  sizeof(double)*tcmds->t1*MAX_CARD_NUM*8, 1, fp);
		fclose(fp);
		for(i=0; i < tcmds->t1; i++) {
			for(j=0; j < TC_MAX_CH; j++) {
				for(k=0; k < 10; k++) {
					raw_data[j][k+ind] = read_data[k+inc];
				}
				inc += 10;
			}
			ind += 10;
		}

		for(i=0; i < TC_MAX_CH; i++) {
			printf("--- TC MDS PUT, [%s], [%d]\n", tcmds->mdsinfo[slot][i].node, tcmds->mdsinfo[slot][i].mdsput);
			memcpy(getData, &raw_data[i], sizeof(double)*nTrueSampleCnt);

	        rawDataDesc = descr(&dtype_Double, getData, &nTrueSampleCnt, &_null);

	        if(tcmds->mdsinfo[slot][i].mdsput != 0) {
				sprintf(tempNode, "\\%s", tcmds->mdsinfo[slot][i].node);

				if(mode == 1) { /* KSTAR MDS */
	         	    status = MdsPut(tempNode, 
						"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
						&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);
				}
				else if(mode == 0) { /* LOCAL MDS */
			        status = MdsPut(tempNode ,
					    "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					    &rawDataDesc, &id_end_Desc, &startDesc, &rateDesc, &_null);
				}

		        if(!((status) & 1)) {
	                printf("MdsPut Error --> node [%s]\n", tcmds->mdsinfo[slot][i].node);
	            }
	        }
		}
		printf("-- TC PUT CARD NUM : %d --\n", slot+1);
    }

	if(mode == 1) /* KSTAR MDS */
        status = MdsClose("HEATING", &shot);
	else if(mode == 0) /* LOCAL MDS */
		status = MdsClose("lhcd1", &shot);

    MdsDisconnect(mdsSock);

    free(getData);
    getData = NULL;

    printf("--- MDSPlus Data Put OK ---\n");

    return 0;
}
