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
#if 0
sql_create_4 (cyberlogview, 1, 4,
		mysqlpp::sql_bigint, indx,
		mysqlpp::sql_int, sdnindex,
		mysqlpp::sql_char, savetime,
		mysqlpp::sql_char, sdndata)
#else
sql_create_6 (CYBER_TEST_0001, 1, 6,
		mysqlpp::sql_int, SDN_INDEX,
		mysqlpp::sql_timestamp, SAVE_TIME,
		mysqlpp::sql_int, NETWORK_ID,
		mysqlpp::sql_char, SOURCE,
		mysqlpp::sql_char, DESTINATION,
		mysqlpp::sql_char, SDN_DATA)
#endif
#endif
