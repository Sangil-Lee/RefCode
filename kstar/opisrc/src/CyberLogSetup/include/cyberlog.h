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
sql_create_8 (SDN_DATA_DB, 1, 8,
		mysqlpp::sql_char, TEST_ID,
		mysqlpp::sql_char, PROJECT_NAME,
		mysqlpp::sql_char, TEST_DESC,
		mysqlpp::sql_datetime, TEST_START_TIME,
		mysqlpp::sql_datetime, TEST_END_TIME,
		mysqlpp::sql_int, PERIODIC_SAVE_TIME,
		mysqlpp::sql_smallint, MODE,
		mysqlpp::sql_smallint, END_TYPE)
#endif
