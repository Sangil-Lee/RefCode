/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * KWT Widget Library
 * Copyright (C) 2010   Leesi
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the KWT License,

 *****************************************************************************/

// vim: expandtab

#include "caBlinkLabel.h"


class CABlinkLabel::PrivateData
{
public:
    PrivateData():
		//normalcolor(QColor("green")),
		normalcolor(QColor(0,190,0,255)),
		alarmcolorR(QColor(255,110,15,200)),
		alarmcolorF(QColor("red")),
		brush(normalcolor),
		reversecolor(0)

    {
		brush.setStyle(Qt::SolidPattern);
    };

	eAlarm alarm;
	QString m_pvname;
    QString m_prefix;
	QString m_truelabel;
	QString m_falselabel;
	QStringList m_msg;
	QColor normalcolor, alarmcolorR, alarmcolorF;
	QBrush brush;
	QPalette palette;
	int timerid;
	int reversecolor;
};

CABlinkLabel::CABlinkLabel(QWidget *parent):QLabel(parent)
{
    init();
}

CABlinkLabel::~CABlinkLabel()
{
    delete d_data;
}

QSize CABlinkLabel::sizeHint() const
{
	return minimumSizeHint();
}

QSize CABlinkLabel::minimumSizeHint() const
{
	int mw = 56;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CABlinkLabel::setPvname(const QString &name)
{
    d_data->m_pvname = name;
}

const QString &CABlinkLabel::getPvname() const
{
	return d_data->m_pvname;
}

void CABlinkLabel::setPrefix(const QString &prefix)
{
    d_data->m_prefix = prefix;
}

const QString &CABlinkLabel::getPrefix() const
{
	return d_data->m_prefix;
}

void CABlinkLabel::init()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

	//setText("CABlinkLabel");
   	setObjectName(QString::fromUtf8("CABlinkLabel"));
	setPvname("pvname");
	setAlarm(NormalState);
	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, d_data->brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, d_data->brush);
    setFrameShape(QFrame::Box);
	setFrameShadow(QFrame::Plain);
	setLineWidth(3);
	setPalette(d_data->palette);
    setAlignment(Qt::AlignCenter);
    setMargin(1);
    setIndent(-1);
	startTimer(1000);
    //setPixmap(QPixmap(QString::fromUtf8("opi_image_5.png")));
}

void CABlinkLabel::changePvName (const QString &pvname)
{
    setPvname(pvname);
}

CABlinkLabel::eAlarm CABlinkLabel::getAlarm()
{
#if 0
	if(d_data->timerid && d_data->alarm == NormalState )
	{
		killTimer(d_data->timerid);
	};
#endif
	setColor(d_data->normalcolor);
	return d_data->alarm;
}

void CABlinkLabel::setAlarm(eAlarm alarm)
{
	//if(alarm == AlarmState)
		//d_data->timerid = startTimer(1000);
	d_data->alarm = alarm;
}
const QColor& CABlinkLabel::getNormalColor() const
{
	return d_data->normalcolor;
}
void CABlinkLabel::setNormalColor(const QColor &color)
{
	d_data->normalcolor = color;
}

const QColor& CABlinkLabel::getAlarmColorR() const
{
	return d_data->alarmcolorR;
}
void CABlinkLabel::setAlarmColorR(const QColor &color)
{
	d_data->alarmcolorR = color;
}

const QColor& CABlinkLabel::getAlarmColorF() const
{
	return d_data->alarmcolorF;
}
void CABlinkLabel::setAlarmColorF(const QColor &color)
{
	d_data->alarmcolorF = color;
}

void CABlinkLabel::timerEvent(QTimerEvent* /*event*/)
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
	else if(getAlarm() == AlarmState && d_data->reversecolor == 1)
	{
		d_data->reversecolor = 0;
		setColor(d_data->alarmcolorF);
	}
	else if(getAlarm() == DisConnState && d_data->reversecolor == 0)
	{
		d_data->reversecolor = 1;
		setColor(QColor(Qt::red));
	}
	else if(getAlarm() == DisConnState && d_data->reversecolor == 1)
	{
		d_data->reversecolor = 0;
		setColor(QColor(Qt::darkRed));
	}
	else if(getAlarm() == NotDefine)
	{
		setColor(QColor("gray"));
	}
}

void CABlinkLabel::changeValue (const short &connstatus, const double &value, const short& severity)
{
	int pvval = static_cast<int> (value);
	if (connstatus != ECA_CONN && severity == -1)
	{
		setAlarm(DisConnState);
	} 
	else if (connstatus != ECA_CONN && severity == -2)
	{
		setAlarm(NotDefine);
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

void CABlinkLabel::setColor(const QColor &color)
{
	QBrush brush(color);
	brush.setStyle(Qt::SolidPattern);
	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	setPalette(d_data->palette);
}
