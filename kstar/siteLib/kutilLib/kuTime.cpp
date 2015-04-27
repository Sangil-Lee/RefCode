/****************************************************************************
 * kuTime.c
 * --------------------------------------------------------------------------
 * data & time API
 * --------------------------------------------------------------------------
 * Copyright(c) 2012 NFRI. All Rights Reserved.
 * --------------------------------------------------------------------------
 * 2012.07.03  yunsw        Initial revision
 ****************************************************************************/

#include "kuTime.h"

// Epoch is 00:00:00 Jan 1, 1970 UTC
kuTimeStamp kuTimeCurrrent (kuUInt32 *sec, kuUInt32 *nsec)
{
	kuTimeStamp epics_time;
	epicsTimeGetCurrent (&epics_time);

	// convert EPICS Epoch (1990/01/01 00:00:00) to Epoch (1970/01/01 00:00:00)
	if (sec)	*sec  = epics_time.secPastEpoch + POSIX_TIME_AT_EPICS_EPOCH;

	if (nsec)	*nsec = epics_time.nsec;

	return (epics_time);
}

kuUInt32 kuTimeEpoch ()
{
	kuUInt32	sec;
	kuTimeCurrrent (&sec, NULL);

	return (sec);
}

kuDateTime::kuDateTime ()
{
	kuInt32		sec;
	kuInt32		nsec;

	getCurrDateTime (sec, nsec);

	setDateDelimiter (DATE_DEFAULT_DELIMITER);
	setTimeDelimiter (TIME_DEFAULT_DELIMITER);

	initDateTime (sec, nsec);
}

kuDateTime::kuDateTime (const kuInt32 sec)
{
	setDateDelimiter (DATE_DEFAULT_DELIMITER);
	setTimeDelimiter (TIME_DEFAULT_DELIMITER);

	initDateTime (sec, 0);
}

kuDateTime::kuDateTime (const kuInt32 sec, const kuInt32 nsec)
{
	setDateDelimiter (DATE_DEFAULT_DELIMITER);
	setTimeDelimiter (TIME_DEFAULT_DELIMITER);

	initDateTime (sec, nsec);
}

kuDateTime::kuDateTime (const kuDateTime &rh)
{
	*this = rh;
}

kuDateTime::~kuDateTime ()
{
}

void kuDateTime::initDateTime (const kuInt32 sec, const kuInt32 nsec)
{
#ifdef SYS_DEBUG
	printf ("[kuDateTime::init before] sec = %d, nsec = %d\n", sec, nsec);
#endif

	m_sec  = sec;
	m_nsec = nsec;
	m_msec = getMillisFromNanos (nsec);

	getDateTimeFields (m_sec, m_dateTimeFields);

#ifdef SYS_DEBUG
	printf ("[kuDateTime::init after] m_sec = %d, m_msec = %d, m_nsec = %d\n", m_sec, m_msec, m_nsec);
	printf ("[kuDateTime::init after] tm_sec = %d\n", m_dateTimeFields.tm_sec);
#endif
}

void kuDateTime::updateDateTime (DateTimeUpdate_e dateTimeUpdate)
{
	if (DATETIME_UPDATE != dateTimeUpdate) {
		return;
	}

	int		sec, nsec;

	getCurrDateTime (sec, nsec);
	initDateTime (sec, nsec);
}

void kuDateTime::setTime (const kuInt32 sec)
{
	initDateTime (sec, 0);
}

void kuDateTime::setTime (const kuInt32 sec, const kuInt32 nsec)
{
	initDateTime (sec, nsec);
}

void kuDateTime::setTime (const kuDateTime &dateTime)
{
	initDateTime (dateTime.m_sec, dateTime.m_nsec);
}

const char *kuDateTime::getLongDateTimeStr (DateTimeUpdate_e dateTimeUpdate)
{
	updateDateTime (dateTimeUpdate);

	return (getLongDateTimeStr (m_sec));
}

const char *kuDateTime::getDateTimeStr (DateTimeUpdate_e dateTimeUpdate)
{
	updateDateTime (dateTimeUpdate);

	return (getDateTimeStr (m_dateTimeFields, m_nsec, m_dateFormat, m_dateDelimiter, m_timeFormat, m_timeDelimiter));
}

const char *kuDateTime::getDateStr (DateTimeUpdate_e dateTimeUpdate)
{
	updateDateTime (dateTimeUpdate);

	return (getDateStr (m_dateTimeFields, m_dateFormat, m_dateDelimiter));
}

const char *kuDateTime::getTimeStr (DateTimeUpdate_e dateTimeUpdate)
{
	updateDateTime (dateTimeUpdate);

	return (getTimeStr (m_dateTimeFields, m_nsec, m_timeFormat, m_timeDelimiter));
}

const char *kuDateTime::getCurrLongDateTimeStr ()
{
	kuInt32		sec, nsec;

	getCurrDateTime (sec, nsec);

	return (getLongDateTimeStr (sec));
}

const char *kuDateTime::getCurrDateTimeStr (const DateFormat_e dateFormat, const char dateDelimiter, 
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	kuInt32		sec, nsec;
	struct tm	dateTimeFields;

	getCurrDateTime (sec, nsec);

	getDateTimeFields (sec, dateTimeFields);

	return (getDateTimeStr (dateTimeFields, nsec, dateFormat, dateDelimiter, timeFormat, timeDelimiter));
}

const char *kuDateTime::getCurrDateStr (const DateFormat_e dateFormat, const char dateDelimiter)
{
	kuInt32		sec, nsec;
	struct tm	dateTimeFields;

	getCurrDateTime (sec, nsec);

	getDateTimeFields (sec, dateTimeFields);

	return (getDateStr (dateTimeFields, dateFormat, dateDelimiter));
}

const char *kuDateTime::getCurrTimeStr (const TimeFormat_e timeFormat, const char timeDelimiter)
{
	kuInt32		sec, nsec;
	struct tm	dateTimeFields;

	getCurrDateTime (sec, nsec);

	getDateTimeFields (sec, dateTimeFields);

	return (getTimeStr (dateTimeFields, nsec, timeFormat, timeDelimiter));
}

const char *kuDateTime::getLongDateTimeStr (const kuInt32 sec)
{
	static char dateTimeFmtBuf[50];

	time_t	tsec	= sec;
	ctime_r (&tsec, dateTimeFmtBuf);

	return (dateTimeFmtBuf);
}

const char *kuDateTime::getDateTimeStr (const struct tm &dateTimeFields, const kuInt32 nsec,
						const DateFormat_e dateFormat, const char dateDelimiter, 
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	static char dateTimeFmtBuf[40];

	return ( getDateTimeStr (dateTimeFmtBuf, dateTimeFields, nsec, dateFormat, dateDelimiter, timeFormat, timeDelimiter) );
}

const char *kuDateTime::getDateTimeStr (char *dateTimeFmtBuf, const struct tm &dateTimeFields, const kuInt32 nsec,
						const DateFormat_e dateFormat, const char dateDelimiter, 
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	if (NULL == dateTimeFmtBuf) {
		return (NULL);
	}

	char		dateFmtBuf[20];
	char		timeFmtBuf[20];

	dateFmtBuf[0] = timeFmtBuf[0] = 0x00;

	getDateStr (dateFmtBuf, dateTimeFields, dateFormat, dateDelimiter);
	getTimeStr (timeFmtBuf, dateTimeFields, nsec, timeFormat, timeDelimiter);

	if ((DATE_NONE == dateFormat) || (TIME_NONE == timeFormat)
			|| (NONE_DELIMITER == dateDelimiter) || (NONE_DELIMITER == timeDelimiter) ) {
		sprintf (dateTimeFmtBuf, "%s%s", dateFmtBuf, timeFmtBuf);
	}
	else {
		sprintf (dateTimeFmtBuf, "%s %s", dateFmtBuf, timeFmtBuf);
	}

	return (dateTimeFmtBuf);
}

const char *kuDateTime::getDateStr (const struct tm &dateTimeFields, const DateFormat_e dateFormat, const char dateDelimiter)
{
	static char dateFmtBuf[20];

	return ( getDateStr (dateFmtBuf, dateTimeFields, dateFormat, dateDelimiter) );
}

const char *kuDateTime::getDateStr (char *dateFmtBuf, const struct tm &dateTimeFields, const DateFormat_e dateFormat, const char dateDelimiter)
{
	if (NULL == dateFmtBuf) {
		return (NULL);
	}

	char tempYear[10];
	char tempMonth[10];
	char tempDay[10];

	dateFmtBuf[0] = 0x00;

	if (DATE_NONE == dateFormat) {
		return (dateFmtBuf);
	}

	tempYear[0] = tempMonth[0] = tempDay[0] = 0x00;

	if ((DATE_YYYYMMDD == dateFormat) || (DATE_MMDDYYYY == dateFormat) || (DATE_YYYYMM == dateFormat) ) {
		sprintf (tempYear, "%04d", dateTimeFields.tm_year);
	}
	else {
		sprintf (tempYear, "%02d", (dateTimeFields.tm_year % 100));
	}

	if (DATE_DD != dateFormat) {
		sprintf (tempMonth, "%02d", dateTimeFields.tm_mon);
	}

	sprintf (tempDay, "%02d", dateTimeFields.tm_mday);

	switch (dateFormat) {
		case DATE_DD :
			sprintf (dateFmtBuf, "%s", tempDay);
			break;

		case DATE_MMDD :
			if (NONE_DELIMITER == dateDelimiter) {
				sprintf (dateFmtBuf, "%s%s", tempMonth, tempDay);
			}
			else {
				sprintf (dateFmtBuf, "%s%c%s", tempMonth, dateDelimiter, tempDay);
			}
			break;

		case DATE_MMDDYY :
		case DATE_MMDDYYYY :
			if (NONE_DELIMITER == dateDelimiter) {
				sprintf (dateFmtBuf, "%s%s%s", tempMonth, tempDay, tempYear);
			}
			else {
				sprintf (dateFmtBuf, "%s%c%s%c%s", tempMonth, dateDelimiter, tempDay, dateDelimiter, tempYear);
			}
			break;

		case DATE_YYYYMM :
			if (NONE_DELIMITER == dateDelimiter) {
				sprintf (dateFmtBuf, "%s%s", tempYear, tempMonth);
			}
			else {
				sprintf (dateFmtBuf, "%s%c%s", tempYear, dateDelimiter, tempMonth);
			}
			break;

		case DATE_YYMMDD :
		case DATE_YYYYMMDD :
		default :
			if (NONE_DELIMITER == dateDelimiter) {
				sprintf (dateFmtBuf, "%s%s%s", tempYear, tempMonth, tempDay);
			}
			else {
				sprintf (dateFmtBuf, "%s%c%s%c%s", tempYear, dateDelimiter, tempMonth, dateDelimiter, tempDay);
			}
			break;
	}

	return (dateFmtBuf);
}

const char *kuDateTime::getTimeStr (const struct tm &dateTimeFields, const kuInt32 nsec, 
		const TimeFormat_e timeFormat, const char timeDelimiter)
{
	static char timeFmtBuf[20];

	return ( getTimeStr (timeFmtBuf, dateTimeFields, nsec, timeFormat, timeDelimiter) );
}

const char *kuDateTime::getTimeStr (char *timeFmtBuf, const struct tm &dateTimeFields, const kuInt32 nsec, 
		const TimeFormat_e timeFormat, const char timeDelimiter)
{
	if (NULL == timeFmtBuf) {
		return (NULL);
	}

	char tempHour[10];
	char tempMinute[10];
	char tempSecond[20];

	timeFmtBuf[0] = 0x00;

	if (TIME_NONE == timeFormat) {
		return (timeFmtBuf);
	}

	kuInt32 msec = getMillisFromNanos (nsec);

	tempHour[0] = tempMinute[0] = tempSecond[0] = 0x00;

	if ((TIME_SS == timeFormat) || (TIME_MMSS == timeFormat) || (TIME_HHMMSS == timeFormat)) {
		sprintf (tempSecond, "%02d", dateTimeFields.tm_sec);
	}
	else if ((TIME_mSS == timeFormat) || (TIME_MMmSS == timeFormat) || (TIME_HHMMmSS == timeFormat)) {
		if (NONE_DELIMITER == timeDelimiter) {
			sprintf (tempSecond, "%02d%03d", dateTimeFields.tm_sec, msec);
		}
		else {
			sprintf (tempSecond, "%02d%c%03d", dateTimeFields.tm_sec, timeDelimiter, msec);
		}
	}
	else {
		if (NONE_DELIMITER == timeDelimiter) {
			sprintf (tempSecond, "%02d%03d%d", dateTimeFields.tm_sec, msec, nsec);
		}
		else {
			sprintf (tempSecond, "%02d%c%03d%c%d", dateTimeFields.tm_sec, timeDelimiter, msec, timeDelimiter, nsec);
		}
	}

	sprintf (tempHour, "%02d", dateTimeFields.tm_hour);
	sprintf (tempMinute, "%02d", dateTimeFields.tm_min);

	switch (timeFormat) {
		case TIME_HHMM :
			break;

		default :
		case TIME_HHMMSS  :
		case TIME_HHMMmSS :
		case TIME_HHMMnSS :
			if (NONE_DELIMITER == timeDelimiter) {
				sprintf (timeFmtBuf, "%s%s%s", 
						tempHour, tempMinute, tempSecond);
			}
			else {
				sprintf (timeFmtBuf, "%s%c%s%c%s", 
						tempHour, timeDelimiter, tempMinute, timeDelimiter, tempSecond);
			}
			break;

		case TIME_MMSS  :
		case TIME_MMmSS :
		case TIME_MMnSS :
			if (NONE_DELIMITER == timeDelimiter) {
				sprintf (timeFmtBuf, "%s%s", tempMinute, tempSecond);
			}
			else {
				sprintf (timeFmtBuf, "%s%c%s", tempMinute, timeDelimiter, tempSecond);
			}
			break;

		case TIME_SS  :
		case TIME_mSS :
		case TIME_nSS :
			sprintf (timeFmtBuf, "%s", tempSecond);
			break;
	}

	return (timeFmtBuf);
}

int kuDateTime::getCurrDateTime ()
{
	kuInt32		sec, nsec;

	getCurrDateTime (sec, nsec);

	return (sec);
}

void kuDateTime::getCurrDateTimeMillis (kuInt32 &sec, kuInt32 &msec)
{
	kuInt32		nsec;

	getCurrDateTime (sec, nsec);
	msec	= nsec / 1000000;
}

void kuDateTime::getCurrDateTime (kuInt32 &sec, kuInt32 &nsec)
{
#if 0
	// sec
	time_t times = time(0);

	// micro
	struct timeval  stime;
	gettimeofday (&stime, NULL);

	// nano
	struct timespec ts;
	clock_gettime (CLOCK_REALTIME, &ts);
#endif

#if !defined(WIN32)
	struct timespec nanosTime;

	clock_gettime (CLOCK_REALTIME, &nanosTime);

	sec  = nanosTime.tv_sec;
	nsec = nanosTime.tv_nsec;
#else
	struct _timeb timebuffer;

	_ftime( &timebuffer );

	sec  = (kuInt32)timebuffer.time;
	nsec = timebuffer.millitm * 1000000;
#endif

#ifdef SYS_DEBUG
	printf ("[kuDateTime::getCurrDateTime after] sec = %d, nsec = %d\n", sec, nsec);
#endif
}

void kuDateTime::getDateTimeFields (const kuInt32 sec, struct tm &dateTimeFields)
{
	time_t	tsec	= sec;
	localtime_r (&tsec, &dateTimeFields);

	dateTimeFields.tm_year += 1900;
	dateTimeFields.tm_mon  += 1;
}

const kuInt32 kuDateTime::getTimeTerm (const kuDateTime &rh)
{
	int			msec;

	if (m_nsec < rh.m_nsec) {
		m_sec  -= 1;
		m_nsec += 1000000000;
	}

	msec = (m_sec - rh.m_sec) * 1000 + (m_nsec - rh.m_nsec) / 1000000;

#ifdef SYS_DEBUG
	printf ("[kuDateTime::getTimeTerm] (%d:%d - %d:%d) = %d\n", m_sec, m_nsec, rh.m_sec, rh.m_nsec, msec);
#endif

	return (msec);
}

const char * kuDateTime::getDateTimeStr (kuInt32 sec,
						const DateFormat_e dateFormat, const char dateDelimiter, 
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getDateTimeStr (tms, 0, dateFormat, dateDelimiter, timeFormat, timeDelimiter));
}

const char * kuDateTime::getDateStr (kuInt32 sec,
						const DateFormat_e dateFormat, const char dateDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getDateStr (tms, dateFormat, dateDelimiter));
}

const char * kuDateTime::getTimeStr (kuInt32 sec,
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getTimeStr (tms, 0, timeFormat, timeDelimiter));
}

const char * kuDateTime::getDateTimeStr (char *buf, kuInt32 sec,
						const DateFormat_e dateFormat, const char dateDelimiter, 
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getDateTimeStr (buf, tms, 0, dateFormat, dateDelimiter, timeFormat, timeDelimiter));
}

const char * kuDateTime::getDateStr (char *buf, kuInt32 sec,
						const DateFormat_e dateFormat, const char dateDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getDateStr (buf, tms, dateFormat, dateDelimiter));
}

const char * kuDateTime::getTimeStr (char *buf, kuInt32 sec,
						const TimeFormat_e timeFormat, const char timeDelimiter)
{
	struct tm	tms;

	getDateTimeFields (sec, tms);

	return (getTimeStr (buf, tms, 0, timeFormat, timeDelimiter));
}

const kuUInt32 kuDateTime::getEpochTime (const char *pTimeStr)
{
	if ( (NULL == pTimeStr) || (14 > strlen(pTimeStr)) ) {
		return (kuNOK);
	}

	char	timeStr[64];
	memset (timeStr, 0x00, sizeof(timeStr));

	for (kuUInt32 i = 0, idx = 0; i < strlen(pTimeStr); i++) {
		if (isdigit (pTimeStr[i])) {
			timeStr[idx++]	= pTimeStr[i];
		}
	}

	char		buf[8];
	struct tm	tms;

	memcpy (buf, timeStr + 0, 4);
	buf[4] = 0x00;
	tms.tm_year	= atoi(buf) - 1900;

	memcpy (buf, timeStr + 4, 2);
	buf[2] = 0x00;
	tms.tm_mon	= atoi(buf) - 1;

	memcpy (buf, timeStr + 6, 2);
	buf[2] = 0x00;
	tms.tm_mday	= atoi(buf);

	memcpy (buf, timeStr + 8, 2);
	buf[2] = 0x00;
	tms.tm_hour	= atoi(buf);

	memcpy (buf, timeStr + 10, 2);
	buf[2] = 0x00;
	tms.tm_min	= atoi(buf);

	memcpy (buf, timeStr + 12, 2);
	buf[2] = 0x00;
	tms.tm_sec	= atoi(buf);

	// 0 for Standard Time, 1 for Daylight Saving Time
	tms.tm_isdst = 0;

#ifdef SYS_DEBUG
	printf ("[kuDateTime::getTimeTerm] mktime : %s -> %d\n", timeStr, (int)mktime(&tms));
#endif

	return ( mktime(&tms) );
}

const kuInt32 kuDateTime::getTimeOfNextYear (const time_t epochTime, const kuInt32 nNextYear, const kuInt32 offset)
{
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_year	+= nNextYear;

	if (offset == 0) {
		timeStructure.tm_mon    = 0;
		timeStructure.tm_mday   = 1;
		timeStructure.tm_hour   = 0;
		timeStructure.tm_min    = 0;
		timeStructure.tm_sec    = 0;
		timeStructure.tm_isdst  = 0;
	}

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getTimeOfNextMonth (const time_t epochTime, const kuInt32 nNextMonth, const kuInt32 offset)
{
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_year   += (timeStructure.tm_mon + nNextMonth) / 12;
	timeStructure.tm_mon     = (timeStructure.tm_mon + nNextMonth) % 12;

	if (timeStructure.tm_mon < 0) {
		timeStructure.tm_year   -= 1;
		timeStructure.tm_mon    += 12;
	}

	if (offset == 0) {
		timeStructure.tm_mday   = 1;
		timeStructure.tm_hour   = 0;
		timeStructure.tm_min    = 0;
		timeStructure.tm_sec    = 0;
		timeStructure.tm_isdst  = 0;
	}

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getStartTimeOfDay (const time_t epochTime)
{    
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_hour   = 0;
	timeStructure.tm_min    = 0;
	timeStructure.tm_sec    = 0;
	timeStructure.tm_isdst  = 0;

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getEndTimeOfDay (const time_t epochTime)
{    
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_hour   = 23;
	timeStructure.tm_min    = 59;
	timeStructure.tm_sec    = 59;
	timeStructure.tm_isdst  = 0;

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getStartTimeOfMonth (const time_t epochTime)
{    
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_mday  	= 1;
	timeStructure.tm_hour   = 0;
	timeStructure.tm_min    = 0;
	timeStructure.tm_sec    = 0;
	timeStructure.tm_isdst  = 0;

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getEndTimeOfMonth (const time_t epochTime)
{    
	struct tm   timeStructure;

	localtime_r (&epochTime, &timeStructure);

	timeStructure.tm_mon	+= 1;

	if (timeStructure.tm_mon > 11) {
		timeStructure.tm_year	+= 1;
		timeStructure.tm_mon	= 0;
	}

	timeStructure.tm_mday  	= 1;
	timeStructure.tm_hour   = 0;
	timeStructure.tm_min    = 0;
	timeStructure.tm_sec    = 0;
	timeStructure.tm_isdst  = 0;

	return ( (int)mktime (&timeStructure) - 1);
}

const kuInt32 kuDateTime::getLastDayOfMonth (const kuInt32 year, const kuInt32 month)
{
	static	int	daytab[12]	= {31,28,31,30,31,30,31,31,30,31,30,31};

	// Feb
	if (1 == month) {
		// for leap year (intercalary year)
		if (year % 4   == 0)	return (29);
		if (year % 100 == 0)	return (28);
		if (year % 400 == 0)	return (29);
	}

	return (daytab[month]);
}

const kuInt32 kuDateTime::getTimeOfNextOneDay (const time_t epochTime, const kuInt32 direction, const kuInt32 offset)
{    
	struct tm   timeStructure;

	// get time structure
	localtime_r (&epochTime, &timeStructure);

	// prev or next one day of today
	timeStructure.tm_mday += (DATETIME_PREV_DAY == direction) ? -1 : 1;

	if (timeStructure.tm_mday < 1) {
		if (--timeStructure.tm_mon < 0) {
			timeStructure.tm_year	-= 1;
			timeStructure.tm_mon	+= 12;
		}

		// last day of month
		timeStructure.tm_mday	= getLastDayOfMonth (timeStructure.tm_year, timeStructure.tm_mon);
	}

	if (timeStructure.tm_mday > getLastDayOfMonth (timeStructure.tm_year, timeStructure.tm_mon)) {
		if (++timeStructure.tm_mon > 11) {
			timeStructure.tm_year	+= 1;
			timeStructure.tm_mon	-= 12;
		}

		// first day of month
		timeStructure.tm_mday	= 1;
	}

	if (offset == 0) {
		timeStructure.tm_hour   = 0;
		timeStructure.tm_min    = 0;
		timeStructure.tm_sec    = 0;
		timeStructure.tm_isdst  = 0;
	}

	return ( (int)mktime (&timeStructure) );
}

const kuInt32 kuDateTime::getTimeOfNextDay (const time_t epochTime, const kuInt32 nNextDay, const kuInt32 offset)
{    
	int		direction	= (0 < nNextDay) ? DATETIME_NEXT_DAY : DATETIME_PREV_DAY;
	int		sec			= (int)epochTime;

	for (int i = 0; i < abs(nNextDay); i++) {
		sec	= getTimeOfNextOneDay (sec, direction, offset);
	}

	return (sec);
}

const kuInt32 kuDateTime::getNextEpochTime (const DateTimeType_e type, const time_t epochTime, const kuInt32 nNext, const kuInt32 offset)
{
	if (DATETIME_DAY == type) {
		return ( getTimeOfNextDay (epochTime, nNext, offset) );
	}
	else if (DATETIME_MON == type) {
		return ( getTimeOfNextMonth (epochTime, nNext, offset) );
	}
	else if (DATETIME_YEAR == type) {
		return ( getTimeOfNextYear (epochTime, nNext, offset) );
	}
	else {
		return ((int)epochTime);
	}
}

