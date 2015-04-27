/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caBobutton.h"
#include "caDisplayer.h"


class CABoButton::PrivateData
{
public:
    PrivateData():
		m_truelabel(QString("True")),
		m_falselabel(QString("False")),
		m_bcontrol(true),
		m_bconfirm(false),
		m_passwd(false),
		m_estop(false),
		m_passwdstr(""),
		selColor(QColor(180,180,180,200)),
		unselColor(QColor(220,220,220,200)),
		mtimeset(0),
		mvalue(true),
		mtimevalue(true)
    {
    };

	QString m_pvname;
	//DisplayMode dmode;
	QString m_truelabel;
	QString m_falselabel;
	bool	m_bcontrol;
	bool	m_bconfirm;
	AlignPolicy m_align;
    //QPalette palette;
	QPushButton *m_truebutton;
	QPushButton *m_falsebutton;
	QColor selColor;
	QColor unselColor;
	QPalette selPalette;
	QPalette unselPalette;
	bool	m_passwd;
	bool	m_estop;
	QString m_passwdstr;
	bool m_status;
	QStringList m_pvlist;
	QStringList m_objlist;
	TrueOrFalse mtruefalse;
	int mtimeset;
	bool mvalue;
	bool mtimevalue;
};

CABoButton::CABoButton(QWidget *parent):QWidget(parent)
{
    initBoButton();
}

CABoButton::~CABoButton()
{
    delete d_data;
}

QSize CABoButton::sizeHint() const
{
	return minimumSizeHint();
}

QSize CABoButton::minimumSizeHint() const
{
	int mw = 80;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CABoButton::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
}

void CABoButton::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st = false;
	else st = true;
	setEnabled(st);
}

const QStringList CABoButton::getPVList() const
{
	return d_data->m_pvlist;
}

void CABoButton::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}

const QStringList CABoButton::getObjectList() const
{
	return d_data->m_objlist;
}
void CABoButton::setObjectList(const QStringList objlist)
{
	d_data->m_objlist=objlist;
}
void CABoButton::setPvname(const QString &name)
{
    d_data->m_pvname = name;
 	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CABoButton::getPvname() const
{
	return d_data->m_pvname;
}


void CABoButton::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}

const bool &CABoButton::getControl() const
{
	return d_data->m_bcontrol;
}

void CABoButton::setConfirm(const bool confirm)
{
    d_data->m_bconfirm = confirm;
}

const bool &CABoButton::getConfirm() const
{
	return d_data->m_bconfirm;
}

void CABoButton::setPasswd(const bool passwd)
{
    d_data->m_passwd = passwd;
}

const bool &CABoButton::getPasswd() const
{
	return d_data->m_passwd;
}

//++leesi
const bool &CABoButton::getEStop() const
{
	return d_data->m_estop;
}

void CABoButton::setEStop(const bool estop)
{
    d_data->m_estop = estop;
}

void CABoButton::setPasswdStr(const QString &passwdstr)
{
    d_data->m_passwdstr = passwdstr;
}

const QString &CABoButton::getPasswdStr() const
{
	return d_data->m_passwdstr;
}

CABoButton::AlignPolicy CABoButton::getAlignPolicy() const
{
	    return d_data->m_align;
}

void CABoButton::setAlignPolicy(AlignPolicy align)
{
	    d_data->m_align = align;
		init2BoButton();
}

#if 0
CABoButton::DisplayMode CABoButton::getDisplaymode() const
{
	    return d_data->dmode;
}

void CABoButton::setDisplaymode(DisplayMode dmode)
{
	    d_data->dmode = dmode;
}
#endif

void CABoButton::setTruelabel(const QString &truelabel)
{
    d_data->m_truelabel = truelabel;
	d_data->m_truebutton->setText(d_data->m_truelabel);
	
}

const QString &CABoButton::getTruelabel() const
{
	return d_data->m_truelabel;
}

void CABoButton::setFalselabel(const QString &falselabel)
{
    d_data->m_falselabel = falselabel;
	d_data->m_falsebutton->setText(d_data->m_falselabel);
}

const QString &CABoButton::getFalselabel() const
{
	return d_data->m_falselabel;
}

const QColor& CABoButton::getSelColor() const
{
	return d_data->selPalette.color(QPalette::Active, QPalette::Button);
}

void CABoButton::setSelColor(const QColor &selCol)
{
	QBrush brush(selCol);
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, brush);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
}

const QColor& CABoButton::getUnselColor() const
{
	return d_data->unselPalette.color(QPalette::Active, QPalette::Button);
}

void CABoButton::setUnselColor(const QColor &unselCol)
{
	QBrush brush(unselCol);
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, brush);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
}

void CABoButton::changePvName (const QString &pvname)
{
    setPvname(pvname);
}

void CABoButton::initBoButton()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif

	d_data = new PrivateData; 

	d_data->m_falsebutton = new QPushButton();
	d_data->m_falsebutton->setObjectName(QString::fromUtf8("falseButton"));
	d_data->m_truebutton = new QPushButton();
	d_data->m_truebutton->setObjectName(QString::fromUtf8("trueButton"));
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, d_data->selColor);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->selColor.dark());
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, d_data->unselColor);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->unselColor.dark());

	setFalselabel(d_data->m_falselabel);
	setTruelabel(d_data->m_truelabel);
	setPvname("pvname");
	//setAlignPolicy(Column);
	//d_data->m_falsebutton->setDown(true);

#if 0
	QBrush brush_select(d_data->selColor);
	QBrush brush_normal(d_data->unselColor);
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, brush_select);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, brush_select);
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, brush_normal);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, brush_normal);
#endif

#if 0
	QHBoxLayout *vhbox = new QHBoxLayout(this);
	vhbox->setSpacing(0);
	vhbox->setMargin(0);
	vhbox->setObjectName(QString::fromUtf8("vhbox"));

	vhbox->addWidget(d_data->m_falsebutton);
	vhbox->addWidget(d_data->m_truebutton);

	setLayout(vhbox);
#endif

	connect(d_data->m_falsebutton, SIGNAL(clicked(bool)), this,  SLOT(falsepressed(bool)));
	connect(d_data->m_truebutton, SIGNAL(clicked(bool)), this, SLOT(truepressed(bool)));
}

void CABoButton::init2BoButton()
{
	switch(getAlignPolicy())
	{
		case Row: 
			{
				QVBoxLayout *vbox = new QVBoxLayout(this);
				vbox->setSpacing(0);
				vbox->setMargin(0);
				vbox->setObjectName(QString::fromUtf8("vbox"));

				vbox->addWidget(d_data->m_falsebutton);
				vbox->addWidget(d_data->m_truebutton);

				setLayout(vbox);
			}
			break;
			//case Column:
		default:
			{
				QHBoxLayout *hbox = new QHBoxLayout(this);
				hbox->setSpacing(0);
				hbox->setMargin(0);
				hbox->setObjectName(QString::fromUtf8("hbox"));

				hbox->addWidget(d_data->m_falsebutton);
				hbox->addWidget(d_data->m_truebutton);

				setLayout(hbox);
			}
			break;
	}
		d_data->m_truebutton->setPalette(d_data->unselPalette);
		d_data->m_truebutton->setCursor(Qt::PointingHandCursor);
		d_data->m_falsebutton->setPalette(d_data->unselPalette);
		d_data->m_falsebutton->setCursor(Qt::PointingHandCursor);
}

void CABoButton::changeValue (const short &connstatus, const double &value)
{
//	qDebug("CABoButton::changeValue 1");
	if((bool)value) 
	{
		d_data->m_truebutton->setDown(true);
		d_data->m_falsebutton->setDown(false);
		d_data->m_truebutton->setPalette(d_data->selPalette);
		d_data->m_falsebutton->setPalette(d_data->unselPalette);
		emit CABoButton::isTrue(true);
		emit CABoButton::isFalse(false);
	} 
	else if(!(bool)value)
	{
		d_data->m_falsebutton->setDown(true);
		d_data->m_truebutton->setDown(false);
		d_data->m_falsebutton->setPalette(d_data->selPalette);
		d_data->m_truebutton->setPalette(d_data->unselPalette);
		emit CABoButton::isTrue(false);
		emit CABoButton::isFalse(true);
	}

	if (connstatus != ECA_CONN)
	{
		d_data->m_truebutton->setPalette(d_data->unselPalette);
		d_data->m_falsebutton->setPalette(d_data->unselPalette);
		setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", connstatus == -1 ")));
	} 
	else
	{
		setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value,1,'g',3)));
	}
	//emit valueChanged((bool)value);
	d_data->mvalue = (bool)value;
	//qDebug("CABoButton-Value: %d", (bool)value);

//	qDebug("CABoButton::changeValue 2");
}

void CABoButton::falsepressed (const bool status)
{
	d_data->m_status = status;
	d_data->m_falsebutton->setDown(true);
	d_data->m_truebutton->setDown(false);
	//d_data->m_falsebutton->setPalette(d_data->selPalette);
	//d_data->m_truebutton->setPalette(d_data->unselPalette);
	//qDebug("false button is clicked");
	//emit CABoButton::isTrue(false);
	emit valueChanged(false);
	if(d_data->mtimeset != 0 && getBoolValue() == CABoButton::False)
	{
		d_data->mtimevalue = true;
		QTimer::singleShot(d_data->mtimeset*1000, this, SLOT(timerCallback()));
	};
}

void CABoButton::truepressed (const bool status)
{
	d_data->m_status = status;
	d_data->m_truebutton->setDown(true);
	d_data->m_falsebutton->setDown(false);
	//d_data->m_truebutton->setPalette(d_data->selPalette);
	//d_data->m_falsebutton->setPalette(d_data->unselPalette);
	//qDebug("true button is clicked");
	//emit CABoButton::isTrue(true);
	emit valueChanged(true);
	if(d_data->mtimeset != 0 && getBoolValue() == CABoButton::True)
	{
		d_data->mtimevalue = false;
		QTimer::singleShot(d_data->mtimeset*1000, this, SLOT(timerCallback()));
	}
}

void CABoButton::timerCallback ()
{
	qDebug("timerCallback:%d", d_data->mtimevalue);
	emit valueChanged(d_data->mtimevalue);
}

const int CABoButton::getTimeSet () const
{
	return d_data->mtimeset;
}

void CABoButton::setTimeSet(const int timeset) 
{
	d_data->mtimeset = timeset;
}

CABoButton::TrueOrFalse CABoButton::getBoolValue() const
{
	return d_data->mtruefalse;
}

void CABoButton::setBoolValue(TrueOrFalse boolval)
{
	d_data->mtruefalse = boolval;
}

void CABoButton::print(const bool status)
{
//	qDebug("CABoButton::print");
}
void CABoButton::enterEvent(QEvent *event)
{
	if(!parent()) return;

	if(d_data->m_objlist.isEmpty() == false)
	{    
		for(int i = 0; i<d_data->m_objlist.count();i++)
		{    
			QString objname = d_data->m_objlist.at(i);
			//qDebug("ObjectName:%s", objname.toStdString().c_str());
			CADisplayer *pDisp = parent()->findChild<CADisplayer *>(objname);
			if(!pDisp) continue;
			emit RelatedCADisplayer(1, objname);
			//connect(this, SIGNAL(), pDisp, SLOT());
		};   
	};   
}

void CABoButton::leaveEvent(QEvent *event)
{
	if(!parent()) return;

	if(d_data->m_objlist.isEmpty() == false)
	{    
		for(int i = 0; i<d_data->m_objlist.count();i++)
		{    
			QString objname = d_data->m_objlist.at(i);
			CADisplayer *pDisp = parent()->findChild<CADisplayer *>(objname);
			if(!pDisp) continue;
			emit RelatedCADisplayer(0, objname);
		};   
	};   
}
const bool &CABoButton::getValue() const
{
	return d_data->mvalue;
}
