#include <errno.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "drvNI6220.h"

int proc_sendData2Tree_sub (int opMode, ST_STD_device *pSTDdev)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	ST_NI6220	*pNI6220	= (ST_NI6220 *)pSTDdev->pUser;
	int 	dypte_Short		= DTYPE_SHORT;
	int 	dtype_Float64	= DTYPE_DOUBLE; /* We are going to read a signal made of float values */
	int 	nTrueSampleCnt	= getTotSampsChan (pNI6220);
	int 	nDataBufSize	= sizeof(intype) * nTrueSampleCnt;
	float64 fStartTime		= getStartTime (pSTDdev);
	float64	fEndTime		= nTrueSampleCnt - 1; /* match the data counts and time counts */
	float64	fRealTimeGap	= 1.0 / (float64)pSTDdev->ST_Base_fetch.nSamplingRate;			
	float64 fVoltRange		= pNI6220->euGradient;

	int		rateDesc		= descr(&dtype_Float64, &fRealTimeGap, &_null);
	int		startDesc		= descr(&dtype_Float64, &fStartTime, &_null);
	int		endDesc			= descr(&dtype_Float64, &fEndTime, &_null);
	int		voltRangeDesc	= descr(&dtype_Float64, &fVoltRange, &_null);

	FILE	*fpRaw			= NULL;	
	intype	*databuf		= NULL;	/* for read bulk data */
	int		rawDataDesc;			/* Signal descriptor */
	int32	i, j;
	int		shotNumber;

	FILE	*fpCalc			= NULL;	
	double	*pdDataBuf		= NULL;	/* for read bulk data */
	int 	nCalcBufSize	= sizeof(double) * nTrueSampleCnt;
	int		calcDataDesc;			/* Signal descriptor */

	shotNumber = pSTDdev->ST_Base_fetch.shotNumber;

	kuDebug (kuMON, "[proc_sendData2Tree] T0(%.2f) T1(%.2f) Blip(%.2f) Rate(%d)\n",
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0],
			pSTDdev->ST_Base_fetch.fBlipTime, pSTDdev->ST_Base_fetch.nSamplingRate);

	kuDebug (kuINFO, "[proc_sendData2Tree] start(%.2f) end(%.2f) gap(%.2f) samples(%d) voltRange(%f)\n",
			fStartTime, fEndTime, fRealTimeGap, (int)nTrueSampleCnt, fVoltRange);

	if (NULL == (databuf = (intype *)malloc(nDataBufSize))) {
		kuDebug (kuERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}

	if (NULL == (pdDataBuf = (double *)malloc(nCalcBufSize))) {
		kuDebug (kuERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}

	if( opMode == OPMODE_REMOTE ){
		kuDebug (kuMON, "[proc_sendData2Tree] connect : task(%s) mode(%s) mdsip(%s)\n",
				pSTDdev->taskName,
				getModeMsg(opMode),
				pSTDdev->ST_mds.treeIPport[opMode]);

		if (mds_Connect(pSTDdev) == WR_ERROR) {
			kuDebug (kuERR, "[proc_sendData2Tree] connect fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
					pSTDdev->taskName, getModeMsg(opMode), 
					pSTDdev->ST_mds.treeIPport[opMode], 
					pSTDdev->ST_mds.treeName[opMode],
					pSTDdev->ST_Base_fetch.shotNumber);
			notify_error (1, "MdsConnet(%s) failed\n", pSTDdev->ST_mds.treeIPport[opMode]);

			free(databuf);
			return WR_ERROR;
		}
	}
	else {
		pSTDdev->ST_mds.StatusMds |= MDS_CONNECTED;
	}

	kuDebug (kuMON, "[proc_sendData2Tree] open : mode(%d) task(%s) tree(%s) shot(%d)\n",
			opMode, pSTDdev->taskName,
			pSTDdev->ST_mds.treeName[opMode], pSTDdev->ST_Base_fetch.shotNumber);

	if (mds_Open_withFetch(pSTDdev) == WR_ERROR) {
		kuDebug (kuERR, "[proc_sendData2Tree] open fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
				pSTDdev->taskName, getModeMsg(opMode), 
				pSTDdev->ST_mds.treeIPport[opMode], 
				pSTDdev->ST_mds.treeName[opMode],
				pSTDdev->ST_Base_fetch.shotNumber);

		notify_error (1, "MdsOpen(%s/%d) failed\n",
			pSTDdev->ST_mds.treeName[opMode], pSTDdev->ST_Base_fetch.shotNumber);

		if (opMode == OPMODE_REMOTE){
			mds_Disconnect(pSTDdev);
		}
		else {
			pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;
		}

		free(databuf);
		return WR_ERROR;
	}

	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
		if (NULL == (fpRaw = pNI6220->ST_Ch[i].write_fp)) {
			continue;
		}

		kuDebug (kuINFO, "[proc_sendData2Tree] path(%s) mdsNode(%s) fp(%p) size(%d)\n",
				pNI6220->ST_Ch[i].path_name, pNI6220->ST_Ch[i].strTagName, pNI6220->ST_Ch[i].write_fp, nDataBufSize);

		// rewind
		fseek (fpRaw, 0L, SEEK_SET);

		if (1 != fread (databuf, nDataBufSize, 1, fpRaw)) {
			kuDebug (kuERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pNI6220->ST_Ch[i].file_name, strerror(errno));
			notify_error (1, "%s read failed\n", pNI6220->ST_Ch[i].file_name);

			fclose(fpRaw);
			pNI6220->ST_Ch[i].write_fp = NULL;
			continue;
		}

		if (TRUE == kuCanPrint (kuDEL)) {
			for (j=0;j<nTrueSampleCnt;j++) {
				if (! (j % 10)) printf ("\n(%04d) ", j);
				printf ("%5d ", databuf[j]);
			}
			printf ("\n");
		}

		fclose(fpRaw);
		pNI6220->ST_Ch[i].write_fp = NULL;
#endif

		rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);

		status = MdsPut(pNI6220->ST_Ch[i].strTagName,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&voltRangeDesc, &rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

		if ( !((status) & 1) ) {
			kuDebug (kuERR, "[proc_sendData2Tree] %s : Error tag(%s), ch%d: %s\n",
					pSTDdev->taskName, pNI6220->ST_Ch[i].strTagName, (int)i, MdsGetMsg(status));
			notify_error (1, "MdsPut(%s) failed", pNI6220->ST_Ch[i].strTagName);
		} 
		else {
			kuDebug (kuINFO, "[proc_sendData2Tree] %s put ok ...\n", pNI6220->ST_Ch[i].strTagName);
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
		}

		if (NULL != (fpCalc = pNI6220->ST_Ch[i].write_fp_eu)) {
			fseek (fpCalc, 0L, SEEK_SET);

			if (1 != fread (pdDataBuf, nCalcBufSize, 1, fpCalc)) {
				kuDebug (kuERR, "[proc_sendData2Tree] %s_EU : fread failed (%s)\n", pNI6220->ST_Ch[i].file_name, strerror(errno));
				notify_error (1, "%s_EU read failed\n", pNI6220->ST_Ch[i].file_name);
			}
			else {
				calcDataDesc = descr(&dtype_Float64, pdDataBuf, &nTrueSampleCnt, &_null);

				status = MdsPut (pNI6220->ST_Ch[i].strTagNameEu,
						"BUILD_SIGNAL($,,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
						&calcDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

				if ( !((status) & 1) ) {
					kuDebug (kuERR, "[proc_sendData2Tree] %s : Error tag(%s), ch%d: %s\n",
							pSTDdev->taskName, pNI6220->ST_Ch[i].strTagNameEu, (int)i, MdsGetMsg(status));
					notify_error (1, "MdsPut(%s) failed", pNI6220->ST_Ch[i].strTagNameEu);
				} 
				else {
					kuDebug (kuINFO, "[proc_sendData2Tree] %s put ok ...\n", pNI6220->ST_Ch[i].strTagNameEu);
				}
			}

			fclose(fpCalc);
			pNI6220->ST_Ch[i].write_fp_eu = NULL;
		} 

		if (NULL != (fpCalc = pNI6220->ST_Ch[i].write_fp_calc)) {
			fseek (fpCalc, 0L, SEEK_SET);

			if (1 != fread (pdDataBuf, nCalcBufSize, 1, fpCalc)) {
				kuDebug (kuERR, "[proc_sendData2Tree] %s_CALC : fread failed (%s)\n", pNI6220->ST_Ch[i].file_name, strerror(errno));
				notify_error (1, "%s_EU read failed\n", pNI6220->ST_Ch[i].file_name);
			}
			else {
				calcDataDesc = descr(&dtype_Float64, pdDataBuf, &nTrueSampleCnt, &_null);

				status = MdsPut (pNI6220->ST_Ch[i].strTagNameCalc,
						"BUILD_SIGNAL($,,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
						&calcDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

				if ( !((status) & 1) ) {
					kuDebug (kuERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
							pSTDdev->taskName, pNI6220->ST_Ch[i].strTagNameCalc, (int)i, MdsGetMsg(status));
					notify_error (1, "MdsPut(%s) failed", pNI6220->ST_Ch[i].strTagNameCalc);
				} 
				else {
					kuDebug (kuINFO, "[proc_sendData2Tree] %s put ok ...\n", pNI6220->ST_Ch[i].strTagNameCalc);
				}
			}

			fclose(fpCalc);
			pNI6220->ST_Ch[i].write_fp_calc = NULL;
		} 

		kuDebug (kuINFO, "[proc_sendData2Tree] path(%s) put ok...\n", pNI6220->ST_Ch[i].path_name);
	}

	kuDebug (kuMON, "[proc_sendData2Tree] %s : Tree Put Done!\n", pSTDdev->taskName);

	free(databuf);

	mds_Close_withFetch(pSTDdev);

	if (opMode == OPMODE_REMOTE) {
		mds_Disconnect(pSTDdev);

		// MDS Data Store complated Event Put in the Data Analysis server
		mds_notify_EndOfTreePut(pSTDdev);
	}
	else {
		pSTDdev->ST_mds.StatusMds &= ~MDS_CONNECTED;

		mds_notify_EndOfTreePut(pSTDdev);
	}

	return WR_OK;
}

int proc_sendData2Tree (ST_STD_device *pSTDdev)
{
#if 0
	proc_sendData2Tree_sub (OPMODE_LOCAL, pSTDdev);

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		proc_sendData2Tree_sub (OPMODE_REMOTE, pSTDdev);
	}
#else
	proc_sendData2Tree_sub (pSTDdev->ST_Base.opMode, pSTDdev);
#endif

	return WR_OK;
}

