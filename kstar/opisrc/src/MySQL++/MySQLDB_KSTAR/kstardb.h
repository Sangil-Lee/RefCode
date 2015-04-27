/***********************************************************************
 kstardb.h - Declares the kstardb
 
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004-2006 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

***********************************************************************/
#if !defined(MYSQL_KSTARDB_H)
#define MYSQL_KSTARDB_H

#include <mysql++.h>
#include <custom.h>
#include <string>

//kstardb-kstartbl SSQLS structure
sql_create_4(kstartbl, 1, 4,
		mysqlpp::sql_bigint, pvindex,
		mysqlpp::sql_char, pvname,
		mysqlpp::sql_double, pvval,
		mysqlpp::sql_date, pvdate)

#endif
