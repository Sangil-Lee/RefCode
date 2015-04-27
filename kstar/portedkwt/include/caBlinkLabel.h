/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_BLINKLABEL_H
#define CA_BLINKLABEL_H

#include "cadef.h"
#include <QtGui>
#include <QLabel>

class CABlinkLabel : public QLabel
{
	//++leesi
	Q_OBJECT
	Q_ENUMS(eAlarm)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);
	Q_PROPERTY(eAlarm alarm READ getAlarm WRITE setAlarm);
	Q_PROPERTY(QColor normalcolor READ getNormalColor WRITE setNormalColor);
	Q_PROPERTY(QColor alarmcolorR READ getAlarmColorR WRITE setAlarmColorR);
	Q_PROPERTY(QColor alarmcolorF READ getAlarmColorF WRITE setAlarmColorF);

public:
	explicit CABlinkLabel(QWidget *parent = NULL);
	virtual ~CABlinkLabel();

	enum eAlarm {
		DisConnState = 0x0000,
		NormalState = 0x0001,
		AlarmState  = 0x0002,
		NotDefine  = 0x0003
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const QString &getPrefix() const;
	void  setPrefix(const QString &prefix);

	const QColor & getNormalColor()const;
	void  setNormalColor(const QColor &color);

	const QColor & getAlarmColorR()const;
	void  setAlarmColorR(const QColor &color);

	const QColor & getAlarmColorF()const;
	void  setAlarmColorF(const QColor &color);

	eAlarm getAlarm();
	void setAlarm(eAlarm alarm);

protected:
	virtual void timerEvent(QTimerEvent *event);

public slots:
	virtual void changeValue (const short &connstatus, const double &value, const short& severity);
	void changePvName (const QString &pvname);

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
