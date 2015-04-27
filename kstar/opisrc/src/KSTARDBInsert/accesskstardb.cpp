#include <mysql++.h>
#include <sys/time.h>
#include <iostream>

#include <kstardb.h>
#include <kstardbconn.h>

using namespace std;
using namespace mysqlpp;

sql_create_2(kstartbl_subset, 1, 0, 
		mysqlpp::sql_char, pvname,
		mysqlpp::sql_double, pvval
		)

void MicroTimePrint ( long *sec, double *msec )
{
	struct timeval tp;

	if( gettimeofday((struct timeval *)&tp,NULL) == 0 )
	{
		(*msec)=(double)(tp.tv_usec/1000000.00);
		(*sec)=tp.tv_sec;
		if((*msec)>=1.0) (*msec)-=(long)(*msec);
		printf("sec: %d, msec:%f\n", *sec, *msec);
	};
};


int main(int argc, char *argv[])
{
	int count =0;
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	}

	MySQLKSTARDB mysqlkstardb;
	string dbname   = "kstardb";
	string hostname = "172.17.100.116";
	string user     = "root";
	string passwd   = "1234";
	mysqlpp::Connection con(mysqlpp::use_exceptions);

#if 1
	int     c;
	int     index;
	int     iserr = 0;

	char    *pvnamesub = NULL; 
	char    *system = NULL;   

	//opterr  =  0;

	while ((c = getopt (argc, argv, "p:s:")) !=  -1) 
	{
		//printf("argument:%c\n", c);
		switch (c) {
			case  'p':
				pvnamesub = optarg;
				break;
			case  's':
				system = optarg;
				break;
			case  '?':
				printf("USAGE: accesskstardb -s system[TMS,VMS,CLS,QDS] -p pvname_substring\n");
				return -1;
		};
	};

	for (index = optind; index < argc; index++) {
		printf ("Non-option argument %s\n", argv[index]);
		iserr++;
	};

	if (iserr||argc<=1) 
	{
		printf("USAGE: accesskstardb -s system[TMS,VMS,CLS,QDS] -p pvname_substring\n");
		return -1;
	};

#endif

	long sec1 = 0, sec2 = 0;
	double msec1 = 0., msec2 = 0.;
	try
	{
		if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
		{
			return -1;
		};

		mysqlpp::Query query = con.query();
		mysqlpp::ResUse res;

		if (!pvnamesub)
		{
			query << "select pvindex, pvname, system from kstarpvtable where system=%0q";
			query.parse();
			res = query.use(system);
		}
		else if(!system)
		{
			query << "select pvindex, pvname, system from kstarpvtable where pvname like %0q";
			query.parse();
			string strpvsub = string("%")+string(pvnamesub)+string("%");
			res = query.use(strpvsub);
		}
		else 
		{
			query << "select pvindex, pvname, system from kstarpvtable where system=%0q and pvname like %1q";
			query.parse();
			string strpvsub = string("%")+string(pvnamesub)+string("%");
			res = query.use(system, strpvsub);
		}

		MicroTimePrint (&sec1, &msec1 );
		mysqlpp::Row row;
		// Display the result set
		while (row = res.fetch_row())
		{
			cout << "pvindex: " << row["pvindex"]<<" pvname: " << row["pvname"] << " system: "<<row["system"]<<endl;
			count++;
		};
	}
	catch (const mysqlpp::BadQuery& er) {
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const mysqlpp::Exception& er) {
		printf("PV Count: %d\n", count);
		MicroTimePrint (&sec2, &msec2 );
		printf("sec diff: %d, msec diff: %d\n", (sec2 - sec1), (int)((msec2-msec1)*1000));
		cerr << er.what() << endl;
		return -1;
	};

	return 0;
}
