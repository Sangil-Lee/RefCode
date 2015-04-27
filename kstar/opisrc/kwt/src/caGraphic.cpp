/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caGraphic.h"


class CAGraphic::PrivateData
{
public:
    PrivateData():
		m_pvname(QString("pvname")),
		m_filename(QString("")),
		m_val(false),
		m_popup(true),
		m_passwd(false),
		m_passwdstr(""),
		first(1),minorcolor(QColor("yellow")),majorcolor(QColor("red"))
    {
    };

	DisplayMode m_fdmode;
	DisplayMode m_ldmode;

	bool m_popup;
	QString m_pvname;
	QString m_filename;
	bool	m_passwd;
	QString m_passwdstr;
	QStringList m_pvs;
	double m_val;
	double m_blinkvalue;
	int first;
	QColor minorcolor;
	QColor majorcolor;
};

/*
CAGraphic::CAGraphic(QWidget *parent)
	:QWidget(parent, Qt::FramelessWindowHint)
*/
CAGraphic::CAGraphic(QWidget *parent) : StaticGraphic(parent)
//	:StaticGraphic(parent, Qt::FramelessWindowHint)
{
    initGraphic();
}

CAGraphic::~CAGraphic()
{
    delete d_data;
}

QSize CAGraphic::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAGraphic::minimumSizeHint() const
{
	int mw = 5;
	int mh = 5;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const bool &CAGraphic::getPopup() const
{
	    return d_data->m_popup;
}
void CAGraphic::setPopup(const bool control)
{
	 d_data->m_popup = control;

	if (control==true)
	{
		setCursor(Qt::PointingHandCursor);
	}
}

void CAGraphic::setPasswd(const bool passwd)
{
    d_data->m_passwd = passwd;
}

const bool &CAGraphic::getPasswd() const
{
	return d_data->m_passwd;
}

void CAGraphic::setPasswdStr(const QString &passwdstr)
{
    d_data->m_passwdstr = passwdstr;
}

const QColor& CAGraphic::getMinorAColor() const
{
	return d_data->minorcolor;
}

void CAGraphic::setMinorAColor(const QColor &color)
{
	d_data->minorcolor = color;
}

const QColor& CAGraphic::getMajorAColor() const
{
	return d_data->majorcolor;
}

void CAGraphic::setMajorAColor(const QColor &color)
{
	d_data->majorcolor = color;
}

const QString &CAGraphic::getPasswdStr() const
{
	return d_data->m_passwdstr;
}


const QString &CAGraphic::getPvname() const
{
	return d_data->m_pvname;
}

void CAGraphic::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	//setToolTip(d_data->m_pvname);
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(" (no channel)")));
}

const QString &CAGraphic::getFilename() const
{
	return d_data->m_filename;
}

void CAGraphic::setFilename(const QString &fname)
{
    d_data->m_filename = fname;
	setStatusTip(QString("Filename : ").append(d_data->m_filename));
}

int CAGraphic::getBlinkValue() const 
{
    return d_data->m_blinkvalue;
}

void CAGraphic::setBlinkValue(int blinkvalue)
{
    d_data->m_blinkvalue = blinkvalue;
}

int CAGraphic::getValue() const
{
	return d_data->m_val;
}
const QStringList &CAGraphic::getPvs() const
{
	return d_data->m_pvs;
}

void CAGraphic::setPvs(const QStringList &pvs)
{
    d_data->m_pvs = pvs;
}

CAGraphic::DisplayMode CAGraphic::fillDisplayMode() const
{
	return (DisplayMode) d_data->m_fdmode;
}

void CAGraphic::setFillDisplayMode(CAGraphic::DisplayMode dmode)
{
	d_data->m_fdmode = dmode;
}

CAGraphic::DisplayMode CAGraphic::lineDisplayMode() const
{
	return (DisplayMode) d_data->m_ldmode;
}

void CAGraphic::setLineDisplayMode(CAGraphic::DisplayMode dmode)
{
	d_data->m_ldmode = dmode;
}

void CAGraphic::initGraphic()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;
	//setFillColor(QColor(0,255,0,255));
	
	setFillDisplayMode(Alarm);
	setLineDisplayMode(Static);
	setGeometryType(Ellipse);
	setFillMode(Solid);
	setFillColor(QColor("orange"));
	setVisible(true);
	setPopup(false);
	setBlinkValue(-1);
	//update();
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(" was initialized. ")));
}

void CAGraphic::changeValue (const short &connstatus, const double &value, const short &severity)
{
	if(d_data->first==1)
	{
		d_data->first==0;
	}

//qDebug("CAGrahpic::changeValue ***1");
	d_data->m_val = value;
	CAGraphic::Severity almseverity = (CAGraphic::Severity) severity;
	

	switch(fillDisplayMode())
	{
		case Static:
			setFillColor(fillColor0());
			break;
		case Alarm:
			if (connstatus == -1) 
			{
				almseverity = AlarmNsev;
			}
			else if (connstatus == ECA_DISCONN)
			{
				almseverity = AlarmNsev;
			}
			switch ((CAGraphic::Severity) almseverity)
			{
				case NoAlarm:
					setFillColor(QColor(0,255,0));
					break;
				case MinorAlarm:
					//setFillColor(QColor("yellow"));
					setFillColor(getMinorAColor());
					break;
				case MajorAlarm:
					//setFillColor(QColor("red"));
					setFillColor(getMajorAColor());
					break;
				case InvalidAlarm:
					setFillColor(QColor("gray"));
					break;
				case AlarmNsev:
					setFillColor(QColor("white"));
					break;
				default:
					setFillColor(QColor("orange"));
					break;
			}
			break;
		case IfNotZero:
			if(d_data->m_val)
			{
				setFillColor(fillColor0());
			}
			else
			{
				setFillColor(QColor(255,255,255,0));
			}
			break;
		case IfZero:
			if(!d_data->m_val)
			{
				setFillColor(fillColor0());
			}
			else
			{
				setFillColor(QColor(255,255,255,0));
			}
			break;
		case ActInact:
			if(d_data->m_val > 0)
			{
				setFillColor(QColor(0,255,0));
			}
			else
			{
				setFillColor(QColor("gray"));
			}
			break;
		case Blink:
			if(getBlinkValue()== -1 && d_data->m_val == 1)
			{
				startBlink();
			}
			else if(getBlinkValue() == (int)d_data->m_val)
			{
				startBlink();
			} else
			{
				stopBlink();
			}
			break;
		default:
			break;
	}

	switch(lineDisplayMode())
	{
		case Static:
			setLineColor(lineColor0());
			break;
		case Alarm:
			switch ((CAGraphic::Severity) almseverity)
			{
				case NoAlarm:
					setLineColor(QColor(0,255,0));
					break;
				case MinorAlarm:
					setLineColor(QColor("yellow"));
					break;
				case MajorAlarm:
					setLineColor(QColor("red"));
					break;
				case InvalidAlarm:
					setLineColor(QColor("gray"));
					break;
				case AlarmNsev:
					setLineColor(QColor("white"));
					break;
				default:
					break;
			}
			break;
		case IfNotZero:
			if(d_data->m_val)
			{
				setLineColor(lineColor0());
			}
			else
			{
				setLineColor(QColor(255,255,255,0));
			}
			break;
		case IfZero:
			if(!d_data->m_val)
			{
				setLineColor(lineColor0());
			}
			else
			{
				setLineColor(QColor(255,255,255,0));
			}
			break;
		case ActInact:
			if(d_data->m_val > 0)
			{
				setLineColor(QColor(0,255,0));
			}
			else
			{
				setLineColor(QColor("gray"));
			}
			break;
		case Blink:
			setLineColor(lineColor0());
			break;
		default:
			break;
	}

	if (connstatus != ECA_CONN)
	{
		almseverity = AlarmNsev;
		setFillColor(QColor("white"));
		setLineColor(QColor("gray"));
		emit changedValue(value);
		emit changedValue(false);
		//qDebug("CAGrahpic::Error connstatus != ECA_CONN, Number:%d", connstatus);
		return; 
	}

	emit changedValue(value);
	emit changedValue((bool)value);

	if ((bool)value == 1) {
		emit isTrue();
	} else if ((bool)value == 0) {
		emit isFalse();
	}

	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: "))
			.append(QString("%1").arg(d_data->m_val,1,'g',3)).append(QString(", SEV: ")).append("%1").arg((int)almseverity, 1)
			.append(QString(", ECA_STATUS: ")).append("%1").arg((int)connstatus, 1));
	update();
	//paint();
//qDebug("CAGrahpic::changeValue ***2");
}

void CAGraphic::changePvName (const QString &pvname)
{
    setPvname(pvname);
}
