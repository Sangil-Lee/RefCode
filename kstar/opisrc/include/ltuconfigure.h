/***********************************************************************
 kstardb.h - Declares the kstardb
 
 Copyright (c) 1998 by Kevin Atkinson, (c) 1999, 2000 and 2001 by
 MySQL AB, and (c) 2004-2006 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

***********************************************************************/
#ifndef MYSQL_LTU_CONFIGURE_H
#define MYSQL_LTU_CONFIGURE_H

#include <mysql++.h>
#include <custom.h>
//#include <string>
//kstarwebdb-shotsummary SSQLS structure
//10 construct number
sql_create_35 (LTU_CONFIGURE, 1, 35,
		mysqlpp::sql_bigint, shotnum,
		mysqlpp::sql_char, shotdate,
		mysqlpp::sql_int, version,
		mysqlpp::sql_int, enclose,
		mysqlpp::sql_int, sysID,
		mysqlpp::sql_char, sysName,
		mysqlpp::sql_char, descr,
		mysqlpp::sql_int, ch,
		mysqlpp::sql_int, enable,
		mysqlpp::sql_int, mode,
		mysqlpp::sql_int, pol,
		mysqlpp::sql_double, sec1T0,
		mysqlpp::sql_double, sec1T1,
		mysqlpp::sql_double, sec1clk,
		mysqlpp::sql_double, sec2T0,
		mysqlpp::sql_double, sec2T1,
		mysqlpp::sql_double, sec2clk,
		mysqlpp::sql_double, sec3T0,
		mysqlpp::sql_double, sec3T1,
		mysqlpp::sql_double, sec3clk,
		mysqlpp::sql_double, sec4T0,
		mysqlpp::sql_double, sec4T1,
		mysqlpp::sql_double, sec4clk,
		mysqlpp::sql_double, sec5T0,
		mysqlpp::sql_double, sec5T1,
		mysqlpp::sql_double, sec5clk,
		mysqlpp::sql_double, sec6T0,
		mysqlpp::sql_double, sec6T1,
		mysqlpp::sql_double, sec6clk,
		mysqlpp::sql_double, sec7T0,
		mysqlpp::sql_double, sec7T1,
		mysqlpp::sql_double, sec7clk,
		mysqlpp::sql_double, sec8T0,
		mysqlpp::sql_double, sec8T1,
		mysqlpp::sql_double, sec8clk)

#endif

