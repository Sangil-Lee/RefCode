/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef _BLINKLABEL_H
#define _BLINKLABEL_H

#include <QtGui>

class BlinkLabel : public QLabel
{
	//++leesi
	Q_OBJECT
	Q_ENUMS(eAlarm)
	Q_PROPERTY(QColor backcolor READ getBackColor WRITE setBackColor);
	Q_PROPERTY(QColor forecolor READ getForeColor WRITE setForeColor);

public:
	explicit BlinkLabel(QWidget *parent = NULL);
	virtual ~BlinkLabel();

	enum eAlarm {
		DisConnState = 0x0000,
		NormalState = 0x0001,
		AlarmState  = 0x0002,
		NotDefine  = 0x0003
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	const QColor & getBackColor()const;
	void  setBackColor(const QColor &color);

	const QColor & getForeColor()const;
	void  setForeColor(const QColor &color);

protected:
	virtual void timerEvent(QTimerEvent *event);

private:
	class PrivateData;
	PrivateData *d_data;
	void init();
	void setColor(const QColor &color);
	int reverse;
};
#endif
