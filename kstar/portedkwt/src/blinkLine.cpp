/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * KWT Widget Library
 * Copyright (C) 2010   Leesi
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the KWT License,

 *****************************************************************************/

// vim: expandtab

#include "blinkLine.h"


class BlinkLine::PrivateData
{
public:
    PrivateData():
		normalcolor(QColor("green")),
		alarmcolorR(QColor(255,110,15,200)),
		alarmcolorF(QColor("red")),
		brush(normalcolor)
    {
		brush.setStyle(Qt::SolidPattern);
		m_count = 0;
    };

	//ColorMode cmode;
	eAlarm alarm;
    QString m_ipaddr;
	QString m_truelabel;
	QString m_falselabel;
	QStringList m_msg;
	QColor normalcolor, alarmcolorR, alarmcolorF;
	unsigned int m_count;
	QBrush brush;
	QPalette palette;
	int timerid;
	int reversecolor;
};

BlinkLine::BlinkLine(QWidget *parent):QFrame(parent)
{
    init();
}

BlinkLine::~BlinkLine()
{
    delete d_data;
}

QSize BlinkLine::sizeHint() const
{
	return minimumSizeHint();
}

QSize BlinkLine::minimumSizeHint() const
{
	int mw = 100;
	int mh = 7;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void BlinkLine::setIPAddr(const QString &ipaddr)
{
    d_data->m_ipaddr = ipaddr;
}

const QString &BlinkLine::getIPAddr() const
{
	return d_data->m_ipaddr;
}
unsigned int BlinkLine::getCount() const
{
	return d_data->m_count;
}
void BlinkLine::setCount(unsigned int count)
{
	d_data->m_count = count;
}
void  BlinkLine::IncCount()
{
	d_data->m_count++;
}

#if 0
BlinkLine::ColorMode BlinkLine::getColormode() const
{
	return d_data->cmode;
}
void BlinkLine::setColormode(ColorMode cmode)
{
	d_data->cmode = cmode;
}
#endif

BlinkLine::eAlarm BlinkLine::getAlarm()
{
	setColor(d_data->normalcolor);
	return d_data->alarm;
}

void BlinkLine::setAlarm(eAlarm alarm)
{
	d_data->alarm = alarm;
}

const QColor& BlinkLine::getNormalColor() const
{
	return d_data->normalcolor;
}
void BlinkLine::setNormalColor(const QColor &color)
{
	d_data->normalcolor = color;
}

const QColor& BlinkLine::getAlarmColorR() const
{
	return d_data->alarmcolorR;
}
void BlinkLine::setAlarmColorR(const QColor &color)
{
	d_data->alarmcolorR = color;
}

const QColor& BlinkLine::getAlarmColorF() const
{
	return d_data->alarmcolorF;
}
void BlinkLine::setAlarmColorF(const QColor &color)
{
	d_data->alarmcolorF = color;
}

void BlinkLine::init()
{
#if QT_VERSION >= 0x040000
	using namespace Qt;
#endif
	d_data = new PrivateData;

	setObjectName(QString::fromUtf8("BlinkLine"));
	//setColormode(Static);

	setAlarm(NormalState);
	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, d_data->brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, d_data->brush);
	setFrameShadow(QFrame::Plain);
	setLineWidth(3);
	setPalette(d_data->palette);
	setFrameShape(QFrame::HLine);
	startTimer(1000);
}

void BlinkLine::timerEvent(QTimerEvent* /*event*/)
{
	if(getAlarm() == NormalState) 
	{
		setColor(d_data->normalcolor);
	}
	else if(getAlarm() == AlarmState && d_data->reversecolor == 0)
	{
		d_data->reversecolor = 1;
		setColor(d_data->alarmcolorR);
	}
	else
	{
		d_data->reversecolor = 0;
		setColor(d_data->alarmcolorF);
	};
}

void BlinkLine::changeValue (const short &connstatus, const double &value, const short& /*severity*/)
{
	int pvval = static_cast<int> (value);
	if (connstatus != ECA_CONN)
	{
		setColor(QColor("gray"));
	} 
	else if ( connstatus == ECA_CONN && pvval == 1)
	{
		setAlarm(AlarmState);
	}
	else
	{
		setAlarm(NormalState);
	};
}

void BlinkLine::setColor(const QColor &color)
{
	QBrush brush(color);
	brush.setStyle(Qt::SolidPattern);
	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	setPalette(d_data->palette);
}
