/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_GRAPHIC_H
#define CA_GRAPHIC_H

#include <QtGui>
#include "cadef.h"
#include "staticGraphic.h"

class CAGraphic : public StaticGraphic
{
	Q_OBJECT
	Q_ENUMS(DisplayMode Severity)
	Q_PROPERTY(DisplayMode fillDisplayMode READ fillDisplayMode WRITE setFillDisplayMode)
	Q_PROPERTY(DisplayMode lineDisplayMode READ lineDisplayMode WRITE setLineDisplayMode)
	Q_PROPERTY(bool popup READ getPopup WRITE setPopup);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname)
	Q_PROPERTY(QString filename READ getFilename WRITE setFilename)
	Q_PROPERTY(QStringList pvs READ getPvs WRITE setPvs)
	Q_PROPERTY(bool passwd READ getPasswd WRITE setPasswd);
	Q_PROPERTY(QString passwdstr READ getPasswdStr WRITE setPasswdStr);
	Q_PROPERTY(QColor minorAcolor READ getMinorAColor WRITE setMinorAColor);
	Q_PROPERTY(QColor majorAcolor READ getMajorAColor WRITE setMajorAColor);
	Q_PROPERTY(int blinkvalue READ getBlinkValue WRITE setBlinkValue);

public:
	explicit CAGraphic(QWidget *parent = NULL);
	virtual ~CAGraphic();

	enum DisplayMode {
		Static = 0x0000, // no change
		Alarm = 0x0001, // changed color as CA alarm severity
		IfNotZero = 0x0002, // show if value != 0
		IfZero = 0x0003, // show if value = 0
		ActInact = 0x0004, // change color as Active or Inactive Status
		Blink = 0x0005, // change Fill mode by timerEvent
	};

	enum Severity {
		NoAlarm = 0x0000,
		MinorAlarm = 0x0001,
		MajorAlarm = 0x0002,
		InvalidAlarm = 0x0003,
		AlarmNsev = 0x0004
	};

	DisplayMode fillDisplayMode() const;
	void setFillDisplayMode(DisplayMode fillDisplayMode);

	DisplayMode lineDisplayMode() const;
	void setLineDisplayMode(DisplayMode lineDisplayMode);

	const bool &getPopup() const;
	void  setPopup(const bool popup);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initGraphic();

	const QString &getPvname() const;
	void setPvname(const QString &pvname);

	const QString &getFilename() const;
	void setFilename(const QString &filename);

	const QStringList &getPvs() const;
	void setPvs(const QStringList &pvs);

	const bool &getPasswd() const;
	void  setPasswd(const bool passwd);
	const QString &getPasswdStr() const;
	void  setPasswdStr(const QString &passwdstr);

	const QColor & getMinorAColor()const;
	void  setMinorAColor(const QColor &color);

	const QColor & getMajorAColor()const;
	void  setMajorAColor(const QColor &color);

	int getBlinkValue() const; 
	//void  setBlinkValue(int &blinkvalue);
	void  setBlinkValue(int blinkvalue);

	int getValue() const;

signals:
	void changedValue (const double &value);
	void changedValue (bool value);
	void isTrue();
	void isFalse();

public slots:
	void changeValue (const short &connstatus, const double &value, const short &severity);
	void changePvName (const QString &pvname);
private:
	class PrivateData;
	PrivateData *d_data;
	bool test;
};
#endif
