/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caWclock.h"


class CAWclock::PrivateData
{
public:
    PrivateData():
		m_trig1(0),
		m_trig2(0),
		m_lastintvalue(0)
    {
    };

	QString m_pvname;
	TimeFormat m_tformat;
	int m_trig1, m_trig2;
	int m_lastintvalue;
};

CAWclock::CAWclock(QWidget *parent):QLabel(parent)
{
    initWclock();
}

CAWclock::~CAWclock()
{
    delete d_data;
}

QSize CAWclock::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAWclock::minimumSizeHint() const
{
	int mw = 120;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAWclock::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAWclock::getPvname() const
{
	return d_data->m_pvname;
}

CAWclock::TimeFormat CAWclock::getTformat() const
{
	return (TimeFormat) d_data->m_tformat;
}

void CAWclock::setTformat (CAWclock::TimeFormat format)
{
	d_data->m_tformat = format;
}

const int CAWclock::getTrigger1()
{
	return d_data->m_trig1;
}

void CAWclock::setTrigger1 (const int trigger1)
{
	d_data->m_trig1 = trigger1;
}

const int CAWclock::getTrigger2()
{
	return d_data->m_trig2;
}

void CAWclock::setTrigger2 (const int trigger2)
{
	d_data->m_trig2 = trigger2;
}

void CAWclock::initWclock()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

    	setObjectName(QString::fromUtf8("CAWclock"));
	setPvname("pvname");
	setText(getPvname());
	setTformat(Date_Time);
}

void CAWclock::changeValue (const double &value)
{
/*
	QString sval;
	sval = QString("%1").arg(value,1,'f',9);
	setText(sval);
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg((double)value,1,'g',3)));
*/
	epicsTimeStamp ts;
	ts.secPastEpoch = (epicsUInt32) value;
	ts.nsec = (epicsUInt32) (1.0E+9*(value - ts.secPastEpoch));

	epicsTime time;
	time_t_wrapper ttw;
	time_t tt;

	time = ts;
	ttw = time;
	tt = ttw.ts;


#if 0
	if (value < 0)
	{
		setText("");
		return;
	} else if (value == 0)
	{
		setText("00:00:00");
		return;
	}
#endif
#if 1
	if (value == 0)
	{
		setText("00:00:00");
		return;
	}
#endif


	if ((int)value == getTrigger1() && d_data->m_lastintvalue != (int)value)
	{
		emit trigger1();
	} else if ((int)value == getTrigger2() && d_data->m_lastintvalue != (int)value)
	{
		emit trigger2();
	}
	d_data->m_lastintvalue = (int)value;

	//epicsTime_gmtime time = (ts);
	char buf[30];
	//time.strftime(buf, 30, "%Y/%m/%d %H:%M:%S.%06f");
	if (getTformat() == Date_Time)
	{
		strftime(buf, 30, "%Y/%m/%d %H:%M:%S",localtime(&tt));
	} else if(getTformat() == Time)
	{
		//time.strftime(buf, 30, "%H:%M:%S");
		strftime(buf, 30, "%H:%M:%S", localtime(&tt));
	} else if(getTformat() == Duration)
	{
		strftime(buf, 30, "%H:%M:%S", gmtime(&tt));
	}

	QString sval = QString(buf);
	setText(sval);
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

void CAWclock::changeValue (const short &connstatus, const QString &value, const short& severity)
{
	QString sval = value;
	setText(sval);
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

void CAWclock::changePvName (const QString &pvname)
{
	setPvname(pvname);
}

