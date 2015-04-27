/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caComboBox.h"


class CAComboBox::PrivateData
{
public:
    PrivateData():
		m_bcontrol(true)
    {
    };

	QString m_pvname;
	bool	m_bcontrol;
	QStringList m_pvlist;
};

CAComboBox::CAComboBox(QWidget *parent):QComboBox(parent)
{
    initComboBox();
}

CAComboBox::~CAComboBox()
{
    delete d_data;
}

QSize CAComboBox::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAComboBox::minimumSizeHint() const
{
	int mw = 80;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const QStringList CAComboBox::getPVList() const
{
	return d_data->m_pvlist;
}

void CAComboBox::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}

void CAComboBox::setPvname(const QString &name)
{
    d_data->m_pvname = name;
 	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAComboBox::getPvname() const
{
	return d_data->m_pvname;
}

void CAComboBox::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}

const bool &CAComboBox::getControl() const
{
	return d_data->m_bcontrol;
}

void CAComboBox::changePvName (const QString &pvname)
{
    setPvname(pvname);
}

void CAComboBox::initComboBox()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif

	d_data = new PrivateData; 

	setPvname("pvname");

}

void CAComboBox::changeValue (const short &connstatus, const int &value)
{
	int intval=value;
	
	setCurrentIndex(intval);
	if (connstatus != ECA_CONN)
	{
		setStatusTip("Disconnected");
	}
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(intval)));
}
void CAComboBox::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
}
void CAComboBox::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st= false;
	else st = true;
	setEnabled(st);
}
