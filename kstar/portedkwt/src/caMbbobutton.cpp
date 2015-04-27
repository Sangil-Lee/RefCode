/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caMbbobutton.h"


class CAMbboButton::PrivateData
{
public:
    PrivateData():
		m_bcontrol(true),
		m_bconfirm(false),
		selColor(QColor(255,0,0,200)),
		unselColor(QColor(220,220,220,200)),
		discColor(QColor("white")),
		checked_id(0)
    {
    };

	QString m_pvname;
	bool	m_bcontrol;
	bool	m_bconfirm;
	AlignPolicy m_align;
    //QPalette palette;
	QButtonGroup *m_buttongroup;
	QList <QPushButton*> m_btnlist;  
	QStringList m_textlist;
	//QMap<QString, QVariant> m_textlist;
	QList<int> m_bitlist;
	QColor selColor;
	QColor unselColor;
	QColor discColor;
	QPalette selPalette;
	QPalette unselPalette;
	QPalette discPalette;
	int checked_id;
	QStringList m_pvlist;
};

CAMbboButton::CAMbboButton(QWidget *parent):QWidget(parent)
{
    initMbboButton();
}

CAMbboButton::~CAMbboButton()
{
    delete d_data;
}

QSize CAMbboButton::sizeHint() const
{
	return minimumSizeHint();
}

QSize CAMbboButton::minimumSizeHint() const
{
	int mw = 80;
	int mh = 20;
	QSize sz;

	sz += QSize(mw, mh);
	return sz;
}

const QStringList CAMbboButton::getPVList() const
{
	return d_data->m_pvlist;
}

void CAMbboButton::setPVList(const QStringList pvlist)
{
	d_data->m_pvlist=pvlist;
}

void CAMbboButton::setPvname(const QString &name)
{
    d_data->m_pvname = name;
 	setStatusTip(QString("PVname : ").append(d_data->m_pvname));
}

const QString &CAMbboButton::getPvname() const
{
	return d_data->m_pvname;
}

const QStringList &CAMbboButton::getTextlist() const
{
	    return d_data->m_textlist;
}

void CAMbboButton::setTextlist(const QStringList &textlist)
{
	    d_data->m_textlist = textlist;
		//init2MbboButton();
}


const QStringList CAMbboButton::getBitlist() const
{
	QStringList slist;
	for (int i=0; i<d_data->m_bitlist.size(); i++)
	{
		if(slist.size()>i)
		{
			slist.replace(i, QString("%1").arg(d_data->m_bitlist.at(i)));
		} else {
			slist.append(QString("%1").arg(d_data->m_bitlist.at(i)));
		}

	//	qDebug("in getBitlist, Number: %d", d_data->m_bitlist.at(i));
	}
	return slist;
}

void CAMbboButton::setBitlist(const QStringList bitlist)
{
	bool ok;
	for (int i=0; i<bitlist.size(); i++)
	{
		if (d_data->m_bitlist.size()>i)
		{
			d_data->m_bitlist.replace(i, bitlist.at(i).toInt(&ok, 10));
		} else {
			d_data->m_bitlist.append(bitlist.at(i).toInt(&ok, 10));
		}
		//qDebug("in setBitlist, Number: %d", d_data->m_bitlist.at(i));
	}
	init2MbboButton();
}


void CAMbboButton::setControl(const bool control)
{
    d_data->m_bcontrol = control;
}

const bool &CAMbboButton::getControl() const
{
	return d_data->m_bcontrol;
}

void CAMbboButton::setConfirm(const bool confirm)
{
    d_data->m_bconfirm = confirm;
}

const bool &CAMbboButton::getConfirm() const
{
	return d_data->m_bconfirm;
}

CAMbboButton::AlignPolicy CAMbboButton::getAlignPolicy() const
{
	    return d_data->m_align;
}

void CAMbboButton::setAlignPolicy(AlignPolicy align)
{
	    d_data->m_align = align;
#if 1
		init2MbboButton();
#endif
}

const QColor& CAMbboButton::getSelColor() const
{
	return d_data->selPalette.color(QPalette::Active, QPalette::Button);
}

void CAMbboButton::setSelColor(const QColor &selCol)
{
	QBrush brush(selCol);
	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, brush);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
}

const QColor& CAMbboButton::getUnselColor() const
{
	return d_data->unselPalette.color(QPalette::Active, QPalette::Button);
}

void CAMbboButton::setUnselColor(const QColor &unselCol)
{
	QBrush brush(unselCol);
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, brush);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
}

void CAMbboButton::changePvName (const QString &pvname)
{
    setPvname(pvname);
}

void CAMbboButton::initMbboButton()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif

	d_data = new PrivateData; 

	d_data->selPalette.setBrush(QPalette::Active, QPalette::Button, d_data->selColor);
	d_data->selPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->selColor.dark());
	d_data->unselPalette.setBrush(QPalette::Active, QPalette::Button, d_data->unselColor);
	d_data->unselPalette.setBrush(QPalette::Inactive, QPalette::Button, d_data->unselColor.dark());
	setPvname("pvname");
	setAlignPolicy(Horizontal);

	d_data->m_buttongroup = new QButtonGroup(this);
	d_data->m_buttongroup->setExclusive(true);
	connect(d_data->m_buttongroup, SIGNAL(buttonClicked(int)), this, SLOT(clicked(int)));

#if 0
	for(int i=0; i<16; i++)
	{
		d_data->m_bitlist.append(i);
	}
#endif

}
#if 1
void CAMbboButton::init2MbboButton()
{
	QPushButton *btn;

	for(int i=0; i<d_data->m_textlist.size(); i++)
	{
		btn = new QPushButton(d_data->m_textlist.at(i));
		d_data->m_buttongroup->addButton(btn, d_data->m_bitlist.at(i));
		//d_data->m_buttongroup->setId(btn, d_data->m_bitlist.at(i));
		//qDebug("init2 Number: %d", d_data->m_bitlist.at(i));
		//qDebug("init2 Button Id: %d", d_data->m_buttongroup->id(btn));
	}

	if(getAlignPolicy() == Vertical)
	{
		QVBoxLayout *vbox = new QVBoxLayout;
		vbox->setSpacing(0);
		vbox->setMargin(0);
		vbox->addStretch(1);
		for(int i=0; i<d_data->m_textlist.size(); i++)
		{
			vbox->addWidget(d_data->m_buttongroup->button(d_data->m_bitlist.at(i)));
			setLayout(vbox);
		}
	}
	else if(getAlignPolicy() == Horizontal)
	{
		QHBoxLayout *hbox = new QHBoxLayout;
		hbox->setSpacing(0);
		hbox->setMargin(0);
		hbox->addStretch(1);
		for(int i=0; i<d_data->m_textlist.size(); i++)
		{
			hbox->addWidget(d_data->m_buttongroup->button(d_data->m_bitlist.at(i)));
			setLayout(hbox);
		}
	}

	setCursor(Qt::PointingHandCursor);

}
#endif

void CAMbboButton::changeValue (const short &connstatus, const int &value)
{
	QList<QAbstractButton *> buttons = d_data->m_buttongroup->buttons();
	foreach (QAbstractButton *button, buttons) {
		if (d_data->m_buttongroup->button(value) != button)
		{
			button->setChecked(false);
			button->setPalette(d_data->unselPalette);
		}
		else
		{
			button->setChecked(true);
			button->setPalette(d_data->selPalette);
		}

	}

	if (connstatus != ECA_CONN)
	{
		QBrush brush(d_data->discColor);
		d_data->discPalette.setBrush(QPalette::Active, QPalette::Button, brush);
		d_data->discPalette.setBrush(QPalette::Inactive, QPalette::Button, brush);
		foreach (QAbstractButton *button, buttons) {
			button->setPalette(d_data->discPalette);
		}
	} 

	setStatusTip(QString("PVname : ").append(d_data->m_pvname).append(QString(", Value: ")).append(QString("%1").arg(value)));
}

void CAMbboButton::clicked (int id)
{
	QList<QAbstractButton *> buttons = d_data->m_buttongroup->buttons();
#if 0
	foreach (QAbstractButton *button, buttons) {
		if (d_data->m_buttongroup->button(id) != button)
		{
			button->setChecked(false);
			button->setPalette(d_data->unselPalette);
		}
		else
		{
			button->setChecked(true);
			button->setPalette(d_data->selPalette);
		}

	}
#endif

	d_data->checked_id = id;
	//qDebug("Button's id: %d", id);

	emit valueChanged(d_data->checked_id);
}

void CAMbboButton::print(const bool status)
{
//	qDebug("CAMbboButton::print");
}
void CAMbboButton::setTrueActivate(const bool status)
{
	bool st = status;
	setEnabled(st);
}
void CAMbboButton::setFalseActivate(const bool status)
{
	bool st;
	if (status == true) st= false;
	else st = true;
	setEnabled(st);
}
