#include <cadef.h>
#include <dbDefs.h>

#include "caFanOut.h"

class CAFanOut::PrivateData
{
public:
    PrivateData()
    {
    };

	QStringList m_pvlist;
};

//
CAFanOut::CAFanOut(QWidget *parent): QWidget(parent)
{
    p_data = new PrivateData;
};

CAFanOut::~CAFanOut()
{
};
int CAFanOut::caPut(const QString pvname, const double &value, int type)
{
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	qDebug("PVName:%s, Value:%f", pvname.toStdString().c_str(), value);

	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	switch(type)
	{
		case DBR_LONG:
			{
				long longval = (long) value;
				status = ca_put(DBR_LONG, chan_id, &longval);
			};
			break;
		case DBR_FLOAT:
			{
				float fval = (float) value;
				status = ca_put(DBR_FLOAT, chan_id, &fval);
			};
			break;
		case DBR_DOUBLE:
			{
				double dval = value;
				status = ca_put(DBR_DOUBLE, chan_id, &dval);
			};
			break;
		case DBR_CHAR:
			{
				char cval = (char)value;
				status = ca_put(DBR_CHAR, chan_id, &cval);
			};
			break;
		case DBR_INT:
		default:	
			{
				int intval = (int)value;
				status = ca_put(DBR_INT, chan_id, &intval);
			};
			break;
	};

	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPut timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	return status;
};
int CAFanOut::caPutString(const QString pvname, const QString value)
{
	chid  chan_id;
	int status = ca_search(pvname.toStdString().c_str(), &chan_id);

	SEVCHK(status,NULL);
	status = ca_pend_io(1.0);
	if(status != ECA_NORMAL)
	{
		ca_clear_channel(chan_id);
		qDebug("Not Found %s\n", pvname.toStdString().c_str());
		return status;
	};

	const char *sval = value.toStdString().c_str();
	status = ca_put(DBR_STRING, chan_id, sval);
	status = ca_pend_io(1.0);
	if (status == ECA_TIMEOUT) {
		qDebug("caPutString timed out: Data was not written.\n");
		return status;
	}
	ca_clear_channel(chan_id);
	return status;
};

const QStringList CAFanOut::getPVs() const
{
	return p_data->m_pvlist;
}

void CAFanOut::setPVs(const QStringList pvlist)
{
	p_data->m_pvlist = pvlist;
}

void CAFanOut::fanOutValue(const double &value)
{
	QStringList  pvlist = getPVs();
	for(int i = 0; i < pvlist.size();++i)
	{
		const QString syncpv = pvlist.at(i);
		caPut(syncpv, value, DBR_DOUBLE);
	};
};

void CAFanOut::fanOutValue(const int &value)
{
	QStringList  pvlist = getPVs();
	for(int i = 0; i < pvlist.size();++i)
	{
		QString syncpv = pvlist.at(i);
		caPut(syncpv, value, DBR_INT);
	};
};

void CAFanOut::fanOutValue(const QString value)
{
	QStringList  pvlist = getPVs();
	for(int i = 0; i < pvlist.size();++i)
	{
		QString syncpv = pvlist.at(i);
		caPutString(syncpv, value);
	};
};


