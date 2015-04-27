/***********************************************************************
 kstardb.h - Declares the kstardb
 
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004-2006 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

***********************************************************************/
#if !defined(MYSQL_SHOTREMARK_H)
#define MYSQL_SHOTREMARK_H

#include <mysql++.h>
#include <custom.h>
sql_create_5 (REMARKT, 1, 5,
		mysqlpp::sql_int, idx,
		mysqlpp::sql_bigint, shotnum,
		mysqlpp::sql_char, writer,
		mysqlpp::sql_char, remark,
		mysqlpp::sql_datetime, wdate)
#endif
