//#include "mdsplusdata.h"
//#include "shotsummary.h"
#include "sessionsummarythr.h"

#define status_ok(status) (((status) & 1) == 1)

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

void eventCallback(struct event_handler_args eha)
{
	MyNode *pNode = (MyNode *)ca_puser(eha.chid);
	SessionSummary *pSummary = (SessionSummary*)pNode->mpsummary;

	if(eha.status!=ECA_NORMAL)
	{
		printChidInfo(eha.chid,"eventCallback");
	}
	else
	{
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
		//int summarystatus = pSummary->ReadMDSData(updateshot);
		//printf("updateshot:%d, status:%d\n",updateshot, summarystatus);
		struct dbr_time_long* pdata = (struct dbr_time_long*)eha.dbr;
		if(pSummary->GetAuto() == true && pdata->value == (long)1)
		{
			QString hostname = pSummary->GetHostname();
			if(hostname.compare("opi16")==0)
			{
				int updateshot = CACommand("ShotNum");
				qDebug("Update--MDSSHOT:%d",updateshot);
				pSummary->ReadMDSData(updateshot);
				CACommand("ResetShotSummary");
			}
			else
			{
				pSummary->Update();
			}
			// check...
			//unsigned int ccsshot=0;
			//unsigned int mdsshot=0;
			//int status = pSummary->GetUpdateShotNum(ccsshot, mdsshot);
			//qDebug("CCSSHOT:%d,MDSSHOT:%d",ccsshot,mdsshot);
			//pSummary->ResetCCSPShotN();
		}
		else
		{
			qDebug("Initialze-----");
			pSummary->SetAuto(true);
		};
#endif
	};
}

SessionSummaryThr::SessionSummaryThr(const SessionSummary *pSession):mpSession(pSession)
{
	start();
}

SessionSummaryThr::~SessionSummaryThr()
{
}
void
SessionSummaryThr::run()
{
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return;
	};
	MyNode  mynode(mpSession);
	//mynode.prevshotnum = startshot;
	SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");
	SEVCHK(ca_add_exception_event(exceptionCallback,NULL), "ca_add_exception_event");
	//string pvnames[]={"CCS_SHOT_NUMBER","CCS_PERFORM_SHOT_SUMMARY"};
	//Test..
	//SEVCHK(ca_create_channel("DDS2_getState",connectionCallback,
	//			&mynode,10,(oldChannelNotify**)&mynode.summary_chid), "ca_create_channel");
	SEVCHK(ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",connectionCallback,
				&mynode,10,(oldChannelNotify**)&mynode.summary_chid), "ca_create_channel");
	SEVCHK(ca_replace_access_rights_event(mynode.summary_chid,
				accessRightsCallback), "ca_replace_access_rights_event");
	ca_create_subscription (DBR_TIME_LONG, 0, mynode.summary_chid, DBE_VALUE|DBE_ALARM, eventCallback, (void*)&mynode, NULL);

	/*Should never return from following call*/
	SEVCHK(ca_pend_event(0.0),"ca_pend_event");
}

