/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caLabel.h"


class CALabel::PrivateData
{
public:
    PrivateData():
		fillcol(QColor("orange")), truecol(QColor("green")), falsecol(QColor("yellow")), m_dbrequest(DBRGDBL),busecolor(false)
    {
    };

	DisplayMode dmode;
	ColorMode cmode;
	ToolTipDisplayMode ttdmode;
	QString m_pvname;
	QString m_truelabel;
	QString m_falselabel;
	QStringList m_msg;
	QColor fillcol;
	QColor truecol;
	QColor falsecol;
	QPalette fillPalette;
	//++leesi
	DBREQUEST m_dbrequest;
	bool busecolor;
};

CALabel::CALabel(QWidget *parent):QLabel(parent)
{
    initLabel();
}

CALabel::~CALabel()
{
    delete d_data;
}

QSize CALabel::sizeHint() const
{
	return minimumSizeHint();
}

QSize CALabel::minimumSizeHint() const
{
	int mw = 120;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

void CALabel::setPvname(const QString &name)
{
    d_data->m_pvname = name;
	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CALabel::getPvname() const
{
	return d_data->m_pvname;
}

CALabel::DisplayMode CALabel::getDisplaymode() const
{
	    return d_data->dmode;
}

void CALabel::setDisplaymode(DisplayMode dmode)
{
	    d_data->dmode = dmode;
}

CALabel::ColorMode CALabel::getColormode() const
{
	    return d_data->cmode;
}

void CALabel::setColormode(ColorMode cmode)
{
	    d_data->cmode = cmode;
}

const QColor& CALabel::getFalseColor() const
{
	return d_data->falsecol;
}

void CALabel::setFalseColor(const QColor &color)
{
	d_data->falsecol = color;
}

const QColor& CALabel::getTrueColor() const
{
	return d_data->truecol;
}

void CALabel::setTrueColor(const QColor &color)
{
	d_data->truecol = color;
}

const QColor& CALabel::getFillColor() const
{
	    return d_data->fillPalette.color(QPalette::Active, QPalette::Window);
}
void CALabel::setFillColor(const QColor &fillcol)
{
	d_data->fillPalette = palette();
	QColor brush(fillcol);

	d_data->fillPalette.setBrush(QPalette::Active, QPalette::Window, brush);
	d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::Window, brush);

	if (brush.name().compare("#ffff00")==0 || brush.name().compare("silver")==0 || brush.name().compare("white")==0 || 
			brush.name().compare("#808080")==0 || brush.name().compare("gray")==0 || brush.name().compare("#787878")==0)    
	// yellow or gray or white
	{
		d_data->fillPalette.setBrush(QPalette::Active, QPalette::WindowText, QColor("black"));
		d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::WindowText, QColor("black"));
	}
	//else if (brush.name().compare("#ff0000")==0 || brush.name().compare("#009600")==0 || brush.name().compare("#808080")==0 
	else if (brush.name().compare("#ff0000")==0 || brush.name().compare("#009600")==0 || 
			brush.name().compare("#00ff00")== 0 ||  brush.name().compare("#008000")==0   )
	// red or darkgreen or gray
	{
		d_data->fillPalette.setBrush(QPalette::Active, QPalette::WindowText, QColor("white"));
		d_data->fillPalette.setBrush(QPalette::Inactive, QPalette::WindowText, QColor("white"));
	}
	setPalette(d_data->fillPalette);
}

CALabel::ToolTipDisplayMode CALabel::getTTDisplaymode() const
{
	    return d_data->ttdmode;
}

void CALabel::setTTDisplaymode(ToolTipDisplayMode ttdmode)
{
	    d_data->ttdmode = ttdmode;
}

const bool & CALabel::getUseColor() const
{
	return d_data->busecolor;
}

void  CALabel::setUseColor(const bool usecolor)
{
	d_data->busecolor = usecolor;
}

void CALabel::setTruelabel(const QString &truelabel)
{
    d_data->m_truelabel = truelabel;
}

const QString &CALabel::getTruelabel() const
{
	return d_data->m_truelabel;
}

void CALabel::setFalselabel(const QString &falselabel)
{
    d_data->m_falselabel = falselabel;
}

const QString &CALabel::getFalselabel() const
{
	return d_data->m_falselabel;
}

//++leesi
CALabel::DBREQUEST CALabel::getDBRequest() const
{
	return d_data->m_dbrequest;
}
void CALabel::setDBRequest(DBREQUEST dbrequest)
{
	d_data->m_dbrequest = dbrequest;
}
//

const QStringList &CALabel::getMsg() const
{
	    return d_data->m_msg;
}

void CALabel::setMsg(const QStringList &msg)
{
	    d_data->m_msg = msg;
}

void CALabel::initLabel()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

	setText("CALabel");
	setTruelabel("True");
	setFalselabel("False");
	d_data->fillPalette = palette();
   	//setText(getTruelabel());
   	setObjectName(QString::fromUtf8("CALabel"));
	setPvname("pvname");
	setDisplaymode(CALabel::TrueFalse);
	setTTDisplaymode(CALabel::Hide);
	setColormode(Static);
}

void CALabel::changeValue (const short &connstatus, const double &value, const short& severity)
{
	QString str;
	bool val = (bool)value;
	if (connstatus != ECA_CONN)
	{
		str = QString("Disconnected");
		setText(str);
		if(getTTDisplaymode() ==CALabel::Show)
		{
			setToolTip(str);
		}
		setFillColor(QColor("white"));
		return;
	} else
	{
		switch(getColormode()) {
			case Alarm:
				setAutoFillBackground(true);
				switch(severity)
				{
					case NoAlarm:
						setFillColor(QColor(0,150,0));
						break;
					case MinorAlarm:
						setFillColor(QColor("yellow"));
						break;
					case MajorAlarm:
						setFillColor(QColor("red"));
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
			case IfZero:
				{
					if(!val)
					{
						setAutoFillBackground(true);
						setFillColor(QColor("silver"));
					}
					else
					{
						setAutoFillBackground(false);
						setFillColor(QColor("silver"));
					}
				}
				break;
			case IfNotZero:
				{
					if(val)
					{
						setAutoFillBackground(true);
						setFillColor(QColor(0, 255, 0));
					}
					else
					{
						setAutoFillBackground(false);
						setFillColor(QColor("silver"));
					}
				}
				break;
			case Static:
				break;
			default:
				break;
		}

		//Why did it use valueChanged signal? //++leesi
		switch(getDisplaymode())
		{
			case Choice:
				if((int)value < getMsg().size())
				{
					str = getMsg().at((int)value);
					setText(str);
				}
				else
				{
					setText("Invalid");
				}
				emit valueChanged(val);
				break;
			case TrueFalse:
				if((bool)value) 
				{
					if(getUseColor()==true)
						setFillColor(getTrueColor());
					setText(getTruelabel());
					//qDebug("text:%s", text().toStdString().c_str());
				} 
				else if(!(bool)value)
				{
					//setText("");
					if(getUseColor()==true)
						setFillColor(getFalseColor());
					setText(getFalselabel());
				}
				emit valueChanged(val);
				break;
			case Int:
				//str = QString("%1").arg(value);
				str = QString("%1").arg(value, 1, 'f',0);
				setText(str);
				emit valueChanged(val);
				break;
			case Double:
				str = QString("%1").arg(value, 1, 'f', 2);
				setText(str);
				emit valueChanged((double)val);
				break;
			case RealOne:
				str = QString("%1").arg(value, 1, 'f', 1);
				setText(str);
				emit valueChanged((double)val);
				break;
			case Extension:
				str = QString("%1").arg(value, 1, 'g', 3);
				setText(str);
				emit valueChanged((double)val);
				break;
			default:
				break;
		}
	}

	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value,1,'g',3)));
	if(getTTDisplaymode() ==CALabel::Show)
	{
		setToolTip(str);
	}
}

void CALabel::changeValue (const short &connstatus, const QString &value, const short& severity)
{
	QString str = value;
	//qDebug("value:%s", value.toStdString().c_str());
	double number = value.toDouble();
	if (connstatus != ECA_CONN)
	{
		str = QString("Disconnected");
		setText(str);
		if(getTTDisplaymode() ==CALabel::Show)
		{
			setToolTip(str);
		}
		setFillColor(QColor("white"));
		return;
	} else
	{
		switch(getDisplaymode())
		{
			case Choice:
				setText(value);
				emit valueChanged(number);
				break;
			case TrueFalse:
				setText(value);
				emit valueChanged((bool)number);
				break;
			case Int:
				str = QString("%1").arg(number);
				setText(str);
				emit valueChanged(number);
				break;
			case Double:
				str = QString("%1").arg(number, 1, 'f', 2);
				setText(str);
				emit valueChanged(number);
				break;

			default:
				str = QString("%1").arg(number, 1, 'f', 2);
				setText(str);
				emit valueChanged(number);
				break;
		}
		switch(getColormode()) 
		{
			case Alarm:
				if (connstatus != ECA_CONN)
				{
					setFillColor(QColor("white"));
					return;
				}
				switch(severity)
				{
					case NoAlarm:
						setFillColor(QColor(0,150,0));
						break;
					case MinorAlarm:
						setFillColor(QColor("yellow"));
						break;
					case MajorAlarm:
						setFillColor(QColor("red"));
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
			case Static:
				break;
			default:
				break;
		}
	}
	//emit valueChanged(number);
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(str));
}

#if 0
void CALabel::changeValue (const int &value)
{
	switch(getDisplaymode())
	{
		case Choice:
			setText(getMsg().at(value));
			break;
		case TrueFalse:
			if((bool)value) 
			{
				setText(getTruelabel());
			} 
			else if(!(bool)value)
			{
				//setText("");
				setText(getFalselabel());
			}
		default:
			break;
	}
	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg((double)value,1,'g',3)));
}
#endif

void CALabel::changePvName (const QString &pvname)
{
    setPvname(pvname);
}
