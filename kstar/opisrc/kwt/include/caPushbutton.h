/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef CA_PUSHBUTTON_H
#define CA_PUSHBUTTON_H

#include <QtGui>
#include "cadef.h"

class CAPushButton : public QPushButton
{
	Q_OBJECT
	Q_ENUMS(DisplayMode ButtonType)
	Q_PROPERTY(DisplayMode displayMode READ displayMode WRITE setDisplayMode);
	Q_PROPERTY(ButtonType buttonType READ buttonType WRITE setButtonType);
	//Q_PROPERTY(int pulsemsec READ getPulseSec WRITE setPulseSec);
	//Q_PROPERTY(bool initstate READ getState WRITE setState);
	Q_PROPERTY(QStringList pvlist READ getPVList WRITE setPVList);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(bool control READ getControl WRITE setControl);
	Q_PROPERTY(bool confirm READ getConfirm WRITE setConfirm);
	Q_PROPERTY(bool passwd READ getPasswd WRITE setPasswd);
	Q_PROPERTY(bool estop READ getEStop WRITE setEStop);
	Q_PROPERTY(QString passwdstr READ getPasswdStr WRITE setPasswdStr);
	Q_PROPERTY(QString truelabel READ getTruelabel WRITE setTruelabel);
	Q_PROPERTY(QString falselabel READ getFalselabel WRITE setFalselabel);
	Q_PROPERTY(QIcon trueicon READ getTrueicon WRITE setTrueicon);
	Q_PROPERTY(QIcon falseicon READ getFalseicon WRITE setFalseicon);
	Q_PROPERTY(QColor selColor READ getSelColor WRITE setSelColor);
	Q_PROPERTY(QColor unselColor READ getUnselColor WRITE setUnselColor);
	Q_PROPERTY(QColor selTColor READ getSelTColor WRITE setSelTColor);
	Q_PROPERTY(QColor unselTColor READ getUnselTColor WRITE setUnselTColor);

public:
	explicit CAPushButton(QWidget *parent = NULL);
	virtual ~CAPushButton();

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	enum DisplayMode {
			TextOnly = 0x0000,
			IconOnly = 0x0001
	};

	enum ButtonType {
			ToggleButton = 0x0000,
			TrueButton = 0x0001,
			FalseButton = 0x0002,
			PulseButton = 0x0003
	};

	DisplayMode displayMode() const;
	void setDisplayMode(DisplayMode dmode);

	ButtonType buttonType() const;
	void setButtonType(ButtonType btype);

	void initPushButton();
	void init2PushButton();

	const QStringList getPVList() const;
	void setPVList(const QStringList pvlist);

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

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

	const QString &getTruelabel() const;
	void  setTruelabel(const QString &truelabel);
	const QString &getFalselabel() const;
	void  setFalselabel(const QString &falselabel);

	const QIcon getTrueicon() const;
	void  setTrueicon(const QIcon &trueicon);
	const QIcon getFalseicon() const;
	void  setFalseicon(const QIcon &falseicon);

	const QColor &getSelColor() const;
	void setSelColor(const QColor &selColor);

	const QColor &getUnselColor() const;
	void setUnselColor(const QColor &unselColor);

	const QColor &getSelTColor() const;
	void setSelTColor(const QColor &selTColor);

	const QColor &getUnselTColor() const;
	void setUnselTColor(const QColor &unselTColor);

#if 0
	const int getPulseSec() const;
	void setPulseSec(const int msec);
	void PulseTime(const bool value);
	const bool &getState() const;
	void  setState(const bool state);
#endif
	const bool &getValue() const;



public slots:
	virtual void changeValue (const short &connstatus, const int &value);
	void changePvName (const QString &pvname);
	void setStatus (const bool status);
	void setTrueActivate (const bool status);
	void setFalseActivate (const bool status);
	//void reSetValue();

signals:
	void currentStatus(const bool status);

private:
	class PrivateData;
	PrivateData *d_data;
};
#endif
