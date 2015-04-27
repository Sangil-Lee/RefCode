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
#include "dbAccess.h"

#define DEBUG_FILE_CONV		1

/* An example of Data readout using the MDSplus Data Access library (mdslib) */
int dtypeFloat = DTYPE_FLOAT; /* We are going to read a signal made of float values */
int dtypeLong = DTYPE_LONG; /* The returned value of SIZE() is an	integer */
int dtypeString = DTYPE_CSTRING;
int dtypeShort = DTYPE_SHORT;

/* float *data;  Array of floats used for signal */
int *data; /*  Array of floats used for signal */
float *dataWong; /* Array of floats used for signal */
float *timebase;   /* Array of floats used for timebase */
int dataDesc; /* Signal descriptor */
int timeDesc; /* Timebase descriptor */
int sampleDesc, rateDesc, startDesc, endDesc, gapDesc, hzDesc;     /* Sample Num, Sample Rate, Data Start Time  */
int range10Desc, range5Desc; /* Voltage Range Select */
int offsetDesc, gainDesc; /* Voltage Range Select */
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
	dataWong = (float *)malloc(size * sizeof(float));
	if ( !dataWong )
	{
		fprintf(stderr,"Error allocating memory for data\n");
		return 0;
	}
/*	timebase = (float *)malloc(size * sizeof(float)); 
	if ( !timebase )
	{
		fprintf(stderr,"Error allocating memory for timebase\n");
		free( (void *)dataWong );
		return 0;
	}
*/
	
	/* create a descriptor for this signal */
/*	dataDesc = descr(&dtypeFloat, dataWong, &size, &null);  */
	
	/* create a descriptor for the timebase */
/*	timeDesc = descr(&dtypeFloat, timebase, &size, &null); */
	

	/* retrieve signal */
	status = MdsValue(NODE_NAME, &dataDesc, &null, &retLen );
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal: %s\n", MdsGetMsg(status));
		free( (void *)dataWong );
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
		free( (void *)dataWong );
/*		free( (void *)timebase ); */
		return 0;
	}
	
	/* do whatever with the data */
	/* print ALL the data */
	for ( i = 0 ; i < size ; i++ )
	{
/*		printf("%i X:%f Y:0x%X\n", i, timebase[i], dataWong[i]); */
		if( i > 20 ) break;
	}

	/* Close tree */
	status = MdsClose(TREE_NAME, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free( (void *)dataWong );
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> Tree close ...  OK\n" );
	
	
	/* free the dynamically allocated memory when done */
	free( (void *)dataWong );
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

int dataChannelizationAndSendData(int shot, int mode)
{
/* Data Channelization Part  */
	FILE *fpData[256], *fpRaw, *fpConfig;
	unsigned int i, j, ntemp=0, ntemp2=0;
	int dma_cnt, dmaBlock_size;
	int int32TempBuf=0;
/*	int int32TempBuf2=0;  */
	int int32TempBuf96[96]; /* All Channel 96 Channel  */
	int int32TempBuf3[30]; /* All Channel 96 Channel  */
/*	unsigned int sampleCnt;  100 sec Over Time at Error ... Maybe... TG.Lee 20081202 */
	unsigned long long sampleCnt;
	unsigned char *cptr;
	char buf[512], buf1[32], buf2[128];

	cptr = (unsigned char*)malloc(4);

	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */

	int onceOpen;
	int tstat, len;
	int idesc = descr(&dtypeLong, &tstat, &null);

	int sampleReCount;
	int fileSampleCnt;
	int numSamples;
	int nCnt;
	int isFirstNode = 1;
	int nSkipCnt = 0;

	char bufLineScan[512];
	char treeIPport[32];
	char treeName[32];
	char treeName2[32];

/*	double dTimeGap = 0.0; */
	int nTimeGap=0;
	float fT0 = 0.;
	float fT1 = 0.;
	int nSampleClk;

			

/* Add TG.Lee MDSplus Blip Time Infomation Variable 20081025 */
	float blip; 
/* Add TG.Lee MDSplus Time Array Infomation Variable */
	float startTime;   /* -1 sec */
	float endTime;     /* -1 sec */
	int samHz;
	float realTimeGap;
	float range10, range5;

	int readStatus;	
	memset(int32TempBuf96, 0, sizeof(int32TempBuf96));
	memset(int32TempBuf3, 0, sizeof(int32TempBuf3));       
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
		sprintf(buf2, "/data/ch%02d.dat", i);
#if 0
		fprintf(stdout, "%s\n", buf2);
#endif
		fpData[i] = fopen(buf2, "w+b");
		if( fpData[i] == NULL) {
			printf("can't make data file\n");
			return(0);
		}
	}


	/* **  ch * 4 byte : byte per one sample clock ,  dmaBlock_size*dma_cnt : total file size ***** */
	sampleCnt = ((unsigned long long)dmaBlock_size*(unsigned long long)dma_cnt) / (used_channel*4);

		/*	fprintf(stdout, "\n>>> sampleingCnt unsigned long long = %d", sizeof(unsigned long long));
			fprintf(stdout, "\n>>> sampleingCnt unsigned int = %d", sizeof(unsigned int));
			fprintf(stdout, "\n>>> sampleingCnt double = %d", sizeof(double));
			fprintf(stdout, "\n>>> sampleingCnt= %u   %fl\n", sampleCnt, (double)sampleCnt);
			fprintf(stdout, "\n>>> sampleingCnt ABS = %fl \n", fabs((double)sampleCnt));
		*/
	g_sampleDataCnt = sampleCnt;
 
	drvM6802_taskConfig *ptaskConfig = drvM6802_find_taskConfig("master");

/*	data = (float *)malloc(sizeof(float)*g_sampleDataCnt);     
	data = (int *)malloc(sizeof(int)*g_sampleDataCnt);   
	data = (float *)malloc(4*g_sampleDataCnt);   Data Size is int 4byte But float is 8Byte */
/*	memset(data, 0, sizeof(float)*g_sampleDataCnt);  */

	nSampleClk = ptaskConfig->appSampleRate;
/* Add TG.Lee MDSplus Blip Time Infomation Variable  20081025 */
	blip = ptaskConfig->blipTime;

/*	nSkipCnt is Data Skip Count. Down Sampling after Max Sample Acquisition Data    */
	switch(nSampleClk) {
	case 1: nSkipCnt = 25 / nSampleClk; break;      /* Skip Count 25 */
	case 2: nSkipCnt = 50 / nSampleClk; break;      /* Skip Count 25 */
	case 5: nSkipCnt = 25 / nSampleClk; break;      /* Skip Count 5  */
	case 10: nSkipCnt = 50 / nSampleClk; break;     /* Skip Count 5  */
	case 20: nSkipCnt = 100 / nSampleClk; break;    /* Skip Count 5  */
	case 25: nSkipCnt = 25 / nSampleClk; break;     /* Skip Count 1 */
	case 50: nSkipCnt = 50 / nSampleClk; break;     /* Skip Count 1 */
	case 100: nSkipCnt = 100 / nSampleClk; break;   /* Skip Count 1 */
	default: {
			fprintf(stderr,"appSampleRate not valid\n");
		/*	free(data); */
			return 0;
			}
	}
	fprintf(stdout, "\nMDSplus >>> Sample clock: %dKHz, nTimeGap: %d, nSkipCnt:%d\n", nSampleClk, nTimeGap, nSkipCnt );

/*************************************************************************/
/************ MDSplus Server Information File ****************************/
	if( (fpConfig = fopen("/usr/local/epics/siteApp/config/mdsplus.cfg", "r") ) == NULL )
	{	
		fprintf(stderr, "Can't Read mdsplus.cfg file\n");
		return(0);
	}
	while(1) {
		if( fgets(bufLineScan, 128, fpConfig) == NULL ) break;
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
				} else if( strcmp(buf, "REMOTE_TREE2") == 0) {
					strcpy(treeName2, buf1);
					fprintf(stdout, "Remote tree name2 = %s\n", treeName2 );
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
				fclose(fpConfig);
				return 0;
			}

		}
	}
	fclose(fpConfig);
/*************************************************************************/
/*************************************************************************/

/*********  Connect to MDSplus ****************************************** */
	socket = MdsConnect(treeIPport);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", treeIPport);
	/*	free(data); */
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);

#if 1
	if( mode != OPMODE_REMOTE ) {
		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
		status = MdsValue(buf, &idesc, &null, &len); 
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
		/*	free(data);  */
			return 0;
		}
		sprintf(buf, "TCL(\"create pulse %d\")", shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
		/*	free(data); */
			return 0;
		}
		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
/*		status = MdsValue("TCL(\"exit\")", &idesc, &null, &len); */

		fprintf(stdout,"\nMDSplus >>> Create new tree(\"%d\")... OK\n", shot);
	}
#endif
	 if( /* ptaskConfig->clockSource */ 1 )
        {
		fT0 = ptaskConfig->fT0;
		fT1 = ptaskConfig->fT1;
		fprintf(stdout, "\nMDSplus >>> Used external clock, nSkipCnt = %d, T0=%f, T1=%f\n",  nSkipCnt, fT0, fT1);

	printf("\nptaskConfig->clockSource: %d\n", ptaskConfig->clockSource);
	}	
	
/**********  Open tree *************************************************************/
	status = MdsOpen(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
	/*	free(data);  */
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsOpen(\"%s\", shot: %d)... OK\n", treeName, shot);


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
/* TG.Lee ******** Data Channelization  Start     */
	nCnt=0;
	fileSampleCnt = 0;
	isFirstNode = 1;
	sampleReCount = 0;

	epicsPrintf("Now, do channelization\n");
	epicsPrintf("Now, data size int: %d short : %d float :%d double :%d \n",sizeof(int),sizeof(short),sizeof(float),sizeof(double));

	for (i=0; i < sampleCnt; i++) 
	{
		readStatus = fread( &int32TempBuf96, 4, 96, fpRaw );
		if(readStatus != 0){
			nCnt++;
			if( isFirstNode ) {
				for( j=0; j< used_channel; j++){ 
					if( channelOnOff[j] ) {
						int32TempBuf96[j] >>= 8; /* 10Volt Module */
						fwrite( (void*)(int32TempBuf96+j), 4, 1, fpData[j] );
					}
				}
				isFirstNode = 0;
				sampleReCount++;
			}
			if( nCnt > (nSkipCnt-1)) {
				nCnt = 0;
				fileSampleCnt++;
				isFirstNode = 1;
			}
		} else {
	                epicsPrintf("\n>>> MDSplus_Data_Channelization : dataChannelizationAndSendData() ... fail\n");
		}
	}  /* Data Channelization End. total sample count Loop */
	free(cptr);
	fclose(fpRaw);

/*	data = (float *)malloc(sizeof(float)*g_sampleDataCnt);     */
/*	data = (float *)malloc(4*g_sampleDataCnt);   Data Size is int 4byte But float is 8Byte */
/*	memset(data, 0, sizeof(float)*g_sampleDataCnt);  */
	data = (int *)malloc(sizeof(int)*sampleReCount);   

	epicsPrintf("\n>>> LocalStorage : dataChannelization() ... DONE\n");
	/* Sample Data count check T.G.Lee */
	fprintf(stdout, "Globle_DMA_Sample Data Count : %llu ==?? File Sample Cnt : %d ==?? RealSample Cnt : %d\n", g_sampleDataCnt, fileSampleCnt,sampleReCount);


	numSamples = sampleReCount -1;
	/* Modify Add - Local & Cal Mode is Data Store Start Time is Zero. Remote Mode is Data Store Start Time is BlipTime  */
	if (!(mode == OPMODE_REMOTE)) {
	/*	startTime = 0;   	Data Zero Start Time */
		startTime = -1;  	/* Data Zero Start Time */
		endTime = numSamples;		 /* endTime = sampleReCount;  Last data Zero Over Count.  */
		/*fprintf(stdout,"\nMDSput Data Blip Time = %f  .... OK??\n",blip);  Test */ 
	} else { 
		startTime = (fT0 - blip) * nSampleClk * 1000 - 1;  /* "-1" Value is Data Start Time ..... */
					/* endTime = (sampleReCount-1) + startTime;   -1 sec */
		endTime = numSamples + startTime -1;  /* -1 sec */
	}
	/* float startTime = nSampleClk * 1000 * -1;   -1 sec */
	samHz = nSampleClk * 1000;
	realTimeGap = 1.0 / samHz;
	range10 = 10. / 8388608.0;
	range5 = 5. / 8388608.0;	
	
	range10Desc = descr(&dtypeFloat, &range10, &null);
	range5Desc = descr(&dtypeFloat, &range5, &null);
/*	sampleDesc = descr(&dtypeFloat, &sampleReCount, &null);   Last Data Zero .. .. Errro  */
	sampleDesc = descr(&dtypeFloat, &numSamples, &null);   
	gapDesc = descr(&dtypeFloat, &nTimeGap, &null); 
	hzDesc = descr(&dtypeFloat, &samHz, &null); 
	rateDesc = descr(&dtypeFloat, &realTimeGap, &null); 
	startDesc = descr(&dtypeFloat, &startTime, &null); 
	endDesc = descr(&dtypeFloat, &endTime, &null); 

	fprintf(stderr,"Read Function Count start \n ");
/*  Data File Read Error Test at 2008.12.01  - TG.Lee
	fseek(fpData[0], 0, SEEK_SET);
	for(j=0; j<30; j++){
		readStatus = fread(&int32TempBuf2, 4, 1, fpData[0] );  
		fprintf(stderr,"Read Function Ch 0 Data 0~29 %d ch :0x12%x \n",j, int32TempBuf2);
	}
*/			
	epicsPrintf("Now, transfer to local model tree\n");	


/*  Remote Mode 3Module data put at other tree  */
	onceOpen=1; 
/* TG.Lee ******** Data MDSplus PUT  Start     */
	for(i=0; i<used_channel; i++){
		if( channelOnOff[i]) {
		fseek(fpData[i], 0, SEEK_SET);
/*		readStatus = fread(data, 4*(g_sampleDataCnt-1),1, fpData[i] ); */ 
/*		readStatus = fread(data, 4,(g_sampleDataCnt-1), fpData[i] );    */
		readStatus = fread(data, 4,(sampleReCount-1), fpData[i] );  
/*  Data File Read Error Test at 2008.12.01  - TG.Lee
		if(i==0){
			for(j=0; j<30; j++){
				fprintf(stderr,"Data Read Function Ch 0 Data 0~29 %d ch :0x12%x \n",j, data[j]);
			}
			fseek(fpData[i], 0, SEEK_SET);
			readStatus = fread(&int32TempBuf3, 4, 30, fpData[0]); 
			for(j=0; j<30; j++){
				fprintf(stderr,"Temp30 Read Function Ch 0 Data 0~29 %d ch :0x12%x \n",j, int32TempBuf3[j]);
			}
		}
*/
/*		readStatus = fread(&int32TempBuf3, 4, 30, fpData[0]);   */
/*
		for(j=0; j<g_sampleDataCnt;j++){
		fread(&int32TempBuf, 4, 1, fpData[i] );
		data[j]= int32TempBuf;
		}
*/
		if(readStatus !=0){
			if(mode == OPMODE_REMOTE && onceOpen==1){
				status = MdsClose(treeName, &shot);
				if ( !statusOk(status) )
				{
					fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
				}
				fprintf(stdout,"\nMDSplus >>> MdsClose(\"%s\", shot: %d)... OK\n", treeName, shot);
				status = MdsOpen(treeName2, &shot);
				if ( !statusOk(status) )
				{
					fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
					return 0;
				}
				onceOpen=0;
				fprintf(stdout,"\nMDSplus >>> MdsOpen(\"%s\", shot: %d)... OK\n", treeName2, shot);
			}
			dataDesc = descr(&dtypeLong, data, &numSamples, &null);    
			offsetDesc = descr(&dtypeFloat, &channelOffsetVal[i], &null);
			gainDesc = descr(&dtypeFloat, &channelGainVal[i], &null);
			status = MdsPut(channelTagName[i], "BUILD_SIGNAL(BUILD_WITH_UNITS(($*$-$)/$,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))",&range10Desc, &dataDesc,&offsetDesc, &gainDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
			if ( !statusOk(status) )
			{
				fprintf(stderr,"Error writing signal ch%d : %s\n", i, MdsGetMsg(status));
			}
			fprintf(stdout,"ch%d .", i );

			if( ntemp > 10) {
				fprintf(stdout,"\n");
				ntemp = 0;
			}
			ntemp++;
		}else {
			epicsPrintf("\n>>>MDSplus_Data_LocalStorage : MDSput ... fail\n");
		}
		} else {
			fprintf(stdout, ". ch%d off ", i);
			if( ntemp2 > 10) {
				fprintf(stdout,"\n");
				ntemp2 = 0;
			}
			ntemp2++;
		}
	}
	/* TG.Lee ******** Data MDSplus Put Function  End     */

	fprintf(stdout, "cvTask >>> total sampling count : %d\n", sampleReCount);

	for( i=0; i< used_channel; i++) {
		fclose(fpData[i]);
	}

/* ********* End Data Put And Channelization ********** Close MDSplus Tree */

	fprintf(stdout,"\n1st and 3nd 5Volt Module, 2nd Module 10Volt Module");
	fprintf(stdout,"\nDone\n");
	fprintf(stdout,"\nEdit Version 2.0\n");
	
#if 0
	float dataT[5000], timeT[5000]; /* Array of floats used for signal */
        int dataTDesc, timeTDesc; /* Signal descriptor */
	int jt;
	int testCount;
	testCount = 5000;
	for(jt=0; jt<5000; jt++){
                        dataT[jt]=(float)jt * 0.0025;
                        timeT[jt]=(float)jt/5000;
                }
                epicsPrintf("\n");
                epicsThreadSleep(0.5);
		epicsPrintf("Now, data Print ....\n");	
                epicsPrintf("\n");
                for(jt=5000 -10; jt<5000; jt++){
                       epicsPrintf("  %f, ",dataT[jt]);
                }
	dataTDesc = descr(&dtypeFloat,dataT, &testCount, &null);
        timeTDesc = descr(&dtypeFloat,timeT, &testCount, &null);

	status = MdsPut("\\MC1P13:FOO", "BUILD_SIGNAL($,,$)", &dataTDesc, &timeTDesc, &null);
#endif
	/* Close tree */
	if(mode == OPMODE_REMOTE){
		status = MdsClose(treeName2, &shot);
	} else status = MdsClose(treeName, &shot); 
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, MdsGetMsg(status));
	}
	fprintf(stdout,"\nMDSplus >>> Data size: %dKB/ch\n", (4*sampleReCount)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	epicsPrintf("\n>>> devM6802_set_LocalStorage : sendEDtoTreeFromChannelData() ... DONE\n");
	/* Done */

	free(data);

	MdsDisconnect(treeIPport);

	if(mode == OPMODE_REMOTE){
		status = completeDataPut();
	} 

	return 1;
}

/******************************************/
int completeDataPut()
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */

	char buf[512];

	int tstat, len;
	int idesc = descr(&dtypeLong, &tstat, &null);


	/* Connect to MDSplus */
	socket = MdsConnect("172.17.100.200:8300");
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server(%s).\n", "172.17.100.200:8300");
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", "172.17.100.200:8300");

	sprintf(buf, "TCL(\"setevent ECEHR\")");
	status = MdsValue(buf, &idesc, &null, &len);
	if( !statusOk(status) )
	{
		fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
		free(dataWong);
		free(timebase); 
		return 0;
	}
	MdsDisconnect("172.17.100.200:8300");	
	return 1;
}

int channelAverageForECE()
{
	int i, j;
	int int32TempBuf=0;
	int int32TempBufStd=0;

	int dma_cnt, dmaBlock_size;
	FILE *fpData[96];
	FILE *fpDataAvr[96];
	char buf1[32], buf2[32], strBuf[256];

	unsigned long long sampleCnt;
	int nCnt;
	int index;
	float fTemp = 0.0;
	float fTempStd = 0.0;
	float fTempSum = 0.0;
	float fTempAvr = 0.0;

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


	/* **  ch * 4 byte : byte per one sample clock ,  dmaBlock_size*dma_cnt : total file size ***** */
	sampleCnt = ((unsigned long long)dmaBlock_size*(unsigned long long)dma_cnt) / (used_channel*4);

		/*	fprintf(stdout, "\n>>> sampleingCnt unsigned long long = %d", sizeof(unsigned long long));
			fprintf(stdout, "\n>>> sampleingCnt unsigned int = %d", sizeof(unsigned int));
			fprintf(stdout, "\n>>> sampleingCnt double = %d", sizeof(double));
			fprintf(stdout, "\n>>> sampleingCnt= %u   %fl\n", sampleCnt, (double)sampleCnt);
			fprintf(stdout, "\n>>> sampleingCnt ABS = %fl \n", fabs((double)sampleCnt));
		*/
	g_sampleDataCnt = sampleCnt;
/*  Read Channl Data for ECE Calibration..Change  used_channel -> 48ch ....  */


	for( i=0; i < 49; i++) 
	{
		sprintf(buf1, "/data/ch%02d.dat", i);
		fpData[i] = fopen(buf1, "r");
		if( fpData[i] == NULL) {
			printf("can't make data file\n");
			return(0);
		}
	}
/* Open Channel Average Data for ECE Calibration  */	
	for( i=0; i < 48; i++) 
	{
		sprintf(buf2, "/data/avrch%02d.dat", i);
		fpDataAvr[i] = fopen(buf2, "w+b");
		if( fpDataAvr[i] == NULL) {
			printf("can't make Avr data file\n");
			return(0);
		}
	}

	for(i = 0; i<48; i++) 
	{
		if( channelOnOff[i] ) {
			nCnt=0;  /* nCnt for Average  */
			fTempSum = 0.0;   /* Sum buffer  */
			index = 0;
			fseek(fpData[i], 0, SEEK_SET);
			fseek(fpData[48], 0, SEEK_SET);
			for(j = 0; j < g_sampleDataCnt; j++)
			{
				fread( &int32TempBuf, 4, 1, fpData[i]);
				fread( &int32TempBufStd, 4, 1, fpData[48]);
				fTemp = ((float)(int32TempBuf >> 8) * 10.0 ) / 8388608.0; /* 10Volt Module */
				fTempStd = ((float)(int32TempBufStd >> 8) * 10.0 ) / 8388608.0; /* 10Volt Module */
				
				if( fTempStd > 1.5 ) {	
					fTempSum += fTemp;
					nCnt++;
					fprintf(stdout,"ch data signal fTempStd >1.5 .");
				}
				if( fTempStd <1.5 && nCnt > 1) {
					fTempAvr = fTempSum / nCnt;
					sprintf(strBuf, "index :%d, nCnt : %d, Avr :%f \n",index, nCnt, fTempAvr);	
					fwrite( (void*)(strBuf), sizeof(strBuf), 1, fpDataAvr[i] );
					fTempSum =0.0;
					nCnt =0;
					fTempAvr =0.0;
					index ++;
					fprintf(stdout,"ch data zero fTempStd <1.5 .");
				}
			}
			fclose(fpData[i]);
			fclose(fpDataAvr[i]);
			fprintf(stdout,"ch%d .", i );

		} else {
			fprintf(stdout, ". ch%d off ", i);
		}
	} 
	fclose(fpData[48]);
	
	fprintf(stdout,"\nMDSplus >>> Data size: %lluKB/ch\n", (4*g_sampleDataCnt)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );

	return 1;
}

#if 0
int sendEDtoTreeFromChannelData(int shot, int mode)
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
	float fFirstNodeData = 0.0;		/* 2008. 06. 27  woong */
	int isFirstNode = 1;
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

/* Add TG.Lee MDSplus Blip Time Infomation Variable 20081025 */
	float blip; 
/* Add TG.Lee MDSplus Time Array Infomation Variable */
	float startTime;   /* -1 sec */
	float endTime;     /* -1 sec */
	int samHz;
	float realTimeGap;


/* just for test .............................. */
/*	g_sampleDataCnt =10; */
/* just for test .............................. */

	drvM6802_taskConfig *ptaskConfig = drvM6802_find_taskConfig("master");

	dataWong = (float *)malloc(sizeof(float)*g_sampleDataCnt);
	timebase = (float *)malloc(sizeof(float)*g_sampleDataCnt); 
	unTimebase = (unsigned int *)malloc(sizeof(unsigned int)*g_sampleDataCnt);

	
	
	nSampleClk = ptaskConfig->appSampleRate;
/* Add TG.Lee MDSplus Blip Time Infomation Variable  20081025 */
	blip = ptaskConfig->blipTime;

/*	dTimeGap = 1.0 / (double)(nSampleClk*1000 ); */
	switch(nSampleClk) {
	case 1: nTimeGap = 1000; break;
	case 2: nTimeGap = 500; break;
	case 5: nTimeGap = 200; break;
	case 10: nTimeGap = 100; break;
	case 20: nTimeGap = 50; break;
	case 50: nTimeGap = 20; break;
	case 100: nTimeGap = 10; break;
	case 200: nTimeGap = 5; break;  /* Add TG.Lee 08.1121 200KHz Test */
	default: {
			fprintf(stderr,"appSampleRate not valid\n");
			free(dataWong);
			free(timebase);
			free(unTimebase);
			return 0;
			}
	}

/*	nSkipCnt is Data Skip Count. Down Sampling after Max Sample Acquisition Data    */
	switch(nSampleClk) {
	case 1: nSkipCnt = 25 / nSampleClk; break;      /* Skip Count 25 */
	case 2: nSkipCnt = 50 / nSampleClk; break;      /* Skip Count 25 */
	case 5: nSkipCnt = 25 / nSampleClk; break;      /* Skip Count 5  */
	case 10: nSkipCnt = 50 / nSampleClk; break;     /* Skip Count 5  */
	case 20: nSkipCnt = 100 / nSampleClk; break;    /* Skip Count 5  */
	case 25: nSkipCnt = 25 / nSampleClk; break;     /* Skip Count 1 */
	case 50: nSkipCnt = 50 / nSampleClk; break;     /* Skip Count 1 */
	case 100: nSkipCnt = 100 / nSampleClk; break;   /* Skip Count 1 */
	default: {
			fprintf(stderr,"appSampleRate not valid\n");
			free(dataWong);
			free(timebase);
			free(unTimebase);
			return 0;
			}
	}
/*	nSkipCnt = 50 / nSampleClk;    Add TG.Lee 08.1121 50KHz tes */
/*	nSkipCnt = 200 / nSampleClk;    Add TG.Lee 08.1121 200KHz tes */
/*	nSkipCnt = 100 / nSampleClk;  Max 100KHz ECEHR Default  */
	fprintf(stdout, "\nMDSplus >>> Sample clock: %dKHz, nTimeGap: %d, nSkipCnt:%d\n", nSampleClk, nTimeGap, nSkipCnt );


/*************************************************************************/
/*************************************************************************/
	if( (fp = fopen("/usr/local/epics/siteApp/config/mdsplus.cfg", "r") ) == NULL )
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
		free(dataWong);
		free(timebase); 
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);

#if 1
	if( mode != OPMODE_REMOTE ) {
		sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
		status = MdsValue(buf, &idesc, &null, &len); 
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
			free(dataWong);
			free(timebase); 
			return 0;
		}
		sprintf(buf, "TCL(\"create pulse %d\")", shot);
		status = MdsValue(buf, &idesc, &null, &len);
		if( !statusOk(status) )
		{
			fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
			free(dataWong);
			free(timebase); 
			return 0;
		}
		status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
/*		status = MdsValue("TCL(\"exit\")", &idesc, &null, &len); */

		fprintf(stdout,"\nMDSplus >>> Create new tree(\"%d\")... OK\n", shot);
	}
#endif
	 if( /* ptaskConfig->clockSource */ 1 )
        {
		fT0 = ptaskConfig->fT0;
		fT1 = ptaskConfig->fT1;
	/*		nSkipCnt = 1; */
		fprintf(stdout, "\nMDSplus >>> Used external clock, nSkipCnt = %d, T0=%f, T1=%f\n",  nSkipCnt, fT0, fT1);

	printf("\nptaskConfig->clockSource: %d\n", ptaskConfig->clockSource);
	}	
	
	/* Open tree */
	status = MdsOpen(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(dataWong);
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
			isFirstNode = 1;
/* Modify +/- 5Volt => +/- 10Voltag Module Changed and edit Program by TGLee at 20080425 : second Slot install  */
			if( 32 <= i && i < 64)
			{	
				for(j = 0; j < g_sampleDataCnt; j++)
				{
					fread( &int32TempBuf, 4, 1, fp );
					fTemp = ((float)(int32TempBuf >> 8) * 10.0 ) / 8388608.0; /* 10Volt Module */
				/*	fTemp = ((float)(int32TempBuf) * 10.0 ) / 8388608.0;  10Volt Module */
					if( isFirstNode ) {
						fFirstNodeData = fTemp;
						isFirstNode = 0;
					}
	
					fTempStore += fTemp;
					nCnt++;

					if( nCnt > (nSkipCnt-1)) {
/*						dataWong[fileSampleCnt] = fTempStore / (float)nCnt;  when use mean value */
						dataWong[fileSampleCnt] = fFirstNodeData;
#if 0
						timebase[fileSampleCnt] = fileSampleCnt;
#else
						if( fileSampleCnt != 0 )
/*							timebase[fileSampleCnt] = timebase[fileSampleCnt-1] + (float)dTimeGap; */
							unTimebase[fileSampleCnt] = unTimebase[fileSampleCnt-1] + nTimeGap;
						else unTimebase[fileSampleCnt] = 0; /* always start 0 time */
#endif
						if( /* ptaskConfig->clockSource */ (int)(fT1-fT0)) {
							if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) );
						/*	if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) ) break;  Local mode error at Trigger = Period value */
						}
						nCnt = 0;
						fTempStore = 0.0;
						fileSampleCnt++;
						isFirstNode = 1;
					}

				}
				fclose(fp);
			} else {
				for(j = 0; j < g_sampleDataCnt; j++)
				{
					fread( &int32TempBuf, 4, 1, fp );
					fTemp = ((float)(int32TempBuf >> 8) * 5.0 ) / 8388608.0;  /*    5Volt Module */
				/*	fTemp = ((float)(int32TempBuf ) * 5.0 ) / 8388608.0;      5Volt Module */
					if( isFirstNode ) {
						fFirstNodeData = fTemp;
						isFirstNode = 0;
					}
					
					fTempStore += fTemp;
					nCnt++;

					if( nCnt > (nSkipCnt-1)) {
/*						dataWong[fileSampleCnt] = fTempStore / (float)nCnt;  when use mean value */
						dataWong[fileSampleCnt] = fFirstNodeData;
#if 0
						timebase[fileSampleCnt] = fileSampleCnt;
#else
						if( fileSampleCnt != 0 )
/*							timebase[fileSampleCnt] = timebase[fileSampleCnt-1] + (float)dTimeGap; */
							unTimebase[fileSampleCnt] = unTimebase[fileSampleCnt-1] + nTimeGap;
						else unTimebase[fileSampleCnt] = 0; /* always start 0 time */
#endif
						if( /* ptaskConfig->clockSource */ (int)(fT1-fT0)) {
							if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) ) ;
					/*		if( (unTimebase[fileSampleCnt] * 0.0000010) >= (fT1 - fT0) ) break; Local mode error at Trigger = P
eriod value */
						}
						nCnt = 0;
						fTempStore = 0.0;
						fileSampleCnt++;
						isFirstNode = 1;
					}

#if 0
		fprintf(stdout, "%d = %d, 0x%X\n", i, dataWong[i], dataWong[i] ); 
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
/* Modify Add - Local & Cal Mode is Data Store Start Time is Zero. Remote Mode is Data Store Start Time is BlipTime  */
			if (!(mode == OPMODE_REMOTE)) {
				startTime = 0;  /* Data Zero Start Time */
				endTime = fileSampleCnt;
/*				fprintf(stdout,"\nMDSput Data Blip Time = %f  .... OK??\n",blip);  Test */ 
			} else { 
				startTime = (fT0 - blip) * nSampleClk * 1000;
				endTime = fileSampleCnt + startTime;  /* -1 sec */
			}
/*			float startTime = nSampleClk * 1000 * -1;   -1 sec */
			samHz = nSampleClk * 1000;
			realTimeGap = 1.0 / samHz;
	
			dataDesc = descr(&dtypeFloat, dataWong, &fileSampleCnt, &null);
			timeDesc = descr(&dtypeFloat, timebase, &fileSampleCnt, &null); 
			sampleDesc = descr(&dtypeFloat, &fileSampleCnt, &null); 
			gapDesc = descr(&dtypeFloat, &nTimeGap, &null); 
			hzDesc = descr(&dtypeFloat, &samHz, &null); 
			rateDesc = descr(&dtypeFloat, &realTimeGap, &null); 
			startDesc = descr(&dtypeFloat, &startTime, &null); 
			endDesc = descr(&dtypeFloat, &endTime, &null); 
				
/*			fprintf(stdout,"\nMDSPUT Parametor fileSampleCount :%d , Sample Clock : %d , startTime : %f, endTime : %f , realTimeGap : %f ... OK\n", fileSampleCnt,samHz, startTime, endTime, realTimeGap); Delete 20081006 */

/*			status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null); Modify TGLEE 20080623  */
/*			status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,BUILD_WITH_UNITS($2,\"s\"))", &dataDesc, &timeDesc, &null); Good Function Modify TG LEE 20080806 */
/* status = MdsPut(channelTagName[i], "BUILD_SIGNAL($,,MAKE_DIM(MAKE_WINDOW($,$,),MAKE_SLOPE(BUILD_WITH_UNITS($,\"s\"))))", &dataDesc, &startDesc, &sampleDesc, &rateDesc, &null); Bad Function - No Data */
                     /*  status = MdsPut(channelTagName[i], "BUILD_SIGNAL($,,MAKE_DIM(MAKE_WINDOW($,$,),MAKE_SLOPE(BUILD_WITH_UNITS($,\"s\"))))", &dataDesc, &startTime, &fileSampleCnt, &realTimeGap, &null); Bad Function -Segmentation Fault   */
			
/* status = MdsPut(channelTagName[i], "BUILD_SIGNAL($,,MAKE_DIM(MAKE_WINDOW($,,),MAKE_SLOPE(BUILD_WITH_UNITS($,\"s\"))))", &dataDesc,  &sampleDesc, &rateDesc, &null);  Bad Function - Data Reading abort*/
/*                       status = MdsPut(channelTagName[i], "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(-1785,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &dataDesc, &sampleDesc, &hzDesc, &rateDesc, &null); */
/*  status = MdsPut(channelTagName[i], "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(-1785,1002,0.001),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &dataDesc,  &rateDesc, &null);  Good Test  */
                       status = MdsPut(channelTagName[i], "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW($,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", &dataDesc, &startDesc, &endDesc, &rateDesc, &rateDesc, &null);
				
			if ( !statusOk(status) )
			{
				fprintf(stderr,"Error writing signal ch%d : %s\n", i, MdsGetMsg(status));
		/*		free(dataWong);
				return 0;   Passing error channel  */
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
	fprintf(stdout,"\nEdit Version 2.0\n");
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
	/*	free(dataWong);
		free(timebase);
		free(unTimebase);
		return 0;  Pass Error Tag Name */
	}
/*	fprintf(stdout,"\nMDSplus >>> Data size: %dKB/ch\n", (4*g_sampleDataCnt)/1024 );  unsigned int */
	fprintf(stdout,"\nMDSplus >>> Data size: %lluKB/ch\n", (4*g_sampleDataCnt)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	/* Done */

	free(dataWong);
	free(timebase);
	free(unTimebase);

	return 1;
}
#endif 


/******************************************/
int dataChannelization()
{
	FILE *fp[256], *fpRaw;
	unsigned int i, j;
	int dma_cnt, dmaBlock_size;
	int int32TempBuf=0, int32TempData96[96];
	unsigned int sampleCnt;
	unsigned char *cptr;
	char buf[128];

	cptr = (unsigned char*)malloc(4);
	memset(int32TempData96,0,sizeof(int32TempData96));

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
		fread( &int32TempData96, 4, 96, fpRaw ); 
		for( j=0; j< used_channel; j++) 
		{
/*			fread( &int32TempBuf, 4, 1, fpRaw );  */
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
/*			fwrite( &int32TempBuf, 4, 1, fp[j] );  */
/*			int32TempData96[j] = int32TempData96[j] >> 8;  */
			fwrite( &int32TempData96[j], 4, 1, fp[j] );
		} /* for( j=0; j< ch; j++)  */

		g_sampleDataCnt++;

	} /* for (i=0; i < sampleCnt; i++)  */

/*	fprintf(stdout, "cvTask >>> total sampling count : %d\n", g_sampleDataCnt); unsigned int */
	fprintf(stdout, "cvTask >>> total sampling count : %llu\n", g_sampleDataCnt);

	fclose(fpRaw);
	for( i=0; i< used_channel; i++) {
		fclose(fp[i]);
	}

	free(cptr);
/*	status=MdsDisconnect(treeIPport);  */
	

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

	dataWong = (float *)malloc(sizeof(float)*g_sampleDataCnt);
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
		free(dataWong);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);
	
	/* Open tree */
	status = MdsOpen(treeName, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		free(dataWong);
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
			dataWong[j] = ((float)(int32TempBuf >> 8) * 5.0 ) / 8388608.0;
			swap32(&dataWong[j]);


/*			timebase[j] = j; */ /*Build a signal which lasts 1 sec */
			if ( fseek(fp, 4*(used_channel-1), SEEK_CUR) == -1 )
			{
				perror("fseek failed");
				return(0);
			}

		}
		fseek(fp, 0, SEEK_SET);

		dataDesc = descr(&dtypeFloat, dataWong, &g_sampleDataCnt, &null);
/*		timeDesc = descr(&dtypeFloat, timebase, &g_sampleDataCnt, &null); */
		/*fprintf(stdout, "%s\n", channelTagName[i] );*/
/*		status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc, &null); */
		status = MdsPut(channelTagName[i], "BUILD_SIGNAL($1,,$2)", &dataDesc, &null); 
		if ( !statusOk(status) )
		{
			/* error */
			fprintf(stderr,"Error writing signal: %s\n", MdsGetMsg(status));
			free(dataWong);
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
		free(dataWong);
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> Data size: %dKB/ch\n", (4*g_sampleDataCnt)/1024 );
	fprintf(stdout,"MDSplus >>> Tree close ...  OK\n" );
	/* Done */

	free(dataWong);
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
/*	if( (fp = fopen("/usr/local/epics/siteApp/config/mdsplus.cfg", "r") ) == NULL )  */
/*************************************************************************/
	if( (fp = fopen("/usr/local/epics/siteApp/config/mdsplus.cfg", "r") ) == NULL )
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
		free(dataWong);
		free(timebase); 
		return 0;
	}
	fprintf(stdout,"\nMDSplus >>> MdsConnect(\"%s\")... OK\n", treeIPport);

#if 1
	sprintf(buf, "TCL(\"set tree %s/shot=-1\")", treeName);
	status = MdsValue(buf, &idesc, &null, &len); 
	if( !statusOk(status) )
	{
		fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
		free(dataWong);
		free(timebase); 
		return 0;
	}
	sprintf(buf, "TCL(\"create pulse %d\")", shot);
	status = MdsValue(buf, &idesc, &null, &len);
	if( !statusOk(status) )
	{
		fprintf(stderr,"Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
		free(dataWong);
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
