#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <mysql++.h>

#include "kstardb.h"
#include "kstardbconn.h"

using namespace std;
//using namespace mysqlpp;

int main (int argc, char *argv[])
{
	if ( argc != 2 )
	{
		printf("Usage:kstardbinsert [DB filename]\n");
		exit(0);
	};

	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	};

	MySQLKSTARDB mysqlkstardweb";
	string dbname   = "kstardb";
	string hostname = "172.17.100.116";
	string user     = "root";
	string passwd   = "1234";
	mysqlpp::Connection con(mysqlpp::use_exceptions);

	ifstream file(argv[1]);
	printf("argc:%d, argv[1]:%s\n", argc, argv[1]);

	if ( file.fail () ) 
	{
		printf("DB filename not found\n");
		return -1;
	};

	string strToken;
	int line = 0;			/*line=id, first pch=recordtype, second pch=pvname, third pch = description*/
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			printf("KSTAR DB Connection failed\n");
			return -1;
		};

		mysqlpp::Query query = con.query();

		while (!file.eof ())
		{
			getline (file, strToken);
			if(strToken[0] == '#' || strToken.empty()==true) continue;

			char str[strToken.size()];
			strcpy (str, strToken.c_str());

			char *pch = 0;
			if(!(pch = strtok (str," \t,"))) continue;
			printf("%d:%s:", line, pch);

			//int pvnamesize = strlen(pch); 
			string pvname;
			pvname = pch;

			int index = 0;
			string system;

			while (pch != NULL)
			{
				index = 0;
				if(!(pch = strtok (NULL," \t,"))) continue;
				if (index == 0) 
				{
					system = pch;
					printf("%s\n", pch);
				}
				else if (index == 1) 
				{
					//printf("%s\n", pch);
				};
				index++;
			};
			kstarpvtable kstartblinsert(line, pvname.c_str(), system.c_str(), 0, "App Name", "ui file", '0');
			query.insert(kstartblinsert);
			query.execute();
			line++;
		};
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		cerr << er.what() << endl;
		return -1;
	};
	file.close ();
	return 0;
}
