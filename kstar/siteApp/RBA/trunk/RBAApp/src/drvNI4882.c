#include "drvNI4882.h"
#include "ni488.h"

static long drvNI4882_io_report(int level);
static long drvNI4882_init_driver();

struct {
	long            number;
	DRVSUPFUN       report;
	DRVSUPFUN       init;
} drvNI4882 = {
	2,
	drvNI4882_io_report,
	drvNI4882_init_driver
};

epicsExportAddress(drvet, drvNI4882);

extern int isFirstDevice (ST_STD_device *pSTDdev);

ST_NI4882		*pNI4882 = NULL;

static int isGpibError ()
{
	return (ibsta & ERR);
}

static int GpibError (int Device, char *msg)
{
	char	stateStr[20]={'\0'};
	char	errStr[2048]={'\0'};

	if (ibsta & ERR )  strcpy (stateStr, " ERR");
	if (ibsta & TIMO)  strcpy (stateStr, " TIMO");
	if (ibsta & END )  strcpy (stateStr, " END");
	if (ibsta & SRQI)  strcpy (stateStr, " SRQI");
	if (ibsta & RQS )  strcpy (stateStr, " RQS");
	if (ibsta & CMPL)  strcpy (stateStr, " CMPL");
	if (ibsta & LOK )  strcpy (stateStr, " LOK");
	if (ibsta & REM )  strcpy (stateStr, " REM");
	if (ibsta & CIC )  strcpy (stateStr, " CIC");
	if (ibsta & ATN )  strcpy (stateStr, " ATN");
	if (ibsta & TACS)  strcpy (stateStr, " TACS");
	if (ibsta & LACS)  strcpy (stateStr, " LACS");
	if (ibsta & DTAS)  strcpy (stateStr, " DTAS");
	if (ibsta & DCAS)  strcpy (stateStr, " DCAS");

	if (iberr == EDVR) strcpy (errStr, " EDVR <Driver Error>\n");
	if (iberr == ECIC) strcpy (errStr, " ECIC <Not Controller-In-Charge>\n");
	if (iberr == ENOL) strcpy (errStr, " ENOL <No Listener>\n");
	if (iberr == EADR) strcpy (errStr, " EADR <Address error>\n");
	if (iberr == EARG) strcpy (errStr, " EARG <Invalid argument>\n");
	if (iberr == ESAC) strcpy (errStr, " ESAC <Not System Controller>\n");
	if (iberr == EABO) strcpy (errStr, " EABO <Operation aborted>\n");
	if (iberr == ENEB) strcpy (errStr, " ENEB <No GPIB board>\n");
	if (iberr == EDMA) strcpy (errStr, " EDMA <DMA Error>\n");   
	if (iberr == EOIP) strcpy (errStr, " EOIP <Async I/O in progress>\n");
	if (iberr == ECAP) strcpy (errStr, " ECAP <No capability>\n");
	if (iberr == EFSO) strcpy (errStr, " EFSO <File system error>\n");
	if (iberr == EBUS) strcpy (errStr, " EBUS <Command error>\n");
	if (iberr == ESRQ) strcpy (errStr, " ESRQ <SRQ stuck on>\n");
	if (iberr == ETAB) strcpy (errStr, " ETAB <Table Overflow>\n");
	if (iberr == ELCK) strcpy (errStr, " ELCK <Interface is locked>\n");
	if (iberr == EARM) strcpy (errStr, " EARM <ibnotify callback failed to rearm>\n");
	if (iberr == EHDL) strcpy (errStr, " EHDL <Input handle is invalid>\n");
	if (iberr == EWIP) strcpy (errStr, " EWIP <Wait in progress on specified input handle>\n");
	if (iberr == ERST) strcpy (errStr, " ERST <The event notification was cancelled due to a reset of the interface>\n");
	if (iberr == EPWR) strcpy (errStr, " EPWR <The interface lost power>\n");

	set_ioc_amp_stat (AMP_ERROR);

	kLog (K_ERR, "[NI4882] %s : sta(0x%x) err(%d) cntl(%d) :%s", msg, ibsta, iberr, ibcntl, errStr);
	notify_error (1, "[NI4882] %s :%s", msg, errStr);

	return (NOK);
}

int getAmpGainValue (int idx)
{
	switch (idx) {
		case 0 : return (20);
		case 1 : return (50);
		case 2 : return (100);
		case 3 : return (200);
		case 4 : return (500);
		case 5 : return (1000);
		case 6 : return (2000);
		case 7 : return (5000);
	}

	return (0);
}

int getAmpFilterValue (int idx)
{
	switch (idx) {
		case 0 : return (200);
		case 1 : return (1000);
		case 2 : return (2000);
		case 3 : return (4000);
	}

	return (0);
}

int ni4882_open (ST_NI4882_MCU *pMCU)
{
	kLog (K_MON, "[ni4882_open] board(%d) pri(%d) sec(%d) timeout(%d)\n",
			pMCU->boardIdx, pMCU->priAddr, pMCU->secAddr, pMCU->timeout);

	pMCU->device = ibdev (		/* Create a unit descriptor handle         */
			pMCU->boardIdx,		/* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
			pMCU->priAddr,		/* Device primary address                  */
			pMCU->secAddr,		/* Device secondary address                */
			pMCU->timeout,		/* Timeout setting (T10s = 10 seconds)     */
			NI4882_ASSERT_EOI,	/* Assert EOI line at end of write         */
			NI4882_MODE_EOS);

	if (isGpibError ()) {
		GpibError(pMCU->device, "ibdev");
		return (NOK);
	}

	ibclr (pMCU->device);

	if (isGpibError ()) {
		GpibError(pMCU->device, "ibclr");
		return (NOK);
	}

	kLog (K_MON, "[ni4882_open] board(%d) pri(%d) sec(%d) timeout(%d) dev(%d)\n",
		pMCU->boardIdx, pMCU->priAddr, pMCU->secAddr, pMCU->timeout, pMCU->device);

	return (OK);
}

int ni4882_close (ST_NI4882_MCU *pMCU)
{
	// take the device and interface offline
	ibonl (pMCU->device, 0);

	if (isGpibError ()) {
		GpibError(pMCU->device, "ibonl");
	}

	pMCU->device	= -1;

	return (OK);
}

int ni4882_comm (int nDevice, char *pszCommand, char *pszResult, int nBufSize)
{
#if 0	//TODO :TEST for without NI488.2
	return NOK;
#endif
	char	szBuf[256];
	strcpy (szBuf, pszCommand);
	szBuf[strlen(pszCommand)-1] = 0x00;

	kLog (K_ERR, "[ni4882_comm] dev(%d) cmd(%s)\n", nDevice, szBuf);

	// writes command
	ibwrt (nDevice, pszCommand, strlen(pszCommand));

	if (isGpibError ()) {
		GpibError (nDevice, "ibwrt");
		return (NOK);
	}

	// no result
	if ( (NULL == pszResult) || (0 >= nBufSize) ) {
		return (OK);
	}

	// reads result
	ibrd (nDevice, pszResult, nBufSize);

	if (isGpibError ()) {
		GpibError (nDevice, "ibrd");
		return (NOK);
	}

	pszResult[ibcntl-1]	= 0x00;		// Null terminate the ASCII string

	kLog (K_ERR, "[ni4882_comm] dev(%d) cmd(%s) : result(%s)\n", nDevice, szBuf, pszResult);

	return (OK);
}

static float64 getRawGradient (ST_NI4882 *pNI4882)
{
	return (NI4882_RAW_GRADIENT);
}

static float64 getEuGradient (ST_NI4882 *pNI4882)
{
	return (NI4882_EU_GRADIENT);
}

static float64 getRawValue (ST_NI4882 *pNI4882, float64 euValue)
{
	return ( euValue * pNI4882->rawGradient );
}

static float64 getEuValue (ST_NI4882 *pNI4882, float64 rawValue)
{
	return ( rawValue * pNI4882->euGradient );
}

int getGpibDevice (int nMCU)
{
	return (pNI4882->ST_MCU[nMCU].device);
}

ST_NI4882_MCU *getGpibMCU (int nMCU)
{
	return (pNI4882->ST_MCU + nMCU);
}

ST_NI4882_CH *getGpibChannel (int nMCU, int nChannel)
{
	ST_NI4882_MCU	*pMCU	= pNI4882->ST_MCU + nMCU;

	if (NULL == pMCU) {
		return (NULL);
	}

	return (pMCU->ST_CH + nChannel);
}

void putAmpData (char *pName, int nChNo, int val)
{
	char	szPVName[64];
	char	szPVVal[64];

	sprintf (szPVName, "%s_%02d", pName, nChNo);
	sprintf (szPVVal, "%d", val);

	kLog (K_ERR, "[putAmpData] %s : %s\n", szPVName, szPVVal);

	DBproc_put (szPVName, szPVVal);
}

int processAmpReset ()
{
	if (get_ioc_amp_stat() == AMP_READY) {
		kLog (K_MON, "[processAmpReset] amp is ready ... \n");
		return 0;
	}

	kLog (K_MON, "[processAmpReset] state of amp : %d\n", get_ioc_amp_stat());

	int		i;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nVal;

	set_ioc_amp_stat (AMP_RESET);

	// make command string
	sprintf (szCommand, "RESET\n");

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
			// process result
			sscanf (szResult, "%d,%d", &nCmd, &nVal);

			if (AMP_CMD_RESET != nCmd) {
				kLog (K_ERR, "[processAmpReset] Err(%s)\n", szResult);

				set_ioc_amp_stat (AMP_ERROR);
			}
		}

		ni4882_close (pMCU);
	}

#if 0
	processGetAmpGain ();
	processGetAmpFilter ();
	processGetAmpTemp ();
#endif

	if (get_ioc_amp_stat() != AMP_ERROR) {
		set_ioc_amp_stat (AMP_READY);
	}

	return 0;
}

int processAmpStart ()
{
	kLog (K_MON, "[processAmpStart] ...\n");

	int		i;
	int		nDevice;
	char	szCommand[64];
	//char	szResult[256];
	ST_NI4882_MCU	*pMCU;

#if 0
	int		nCmd;
	int		nVal;
#endif
	int		nResult	= OK;

	// make command string
	sprintf (szCommand, "START\n");

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		if (OK == ni4882_comm (nDevice, "RESET\n", NULL, 0)) {
		}

		//if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
		if (OK == ni4882_comm (nDevice, szCommand, NULL, 0)) {
#if 0
			// process result
			sscanf (szResult, "%d,%d", &nCmd, &nVal);

			if (AMP_CMD_START != nCmd) {
				nResult	= NOK;
				kLog (K_ERR, "[processAmpStart] Err(%s)\n", szResult);
			}
#endif
		}
		else {
			nResult	= NOK;
		}

		ni4882_close (pMCU);
	}

	if (OK != nResult) {
		processAmpReset ();
	}
	else {
		set_ioc_amp_stat (AMP_STARTED);
	}

	return 0;
}

int processGetAmpTemp ()
{
	kLog (K_MON, "[processGetAmpTemp] ...\n");

	int		i, j;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nTemp;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			// make command string
			sprintf (szCommand, "T1,C%d\n", j + 1);

			// write command
			if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
				// process result
				sscanf (szResult, "%d,%d", &nCmd, &nTemp);

				if (AMP_CMD_T1 != nCmd) {
					set_ioc_amp_stat (AMP_ERROR);
					kLog (K_ERR, "[processGetAmpTemp] MCU(%d) CH(%d) : Err(%s)\n", i, j, szResult);
					continue;
				}

				pMCU->ST_CH[j].temp	= nTemp;

				// put the retrieved value to PV
				putAmpData ("RBA_AMP_T", pMCU->ST_CH[j].chNo, nTemp);

				kLog (K_INFO, "[processGetAmpTemp] MCU(%d) CH(%d) : T(%d)\n", i, j, nTemp);
			}
		}

		ni4882_close (pMCU);
	}

	return 0;
}

int processGetAmpGain ()
{
	kLog (K_MON, "[processGetAmpGain] ...\n");

	int		i, j;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nGain;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			// make command string
			sprintf (szCommand, "GG,C%d\n", j + 1);

			// write command
			if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
				// process result
				sscanf (szResult, "%d,%d", &nCmd, &nGain);

				if (AMP_CMD_GG != nCmd) {
					set_ioc_amp_stat (AMP_ERROR);
					kLog (K_ERR, "[processGetAmpGain] MCU(%d) CH(%d) : Err(%s)\n", i, j, szResult);
					continue;
				}

				pMCU->ST_CH[j].gain	= nGain - 1;

				// put the retrieved value to PV
				putAmpData ("RBA_AMP_GAIN", pMCU->ST_CH[j].chNo, pMCU->ST_CH[j].gain);

				kLog (K_INFO, "[processGetAmpTemp] MCU(%d) CH(%d) : Gain(%d)\n", i, j, nGain);
			}
		}

		ni4882_close (pMCU);
	}

	return 0;
}

int processSetAmpGain ()
{
	kLog (K_MON, "[processSetAmpGain] ...\n");

	int		i, j;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nGain;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			// make command string
			sprintf (szCommand, "SG,C%d,%d\n", j + 1, pMCU->ST_CH[j].gain + 1);

			// write command
			if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
				// process result
				sscanf (szResult, "%d,%d", &nCmd, &nGain);

				if ( !((AMP_CMD_SG_MIN <= nCmd) && (nCmd <= AMP_CMD_SG_MAX)) ) {
					set_ioc_amp_stat (AMP_ERROR);
					kLog (K_ERR, "[processSetAmpGain] MCU(%d) CH(%d) : Err(%s)\n", i, j, szResult);
					continue;
				}

				kLog (K_INFO, "[processSetAmpTemp] MCU(%d) CH(%d) : Gain(%d)\n", i, j, nGain);
			}
		}

		ni4882_close (pMCU);
	}

	return 0;
}

int processGetAmpFilter ()
{
	kLog (K_MON, "[processGetAmpFilter] ...\n");

	int		i, j;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nFilter;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			// make command string
			sprintf (szCommand, "GF,C%d\n", j + 1);

			// write command
			if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
				// process result
				sscanf (szResult, "%d,%d", &nCmd, &nFilter);

				if (AMP_CMD_GF != nCmd) {
					set_ioc_amp_stat (AMP_ERROR);
					kLog (K_ERR, "[processGetAmpFilter] MCU(%d) CH(%d) : Err(%s)\n", i, j, szResult);
					continue;
				}

				pMCU->ST_CH[j].filter	= nFilter - 1;

				// put the retrieved value to PV
				putAmpData ("RBA_AMP_FLT", pMCU->ST_CH[j].chNo, pMCU->ST_CH[j].filter);

				kLog (K_INFO, "[processGetAmpFilter] MCU(%d) CH(%d) : Filter(%d)\n", i, j, nFilter);
			}
		}

		ni4882_close (pMCU);
	}

	return 0;
}

int processSetAmpFilter ()
{
	kLog (K_MON, "[processSetAmpFilter] ...\n");

	int		i, j;
	int		nDevice;
	char	szCommand[64], szResult[256];
	ST_NI4882_MCU	*pMCU;

	int		nCmd;
	int		nFilter;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= getGpibMCU (i);

		if (OK != ni4882_open (pMCU)) {
			continue;
		}

		nDevice	= pMCU->device;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			// make command string
			sprintf (szCommand, "SF,C%d,%d\n", j + 1, pMCU->ST_CH[j].filter + 1);

			// write command
			if (OK == ni4882_comm (nDevice, szCommand, szResult, sizeof(szResult))) {
				// process result
				sscanf (szResult, "%d,%d", &nCmd, &nFilter);

				if ( !((AMP_CMD_SF_MIN <= nCmd) && (nCmd <= AMP_CMD_SF_MAX)) ) {
					kLog (K_ERR, "[processSetAmpFilter] MCU(%d) CH(%d) : Err(%s)\n", i, j, szResult);
					continue;
				}

				kLog (K_INFO, "[processSetAmpFilter] MCU(%d) CH(%d) : Filter(%d)\n", i, j, nFilter);
			}
		}

		ni4882_close (pMCU);
	}

	return 0;
}

int resetDeviceAmp (ST_STD_device *pSTDdev)
{
	kLog (K_MON, "[resetDeviceAmp] %s \n", pSTDdev->taskName);

	if (TRUE != isFirstDevice(pSTDdev)) {
		return (WR_OK);
	}

	DBproc_put (PV_AMP_RESET_STR, "1");
	DBproc_put (PV_AMP_START_STR, "0");

	return (0);
}

int startDeviceAmp (ST_STD_device *pSTDdev)
{
	kLog (K_MON, "[startDeviceAmp] %s \n", pSTDdev->taskName);

	if (TRUE != isFirstDevice(pSTDdev)) {
		return (WR_OK);
	}

	DBproc_put (PV_AMP_START_STR, "1");
	DBproc_put (PV_AMP_RESET_STR, "0");

	return (0);
}

static long drvNI4882_init_driver(void)
{
	kLog (K_TRACE, "[drvNI4882_init_driver] ...\n");

	//ST_NI4882		*pNI4882 = NULL;
	ST_NI4882_MCU	*pMCU = NULL;
	int i, j;

	pNI4882 = (ST_NI4882*) calloc(1, sizeof(ST_NI4882));
	if(!pNI4882) return WR_ERROR;
	
	pNI4882->taskHandle = 0x0;

	for (i = 0; i < NI4882_MAX_MCU; i++) {
		pMCU	= pNI4882->ST_MCU + i;

		pMCU->device	= -1;
		pMCU->boardIdx	= 0;
		pMCU->priAddr	= 1 + i;
		pMCU->secAddr	= 0;
		pMCU->timeout	= NI4882_TIME_OUT;

		for (j = 0; j < NI4882_MAX_CHAN; j++) {
			pMCU->ST_CH[j].chNo		= i * NI4882_MAX_CHAN + j + 1;
			pMCU->ST_CH[j].gain		= 0;
			pMCU->ST_CH[j].filter	= 0;
			pMCU->ST_CH[j].temp		= 0;
			pMCU->ST_CH[j].euValue	= 0;
		}
	}

	pNI4882->rawGradient	= getRawGradient (pNI4882);
	pNI4882->euGradient		= getEuGradient (pNI4882);

	kLog (K_DEBUG, "[drvNI4882_init_driver] initialized.... OK!\n");

	return 0;
}

static long drvNI4882_io_report(int level)
{
	kLog (K_TRACE, "[drvNI4882_io_report] ...\n");

	return 0;
}
