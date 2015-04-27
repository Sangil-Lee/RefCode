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

int mds_createNewShot(int shot, char *tree)
{
    int null = 0; /* Used to mark the end of the argument list */
    int status; /* Will contain the status of the data access operation */
    int tstat, len;
    char buf[128];
    int dtype_Long = DTYPE_LONG;
    int idesc = descr(&dtype_Long, &tstat, &null);

    sprintf(buf, "TCL(\"set tree %s/shot=-1\")", tree);
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

int acqMdsPutData(DaqAiPvt *daqpvt, int mode)
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

    char  szFile[256];

#if 0
    float64 getData[nNumSampleCnt];
#else
    float64 *getData;

	printf("*** MDSPlus #!!!!!!.... ***\n");

    getData = (float64 *)malloc(sizeof(float64)*nNumSampleCnt);
#endif

	printf("*** MDSPlus Server Connection.... ***\n");

	if(mode == KSTAR_MDS) {
		mdsSock = MdsConnect("172.17.100.202:8000");
	}
	else if(mode == LOCAL_MDS) {
		mdsSock = MdsConnect("172.17.121.218:8000");
	}

    if(mdsSock == -1) {
		free (getData);

        printf("--- MDS Connect Faile ---\n");
        return -1;
    }

    printf("--- MdsOpen() ---\n");
	if(mode == KSTAR_MDS) {
		status = MdsOpen("HEATING", &shot);
	}
	else if(mode == LOCAL_MDS) {
		status = MdsOpen("lhcd1", &shot);
	}

    if ( !STATUS_OK(status) )
    {
        fprintf(stderr,"Error opening tree for shot %d.\n",shot);
        printf("%s\n", MdsGetMsg(status));
        return -1;
    }

	if(mode == KSTAR_MDS) {
		fStartTime = (daqpvt->t0 - 16.0) * daqpvt->rate;
		fEndTime = daqpvt->sampsPerChan + fStartTime - 1.0;
	}
	else if(mode == LOCAL_MDS) {
		fStartTime = daqpvt->t0 * daqpvt->rate;
		fEndTime = daqpvt->sampsPerChan + fStartTime - 1.0;
	}

    fRealTimeGap = 1.0 / (float64)daqpvt->rate;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

    printf("--- MdsPut ---\n");

    for(i=0; i < 16; i++) {

        if(daqpvt->mdsput[i] == 1) {

            if(daqpvt->fp[i] != NULL) {
                daqpvt->fp[i] = NULL;
            }

            sprintf(szFile, "/tmp/%s_%d.dat", daqpvt->fileName, i);
            daqpvt->fp[i] = fopen(szFile, "r");

			if (NULL == daqpvt->fp[i]) {
				continue;
			}

            if(1 != fread (&getData[0], sizeof(float64)*nNumSampleCnt, 1, daqpvt->fp[i])) {
				fclose(daqpvt->fp[i]);
				daqpvt->fp[i] = NULL;

                printf("--- file read error ---\n");
                continue;
            }

            fclose(daqpvt->fp[i]);
            daqpvt->fp[i] = NULL;

            rawDataDesc = descr(&dtype_Double, getData, &nNumSampleCnt, &_null);

            printf("MdsPut : Ch %d\n", i);
            sprintf(tempNode, "\\%s:FOO", daqpvt->mdsnode[i]);
            status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

            if(!((status) & 1)) {
                printf("MdsPut Error [%d] --> node [%s]\n", i, daqpvt->mdsnode[i]);
            }

            printf("MdsPut Ok [%s]\n", daqpvt->mdsnode[i]);

            if(daqpvt->int_mode[i] == 1) {
                DeltaT_Integral(daqpvt, getData, i);
            }

            if(daqpvt->gain_mode[i] == 1) {
                sprintf(szTemp, "%f", daqpvt->gain[i]);
                sprintf(tempNode, "\\%s:P_GAIN", daqpvt->mdsnode[i]);
                status = MdsPut(tempNode, szTemp, &_null);

                if(!((status) & 1)) {
                    printf("MdsPut Error [%d] --> node [%s:P_GAIN]\n", i, daqpvt->mdsnode[i]);
                }
            }

            if(daqpvt->offset_mode[i] == 1) {
                sprintf(szTemp, "%f", daqpvt->offset[i]);
                sprintf(tempNode, "\\%s:P_OFFSET", daqpvt->mdsnode[i]);
                status = MdsPut(tempNode, szTemp, &_null);

                if(!((status) & 1)) {
                    printf("MdsPut Error [%d] --> node [%s:P_OFFSET]\n", i, daqpvt->mdsnode[i]);
                }
            }
        }
    }

    free(getData);
    getData = NULL;

	if(mode == KSTAR_MDS)
		status = MdsClose("HEATING", &shot);
	else if(mode == LOCAL_MDS)
		status = MdsClose("lhcd1", &shot);
		    
	MdsDisconnect(mdsSock);

    printf("--- MDSPlus Data Put OK ---\n");

    return 0;
}

int rfMdsPutData(RFCTRL_INFO *lhcd_info, int mode)
{
    int _null = 0;
    int status;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Double = DTYPE_DOUBLE;
	int dtype_Short = DTYPE_SHORT;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;

    int i, j;
    int nIndex = 0;
    int nNumSampleCnt = lhcd_info->data_count;

    float fStartTime;
    float fEndTime;
    float fRealTimeGap;

    int   mdsSock;
    int   shot = mdsShot;
    char  tempNode[256];
    char  szTemp[256];

    uint16 *getData;
#if 0
	uint16 *calcPhase_i;
	uint16 *calcPhase_q;
	uint16 *putPhase;
#else
	/*
	float calcPhase_i[nNumSampleCnt];
	float calcPhase_q[nNumSampleCnt];
	*/
	float *calcPhase_i;
	float *calcPhase_q;
	float *putPhase;
#endif

    getData = (uint16 *)malloc(sizeof(uint16)*nNumSampleCnt);
#if 0
    calcPhase_i = (uint16 *)malloc(sizeof(uint16)*nNumSampleCnt);
    calcPhase_q = (uint16 *)malloc(sizeof(uint16)*nNumSampleCnt);
    putPhase = (uint16 *)malloc(sizeof(uint16)*nNumSampleCnt);
#endif
    putPhase = (float *)malloc(sizeof(float)*nNumSampleCnt);
    calcPhase_i = (float *)malloc(sizeof(float)*nNumSampleCnt);
    calcPhase_q = (float *)malloc(sizeof(float)*nNumSampleCnt);

	printf("*** RF MDSPlus Server Connection.... ***\n");

	if(mode == KSTAR_MDS) {
	    mdsSock = MdsConnect("172.17.100.202:8000");
    }
    else if(mode == LOCAL_MDS) {
		mdsSock = MdsConnect("172.17.121.218:8000");
	}

	if(mdsSock == -1) {
		if (getData)		free (getData);
		if (putPhase)		free (putPhase);
		if (calcPhase_i)	free (calcPhase_i);
		if (calcPhase_q)	free (calcPhase_q);

		printf("--- MDS Connect Faile ---\n");
		return -1;
	}

	printf("--- MdsOpen() ---\n");
	if(mode == KSTAR_MDS) {
		status = MdsOpen("HEATING", &shot);
	}
	else if(mode == LOCAL_MDS) {
		status = MdsOpen("lhcd1", &shot);
	}

    if ( !STATUS_OK(status) )
    {
		if (getData)		free (getData);
		if (putPhase)		free (putPhase);
		if (calcPhase_i)	free (calcPhase_i);
		if (calcPhase_q)	free (calcPhase_q);

        fprintf(stderr,"Error opening tree for shot %d.\n",shot);
        printf("%s\n", MdsGetMsg(status));
        return -1;
    }

	if(mode == KSTAR_MDS) {
		fStartTime = (lhcd_info->t0 - 16.0) * lhcd_info->samplclk;
		fEndTime = (float)(nNumSampleCnt + fStartTime - 1.0);
	}
	else if(mode == LOCAL_MDS) {
		fStartTime = (lhcd_info->t0 * lhcd_info->samplclk);
		fEndTime = (float)(nNumSampleCnt + fStartTime - 1.0);
	}

	printf("-- RF CTRL Start Time : %f \n", fStartTime);

    fRealTimeGap = 1.0 / lhcd_info->samplclk;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

	for(i=0; i < 4; i++) {
		if (NULL == lhcd_info->fp[i]) {
			continue;
		}

		if(1 != fread(&getData[0], sizeof(uint16)*nNumSampleCnt, 1, lhcd_info->fp[i])) {
			printf("--- file read error ---\n");
		}

// Remove TG 2012.10.03	    fclose(lhcd_info->fp[i]);
// Remove TG 2012.10.03		lhcd_info->fp[i] = NULL;

	    rawDataDesc = descr(&dtype_Short, getData, &nNumSampleCnt, &_null);

		if(i == 0) {
			printf("MdsPut : RF FWD PUT\n");
			sprintf(tempNode, "\\LH1_KLYRAW:FWD_RAW");
		}
		else if(i == 1) {
			printf("MdsPut : RF REV PUT\n");
			sprintf(tempNode, "\\LH1_KLYRAW:REV_RAW");
		}
		else if(i == 2) {
			printf("MdsPut : RF IQ_I PUT\n");
			sprintf(tempNode, "\\LH1_KLYRAW:IQ_I");
			/* Phase I Calc */
			for(j=0; j < nNumSampleCnt; j++) {
				calcPhase_i[j] = (getData[j] - lhcd_info->iq_i_offset) * lhcd_info->phase_offset;
				if(calcPhase_i[j] == 0) calcPhase_i[j] = 0.001;
			}
		}
		else if(i == 3) {
			printf("MdsPut : RF IQ_Q PUT\n");
			sprintf(tempNode, "\\LH1_KLYRAW:IQ_Q");
			/* Phase Q Calc */
			for(j=0; j < nNumSampleCnt; j++) {
				calcPhase_q[j] = (getData[j] - lhcd_info->iq_q_offset);
			}
		}

	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

	    if(!((status) & 1)) {
		    printf("MdsPut Error [%d]\n", i);
	    }

	}

	printf("RF : IQ_I OFFSET [%f]\n", lhcd_info->iq_i_offset);
	printf("RF : IQ_Q OFFSET [%f]\n", lhcd_info->iq_q_offset);
	printf("RF : PH   OFFSET [%f]\n", lhcd_info->phase_offset);

	for(j=0; j < nNumSampleCnt; j++) {
#if 0
		printf("IQ_I PH : [%f], IQ_Q PH : [%f]\n", calcPhase_i[j], calcPhase_q[j]);
#endif
		putPhase[j] = atan(calcPhase_q[j]/calcPhase_i[j]) * 57.29578;
		if(calcPhase_i[j] < 0) putPhase[j] = putPhase[j] + 180;
		else if(calcPhase_q[j] < 0) putPhase[j] = putPhase[j] + 360;
	}

	rawDataDesc = descr(&dtype_Float, putPhase, &nNumSampleCnt, &_null);
    sprintf(tempNode, "\\LH1_KPH:FOO");
	status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
						&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);


	sprintf(tempNode, "\\LH1_KLYRAW:FWD_1ST");
    sprintf(szTemp, "%f", lhcd_info->fwd_1st);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:FWD_2ND");
    sprintf(szTemp, "%f", lhcd_info->fwd_2st);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:FWD_ITS");
    sprintf(szTemp, "%f", lhcd_info->fwd_its);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:FWD_OFS");
    sprintf(szTemp, "%f", lhcd_info->fwd_ofs);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:FWD_LOSS");
    sprintf(szTemp, "%f", powf(10,(lhcd_info->fwd_loss/10)));
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:REV_1ST");
    sprintf(szTemp, "%f", lhcd_info->rev_1st);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:REV_2ND");
    sprintf(szTemp, "%f", lhcd_info->rev_2st);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:REV_ITS");
    sprintf(szTemp, "%f", lhcd_info->rev_its);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:REV_OFS");
    sprintf(szTemp, "%f", lhcd_info->rev_ofs);
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:REV_LOSS");
    sprintf(szTemp, "%f", powf(10,(lhcd_info->rev_loss/10)));
    status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:PH_OFS");
	sprintf(szTemp, "%f", lhcd_info->phase_offset);
	status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:IQ_I_OFS");
	sprintf(szTemp, "%f", lhcd_info->iq_i_offset);
	status = MdsPut(tempNode, szTemp, &_null);

	sprintf(tempNode, "\\LH1_KLYRAW:IQ_Q_OFS");
	sprintf(szTemp, "%f", lhcd_info->iq_q_offset);
	status = MdsPut(tempNode, szTemp, &_null);



    free(getData);
#if 1
	free(calcPhase_i);
	free(calcPhase_q);
#endif
	free(putPhase);
    getData = NULL;


	if(mode == KSTAR_MDS)
		status = MdsClose("HEATING", &shot);
	else if(mode == LOCAL_MDS)
		status = MdsClose("lhcd1", &shot);
		    
	MdsDisconnect(mdsSock);

    printf("--- RF MDSPlus Data Put OK ---\n");

    return 0;
}

int rfantMdsPutData(RFANT_INFO *lhcd_info, int mode)
{
    int _null = 0;
    int status;
    int dtype_Float = DTYPE_FLOAT;
    int dtype_Double = DTYPE_DOUBLE;
	int dtype_Short = DTYPE_SHORT;

    int rawDataDesc;
    int rateDesc, startDesc, endDesc;

    int i, j;
    int nIndex = 0;
    int nNumSampleCnt = lhcd_info->data_count;

    float fStartTime;
    float fEndTime;
    float fRealTimeGap;

    int   mdsSock;
    int   shot = mdsShot;
    char  tempNode[256];
    char  szTemp[256];

    char  szFile[256];

    uint16 *getData;

    getData = (uint16 *)malloc(sizeof(uint16)*nNumSampleCnt);

	printf("*** RF ANT MDSPlus Server Connection.... ***\n");

	if(mode == KSTAR_MDS) {
	    mdsSock = MdsConnect("172.17.100.202:8000");
    }
    else if(mode == LOCAL_MDS) {
		mdsSock = MdsConnect("172.17.121.218:8000");
	}

	if(mdsSock == -1) {
		if (getData)	free (getData);

		printf("--- MDS Connect Faile ---\n");
		return -1;
	}

	printf("--- MdsOpen() ---\n");
	if(mode == KSTAR_MDS) {
		status = MdsOpen("HEATING", &shot);
	}
	else if(mode == LOCAL_MDS) {
		status = MdsOpen("lhcd1", &shot);
	}

    if ( !STATUS_OK(status) )
    {
		if (getData)	free (getData);

        fprintf(stderr,"Error opening tree for shot %d.\n",shot);
        printf("%s\n", MdsGetMsg(status));
        return -1;
    }

	if(mode == KSTAR_MDS) {
		fStartTime = (lhcd_info->t0 - 16.0) * lhcd_info->samplclk;
		fEndTime = nNumSampleCnt + fStartTime - 1.0;
	}
	else if(mode == LOCAL_MDS) {
		fStartTime = (lhcd_info->t0 * lhcd_info->samplclk);
		fEndTime = nNumSampleCnt + fStartTime - 1.0;
	}

	printf("-- RF ANT Start Time : %f \n", fStartTime);

    fRealTimeGap = 1.0 / lhcd_info->samplclk;

    rateDesc = descr(&dtype_Float, &fRealTimeGap, &_null);
    startDesc = descr(&dtype_Float, &fStartTime, &_null);
    endDesc = descr(&dtype_Float, &fEndTime, &_null);

	/* FWD Data MDSPlus PUT */
	for(i=0; i < 8; i++) {
		if (NULL == lhcd_info->fwd_fp[i]) {
			continue;
		}

		if(1 != fread(&getData[0], sizeof(uint16)*nNumSampleCnt, 1, lhcd_info->fwd_fp[i])) {
			printf("--- RF ANT file read error ---\n");
		}

		//fclose(lhcd_info->fwd_fp[i]);
		//lhcd_info->fwd_fp[i] = NULL;

		rawDataDesc = descr(&dtype_Short, getData, &nNumSampleCnt, &_null);

		sprintf(tempNode, "\\LH1_A_FWD%d:FOO", i+1);

	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

	    if(!((status) & 1)) {
		    printf("MdsPut Error [%s]\n", tempNode);
	    }

		sprintf(tempNode, "\\LH1_A_FWD%d:COFF_1ST", i+1);
		sprintf(szTemp, "%f", lhcd_info->fwd_1st[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_FWD%d:COFF_2ND", i+1);
		sprintf(szTemp, "%f", lhcd_info->fwd_2st[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_FWD%d:ITS", i+1);
		sprintf(szTemp, "%f", lhcd_info->fwd_its[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_FWD%d:OFS_GAIN", i+1);
		sprintf(szTemp, "%f", lhcd_info->fwd_ofs[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_FWD%d:LOSS", i+1);
		sprintf(szTemp, "%f", powf(10, (lhcd_info->fwd_loss[i]/10)));
		status = MdsPut(tempNode, szTemp, &_null);
	}
	/* REV Data MDSPlus PUT */
	for(i=0; i < 8; i++) {
		if (NULL == lhcd_info->rev_fp[i]) {
			continue;
		}

		if(1 != fread(&getData[0], sizeof(uint16)*nNumSampleCnt, 1, lhcd_info->rev_fp[i])) {
			printf("--- RF ANT file read error ---\n");
		}

		//fclose(lhcd_info->rev_fp[i]);
		//lhcd_info->rev_fp[i] = NULL;

		rawDataDesc = descr(&dtype_Short, getData, &nNumSampleCnt, &_null);

		sprintf(tempNode, "\\LH1_A_REV%d:FOO", i+1);

	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

	    if(!((status) & 1)) {
		    printf("MdsPut Error [%s]\n", tempNode);
	    }

		sprintf(tempNode, "\\LH1_A_REV%d:COFF_1ST", i+1);
		sprintf(szTemp, "%f", lhcd_info->rev_1st[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_REV%d:COFF_2ND", i+1);
		sprintf(szTemp, "%f", lhcd_info->rev_2st[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_REV%d:ITS", i+1);
		sprintf(szTemp, "%f", lhcd_info->rev_its[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_REV%d:OFS_GAIN", i+1);
		sprintf(szTemp, "%f", lhcd_info->rev_ofs[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_REV%d:LOSS", i+1);
		sprintf(szTemp, "%f", powf(10, (lhcd_info->rev_loss[i]/10)));
		status = MdsPut(tempNode, szTemp, &_null);

	}
#if 0  /* IQ - old */
	/* IQ_I Parameter PUT */
	for(i=0; i < 7; i++) {
		if (NULL == lhcd_info->iq_i_fp[i]) {
			continue;
		}

		if(1 != fread(&getData[0], sizeof(uint16)*nNumSampleCnt, 1, lhcd_info->iq_i_fp[i])) {
			printf("--- RF ANT file read error ---\n");
		}

		//fclose(lhcd_info->iq_i_fp[i]);
		//lhcd_info->iq_i_fp[i] = NULL;

		rawDataDesc = descr(&dtype_Short, getData, &nNumSampleCnt, &_null);

		sprintf(tempNode, "\\LH1_A_IQI%d:FOO", i+1);

	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

	    if(!((status) & 1)) {
		    printf("MdsPut Error [%s]\n", tempNode);
	    }

		sprintf(tempNode, "\\LH1_A_IQI%d:I_OFFSET", i+1);
		sprintf(szTemp, "%f", lhcd_info->iq_i_offset[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_IQI%d:GAIN", i+1);
		sprintf(szTemp, "%f", lhcd_info->iq_gain[i]);
		status = MdsPut(tempNode, szTemp, &_null);
	}
	/* IQ_Q Parameter PUT */
	for(i=0; i < 7; i++) {
		if (NULL == lhcd_info->iq_q_fp[i]) {
			continue;
		}

		if(1 != fread(&getData[0], sizeof(uint16)*nNumSampleCnt, 1, lhcd_info->iq_q_fp[i])) {
			printf("--- RF ANT file read error ---\n");
		}

		//fclose(lhcd_info->iq_q_fp[i]);
		//lhcd_info->iq_q_fp[i] = NULL;

		rawDataDesc = descr(&dtype_Short, getData, &nNumSampleCnt, &_null);

		sprintf(tempNode, "\\LH1_A_IQQ%d:FOO", i+1);

	    status = MdsPut(tempNode , "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
							&rawDataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &_null);

	    if(!((status) & 1)) {
		    printf("MdsPut Error [%s]\n", tempNode);
	    }

		sprintf(tempNode, "\\LH1_A_IQQ%d:Q_OFFSET", i+1);
		sprintf(szTemp, "%f", lhcd_info->iq_q_offset[i]);
		status = MdsPut(tempNode, szTemp, &_null);

		sprintf(tempNode, "\\LH1_A_IQQ%d:GAIN", i+1);
		sprintf(szTemp, "%f", lhcd_info->iq_gain[i]);
		status = MdsPut(tempNode, szTemp, &_null);
	}
#endif

    free(getData);
    getData = NULL;

	if(mode == KSTAR_MDS)
		status = MdsClose("HEATING", &shot);
	else if(mode == LOCAL_MDS)
		status = MdsClose("lhcd1", &shot);
		    
	MdsDisconnect(mdsSock);

    printf("--- RF MDSPlus Data Put OK ---\n");

    return 0;
}
