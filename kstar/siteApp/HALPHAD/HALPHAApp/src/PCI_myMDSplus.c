
#include "sfwCommon.h"
#include "sfwDriver.h"
#include "sfwMDSplus.h"


#include "myMDSplus.h"

/*
#include <dbCommon.h>	 for epicsPrintf 
#include <epicsThread.h>  for epicsThreadSleep() 
*/
/*
- DATA is an expression defining the Y values. (sin(6.28 * [0..999]/1000.) in our example)
- RAW is an expression indicating raw data. Often an acquired signal is made of raw data converted then by taking into account parameters such as gain and offset. Now we are not interested in it, so we omit its definition (the two commas in the TCL example are not a typing error).
- DIMENSION is an expression returning the array of X axis, usually (but not always) time values. (In our example the expression is [0..999]/1000.) 
*/


int channelMapping[96];




int proc_sendData2Tree(ST_STD_device *pSTDdev)
{
	int _null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int dypte_Short = DTYPE_SHORT ;
	int dtype_Double = DTYPE_DOUBLE;
	int dtype_ULong = DTYPE_ULONG; 

	short *databuf=NULL;	/* 2009. 7. 24 by woong.. for read bulk data */

	int rawDataDesc; /* Signal descriptor */
	int voltageTDesc;	
	int realRate_Desc, value_at_start_Desc;
	int id_end_Desc;

	int i; /*, ntemp2=0; */

	FILE *fpRaw=NULL;	
	char buf[128];
	int nTrueSampleCnt=0, nTrueSampleCnt_1;
	double dVpp10_16bit;
	int opmode;


	double dStartTime;   /* -1 sec */
	double dRealTimeGap;

	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;


	nTrueSampleCnt = (pSTDdev->ST_Base_fetch.dT1[0] - pSTDdev->ST_Base_fetch.dT0[0])*pSTDdev->ST_Base_fetch.nSamplingRate;

	databuf = (short *)malloc(sizeof(short)* nTrueSampleCnt);
	if( databuf == NULL){		
		epicsPrintf("\n%s: >>> malloc(sizeof(short)* %d)... fail\n", pSTDdev->taskName, nTrueSampleCnt);
		return WR_ERROR;
	}
				
/*	
	switch( nSampleClk ) {
	case 1000: nTimeGap = 1000; break;
	case 2000: nTimeGap = 500; break;
	case 5000: nTimeGap = 200; break;
	case 10000: nTimeGap = 100; break;
	case 20000: nTimeGap = 50; break;
	case 50000: nTimeGap = 20; break;
	case 100000: nTimeGap = 10; break;
	case 200000: nTimeGap = 5; break;
	default: {
			fprintf(stderr,"appSampleRate not valid\n");
			free(data);
			free(timebase);
			free(unTimebase);
			return 0;
			}
	}
	nSkipCnt = 200000 / nSampleClk;
	fprintf(stdout, "\nMDSplus >>> Sample clock:%dHz, nTimeGap:%d, nSkipCnt:%d\n", nSampleClk, nTimeGap, nSkipCnt );
*/

/*************************************************************************/
#if 1
	if( mds_Connect(pSTDdev) == WR_ERROR ) {
		free(databuf);
		return WR_ERROR;
	}

	if( mds_Open_withFetch(pSTDdev) == WR_ERROR ) {
		free(databuf);
		return WR_ERROR;
	}
#endif

	opmode = pSTDdev->ST_Base.opMode;

/*************************************************************************/
	if ( opmode != OPMODE_REMOTE ) {
		dStartTime = 0.0;  /* Data Zero Start Time */
	} else {
/*		dStartTime = (pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime) * pSTDdev->ST_Base_fetch.nSamplingRate; */
		dStartTime = pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime;
	}
	dRealTimeGap = 1.0 / (double)pSTDdev->ST_Base_fetch.nSamplingRate;
	dVpp10_16bit = 10.0 / 32768.0;
	nTrueSampleCnt_1 = nTrueSampleCnt - 1;
#if 0
	fprintf(stdout,"True Cnt:%d, Freq: %d, T0: %f, T1: %f, gap: %f \n", 
			nTrueSampleCnt, 
			pSTDdev->mds_nSamplingRate, 
			pSTDdev->mds_dT0[0], 
			pSTDdev->mds_dT1[0], 
			dRealTimeGap);
#endif
	realRate_Desc = descr(&dtype_Double, &dRealTimeGap, &_null);
	voltageTDesc = descr(&dtype_Double, &dVpp10_16bit, &_null);
	value_at_start_Desc = descr(&dtype_Double, &dStartTime, &_null);
	id_end_Desc = descr(&dtype_ULong, &nTrueSampleCnt_1, &_null);
	
	pAcq196->mdsplus_snd_cnt = 0;
	for(i = 0; i<96; i++) 
	{
		if( pAcq196->channelOnOff[i] ) 
		{
			sprintf(buf, "%s/b%d.%02d.dat", STR_CHANNEL_DATA_DIR, pAcq196->slot, i);

			fpRaw = fopen(buf, "rb");
			if( fpRaw == NULL) {
				status = MdsClose(pSTDdev->ST_mds.treeName[opmode], (int *)(pSTDdev->ST_Base_fetch.shotNumber));
				
				epicsPrintf("can't open data file : %s\n", buf);
				MdsDisconnect(pSTDdev->ST_mds.treeIPport);
				free(databuf);
				return(0);
			}
			fread( databuf, 2, nTrueSampleCnt, fpRaw);
			fclose(fpRaw);
#if 0			
/*			for(j = 0; j < pAcq196->sampleDataCnt; j++) */
			for(j = 0; j < nTrueSampleCnt; j++) 
			{
/*				fread( &int16TempBuf, 2, 1, fpRaw );
				fTemp = ((float)(int16TempBuf) * 10.0 ) / 32768.0;  */
				data[j] = ((float)(databuf[j]) * 10.0 ) / 32768.0; 
/*				
				if ( j > nTrueSampleCnt ) {
					epicsPrintf(" got to end count:%lu\n", j); 
					break;
				}
*/
			}
#endif


/*			dataDesc = descr(&dtype_Float, data, &nTrueSampleCnt, &_null); */
			rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);

			status = MdsPut(pAcq196->strTagName[i] ,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&rawDataDesc, &voltageTDesc,  &id_end_Desc, &value_at_start_Desc, &realRate_Desc, &_null);
			
			if ( !((status) & 1) )
			{
				printf("%s: Error tag\"%s\", ch%d: %s\n",pSTDdev->taskName, pAcq196->strTagName[i], i, MdsGetMsg(status));
				printlog("%s: Error tag\"%s\", ch%d, shot %d: %s\n",pSTDdev->taskName, pAcq196->strTagName[i], i, pSTDdev->ST_Base_fetch.shotNumber, MdsGetMsg(status));
#if 0
				mds_Close_Disconnect(pAcq196);
				clearAllchFiles((int)pAcq196->slot);
	
				free(databuf);
				free(data);
				data=NULL;
				databuf=NULL;
				return 0;
#endif
			} 
			else 
				pAcq196->mdsplus_snd_cnt++;

/*			if( (pAcq196->mdsplus_snd_cnt % 10) == 9 ) 2010. 11. 11*/
				scanIoRequest(pSTDdev->ioScanPvt_userCall); 

/*			printf("%s, <cnt:%d>\n", pSTDdev->taskName, i ); */

		} 
		else  /*  if( pAcq196->channelOnOff[i] )  */
		{ 
/*			epicsPrintf(". ch%d off ", i)
			if( ntemp2 > 10) {
				epicsPrintf("\n");
				ntemp2 = 0;
			}
			ntemp2++;
*/
		}
	} /* for(i = 0; i<96; i++)   */

/*	printf("%s, Tree Put Done! <cnt:%d>\n", pSTDdev->taskName, pAcq196->mdsplus_snd_cnt ); */

	if( opmode == OPMODE_REMOTE ) 
		sprintf(buf, "Remote");
	else if( opmode == OPMODE_LOCAL) 
		sprintf(buf, "Local");
	else if( opmode == OPMODE_CALIBRATION) 
		sprintf(buf, "Calibration");
	else
		sprintf(buf, "N/A");


	printlog("%s %d: %s:(%.2f~%.2f) %dKHz, (snd: %.2f MB) (%d ch), Cnt:%d\n", 
		buf, pSTDdev->ST_Base_fetch.shotNumber, pSTDdev->taskName, 
				pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0], 
				pSTDdev->ST_Base_fetch.nSamplingRate/1000, 
/*				pAcq196->needDataSize/1024.0/1024.0, */
				pAcq196->mdsplus_snd_cnt *2*nTrueSampleCnt/1024.0/1024.0, 
				pAcq196->mdsplus_snd_cnt,
				nTrueSampleCnt 	);

	scanIoRequest(pSTDdev->ioScanPvt_userCall);  /* for last update */

/*
	printf("Put %s: raw:%.2fMB, snd:%.2fMB(%d), Cnt:%d ... OK!\n", pSTDdev->taskName, 
						pAcq196->needDataSize/1024.0/1024.0,
						pAcq196->mdsplus_snd_cnt *2*nTrueSampleCnt/1024.0/1024.0, 
						pAcq196->mdsplus_snd_cnt,
						nTrueSampleCnt );
*/
/*	fprintf(stdout,"\nMDSplus >>> Data size: %luKB/ch\n", (4*pAcq196->sampleDataCnt)/1024 ); */

	free(databuf);
	databuf = NULL;

#if 1
	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);
#endif
	
#if 0
	clearAllchFiles((int)pAcq196->slot);
#endif
	
	
	return WR_OK;
}



/******************************************
  use file pointer   by woong 2009. 3. 25 
  use ptaskconfig pointer   2009. 05. 13
  
******************************************/
#if 1
int proc_dataChannelization( ST_STD_device *pSTDdev)
{	
	FILE *fp[96], *fpRaw;
	unsigned int i, j;
/*	short int16TempBuf=0; */
	short int16TempPack[96];
	unsigned long int sampleCnt;
	unsigned long int sampleDataCnt;
	char buf[128];
	int nVal, nTemp;
	ST_ACQ196 *pAcq196 = (ST_ACQ196 *)pSTDdev->pUser;
	
/*	epicsPrintf("Data Size is %lu \n", pAcq196->rawDataSize); */
	if( pAcq196->rawDataSize < 1)
	{
		epicsPrintf("raw file size error \n");
		return (0);
	}
	
/*	sprintf(buf, "%s%d", STR_CH_MAPPING_FILE, pAcq196->slot);*/
	sprintf(buf, "%s", STR_CH_MAPPING_FILE);
	fpRaw = fopen(buf, "r");
	if( fpRaw == NULL) {
		epicsPrintf("can't open '%s'\n", buf);
		return(0);
	}

	for( i=0; i < 96; i++) 
	{
		if( fgets(buf, 32, fpRaw) == NULL ) {
			fprintf(stdout, "file EOF error!\n");
			return(0);
		}
		sscanf(buf, "%d %d", &nTemp, &nVal);
		if( nTemp != (i+1) ) {
			fprintf(stdout, "channel index order error!\n");
			return (0);
		}
		channelMapping[nVal] = i;
#if 0
		epicsPrintf("channelMapping[%d] : %d\n", i, channelMapping[i]);
#endif
	}
	fclose(fpRaw);
	
/***********************************************/
/**   Open Raw data file... not channelization  **********/
	sprintf(buf, "%s", pAcq196->dataFileName);
	fpRaw = fopen(buf, "rb");
	if( fpRaw == NULL) {
		epicsPrintf("can't open raw data file\n");
		return(0);
	}
	
/***********************************************/
/**   Open parsing data files for write ... **********/

	for( i=0; i < 96; i++) 
	{
/*		sprintf(buf, "/media/data2/b%d.%02d.dat", pAcq196->slot, i); */
		sprintf(buf, "%s/b%d.%02d.dat", STR_CHANNEL_DATA_DIR, pAcq196->slot, i); 
#if 0
		epicsPrintf("%s\n", buf);
#endif
		fp[i] = fopen(buf, "wb");
		if( fp[i] == NULL) {
			epicsPrintf("can't open \"%s\"\n", buf);
			return(0);
		}
	}
	sampleCnt = pAcq196->rawDataSize / ( 96*2 );

/*	printf("%s: Channelization : please wait ...", pSTDdev->taskName); */

	sampleDataCnt = 0;
	for (i=0; i < sampleCnt-1; i++) 
	{
		/* read one bulk data  */
		fread( &int16TempPack, 192, 1, fpRaw );
		for( j=0; j< 96; j++) 
		{
			fwrite( &int16TempPack[j], 2 , 1, fp[ channelMapping[j] ] ); 
		} /* for( j=0; j< ch; j++)  */

		if( nTemp > 10000) {			
			pAcq196->parsing_remained_cnt = (sampleCnt - sampleDataCnt)/100000;
/*			epicsPrintf("%s: remained cnt: %d\n", pSTDdev->taskName, pAcq196->parsing_remained_cnt ); */
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
			nTemp = 0;
		}
		nTemp++;

		sampleDataCnt++;

	} /* for (i=0; i < sampleCnt; i++)  */

/*	epicsPrintf("%s: total sampling count : %lu\n", pSTDdev->taskName, sampleDataCnt); */

	fclose(fpRaw);
	for( i=0; i< 96; i++) {
		fclose(fp[i]);
	}

#if 1 
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	
	sprintf(strCmd, "rm -rf %s", pAcq196->dataFileName);
/*	epicsPrintf("\n >>  %s\n", strCmd); */
	pfp=popen( strCmd, "r");
	if(!pfp) {
		epicsPrintf("\n>>> proc_dataChannelization : pipe command ... failed\n");
		return 0;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		epicsPrintf("\n>>> proc_dataChannelization : %s", buff);
	}
	pclose(pfp);
}
#endif

/*	epicsPrintf("... DONE\n"); */

	return (1);
}
#endif

/* use file descriptor   by woong 2009. 3. 25 */
#if 0
int proc_dataChannelization(char *rawFileName, int slot, unsigned long long fileSize)
{
	FILE *fp;
	int fd[96], fdRaw;
	unsigned int i, j;
	short int16TempBuf=0;
	short int16TempPack[96];
	unsigned int sampleCnt;
	char buf[128];
	int nVal, nTemp;
	FILE *pfp;
	char strCmd[128];
	char buff[128];
	
/* Edit by TGLee at 20080502  */	
	epicsPrintf("Data Size is %llu \n", fileSize);
	if( fileSize < 1)
	{
		epicsPrintf("raw file size some error \n");
		return (0);
	}
	
	sprintf(buf, "/root/ioc/config/channel_mapping.slot%d", slot);
	fp = fopen(buf, "r");
	if( fp == NULL) {
		epicsPrintf("can't open '%s'\n", buf);
		return(0);
	}

	for( i=0; i < 96; i++) 
	{
		if( fgets(buf, 32, fp) == NULL ) {
			fprintf(stdout, "file EOF error!\n");
			return(0);
		}
		sscanf(buf, "%d %d", &nTemp, &nVal);
		if( nTemp != (i+1) ) {
			fprintf(stdout, "channel index order error!\n");
			return (0);
		}
		channelMapping[nVal] = i;
#if 0
		epicsPrintf("channelMapping[%d] : %d\n", i, channelMapping[i]);
#endif
	}
	fclose(fp);


	sprintf(buf, "%s", rawFileName);
	fdRaw = open(buf, O_RDONLY);
	if( fdRaw < 0) {
		epicsPrintf("can't open raw data file\n");
		return(0);
	}

	
	for( i=0; i < 96; i++) 
	{
		sprintf(buf, "/data2/b%d.%02d.dat", slot, i);
#if 0
		epicsPrintf("%s\n", buf);
#endif
		fd[i] = open(buf, O_WRONLY|O_CREAT);
		if( fd[i] < 0) {
			epicsPrintf("can't make data file\n");
			return(0);
		}
	}

	sampleCnt = fileSize / ( 96*2 );

	epicsPrintf("\n>>> sampleingCnt= %d \n", sampleCnt);
	epicsPrintf(">>> channelization : please wait ...\n ");

	g_sampleDataCnt = 0;
	for (i=0; i < sampleCnt-2; i++) 
	{
		nVal = read(fdRaw, int16TempPack, 192 );
		if( nVal != 192 )
		{
			epicsPrintf(">>> read(fdRaw) = (%d) not 192byte\n", nVal );
			close(fdRaw);
			return 0;
		}			
		
		for( j=0; j< 96; j++) 
		{
/*			fread( &int16TempBuf, 2, 1, fpRaw ); */
/*			fwrite( &int16TempBuf, 2 , 1, fp[ channelMapping[j] ] ); */
			write(fd[ channelMapping[j] ], &int16TempPack[j], 2);
		} /* for( j=0; j< ch; j++)  */
		
		if( nTemp > 100000) {
			epicsPrintf("remained cnt: %d\n", (sampleCnt - g_sampleDataCnt)/100000 );
			nTemp = 0;
		}
		nTemp++;
				
		g_sampleDataCnt++;
	} /* for (i=0; i < sampleCnt; i++)  */

	fprintf(stdout, "cvTask >>> total sampling count : %d\n", g_sampleDataCnt);

	close(fdRaw);
	for( i=0; i< 96; i++) {
		close(fd[i]);
	}
	
	sprintf(strCmd, "rm -rf /data1/acq196.%d", slot);
	pfp=popen( strCmd, "r");
	if(!pfp) {
		epicsPrintf("\n>>> proc_dataChannelization : pipe command ... failed\n");
		return 0;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		epicsPrintf("\n>>> proc_dataChannelization : %s", buff);
	}
	pclose(pfp);
	

	printf ("OK\n");
	return (1);
}
#endif

int swap32(int *n)
{
	unsigned char *cptr, tmp0, tmp1;

	cptr = (unsigned char*)n;
	
	tmp0 = cptr[0];
	tmp1 = cptr[1];

	cptr[0] = cptr[3];
	cptr[1] = cptr[2];
	cptr[2] = tmp1;
	cptr[3] = tmp0;

	return 1;
}

void clearAllchFiles(int slot)
{
	FILE *pfp;
	char strCmd[128];
	char buff[128];

	sprintf(strCmd, "rm -rf /media/data2/b%d.*", slot);
/*	epicsPrintf("sys cmd: %s\n", strCmd); */
	pfp=popen( strCmd, "r");
	if(!pfp) {
		epicsPrintf("\n>>> devACQ196_set_ADCstart : pipe command ... failed\n");
		return ;
	}
	while(fgets(buff, 128, pfp) != NULL ) {
		epicsPrintf("\n>>> clearAllfiles : %s ", buff);
	}
	pclose(pfp);
}




