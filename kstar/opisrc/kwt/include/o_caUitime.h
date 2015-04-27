/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef CA_UITIME_H
#define CA_UITIME_H

#include <QtGui>

class CAUITime : public QWidget
{
	Q_OBJECT
	Q_ENUMS(TimeSet PeriodicSecond)
	Q_PROPERTY(TimeSet timeset READ getTimeSet WRITE setTimeSet);
	Q_PROPERTY(PeriodicSecond periodicsecond READ getPeriodicSecond WRITE setPeriodicSecond);
	Q_PROPERTY(QString pvname READ getMasterPV WRITE setMasterPV);

public:
	enum TimeSet {
		Periodic = 0,
		Event = 1
	};
#if 0
	enum PeriodicSecond{
		Second1 = 1,
		Second2 = 2,
		Second5 = 5,
		Second10 = 10,
		Second30 = 30
	};
#endif
	enum PeriodicSecond{
		One = 1,
		Two = 2,
		Five = 5,
		Ten = 10,
		Thirty = 30
	};

	explicit CAUITime(QWidget *parent = NULL);
	virtual ~CAUITime();

	TimeSet getTimeSet() const;
	void setTimeSet(TimeSet timeset);

	PeriodicSecond getPeriodicSecond() const;
	void setPeriodicSecond(PeriodicSecond periodicecond);

	const QString &getMasterPV() const;
	void setMasterPV(const QString &pvname);

public slots:
	virtual void changeValue (const int &value);

signals:
	void valueChanged(const bool &value);
private:
	class PrivateData;
	PrivateData *p_data;
};
#endif

