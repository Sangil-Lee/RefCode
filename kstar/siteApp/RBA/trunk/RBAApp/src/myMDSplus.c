#include <errno.h>

#include <mdslib.h>
#include <mdsdescrip.h>
#include <mdsshr.h>

#include "drvNI6122.h"
#include "drvNI4882.h"

int proc_sendData2Tree_str (char *pszTagName, char *pszValueStr)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	if ( (NULL == pszTagName) || (NULL == pszValueStr) ) {
		return WR_ERROR;
	}

	kLog (K_INFO, "[proc_sendData2Tree_str] tag(%s) : val(%s) \n", pszTagName, pszValueStr);

	status = MdsPut (pszTagName, pszValueStr, &_null);

	if ( !((status) & 1) ) {
		//kLog (K_MON, "[proc_sendData2Tree_str] Error : tag(%s) val(%s) \n", pszTagName, pszValueStr);
		kLog (K_MON, "[proc_sendData2Tree_str] Error : tag(%s) val(%s) %s\n", pszTagName, pszValueStr, MdsGetMsg(status));
		notify_error (1, "MdsPut(%s) failed", pszTagName);
	} 
	else {
		kLog (K_INFO, "[proc_sendData2Tree_str] %s : Tree Put Done!\n", pszTagName);
	}

	return WR_OK;
}

int proc_sendData2Tree_value (char *pszTagName, const float fValue)
{
	char	szValueStr[256];
	sprintf (szValueStr, "%f",  fValue);

	return (proc_sendData2Tree_str (pszTagName, szValueStr));
}

int proc_sendData2Tree_values (ST_STD_device *pSTDdev)
{
	char			szBuf[64];
	char			szBuf2[64];
	char			szPvName[128];
	char			szTagName[128];
	int				i;
	FILE			*fpRaw;
	ST_NI4882_CH	*pAmpCh;
	ST_NI6122		*pNI6122	= (ST_NI6122 *)pSTDdev->pUser;

	if (TRUE == isFirstDevice(pSTDdev)) {
		DBproc_get ("RBA_CAL_DATE", szBuf2);
		sprintf (szBuf, "\"%s\"", szBuf2);
		proc_sendData2Tree_str   ("\\RBA_CAL_DATE", szBuf); // "\"2012.09.17\""

		DBproc_get ("RBA_SHUT_STATUS", szBuf);
		proc_sendData2Tree_str   ("\\SHUT_STATUS", szBuf);

		proc_sendData2Tree_value ("\\RBA_DIGI.SAMPLING", pSTDdev->ST_Base_fetch.nSamplingRate);
		proc_sendData2Tree_value ("\\RBA_DIGI.T_START", pSTDdev->ST_Base_fetch.dT0[0] - pSTDdev->ST_Base_fetch.fBlipTime);
		proc_sendData2Tree_value ("\\RBA_DIGI.T_END", pSTDdev->ST_Base_fetch.dT1[0] - pSTDdev->ST_Base_fetch.fBlipTime);
	}

	// GAIN, FILTER, T_AMP
	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
		if (NULL == (fpRaw = pNI6122->ST_Ch[i].write_fp)) {
			continue;
		}

		if (NULL == (pAmpCh = getGpibChannel (pNI6122->nAmpDeviceIdx, i))) {
			continue;
		}

		sprintf (szTagName, "\\%s:GAIN", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_value (szTagName, getAmpGainValue (pAmpCh->gain));

		sprintf (szTagName, "\\%s:FILTER", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_value (szTagName, getAmpFilterValue (pAmpCh->filter));

		sprintf (szTagName, "\\%s:T_AMP", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_value (szTagName, pAmpCh->temp);

		sprintf (szPvName, "%s_RM", pNI6122->ST_Ch[i].inputPvName);
		DBproc_get (szPvName, szBuf);
		sprintf (szTagName, "\\%s:RM", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_str (szTagName, szBuf);

		sprintf (szPvName, "%s_U1", pNI6122->ST_Ch[i].inputPvName);
		DBproc_get (szPvName, szBuf);
		sprintf (szTagName, "\\%s:U1", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_str (szTagName, szBuf);

		sprintf (szPvName, "%s_U2", pNI6122->ST_Ch[i].inputPvName);
		DBproc_get (szPvName, szBuf);
		sprintf (szTagName, "\\%s:U2", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_str (szTagName, szBuf);

		sprintf (szPvName, "%s_TAU", pNI6122->ST_Ch[i].inputPvName);
		DBproc_get (szPvName, szBuf);
		sprintf (szTagName, "\\%s:TAU", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_str (szTagName, szBuf);

		sprintf (szPvName, "%s_DU", pNI6122->ST_Ch[i].inputPvName);
		DBproc_get (szPvName, szBuf);
		sprintf (szTagName, "\\%s:DU", pNI6122->ST_Ch[i].inputTagName);
		proc_sendData2Tree_str (szTagName, szBuf);
	}

	return WR_OK;
}

int proc_sendData2Tree_sub (int opMode, ST_STD_device *pSTDdev)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	ST_NI6122	*pNI6122	= (ST_NI6122 *)pSTDdev->pUser;
	int 	dypte_Short		= DTYPE_SHORT;
	int 	dtype_Float64	= DTYPE_DOUBLE; /* We are going to read a signal made of float values */
	int 	nTrueSampleCnt	= getTotSampsChan (pNI6122);
	int 	nDataBufSize	= sizeof(intype) * nTrueSampleCnt;
	float64 fStartTime		= getStartTime (pSTDdev);
	float64	fEndTime		= nTrueSampleCnt - 1; /* match the data counts and time counts */
	float64	fRealTimeGap	= 1.0 / (float64)pSTDdev->ST_Base_fetch.nSamplingRate;			
	float64 fVoltRange		= pNI6122->euGradient;

	int		rateDesc		= descr(&dtype_Float64, &fRealTimeGap, &_null);
	int		startDesc		= descr(&dtype_Float64, &fStartTime, &_null);
	int		endDesc			= descr(&dtype_Float64, &fEndTime, &_null);
	int		voltRangeDesc	= descr(&dtype_Float64, &fVoltRange, &_null);

	FILE	*fpRaw			= NULL;	
	intype	*databuf		= NULL;	/* for read bulk data */
	int		rawDataDesc;			/* Signal descriptor */
	int32	i, j;
	int		shotNumber;

	shotNumber = pSTDdev->ST_Base_fetch.shotNumber;

	kLog (K_MON, "[proc_sendData2Tree] T0(%.2f) T1(%.2f) Blip(%.2f) Rate(%d)\n",
			pSTDdev->ST_Base_fetch.dT0[0], pSTDdev->ST_Base_fetch.dT1[0],
			pSTDdev->ST_Base_fetch.fBlipTime, pSTDdev->ST_Base_fetch.nSamplingRate);

	kLog (K_INFO, "[proc_sendData2Tree] start(%.2f) end(%.2f) gap(%.2f) samples(%d) voltRange(%f)\n",
			fStartTime, fEndTime, fRealTimeGap, (int)nTrueSampleCnt, fVoltRange);

	if (NULL == (databuf = (intype *)malloc(nDataBufSize))) {
		kLog (K_ERR, "[proc_sendData2Tree] %s : malloc(%d) failed\n", pSTDdev->taskName, (int)nTrueSampleCnt);
		notify_error (1, "malloc*%d) failed", (int)nTrueSampleCnt);
		return WR_ERROR;
	}

	if( opMode == OPMODE_REMOTE ){
		kLog (K_MON, "[proc_sendData2Tree] connect : task(%s) mode(%s) mdsip(%s)\n",
				pSTDdev->taskName,
				getModeMsg(opMode),
				pSTDdev->ST_mds.treeIPport[opMode]);

		if (mds_Connect(pSTDdev) == WR_ERROR) {
			set_ioc_mds_stat (MDS_CONN_ERR);

			kLog (K_ERR, "[proc_sendData2Tree] connect fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
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

	kLog (K_MON, "[proc_sendData2Tree] open : task(%s) tree(%s) shot(%d)\n",
			pSTDdev->taskName,
			pSTDdev->ST_mds.treeName[opMode], pSTDdev->ST_Base_fetch.shotNumber);

	if (mds_Open_withFetch(pSTDdev) == WR_ERROR) {
		set_ioc_mds_stat (MDS_FILE_ERR);

		kLog (K_ERR, "[proc_sendData2Tree] open fail : task(%s) mode(%s) mdsip(%s) tree(%s) shot(%d)\n",
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

	set_ioc_mds_stat (MDS_ARCHIVING);

	for (i = 0; i<INIT_MAX_CHAN_NUM; i++) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
		if (NULL == (fpRaw = pNI6122->ST_Ch[i].write_fp)) {
			continue;
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) mdsNode(%s) fp(%p) size(%d)\n",
				pNI6122->ST_Ch[i].path_name, pNI6122->ST_Ch[i].strTagName, pNI6122->ST_Ch[i].write_fp, nDataBufSize);

		// rewind
		fseek (fpRaw, 0L, SEEK_SET);

		if (1 != fread (databuf, nDataBufSize, 1, fpRaw)) {
			set_ioc_mds_stat (MDS_FILE_ERR);

			kLog (K_ERR, "[proc_sendData2Tree] %s : fread failed (%s)\n", pNI6122->ST_Ch[i].file_name, strerror(errno));
			notify_error (1, "%s read failed\n", pNI6122->ST_Ch[i].file_name);

			fclose(fpRaw);
			pNI6122->ST_Ch[i].write_fp = NULL;
			continue;
		}

		if (TRUE == isPrintDebugMsg (K_DEL)) {
			for (j=0;j<nTrueSampleCnt;j++) {
				if (! (j % 10)) printf ("\n(%04d) ", j);
				printf ("%5d ", databuf[j]);
			}
			printf ("\n");
		}

		//fclose(fpRaw);
		//pNI6122->ST_Ch[i].write_fp = NULL;
#endif

		rawDataDesc = descr(&dypte_Short, databuf, &nTrueSampleCnt, &_null);

		status = MdsPut(pNI6122->ST_Ch[i].strTagName,
				"BUILD_SIGNAL(BUILD_WITH_UNITS($*$,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&voltRangeDesc, &rawDataDesc, &endDesc, &startDesc, &rateDesc, &_null);

		if ( !((status) & 1) ) {
			set_ioc_mds_stat (MDS_FILE_ERR);

			kLog (K_ERR, "[proc_sendData2Tree] %s : Error tag\"%s\", ch%d: %s\n",
					pSTDdev->taskName, pNI6122->ST_Ch[i].strTagName, (int)i, MdsGetMsg(status));
			notify_error (1, "MdsPut(%s) failed", pNI6122->ST_Ch[i].strTagName);
		} 
		else {
			scanIoRequest(pSTDdev->ioScanPvt_userCall);
		}

		kLog (K_INFO, "[proc_sendData2Tree] path(%s) put ok...\n", pNI6122->ST_Ch[i].path_name);
	}

	kLog (K_MON, "[proc_sendData2Tree] %s : Tree Put Done!\n", pSTDdev->taskName);

	free(databuf);

	// put values : GAIN/FILTER/TEMP/...
	proc_sendData2Tree_values (pSTDdev);

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

	if (MDS_ARCHIVING == get_ioc_mds_stat()) {
		set_ioc_mds_stat (MDS_NORMAL);
	}

	return WR_OK;
}

int proc_mds_close_files (ST_STD_device *pSTDdev)
{
	ST_NI6122	*pNI6122	= (ST_NI6122 *)pSTDdev->pUser;
	int	i;

	for (i = 0; i < INIT_MAX_CHAN_NUM; i++) {
#if !TEST_WITHOUT_DAQ	// for testing without DAQ
		if (NULL != pNI6122->ST_Ch[i].write_fp) {
			fclose (pNI6122->ST_Ch[i].write_fp);
			pNI6122->ST_Ch[i].write_fp = NULL;
		}
#endif
	}

	return WR_OK;
}

int proc_sendData2Tree (ST_STD_device *pSTDdev)
{
	proc_sendData2Tree_sub (OPMODE_LOCAL, pSTDdev);

	if (pSTDdev->ST_Base.opMode == OPMODE_REMOTE) {
		proc_sendData2Tree_sub (OPMODE_REMOTE, pSTDdev);
	}

	proc_mds_close_files (pSTDdev);

	return WR_OK;
}

