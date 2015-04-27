/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef _BLINKLINE_H
#define _BLINKLINE_H

#include "cadef.h"
#include <QtGui>

class BlinkLine : public QFrame
{
	//++leesi
	Q_OBJECT
	//Q_ENUMS(eAlarm ColorMode)
	Q_ENUMS(eAlarm)
	Q_PROPERTY(QString ipaddr READ getIPAddr WRITE setIPAddr);
	Q_PROPERTY(eAlarm alarm READ getAlarm WRITE setAlarm);
	Q_PROPERTY(QColor normalcolor READ getNormalColor WRITE setNormalColor);
	Q_PROPERTY(QColor alarmcolorR READ getAlarmColorR WRITE setAlarmColorR);
	Q_PROPERTY(QColor alarmcolorF READ getAlarmColorF WRITE setAlarmColorF);
	Q_PROPERTY(unsigned int count READ getCount WRITE setCount);

public:
	explicit BlinkLine(QWidget *parent = NULL);
	virtual ~BlinkLine();

	enum eAlarm {
		NormalState = 0x0000,
		AlarmState  = 0x0001
	};

#if 0
	enum ColorMode {
		Static = 0x0000,
		Alarm = 0x0001,
		IfZero = 0x0002,
		IfNotZero = 0x0003
	};
#endif

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	const QString &getIPAddr() const;
	void  setIPAddr(const QString &ipaddr);

	unsigned int getCount() const;
	void  setCount(unsigned int count);
	void  IncCount();

	//ColorMode getColormode() const;
	//void setColormode(ColorMode cmode);

	const QColor & getNormalColor()const;
	void  setNormalColor(const QColor &color);

	const QColor & getAlarmColorR()const;
	void  setAlarmColorR(const QColor &color);

	const QColor & getAlarmColorF()const;
	void  setAlarmColorF(const QColor &color);

	eAlarm getAlarm();
	void setAlarm(eAlarm alarm);

public slots:
	virtual void changeValue (const short &connstatus, const double &value, const short& severity);
protected:
	virtual void timerEvent(QTimerEvent *event);

signals:
	void valueChanged (const double &value);
	void valueChanged (const bool &value);

private:
	class PrivateData;
	PrivateData *d_data;
	void init();
	void setColor(const QColor &color);
};
#endif
