#include <errno.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "drvNI6123.h"

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

int proc_sendData2Tree (ST_STD_device *pSTDdev)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	ST_NI6123	*pNI6123	= (ST_NI6123 *)pSTDdev->pUser;
	int 	dypte_Short		= DTYPE_SHORT;
	int 	dtype_Float64	= DTYPE_DOUBLE; /* We are going to read a signal made of float values */
	int 	nTrueSampleCnt	= getTotSampsChan (pNI6123);
	int 	nDataBufSize	= sizeof(intype) * nTrueSampleCnt;
	float64 fStartTime		= getStartTime (pSTDdev);
	float64	fEndTime		= nTrueSampleCnt -1; /* match the data counts and time counts */
	float64	fRealTimeGap	= 1.0 / (float64)pSTDdev->ST_Base_fetch.nSamplingRate;			
	float64 fVoltRange		= EU_GRADIENT;

	int		rateDesc		= descr(&dtype_Float64, &fRealTimeGap, &_null);
	int		startDesc		= descr(&dtype_Float64, &fStartTime, &_null);
	int		endDesc			= descr(&dtype_Float64, &fEndTime, &_null);
	int		voltRangeDesc	= descr(&dtype_Float64, &fVoltRange, &_null);

	FILE	*fpRaw			= NULL;	
	FILE	*fpDen			= NULL;	
	intype	*databuf		= NULL;	/* for read bulk data */
	int		rawDataDesc;			/* Signal descriptor */
	int32	i, j;
	
	int		shotNumber;
	int 	denDataBufSize	= sizeof(float64) * nTrueSampleCnt;
	char	mdsNodeBuffer[30];

	shotNumber = pSTDdev->ST_Base_fetch.shotNumber;

	kLog (K_MON, "[proc_sendData2Tree] T0(%f) T1(%f) Blip(%f) Rate(%d)\n",
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0],
			pSTDdev->ST_Base_fetch.fBlipTime, pSTDdev->ST_Base_fetch.nSamplingRate);

	kLog (K_INFO, "[proc_sendData2Tree] start(%d) end(%d) gap(%f) samples(%d)\n",
			(int)fStartTime, (int)fEndTime, fRealTimeGap, (int)nTrueSampleCnt);

	if (NULL == (databuf = (intype *)malloc(nDataBufSize))) {
		kLog (K_ERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}
#ifdef MMWI_MDSPLUS
	float64	*dendatabuf		= NULL;	/* for read density data */
	if (NULL == (dendatabuf = (float64 *)malloc(denDataBufSize))) {
		kLog (K_ERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}
#endif
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
	  	pSTDdev->ST_mds.StatusMds |= MDS_CONNECTED;
#ifdef MMWI_MDSPLUS
		status = MdsOpen("processed", &shotNumber);
		if ( !((status) & 1) )
		{
			fprintf(stderr,"%s: Error opening \"%s\" local tree for shot %d: %s. so Make local shot tree. \n", pSTDdev->taskName, "processed", shotNumber, MdsGetMsg(status));
			printlog("%s: Error opening processed tree for shot %d: %s. so Make local shot tree.\n", pSTDdev->taskName, shotNumber, MdsGetMsg(status));
			if(	mds_directCreateNewShot(pSTDdev) == WR_ERROR) {
				kLog (K_ERR, "[proc_sendData2Tree] process tree create fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
						pSTDdev->taskName, getModeMsg(pSTDdev->ST_Base.opMode),
						pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode],
						pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode],
						pSTDdev->ST_Base_fetch.shotNumber);
				notify_error (1, "mds_directCreateNewShot (%s) failed\n", pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode]);
			}
		} else  status = MdsClose("processed", &shotNumber);
#endif
	}

	kLog (K_MON, "[proc_sendData2Tree] open : task(%s) tree(%s) shot(%d)\n",
			pSTDdev->taskName,
			pSTDdev->ST_mds.treeName[pSTDdev->ST_Base.opMode], pSTDdev->ST_Base_fetch.shotNumber);

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

	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
		if (NULL == (fpRaw = pNI6123->ST_Ch[i].write_fp)) {
			continue;
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) mdsNode(%s) fp(%p) size(%d)\n",
				pNI6123->ST_Ch[i].path_name, pNI6123->ST_Ch[i].strTagName, pNI6123->ST_Ch[i].write_fp, nDataBufSize);

		// rewind
		fseek (fpRaw, 0L, SEEK_SET);

		if (1 != fread (databuf, nDataBufSize, 1, fpRaw)) {
			kLog (K_ERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pNI6123->ST_Ch[i].file_name, strerror(errno));
			notify_error (1, "%s read failed\n", pNI6123->ST_Ch[i].file_name);

			fclose(fpRaw);
			pNI6123->ST_Ch[i].write_fp = NULL;
			continue;
		}

		if (TRUE == isPrintDebugMsg (K_DEL)) {
			for (j=0;j<nTrueSampleCnt;j++) {
				if (! (j % 10)) printf ("\n(%04d) ", j);
				printf ("%5d ", databuf[j]);
			}
			printf ("\n");
		}

		fclose(fpRaw);
		pNI6123->ST_Ch[i].write_fp = NULL;

		rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);

		status = MdsPut(pNI6123->ST_Ch[i].strTagName,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&voltRangeDesc, &rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);
		if ( !((status) & 1) ) {
			kLog (K_ERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
					pSTDdev->taskName, pNI6123->ST_Ch[i].strTagName, (int)i, MdsGetMsg(status));
			notify_error (1, "MdsPut(%s) failed", pNI6123->ST_Ch[i].strTagName);
		} 
		else {
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) put ok...\n", pNI6123->ST_Ch[i].path_name);
	}

	kLog (K_MON, "[proc_sendData2Tree] %s : Tree Put Done!\n", pSTDdev->taskName);
	free(databuf);
	mds_Close_withFetch(pSTDdev);

/*  This function is write to other tree for density analysis data store into processed tree */
#ifdef MMWI_MDSPLUS
	status = MdsOpen("processed", &shotNumber);
	if ( !((status) & 1) )
	{
		fprintf(stderr,"%s: Error opening \"%s\" for shot %d: %s.\n", pSTDdev->taskName, "processed", shotNumber, MdsGetMsg(status));
		printlog("%s: Error opening processed tree for shot %d: %s.\n", pSTDdev->taskName, shotNumber, MdsGetMsg(status));
	}

	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
		if (pNI6123->ST_Ch[i].inputType == DENSITY_CALC) {
			if (NULL == (fpDen = pNI6123->ST_Ch[i].write_den_fp)) {
				kLog (K_INFO, "[proc_sendData2Tree] write den file define to fdDen fp(%p) size(%d)\n", pNI6123->ST_Ch[i].write_den_fp, denDataBufSize);
				continue;
			}
			if(i==0){
				sprintf (mdsNodeBuffer,"%s","\\NE_INTER01:RT");
			} else if (i==2){
				sprintf (mdsNodeBuffer,"%s","\\NE_INTER02:RT");
			} else sprintf (mdsNodeBuffer,"%s%d","\\badNodeName",i);
			kLog (K_INFO, "[proc_sendData2Tree] path(%s) mdsNode(%s) fp(%p) size(%d)\n",
					pNI6123->ST_Ch[i].path_den_name, mdsNodeBuffer, pNI6123->ST_Ch[i].write_den_fp, denDataBufSize);

			// rewind
			fseek (fpDen, 0L, SEEK_SET);

			if (1 != fread (dendatabuf, denDataBufSize, 1, fpDen)) {
				kLog (K_ERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pNI6123->ST_Ch[i].path_den_name, strerror(errno));
				notify_error (1, "%s read failed\n", pNI6123->ST_Ch[i].path_den_name);

				fclose(fpDen);
				pNI6123->ST_Ch[i].write_den_fp = NULL;
				continue;
			}

			if (TRUE == isPrintDebugMsg (K_DEL)) {
				for (j=0;j<nTrueSampleCnt;j++) {
					if (! (j % 10)) printf ("\n(%04d) ", j);
					printf ("%f ", dendatabuf[j]);
				}
				printf ("\n");
			}

			fclose(fpDen);
			pNI6123->ST_Ch[i].write_den_fp = NULL;

			rawDataDesc = descr(&dypte_Short, dendatabuf, &nTrueSampleCnt, &_null);

			status = MdsPut(mdsNodeBuffer,
					"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"density 10^19\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
					&rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

			if ( !((status) & 1) ) {
				kLog (K_ERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
						pSTDdev->taskName, mdsNodeBuffer, (int)i, MdsGetMsg(status));
				notify_error (1, "MdsPut(%s) failed", mdsNodeBuffer);
			} 
			else {
				scanIoRequest(pSTDdev->ioScanPvt_userCall);
			}

			kLog (K_INFO, "[proc_sendData2Tree] path(%s) put ok...\n", pNI6123->ST_Ch[i].path_den_name);
		}
	}

	free(dendatabuf);
	status = MdsClose("processed", &shotNumber);
	if ( !((status) & 1) )
	{
		printf("\n>>> Error closing processed tree for shot %d: %s.\n", shotNumber, MdsGetMsg(status));
	}
#endif


	if( pSTDdev->ST_Base.opMode == OPMODE_REMOTE ){
		mds_Disconnect(pSTDdev);
	} else  pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
	return WR_OK;
}

