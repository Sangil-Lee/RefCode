/* recTimestamp.c */
/* recTimestamp.c - Record Support Routines for Timestamp records */
/*
 *      Original Author:  Susumu Yoshida
 *      Current Author: 
 *      Date:            10-12-98
 *
 * Modification Log:
 * -----------------
 *      8-Aug-2003 saa - Port to 3.14.
 *      2-Jun-2000 rpc - Use tsStampToLocal to get daylight savings time
 *                       (involves changes to tsSubr.c and tsDefs.h)
 *                       Remove superfluous subroutines
 *                       Add more formats (see also timestampRecord.dbd)
 */

#include        "epicsVersion.h"
#if EPICS_VERSION >= 3 && EPICS_REVISION >= 14
#include        "epicsTime.h"
#else
#include	<vxWorks.h>
#include	<types.h>
#include	<stdioLib.h>
#include	<lstLib.h>
#include        <tsDefs.h> 
#endif
#include	<string.h>

#include	"dbDefs.h"
#include	"dbAccess.h"
#include	"dbFldTypes.h"
#include	"devSup.h"
#include	"recSup.h"
#include	"recGbl.h"
#include        "dbEvent.h"    /* for db_post_events() */

#define GEN_SIZE_OFFSET
#include	"timestampRecord.h"
#undef  GEN_SIZE_OFFSET

/* Create RSET - Record Support Entry Table*/
#define report NULL
#define initialize NULL
#define init_record NULL
static long process();
static void monitor();
#define special NULL
#define cvt_dbaddr NULL
#define get_array_info NULL
#define put_array_info NULL
#define get_units NULL
#define get_precision NULL
#define get_enum_str NULL
#define get_enum_strs NULL
#define put_enum_str NULL
#define get_graphic_double NULL
#define get_control_double NULL
#define get_alarm_double NULL

#if EPICS_VERSION >= 3 && EPICS_REVISION >= 14 && EPICS_MODIFICATION > 4
#include "epicsExport.h"
#else
struct
#endif
rset timestampRSET={
	RSETNUMBER,
	report,
	initialize,
	init_record,
	process,
	special,
	cvt_dbaddr,
	get_array_info,
	put_array_info,
	get_units,
	get_precision,
	get_enum_str,
	get_enum_strs,
	put_enum_str,
	get_graphic_double,
	get_control_double,
	get_alarm_double };

#if EPICS_VERSION >= 3 && EPICS_REVISION >= 14
#if EPICS_MODIFICATION > 4
epicsExportAddress(rset,timestampRSET);
#endif
#else
static char MONTH[12][4]=
 {"JAN","FEB","MAR","APR","MAY","JUN","JUL","AUG","SEP","OCT","NOV","DEC"};
#endif

static long process(struct timestampRecord *ptimestamp){

#if EPICS_VERSION >= 3 && EPICS_REVISION >= 14
#else
  struct tsDetail time_s;
#endif
  
  ptimestamp->udf  = FALSE;
  ptimestamp->pact = TRUE;

  recGblGetTimeStamp(ptimestamp);
  ptimestamp->sec  = ptimestamp->time.secPastEpoch;
  ptimestamp->nsec = ptimestamp->time.nsec;
  
#if EPICS_VERSION >= 3 && EPICS_REVISION >= 14  
  switch(ptimestamp->tst) {
  case timestampTST_YY_MM_DD_HH_MM_SS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%y/%m/%d %H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_MM_DD_YY_HH_MM_SS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%m/%d/%y %H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_MM_DD_HH_MM_SS_YY:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%b %d %H:%M:%S %y",&ptimestamp->time);
    break;
  case timestampTST_MM_DD_HH_MM_SS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%b %d %H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_HH_MM_SS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_HH_MM:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%H:%M",&ptimestamp->time);
    break;
  case timestampTST_DD_MM_YY_HH_MM_SS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%d/%m/%y %H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_DD_MM_HH_MM_SS_YY:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%d %b %H:%M:%S %y",&ptimestamp->time);
    break;
  case timestampTST_VMS:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%e-%b-%Y %H:%M:%S",&ptimestamp->time);
    break;
  case timestampTST_MM_DD_YYYY:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%b %d, %Y %H:%M:%S.%04f",&ptimestamp->time);
    break;
  case timestampTST_MM_DD_YY:
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%m/%d/%y %H:%M:%S.%04f",&ptimestamp->time);
    break;
  default :  /* YY/MM/DD HH:MM:SS */
    epicsTimeToStrftime(ptimestamp->val,sizeof(ptimestamp->val),
                        "%y/%m/%D %H:%M:%S",&ptimestamp->time);
    break;
  }
#else
  tsStampToLocal(ptimestamp->time, &time_s);
  
  switch(ptimestamp->tst) {
  case timestampTST_YY_MM_DD_HH_MM_SS:
    sprintf(ptimestamp->val,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d"
	    ,time_s.year,time_s.monthNum+1,time_s.dayMonth+1
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_MM_DD_YY_HH_MM_SS:
    sprintf(ptimestamp->val,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d"
	    ,time_s.monthNum+1,time_s.dayMonth+1,time_s.year
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_MM_DD_HH_MM_SS_YY:
    sprintf(ptimestamp->val,"%s %.2d %.2d:%.2d:%.2d %.2d"
	    ,MONTH[time_s.monthNum],time_s.dayMonth+1
	    ,time_s.hours,time_s.minutes,time_s.seconds,time_s.year);
    break;
  case timestampTST_MM_DD_HH_MM_SS:
    sprintf(ptimestamp->val,"%s %.2d %.2d:%.2d:%.2d"
	    ,MONTH[time_s.monthNum],time_s.dayMonth+1
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_HH_MM_SS:
    sprintf(ptimestamp->val,"%.2d:%.2d:%.2d"
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_HH_MM:
    sprintf(ptimestamp->val,"%.2d:%.2d"
	    ,time_s.hours,time_s.minutes);
    break;
  case timestampTST_DD_MM_YY_HH_MM_SS:
    sprintf(ptimestamp->val,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d"
	    ,time_s.dayMonth+1,time_s.monthNum+1,time_s.year
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_DD_MM_HH_MM_SS_YY:
    sprintf(ptimestamp->val,"%.2d %s %.2d:%.2d:%.2d %.2d"
	    ,time_s.dayMonth+1,MONTH[time_s.monthNum]
	    ,time_s.hours,time_s.minutes,time_s.seconds,time_s.year);
    break;
  case timestampTST_VMS:
    sprintf(ptimestamp->val,"%.2d-%s-%.4d %.2d:%.2d:%.2d"
	    ,time_s.dayMonth+1,MONTH[time_s.monthNum],time_s.year
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_MM_DD_YYYY:
    sprintf(ptimestamp->val,"%s %.2d, %.4d %.2d:%.2d:%.2d"
	    ,MONTH[time_s.monthNum],time_s.dayMonth+1
	    ,time_s.year,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  case timestampTST_MM_DD_YY:
    sprintf(ptimestamp->val,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d"
	    ,time_s.monthNum+1,time_s.dayMonth+1,time_s.year
	    ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  default :  /* YY/MM/DD HH:MM:SS */
    sprintf(ptimestamp->val,"%.2d/%.2d/%.2d %.2d:%.2d:%.2d"
            ,time_s.year,time_s.monthNum+1,time_s.dayMonth+1
            ,time_s.hours,time_s.minutes,time_s.seconds);
    break;
  }
#endif
  /* check event list */
  monitor(ptimestamp);

  /* process the forward scan link record */
  recGblFwdLink(ptimestamp);
  ptimestamp->pact=FALSE;
  return(0);
}


static void monitor(ptimestamp)
     struct timestampRecord             *ptimestamp;
{
  unsigned short  monitor_mask;
  
  monitor_mask = recGblResetAlarms(ptimestamp);
  monitor_mask |=DBE_VALUE|DBE_LOG;
  if(strncmp(ptimestamp->oval,ptimestamp->val,sizeof(ptimestamp->val))) {
    db_post_events(ptimestamp,&(ptimestamp->val[0]), monitor_mask);
    db_post_events(ptimestamp,&ptimestamp->rval,monitor_mask);
    db_post_events(ptimestamp,&ptimestamp->sec, monitor_mask);
    db_post_events(ptimestamp,&ptimestamp->nsec,monitor_mask);
    strncpy(ptimestamp->oval,ptimestamp->val,sizeof(ptimestamp->val));
  }
  return;
}
