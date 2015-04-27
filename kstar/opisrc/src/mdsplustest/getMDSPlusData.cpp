#include "mdsplusdata.h"
#include "shotsummary.h"

#define status_ok(status) (((status) & 1) == 1)
using namespace std;

int main(int argc, char *argv[])
{
	/* local vars */
	int shot = 0;
	if(argc!=4)
	{
		printf("USAGE:%s [nodename] [treename] [shotnum]\n",argv[0]);
		exit(-1);
	};
	string nodename = string(argv[1]);
	string treename = string(argv[2]);
	shot = atoi(argv[3]);

	// MySQL DB Version check
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	};
#if 1
	MDSPlusData *mdsplusdata = new MDSPlusData(nodename,treename,shot);
	if(mdsplusdata->getShotData()<0)
	{
		delete mdsplusdata;
		return -1;
	};
#else
	MDSPlusData *mdsplusdata = new MDSPlusData;
	mdsplusdata->setMDSDataInfo(nodename,treename,shot);
	int size = mdsplusdata->getNodeSize();
	double *timebase = new double[size];
	double *shotdata = new double[size];
	if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
	{
		delete mdsplusdata;
		return -1;
	};
	double mxVal = *max_element(&shotdata[0], &shotdata[size]);
	printf("Max**1:%f\n",mxVal);
	//double mnVal = *min_element(&mshotdata[0], &mshotdata[msize]);
	//for ( int i = 0 ; i < size; i++ )
	//{
	//	printf("%i  X:%f  Y:%f\n", i, timebase[i], shotdata[i]);
	//};
	delete timebase;
	delete shotdata;

	mdsplusdata->setMDSDataInfo("pcs_kstar","PCRC03",shot);
	size = mdsplusdata->getNodeSize();
	timebase = new double[size];
	shotdata = new double[size];
	if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
	{
		delete mdsplusdata;
		return -1;
	};
	mxVal = *max_element(&shotdata[0], &shotdata[size]);
	printf("Max**1:%f\n",mxVal);
#if 0
	for ( int i = 0 ; i < size; i++ )
	{
		printf("%i  X:%f  Y:%f\n", i, timebase[i], shotdata[i]);
	};
#endif

	printf("ShotTime:%s\n",mdsplusdata->getShotTime().c_str());
#endif

#if 0
	//DB Insert
	MySQLKSTARDB mysqlkstardb;
	string dbname   = "kstarweb";
	string hostname = "203.230.119.122";
	string user     = "root";
	string passwd   = "kstar2004";
	mysqlpp::Connection con(mysqlpp::use_exceptions);
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
#endif

	/* done */
	/* free the denamically allocated memory when done */
	//free( (void *)shotdata );
	//delete shotdata;
	//delete timebase;
	return 0;
}
