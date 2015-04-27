#include "ArchiveThread.h"
#include "ArchiveSheet.h"

static bool viewer(void *arg, const char *name, size_t , size_t i, const CtrlInfo &,
             DbrType , DbrCount , const RawValue::Data *value)
{
	static int row = 0, col = 0;
	ArchiveSheet *pSheet = (ArchiveSheet*)arg;
	if(pSheet -> getRowCol()== true) row = col = 0;

    if (i==0)
    {
		row = 0;
		pSheet->setPvNames(name);
		col++;
    };

	if (col == 1)
	{
		char timebuf[30];
		epicsTimeToStrftime(timebuf,28,"%m/%d/%Y %H:%M:%S", &(value->stamp));
		QString qtime = timebuf;
		pSheet->setEpicTimes(qtime);
	};

	QString pvname = name;
	double pvvalue = value->value;
	pSheet->addPvValues(pvname, pvvalue);

	if ( ++row == ArchiveSheet::MAX_COUNT )
	{
		pSheet -> setEpicsTimeStamp(value->stamp);
	}
	pSheet->setRowCol(false);
    return true;
}

ArchiveThread::ArchiveThread(ArchiveDataClient *client, stdVector<stdString> &names,
					epicsTime &start, epicsTime &end, int count, int how, double interval, void *arg)
	: m_arclient(client),mpvnames(names),mstart(start),
	mend(end),m_interval(interval),mcount(count),mhow(how),marg(arg)
{
}

ArchiveThread::~ArchiveThread()
{
	exit();
}

void ArchiveThread::run()
{
	mutex.lock();
	ArchiveSheet *pSheet = (ArchiveSheet*)marg;
	int archivekey = pSheet->archiveKey();
    m_arclient->getValues(archivekey, mpvnames, mstart, mend, mcount, mhow, viewer, marg, m_interval);
	pSheet->updateDisplay();
	//Buffering... +- 2pages.

	mutex.unlock();
}
