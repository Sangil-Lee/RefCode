#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <mysql++.h>

#include "kstardbconn.h"
#include "shotsummary.h"

//using namespace std;
//using namespace mysqlpp;

int main (int argc, char *argv[])
{
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	};

	MySQLKSTARDB mysqlkstardb;
	string dbname   = "kstarweb";
	string hostname = "203.230.119.122";
	string user     = "root";
	string passwd   = "kstar2004";
	mysqlpp::Connection con(mysqlpp::use_exceptions);

	string strToken;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			printf("KSTAR DB Connection failed\n");
			return -1;
		};

		mysqlpp::sql_datetime shotdate;
		shotdate.year = 2008;
		shotdate.month=6;
		shotdate.day=8;
		shotdate.hour=20;
		shotdate.minute=0;
		shotdate.second=0;

#if 1
		mysqlpp::Query query = con.query();
		SHOTSUMMARY shotsummaryinsert(688,shotdate,25.4 ,51.40,34.34 ,43.323, 323, 123,"Comment Test");
		query.insert(shotsummaryinsert);
		query.execute();
#endif
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return -1;
	};
	return 0;
}
