/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caDisplayer.h"

#define verticalMargin 0
#define horizontalMargin 0


class CADisplayer::PrivateData
{
public:
    PrivateData():editable(false),
		showlabel(false),
		framecol(QColor("black")),
		fillcol(QColor("orange")),
		lwid(2)
    {
    };

    bool editable;
    bool showlabel;
    QColor framecol;
    QColor fillcol;
    int lwid;
    QColor orgcol;
    int orglwidth;
    QFrame *frame;
    QLabel *pvName;
    QPalette framePalette;
    QPalette fillPalette;
    QLabel *valueEdit;
	NumeticFormat nformat;
	ePeriodic periodic;
    //QLineEdit *valueEdit;
    //QLabel *pvUnit;
};

CADisplayer::CADisplayer(QWidget *parent):QWidget(parent)
{
    initDisplayer();
}

QSize CADisplayer::sizeHint() const
{
	return minimumSizeHint();
}

QSize CADisplayer::minimumSizeHint() const
{
	int mw = 75;
	int mh = 24;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CADisplayer::setPvname(const QString &name)
{
    m_pvname = name;
    QString m_pvnamel;
    d_data->valueEdit->setText(m_pvname);
	//************************************//
	//m_pvnamel = m_pvname;
    //d_data->valueEdit->setText(m_pvname.remove(getPrefix()));
    //d_data->valueEdit->setText(m_pvname.replace(QString("_"), QString(".")));
	//************************************//
    d_data->valueEdit->setWindowTitle(m_pvname);
	setStatusTip(QString("PVname : ").append(m_pvname));

    if (d_data->showlabel) {
	m_pvnamel = name;
    d_data->pvName->setText(m_pvnamel.remove(getPrefix()));
    } else {
	m_pvnamel = "";
        //d_data->pvName->clear();
    }
}

const QString &CADisplayer::getPvname() const
{
	return m_pvname;
}

void CADisplayer::setPrefix(const QString &prefix)
{
    QString m_pvnamel = m_pvname;
    m_prefix = prefix;
        setPvname(m_pvname);
}

const QString &CADisplayer::getPrefix() const
{
	return m_prefix;
}

const QString& CADisplayer::getUnit() const
{
	return m_unit;
}

CADisplayer::NumeticFormat CADisplayer::getNuFormat() const
{
	return (NumeticFormat) d_data->nformat;
}

void CADisplayer::setNuFormat(CADisplayer::NumeticFormat format)
{
	d_data->nformat = format;
}

void  CADisplayer::setUnit(const QString &unit)
{
	m_unit = unit;
	//d_data->pvUnit->setText(unit);
	update();
}

void CADisplayer::initDisplayer()
{
    d_data = new PrivateData;
    m_prefix = "";

#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif

	setNuFormat(Temperature);

    QVBoxLayout *vboxLayout = new QVBoxLayout(this);
    vboxLayout->setSpacing(0);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));

    d_data->frame = new QFrame(this);
    d_data->frame->setObjectName(QString::fromUtf8("frame"));
    d_data->frame->setFrameShape(QFrame::Panel);
    d_data->frame->setFrameShadow(QFrame::Plain);
    d_data->frame->setLineWidth(d_data->lwid);
	setFrameColor(getFrameColor());

    QVBoxLayout *vfboxLayout = new QVBoxLayout(d_data->frame);
    vfboxLayout->setSpacing(0);
    vfboxLayout->setMargin(0);
    vfboxLayout->setObjectName(QString::fromUtf8("vfboxLayout"));

	//********* debugging **********//
	// QFont f0("Sans Serif", 8);
	// f0.setBold(false);
	//**************************//
    d_data->valueEdit = new QLabel();
    d_data->valueEdit->setObjectName(QString::fromUtf8("lineEdit"));
    d_data->valueEdit->setAlignment(Qt::AlignRight);
	//********* debugging **********//
    // d_data->valueEdit->setFont(f0);
    // d_data->valueEdit->setEnabled(false);
	//**************************//
    d_data->valueEdit->setContentsMargins(0,-1,1,-1);

	d_data->valueEdit->setFrameShadow(QFrame::Plain);
	d_data->valueEdit->setAutoFillBackground(true);
	d_data->valueEdit->setText("Empty chan");
	setFillColor(getFillColor());

    if (d_data->showlabel) {
	QFont f("Sans Serif", 8);
    d_data->pvName = new QLabel(this);
    d_data->pvName->setObjectName(QString::fromUtf8("pvName"));
    d_data->pvName->setText("PVname");
    d_data->pvName->setFont(f);
    //vboxLayout->insertWidget(1, d_data->pvName);
    vboxLayout->addWidget(d_data->pvName);
    }

    //d_data->pvUnit = new QLabel(this);
    //d_data->pvUnit->setObjectName(QString::fromUtf8("pvUnit"));

    vfboxLayout->addWidget(d_data->valueEdit);
    vboxLayout->addWidget(d_data->frame);
    //vfboxLayout->addWidget(d_data->pvUnit);
    //vboxLayout->addLayout(vfboxLayout);
    setLayout(vboxLayout);
	setPeriodic(OneSec);
}
void CADisplayer::changeValue (const short &connstatus, const double &value, const short& severity, const short& precision )
{
	QString sval;

	if (connstatus != ECA_CONN) 
	{
		//almsever = ALARM_NSEV;
		setFillColor(QColor("white"));
		//qDebug("*****CADisplayer: connstatus number: %d", connstatus);
		sval = "R";
		return;
	}
	else
	{
		//Normal
		//qDebug("PV: %s, CADisplayer: connstatus number: %d, %s", getPvname(), connstatus, ca_message(connstatus));
		//qDebug("*****CADisplayer: connstatus number: %d", connstatus);
	}

	if (getNuFormat() == Temperature)
	{
		sval = QString("%1").arg(value, 1, 'f', 1);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == Hall)
	{
		sval = QString("%1").arg(value, 1, 'f', 4);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	}
	else if (getNuFormat() == Strain)
	{
		sval = QString("%1").arg(value, 1, 'f', 6);
	}
	else if (getNuFormat() == Displace)
	{
		sval = QString("%1").arg(value, 1, 'f', 2);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	}
	else if (getNuFormat() == Pressure)
	{
		sval = QString("%1").arg(value, 1, 'e', 2);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	}
	else if (getNuFormat() == RGA)
	{
		sval = QString("%1").arg(value, 1, 'e', 3);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	}
	else if (getNuFormat() == Integer)
	{
		sval = QString("%1").arg(value, 1, 'f', 0);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == Flowrate)
	{
		sval = QString("%1").arg(value, 1, 'f', 2);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealOne)
	{
		sval = QString("%1").arg(value, 1, 'f', 1);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealTwo)
	{
		sval = QString("%1").arg(value, 1, 'f', 2);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealThree)
	{
		sval = QString("%1").arg(value, 1, 'f', 3);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealFour)
	{
		sval = QString("%1").arg(value, 1, 'f', 4);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealFive)
	{
		sval = QString("%1").arg(value, 1, 'f', 5);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		}
	} 
	else if (getNuFormat() == RealSix)
	{
		sval = QString("%1").arg(value, 1, 'f', 6);
		if (getUnit().size()>0)
		{
			sval.append(" ");
			sval.append(getUnit());
		};
	} 
	//******* debugging ********//
	// setPrefix(QString("TMS_"));
	// sval = getPvname();
	//**************************//

	short almsever = severity;
	switch(almsever)
	{
		case MAJOR_ALARM:
			setFillColor(QColor("red"));
			break;
		case MINOR_ALARM:
			setFillColor(QColor("yellow"));
			break;
		case INVALID_ALARM:
			setFillColor(QColor("gray"));
			break;
		case ALARM_NSEV:
			setFillColor(QColor("white"));
			break;
		case NO_ALARM:
			setFillColor(QColor(0,150,0));
			break;
		default:
			setFillColor(QColor("orange"));
			break;
	};

	//**************************//

	//**************************//
    d_data->valueEdit->setText(sval);
	emit valueChanged(value);
	update();
	setStatusTip(QString("PVname : ").append(getPvname()).append(QString(", Value: "))
			.append(QString("%1").arg(value,1,'g',3)).append(QString(", SEV: ")).append("%1").arg((int)almsever, 1)
			.append(QString(", ECA_STATUS: ")).append("%1").arg((int)connstatus, 1));
}
void CADisplayer::setLinewidth(const int line)
{
	d_data->lwid = line;
    d_data->frame->setLineWidth(d_data->lwid);
	update();
}
void CADisplayer::changePvName (const QString &pvname)
{
    d_data->pvName->setText(pvname);
    d_data->valueEdit->setText(pvname);
}
void CADisplayer::changePvUnit (const QString &pvunit)
{
    //d_data->pvUnit->setText(pvunit);
}
void CADisplayer::DisplayObjectFrame(const int type, QString objname)
{
	//qDebug("type: %d, passed objname:%s, objectname:%s", type, objname.toStdString().c_str(), objectName().toStdString().c_str());
	if(objname != objectName()) return;
	if( type == 1 )
	{
		d_data->orgcol = getFrameColor();
		d_data->orglwidth = getLinewidth();
		setFrameColor(QColor(Qt::blue));
		//setFrameColor(getFillColor().convertTo(QColor::Rgb));
		setLinewidth(4);
	}
	else
	{
		setFrameColor(d_data->orgcol);
		setLinewidth(d_data->orglwidth);
	}

}
#if 1
const QColor& CADisplayer::getFrameColor() const
{
	return d_data->framePalette.color(QPalette::Active, QPalette::WindowText);
}
void  CADisplayer::setFrameColor(const QColor &framecol)
{
    QBrush brush(framecol);
    d_data->framePalette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    d_data->framePalette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    d_data->frame->setPalette(d_data->framePalette);
}
#endif

const QColor& CADisplayer::getFillColor() const
{
	return d_data->fillPalette.color(QPalette::Active, QPalette::Window);
}
void  CADisplayer::setFillColor(const QColor &fillcol)
{
    QColor brush(fillcol);
    d_data->fillPalette.setBrush(QPalette::Active, QPalette::Window, brush);
    d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::Window, brush);

	if (brush.name().compare("#ffff00")==0 || brush.name().compare("gray")==0 || brush.name().compare("white")==0) 
	// yellow or gray or white
	{
    	d_data->fillPalette.setBrush(QPalette::Active, QPalette::WindowText, QColor("black"));
    	d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::WindowText, QColor("black"));
	}
	else if (brush.name().compare("#ff0000")==0 || brush.name().compare("#009600")==0 || brush.name().compare("#808080")==0)
	// red or darkgreen or gray
	{
    	d_data->fillPalette.setBrush(QPalette::Active, QPalette::WindowText, QColor("white"));
    	d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::WindowText, QColor("white"));
	}
    d_data->valueEdit->setPalette(d_data->fillPalette);
}

const int CADisplayer::getLinewidth()
{
	return d_data->lwid;
}

CADisplayer::~CADisplayer()
{
    delete d_data;
}

void CADisplayer::setEditable(bool editable)
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
    if ( editable == d_data->editable ) 
        return;

    d_data->editable = editable;
    //d_data->valueEdit->setReadOnly(!editable);
}

//! returns whether the line edit is edatble. (default is yes)
bool CADisplayer::getEditable() const 
{   
    return d_data->editable;
}

void CADisplayer::setShowLabel(bool showlabel)
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
    if ( showlabel == d_data->showlabel ) { 
        return;
    } else {
	d_data->showlabel = showlabel;
        setPvname(m_pvname);
    }
}

bool CADisplayer::getShowLabel() const 
{   
    return d_data->showlabel;
}

void CADisplayer::setPeriodic(ePeriodic periodic)
{
	d_data->periodic = periodic;
}

CADisplayer::ePeriodic CADisplayer::getPeriodic()
{
	return d_data->periodic;
}
