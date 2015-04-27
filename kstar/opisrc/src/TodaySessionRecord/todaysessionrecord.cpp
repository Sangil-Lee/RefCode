#include <string>
#include <vector>
#include <mysql++.h>
#include <cadef.h>
#include <dbDefs.h>
#include <kstardbconn.h>


using namespace std;
int main(int argc,char* argv[])
{
	string dbname   = "kstarweb";
	string hostname = "203.230.119.122";
	string user     = "root";
	string passwd   = "kstar2004";
	//mysqlpp::Connection con(mysqlpp::use_exceptions);
	mysqlpp::Connection con(false);
	MySQLKSTARDB mysqlkstardb;

	if(argc != 2)
	{
		printf("USAGE:%s [0,1]\n",argv[0]);
		exit(-1);
	}
	int cmd=atoi(argv[1]);

	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	};

	if( !mysqlkstardb.Connect_to_db(dbname, hostname, user, passwd, con) )
	{
		return -1;
	};

	try
	{
#if 1
		mysqlpp::Query query = con.query();
		mysqlpp::ResUse res;

		string strquery = "select max(plsmacurrent) from SHOTSUMMARY";
		query << strquery;
		query.parse();
		res = query.use();

		mysqlpp::Row row;
		double maxplcur =0.0;
		string strmaxplcur;
		while (row = res.fetch_row())
		{
			cout <<endl<< "plasm current record of today : " << row["max(plsmacurrent)"] << endl;
			maxplcur = row["max(plsmacurrent)"];
			strmaxplcur =row["max(plsmacurrent)"].get_string();

		};
		//printf("max:%f, maxstr:%s\n", maxplcur, strmaxplcur.c_str());

		mysqlpp::Query query2 = con.query();
		//strquery = "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,comment from SHOTSUMMARY";
		if(cmd == 0)
		{
			strquery = "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,comment from SHOTSUMMARY";
			query2 << strquery;
			query2.parse();
			res = query2.use();
		}
		else
		{
			strquery = "select shotnum,shotdate,tfcurrent,plsmacurrent,edensity,etemp,airpressure,echpow,comment from SHOTSUMMARY where plsmacurrent=%0q";
			query2 << strquery;
			query2.parse();
			res = query2.use(maxplcur);
		}


		// Display the result set
		while (row = res.fetch_row())
		{
			cout <<endl<< "shotnum:[" << row["shotnum"]<<"] \nshotdate:[" << row["shotdate"] << "] \ntfcur:["<<row["tfcurrent"]
				<<"] \nplsmacurrent:[" <<row["plsmacurrent"] <<"] \nedensity:[" <<row["edensity"] <<"] \nairpressure:[" <<row["airpressure"]
				<<"] \nechpow:[" <<row["echpow"] <<"]" << endl;
			double plcur = row["plsmacurrent"];
			printf("%f\n", plcur);
		};
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
