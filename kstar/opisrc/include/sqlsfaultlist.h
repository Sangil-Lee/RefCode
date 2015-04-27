/***********************************************************************
 kstardb.h - Declares the kstardb
 
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004-2006 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

***********************************************************************/
#ifndef MYSQL_FAULTLIST_H
#define MYSQL_FAULTLIST_H

#include <mysql++.h>
#include <custom.h>
sql_create_20 (FAULTLISTT, 1, 20,
		mysqlpp::sql_int, num,
		mysqlpp::sql_enum, sname,
		mysqlpp::sql_char, scomponent,
		mysqlpp::sql_char, sfunction,
		mysqlpp::sql_datetime, fdatetime,
		mysqlpp::sql_enum, fclass,
		mysqlpp::sql_char, fname,
		mysqlpp::sql_int, fseverity,
		mysqlpp::sql_char, fcause,
		mysqlpp::sql_char, feffect,
		mysqlpp::sql_datetime, rdatetime,
		mysqlpp::sql_char, rdesc,
		mysqlpp::sql_char, rduration,
		mysqlpp::sql_enum, intlevel,
		mysqlpp::sql_char, intdesc,
		mysqlpp::sql_bigint, refshotnum,
		mysqlpp::sql_int, mtbf,
		mysqlpp::sql_char, writer,
		mysqlpp::sql_char, comment,
		mysqlpp::sql_datetime, wdate)
#endif
