#include <math.h>

#include "kutil.h"
#include "kutilObj.h"
#include "kupaLib.h"
#include "kupaMDS.h"

#include "sfwCommon.h"
#include "nbiCommon.h"

extern "C" void lockMDS ();
extern "C" void unlockMDS ();

int proc_cal_db_put (const char *pname, const double value)
{
	char buf[65];
	sprintf (buf, "%f", value);

	kLog (K_INFO, "[proc_cal_db_put] %s : %f\n", pname, value);

	return (db_put (pname, buf));
}

int proc_cal_alarm_filter (double val)
{
	return ( (val >= 0.1) ? 1 : 0 );
}

int proc_cal_alarm_filter_current (double val)
{
	return ( (val >= 10.0) ? 1 : 0 );
}

extern "C" int proc_cal_alarm (char *pszTreeName, char *pszTreeIPport, int nShotNumber)
{
	char	szTagName[64];
	double	dValue;

	lockMDS ();

	kupaMDS	mdsAccess;

	mdsAccess.set (pszTreeName, pszTreeIPport, "", "");
	mdsAccess.connect ();
	mdsAccess.open (nShotNumber);
	//mdsAccess.open (8004753);		// for test

#if 0
	strcpy (szTagName, "\\NB11_IFS");
	mdsAccess.getAverage (szTagName, NULL, dValue);
	proc_cal_db_put ("NB1_LODAQ_IS1_IFS", dValue);

	strcpy (szTagName, "\\NB11_PNB");
	mdsAccess.getAverage (szTagName, proc_cal_alarm_filter, dValue);
	proc_cal_db_put ("NB1_LODAQ_IS1_PNB", dValue);

	strcpy (szTagName, "\\NB12_IFS");
	mdsAccess.getAverage (szTagName, NULL, dValue);
	proc_cal_db_put ("NB1_LODAQ_IS2_IFS", dValue);

	strcpy (szTagName, "\\NB12_PNB");
	mdsAccess.getAverage (szTagName, proc_cal_alarm_filter, dValue);
	proc_cal_db_put ("NB1_LODAQ_IS2_PNB", dValue);
#endif

	strcpy (szTagName, "\\NB11_IG1");
	//mdsAccess.getIntegral (szTagName, dValue);
	mdsAccess.getAverage (szTagName, proc_cal_alarm_filter_current, dValue);
	if (isnan(dValue))	dValue = 0;
	proc_cal_db_put ("NB1_LODAQ_IS1_IG1", dValue);

	strcpy (szTagName, "\\NB12_IG1");
	//mdsAccess.getIntegral (szTagName, dValue);
	mdsAccess.getAverage (szTagName, proc_cal_alarm_filter_current, dValue);
	if (isnan(dValue))	dValue = 0;
	proc_cal_db_put ("NB1_LODAQ_IS2_IG1", dValue);

	mdsAccess.close ();
	mdsAccess.disConnect ();

	unlockMDS ();

	return WR_OK;
}

