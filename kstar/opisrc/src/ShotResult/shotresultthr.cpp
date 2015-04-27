#include "shotresultthr.h"

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
};
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
	ShotResult *pShot = (ShotResult*)pNode->mpshot;

	if(eha.status!=ECA_NORMAL)
	{
		printChidInfo(eha.chid,"eventCallback");
	}
	else
	{
	};
}

ShotResultThr::ShotResultThr(const ShotResult *pShot):mpShot(pShot)
{
	start();
}

ShotResultThr::~ShotResultThr()
{
}
void
ShotResultThr::run()
{
	if ( mysqlpp::get_library_version() != MYSQLPP_HEADER_VERSION )
	{
		cerr<< "Library/header version number mismatch" << endl;
		return;
	};
	MyNode  mynode(mpShot);
	SEVCHK(ca_context_create(ca_disable_preemptive_callback),"ca_context_create");
	SEVCHK(ca_add_exception_event(exceptionCallback,NULL), "ca_add_exception_event");
	SEVCHK(ca_create_channel("CCS_PERFORM_SHOT_SUMMARY",connectionCallback,
				&mynode,10,(oldChannelNotify**)&mynode.shot_chid), "ca_create_channel");
	SEVCHK(ca_replace_access_rights_event(mynode.shot_chid,
				accessRightsCallback), "ca_replace_access_rights_event");
	ca_create_subscription (DBR_TIME_LONG, 0, mynode.shot_chid, DBE_VALUE|DBE_ALARM, eventCallback, (void*)&mynode, NULL);

	/*Should never return from following call*/
	SEVCHK(ca_pend_event(0.0),"ca_pend_event");
}

