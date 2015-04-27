//#include "mdsplusdata.h"
//#include "shotsummary.h"

#include "cadef.h"
#include "dbDefs.h"

#define status_ok(status) (((status) & 1) == 1)

using namespace std;

class ShotInfo 
{
public:
	string paraname;
	double paramaxval;
};

class MyNode{
public:
	MyNode():prevshotnum(0)
	{};
	char        value[20];
	chid        summary_chid;
	unsigned long prevshotnum;
};


static void printChidInfo(chid chid, char *message)
{
	printf("\n%s\n",message);
	printf("pv: %s  type(%d) nelements(%ld) host(%s)",
			ca_name(chid),ca_field_type(chid),ca_element_count(chid),
			ca_host_name(chid));
	printf(" read(%d) write(%d) state(%d)\n",
			ca_read_access(chid),ca_write_access(chid),ca_state(chid));
}

static void exceptionCallback(struct exception_handler_args args)
{
	chid    chid = args.chid;
	long    stat = args.stat; /* Channel access status code*/
	const char  *channel;
	static char *noname = "unknown";

	channel = (chid ? ca_name(chid) : noname);


	if(chid) printChidInfo(chid,"exceptionCallback");
	printf("exceptionCallback stat %s channel %s\n",
			ca_message(stat),channel);
}

static void connectionCallback(struct connection_handler_args args)
{
	chid    chid = args.chid;

	printChidInfo(chid,"connectionCallback");
}

static void accessRightsCallback(struct access_rights_handler_args args)
{
	chid    chid = args.chid;
	printChidInfo(chid,"accessRightsCallback");
}

static int  CACommand(string strcmd)
{
#if 0
	//In event callback function, It' dosen't work ca_get() function. I don't understand the reason.
	struct dbr_time_long shotdata;
	chid shot_id;
	ca_create_channel("CCS_SHOT_NUMBER",NULL, NULL,20,&shot_id);
	ca_pend_io(1.0);
	int state = ca_state(shot_id);
	printf("State:%d\n", state);
	ca_get(DBR_TIME_LONG, shot_id, (void *)&shotdata);
	int status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		fprintf(stdout, "SHOT number: %s denied access\n",ca_name(shot_id));
		return;
	};
	fprintf(stdout, "%s:%d\n",ca_name(shot_id),shotdata.value);
#else
	FILE *fp= popen (strcmd.c_str(), "r");
	char buf[20];
	while (fgets (buf, sizeof(buf), fp) ) {};
	pclose(fp);
	return ((strcmd.compare("ShotNum")== 0)?atoi(buf):0);
#endif
}

#if 0
int ReadMDSData(const int shot)
{
	MDSPlusData *mdsplusdata = new MDSPlusData;
	int size = 0;
	double *timebase = 0;
	double *shotdata = 0;
	vector<ShotInfo> vec_shotInfo;
	ShotInfo shotinfo;
	double mxVal = 0;
	string strPara[]={"ITFMSRD", "PCRC03", "NE_INTER01", "B_PKR251B","ECE07", "ECH_VFWD1"};
	bool check;
	for (int i = 0;i<6;i++)
	{
		switch(i)
		{
			case 0:
			case 1:
				mdsplusdata->setMDSDataInfo("pcs_kstar",strPara[i],shot,check);
				break;
			case 3:
				mdsplusdata->setMDSDataInfo("rdata",strPara[i],shot,check);
				break;
			case 2:
			default:
				mdsplusdata->setMDSDataInfo("adata",strPara[i],shot,check);
				break;
		};
		if(check==MDSPlusData::FAIL) 
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			continue;
		};
		size = mdsplusdata->getNodeSize();
		timebase = new double[size];
		shotdata = new double[size];
		if(mdsplusdata->getShotData(timebase, shotdata, size)<0) //if(mdsplusdata->getShotData()<0)
		{
			shotinfo.paraname = strPara[i];
			shotinfo.paramaxval = 0;
			vec_shotInfo.push_back(shotinfo);
			delete timebase;
			delete shotdata;
			continue;
		};

		if ( strPara[i].compare("PCRC03")==0)
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
			double mnVal =*min_element(&shotdata[0], &shotdata[size]);
			if ( mxVal<fabs(mnVal) ) mxVal=mnVal;
		}
		else
		{
			mxVal = *max_element(&shotdata[0], &shotdata[size]);
		}

		printf("Max of [%s]:%f\n",strPara[i].c_str(), mxVal);
		shotinfo.paraname = strPara[i];
		shotinfo.paramaxval = mxVal;
		vec_shotInfo.push_back(shotinfo);
		delete timebase;
		delete shotdata;
	};

	string strTime = mdsplusdata->getShotTime();
	printf("Time:%s\n", strTime.c_str());
	char strBuf[strTime.size()];
	strcpy(strBuf,strTime.c_str());
	char *pch = strtok(strBuf, " /:");
	int year = 0, mon = 0, day =0, hour=0, min=0, sec=0;
	int idx = 0;
	while(pch!=0)
	{
		switch(idx)
		{
			case 0:
				year = atoi(pch);
				break;
			case 1:
				mon = atoi(pch);
				break;
			case 2:
				day = atoi(pch);
				break;
			case 3:
				hour = atoi(pch);
				break;
			case 4:
				min = atoi(pch);
				break;
			case 5:
				sec = atoi(pch);
				break;
		};
		if(!(pch = strtok(NULL, " /:"))) continue;
		idx++;
	};

	vector<ShotInfo>::iterator veciter;
	double dMxval[vec_shotInfo.size()];
	int index =0;
	for(veciter=vec_shotInfo.begin(), index=0;veciter!=vec_shotInfo.end(); ++veciter,index++)
	{
		dMxval[index]=veciter->paramaxval;
		printf("%s,%f\n", veciter->paraname.c_str(), veciter->paramaxval);
	};

	//printf("Y:%d,M:%d,D:%d,H:%d,m:%d,s:%d,ShotTime:%s\n",year,mon,day,hour,min,sec,mdsplusdata->getShotTime().c_str());
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
		shotdate.year = year;
		shotdate.month=mon;
		shotdate.day=day;
		shotdate.hour=hour;
		shotdate.minute=min;
		shotdate.second=sec;
#if 1
		mysqlpp::Query query = con.query();
		SHOTSUMMARY shotsummaryinsert(shot,shotdate,dMxval[0],dMxval[1],dMxval[2],dMxval[4],dMxval[3],dMxval[5],"");
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
#endif

/*static void eventCallback(struct event_handler_args eha)*/
void eventCallback(struct event_handler_args eha)
{
	MyNode *pNode = (MyNode *)ca_puser(eha.chid);
	if(eha.status!=ECA_NORMAL)
	{
		printChidInfo(eha.chid,"eventCallback");
	}
	else
	{
		int currentshot = CACommand("ShotNum");
		printf("currentshot:%d\n",currentshot);

		int updateshot = currentshot - 1;
#if 0
		int updateshot = 0;
		if(pNode->prevshotnum == 0)
		{
			updateshot = currentshot-1;
		}
		else
		{
			updateshot = (currentshot - pNode->prevshotnum)-1;
			if( updateshot <= 0 )
				updateshot = currentshot-1;
		};

		for (int i = updateshot; i > 0;i--)
		{
			int summarystatus = ReadMDSData((currentshot-1)-i);
			printf("update:%d, status:%d\n",shotnum, summarystatus);
		}
#else
		int summarystatus = ReadMDSData(updateshot);
		printf("updateshot:%d, status:%d\n",updateshot, summarystatus);
#endif

		//SaveToMySQLDB();
		CACommand("ResetShotSummary");
		pNode->prevshotnum = updateshot;
	};
}

int main(int argc, char *argv[])
{
	// MySQL DB Version check
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return -1;
	};
	int startshot = 0;
	if(!argv[1]) startshot = atoi(argv[1]);
	MyNode  mynode;
	mynode.prevshotnum = startshot;
	SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");
	SEVCHK(ca_add_exception_event(exceptionCallback,NULL), "ca_add_exception_event");
	//string pvnames[]={"CCS_SHOT_NUMBER","CCS_PERFORM_SHOT_SUMMARY"};
	//SEVCHK(ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",connectionCallback,
				//&mynode,10,(oldChannelNotify**)&mynode.summary_chid), "ca_create_channel");
	SEVCHK(ca_create_channel("DDS2_getState",connectionCallback,
				&mynode,10,(oldChannelNotify**)&mynode.summary_chid), "ca_create_channel");
	SEVCHK(ca_replace_access_rights_event(mynode.summary_chid,
				accessRightsCallback), "ca_replace_access_rights_event");
	ca_create_subscription (DBR_TIME_LONG, 0, mynode.summary_chid, DBE_VALUE|DBE_ALARM, eventCallback, (void*)&mynode, NULL);

	/*Should never return from following call*/
	SEVCHK(ca_pend_event(0.0),"ca_pend_event");
	return 0;
}
