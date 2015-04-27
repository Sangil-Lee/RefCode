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
 //7 construct number
sql_create_7 (kstarpvtable, 1, 7,
		mysqlpp::sql_bigint, pvindex,
		mysqlpp::sql_char, pvname,
		mysqlpp::sql_char, system,
		mysqlpp::sql_int, sigtype,
		mysqlpp::sql_char, application,
		mysqlpp::sql_char, uiname,
		mysqlpp::sql_tinyint, detail)

#endif
