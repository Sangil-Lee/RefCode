#include "DataModellerThread.h"
#include "MultiPlot.h"

static void printChidInfo(chid chid, char *message)
{
    printf("pv: %s  type(%d) nelements(%ld) host(%s)", ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid));
    printf(" read(%d) write(%d) state(%d), message:%s\n", ca_read_access(chid),ca_write_access(chid),ca_state(chid), message);
};

static void exceptionCallback(struct exception_handler_args args)
{
	qDebug("exceptionCallback called");
    chid	chid = args.chid;
    //long	stat = args.stat; /* Channel access status code*/
    const char  *channel;
    static char *noname = "unknown";
    channel = (chid ? ca_name(chid) : noname);
    if(chid) printChidInfo(chid,"exceptionCallback");

};

DataModellerThread::DataModellerThread(MultiPlot *plot, const vector<string> &pvnames):m_plot(plot), m_pvnames(pvnames)
{
    //ca_context_create(ca_disable_preemptive_callback); //ca_enable_preemptive_callback
    //ca_add_exception_event(exceptionCallback,NULL);
	isstop = false;
}

DataModellerThread::~DataModellerThread()
{
#if 0
	qDebug("Thread Terminate");
	for(size_t i = 0; i < m_node.size(); i++)
	{
		ca_clear_channel(m_node.at(i));
		//ca_context_destroy();
	};
	exit();
#endif
}

void DataModellerThread::SetPvnames(const vector<string> &pvnames)
{
	m_pvnames=pvnames;
}
void DataModellerThread::run()
{
	struct dbr_time_double data[m_pvnames.size()];
	chid mychid[m_pvnames.size()];
	mutex.lock();
	bool bfirst[MultiPlot::MAX_PV_CNT];
	for (int i = 0; i < MultiPlot::MAX_PV_CNT; i++ )
	{
		bfirst[i] = true;
	};
	for(size_t i = 0; i < m_pvnames.size(); i++)
	{
		ca_create_channel(m_pvnames.at(i).c_str(), NULL, NULL, 10, &mychid[i]);
		m_node.push_back(mychid[i]);
		ca_pend_io(1.0);
	};
	epicsTime  stamp;
	struct local_tm_nano_sec tm;
	int totsec = 0;
	while(getStop()==false)
	{
		for(size_t i = 0; i < m_pvnames.size(); i++)
		{
			ca_get(DBR_TIME_DOUBLE,m_node.at(i), (void *)&data[i]);
			ca_pend_io(1.0);
			//qDebug("%s : %f\n",ca_name(m_node.at(i)), data[i].value);
			stamp = data[0].stamp;
			tm = (local_tm_nano_sec) stamp;
			totsec = tm.ansi_tm.tm_hour*3600+tm.ansi_tm.tm_min*60+tm.ansi_tm.tm_sec;

			if (bfirst[i] == true)
			{
				bfirst[i] = false;
				m_plot->Initialize(i, data[i].value);
			};
			m_plot->GetValue(i, data[i].value, totsec,tm.ansi_tm.tm_year,tm.ansi_tm.tm_mon, tm.ansi_tm.tm_mday );
		};
		//usleep(1000000);
		usleep(m_plot->GetTimeInterval());
	};
	mutex.unlock();
	for(size_t i = 0; i < m_node.size(); i++)
	{
		ca_clear_channel(m_node.at(i));
		//ca_context_destroy();
	};
	exit();
}
void
DataModellerThread::printchannel()
{
}
