#include "caUitime.h"

class CAUITime::PrivateData
{
public:
    PrivateData()	//: initializ3
    {
		timeset=Periodic;
		//periodicsecond=2Second;
		periodicsecond=Two;
		//m_masterpv = "pvname";
    };

	TimeSet timeset;
	PeriodicSecond periodicsecond;
	QString m_masterpv;
	bool m_val;

};

//
CAUITime::CAUITime(QWidget *parent): QWidget(parent)
{
    p_data = new PrivateData;
};

CAUITime::~CAUITime()
{
};

CAUITime::TimeSet 
CAUITime::getTimeSet() const
{
	return (TimeSet)p_data->timeset;
}
void 
CAUITime::setTimeSet(TimeSet timeset)
{
	p_data->timeset=timeset;
}

CAUITime::PeriodicSecond 
CAUITime::getPeriodicSecond() const
{
	return (PeriodicSecond)p_data->periodicsecond;
}
void 
CAUITime::setPeriodicSecond(PeriodicSecond periodicsecond)
{
	p_data->periodicsecond=periodicsecond;
}

const QString &CAUITime::getMasterPV() const
{
	return p_data->m_masterpv;
} 

void
CAUITime::setMasterPV(const QString &masterpv)
{
	p_data->m_masterpv = masterpv;
	setStatusTip(QString("MasterPV : ").append(p_data->m_masterpv));
}

void 
CAUITime::changeValue(const int &value)
{
	p_data->m_val = (bool) value;
	setStatusTip(QString("PVname : ").append(p_data->m_masterpv).append(QString(", Value: ")).append(QString("%1").arg((double)value,1,'g',3)));
	//qDebug("Emitted valueChanged signal. 1, value:%f", value);
#if 0
	if (p_data->m_val == true)
		emit valueChanged(true);
	else
		emit valueChanged(false);
#endif
	//qDebug("Emitted valueChanged signal. 2");
}
//
