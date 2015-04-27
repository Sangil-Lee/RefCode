#include <math.h>
#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "sfwCommon.h"
#include "sfwDriver.h"
#include "sfwMDSplus.h"

#include "myMDSplus.h"

/*
#include <dbCommon.h>	 for epicsPrintf 
#include <epicsThread.h>  for epicsThreadSleep() 
*/


#define statusOk(status) ((status) & 1)


int rmp_put_raw_value(ST_STD_device *pSTDdev)
{
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int dypte_UShort = DTYPE_USHORT ;
	int dtype_ULong = DTYPE_ULONG; 
	int dtype_Double = DTYPE_DOUBLE;
	
	int rawDataDesc; /* must int type */
	int voltageTDesc;	
	int realRate_Desc, realRate_Desc_Tx, value_at_start_Desc;
	int id_end_Desc, id_end_Desc_Tx;
//	int error_bit = 0;
	int run_time;
	

	int i, opmode, got; /*, ntemp2=0; */

	int nTrueSampleCnt=0, nTrueSampleCnt_1;
	int nTrueTxCnt, nTrueTxCnt_1;
	double dVpp20_16bit;


	FILE *fp=NULL;	
	char fname[128];
	char buffer[30];


	double dStartTime;
	double dRealTimeGap;
	double dRealTimeGapTx;
	unsigned short *databuf=NULL;
	

	ST_16aiss8ao4 *p16aiss8ao4 = (ST_16aiss8ao4 *)pSTDdev->pUser;
	if( !p16aiss8ao4) {
		PRINT_ERR();
		printf("%s: can't get ST_16aiss8ao4  pointer.\n", pSTDdev->taskName);
		return WR_ERROR;
	}

	run_time = (int)pSTDdev->ST_Base_fetch.dRunPeriod;

	opmode = pSTDdev->ST_Base.opMode;
	nTrueSampleCnt = run_time * pSTDdev->ST_Base_fetch.nSamplingRate;
	
	nTrueTxCnt = run_time * INIT_TX_CLOCK;

	if ( opmode != OPMODE_REMOTE ) {
		dStartTime = -1 * TIME_FIX_LTU_START;  /* Data Zero Start Time */
	} else {
/*		dStartTime = (pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime) * pSTDdev->ST_Base_fetch.nSamplingRate; */
		dStartTime = pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime;
	}
	dRealTimeGap = 1.0 / (double)pSTDdev->ST_Base_fetch.nSamplingRate;
	nTrueSampleCnt_1 = nTrueSampleCnt - 1;

	dRealTimeGapTx = 1.0 / INIT_TX_CLOCK;
	nTrueTxCnt_1 = nTrueTxCnt -1;
	
	dVpp20_16bit = 20.0 / 65535.0;


	realRate_Desc = descr(&dtype_Double, &dRealTimeGap, &_null);
	voltageTDesc = descr(&dtype_Double, &dVpp20_16bit, &_null);
	value_at_start_Desc = descr(&dtype_Double, &dStartTime, &_null);
	id_end_Desc = descr(&dtype_ULong, &nTrueSampleCnt_1, &_null);

	realRate_Desc_Tx= descr(&dtype_Double, &dRealTimeGapTx, &_null);
	id_end_Desc_Tx= descr(&dtype_ULong, &nTrueTxCnt_1, &_null);
	

	databuf = (unsigned short *)malloc(sizeof(unsigned short)* nTrueSampleCnt);
	if( databuf == NULL){		
		epicsPrintf("\n%s: >>> malloc(sizeof(unsigned short)* %d)... fail\n", pSTDdev->taskName, nTrueSampleCnt);
		return WR_ERROR;
	}

	

	if( mds_Connect(pSTDdev) == WR_ERROR ) {
		free(databuf);
		return WR_ERROR;
	}
	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		mds_Disconnect(pSTDdev);
		free(databuf);
		return WR_ERROR;
	}


//	for(i = 0; i<CNT_AI_CH; i++) 
	for(i = 0; i<6; i++) 
	{
		if( !(pSTDdev->channel_mask & ( 0x1 << i) ) ) {
			printf("\n%s: %d not used!\n", pSTDdev->taskName, i);
			continue;
		}
		sprintf(fname, "%s/raw%d_%d.dat", STR_CHANNEL_DATA_DIR, pSTDdev->BoardID, i);

		fp = fopen(fname, "rb");
		if( fp == NULL) {
			mds_Close_withFetch(pSTDdev);
			mds_Disconnect(pSTDdev);
			printf("can't open data file : %s\n", fname);
			free(databuf);
			return(WR_ERROR);
		}
		got = fread( databuf, 2, nTrueSampleCnt, fp);
//		printf("%s: Ch%d, got:%d, trueCnt:%d\n", pSTDdev->taskName, i, got, nTrueSampleCnt );
		fclose(fp);
		if( got != nTrueSampleCnt )
		{
			printf("%s:ERROR! Ch%d, got:%d, trueCnt:%d\n", pSTDdev->taskName, i, got, nTrueSampleCnt );
//			error_bit = 1;
			continue;
		}

		rawDataDesc = descr(&dypte_UShort, databuf, &nTrueSampleCnt, &_null);

		status = MdsPut(p16aiss8ao4->strTagName[i] ,
			"BUILD_SIGNAL(BUILD_WITH_UNITS($*$-10,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
			&rawDataDesc, &voltageTDesc,  &id_end_Desc, &value_at_start_Desc, &realRate_Desc, &_null);

		if ( !((status) & 1) )
		{
			printf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, p16aiss8ao4->strTagName[i], i, MdsGetMsg(status));
			printlog("%s: Error tag\"%s\", ch%d, shot %d: %s\n",pSTDdev->taskName, p16aiss8ao4->strTagName[i], i, pSTDdev->ST_Base_fetch.shotNumber, MdsGetMsg(status));
		} 

		
	}

	
//	for(i = 0; i<CNT_AO_CH; i++) 
	for(i = 0; i<3; i++) 
	{

//		if( error_bit ) break;
		
		drv16aiss8ao4_copy_data(i, nTrueTxCnt, databuf );
			
		rawDataDesc = descr(&dypte_UShort, databuf, &nTrueTxCnt, &_null);

		if( p16aiss8ao4->u8_PCS_control) 
		{
			status = MdsPut(p16aiss8ao4->strTagCmd[i] ,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"KA\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&rawDataDesc, &id_end_Desc_Tx, &value_at_start_Desc, &realRate_Desc_Tx, &_null);
		} 
		else {
			status = MdsPut(p16aiss8ao4->strTagCmd[i] ,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$-10,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&rawDataDesc, &voltageTDesc,  &id_end_Desc_Tx, &value_at_start_Desc, &realRate_Desc_Tx, &_null);
		}

		if ( !((status) & 1) )
		{
			printf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, p16aiss8ao4->strTagCmd[i], i, MdsGetMsg(status));
			printlog("%s: Error tag\"%s\", ch%d, shot %d: %s\n",pSTDdev->taskName, p16aiss8ao4->strTagCmd[i], i, pSTDdev->ST_Base_fetch.shotNumber, MdsGetMsg(status));
		} 
	}

	free(databuf);

	sprintf(buffer, "%d", p16aiss8ao4->u32Mode_tag[ID_PS_BOTTOM]);
	status = MdsPut("\\RMPB_CONFIG", buffer, &_null);
	if ( !((status) & 1) )
	{
		printf("%s: Error tag\"\\RMPB_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
		printlog("%s: Error tag\"\\RMPB_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
	}
	sprintf(buffer, "%d", p16aiss8ao4->u32Mode_tag[ID_PS_MIDDLE]);
	status = MdsPut("\\RMPM_CONFIG", buffer, &_null);
	if ( !((status) & 1) )
	{
		printf("%s: Error tag\"\\RMPM_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
		printlog("%s: Error tag\"\\RMPM_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
	}
	sprintf(buffer, "%d", p16aiss8ao4->u32Mode_tag[ID_PS_TOP]);
	status = MdsPut("\\RMPT_CONFIG", buffer, &_null);
	if ( !((status) & 1) )
	{
		printf("%s: Error tag\"\\RMPT_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
		printlog("%s: Error tag\"\\RMPT_CONFIG\": %s\n",pSTDdev->taskName, MdsGetMsg(status));
	}

	
	
	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);

	return WR_OK;
}




