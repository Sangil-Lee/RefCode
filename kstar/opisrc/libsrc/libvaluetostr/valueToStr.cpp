// valueToStr.cpp
// 
// ***************************************************************************
// 
// Copyright(C) 2005, BNF Technology, All rights reserved.
// Proprietary information, BNF Technology
// 
// ***************************************************************************
//
// ***** HIGH LEVEL DESCRIPTION **********************************************
//
// Module for managing process information
//
// ***************************************************************************
//
// **** REVISION HISTORY *****************************************************
//
// Revision 1  2004-04-28
// Author: silee
// Initial revision
//
// ***************************************************************************

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------
#include <stdlib.h>
// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------
#include "valueToStr.h"

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Import Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Export Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Module Variables & Function Declarations
// ---------------------------------------------------------------------------
// conversion value to string. 
string IntToStr ( const int value, const char *format )
{
	char tmpVal[8];
	snprintf	(tmpVal, sizeof(tmpVal), format, value );
	RemoveSpace ( tmpVal );
	return (value > 32767 ? "IntOverflow": value < -32768 ? "IntUnderflow": tmpVal);
};
string FloatToStr ( const float value, const char *format)
{
	char tmpVal[20];
	snprintf	(tmpVal, sizeof(tmpVal), format, value );
	RemoveSpace ( tmpVal );
	return (tmpVal);
};
string DoubleToStr ( const double value, const char *format )
{
	char tmpVal[30];
	snprintf	(tmpVal, sizeof(tmpVal), format, value );
	RemoveSpace ( tmpVal );
	return (tmpVal);
};
string LongToStr ( const long value, const char *format )
{
	char tmpVal[20];
	snprintf	(tmpVal, sizeof(tmpVal), format, value );
	RemoveSpace ( tmpVal );
	return (tmpVal);
};

// conversion string to value. 
int StrToInt ( string &strvalue )
{
	return strvalue.empty() ? 0 : atoi ( strvalue.c_str() );
};
int StrToInt ( const char *strvalue )
{
	return (strvalue == NULL ? 0 : atoi ( strvalue ));
};
long StrToLong ( string &strvalue )
{
	// old
	//return strvalue.empty() ? 0 : atol ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	long lValue = strtol ( strvalue.c_str(), &ptrEnd, 0 );
	return ( (*ptrEnd) == '\0' ? lValue : 0 );
};
long StrToLong ( const char *strvalue )
{
	char* ptrEnd; 
	long lValue = strtol ( strvalue, &ptrEnd, 0 );
	return ( (*ptrEnd) == '\0' ? lValue : 0 );
};
long StrToLong ( string &strvalue, bool &isOK )
{
	// old
	//return strvalue.empty() ? 0 : atol ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	long lValue = strtol ( strvalue.c_str(), &ptrEnd, 0 );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0);
	};

	isOK = true;
	return (lValue);
};
long StrToLong ( const char *strvalue, bool &isOK )
{
	// old
	//return strvalue.empty() ? 0 : atol ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	long lValue = strtol ( strvalue, &ptrEnd, 0 );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0);
	};

	isOK = true;
	return (lValue);
};
float StrToFloat ( const char *strvalue )
{
	char* ptrEnd; 
	float fValue = strtod ( strvalue, &ptrEnd );
	return ( (*ptrEnd) == '\0' ? fValue : 0. );

};
float StrToFloat ( const char *strvalue, bool &isOK )
{
	char* ptrEnd; 
	float fValue = strtod ( strvalue, &ptrEnd );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0.);
	};
	isOK = true;
	return ( fValue );

};
float StrToFloat ( string &strvalue )
{
	// old
	//	return strvalue.empty() ? 0. : atof ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	float fValue = strtod ( strvalue.c_str(), &ptrEnd );
	return ( (*ptrEnd) == '\0' ? fValue : 0. );

};
float StrToFloat ( string &strvalue, bool &isOK )
{
	// old
	//	return strvalue.empty() ? 0. : atof ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	float fValue = strtod ( strvalue.c_str(), &ptrEnd );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0.);
	};
	isOK = true;
	return ( fValue );

};
double StrToDouble ( const char *strvalue )
{
	char* ptrEnd; 
	double dValue = strtod ( strvalue, &ptrEnd );
	return ( (*ptrEnd) == '\0' ? dValue : 0. );
};
double StrToDouble ( string &strvalue )
{
	// old
	//return strvalue.empty() ? 0. : atof ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	double dValue = strtod ( strvalue.c_str(), &ptrEnd );
	return ( (*ptrEnd) == '\0' ? dValue : 0. );
};
double StrToDouble ( const char *strvalue, bool &isOK )
{
	char* ptrEnd; 
	double dValue = strtod ( strvalue, &ptrEnd );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0.);
	};

	isOK = true;
	return (dValue);
};
double StrToDouble ( string &strvalue, bool &isOK )
{
	// old
	//return strvalue.empty() ? 0. : atof ( strvalue.c_str() );
	// new
	char* ptrEnd; 
	double dValue = strtod ( strvalue.c_str(), &ptrEnd );

	if ((*ptrEnd) != '\0' )
	{
		isOK = false;
		return (0.);
	};

	isOK = true;
	return (dValue);
};

const char* RemoveSpace ( char* expr )
{
	int len = strlen ( expr );
	int j = 0;
	for ( int i = 0; i < len ; i++ )
	{
		if ( expr [i] == ' ' ) continue;
		expr[j++] = expr[i];
	};

	expr[j] = '\0';
	return expr;
};

const char* TrimStr ( char* str )
{
	char    szBuf[1024];
	int     i, j, len;

	if( !str )  return (0);
	len     = strlen( str );

	for( i = j = 0; i < len; i++ )
	{
		if( str[i] != ' ' ) break;
	};

	if( str[i] == '\0' )    return (0);
	strcpy( szBuf, &str[i] );

	len     = strlen( szBuf );
	for( i = len - 1; i >= 0; i-- )
	{
		if( szBuf[i] != ' ' )   break;
	};
	szBuf[i + 1]    = '\0';
	str = szBuf;

	return (str);
};

string & ToUpper ( string & str )
{
	transform ( str.begin(), str.end (), str.begin(), toupper );
	return str;
};

string & ToLower ( string & str )
{
	transform ( str.begin(), str.end (), str.begin(), tolower );
	return str;
};

string ToUpper ( char *cstr ) 
{
	string str = cstr;
	transform ( str.begin(), str.end (), str.begin(), toupper );
	return str;
};

string ToLower ( char *cstr ) 
{
	string str = cstr;
	transform ( str.begin(), str.end (), str.begin(), tolower );
	return str;
};

bool GetMicroTime ( long *sec, double *msec )
{
	struct timeval tp;

	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;

		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		return true;
	}
	return false;
};

long Datetime2Epoch ( const int year,const int month,const int day, const int hour,const int minute,const int second)
{
	struct tm   t_date;

	if ( (year <= 0 || year >= 2039) || ( month <= 0 || month > 12 ) || ( day <= 0 || day > 31 ) ||
		( minute < 0 || minute > 59 ) || ( second < 0 || second > 59 ) )
	{
		return (0);
	};

	t_date.tm_sec = second;
	t_date.tm_min = minute;
	t_date.tm_hour = hour;
	t_date.tm_mday = day;
	t_date.tm_mon = month - 1;
	t_date.tm_year = year - 1900;

	return ( mktime ( &t_date ) );
};

void Epoch2Datetime ( int &year, int &month, int &day, int &hour, int &minute, int &second, time_t epochtime )
{
	struct tm *ptime;

	if ( epochtime == 0 ) epochtime = time (NULL);
	ptime = localtime ( &epochtime );

	year	= ptime -> tm_year + 1900;
	month	= ptime -> tm_mon + 1;
	day		= ptime -> tm_mday;
	hour	= ptime -> tm_hour;
	minute  = ptime -> tm_min;
	second  = ptime -> tm_sec;
};

string  Epoch2Datetimestr ( time_t epochtime )
{
	struct tm *ptime;

	int year = 0;

	int month = 0, day = 0, hour = 0, minute = 0, second = 0;

	//ptime = ( epochtime == 0 ) ? localtime ((const time_t*)time(NULL)) : localtime ( &epochtime );
	if ( epochtime == 0 ) epochtime = time (NULL);

	ptime =	localtime ( &epochtime );

	year	= ptime -> tm_year + 1900;
	month	= ptime -> tm_mon + 1;
	day		= ptime -> tm_mday;
	hour	= ptime -> tm_hour;
	minute  = ptime -> tm_min;
	second  = ptime -> tm_sec;

	return ( IntToStr (year) + "/" + IntToStr (month,"%02d") + "/" + IntToStr (day,"%02d")+ " "+ IntToStr (hour,"%02d") + ":" + IntToStr (minute,"%02d") + ":" + IntToStr(second,"%02d") );
};
bool StrToken(const char *str, const char *token, tokened **tokened_head)
{
	tokened *cur = NULL, *prev = NULL;
	char chr; int i = 0, cnt = 0; int token_len = 0;
	char *last_token_occur = NULL;
	if( str == NULL ||token == NULL )
    {
		return false;
	};
	token_len = strlen ( token );
	last_token_occur = (char *) str;
	while ( ( chr = *( str + cnt ) ) != '\0' )
    {
		for( i = 0; token_len > i; ++i )
        {
			if ( chr == token[i] )
            {
				if ( (str + cnt) > last_token_occur )
                {
					//cur=(tokened *)malloc(sizeof(tokened));
					cur = new tokened;
					memset (cur, 0x0, sizeof(tokened) );
					cur -> type = TOK_TOKEN_X;
					cur -> sta  = last_token_occur;
					cur -> end  = (char *)(str+cnt);
					if ( (*tokened_head) == NULL )
                    {
						(*tokened_head) = cur;
					}
                    else
                    {
						prev -> next = cur;
					};
					prev = cur;
				};

				cur = new tokened;
				memset( cur, 0x0, sizeof(tokened) );
				cur->type = TOK_TOKEN_O;
				cur->sta = (char *)(str+cnt);
				cur->end = (char *)(str+cnt+1);

				if ( (*tokened_head) == NULL ) (*tokened_head) = cur;
                else	prev->next=cur;
				prev = cur;

				last_token_occur = (char *)(str+cnt+1);
				break;
			};
		};
		++cnt;
	};
	if ( ( str + cnt ) > last_token_occur )
    {
		cur = new tokened;
		memset(cur, 0x0, sizeof(tokened));
		cur->type = TOK_TOKEN_X;
		cur->sta = last_token_occur;
		cur->end = (char *)(str+cnt);
		if ( (*tokened_head) == NULL) (*tokened_head) = cur;
        else		 prev -> next = cur;
		prev = cur;
	};
	return true;
};

#if 0
bool GetTokenStr ( const char *str, const char *token, int &tokentype)
{
	char *buf = NULL;
	int size = 0;
	tokened *tokened_head = NULL, *cur = NULL;
	strToken ( str, token, &tokened_head );
	cur = tokened_head;
	while ( cur != NULL )
	{
		size = (cur->end - cur->sta);
		buf = new char[size+1];
		buf[size+1] = '\0';
		memcpy( buf, cur->sta, size);
		tokentype = cur->type;
		printf("%s : '%s'\n",((cur->type==TOK_TOKEN_O) ? "TOKEN_O" : "TOKEN_X"),buf);
		cur = cur->next;
	};
	strTokenfree(tokened_head);
	delete [] buf;
	return true;
}
#endif

bool StrTokenfree ( tokened *tokened_head )
{
    tokened *cur = NULL, *next = NULL;
    cur = tokened_head;
    while( cur!=NULL)
    {
        next=cur->next;
        //free(cur);
        delete cur;
        cur=next;
    };
    return 0;
};

// 타밈 설정. root로 실행.
int SetSystemTime ( const time_t epochsec, const long microsec ) 
{
	struct timeval settime;

	settime.tv_sec  = epochsec;
	settime.tv_usec = microsec;

	return ( settimeofday ( &settime, NULL ) );
}
int SetSystemTime ( const int year, const int month, const int day, const int hour, const int minute, const int second)
{
	struct tm   t_date;

	if ( (year <= 0 || year >= 2039) || ( month <= 0 || month > 12 ) || ( day <= 0 || day > 31 ) ||
		( minute < 0 || minute > 59 ) || ( second < 0 || second > 59 ) )
	{
		return (0);
	};

	t_date.tm_sec = second;
	t_date.tm_min = minute;
	t_date.tm_hour = hour;
	t_date.tm_mday = day;
	t_date.tm_mon = month - 1;
	t_date.tm_year = year - 1900;

	struct timeval settime;
	settime.tv_sec = mktime ( &t_date );
	settime.tv_usec = 0;
	return ( settimeofday ( &settime, NULL ) );
}
