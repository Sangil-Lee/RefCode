/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caProgressBar.h"


class CAProgressBar::PrivateData
{
public:
    PrivateData()
    {};
};

CAProgressBar::CAProgressBar(QWidget *parent):QProgressBar(parent)
{
    initProgressBar();
}

CAProgressBar::~CAProgressBar()
{
	delete d_data;
}

const QString &CAProgressBar::getPvname() const
{
	return m_pvname;
}

void CAProgressBar::setPvname(const QString &name)
{
    m_pvname = name;
	setStatusTip(QString("PVname : ").append(m_pvname));
}

QSize CAProgressBar::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAProgressBar::minimumSizeHint() const
{
	int mw = 75;
	int mh = 10;
	QSize sz;
	sz += QSize(mw, mh);
	return sz;
}

void CAProgressBar::initProgressBar()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
    d_data = new PrivateData;
}
void CAProgressBar::changeValue (const short &connstatus, const double &value, const short& severity, const short& precision )
{
	if (connstatus != ECA_CONN) return;

	//emit valueChanged(value);
	setValue(value);
	update();
	short almsever = severity;

	setStatusTip(QString("PVname : ").append(getPvname()).append(QString(", Value: "))
			.append(QString("%1").arg(value,1,'g',3)).append(QString(", SEV: ")).append("%1").arg((int)almsever, 1)
			.append(QString(", ECA_STATUS: ")).append("%1").arg((int)connstatus, 1));
}

