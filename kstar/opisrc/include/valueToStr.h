// Filename : valueToStr.h
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
#ifndef _VALUETOSTR_H
#define _VALUETOSTR_H

// ---------------------------------------------------------------------------
// System Include Files
// ---------------------------------------------------------------------------
#include <cstdio>
#include <cstdlib>
#include <sys/time.h>
#include <string>
#include <algorithm>

using std::string;

// ---------------------------------------------------------------------------
// User Defined Include Files
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Constant & Macro Definitions
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// User Type Definitions
// ---------------------------------------------------------------------------
#define TOK_TOKEN_O 0x01
#define TOK_TOKEN_X 0x10

#if 1
typedef struct tokened_st
{
	int type;
	char *sta;
	char *end;
	struct tokened_st *next;
} tokened;
#else
struct tokened
{
	int type;
	char *sta;
	char *end;
	tokened *next;
};
#endif

// ---------------------------------------------------------------------------
// Import Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Export Global Variables & Function Declarations
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Module Variables & Function Declarations
// ---------------------------------------------------------------------------
string  IntToStr (const int value, const char *format = "%d");
string	FloatToStr (const float value, const char *format = "%f");
string	DoubleToStr (const double value, const char *format = "%lf");
string	LongToStr (const long value, const char *format = "%ld");
int		StrToInt ( string &strvalue );
long	StrToLong ( string &strvalue );
long	StrToLong ( string &strvalue, bool &isOK );
float	StrToFloat ( const char *strvalue );
float	StrToFloat ( string &strvalue );
float	StrToFloat ( string &strvalue, bool &isOK );
double	StrToDouble ( string &strvalue );
double	StrToDouble ( string &strvalue , bool &isOK);

// This kind overloading functin can happen to ambiguous error.
int		StrToInt ( const char * strvalue );
long	StrToLong ( const char * strvalue );
long	StrToLong ( const char * strvalue, bool &isOK );
float	StrToFloat ( const char * strvalue );
float	StrToFloat ( const char * strvalue, bool &isOK );
double	StrToDouble ( const char * strvalue );
double	StrToDouble ( const char * strvalue , bool &isOK);

const char* RemoveSpace ( char* expr );
const char* TrimStr ( char* str );
string	&ToUpper ( string &str);
string	&ToLower ( string &str);
string	ToUpper ( char *cstr );
string	ToLower ( char *cstr );

// 문자열 토큰 분리자
bool StrToken ( const char *str,const char *token,tokened **tokened_head );
bool StrTokenfree ( tokened *tokened_head );
int SetSystemTime ( const int year, const int month, const int day, const int hour, const int minute, const int second);
int SetSystemTime ( const time_t epochsec, const long microsec );
string  Epoch2Datetimestr ( time_t epochtime = 0 );
//void Epoch2Datetime ( const time_t epochtime, int &year, int &month, int &day, int &hour, int &minute, int &second );
void Epoch2Datetime ( int &year, int &month, int &day, int &hour, int &minute, int &second,  time_t epochtime = 0);
long Datetime2Epoch ( const int year,const int month,const int day, const int hour,const int minute,const int second);
bool GetMicroTime ( long *sec, double *msec );

#endif
