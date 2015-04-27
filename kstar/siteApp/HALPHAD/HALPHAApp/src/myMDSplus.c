#include <errno.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "drvACQ196.h"

//short int16TempPack5461[524256]; same process time about 96ch
int data_Channelization(ST_STD_device *pSTDdev)
{

	ST_ACQ196	*pACQ196	= (ST_ACQ196 *)pSTDdev->pUser;
	int i,j;
	int ch_loop;	
	FILE *fp;
	char buffRaw[128];
	int channelMapping[96];
        int nVal, nTemp;
//	int step,k;
	short int16TempPack[96];
	ch_loop = pACQ196->totalRead;

//	ch_loop = (int) pACQ196->totalRead / 5461;
		
	
	kLog (K_MON, "[proc_sendData2Tree] data_Channelization  start \n");
	sprintf(buffRaw, "%s", STR_CH_MAPPING_FILE);
        fp = fopen(buffRaw,"r");
        if(fp == NULL) {
                printf("Can't open channel mapping file :%s \n",buffRaw);
        } else {
                for(i=0; i<96; i++)
                {
                        if( fgets(buffRaw, 32, fp) == NULL ) {
                        kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping  file EOF error \n");
                        }
                        sscanf(buffRaw, "%d %d", &nTemp, &nVal);
                        if( nTemp != (i+1) ) {
                                kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping channel index order error! \n");
                        }
                        channelMapping[nVal] = i;

                        kLog (K_MON, "[threadFunc_ACQ196_RingBuf] : Channel Mapping check after mapping nTemp(%d), nVal(%d), indexing(%d)\n", nTemp, nVal, i);
                }
                fclose(fp);
                for(i=0; i<96; i++)
                {
                        kLog (K_MON, "[threadFunc_ACQ196_RingBuf checking] : Channel Mapping check after mapping index(%d) Final MappingVal(%d) \n", i, channelMapping[i]);
                }
        }

	fseek ( pACQ196->writeRaw_fp, 0L, SEEK_SET);

	for(i=0; i<ch_loop; i++)
	{
#if 0
		step = 0;
		for(k=0; k<5461; k++)
			step = k * 96;
#endif
			fread(&int16TempPack, 192, 1, pACQ196->writeRaw_fp);
		//	fread(&int16TempPack5461, 1048512, 1, pACQ196->writeRaw_fp);
			for(j=0; j<96; j++)
			{
                        	fwrite(&int16TempPack[j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
		//		fwrite(&int16TempPack5461[step+j], 2, 1, pACQ196->ST_Ch[channelMapping[j]].write_fp);
			}

	}
// reCounts totalRead data	
//	pACQ196->totalRead = ch_loop * 5461;	

	for(j=0; j<96; j++)
	{
		fflush(pACQ196->ST_Ch[j].write_fp);
	}
	kLog (K_MON, "[proc_sendData2Tree] data_Channelization  stop \n");
	return WR_OK;
}
int mds_directCreateNewShot(ST_STD_device *pSTDdev)
{
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
        int shot, op;
        int tstat, len;
        char buf[128];
        int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an      integer */
        int idesc = descr(&dtype_Long, &tstat, &null);


        shot = pSTDdev->ST_Base.shotNumber;
        op = pSTDdev->ST_Base.opMode;

		if( op == OPMODE_REMOTE ) {
			printf("_SFW %s: Must be Test mode run!!\n", pSTDdev->taskName);
			return WR_ERROR;
		} else {
			sprintf(buf, "TCL(\"set tree %s/shot=-1\")", "processed");
			status = MdsValue(buf, &idesc, &null, &len);
			if ( !((status) & 1) )
			{
				printf("Error TCL command: %s.\n",MdsGetMsg(status));
				return WR_ERROR;
			}
			sprintf(buf, "TCL(\"create pulse %d\")", shot);
			status = MdsValue(buf, &idesc, &null, &len);
			if ( !((status) & 1) )
			{
				printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
				return WR_ERROR;
			}
			status = MdsValue("TCL(\"close\")", &idesc, &null, &len);
			printf("\%s: Create new tree(\"%d\")... OK\n", pSTDdev->taskName, shot);
		}
		return WR_OK;
}

int mds_ParamDataPut(ST_STD_device *pSTDdev, int i)
{
	ST_ACQ196	*pACQ196	= (ST_ACQ196 *)pSTDdev->pUser;
        int null = 0; /* Used to mark the end of the argument list */
        int status; /* Will contain the status of the data access operation */
	char mdsNodeParam1[60];
	char mdsNodeParam2[60];
	char mdsNodeParam3[60];
	char mdsNodeParam4[60];
	char mdsNodeParam5[60];
        char tdiBuffer[60];

	sprintf(mdsNodeParam1, "\\%s:P_GAIN", pACQ196->ST_Ch[i].inputTagName);
	kLog (K_MON, "%s\n", &mdsNodeParam1);
	sprintf(mdsNodeParam2, "\\%s:P_OFFSET", pACQ196->ST_Ch[i].inputTagName);
	kLog (K_MON, "%s\n", &mdsNodeParam2);
	sprintf(mdsNodeParam3, "\\%s:A", pACQ196->ST_Ch[i].inputTagName);
	kLog (K_MON, "%s\n", &mdsNodeParam3);
	sprintf(mdsNodeParam4, "\\%s:B", pACQ196->ST_Ch[i].inputTagName);
	kLog (K_MON, "%s\n", &mdsNodeParam4);
	sprintf(mdsNodeParam5, "\\%s:C", pACQ196->ST_Ch[i].inputTagName);
	kLog (K_MON, "%s\n", &mdsNodeParam5);

	if(4<=i && i<=9 ) {
		sprintf(tdiBuffer, "FS_FLOAT(%f)", pACQ196->ST_Ch[i].mdsParam3);
		status = MdsPut(mdsNodeParam3, tdiBuffer, &null);
		if ( !((status) & 1) )
		{
			printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			return WR_ERROR;
		}
		kLog (K_MON, "[proc_sendData2Tree mds_ParamDataPut] MDSNode (%s), Put Value:%f \n",
				&mdsNodeParam3, pACQ196->ST_Ch[i].mdsParam3);

		sprintf(tdiBuffer, "FS_FLOAT(%f)", pACQ196->ST_Ch[i].mdsParam4);
		status = MdsPut(mdsNodeParam4, tdiBuffer, &null);
		if ( !((status) & 1) )
		{
			printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			return WR_ERROR;
		}
		kLog (K_MON, "[proc_sendData2Tree mds_ParamDataPut] MDSNode (%s), Put Value:%f \n",
				&mdsNodeParam4, pACQ196->ST_Ch[i].mdsParam4);

		sprintf(tdiBuffer, "FS_FLOAT(%f)", pACQ196->ST_Ch[i].mdsParam5);
		status = MdsPut(mdsNodeParam5, tdiBuffer, &null);
		if ( !((status) & 1) )
		{
			printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			return WR_ERROR;
		}
		kLog (K_MON, "[proc_sendData2Tree mds_ParamDataPut] MDSNode (%s), Put Value:%f \n",
				&mdsNodeParam5, pACQ196->ST_Ch[i].mdsParam5);

	} else if(0<=i && i<=3){
		sprintf(tdiBuffer, "FS_FLOAT(%f)", pACQ196->ST_Ch[i].mdsParam1);
		status = MdsPut(mdsNodeParam1, tdiBuffer, &null);
		if ( !((status) & 1) )
		{
			printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			return WR_ERROR;
		}
		kLog (K_MON, "[proc_sendData2Tree mds_ParamDataPut] MDSNode (%s), Put Value:%f \n",
				&mdsNodeParam1, pACQ196->ST_Ch[i].mdsParam1);
		sprintf(tdiBuffer, "FS_FLOAT(%f)", pACQ196->ST_Ch[i].mdsParam2);
		status = MdsPut(mdsNodeParam2, tdiBuffer, &null);
		if ( !((status) & 1) )
		{
			printf("Error TCL command: create pulse: %s.\n",MdsGetMsg(status));
			return WR_ERROR;
		}
		kLog (K_MON, "[proc_sendData2Tree mds_ParamDataPut] MDSNode (%s), Put Value:%f \n",
				&mdsNodeParam2, pACQ196->ST_Ch[i].mdsParam2);
	}
	return WR_OK;
}
int proc_sendData2Tree (ST_STD_device *pSTDdev)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	ST_ACQ196	*pACQ196	= (ST_ACQ196 *)pSTDdev->pUser;
	int 	dypte_Short		= DTYPE_SHORT;
	int 	dtype_Float64	= DTYPE_DOUBLE; /* We are going to read a signal made of float values */
//	int 	nTrueSampleCnt	= getTotSampsChan (pACQ196);    // If I didnot enough data size(sample Rate * acqTime) before the device stop (abort)
	int 	nTrueSampleCnt	= pACQ196->totalRead;		// I have to process with last save data size
	int 	nDataBufSize	= sizeof(intype) * nTrueSampleCnt;
	float64 fStartTime		= getStartTime (pSTDdev);
	float64	fEndTime		= nTrueSampleCnt -1; /* match the data counts and time counts */
	float64	fRealTimeGap	= 1.0 / (float64)pSTDdev->ST_Base_fetch.nSamplingRate;			
	float64 fVoltNormalRange	= EU_GRADIENT; //   ((INIT_HIGH_LIMIT - INIT_LOW_LIMIT) / I16_FULL_SCALE) 65536.0
	float64 fVoltRange;   
	float64 fVoltMinRange;  

	int	fVoltR1val = 32768;   // - - 32768
	int	fVoltR2R1val = 65535; // 32767 + 32768;
	int	rateDesc		= descr(&dtype_Float64, &fRealTimeGap, &_null);
	int	startDesc		= descr(&dtype_Float64, &fStartTime, &_null);
	int	endDesc			= descr(&dtype_Float64, &fEndTime, &_null);
	
	int	voltR1Desc		=descr(&dypte_Short,&fVoltR1val, &_null);
	int	voltR2R1Desc		=descr(&dypte_Short,&fVoltR2R1val, &_null);
	
	FILE	*fpRaw			= NULL;	
	intype	*databuf		= NULL;	/* for read bulk data */
	int	rawDataDesc;			/* Signal descriptor */
	int	voltRangeDesc;			/* Signal descriptor */
	int	voltMinRangeDesc;			/* Signal descriptor */
	int	voltNormalRangeDesc		= descr(&dtype_Float64, &fVoltNormalRange, &_null);
	int32	i, j;
	
	int		shotNumber;
	float	dataBuf;
	
	int	chanNum;
	/* TGLee change from INIT_SAMPLE_RATE to chanNum(ellCount) */
    	chanNum = ellCount(pSTDdev->pList_Channel);

	dataBuf = 0.0;
	shotNumber = pSTDdev->ST_Base_fetch.shotNumber;

	kLog (K_MON, "[proc_sendData2Tree] T0(%f) T1(%f) Blip(%f) Rate(%d)\n",
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0],
			pSTDdev->ST_Base_fetch.fBlipTime, pSTDdev->ST_Base_fetch.nSamplingRate);

	kLog (K_INFO, "[proc_sendData2Tree] start(%d) end(%d) gap(%f) samples(%d) compareFileLastSamples(%d) HTime(%f)\n",
			(int)fStartTime, (int)fEndTime, fRealTimeGap, (int)nTrueSampleCnt,pACQ196->nsamplesRead, pACQ196->htimeRead);

	if (NULL == (databuf = (intype *)malloc(nDataBufSize))) {
		kLog (K_ERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}

	kLog (K_MON, "[proc_sendData2Tree] connect : task(%s) mode(%s) mdsip(%s)\n",
			pSTDdev->taskName,
			getModeMsg(pSTDdev->ST_Base.opMode),
			pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode]);
	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
		if (mds_Connect(pSTDdev) == WR_ERROR) {
			kLog (K_ERR, "[proc_sendData2Tree] connect fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
					pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode), 
					pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode], 
					pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode],
					pSTDdev->ST_Base_fetch.shotNumber);
			notify_error (1, "MdsConnet(%s) failed\n", pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode]);

			free(databuf);
			return WR_ERROR;
		}
	} else
	{
		kLog (K_MON, "[proc_sendData2Tree] LocalMode : tree (%s) \n",pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode]);
		pSTDdev->ST_mds.StatusMds |= MDS_CONNECTED;
	}

	kLog (K_MON, "[proc_sendData2Tree] open : task(%s) tree(%s) shot(%d)\n",
			pSTDdev->taskName,
			pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode], pSTDdev->ST_Base_fetch.shotNumber);

	epicsThreadSleep(2.0);		

	if (mds_Open_withFetch(pSTDdev) == WR_ERROR) {
		kLog (K_ERR, "[proc_sendData2Tree] open fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
				pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode), 
				pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode], 
				pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode],
				pSTDdev->ST_Base_fetch.shotNumber);

		notify_error (1, "MdsOpen(%s/%d) failed\n",
			pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode], pSTDdev->ST_Base_fetch.shotNumber);

		if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
			mds_Disconnect(pSTDdev);
		} else  pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;

		free(databuf);
		return WR_ERROR;
	}

#if NO_RT_CH
// Doing channelization 
	data_Channelization(pSTDdev);

#endif
	for (i = 0; i<chanNum; i++) {
		if(pACQ196->ST_Ch[i].saveToMds ==1){
			if (NULL == (fpRaw = pACQ196->ST_Ch[i].write_fp)) {
				continue;
			}
			fVoltMinRange 		= pACQ196->ST_Ch[i].minVoltRange;
			fVoltRange 		= pACQ196->ST_Ch[i].maxVoltRange - pACQ196->ST_Ch[i].minVoltRange;
			voltRangeDesc		= descr(&dtype_Float64, &fVoltRange, &_null);
			voltMinRangeDesc	= descr(&dtype_Float64, &fVoltMinRange, &_null);
			kLog (K_INFO, "[proc_sendData2Tree] path(%s) mdsNode(%s) fp(%p) size(%d), minV(%f), max-minV(%f)\n",
					pACQ196->ST_Ch[i].path_name, pACQ196->ST_Ch[i].strTagName, pACQ196->ST_Ch[i].write_fp, nDataBufSize, fVoltMinRange, fVoltRange);

			// rewind
			fseek (fpRaw, 0L, SEEK_SET);

			if (1 != fread (databuf, nDataBufSize, 1, fpRaw)) {
				kLog (K_ERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pACQ196->ST_Ch[i].file_name, strerror(errno));
				notify_error (1, "%s read failed\n", pACQ196->ST_Ch[i].file_name);

				fclose(fpRaw);
				pACQ196->ST_Ch[i].write_fp = NULL;
				continue;
			}

			if (TRUE == isPrintDebugMsg (K_DEL)) {
				for (j=0;j<nTrueSampleCnt;j++) {
					if (! (j % 10)) printf ("\n(%04d) ", j);
					printf ("%5d ", databuf[j]);
				}
				printf ("\n");
			}


			rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);
#if 0			
			   status = MdsPut(pACQ196->ST_Ch[i].strTagName,
			   "BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
			   &voltNormalRangeDesc, &rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);
#endif
#if 0 
// fail - not success  data range some strange

			status = MdsPut(pACQ196->ST_Ch[i].strTagName,
					"BUILD_SIGNAL(BUILD_WITH_UNITS(($+($+$)*$/$),\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					&voltMinRangeDesc, &rawDataDesc, &voltR1Desc, &voltRangeDesc, &voltR2R1Desc, &endDesc, &startDesc, &rateDesc, &_null);
#endif
#if 1
			status = MdsPut(pACQ196->ST_Ch[i].strTagName,
					"BUILD_SIGNAL(BUILD_WITH_UNITS(($ + (($ + 32768) * $ / 65535)),\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					 &voltMinRangeDesc, &rawDataDesc, &voltRangeDesc, &endDesc, &startDesc, &rateDesc, &_null);
#endif
			if ( !((status) & 1) ) {
				kLog (K_ERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
						pSTDdev->taskName, pACQ196->ST_Ch[i].strTagName, (int)i, MdsGetMsg(status));
				notify_error (1, "MdsPut(%s) failed", pACQ196->ST_Ch[i].strTagName);
			}
/* 
			else {
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
			}
*/
			fclose(fpRaw);
			pACQ196->ST_Ch[i].write_fp = NULL;

			kLog (K_INFO, "[proc_sendData2Tree] path(%s) put ok...\n", pACQ196->ST_Ch[i].path_name);
		}
	}
	kLog (K_MON, "[proc_sendData2Tree] %s : Tree Put Done!\n", pSTDdev->taskName);
	free(databuf);
	mds_Close_withFetch(pSTDdev);

/*  This function is write to other tree for density analysis data store into processed tree */
	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
		mds_Disconnect(pSTDdev);
//		status = completeDataPut();   /* MDS Data Store complated Event Put in the Data Analysis server */
	} else  pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
	return WR_OK;
}

int completeDataPut()
{
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access operation */
	int socket; /* Will contain the handle to the remote mdsip server */
	int dtype_Long = DTYPE_LONG; /* The returned value of SIZE() is an      integer */
	char buf[512];

	int tstat, len;
	int idesc = descr(&dtype_Long, &tstat, &null);

	/* Connect to MDSplus */
	socket = MdsConnect("172.17.100.200:8300");
	if ( socket == -1 )
	{
		epicsPrintf("Error connecting to mdsip server(%s).\n", "172.17.100.200:8300");
		return 0;
	}
	epicsPrintf("\nMDSplus >>> MdsConnect(\"%s\")... OK\n", "172.17.100.200:8300");

	sprintf(buf, "TCL(\"setevent GCDS\")");
	status = MdsValue(buf, &idesc, &null, &len);
	if( !((status) & 1 ))
	{
		epicsPrintf("Error TCL command : create pulse: %s.\n", MdsGetMsg(status));
		return 0;
	}
	MdsDisconnect("172.17.100.200:8300");
	return WR_OK;
}
