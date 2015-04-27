/****************************************************************************

Module      : sfwCommon.c

Copyright(c): 2009 NFRI. All Rights Reserved.

Revision History:
Author: woong   2009-11-24
  

*****************************************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <string.h>


#include <dbAccess.h>
#include "errlog.h" /* for errMessage(xxx) */


#include "sfwGlobalDef.h"
#include "sfwCommon.h"





int wf_print_date()
{
	
	struct tm *clock;
	time_t current;
	time(& current);
	clock=localtime(& current);
	
	printf("%d-%d-%d, ", clock->tm_year+1900, clock->tm_mon+1, clock->tm_mday);
	printf("%d:%d:%d\n", clock->tm_hour, clock->tm_min, clock->tm_sec);

	return WR_OK;
}


void printlog(const char *fmt, ... )
{
	char buf[1024];
	va_list argp;
	FILE *fpLog;
	int len;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);


	len = strlen(buf);
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;


	fpLog = fopen(STR_LOG_FILENAME, "a+");
	if( fpLog == NULL) {
		epicsPrintf("can't open log file: \"%s\"\n", STR_LOG_FILENAME);
	} else {
		struct tm *clock;
		time_t current;
		time(& current);
		clock=localtime(& current);

		fprintf(fpLog, "%d/%d/%d, %d:%d:%d\t%s\n", clock->tm_year+1900, clock->tm_mon+1, clock->tm_mday, clock->tm_hour, clock->tm_min, clock->tm_sec, buf);
		fclose(fpLog);
	}
	
}

unsigned long long int wf_getCurrentUsec(void)
{
	unsigned long long int x;
	struct timeval tv;

/*	__asm__ volatile(".byte 0x0f, 0x31": "=A"(x));	*/
	gettimeofday( &tv, NULL);

	x = tv.tv_sec;
	x = (x*1.E+6) + tv.tv_usec; 
	
/*	return timecurrent.tv_sec*1000 + timecurrent.tv_usec/	
	return x;
*/
	return x;
	
}

double wf_intervalUSec(unsigned long long int start, unsigned long long int stop)
{
	return (stop-start);
}







