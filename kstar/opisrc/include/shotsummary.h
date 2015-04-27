/***********************************************************************
 kstardb.h - Declares the kstardb
 
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004-2006 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

***********************************************************************/
#if !defined(MYSQL_SHOTSUMMARY_H)
#define MYSQL_SHOTSUMMARY_H

#include <mysql++.h>
#include <custom.h>
//#include <string>
//kstarwebdb-shotsummary SSQLS structure
//10 construct number
sql_create_17 (SHOTSUMMARY, 1, 17,
		mysqlpp::sql_bigint, shotnum,
		mysqlpp::sql_datetime, shotdate,
		mysqlpp::sql_double, tfcurrent,
		mysqlpp::sql_double, plsmacurrent,
		mysqlpp::sql_double, edensity,
		mysqlpp::sql_double, etemp,
		mysqlpp::sql_double, airpressure,
		mysqlpp::sql_double, echpow,
		mysqlpp::sql_double, icrhpower,
		mysqlpp::sql_double, nbi1power,
		mysqlpp::sql_double, eccdpower,
		mysqlpp::sql_double, lhcdpower,
		mysqlpp::sql_double, ipwidth,
		mysqlpp::sql_double, ipwidth100k,
		mysqlpp::sql_char, comment,
		mysqlpp::sql_char, type,
		mysqlpp::sql_char, campaign)

sql_create_8 (SESSIONT, 1, 8,
		mysqlpp::sql_int, idx,
		mysqlpp::sql_datetime, sesnum,
		mysqlpp::sql_char, sestitle,
		mysqlpp::sql_char, sesleader,
		mysqlpp::sql_char, sescmo,
		mysqlpp::sql_char, proposal,
		mysqlpp::sql_bigint, startshot,
		mysqlpp::sql_bigint, endshot)

#endif
