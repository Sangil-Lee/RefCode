/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "caColorcheckbox.h"


#if 0
class CAColorCheckBox::PrivateData
{
public:
    PrivateData()
    {
    };
};
#endif

//CAColorCheckBox::CAColorCheckBox(QWidget *parent):QLabel(parent)
//CAColorCheckBox::CAColorCheckBox(QWidget *parent):QPushButton(parent)
//CAColorCheckBox::CAColorCheckBox(QWidget *parent):QRadioButton(parent)
CAColorCheckBox::CAColorCheckBox(QWidget *parent):QCheckBox(parent)
{
    initLabel();
}

CAColorCheckBox::~CAColorCheckBox()
{
    //delete d_data;
}

void CAColorCheckBox::initLabel()
{
	//d_data = new PrivateData;
	setObjectName(QString::fromUtf8("CAColorCheckBox"));
	setText(QString::fromUtf8("colorcheckbox"));
	setCheckable(true);
	setChecked(true);
	setAutoFillBackground(true);
	connect(this, SIGNAL(clicked(bool)), SLOT(pshowlabel(const bool)) );
}
void CAColorCheckBox::pshowlabel( const bool bshow )
{
	emit showLabel(bshow, text());
}

void 
CAColorCheckBox::changeColorLabel(const QString &strcolor, const QString &label)
{
	QPalette pal(palette());
	QColor coltest(strcolor);
	//pal.setBrush(QPalette::Active,  QPalette::Base, QColor(strcolor));
	//pal.setBrush(QPalette::Inactive,QPalette::Base, QColor(strcolor));
	pal.setBrush(QPalette::Active,  QPalette::WindowText, QColor(strcolor));
	pal.setBrush(QPalette::Inactive,QPalette::WindowText, QColor(strcolor));

	//int v = coltest.value();
	//qDebug("value:%d", v);
	//for complement color
	//QColor revcol(strcolor);
	//revcol.setRgb(~revcol.rgb());
	//pal.setBrush(QPalette::Active, QPalette::Text, revcol);
	//pal.setBrush(QPalette::Inactive, QPalette::Text, revcol);
	//pal.setBrush(QPalette::Active, QPalette::Text, QColor(strcolor));
	//pal.setBrush(QPalette::Inactive, QPalette::Text, QColor(strcolor));
	setPalette(pal);
	setText(label);
}
