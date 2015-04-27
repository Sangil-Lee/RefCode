/****************************************************************************
 * kupaMDS.cpp
 * --------------------------------------------------------------------------
 * MDSplus API
 * --------------------------------------------------------------------------
 * Copyright(c) 2013 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2013.05.13  yunsw        Initial revision
 ****************************************************************************/

#include "kutilObj.h"
#include "kupaMDS.h"

kupaMDS::kupaMDS ()
{
	kuDebug (kuINFO, "[kupaMDS::kupaMDS] object was created\n");

	init ();
}

kupaMDS::~kupaMDS ()
{
	disConnect ();
}

const kuBoolean kupaMDS::init ()
{
	m_socket		= -1;
	m_bConnected	= kuFALSE;
	m_bOpened		= kuFALSE;
	m_status		= 1;
	m_nShotNo		= 0;

	return (kuTRUE);
}

const kuBoolean kupaMDS::set (string treeName, string mdsAddr, string eventName, string eventAddr)
{
	m_treeName		= treeName;
	m_mdsAddr		= mdsAddr;
	m_eventName		= eventName;
	m_eventAddr		= eventAddr;

	printInfo (kuINFO, "set");

	return (kuTRUE);
}

void kupaMDS::printInfo (const int nPrintLevel, const char *name)
{
	kuDebug (nPrintLevel, "[kupaMDS::%s] tree(%s) addr(%s) event name(%s) addr(%s)\n",
			name, m_treeName.c_str(), m_mdsAddr.c_str(), m_eventName.c_str(), m_eventAddr.c_str());
}

const kuBoolean kupaMDS::connect ()
{
	disConnect ();

	kuDebug (kuINFO, "[kupaMDS::connect] mdsAddr(%s) : connecting ...\n", m_mdsAddr.c_str());

	m_socket = MdsConnect ((char *)m_mdsAddr.c_str());

	if (-1 == m_socket) {
		kuDebug (kuERR, "[kupaMDS::connect] MdsConnect() failed for %s\n", m_mdsAddr.c_str());
		return (kuFALSE);
	}

	setConnected (kuTRUE);

	kuDebug (kuINFO, "[kupaMDS::connect] mdsAddr(%s) : connected\n", m_mdsAddr.c_str());

	return (kuTRUE);
}

const kuBoolean kupaMDS::disConnect ()
{
	if (kuTRUE == isConnected ()) {
		kuDebug (kuINFO, "[kupaMDS::disConnect] mdsAddr(%s) : disconnecting ...\n", m_mdsAddr.c_str());

		if (kuTRUE == isOpened ()) {
			close ();
		}

		MdsDisconnect();

		setConnected (kuFALSE);
	}

	return (kuTRUE);	
}

const kuBoolean kupaMDS::open (const int nShotNo)
{
	if (kuTRUE != isConnected ()) {
		kuDebug (kuERR, "[kupaMDS::open] mdsAddr(%s) : not connected\n", m_mdsAddr.c_str());
		return (kuFALSE);
	}

	if (kuTRUE != isOK (MdsOpen((char *)m_treeName.c_str(), (int *)&nShotNo))) {
		kuDebug (kuERR, "[kupaMDS::open] mdsAddr(%s) tree(%s) shot(%d) : open failed\n",
					m_mdsAddr.c_str(), m_treeName.c_str(), nShotNo);
		disConnect ();
		return (kuFALSE);
	}

	setOpened (kuTRUE);
	setShotNo (nShotNo);

	kuDebug (kuINFO, "[kupaMDS::open] mdsAddr(%s) tree(%s) shot(%d) : ok\n",
				m_mdsAddr.c_str(), m_treeName.c_str(), getShotNo());
		
	return (kuTRUE);
}

const kuBoolean kupaMDS::close ()
{
	if (kuTRUE == isOpened ()) {
		if (kuTRUE != isOK (MdsClose ((char *)m_treeName.c_str(), &m_nShotNo))) {
			kuDebug (kuERR, "[kupaMDS::close] mdsAddr(%s) tree(%s) shot(%d) : close failed\n",
						m_mdsAddr.c_str(), m_treeName.c_str(), getShotNo());
			disConnect();
			return (kuFALSE);
		}
	}

	setOpened (kuFALSE);

	kuDebug (kuINFO, "[kupaMDS::close] mdsAddr(%s) tree(%s) shot(%d) : close ok\n",
				m_mdsAddr.c_str(), m_treeName.c_str(), getShotNo());
		
	return (kuTRUE);
}

const kuBoolean kupaMDS::createTree (const int nShotNo)
{
	int null = 0; /* Used to mark the end of the argument list */
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG;
	int idesc = descr(&dtype_Long, &tstat, &null);

	kuDebug (kuINFO, "[kupaMDS::createTree] Create new tree(%d)... \n", nShotNo);

	if (kuTRUE != isConnected ()) {
		kuDebug (kuERR, "[kupaMDS::createTree] MDS tree not connected\n");
		return (kuFALSE);
	}

	sprintf(buf, "TCL(\"set tree %s/shot=-1\")", m_treeName.c_str());

	if (kuTRUE != isOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[kupaMDS::createTree] Error TCL command: %s.\n", getStatusMsg());
		disConnect();
		return (kuFALSE);
	}

	sprintf(buf, "TCL(\"create pulse %d\")", nShotNo);

	if (kuTRUE != isOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[kupaMDS::createTree] Error TCL command: create pulse: %s.\n", getStatusMsg());
		disConnect();
		return (kuFALSE);
	}

	sprintf(buf, "TCL(\"close\")");
	MdsValue(buf, &idesc, &null, &len);

	kuDebug (kuINFO, "[kupaMDS::createTree] Create new tree(%d)... OK\n", nShotNo);

	return (kuTRUE);	
}

const kuBoolean kupaMDS::write (const char *szTagName, const char *szUnit, const void *databuf,
		int nDataType, int nSamples, kuFloat64 fStartTime, kuFloat64 fTimeGap)
{
	int			_null = 0;	/* Used to mark the end of the argument list */
	int			status;		/* Will contain the status of the data access operation */

	int			dtype_Float		= DTYPE_DOUBLE;
	kuFloat64	fEndIdx			= nSamples - 1;

	int			startTimeDesc	= descr(&dtype_Float, &fStartTime, &_null);	// T0 - Blip
	int			endIdxDesc		= descr(&dtype_Float, &fEndIdx, &_null);
	int			rateDesc		= descr(&dtype_Float, &fTimeGap, &_null);
	int			valueDesc		= descr(&nDataType  , (void *)databuf, &nSamples, &_null);

	kuDebug (kuINFO, "[kupaMDS::write] tag(%s) gap(%f) samples(%d) startTime(%f) ...\n",
			szTagName, fTimeGap, nSamples, fStartTime);

#if 0
	status = MdsPut ((char *)szTagName,
		"BUILD_SIGNAL(BUILD_WITH_UNITS($,\"V\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
				&valueDesc, &endIdxDesc, &startTimeDesc, &rateDesc, &_null);
#else
	char	expr[256];
	sprintf (expr,  "BUILD_SIGNAL(BUILD_WITH_UNITS($,\"%s\"),,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))))", 
			kuStd::nullprint(szUnit));

	status = MdsPut ((char *)szTagName, expr, 
				&valueDesc, &endIdxDesc, &startTimeDesc, &rateDesc, &_null);
#endif

	if (kuTRUE != isOK (status)) {
		kuDebug (kuERR, "[kupaMDS::write] Error tag(%s) : %s\n", szTagName, getStatusMsg());
		return (kuFALSE);
	} 

	kuDebug (kuINFO, "[kupaMDS::write] tag(%s) gap(%f) samples(%d) startTime(%f) OK\n",
			szTagName, fTimeGap, nSamples, fStartTime);

	return (kuTRUE);
}

const kuBoolean kupaMDS::write (const char *szTagName, const void *databuf)
{
	int 	_null = 0;	/* Used to mark the end of the argument list */
	int 	status;		/* Will contain the status of the data access operation */

	float	*pfValue	= (float *)databuf;
	char	szValueStr[64];

	sprintf (szValueStr, "%f",  pfValue[0]);

	kuDebug (kuINFO, "[kupaMDS::write] tag(%s) val(%s) ...\n", szTagName, szValueStr);

	status = MdsPut ((char *)szTagName, szValueStr, &_null);

	if (kuTRUE != isOK (status)) {
		kuDebug (kuERR, "[kupaMDS::write] Error tag(%s) : %s\n", szTagName, getStatusMsg());
		return (kuFALSE);
	} 

	kuDebug (kuINFO, "[kupaMDS::write] tag(%s) val(%s) OK\n", szTagName, szValueStr);

	return (kuTRUE);
}

const kuBoolean kupaMDS::sendEvent ()
{
	if ( ("" == m_eventAddr) || ("" == m_eventName) ) {
		return (kuTRUE);
	}

	kuDebug (kuINFO, "[kupaMDS::sendEvent] tree(%s) addr(%s) event(%s) connecting ...\n",
			m_treeName.c_str(), m_eventAddr.c_str(), m_eventName.c_str());

	int	socket = MdsConnect ((char *)m_eventAddr.c_str());

	if (-1 == socket) {
		kuDebug (kuERR, "[kupaMDS::sendEvent] MdsConnect() failed for %s\n", m_eventAddr.c_str());
		return (kuFALSE);
	}

	int null = 0; /* Used to mark the end of the argument list */
	int tstat, len;
	char buf[128];
	int dtype_Long = DTYPE_LONG;
	int idesc = descr(&dtype_Long, &tstat, &null);

	kuDebug (kuINFO, "[kupaMDS::sendEvent] setevent(%s)... \n", m_eventName.c_str());

	sprintf(buf, "TCL(\"setevent %s\")", m_eventName.c_str());

	if (kuTRUE != isOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[kupaMDS::sendEvent] Error TCL(%s): %s.\n", buf, getStatusMsg());
		MdsDisconnect();
		return (kuFALSE);
	}

	MdsDisconnect();

	kuDebug (kuINFO, "[kupaMDS::sendEvent] tree(%s) addr(%s) event(%s) ok\n",
			m_treeName.c_str(), m_eventAddr.c_str(), m_eventName.c_str());

	return (kuTRUE);
}

const kuInt32 kupaMDS::getSize (const char *szTagName)
{
	int null = 0;
	int len;
	int size;
	char buf[128];
	int dtype_long = DTYPE_LONG;
	int idesc = descr(&dtype_long, &size, &null);

	sprintf (buf, "SIZE(%s)", szTagName);

	if (kuTRUE != isOK (MdsValue(buf, &idesc, &null, &len))) {
		kuDebug (kuERR, "[kupaMDS::getSize] Error tag(%s) : %s\n", szTagName, getStatusMsg());
		return (kuNOK);
	}

	kuDebug (kuINFO, "[kupaMDS::getSize] tag(%s) size(%d) OK\n", szTagName, size);

	return (size);
}

const kuBoolean kupaMDS::getData (const char *szTagName, double *timebase, double *shotdata, int size)
{
	int null = 0;
	int len = 0;
	char buf[128];
	int dtype_float = DTYPE_DOUBLE;

	if (NULL == szTagName) {
		return (kuFALSE);
	}

	if (NULL != shotdata) {
		int sigdesc = descr(&dtype_float, shotdata, &size, &null);

		/* retrieve signal */
		if (kuTRUE != isOK (MdsValue((char *)szTagName, &sigdesc, &null, &len))) {
			kuDebug (kuERR, "[kupaMDS::getData] Error tag(%s) : %s\n", szTagName, getStatusMsg());
			return (kuFALSE);
		}

		kuDebug (kuINFO, "[kupaMDS::getData] data : tag(%s) len(%d) OK\n", szTagName, len);
	}

	if (NULL != timebase) {
		int timedesc = descr(&dtype_float, timebase, &size, &null);

		sprintf (buf, "DIM_OF(%s)", szTagName);

		/* retrieve timebase of signal */
		if (kuTRUE != isOK (MdsValue(buf, &timedesc, &null, &len))) {
			kuDebug (kuERR, "[kupaMDS::getData] Error tag(%s) : %s\n", szTagName, getStatusMsg());
			return (kuFALSE);
		}

		kuDebug (kuINFO, "[kupaMDS::getData] time : tag(%s) len(%d) OK\n", szTagName, len);
	}

	return (kuTRUE);
}

const kuBoolean kupaMDS::getAverage (const char *szTagName, int(*filter)(double), double &dAverage)
{
	dAverage = 0;

	int size;

	if (0 >= (size = getSize (szTagName))) {
		return (kuFALSE);
	}

	double *pdValues = NULL;

	if (NULL == (pdValues = (double *)malloc(size * sizeof(double)))) {
		return (kuFALSE);
	}

	if (kuTRUE != getData (szTagName, NULL, pdValues, size)) {
		free (pdValues);
		return (kuFALSE);
	}

	int i;
	double nCnt = 0;
	double dTotal = 0;

	for (i = 0; i < size; i++) {
		if (NULL != filter) {
			if (0 == filter (pdValues[i])) {
				// skip
				continue;
			}
		}

		nCnt++;
		dTotal += pdValues[i];
	}

	dAverage = dTotal / nCnt;

	free (pdValues);
	pdValues = NULL;

	return (kuTRUE);
}

const kuBoolean kupaMDS::getIntegral (const char *szTagName, double &dIntegral)
{
	dIntegral = 0;

	int size;

	if (0 >= (size = getSize (szTagName))) {
		return (kuFALSE);
	}

	double *pdValues = NULL;
	double *pdTimes = NULL;

	if (NULL == (pdValues = (double *)malloc(size * sizeof(double)))) {
		return (kuFALSE);
	}

	if (NULL == (pdTimes = (double *)malloc(size * sizeof(double)))) {
		free (pdValues);
		return (kuFALSE);
	}

	if (kuTRUE != getData (szTagName, pdTimes, pdValues, size)) {
		free (pdValues);
		free (pdTimes);
		return (kuFALSE);
	}

	int i;
	double dTotal = 0;
	double dTemp = 0;

	for (i = 1; i < size; i++) {
		dTemp = pdValues[i-1] * (pdTimes[i] - pdTimes[i-1]);
		dTotal += pdValues[i-1] * (pdTimes[i] - pdTimes[i-1]);
		//printf ("%d : Total(%f) : value(%f) * time(%f - %f) = %f\n",
		//		i, dTotal, pdValues[i-1], pdTimes[i], pdTimes[i-1], dTemp);
	}

	dIntegral = dTotal;

	free (pdValues);
	pdValues = NULL;

	free (pdTimes);
	pdTimes = NULL;

	return (kuTRUE);
}
