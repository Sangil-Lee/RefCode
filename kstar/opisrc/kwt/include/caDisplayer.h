/*!\class CADisplayer
 * 
 *\details CADisplayer gets analog data from EPICS and displays with specified style.
 *			Pvname property is the most important to communicate with EPICS.
 *			Number format to be displayed changes configuring NuFormat property. 
 *			Apperance changes configuring fillColor, lineWidth & lineColor, Unit. 
 *			If the operator right-click CADisplayer, it pop-ups single plot.
 *			
 *			CADisplayer는 EPICS로부터 아날로그 데이터를 수신하여 정의된 스타일로 보여준다.
 *			EPICS와 통신을 하기 위해서는 pvname 속성이 정확히 입력되어야 한다.
 *			또한, NuFormat 속성을 설정함에 따라 소수점 이하 표시 자리수를 지정하거나 지수로 표시할 수 있다.  
 *			배경색은 EPICS Channel의 알람 상태에 따라 빨간색(주알람), 노란색(부알람), 녹색(정상, 알람없음), 회색(연결안됨 또는 비정상 비정상)
 *			으로 표시하며 CADisplayer 인스턴스 위에 마우스를 올려놓으면 상태표시줄에 pvname이 보이는 기능이 있다. 
 *			
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GPL License, Version 2.0.
 *
 *\author Sang Il Lee, 
 *		  Sulhee Baek
 *\date 2007-09-20
 *\brief CADisplayer displays analog data.
 */

// vim: expandtab

#ifndef CA_DISPLAYER_H
#define CA_DISPLAYER_H

#include <QtGui>
#include "cadef.h"
#include "qwt_global.h"

#define NO_ALARM		0x0
#define	MINOR_ALARM		0x1
#define	MAJOR_ALARM		0x2
#define INVALID_ALARM	0x3
#define ALARM_NSEV	INVALID_ALARM+1

class QWT_EXPORT CADisplayer : public QWidget
{
	Q_OBJECT
	Q_ENUMS(NumeticFormat);
	Q_ENUMS(ePeriodic);
	Q_PROPERTY(NumeticFormat nuformat READ getNuFormat WRITE setNuFormat);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);
	Q_PROPERTY(bool editable READ getEditable WRITE setEditable);
	Q_PROPERTY(bool showlabel READ getShowLabel WRITE setShowLabel);
    Q_PROPERTY(int lineWidth READ getLinewidth WRITE setLinewidth); 
	Q_PROPERTY(QColor framecol READ getFrameColor WRITE setFrameColor);
	Q_PROPERTY(QColor fillcol READ getFillColor WRITE setFillColor);
	Q_PROPERTY(QString unit READ getUnit WRITE setUnit);
	Q_PROPERTY(ePeriodic periodicdata READ getPeriodic WRITE setPeriodic);

public:
	explicit CADisplayer(QWidget *parent = NULL);
	virtual ~CADisplayer();

	enum NumeticFormat {
		Temperature = 0x0000,
		Hall = 0x0001,
		Strain = 0x0002,
		Displace = 0x0003,
		Pressure = 0x0004,
		RGA = 0x0005,
		Integer = 0x0006,
		Flowrate = 0x0007,
		RealOne = 0x0011,
		RealTwo = 0x0012,
		RealThree = 0x0013,
		RealFour = 0x0014,
		RealFive = 0x0015,
		RealSix = 0x0016
	};

	enum ePeriodic {
		PointOne	= 0x0000,
		PointFive	= 0x0001,
		OneSec		= 0x0002,
		FiveSec		= 0x0003,
		TenSec		= 0x0004
	};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	NumeticFormat getNuFormat() const;
	void setNuFormat(NumeticFormat nuformat);

	bool getEditable() const;
	void setEditable(bool);
	bool getShowLabel() const;
	void setShowLabel(bool);
	void initDisplayer();

	const QString &getPvname() const;
	const QString &getPrefix() const;
	void  setPvname(const QString &pvname);
	void  setPrefix(const QString &prefix);

	const QString &getUnit() const;
	void  setUnit(const QString &unit);
	const QColor & getFrameColor()const;
	void  setFrameColor(const QColor &framecol);
	const QColor & getFillColor()const;
	void  setFillColor(const QColor &framecol);
    const int getLinewidth();
    void setLinewidth(const int line);
	ePeriodic getPeriodic();
	void setPeriodic(ePeriodic periodic);

	// a set of dummies to help the designer
protected:

signals:
	//const QString & valueChanged (const QString &stringvalue);
	//void valueChanged (const QString &stringvalue);
	void valueChanged (const double &value);
public slots:
	virtual void changeValue (const short &connstatus, const double &value, const short& severity, const short& precision );
	void changePvName (const QString &pvname);
	void changePvUnit (const QString &pvunit);
	void DisplayObjectFrame(const int type, QString objname);

private:
	QString m_pvname;
	QString m_prefix;
	QString m_unit;
	class PrivateData;
	PrivateData *d_data;
};
#endif
