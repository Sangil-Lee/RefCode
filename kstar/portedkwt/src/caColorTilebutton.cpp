/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caColorTilebutton.h"


class CAColorTileButton::PrivateData
{
public:
    PrivateData()
    {
    };
	double mValue;
	QString m_pvname;
	QPalette mPalette;
};

CAColorTileButton::CAColorTileButton(QWidget *parent):QPushButton(parent)
{
    initPushButton();
}

CAColorTileButton::~CAColorTileButton()
{
    delete d_data;
}

QSize CAColorTileButton::sizeHint() const
{
	//return minimumSizeHint();
	int mw = 50;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

QSize CAColorTileButton::minimumSizeHint() const
{
	int mw = 10;
	int mh = 10;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CAColorTileButton::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAColorTileButton::getPvname() const
{
	return d_data->m_pvname;
}

void CAColorTileButton::initPushButton()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

	setIconSize(QSize(100,100));
    setObjectName(QString::fromUtf8("CAColorTileButton"));
	setPvname("pvname");
}

const bool &CAColorTileButton::getValue() const
{
	return d_data->mValue;
}

void CAColorTileButton::GetColorCode(const QString pvname, const QString color)
{
	if(getPvname().compare(pvname) !=0 ) return;

	//qDebug("Color Code:[%s]", color.toStdString().c_str());
	//Display Tile Color.

	QColor selCol(color);
	QBrush brush(selCol);
	QPalette palette;
	palette.setBrush(QPalette::Active, QPalette::Button, brush);
	palette.setBrush(QPalette::Inactive, QPalette::Button, brush);
	setPalette(palette);
}
void CAColorTileButton::changeValue (const short &connstatus, const double &value)
{
	//emit currentStatus((bool)value);
	d_data->mValue = value;
	emit GetColorFromColorMapBar(getPvname(), value);

	if(connstatus != ECA_CONN)
	{
		setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", connstatus == -1 ")));
	};
	
	//setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value,0,10)));
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value)));
}
