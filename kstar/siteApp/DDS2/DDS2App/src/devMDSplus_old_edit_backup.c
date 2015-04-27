/******************************************************************************
 *  Copyright (c) 2006 ~ by OLZETEK. All Rights Reserved.                     *
 ******************************************************************************/

/*==============================================================================
                        EDIT HISTORY FOR MODULE


2007. 10. 08 
int sendEDtoTreeFromChannelData(int shot, int mode, int nSampleClk )
	if( timebase[fileSampleCnt] >= nT1 ) break;
	cut off by app for fitting end of shot





*/


#include "drvM6802.h"

#include "devMDSplus.h"


#define DEBUG_FILE_CONV		0

/* An example of Data readout using the MDSplus Data Access library (mdslib) */
int dtypeFloat = DTYPE_FLOAT; /* We are going to read a signal made of float values */
int dtypeLong = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
int dtypeString = DTYPE_CSTRING;

float *data; /* Array of floats used for signal */
float *timebase;   /* Array of floats used for timebase */
int dataDesc; /* Signal descriptor */
int timeDesc; /* Timebase descriptor */
unsigned int size; /* Length of signal */

int used_channel;

int readModelTree(int shot)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */
	int i, retLen; /* Returned length */
	char buf[128];

	/* Connect to MDSplus */
	socket = MdsConnect(ADDR_MODEL_TREE);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server.\n");
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", ADDR_MODEL_TREE);
	
	/* Open tree */
	status = MdsOpen(TREE_NAME, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsOpen(\"%s\", shot: %d)... OK\n", TREE_NAME, shot);

	/* use getSize to get size of signal */
	size = getSize(NODE_NAME);
	if ( size < 1 )
	{
		/* error */
		fprintf(stderr,"Error retrieving length of signal\n");
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> getSize(\"%s\") = %d ... OK\n", NODE_NAME, size);
	
	/* use malloc() to allocate memory for the signal */
	data = (float *)malloc(size * sizeof(float));
	if ( !data )
	{
		fprintf(stderr,"Error allocating memory for data\n");
		return 0;
	}
/*	timebase = (float *)malloc(size * sizeof(float)); 
	if ( !timebase )
	{
		fprintf(stderr,"Error allocating memory for timebase\n");
		free( (void *)data );
		return 0;
	}
*/
	
	/* create a descriptor for this signal */
	dataDesc = descr(&dtypeFloat, data, &size, &null);
	
	/* create a descriptor for the timebase */
/*	timeDesc = descr(&dtypeFloat, timebase, &size, &null); */
	

	/* retrieve signal */
	status = MdsValue(NODE_NAME, &dataDesc, &null, &retLen );
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal: %s\n", MdsGetMsg(status));
		free( (void *)data );
		return 0;
	}
	
	strcpy(buf, "DIM_OF("); strcat(buf, NODE_NAME);	strcat(buf, ")");
	printf("%s\n", buf);	
	/* retrieve timebase of signal */
/*	status = MdsValue(buf, &timeDesc, &null, &retLen); */
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving timebase: %s\n", MdsGetMsg(status));
		free( (void *)data );
/*		free( (void *)timebase ); */
		return 0;
	}
	
	/* do whatever with the data */
	/* print ALL the data */
	for ( i = 0 ; i < size ; i++ )
	{
/*		printf("%i X:%f Y:0x%X\n", i, timebase[i], data[i]); */
		if( i > 20 ) break;
	}

	/* Close tree */
	status = MdsClose(TREE_NAME, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free( (void *)data );
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> Tree close ...  OK\n" );
	
	
	/* free the dynamically allocated memory when done */
	free( (void *)data );
/*	free( (void *)timebase ); */
	
	/* done */
	return 0;
}

	/* Get the size (number of elements) of the node whose name is passed as argument */
	/* It will use TDI function SIZE(), returning the number of elements of its argument */
int getSize(char *sigName)
{
	int retSize; /* Will containg the returned size */
	int lenDescr; /* Descriptor for returned size */
	int null = 0; /* Used to mark the end of the argument	list */
	int status; /* Returned function status */
	char expression[1024]; /* A buffer large enough to contain the 	expression SIZE(<signal>) */
	
	/* Build the expression SIZE(<signal>) */
	sprintf(expression, "SIZE(%s)", sigName);
	
	/* Create the descriptor */
	lenDescr = descr(&dtypeLong, &retSize, &null);
	
	/* Evaluate the expression */
	status = MdsValue(expression, &lenDescr, &null, 0);
	/* Check status */
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error getting the size of %s.\n", sigName);
		return 0;
	}
	return retSize;
}

int sendEDtoTreeFromChannelData(int shot, int mode  )
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */

	int tstat, len;
	int idesc = descr(&dtypeLong, &tstat, &null);



	int i, j, ntemp=0, ntemp2=0;
	int int32TempBuf=0;
	FILE *fp;
	char buf[512], buf1[32];
	int fileSampleCnt;
	int nCnt;
	float fTemp = 0.0;
	float fTempStore = 0.0;
	int nSkipCnt = 0;

	char bufLineScan[512];
	char treeIPport[32];
	char treeName[32];

/*	double dTimeGap = 0.0; */
	int nTimeGap=0;
	float fT0 = 0.;
	float fT1 = 0.;
	int nSampleClk;
	
	unsigned int *unTimebase;   /* Array of unsigned int for  floating point error */

/* just for test .............................. */
/*	g_sampleDataCnt =10; */
/* just for test .............................. */

	drvM6802_taskConfig *ptaskConfig = drvM6802_find_taskConfig("master");

	data = (float *)malloc(sizeof(float)*g_sampleDataCnt);
	timebase = (float *)malloc(sizeof(float)*g_sampleDataCnt); 
	unTimebase = (unsigned int *)malloc(sizeof(unsigned int)*g_sampleDataCnt);

	
	
	nSampleClk = ptaskConfig->appSampleRate;

/*	dTimeGap = 1.0 / (double)(nSampleClk*1000 ); */
	switch(nSampleClk) {
	case 1: nTimeGap = 1000; break;
	case 2: nTimeGap = 500; break;
	case 5: nTimeGap = 200; break;
	case 10: nTimeGap = 100; break;
	case 20: nTimeGap = 50; break;
	case 50: nTimeGap = 20; break;
	case 100: nTimeGap = 10; break;
	default: {
			fprintf(stderr,"appSampleRate not valid\n");
			free(data);
			free(timebase);
			free(unTimebase);
			return 0;
			}
	}
	nSkipCnt = 100 / nSampleClk;
	fprintf(stdout, "\nMDSplus >>> Sample clock: %dKHz, nTimeGap: %d, nSkipCnt:%d\n", nSampleClk, nTimeGap, nSkipCnt );


/*************************************************************************/
/*************************************************************************/
	if( (fp = fopen("/root/ioc/config/mdsplus.cfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can't Read mdsplus.cfg file\n");
		return(0);
	}
	while(1) {
		if( fgets(bufLineScan, 128, fp) == NULL ) break;
		if( bufLineScan[0] == '#') ;
		else {
			sscanf(bufLineScan, "%s %s", buf, buf1);
			if( mode == OPMODE_TEST ) {
				if( strcmp(buf, "TEST_IP") == 0) {
					strcpy(treeIPport, buf1);
					fprintf(stdout, "Testl MDS server = %s\n", treeIPport );
				} else if( strcmp(buf, "TEST_TREE") == 0) {
					strcpy(treeName, buf1);
					fprintf(stdout, "Test tree name = %s\n", treeName );
				} 
			} else if ( mode == OPMODE_REMOTE ) {
				if( strcmp(buf, "REMOTE_IP") == 0) {
					strcpy(treeIPport, buf1);
					fprintf(stdout, "Remote MDS server = %s\n", treeIPport );
				} else if( strcmp(buf, "REMOTE_TREE") == 0) {
					strcpy(treeName, buf1);
					fprintf(stdout, "Remote tree name = %s\n", treeName );
				}
			} else if ( mode == OPMODE_CALIBRATION ) {
				if( strcmp(buf, "CALIBRATION_IP") == 0) {
					strcpy(treeIPport, buf1);
					fprintf(stdout, "Calibration MDS server = %s\n", treeIPport );
				} else if( strcmp(buf, "CALIBRATION_TREE") == 0) {
					strcpy(treeName, buf1);
					fprintf(stdout, "Calibration tree name = %s\n", treeName );
				}
			} else {
				fprintf(stdout, "Must set Local or Remote or Calibration!\n");
				fclose(fp);
				return 0;
			}

		}
	}
	fclose(fp);
/*************************************************************************/
/*************************************************************************/

	/* Connect to MDSplus */
	socket = MdsConnect(treeIPport);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", treeIPport);
		free(data);
		free(timebase); 
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);

#if 1
	if( mode != OPMODE_REMOTE ) {
		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
/*		status = MdsValue("TCL(\"set tree dds2lr/shot=-1\")", &idesc, &null, &len); */
		status = MdsValue(buf, &idesc, &null, &len); 
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
			free(data);
			free(timebase); 
			return 0;
		}
		sprintf(buf, "TCL(\"create pulse %d\")", shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
			free(data);
			free(timebase); 
			return 0;
		}
		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
/*		status = MdsValue("TCL(\"exit\")", &idesc, &null, &len); */

		fprintf(stdout,"\nMDSplus >>> Create new tree(\"%d\")... OK\n", shot);
	}
#endif

	fT0 = ptaskConfig->fT0;
	fT1 = ptaskConfig->fT1;
	fprintf(stdout, "\nMDSplus >>> Used external clock, nSkipCnt = %d, T0=%f, T1=%f\n",  nSkipCnt, fT0, fT1);

	
	
	
	/* Open tree */
	status = MdsOpen(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(data);
		free(timebase); 
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsOpen(\"%s\", shot: %d)... OK\n", treeName, shot);

	

	for(i = 0; i<used_channel; i++) 
	{
		if( channelOnOff[i] ) {

			sprintf(buf, "/data/ch%02d.dat", i);
			fp = fopen(buf, "rb");
			if( fp == NULL) {
				fprintf(stderr, "can't oepn data file : %s\n", buf);
				return(0);
			}
/*			fseek(fp, 0, SEEK_END);
			fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);
			fileSampleCnt = fileSize/4;
*/
/*			fprintf(stdout, "%d file size: %d\n", fileSize); */
			nCnt=0;
			fTempStore = 0.0;
			fileSampleCnt = 0;
/* Modify +/- 5Volt => +/- 10Voltag Module Changed and edit Program by TGLee at 20080425 : second Slot install  */
			if( 32 <= i && i < 64)
			{	
				for(j = 0; j < g_sampleDataCnt; j++)
				{
					fread( &int32TempBuf, 4, 1, fp );
					fTemp = ((float)(int32TempBuf >> 8) * 10.0 ) / 8388608.0; /* 10Volt Module */
					fTempStore += fTemp;
					nCnt++;

					if( nCnt > (nSkipCnt-1)) {
/*						data[fileSampleCnt] = fTempStore / (float)nCnt;  when use mean value */
						data[fileSampleCnt] = fTemp;
#if 0
						timebase[fileSampleCnt] = fileSampleCnt;
#else
						if( fileSampleCnt != 0 )
/*							timebase[fileSampleCnt] = timebase[fileSampleCnt-1] + (float)dTimeGap; */
							unTimebase[fileSampleCnt] = unTimebase[fileSampleCnt-1] + nTimeGap;
						else unTimebase[fileSampleCnt] = 0; /* always start 0 time */
#endif

						if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) ) break;
					
						nCnt = 0;
						fTempStore = 0.0;
						fileSampleCnt++;
					}
				}
				fclose(fp);
			} else {
				for(j = 0; j < g_sampleDataCnt; j++)
				{
					fread( &int32TempBuf, 4, 1, fp );
					fTemp = ((float)(int32TempBuf >> 8) * 5.0 ) / 8388608.0;  /*    5Volt Module */
					fTempStore += fTemp;
					nCnt++;

					if( nCnt > (nSkipCnt-1)) {
/*						data[fileSampleCnt] = fTempStore / (float)nCnt;  when use mean value */
						data[fileSampleCnt] = fTemp;
#if 0
						timebase[fileSampleCnt] = fileSampleCnt;
#else
						if( fileSampleCnt != 0 )
/*							timebase[fileSampleCnt] = timebase[fileSampleCnt-1] + (float)dTimeGap; */
							unTimebase[fileSampleCnt] = unTimebase[fileSampleCnt-1] + nTimeGap;
						else unTimebase[fileSampleCnt] = 0;  /* always start 0 time */
#endif

						if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) ) break;
					
						nCnt = 0;
						fTempStore = 0.0;
						fileSampleCnt++;
					}

#if 0
		fprintf(stdout, "%d = %d, 0x%X\n", i, data[i], data[i] ); 
#endif
				}
				fclose(fp);
			}
/*
			fprintf(stdout, "ch%d count = %d\n", i, fileSampleCnt); 
*/
			for(j=0; j< fileSampleCnt; j++) {
				timebase[j] = (float)((double)unTimebase[j] * (double)0.0000010 );
			}
			
			
			dataDesc = descr(&dtypeFloat, data, &fileSampleCnt, &null);
			timeDesc = descr(&dtypeFloat, timebase, &fileSampleCnt, &null); 
			status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null); 
			if ( !statusOk(status) )
			{
				fprintf(stderr,"Error writing signal: %s\n", MdsGetMsg(status));
				free(data);
				return 0;
			}
			fprintf(stdout,"ch%d .", i );

			if( ntemp > 10) {
				fprintf(stdout,"\n");
				ntemp = 0;
			}
			ntemp++;

		} else {
			fprintf(stdout, ". ch%d off ", i);
			if( ntemp2 > 10) {
				fprintf(stdout,"\n");
				ntemp2 = 0;
			}
			ntemp2++;
		}
	} /* for(i = 0; i<used_channel; i++)  */
	fprintf(stdout,"\n1st and 3nd 5Volt Module, 2nd Module 10Volt Module");
	fprintf(stdout,"\nDone\n");
#if 0
	{
		FILE *fpTemp;
		int index;
		
		fpTemp = fopen("mdstest.txt", "w");
		for( index=0; index< fileSampleCnt; index++) {
			fprintf(fpTemp, "%d=%f\n", index, timebase[index] );
		}
		fclose(fpTemp);
	}
#endif
	
	/* Close tree */
	status = MdsClose(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(data);
		free(timebase);
		free(unTimebase);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> Data size: %dKB/ch\n", (4*g_sampleDataCnt)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	/* Done */

	free(data);
	free(timebase);
	free(unTimebase);

	return 1;
}



/******************************************/

/******************************************/
int dataChannelization()
{
	FILE *fp[256], *fpRaw;
	unsigned int i, j;
	int dma_cnt, dmaBlock_size;
	int int32TempBuf=0;
	unsigned int sampleCnt;
	unsigned char *cptr;
	char buf[128];

	cptr = (unsigned char*)malloc(4);


	fpRaw = fopen(FILE_NAME_RAWDATA, "rb");
	if( fpRaw == NULL) {
		printf("can't open raw data file\n");
		return(0);
	}

	if ( fseek(fpRaw, 0, SEEK_END) == -1 ) {
		perror("fseek failed");
		return(0);
	}
	fseek(fpRaw, -12L, SEEK_CUR);
	fread( &used_channel, 4, 1, fpRaw );
	fread( &dmaBlock_size, 4, 1, fpRaw );
	fread( &dma_cnt, 4, 1, fpRaw );

	if( (dma_cnt < 1) || (used_channel < 1))
	{
		printf("raw file have some error\n");
		return (0);
	}
	fprintf(stdout, "\ndma block: %dKB, dma count: %d, ch: %d \n", dmaBlock_size/1024, dma_cnt, used_channel);

	if( used_channel >= 256 )
	{
		printf("max to 256 ch\n");
		return(0);
	}

	for( i=0; i < used_channel; i++) 
	{
		sprintf(buf, "/data/ch%02d.dat", i);
#if 0
		fprintf(stdout, "%s\n", buf);
#endif
		fp[i] = fopen(buf, "wb");
		if( fp[i] == NULL) {
			printf("can't make data file\n");
			return(0);
		}
	}


/*
// ch * 4 byte : byte per one sample clock
// dmaBlock_size*dma_cnt : total file size
*/
	sampleCnt = ((unsigned long long)dmaBlock_size*(unsigned long long)dma_cnt) / (used_channel*4);
/*	fprintf(stdout, "\n>>> sampleingCnt unsigned long long = %d", sizeof(unsigned long long));
	fprintf(stdout, "\n>>> sampleingCnt unsigned int = %d", sizeof(unsigned int));
	fprintf(stdout, "\n>>> sampleingCnt double = %d", sizeof(double));
	fprintf(stdout, "\n>>> sampleingCnt= %u   %fl\n", sampleCnt, (double)sampleCnt);
	fprintf(stdout, "\n>>> sampleingCnt ABS = %fl \n", fabs((double)sampleCnt));
*/

	printf(">>> channelization : please wait ...\n ");
	fseek(fpRaw, 0, SEEK_SET);

	for (i=0; i < sampleCnt*used_channel; i++) {
		fread( &int32TempBuf, 4, 1, fpRaw );
		memcpy(cptr, &int32TempBuf, 4);
		if( cptr[3] == 0 ) {
			fprintf(stdout, "cvTask >>> %d : meet first channel\n", i);
#if DEBUG_FILE_CONV
		fprintf(stdout, "%d 1=%02X \t", i, cptr[0]);
		fprintf(stdout, "%d 2=%02X \t", i, cptr[1]);
		fprintf(stdout, "%d 3=%02X \t", i, cptr[2]);
		fprintf(stdout, "%d 4=%02X \n", i, cptr[3]);
#endif
			fseek(fpRaw, -4L, SEEK_CUR);
			break;
		}

	}

#if DEBUG_FILE_CONV
	fprintf(stdout, "cvTask >>> Enter converting loop...\n");
#endif
	g_sampleDataCnt = 0;
	for (i=0; i < sampleCnt-1; i++) 
	{
		for( j=0; j< used_channel; j++) 
		{
			fread( &int32TempBuf, 4, 1, fpRaw );
#if 0
			if( j == 0 ) 
			{
				memcpy(cptr, &int32TempBuf, 4);
				if( cptr[3] != 0 ) {
					fprintf(stdout, "cvTask >>> cnt : %d (%d) Not first, val: 0x%X\n", i, (used_channel*i), int32TempBuf );
					return (0);
				}
			}
#endif
			fwrite( &int32TempBuf, 4, 1, fp[j] );
		} /* for( j=0; j< ch; j++)  */

		g_sampleDataCnt++;

	} /* for (i=0; i < sampleCnt; i++)  */

	fprintf(stdout, "cvTask >>> total sampling count : %d\n", g_sampleDataCnt);

	fclose(fpRaw);
	for( i=0; i< used_channel; i++) {
		fclose(fp[i]);
	}

	free(cptr);

	printf ("OK\n");
	return (1);
}

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


int checkDataMiss()
{
	FILE *fpRaw, *fpNet;
	unsigned int i;
	unsigned int dma_cnt, dmaBlock_size;
	int int32TempBuf=0;
	void *dummybuf;
	unsigned int sampleCnt;
	unsigned char *ptmp;
	int nTemp, nStart, nStop;
	char cPrev=0;

	dummybuf = (int *)malloc(4*100);
	ptmp = (unsigned char*)malloc(4);


	fpRaw = fopen(FILE_NAME_RAWDATA, "rb");
	if( fpRaw == NULL) {
		printf("can't open raw data file\n");
		free(dummybuf);
		free(ptmp);
		return(0);
	}

	fpNet = fopen(FILE_NAME_NETDATA, "wb");
	if( fpNet == NULL) {
		printf("can't open net data file\n");
		fclose(fpRaw);
		free(dummybuf);
		free(ptmp);
		return(0);
	}


	if ( fseek(fpRaw, 0, SEEK_END) == -1 ) {
		perror("fseek failed");
		fclose(fpRaw);
		fclose(fpNet);
		free(dummybuf);
		free(ptmp);
		return(0);
	}
	fseek(fpRaw, -12L, SEEK_CUR);
	fread( &used_channel, 4, 1, fpRaw );
	fread( &dmaBlock_size, 4, 1, fpRaw );
	fread( &dma_cnt, 4, 1, fpRaw );

	fprintf(stdout, "\ndma block: %dKB, dma count: %d, ch: %d \n", dmaBlock_size/1024, dma_cnt, used_channel);

	if( (dma_cnt < 1) || (used_channel < 1))
	{
		printf("raw file have some error\n");
		fclose(fpRaw);
		free(dummybuf);
		free(ptmp);
		return (0);
	}
	if( used_channel > 255 )
	{
		printf("max to 255 ch\n");
		fclose(fpRaw);
		free(dummybuf);
		free(ptmp);
		return(0);
	}

	/*
// ch * 4 byte : 한번 샘플링 할때 파일 크기
// dmaBlock_size*dma_cnt : 전체 파일 크기
*/
	sampleCnt = (dmaBlock_size*dma_cnt) / (used_channel*4);

	fprintf(stdout, "\n>>> sampleingCnt= %d \n", sampleCnt);

	fseek(fpRaw, 0, SEEK_SET);


	for (i=0; i < sampleCnt*used_channel; i++) {
		fread( &int32TempBuf, 4, 1, fpRaw );

		memcpy(ptmp, &int32TempBuf, 4);
		if( ptmp[3] == 0 ) {
			fprintf(stdout, "%d : meet first channel  \n", i);
			cPrev = ptmp[3];
			nStart = i;
			nTemp = i;
			break;
		}
	}
	if ( fseek(fpRaw, -4L, SEEK_CUR) == -1 ) {
		perror("fseek failed");
		fclose(fpRaw);
		free(ptmp);
		free(dummybuf);
		return(0);
	}

	for (i=0; i < sampleCnt-1; i++) {

		if( fread( &int32TempBuf, 4, 1, fpRaw ) != 1) {
			fprintf(stdout, "error hit: %d\n", i);
			fclose(fpRaw);
			free(ptmp);
			free(dummybuf);
			return 0;
		}
		memcpy(ptmp, &int32TempBuf, 4);
		if( ptmp[3] != cPrev ) {
			nStop = (i*used_channel)+nTemp;
			fprintf(stdout, "error hit: cnt: %d  \n\n", i);
			fclose(fpRaw);
			free(ptmp);
			free(dummybuf);
			return 0;
			nStart = (i*used_channel)+nTemp;
			cPrev = ptmp[3];
		}
		fwrite( &int32TempBuf, 4, 1, fpNet );

		if( fread( dummybuf, 4, (used_channel-1), fpRaw ) != (used_channel-1) ) 
		{
			fprintf(stdout, "error hit: %d\n", i);
			fclose(fpRaw);
			free(ptmp);
			free(dummybuf);
			return 0;
		}
		fwrite( dummybuf, 4, (used_channel-1), fpNet );
		g_sampleDataCnt++;
	}

	fclose(fpRaw);
	fclose(fpNet);
	free(ptmp);
	free(dummybuf);

	return 1;

}
#if 0
int sendEDtoTreeFromBigFile(int shot)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */

/*	int len; *//* The dimension of X and Y arrays */
	int i, j, ch, ntemp=0;
	int int32TempBuf=0;
	FILE *fp;
	char buf[64], buf1[32];

	char bufLineScan[512];
	char strTag[32];
	char treeIPport[32];
	char treeName[32];

/* just for test .............................. */
/*	g_sampleDataCnt =10; */
/* just for test .............................. */

	data = (float *)malloc(sizeof(float)*g_sampleDataCnt);
/*	timebase = (float *)malloc(sizeof(float)*g_sampleDataCnt);  */

	
	if( (fp = fopen("/root/ioc/config/mdsplus.cfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can't Read modelTreeIP file\n");
		return(0);
	}
	while(1) {
		if( fgets(bufLineScan, 128, fp) == NULL ) break;
		if( bufLineScan[0] == '#') ;
		else {
			sscanf(bufLineScan, "%s %s", buf, buf1);
			if( strcmp(buf, "IP") == 0) {
				strcpy(treeIPport, buf1);
				fprintf(stdout, "MDS server = %s\n", treeIPport );
			} else if( strcmp(buf, "TREE") == 0) {
				strcpy(treeName, buf1);
				fprintf(stdout, "Tree name = %s\n", treeName );
			}
		}
	}
	fclose(fp);



	/* Connect to MDSplus */
	socket = MdsConnect(treeIPport);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", treeIPport);
		free(data);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);
	
	/* Open tree */
	status = MdsOpen(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(data);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsOpen(\"%s\", shot: %d)... OK\n", TREE_NAME, shot);



	/* Open raw file */
	fp = fopen(FILE_NAME_NETDATA, "rb");
	if( fp == NULL) {
		fprintf(stderr, "can't oepn data file : %s\n", FILE_NAME_NETDATA);
		return(0);
	}


	for(i = 0; i<used_channel; i++) 
	{
		if ( fseek(fp, (i*4), SEEK_CUR) == -1 )
		{
			perror("fseek failed");
			return(0);
		}
		for(j = 0; j < g_sampleDataCnt; j++)
		{
			fread( &int32TempBuf, 4, 1, fp );
			data[j] = ((float)(int32TempBuf >> 8) * 5.0 ) / 8388608.0;
			swap32(&data[j]);


/*			timebase[j] = j; */ /*Build a signal which lasts 1 sec */
			if ( fseek(fp, 4*(used_channel-1), SEEK_CUR) == -1 )
			{
				perror("fseek failed");
				return(0);
			}

		}
		fseek(fp, 0, SEEK_SET);

		dataDesc = descr(&dtypeFloat, data, &g_sampleDataCnt, &null);
/*		timeDesc = descr(&dtypeFloat, timebase, &g_sampleDataCnt, &null); */
		/*fprintf(stdout, "%s\n", channelTagName[i] );*/
/*		status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null); */
		status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &null);
		if ( !statusOk(status) )
		{
			/* error */
			fprintf(stderr,"Error writing signal: %s\n", MdsGetMsg(status));
			free(data);
			return 0;
		}
		fprintf(stdout,"ch%d .", i );

		if( ntemp > 10) {
			fprintf(stdout,"\n");
			ntemp = 0;
		}
		ntemp++;
	} /* for(i = 0; i<used_channel; i++)  */
/*	fprintf(stdout,"\nMDSplus >>> sampling count :  %d\n", g_sampleDataCnt);	*/
	fprintf(stdout,"\nDone\n");
	fclose(fp);


	
	/* Close tree */
	status = MdsClose(TREE_NAME, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(data);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> Data size: %dKB/ch\n", (4*g_sampleDataCnt)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	/* Done */

	free(data);
/*	free(timebase); */

	return 1;

}
#endif


int createNewShot(int shot)
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */

	char buf[512], buf1[32];

	char bufLineScan[512];
	char treeIPport[32];
	char treeName[32];

	FILE *fp;


	int tstat, len;
	int idesc = descr(&dtypeLong, &tstat, &null);

/*************************************************************************/
/*************************************************************************/
	if( (fp = fopen("/root/ioc/config/mdsplus.cfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can't Read modelTreeIP file\n");
		return(0);
	}
	while(1) {
		if( fgets(bufLineScan, 128, fp) == NULL ) break;
		if( bufLineScan[0] == '#') ;
		else {
			sscanf(bufLineScan, "%s %s", buf, buf1);
			
			if( strcmp(buf, "LOCAL_IP") == 0) {
				strcpy(treeIPport, buf1);
				fprintf(stdout, "Local MDS server = %s\n", treeIPport );
			} else if( strcmp(buf, "LOCAL_TREE") == 0) {
				strcpy(treeName, buf1);
				fprintf(stdout, "Local tree name = %s\n", treeName );
			} 
		}
	}
	fclose(fp);
/*************************************************************************/
/*************************************************************************/

	/* Connect to MDSplus */
	socket = MdsConnect(treeIPport);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", treeIPport);
		free(data);
		free(timebase); 
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);

#if 1
	sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
/*	status = MdsValue("TCL(\"set tree dds2lr/shot=-1\")", &idesc, &null, &len); */
	status = MdsValue(buf, &idesc, &null, &len); 
	if( !statusOk(status) )
	{
		fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
		free(data);
		free(timebase); 
		return 0;
	}
	sprintf(buf, "TCL(\"create pulse %d\")", shot);
	status = MdsValue(buf, &idesc, &null, &len);
	if( !statusOk(status) )
	{
		fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
		free(data);
		free(timebase); 
		return 0;
	}
	status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
/*	status = MdsValue("TCL(\"exit\")", &idesc, &null, &len); */

	fprintf(stdout,"\nMDSplus >>> Create new tree(\"%d\")... OK\n", shot);

#endif

	/* Close tree */
	status = MdsClose(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		return 0;
	}
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	
	return 1;
}
