/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caImageMbbi.h"


class CAImageMbbi::PrivateData
{
public:
    PrivateData():
		index(0)
    {
    };

	QString pvname;
	DisplayMode dmode;
	QPixmap pixmap[16];
	QPixmap truepixmap;
	QPixmap falsepixmap;
	int index;

	QLabel *label;

};

CAImageMbbi::CAImageMbbi(QWidget *parent):QWidget(parent)
{
    initCAImageMbbi();
}

CAImageMbbi::~CAImageMbbi()
{
    delete d_data;
}

QSize CAImageMbbi::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAImageMbbi::minimumSizeHint() const
{
	int mw = 120;
	int mh = 100;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const QString &CAImageMbbi::getPvname() const
{
	return d_data->pvname;
}

void CAImageMbbi::setPvname(const QString &name)
{
    d_data->pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->pvname));
}

CAImageMbbi::DisplayMode CAImageMbbi::getDisplaymode() const
{
	return d_data->dmode;
}

void CAImageMbbi::setDisplaymode(DisplayMode dmode)
{
	d_data->dmode = dmode;
}

const QPixmap &CAImageMbbi::getZrpixmap() const
{
	return d_data->pixmap[0];
}

void CAImageMbbi::setZrpixmap(const QPixmap &pxm)
{
	d_data->pixmap[0]= pxm;
}

const QPixmap &CAImageMbbi::getOnpixmap() const
{
	return d_data->pixmap[1];
}

void CAImageMbbi::setOnpixmap(const QPixmap &pxm)
{
	d_data->pixmap[1]= pxm;
}

const QPixmap &CAImageMbbi::getTwpixmap() const
{
	return d_data->pixmap[2];
}

void CAImageMbbi::setTwpixmap(const QPixmap &pxm)
{
	d_data->pixmap[2]= pxm;
}

const QPixmap &CAImageMbbi::getThpixmap() const
{
	return d_data->pixmap[3];
}

void CAImageMbbi::setThpixmap(const QPixmap &pxm)
{
	d_data->pixmap[3]= pxm;
}

const QPixmap &CAImageMbbi::getFrpixmap() const
{
	return d_data->pixmap[4];
}

void CAImageMbbi::setFrpixmap(const QPixmap &pxm)
{
	d_data->pixmap[4]= pxm;
}

const QPixmap &CAImageMbbi::getFvpixmap() const
{
	return d_data->pixmap[5];
}

void CAImageMbbi::setFvpixmap(const QPixmap &pxm)
{
	d_data->pixmap[5]= pxm;
}

const QPixmap &CAImageMbbi::getSxpixmap() const
{
	return d_data->pixmap[6];
}

void CAImageMbbi::setSxpixmap(const QPixmap &pxm)
{
	d_data->pixmap[6]= pxm;
}

const QPixmap &CAImageMbbi::getSvpixmap() const
{
	return d_data->pixmap[7];
}

void CAImageMbbi::setSvpixmap(const QPixmap &pxm)
{
	d_data->pixmap[7]= pxm;
}

const QPixmap &CAImageMbbi::getEipixmap() const
{
	return d_data->pixmap[8];
}

void CAImageMbbi::setEipixmap(const QPixmap &pxm)
{
	d_data->pixmap[8]= pxm;
}

const QPixmap &CAImageMbbi::getNipixmap() const
{
	return d_data->pixmap[9];
}

void CAImageMbbi::setNipixmap(const QPixmap &pxm)
{
	d_data->pixmap[9]= pxm;
}

const QPixmap &CAImageMbbi::getTepixmap() const
{
	return d_data->pixmap[10];
}

void CAImageMbbi::setTepixmap(const QPixmap &pxm)
{
	d_data->pixmap[10]= pxm;
}

const QPixmap &CAImageMbbi::getElpixmap() const
{
	return d_data->pixmap[11];
}

void CAImageMbbi::setElpixmap(const QPixmap &pxm)
{
	d_data->pixmap[11]= pxm;
}

const QPixmap &CAImageMbbi::getTvpixmap() const
{
	return d_data->pixmap[12];
}

void CAImageMbbi::setTvpixmap(const QPixmap &pxm)
{
	d_data->pixmap[12]= pxm;
}

const QPixmap &CAImageMbbi::getTtpixmap() const
{
	return d_data->pixmap[13];
}

void CAImageMbbi::setTtpixmap(const QPixmap &pxm)
{
	d_data->pixmap[13]= pxm;
}

const QPixmap &CAImageMbbi::getFtpixmap() const
{
	return d_data->pixmap[14];
}

void CAImageMbbi::setFtpixmap(const QPixmap &pxm)
{
	d_data->pixmap[14]= pxm;
}

const QPixmap &CAImageMbbi::getFfpixmap() const
{
	return d_data->pixmap[15];
}

void CAImageMbbi::setFfpixmap(const QPixmap &pxm)
{
	d_data->pixmap[15]= pxm;
}

const QPixmap &CAImageMbbi::getFalsepixmap() const
{
	return d_data->falsepixmap;
}

void CAImageMbbi::setFalsepixmap(const QPixmap &pxm)
{
	d_data->falsepixmap= pxm;
}
	  
void CAImageMbbi::setTruepxm(const QPixmap &truepxm)
{
	d_data->truepixmap = truepxm;
}
void CAImageMbbi::setFalsepxm(const QPixmap &falsepxm)
{
	d_data->falsepixmap = falsepxm;
}

const QPixmap &CAImageMbbi::getTruepixmap() const
{
	return d_data->truepixmap;
}

void CAImageMbbi::setTruepixmap(const QPixmap &pxm)
{
	d_data->truepixmap= pxm;
}

int CAImageMbbi::getIndex() const
{
	return d_data->index;
}

void CAImageMbbi::setIndex(int index)
{
	d_data->index= index;
}

void CAImageMbbi::initCAImageMbbi()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;
	setDisplaymode(Choice);
	setPvname("pvname");
	
	QVBoxLayout *vbox = new QVBoxLayout(this);
	vbox->setSpacing(0);
	vbox->setMargin(0);
	vbox->setObjectName(QString::fromUtf8("vboxLayout"));

	d_data->label = new QLabel(this);
	d_data->label->setObjectName(QString::fromUtf8("label"));
	d_data->label->setAlignment(Qt::AlignLeft);
	d_data->label->setText(getPvname());
	d_data->label->setScaledContents(true);
	d_data->label->setPixmap(QPixmap(":images/white.png"));

	vbox->addWidget(d_data->label); 
	setLayout(vbox);
}

void CAImageMbbi::changeValue (const short &connstatus, const int &value)
{
	if (connstatus != ECA_CONN)
	{
		d_data->label->setPixmap(QPixmap(":images/white.png"));
		setStatusTip(QString("PVname : ").append(d_data->pvname).append(QString(", connstatus: ")).append(QString("%1").arg(connstatus)));
		return;
	}
	else 
	{

		switch(getDisplaymode())
		{
			case Choice:
				d_data->label->setPixmap(d_data->pixmap[value]);
				break;
			case TrueFalse:
				if (getIndex()==value)
				{
					d_data->label->setPixmap(d_data->truepixmap);
				}
				else
				{
					d_data->label->setPixmap(d_data->falsepixmap);
				}
				break;
			default:
				break;
		}
		setStatusTip(QString("PVname : ").append(d_data->pvname).append(QString(", Value: ")).append(QString("%1").arg((double)value,1,'g',0)));
	}
}
