#include "SinglePlotThread.h"
#include "SinglePlot.h"

static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
    chid	chid = args.chid;
    //long	stat = args.stat; /* Channel access status code*/
    const char  *channel;
    static char *noname = "unknown";
    channel = (chid ? ca_name(chid) : noname);
    if(chid) printChidInfo(chid,"exceptionCallback");

};

static void accessRightsCallback(struct access_rights_handler_args args)
{
    chid	chid = args.chid;
    printChidInfo(chid,"accessRightsCallback");
};

static void connectionCallback(struct connection_handler_args args)
{
    chid	chid = args.chid;
    printChidInfo(chid,"connectionCallback");
};

static void eventCallback(struct event_handler_args eha)
{
    chid	chid = eha.chid;
	MYNODE *pNode = (MYNODE*)eha.usr;
	SinglePlot	*plot = (SinglePlot *)pNode->plot;
    if( eha.status != ECA_NORMAL ) 
	{
		printChidInfo(chid,"eventCallback");
    } 
	else 
	{
#if 1
		struct dbr_time_double* pdata = (struct dbr_time_double*)eha.dbr;
		epicsTime  stamp = pdata -> stamp;          /* time stamp */
		struct local_tm_nano_sec tm = (local_tm_nano_sec) stamp;
		int totsec = tm.ansi_tm.tm_hour*3600+tm.ansi_tm.tm_min*60+tm.ansi_tm.tm_sec;
		plot -> GetValue( pdata -> value, totsec,tm.ansi_tm.tm_year,tm.ansi_tm.tm_mon, tm.ansi_tm.tm_mday );
		//qDebug("value:%f, unit:%s, serverity:%d\n", pdata->value, pdata->units, pdata->severity);
		//plot -> GetValue( pdata -> value, totsec );
		//qDebug("Thred-tot:%d, hour:%d, min:%d, sec:%d", totsec,tm.ansi_tm.tm_hour,tm.ansi_tm.tm_min,tm.ansi_tm.tm_sec);
		//plot -> GetValue( pdata -> value, tm.ansi_tm );
		//plot -> GetValue( pdata -> value, tm );
#else
		struct dbr_gr_double* pdata = (struct dbr_gr_double*)eha.dbr;
		qDebug("value:%f, unit:%s, serverity:%d, ulimit:%f, llimit:%f\n", pdata->value, pdata->units, pdata->severity,
								pdata->upper_disp_limit, pdata->lower_disp_limit);
#endif
    };
};

SinglePlotThread::SinglePlotThread(const QString &pvname, SinglePlot *plot, const int periodic):m_pvname(pvname), m_plot(plot), mperiodic(periodic)
{
    //ca_context_create(ca_enable_preemptive_callback);
    //ca_context_create(ca_disable_preemptive_callback);
    ca_add_exception_event(exceptionCallback,NULL);
	setStop(false);
	//start();
}

SinglePlotThread::~SinglePlotThread()
{
	qDebug("SinglePlotThread Terminate");
	//for event
	//ca_clear_subscription (mynode.myevid);
	//ca_clear_channel(mynode.mychid);
	this->exit();
}

void SinglePlotThread::run()
{
#if 0
	// event gathering
	//printchannel();	//for Debug
	mutex.lock();
	mynode.plot	= m_plot;
	chid unit_chid;
	dbr_string_t units;
	QString unitch = m_pvname + ".EGU";
	ca_create_channel(unitch.toStdString().c_str(), 0, 0, 0, &unit_chid);
	ca_pend_io(0.1);
	ca_get(DBR_STRING, unit_chid, (void *)&units);
	ca_pend_io(0.1);
	m_plot->SetUnit(units);

	ca_create_channel(m_pvname.toStdString().c_str(), connectionCallback, &mynode, 0, (oldChannelNotify**)&mynode.mychid);
	ca_replace_access_rights_event(mynode.mychid, accessRightsCallback);
	ca_create_subscription (DBR_TIME_DOUBLE, 0, mynode.mychid, DBE_VALUE|DBE_ALARM, eventCallback, &mynode, &mynode.myevid);
	//ca_add_event(DBR_GR_DOUBLE, mynode.mychid, eventCallback, &mynode, &mynode.myevid);
	mutex.unlock();
	ca_pend_event(0.0);
#else
	//periodic gathering
	mutex.lock();
	chid unit_chid, val_chid;
	dbr_string_t units;

	QString unitch = m_pvname + ".EGU";
	ca_create_channel(unitch.toStdString().c_str(), 0, 0, 0, &unit_chid); ca_pend_io(0.2);
	ca_get(DBR_STRING, unit_chid, (void *)&units); ca_pend_io(0.2);
	m_plot->SetUnit(units);

	struct dbr_time_double data;
	ca_create_channel(m_pvname.toStdString().c_str(), 0, 0, 0, &val_chid);
	ca_pend_io(0.2);

	epicsTime  stamp;

	struct local_tm_nano_sec tm;
	int totsec = 0;
	//for periodic single plot local time
	int year, month, day, hour, min, sec;
	int factor = 1;
	switch(mperiodic)
	{
		case PointOne:
			factor *= 0.1;
			break;
		case PointFive:
			factor *= 0.5;
			break;
		case FiveSec:
			factor *= 5;
			break;
		case TenSec:
			factor *= 10;
			break;
		case OneSec:
		default:
			break;
	};
	while(getStop()==false)
	{
		ca_get(DBR_TIME_DOUBLE, val_chid, (void *)&data);
		ca_pend_io(0.2);
		//qDebug("%s : %f\n",ca_name(val_chid), data.value);
		Epoch2Datetime(year, month, day, hour, min, sec);
#if 0
		stamp = data.stamp;
		tm = (local_tm_nano_sec) stamp;
		totsec = tm.ansi_tm.tm_hour*3600+tm.ansi_tm.tm_min*60+tm.ansi_tm.tm_sec;
		m_plot->GetValue(data.value, totsec,tm.ansi_tm.tm_year,tm.ansi_tm.tm_mon, tm.ansi_tm.tm_mday );
#else
		totsec = hour*3600+min*60+sec;
		m_plot->GetValue(data.value, totsec, year, month, day);
#endif
		usleep(1000000*factor);
	};
	//ca_clear_channel(unit_chid);
	//ca_clear_channel(val_chid);
	ca_context_destroy();
	mutex.unlock();
	exit();
	qDebug("SinglePlot Exit");
#endif
}
void 
SinglePlotThread::Epoch2Datetime (int &year, int &month, int &day, int &hour, int &minute, int &second, time_t epochtime )
{
	struct tm *ptime;

	if ( epochtime == 0 ) epochtime = time (NULL);
	ptime = localtime ( &epochtime );

	year    = ptime -> tm_year;
	month   = ptime -> tm_mon;
	day     = ptime -> tm_mday;
	hour    = ptime -> tm_hour;
	minute  = ptime -> tm_min;
	second  = ptime -> tm_sec;
};

const bool
SinglePlotThread::getStop()
{
	return mstop;
}
void
SinglePlotThread::setStop( const bool stop)
{
	mstop = stop;
}
void
SinglePlotThread::printchannel()
{
}
