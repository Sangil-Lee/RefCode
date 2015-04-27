/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_LINEEDIT_H
#define CA_LINEEDIT_H

#include <QtGui>
#include "cadef.h"
#include "qwt_global.h"

#define NO_ALARM		0x0
#define	MINOR_ALARM		0x1
#define	MAJOR_ALARM		0x2
#define INVALID_ALARM	0x3
#define ALARM_NSEV	INVALID_ALARM+1

class QWT_EXPORT CALineEdit : public QLineEdit
{
	Q_OBJECT
	Q_ENUMS(NumeticFormat DBREQUEST)
	Q_PROPERTY(NumeticFormat nuformat READ getNuFormat WRITE setNuFormat);
	Q_PROPERTY(bool control READ getControl WRITE setControl);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);
	Q_PROPERTY(QString unit READ getUnit WRITE setUnit);
	Q_PROPERTY(DBREQUEST dbrequest READ getDBRequest WRITE setDBRequest);

public:
	explicit CALineEdit(QWidget *parent = NULL);
	virtual ~CALineEdit();

	enum NumeticFormat {
		RealOne = 0x0001,
		RealTwo = 0x0002,
		RealThree = 0x0003,
		RealFour = 0x0004,
		RealFive = 0x0005,
		RealSix = 0x0006,
		Integer = 0x0011,
		Eng = 0x0021
	};

	enum DBREQUEST {
		DBRGDBL = DBR_GR_DOUBLE,
		DBRGSTR = DBR_TIME_STRING
	};


	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	NumeticFormat getNuFormat() const;
	void setNuFormat(NumeticFormat nuformat);

	const bool &getControl() const;
	void  setControl(const bool control);

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

	const QString &getPrefix() const;
	void  setPrefix(const QString &prefix);

	const QString &getUnit() const;
	void  setUnit(const QString &unit);

	DBREQUEST getDBRequest() const;
	void setDBRequest(DBREQUEST dbrequest);

	void initLineEdit();

	// a set of dummies to help the designer
signals:
	//const QString & valueChanged (const QString &stringvalue);
	//void valueChanged (const QString &stringvalue);
	//double valueChanged (const QString &stringvalue);
	void changedValue (const double &value);
	void enteredValue (const double &value);
	void enteredValue (const QString &value);
public slots:
	virtual void valueEntered();
	virtual void changeValue (const short &connstatus, const double &value, const short& severity, const short& precision );
	virtual void changeValue (const short &connstatus, const QString &value, const short& severity, const short& precision );
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);
	//void changePvName (const QString &pvname);
	//void changePvUnit (const QString &pvunit);
private:
	QString m_pvname;
	QString m_prefix;
	QString m_unit;
	//double m_val;
	class PrivateData;
	PrivateData *d_data;
};
#endif
