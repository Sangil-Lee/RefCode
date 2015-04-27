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

int ts_put_calib_Iped_value(ST_STD_device *pSTDdev)
{
#if 0
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int dtype_Float = DTYPE_FLOAT; /* We are going to read a signal made of float values */
	int dypte_Short = DTYPE_SHORT ;
	
	int rawDataDesc; /* Signal descriptor */
	int timeDesc;

	int i,j,k, op; /*, ntemp2=0; */

	int nTrueSampleCnt=0;
	unsigned short nTimeBase[256];
	float fData[256];


	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;

	op = pSTDdev->ST_Base.opMode;

	if( mds_Connect(pSTDdev) == WR_ERROR ) {
		return WR_ERROR;
	}
	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		return WR_ERROR;
	}

	nTrueSampleCnt = 255 / pV792->calib_increase_cnt + 1;
/*	printf("%s: nTrueSampleCnt=%d\n", pSTDdev->taskName, nTrueSampleCnt); */
	i=j=0;
	while(i<nTrueSampleCnt){
		nTimeBase[i] = j;
		i++;
		j+= pV792->calib_increase_cnt;
	}
/*	printf("%s: (i, j) = (%d, %d) %d\n", pSTDdev->taskName, i, j, nTrueSampleCnt); */
	if( j != 255 ) {
/*		nTrueSampleCnt++; */
		nTimeBase[i] = 255;
	}
	

	for( i=0; i< pV792->chNum; i++) 
	{
		for( j=0, k=0; j<nTrueSampleCnt; j++, k += pV792->calib_increase_cnt) {
			fData[j] = pV792->ST_Ch[i].Iped_ref[k];
/*			printf("%s: j, value: %d, %d\n", pSTDdev->taskName, j, k); */
		}
#if 1		
		if( k != 255 ) {
			nTrueSampleCnt++; 
			fData[j] = pV792->ST_Ch[i].Iped_ref[255];
		}
/*		printf("%s: (j, k) = (%d, %d) %d\n", pSTDdev->taskName, j, k, nTrueSampleCnt); */
#endif
		rawDataDesc = descr(&dtype_Float, fData, &nTrueSampleCnt, &_null);
		timeDesc = descr(&dypte_Short, nTimeBase, &nTrueSampleCnt, &_null); 
/*		printf("%s: %d = %f\n", pSTDdev->taskName, i, pV792->ST_Ch[i].Iped_ref[0]); */
		status = MdsPut(pV792->ST_Ch[i].strTagName, "BUILD_SIGNAL($1,,$2)", &rawDataDesc, &timeDesc, &_null); 
		if ( !statusOk(status)  ) {
			printf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, pV792->ST_Ch[i].strTagName, i, MdsGetMsg(status));
		}

		nTrueSampleCnt--;
		
		
	} 
	printf("%s: %s, %lu put finished!\n", pSTDdev->taskName, pSTDdev->ST_mds.treeIPport[op], pSTDdev->ST_Base_fetch.shotNumber );
	
	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);
#endif
	return WR_OK;
}


int ts_put_raw_value(ST_STD_device *pSTDdev)
{
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
/*	int dypte_Short = DTYPE_SHORT ; */
	int dypte_UShort = DTYPE_USHORT ;
	int dtype_ULong = DTYPE_ULONG; 
	int dtype_Double = DTYPE_DOUBLE;
	
	int rawDataDesc; /* must int type */
	int realRate_Desc, value_at_start_Desc;
	int id_end_Desc;
	int time_Desc;
	
	double *dTimeBase;

	int i, opmode; /*, ntemp2=0; */
	int accum_true_num, accum_true_num_1;
//	int diff_cnt=0;
	int calc_cnt;
	int put_cnt;

	int sampleRate;


	FILE *fpRaw=NULL;	
	char buf[128];


	double dStartTime;
	double dRealTimeGap;

	unsigned short *databuf=NULL;
//	unsigned short *databuf_tmp=NULL;

	char current_TagName[40];

	ST_V792 *pV792 = (ST_V792 *)pSTDdev->pUser;


	opmode = pSTDdev->ST_Base.opMode;

	if( pV792->accum_cnt < 1) {
		notify_error(1, "\"%s\" has no data! \n", pSTDdev->taskName );
		return WR_ERROR;
	}
	if( !pV792->use_file_save ) {
		notify_error(1, "\"%s\" has no file! \n", pSTDdev->taskName );
		return WR_ERROR;
	}	

#if USE_TSSC_FPGA 
	accum_true_num = (int)pV792->count_DG535_IN_PULSE; 
#else
	accum_true_num = (int)pV792->accum_cnt; 
#endif
//	accum_true_num = (int)pV792->accum_cnt;
	
	accum_true_num_1 = accum_true_num -1; /* 'cause start with 0 index in array */

//	diff_cnt = drvV792_get_header_dreg();
//	diff_cnt = 0; /* 2012. 7. 3  don't need this, FPGA modifed. */

	printf("%s: get DAQ:%d, get FPGA:%d\n", pSTDdev->taskName, pV792->accum_cnt, accum_true_num);
	

	databuf = (unsigned short *)malloc(sizeof(unsigned short)* accum_true_num);
	if( databuf == NULL){		
		epicsPrintf("\n%s: >>> malloc(sizeof(short)* %d)... fail\n", pSTDdev->taskName, accum_true_num);
		return WR_ERROR;
	}
	
	dTimeBase = (double *)malloc(sizeof(double)* accum_true_num);
	if( dTimeBase == NULL){		
		epicsPrintf("\n%s: >>> malloc(sizeof(double)* %d)... fail\n", pSTDdev->taskName, accum_true_num);
		free(databuf);
		return WR_ERROR;
	}

	if( mds_Connect(pSTDdev) == WR_ERROR ) {
		free(databuf);
		free(dTimeBase);
		return WR_ERROR;
	}
	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		mds_Disconnect(pSTDdev);
		free(databuf);
		free(dTimeBase);
		return WR_ERROR;
	}

	/* add between LTU and DG535 input time gap at the start point */
	if ( opmode != OPMODE_REMOTE ) {
		dStartTime = 0.0 + ((double)pV792->time_LTU_DG535_us/1000000.0);		
/*		printf("%s: time_LTU_DG535_us:%lf,  divided:%lf\n", pSTDdev->taskName, pV792->time_LTU_DG535_us, (pV792->time_LTU_DG535_us/1000000.0) ); */
	} else {
		dStartTime = pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime + ((double)pV792->time_LTU_DG535_us/1000000.0);
	}

	sampleRate = pSTDdev->ST_Base_fetch.nSamplingRate;

	dRealTimeGap = 1.0 / (double)sampleRate;

/*	printf("%s: start time:%lf,  DAQ cnt:%d, TSSC cnt:%d\n", pSTDdev->taskName, dStartTime, accum_true_num, pV792->count_DG535_IN_PULSE ); */

	calc_cnt = pV792->my_shot_period  * sampleRate; /*with b.g */

#if USE_TSSC_FPGA
	if( accum_true_num > (calc_cnt+(sampleRate*4))   )
	{
		printf("%s: Tree put with background date.(%d/%d)\n", pSTDdev->taskName, accum_true_num, calc_cnt);
		dTimeBase[0] = dStartTime;
		dTimeBase[1] = dStartTime+((double)pV792->time_Bg_delay_us/1000000.); 
/*		dTimeBase[1] = dStartTime+(0.05); */ /* fix to 50ms   2011. 4. 4 */
		for(i=2; i< accum_true_num; i++ )
			dTimeBase[i] = dTimeBase[i-2] + 0.10; /* 0.1s = 100ms = 10Hz  add 100ms to two step before */
	} 
	else 
		printf("%s: Tree put with only true date.(%d/%d)\n", pSTDdev->taskName, accum_true_num, calc_cnt );
#else 

	if ( opmode != OPMODE_REMOTE ) {
		dStartTime = 0.0 ;
	} else {
		dStartTime = -1 * pSTDdev->ST_Base_fetch.fBlipTime;
	}
	printf("%s: Tree put with only true date.(%d/%d)\n", pSTDdev->taskName, accum_true_num, calc_cnt );
		
#endif
		

	realRate_Desc = descr(&dtype_Double, &dRealTimeGap, &_null);
	value_at_start_Desc = descr(&dtype_Double, &dStartTime, &_null);
	id_end_Desc = descr(&dtype_ULong, &accum_true_num_1, &_null);
	time_Desc = descr(&dtype_Double, dTimeBase, &accum_true_num, &_null); 


	put_cnt = 0;
	for( i=0; i< pV792->chNum; i++) 
	{

		if( !(pV792->mask_channel & ( 0x1 << i) ) ) {
/*			printf("%s: %d not used!\n", pSTDdev->taskName, i); */
			continue;
		}
/*		sprintf(buf, "%s/b%d.%02d.dat", STR_CHANNEL_DATA_DIR, pSTDdev->BoardID,  i);  */
		sprintf(buf, "%s/%d_b%d.%02d.dat", STR_CHANNEL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->BoardID,  i); 

		strcpy( current_TagName, pV792->strTagName[i]);
//		printf("%s:%d tag name: %s\n", pSTDdev->taskName, i, pV792->ST_Ch[i].strTagName);

		fpRaw = fopen(buf, "rb");
		if( fpRaw == NULL) {
			epicsPrintf("can't open data file : %s\n", buf);
			mds_Close_withFetch(pSTDdev);
			mds_Disconnect(pSTDdev);
			free(databuf);
			free(dTimeBase);
			return(0);
		}
		/* 2012. 7. 3  don't need this, FPGA modifed. */
/*		fread( databuf_tmp, 2, diff_cnt, fpRaw);  */
		fread( databuf, 2, accum_true_num, fpRaw);
		fclose(fpRaw);
		fpRaw = NULL;

		rawDataDesc = descr(&dypte_UShort, databuf, &accum_true_num, &_null);
		
#if USE_TSSC_FPGA		
		if( accum_true_num > (calc_cnt+(sampleRate*4))  ) /* include background signal */
		{
			status = MdsPut(pV792->strTagName[i], "BUILD_SIGNAL($1,,$2)", &rawDataDesc, &time_Desc, &_null); 
		}
		else
		{
			status = MdsPut(pV792->strTagName[i], 
					"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"I\"), ,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					&rawDataDesc, &id_end_Desc, &value_at_start_Desc, &realRate_Desc, &_null);
		}
#else
		status = MdsPut(pV792->strTagName[i], 
					"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"I\"), ,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					&rawDataDesc, &id_end_Desc, &value_at_start_Desc, &realRate_Desc, &_null);
#endif

		put_cnt++;
		if ( !((status) & 1) )
		{
			printf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, pV792->strTagName[i], i, MdsGetMsg(status));
			printlog("%s: Error tag\"%s\", ch%d, shot %d: %s\n",pSTDdev->taskName, pV792->strTagName[i], i, pSTDdev->ST_Base_fetch.shotNumber, MdsGetMsg(status));
			notify_error(1,"%s: ch%d, %s: put failed!", pSTDdev->taskName, i, pV792->strTagName[i]);
		} 
		
	} 
	printf("    %s, %lu put finished!\n", pSTDdev->ST_mds.treeIPport[opmode], pSTDdev->ST_Base_fetch.shotNumber );
	
	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);

	if( opmode == OPMODE_REMOTE ) 
		sprintf(buf, "Remote");
	else if( opmode == OPMODE_LOCAL) 
		sprintf(buf, "Local");
	else if( opmode == OPMODE_CALIBRATION) 
		sprintf(buf, "Calibration");
	else
		sprintf(buf, "N/A");

	printlog("%s %d:%s: T0:%.2f, Gap:%dus, Priod:%d, snd:%d, put:%d\n", buf,
		pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->taskName, /*pSTDdev->ST_mds.treeIPport[opmode], */
		pSTDdev->ST_Base_fetch.dT0[0],
		pV792->time_LTU_DG535_us, /* pV792->time_LTU_LASER_us,  */
		pV792->my_shot_period, 
/*		pV792->time_Bg_delay_us, */
		 accum_true_num, put_cnt );
#if 0
	clearAllchFiles(pSTDdev); 
#endif
	free(databuf);
	free(dTimeBase);

	pV792->accum_cnt = 0;

	return WR_OK;
}


void clearAllchFiles(ST_STD_device *pSTDdev)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];

//	sprintf(strCmd, "rm -rf %s/b%d.*", STR_CHANNEL_DATA_DIR, slot); 
//	sprintf(strCmd, "rm -rf %s/*", STR_CHANNEL_DATA_DIR); 
	sprintf(strCmd, "rm -rf %s/%d_b%d.*", STR_CHANNEL_DATA_DIR, (int)pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->BoardID); 



/*	epicsPrintf("sys cmd: %s\n", strCmd); */
	pfp=popen( strCmd, "r");
	if(!pfp) {
		epicsPrintf("\n>>> devV792_set_ADCstart : pipe command ... failed\n");
		return ;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		epicsPrintf("\n>>> clearAllfiles : %s ", buff);
	}
	pclose(pfp);
}



