/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * KWT Widget Library
 * Copyright (C) 2010   Leesi
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the KWT License,

 *****************************************************************************/

// vim: expandtab

#include "blinkLabel.h"


class BlinkLabel::PrivateData
{
public:
    PrivateData():
		backcolor(QColor(255,110,15,200)),
		forecolor(QColor("red")),
		brush(forecolor),
		reversecolor(0)
    {
		brush.setStyle(Qt::SolidPattern);
    };

	QColor backcolor, forecolor;
	QBrush brush;
	QPalette palette;
	int timerid;
	int reversecolor;
};

BlinkLabel::BlinkLabel(QWidget *parent):QLabel(parent),reverse(0)
{
    init();
}

BlinkLabel::~BlinkLabel()
{
    delete d_data;
}

QSize BlinkLabel::sizeHint() const
{
	return minimumSizeHint();
}

QSize BlinkLabel::minimumSizeHint() const
{
	int mw = 56;
	int mh = 20;
	QSize sz;
	sz += QSize(mw, mh);
	return sz;
}

void BlinkLabel::init()
{
#if QT_VERSION >= 0x040000
    using namespace Qt;
#endif
	d_data = new PrivateData;

	setText("blinkLabel");
   	setObjectName(QString::fromUtf8("BlinkLabel"));

	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, d_data->brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, d_data->brush);

    //setFrameShape(QFrame::Box);
	//setFrameShadow(QFrame::Plain);
	//setLineWidth(3);
	setPalette(d_data->palette);
	startTimer(1000);
}

const QColor& BlinkLabel::getBackColor() const
{
	return d_data->backcolor;
}
void BlinkLabel::setBackColor(const QColor &color)
{
	d_data->backcolor = color;
}

const QColor& BlinkLabel::getForeColor() const
{
	return d_data->forecolor;
}
void BlinkLabel::setForeColor(const QColor &color)
{
	d_data->forecolor = color;
}

void BlinkLabel::timerEvent(QTimerEvent* /*event*/)
{
	if(reverse == 0)
	{
		reverse = 1;
		setColor(d_data->backcolor);
	}
	else
	{
		reverse = 0;
		setColor(d_data->forecolor);
	};

	update();
}

void BlinkLabel::setColor(const QColor &color)
{
	QBrush brush(color);
	brush.setStyle(Qt::SolidPattern);
	d_data->palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
	d_data->palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
	setPalette(d_data->palette);
}
