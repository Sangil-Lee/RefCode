/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caPushbutton.h"


class CAPushButton::PrivateData
{
public:
    PrivateData():
		m_bcontrol(true),
		m_bconfirm(false),
		m_estop(false),
		selColor(QColor(180,180,180,220)),
		unselColor(QColor(220,220,220,220)),
		selTColor(QColor(0,0,0,255)),
		unselTColor(QColor(0,0,0,255))
		//m_timerid(0),
		//m_pulsemsec(0)
    {
    };

	bool m_val;
	DisplayMode m_dmode;
	ButtonType m_btype;
	QString m_pvname;
	QString m_truelabel;
	QString m_falselabel;
	QIcon m_trueicon;
	QIcon m_falseicon;
	QColor selColor;
	QColor unselColor;
	QColor selTColor;
	QColor unselTColor;
	QPalette selPalette;
	QPalette unselPalette;
	bool	m_bcontrol;
	bool    m_bconfirm;
	bool	m_passwd;
	bool	m_estop;
	QString m_passwdstr;
	bool m_status;
	QStringList m_pvlist;
};

CAPushButton::CAPushButton(QWidget *parent):QPushButton(parent)
{
    initPushButton();
}

CAPushButton::~CAPushButton()
{
    delete d_data;
}

QSize CAPushButton::sizeHint() const
{
	//return minimumSizeHint();
	int mw = 50;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

QSize CAPushButton::minimumSizeHint() const
{
	int mw = 10;
	int mh = 10;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const QColor& CAPushButton::getSelColor() const
{
    return d_data->selPalette.color(QPalette::Active, QPalette::Button);
}

void CAPushButton::setSelColor(const QColor &selCol)
{
    QBrush brush(selCol);
    d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, brush);
    d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
	update();
}

const QColor& CAPushButton::getUnselColor() const
{
    return d_data->unselPalette.color(QPalette::Active, QPalette::Button);
}

void CAPushButton::setUnselColor(const QColor &unselCol)
{
    QBrush brush(unselCol);
    d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, brush);
    d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
	update();
}

const QColor& CAPushButton::getSelTColor() const
{
    return d_data->selPalette.color(QPalette::Active, QPalette::ButtonText);
}

void CAPushButton::setSelTColor(const QColor &selTCol)
{
    QBrush brush(selTCol);
    d_data->selPalette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    d_data->selPalette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
	update();
}


const QColor& CAPushButton::getUnselTColor() const
{
    return d_data->unselPalette.color(QPalette::Active, QPalette::ButtonText);
}

void CAPushButton::setUnselTColor(const QColor &unselTCol)
{
    QBrush brush(unselTCol);
    d_data->unselPalette.setBrush(QPalette::Active, QPalette::ButtonText, brush);
    d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush);
}

#if 0
const int CAPushButton:: getPulseSec() const
{
    return d_data->m_pulsemsec;
}
void CAPushButton:: setPulseSec(const int msec)
{
	d_data->m_pulsemsec = msec;
}
#endif

void CAPushButton::setStatus(const bool status)
{
	d_data->m_status = status;
}

void CAPushButton::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
	//qDebug("setTrueActivate was called. Value:%d\n", st);
}

void CAPushButton::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st= false;
	else st = true;
	setEnabled(st);
	//qDebug("setFalseActivate was called. Value:%d\n", st);
}

const QStringList CAPushButton::getPVList() const
{
	return d_data->m_pvlist;
}

void CAPushButton::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}
void CAPushButton::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAPushButton::getPvname() const
{
	return d_data->m_pvname;
}

CAPushButton::DisplayMode CAPushButton::displayMode() const
{
	return (CAPushButton::DisplayMode) d_data->m_dmode;
}

void CAPushButton::setDisplayMode(CAPushButton::DisplayMode dmode)
{
	d_data->m_dmode = dmode;
	init2PushButton();
}
				
CAPushButton::ButtonType CAPushButton::buttonType() const
{
	return (CAPushButton::ButtonType) d_data->m_btype;
}

void CAPushButton::setButtonType(CAPushButton::ButtonType btype)
{
	d_data->m_btype = btype;
	if(btype == ToggleButton)
	{
		setCheckable(true);
		setChecked(true);
		//qDebug("setCheckable: true, in setButtonType");
	} else if (btype == TrueButton)
	{
		setCheckable(false);
		setChecked(true);
		//qDebug("setCheckable: false, in setButtonType");
	} else if (btype == FalseButton)
	{
		setCheckable(false);
		setChecked(false);
		//qDebug("setCheckable: false, in setButtonType");
	}

	update();
}

#if 0
const bool &CAPushButton::getState() const
{
	return d_data->m_val;
}
void  
CAPushButton::setState(const bool state)
{
    d_data->m_val = state;
	setButtonType(buttonType());
	if(d_data->m_val == true)
		setTruelabel(getTruelabel());
	else
		setFalselabel(getFalselabel());
}
#endif
				
void CAPushButton::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}

const bool &CAPushButton::getControl() const
{
	return d_data->m_bcontrol;
}

void CAPushButton::setConfirm(const bool confirm)
{
    d_data->m_bconfirm = confirm;
}

const bool &CAPushButton::getConfirm() const
{
        return d_data->m_bconfirm;
}
//++leesi
const bool &CAPushButton::getEStop() const
{
	return d_data->m_estop;
}

void CAPushButton::setEStop(const bool estop)
{
    d_data->m_estop = estop;
}

void CAPushButton::setPasswd(const bool passwd)
{
    d_data->m_passwd = passwd;
}

const bool &CAPushButton::getPasswd() const
{
	return d_data->m_passwd;
}

void CAPushButton::setPasswdStr(const QString &passwdstr)
{
    d_data->m_passwdstr = passwdstr;
}

const QString &CAPushButton::getPasswdStr() const
{
	return d_data->m_passwdstr;
}

void CAPushButton::setTruelabel(const QString &truelabel)
{
    d_data->m_truelabel = truelabel;
	//setText(truelabel);
}

const QString &CAPushButton::getTruelabel() const
{
	return d_data->m_truelabel;
}

void CAPushButton::setFalselabel(const QString &falselabel)
{
    d_data->m_falselabel = falselabel;
	//setText(falselabel);
}

const QString &CAPushButton::getFalselabel() const
{
	return d_data->m_falselabel;
}

const QIcon CAPushButton::getTrueicon() const
{
	return d_data->m_trueicon;
}

void CAPushButton::setTrueicon(const QIcon &trueicon)
{
    d_data->m_trueicon = trueicon;
	setIcon(getTrueicon());
}

const QIcon CAPushButton::getFalseicon() const
{
	return d_data->m_falseicon;
}

void CAPushButton::setFalseicon(const QIcon &falseicon)
{
    d_data->m_falseicon = falseicon;
	setIcon(getFalseicon());
}

void CAPushButton::initPushButton()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

	setIconSize(QSize(100,100));
    setObjectName(QString::fromUtf8("CAPushButton"));
	setButtonType(ToggleButton);
	setPvname("pvname");
	setControl(true);
	setPasswd(false);
	setDisplayMode(TextOnly);
	
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, d_data->selColor);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->selColor.light());
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, d_data->unselColor);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->unselColor.light());

	QBrush brush_select(d_data->selColor);
	QBrush brush_normal(d_data->unselColor);
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, brush_select);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, brush_select);
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, brush_normal);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, brush_normal);

}

void CAPushButton::init2PushButton()
{
		switch(displayMode())
		{
			case IconOnly:
				setTrueicon(QIcon(":/images/valve_on.png"));
				setFalseicon(QIcon(":/images/valve_off.png"));
				if(buttonType()==TrueButton)
				{
					setIcon(getTrueicon());
				} else {
					setIcon(getFalseicon());
				}
				break;
			case TextOnly:
				//setTruelabel("True");
				//setFalselabel("False");
				if(buttonType()==TrueButton)
				{
					setText(getTruelabel());
				} else {
					setText(getFalselabel());
				}
				break;
		}

		setCursor(Qt::PointingHandCursor);
}

const bool &CAPushButton::getValue() const
{
	return d_data->m_val;
}

#if 0
void CAPushButton::PulseTime(const bool value)
{
	setEnabled(true);
	d_data->m_val = value;
	QTimer::singleShot(d_data->m_pulsemsec, this, SLOT(reSetValue()));
}

void CAPushButton::reSetValue()
{
	setEnabled(true);
	emit pressed();
}
#endif

void CAPushButton::changeValue (const short &connstatus, const int &value)
{
	setChecked((bool)value);
	d_data->m_val = (bool)value;
	emit currentStatus((bool)value);

	if((bool)value)
	{
		if(buttonType() == FalseButton)
		{
			setPalette(d_data->unselPalette);
		} else 
		{
			setPalette(d_data->selPalette);
		}

		switch(displayMode())
		{
			case IconOnly:
				setIcon(getFalseicon());
				break;
			case TextOnly:
	 			setText(getFalselabel());
				break;
		}
	} else {
		if(buttonType() == FalseButton)
		{
			setPalette(d_data->selPalette);
		} else 
		{
			setPalette(d_data->unselPalette);
		}

		switch(displayMode())
		{
			case IconOnly:
				setIcon(getTrueicon());
				break;
			case TextOnly:
	 			setText(getTruelabel());
				break;
		}
	}

	if(buttonType()==ToggleButton)
	{
		toggle();
	}

	if(connstatus != ECA_CONN)
	{
		setPalette(d_data->unselPalette);
		setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", connstatus == -1 ")));
	}
	

#if 0
	if(buttonType()==ToggleButton)
	{
		if((bool)value) 
		{
			switch(displayMode())
			{
				case IconOnly:
					setIcon(getFalseicon());
					break;
				case TextOnly:
		 			setText(getFalselabel());
					break;
			}
		} 
		else if(!(bool)value)
		{
			switch(displayMode())
			{
				case IconOnly:
					setIcon(getTrueicon());
					break;
				case TextOnly:
		 			setText(getTruelabel());
					break;
			}
		}
		toggle();
	} else if(buttonType() == TrueButton)
	{
		switch(displayMode())
		{
			case IconOnly:
				setIcon(getTrueicon());
				break;
			case TextOnly:
			//defalut:
		 		setText(getTruelabel());
				break;
		}

		if((bool)value)
		{
			setText(getTruelabel());
		} else {
			setText(getFalselabel());
		}

	} else if(buttonType() == FalseButton)
	{
		switch(displayMode())
		{
			case IconOnly:
				setIcon(getFalseicon());
				break;
			case TextOnly:
			//defalut:
		 		setText(getFalselabel());
				break;
		}
	}
#endif
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value,0,10)));
}

void CAPushButton::changePvName (const QString &pvname)
{
    setPvname(pvname);
}
