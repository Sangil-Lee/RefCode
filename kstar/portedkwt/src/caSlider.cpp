/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caSlider.h"


class CASlider::PrivateData
{
public:
    PrivateData(): m_bcontrol(true),m_bimmediate(true), m_btracking(false)
    {
    };
	bool m_bcontrol;
    bool m_bimmediate;
	bool m_btracking;
};

CASlider::CASlider(QWidget *parent):QSlider(Qt::Horizontal, parent)
{
    initSlider();
}

CASlider::~CASlider()
{
	delete d_data;
}

void CASlider::wheelEvent(QWheelEvent *e)
{
}

const bool CASlider::getImmediate() const
{
	return d_data->m_bimmediate;
}
void  CASlider::setImmediate(const bool immediate)
{
	d_data->m_bimmediate = immediate;
}

const QString &CASlider::getPvname() const
{
	return m_pvname;
}

void CASlider::setPvname(const QString &name)
{
    m_pvname = name;
	//setStatusTip(QString("PVname : ").append(m_pvname));
}

const bool CASlider::getControl() const
{
	return d_data->m_bcontrol;
}

void CASlider::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}
const bool CASlider::getTRacking() const
{
    return d_data->m_btracking;
}

void  CASlider::setTRacking(const bool tracking)
{
	d_data->m_btracking = tracking;
	setTracking(d_data->m_btracking);
}

QSize CASlider::sizeHint() const
{
	return minimumSizeHint();
}

QSize CASlider::minimumSizeHint() const
{
	int mw = 75;
	int mh = 10;
	QSize sz;
	sz += QSize(mw, mh);
	return sz;
}

void CASlider::initSlider()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
    d_data = new PrivateData;
	setControl(true);

	connect(this, SIGNAL(valueChanged(int)), this, SLOT(casliderSlot(int)));
}

void CASlider::casliderSlot(int val)
{
	qDebug("CASlider Value:%d", val);
	QString strvalue = QString("%1").arg(val);
	emit TrackingValue(strvalue);
}

void CASlider::changeValue (const short &connstatus, const double &value, const short& severity, const short& precision )
{
	if (connstatus != ECA_CONN) return;

	setValue(value);
	update();

	short almsever = severity;
	setStatusTip(QString("PVname : ").append(getPvname()).append(QString(", Value: "))
			.append(QString("%1").arg(value,1,'g',3)).append(QString(", SEV: ")).append("%1").arg((int)almsever, 1)
			.append(QString(", ECA_STATUS: ")).append("%1").arg((int)connstatus, 1));
}

void CASlider::SetMinimum(const int minvalue)
{
	setMinimum(minvalue);
}

void CASlider::SetMaximum(const int maxvalue)
{
	setMaximum(maxvalue);
}
