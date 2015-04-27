#include <errno.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "sfwMDSplus.h"
#include "drvNI6254.h"

int proc_sendData2Tree (ST_STD_device *pSTDdev)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	ST_NI6254	*pNI6254	= (ST_NI6254 *)pSTDdev->pUser;
	int 	dypte_Short		= DTYPE_SHORT;
	int 	dtype_Float64	= DTYPE_DOUBLE; /* We are going to read a signal made of float values */
	int 	nTrueSampleCnt	= getTotSampsChan (pNI6254);
	int 	nDataBufSize	= sizeof(intype) * nTrueSampleCnt;
	float64 fStartTime		= getStartTime (pSTDdev);
	float64	fEndIdx			= nTrueSampleCnt - 1;
	float64	fRealTimeGap	= 1.0 / (float64)pSTDdev->ST_Base_fetch.nSamplingRate;			
	float64 fVoltRange		= EU_GRADIENT;

	int		rateDesc		= descr(&dtype_Float64, &fRealTimeGap, &_null);
	int		startDesc		= descr(&dtype_Float64, &fStartTime, &_null);
	int		endDesc			= descr(&dtype_Float64, &fEndIdx, &_null);
	int		voltRangeDesc	= descr(&dtype_Float64, &fVoltRange, &_null);

	FILE	*fpRaw			= NULL;	
	intype	*databuf		= NULL;	/* for read bulk data */
	int		rawDataDesc;			/* Signal descriptor */
	int32	i, j;

	kLog (K_MON, "[proc_sendData2Tree] T0(%f) T1(%f) Blip(%f) Rate(%d)\n",
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0],
			pSTDdev->ST_Base_fetch.fBlipTime, pSTDdev->ST_Base_fetch.nSamplingRate);

	kLog (K_INFO, "[proc_sendData2Tree] start(%d) end(%d) gap(%f) samples(%d)\n",
			(int)fStartTime, (int)fEndIdx, fRealTimeGap, (int)nTrueSampleCnt);

	if (NULL == (databuf = (intype *)malloc(nDataBufSize))) {
		kLog (K_ERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}

	kLog (K_MON, "[proc_sendData2Tree] connect : task(%s) mode(%s) mdsip(%s)\n",
			pSTDdev->taskName,
			getModeMsg(pSTDdev->ST_Base.opMode),
			pSTDdev->ST_mds.treeIPport[pSTDdev->ST_Base.opMode]);

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

		mds_Disconnect(pSTDdev);
		free(databuf);
		return WR_ERROR;
	}

	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
		if (NULL == (fpRaw = pNI6254->ST_Ch[i].write_fp)) {
			continue;
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) fp(%p) size(%d)\n",
				pNI6254->ST_Ch[i].path_name, pNI6254->ST_Ch[i].write_fp, nDataBufSize);

		// rewind
		fseek (fpRaw, 0L, SEEK_SET);

		if (1 != fread (databuf, nDataBufSize, 1, fpRaw)) {
			kLog (K_ERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pNI6254->ST_Ch[i].file_name, strerror(errno));
			notify_error (1, "%s read failed\n", pNI6254->ST_Ch[i].file_name);

			fclose(fpRaw);
			pNI6254->ST_Ch[i].write_fp = NULL;
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
		pNI6254->ST_Ch[i].write_fp = NULL;

		rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);

		status = MdsPut(pNI6254->ST_Ch[i].strTagName,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&voltRangeDesc, &rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

		if ( !((status) & 1) ) {
			kLog (K_ERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
					pSTDdev->taskName, pNI6254->ST_Ch[i].strTagName, (int)i, MdsGetMsg(status));
			notify_error (1, "MdsPut(%s) failed", pNI6254->ST_Ch[i].strTagName);
		} 
		else {
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) put ok...\n", pNI6254->ST_Ch[i].path_name);
	}

	kLog (K_MON, "[proc_sendData2Tree] %s : Tree Put Done!\n", pSTDdev->taskName);

	free(databuf);

	mds_Close_withFetch(pSTDdev);
	mds_Disconnect(pSTDdev);

	return WR_OK;
}

