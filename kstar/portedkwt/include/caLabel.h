/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_LABEL_H
#define CA_LABEL_H

#include "cadef.h"
#include <QtGui>
#include <QLabel>

class CALabel : public QLabel
{
	Q_OBJECT
	//++leesi
	Q_ENUMS(DisplayMode Severity ColorMode ToolTipDisplayMode DBREQUEST)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(DisplayMode dmode READ getDisplaymode WRITE setDisplaymode);
	Q_PROPERTY(ColorMode cmode READ getColormode WRITE setColormode);
	Q_PROPERTY(QColor fillcol READ getFillColor WRITE setFillColor);
	Q_PROPERTY(ToolTipDisplayMode ttdmode READ getTTDisplaymode WRITE setTTDisplaymode);
	Q_PROPERTY(bool usecolor READ getUseColor WRITE setUseColor)
	Q_PROPERTY(QString truelabel READ getTruelabel WRITE setTruelabel);
	Q_PROPERTY(QColor truecolor READ getTrueColor WRITE setTrueColor);
	Q_PROPERTY(QString falselabel READ getFalselabel WRITE setFalselabel);
	Q_PROPERTY(QColor falsecolor READ getFalseColor WRITE setFalseColor);

	//++leesi
	Q_PROPERTY(DBREQUEST dbrequest  READ getDBRequest WRITE setDBRequest);
	Q_PROPERTY(QStringList messages READ getMsg WRITE setMsg);

public:
	explicit CALabel(QWidget *parent = NULL);
	virtual ~CALabel();

    enum DisplayMode {
        Choice = 0x0000,
        TrueFalse = 0x0001,
		Int = 0x0002,
		Double = 0x0003,
		RealOne = 0x0004,
		Extension = 0x0005
    };

	enum Severity {
		NoAlarm = 0x0000,
		MinorAlarm = 0x0001,
		MajorAlarm = 0x0002,
		InvalidAlarm = 0x0003,
		AlarmNsev = 0x0004
	};

	enum ColorMode {
		Static = 0x0000,
		Alarm = 0x0001,
		IfZero = 0x0002,
		IfNotZero = 0x0003
	};

	enum ToolTipDisplayMode {
		Hide = 0x0000,
		Show = 0x0001
	};

	//++leesi EPICS DB Request Type
	enum DBREQUEST {
		DBRGDBL = DBR_GR_DOUBLE,
		DBRGSTR = DBR_TIME_STRING
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initLabel();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	DisplayMode getDisplaymode() const;
	void setDisplaymode(DisplayMode dmode);
	
	ColorMode getColormode() const;
	void setColormode(ColorMode cmode);

	const QColor & getFillColor()const;
	void  setFillColor(const QColor &framecol);
	
	ToolTipDisplayMode getTTDisplaymode() const;
	void setTTDisplaymode(ToolTipDisplayMode ttdmode);

	const bool &getUseColor() const;
	void  setUseColor(const bool confirm);

	const QString &getTruelabel() const;
	void  setTruelabel(const QString &truelabel);

	const QColor & getTrueColor()const;
	void  setTrueColor(const QColor &color);

	const QString &getFalselabel() const;
	void  setFalselabel(const QString &falselabel);

	const QColor & getFalseColor()const;
	void  setFalseColor(const QColor &color);

	//++leesi
	DBREQUEST getDBRequest() const;
	void setDBRequest(DBREQUEST dbrequest);


	const QStringList &getMsg() const;
	void setMsg(const QStringList &messages);

public slots:
	//virtual void changeValue (const double &value);
	virtual void changeValue (const short &connstatus, const double &value, const short& severity);
	virtual void changeValue (const short &connstatus, const QString &value, const short& severity);
#if 0
	virtual void changeValue (const int &value);
#endif
	void changePvName (const QString &pvname);

signals:
	void valueChanged (const double &value);
	void valueChanged (const bool &value);
	void valueChangedTrue (void);
	void valueChangedFalse (void);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
