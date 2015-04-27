/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_WCLOCK_H
#define CA_WCLOCK_H

#include <QtGui>

#include <epicsTime.h>

class CAWclock : public QLabel
{
	Q_OBJECT
	Q_ENUMS (TimeFormat)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY (TimeFormat tformat READ getTformat WRITE setTformat);
	Q_PROPERTY (int trigger1 READ getTrigger1 WRITE setTrigger1);
	Q_PROPERTY (int trigger2 READ getTrigger2 WRITE setTrigger2);

public:
	explicit CAWclock(QWidget *parent = NULL);
	virtual ~CAWclock();

	enum TimeFormat {
		Date_Time = 0x0000,
		Time = 0x0001,
		Duration = 0x0002
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initWclock();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	TimeFormat getTformat() const;
	void setTformat (TimeFormat tformat);

	const int getTrigger1();
	void setTrigger1 (const int trigger1);

	const int getTrigger2();
	void setTrigger2 (const int trigger2);

public slots:
	virtual void changeValue (const double &value);
	virtual void changeValue (const short &connstatus, const QString &value, const short& severity);
	void changePvName (const QString &pvname);
signals:
	void trigger1(void);
	void trigger2(void);
private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
