/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_BOBUTTON_H
#define CA_BOBUTTON_H

#include <QtGui>
#include <QWidget>
#include <QPushButton>
#include <QButtonGroup>
#include "cadef.h"

class CABoButton : public QWidget
{
	Q_OBJECT
#if 0
	Q_ENUMS(DisplayMode)
	Q_PROPERTY(DisplayMode dmode READ getDisplaymode WRITE setDisplaymode)
#endif
	Q_ENUMS(AlignPolicy)
	Q_ENUMS(TrueOrFalse)
	Q_PROPERTY(AlignPolicy align READ getAlignPolicy WRITE setAlignPolicy)
	Q_PROPERTY(QStringList pvlist READ getPVList WRITE setPVList);
	Q_PROPERTY(QStringList objlist READ getObjectList WRITE setObjectList);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname)
	Q_PROPERTY(bool control READ getControl WRITE setControl)
	Q_PROPERTY(bool confirm READ getConfirm WRITE setConfirm)
	Q_PROPERTY(bool passwd READ getPasswd WRITE setPasswd);
	Q_PROPERTY(bool estop READ getEStop WRITE setEStop);
	Q_PROPERTY(QString passwdstr READ getPasswdStr WRITE setPasswdStr);
	Q_PROPERTY(QString truelabel READ getTruelabel WRITE setTruelabel)
	Q_PROPERTY(QString falselabel READ getFalselabel WRITE setFalselabel)
	Q_PROPERTY(QColor selColor READ getSelColor WRITE setSelColor)
	Q_PROPERTY(QColor unselColor READ getUnselColor WRITE setUnselColor)
	Q_PROPERTY(int timeset READ getTimeSet WRITE setTimeSet)
	Q_PROPERTY(TrueOrFalse boolvalue READ getBoolValue WRITE setBoolValue)

public:
	explicit CABoButton(QWidget *parent = NULL);
	virtual ~CABoButton();

	enum DisplayMode {
		TrueFalse = 0x0000,
		TrueOnly = 0x0001,
		FalseOnly = 0x0002
	};

	enum TrueOrFalse {
		False = 0,
		True
	};

	enum AlignPolicy {
		Column = 0x0000,
		Row = 0x0001
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initBoButton();
	void init2BoButton();

	const QStringList getPVList() const;
	void setPVList(const QStringList pvlist);

	const QStringList getObjectList() const;
	void setObjectList(const QStringList objlist);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

#if 0
	DisplayMode getDisplaymode() const;
	void setDisplaymode(DisplayMode dmode);
#endif

	const bool &getControl() const;
	void  setControl(const bool control);

	const bool &getConfirm() const;
	void  setConfirm(const bool confirm);

	const bool &getPasswd() const;
	void  setPasswd(const bool passwd);

	const bool &getEStop() const;
	void setEStop(const bool estop);

	const QString &getPasswdStr() const;
	void  setPasswdStr(const QString &passwdstr);

	AlignPolicy getAlignPolicy() const;
	void  setAlignPolicy(AlignPolicy align);

	const QString &getTruelabel() const;
	void  setTruelabel(const QString &truelabel);

	const QString &getFalselabel() const;
	void  setFalselabel(const QString &falselabel);

	const QColor &getSelColor() const;
	void setSelColor(const QColor &selColor);

	const QColor &getUnselColor() const;
	void setUnselColor(const QColor &unselColor);

	const int getTimeSet () const;
	void setTimeSet(const int timeset); 

	TrueOrFalse getBoolValue() const;
	void setBoolValue(TrueOrFalse boolval);

	const bool & getValue() const;
//	bool getStatus();

public slots:
	virtual void changeValue (const short &connstatus, const double &value);
	void changePvName (const QString &pvname);
	void falsepressed(const bool status);
	void truepressed(const bool status);
	void print(const bool status);
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);
	void timerCallback();

signals:
	void isTrue(const bool status);
	void isFalse(const bool status);
	void valueChanged (const bool value);
	void RelatedCADisplayer(const int type, QString objname);

protected:
	//virtual void timerEvent(QTimerEvent *event);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
