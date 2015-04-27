#include "caQtSignal.h"

class CAQtSignal::PrivateData
{
public:
    PrivateData()
    {
    };
	QString m_prefix;
	QString m_pvname;
	QString m_condition;
	double  m_oldvalue;
};

//
CAQtSignal::CAQtSignal(QWidget *parent): QWidget(parent)
{
    p_data = new PrivateData;
};

CAQtSignal::~CAQtSignal()
{
};

void CAQtSignal::changeValue (const short &connstatus, const double &value)
{
	//QString strValue = QString("%1").arg(value);
	//if(getPvname() == "CCS_LCSST_DATA" && (value == 0 || value == 2))
	//if(getPvname() == "DDS2_DAQ_STATUS")
	QString pvname = getPvname();
	if(pvname == "PCS_LS_STATUS")
	{
		//if( p_data->m_oldvalue == 7 && value == 1 )
		if( value == 1 )
			emit caQtSignal(value);
		p_data->m_oldvalue = value;
	}
	else if(pvname == "MDS_SHOT_NUMBER")
	{
		QString strLabel = QString("@%1").arg(value); 
		emit changeLabel(strLabel);

		emit caQtSignal(value);
	}
	else if(pvname.contains("OP_MODE", Qt::CaseInsensitive))
	{
		emit setEnable((bool)value); 
	}
	else
	{
		emit caQtSignal(value);
	};
};

void CAQtSignal::setPrefix(const QString &prefix)
{
	    p_data->m_prefix = prefix;
}

const QString &CAQtSignal::getPrefix() const
{
	    return p_data->m_prefix;
}

const QString &CAQtSignal::getCondition() const
{
	return p_data->m_condition;
}

void CAQtSignal::setCondition(const QString &condition)
{
    p_data->m_condition = condition;
}

const QString &CAQtSignal::getPvname() const
{
	return p_data->m_pvname;
}

void CAQtSignal::setPvname(const QString &name)
{
    p_data->m_pvname = name;
}
