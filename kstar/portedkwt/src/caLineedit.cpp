/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caLineedit.h"

#define verticalMargin 0
#define horizontalMargin 0


class CALineEdit::PrivateData
{
public:
    PrivateData():
		m_bcontrol(true), m_dbrequest(DBRGDBL),m_bimmediate(true)
    {
    };

    bool m_bcontrol;
	QPalette p;
	NumeticFormat nformat;
	DBREQUEST m_dbrequest;
    bool m_bimmediate;
	QStringList m_pvlist;
};

CALineEdit::CALineEdit(QWidget *parent):QLineEdit(parent)
{
    initLineEdit();
}

CALineEdit::~CALineEdit()
{
	delete d_data;
}

QSize CALineEdit::sizeHint() const
{
	return minimumSizeHint();
}

QSize CALineEdit::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const bool &CALineEdit::getControl() const
{
	return d_data->m_bcontrol;
}

void CALineEdit::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}

const bool CALineEdit::getImmediate() const
{
	return d_data->m_bimmediate;
}
void  CALineEdit::setImmediate(const bool immediate)
{
	d_data->m_bimmediate = immediate;
}

void CALineEdit::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
}

void CALineEdit::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st = false;
	else st = true;
	setEnabled(st);
}

const QString &CALineEdit::getPvname() const
{
	return m_pvname;
}

void CALineEdit::setPvname(const QString &name)
{
    m_pvname = name;
    setText(m_pvname);
	
	//************************************//
	//QString m_pvnamel;
	//m_pvnamel = m_pvname;
	//setText(m_pvname.remove(getPrefix()));
	//setText(m_pvname.replace(QString("_"), QString(".")));
	//************************************//
	setStatusTip(QString("PVname : ").append(m_pvname));
}

const QString &CALineEdit::getPrefix() const
{
	    return m_prefix;
}

void CALineEdit::setPrefix(const QString &prefix)
{
	    QString m_pvnamel = m_pvname;
		    m_prefix = prefix;
			        setPvname(m_pvname);
}

const QString& CALineEdit::getUnit() const
{
	return m_unit;
}

void  CALineEdit::setUnit(const QString &unit)
{
	m_unit = unit;
	update();
}

CALineEdit::NumeticFormat CALineEdit::getNuFormat() const
{
		return (NumeticFormat) d_data->nformat;
}

void CALineEdit::setNuFormat(CALineEdit::NumeticFormat format)
{
		d_data->nformat = format;
}

const QStringList CALineEdit::getPVList() const
{
	return d_data->m_pvlist;
}

void CALineEdit::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}

CALineEdit::DBREQUEST CALineEdit::getDBRequest() const
{
	return d_data->m_dbrequest;
}
void CALineEdit::setDBRequest(DBREQUEST dbrequest)
{
	d_data->m_dbrequest = dbrequest;
}

void CALineEdit::initLineEdit()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif

    d_data = new PrivateData;
	setControl(d_data->m_bcontrol);
	setNuFormat(RealOne);

	//**************************//
	QFont f0("Sans Serif", 7);
	f0.setBold(false);
	setFont(f0);
	//**************************//

	setText(QString("-1.0 ").append(getUnit()));
	setFrame(true);
	setAlignment(Qt::AlignRight);
	d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("black"));
	d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("black"));
	setPalette(d_data->p);


	connect(this, SIGNAL(returnPressed()), this, SLOT(valueEntered()));
	//connect(this, SIGNAL(editingFinished()), this, SLOT(valueEntered()));
	//connect(this, SIGNAL(textChanged(QString)), this, SLOT(valueEntered(QString)));

}
void CALineEdit::changeValue (const short &connstatus, const double &value, const short& severity, const short& precision )
{
	QString sval;
	if (connstatus != ECA_CONN)
	{
		setText(QString("Disconnected"));
		emit changedValue(value);
		update();
		return;

	} else {
		if (getNuFormat() == Integer)
		{
			sval = QString("%1").arg(value, 1, 'f', 0);
		} 
		else if (getNuFormat() == Eng)
		{
			sval = QString("%1").arg(value, 1, 'e', 2);
		}
		else if (getNuFormat() == RealOne)
		{
			sval = QString("%1").arg(value, 1, 'f', 1);
		}
		else if (getNuFormat() == RealTwo)
		{
			sval = QString("%1").arg(value, 1, 'f', 2);
		}
		else if (getNuFormat() == RealThree)
		{
			sval = QString("%1").arg(value, 1, 'f', 3);
		}
		else if (getNuFormat() == RealFour)
		{
			sval = QString("%1").arg(value, 1, 'f', 4);
		}
		else if (getNuFormat() == RealFive)
		{
			sval = QString("%1").arg(value, 1, 'f', 5);
		}
		else if (getNuFormat() == RealSix)
		{
			sval = QString("%1").arg(value, 1, 'f', 6);
		}
		else if (getNuFormat() == RealSeven)
		{
			sval = QString("%1").arg(value, 1, 'f', 7);
		}
		else if (getNuFormat() == String)
		{
		}
	
		if (getNuFormat() != String && getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	}

#if 1
	short almsever = severity;
	if (connstatus == -1 ) 
	{
		almsever = ALARM_NSEV;
		sval = "R";
	}
	else if (connstatus == ECA_DISCONN ) 
	{
		almsever = INVALID_ALARM;
		sval = "DisconnCh";
	};
#endif

	//**************************//
	//setPrefix(QString("CLS."));
	//sval = getPvname();
	//**************************//
	//QFont f0("Sans Serif", 7);
	//f0.setBold(false);
	//setFont(f0);
    setText(sval);

#if 1
	switch(almsever)
	{
		case MAJOR_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("red"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("red"));
			setPalette(d_data->p);
			break;
		case MINOR_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("orange"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("orange"));
			setPalette(d_data->p);
			break;
		case INVALID_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("gray"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("gray"));
			setPalette(d_data->p);
			break;
		case ALARM_NSEV:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("black"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("black"));
			setPalette(d_data->p);
			break;
		case NO_ALARM:
		default:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor(0,150,0));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor(0,150,0));
			setPalette(d_data->p);
			break;
	};
#endif


	emit changedValue(value);
	update();
	setStatusTip(QString("PVname : ").append(getPvname()).append(QString(", Value: ")).append(QString("%1 ").arg(value,1,'f',3)).append(sval));
}

void CALineEdit::changeValue (const short &connstatus, const QString &value, const short& severity, const short& precision )
{
	QString sval = value;
	double number = value.toDouble();
	short almsever = severity;

	if (connstatus == -1)
	{
		almsever = ALARM_NSEV;
		sval = "R";
	}
	else if (connstatus != ECA_CONN)
	{
		almsever = INVALID_ALARM;
		sval = "DisconnCh";
		return;

	} else {
		sval = value;
	}

	//**************************//
	//setPrefix(QString("CLS."));
	//sval = getPvname();
	//**************************//
	//QFont f0("Sans Serif", 7);
	//f0.setBold(false);
	//setFont(f0);
    setText(sval);

#if 1
	switch(almsever)
	{
		case MAJOR_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("red"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("red"));
			setPalette(d_data->p);
			break;
		case MINOR_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("orange"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("orange"));
			setPalette(d_data->p);
			break;
		case INVALID_ALARM:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("gray"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("gray"));
			setPalette(d_data->p);
			break;
		case ALARM_NSEV:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor("black"));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor("black"));
			setPalette(d_data->p);
			break;
		case NO_ALARM:
		default:
			d_data->p.setBrush(QPalette::Active, QPalette::Text, QColor(0,150,0));
			d_data->p.setBrush(QPalette::Inactive, QPalette::Text, QColor(0,150,0));
			setPalette(d_data->p);
			break;
	};
#endif


	emit changedValue(number);
	update();
	setStatusTip(QString("PVname : ").append(getPvname()).append(QString(", Value: ")).append(sval));
}

#if 0
void CALineEdit::changePvName (const QString &pvname)
{
    d_data->pvName->setText(pvname);
    d_data->valueEdit->setText(pvname);
}
void CALineEdit::changePvUnit (const QString &pvunit)
{
    //d_data->pvUnit->setText(pvunit);
}
#endif

void CALineEdit::externalEntered()
{
	bool immediate = getImmediate();
	setImmediate(true);
	valueEntered();
	setImmediate(immediate);
}
void CALineEdit::valueEntered()
{
	double val = 0.0;
	QString sval = "";
	if(getImmediate()==false) return;
	switch (getDBRequest())
	{
		case DBRGSTR:
			sval = text();
			emit enteredValue(sval);
			break;
		case DBRGDBL:
			val = text().toDouble();
			emit enteredValue(val);
			break;
		default:
			break;
	}
}
