/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/


#ifndef CA_MULTIPLOTWAVE_H
#define CA_MULTIPLOTWAVE_H
#include <vector>
#include <string>

#include <QtGui>
#include <QApplication>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QListWidgetItem>
#include <QColorDialog>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_legend_label.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <cadef.h>
#include "caArrayThread.h"

using std::vector;
using std::string;

class CAMultiwaveplot;
class SetProperty_Form:public QWidget
{
	Q_OBJECT

public:
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *XMinSB;
    QDoubleSpinBox *XMaxSB;
    QDoubleSpinBox *YMinSB;
    QDoubleSpinBox *YMaxSB;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QListWidget *WavePVList;
    QLabel *label_7;
    QSpinBox *LineWidthSB;
    QLabel *label_8;
    QLabel *label_9;
    QPushButton *WaveColorPB;
    QPushButton *ShowHidePB;
    QLabel *label_10;

	QWidget *pForm;

	SetProperty_Form(QWidget *pWidget = NULL):QWidget(pWidget, Qt::WindowStaysOnTopHint),singleton(1)
	{
		setupUi();
	}
	~SetProperty_Form()
	{
		singleton = 0;
		delete pForm;
	}

	void setupUi()
	{
		pForm = new QWidget(this);
		pForm->resize(455, 419);
		label = new QLabel(pForm);
		label->setObjectName(QString::fromUtf8("label"));
		label->setGeometry(QRect(10, 10, 71, 22));
		QFont font;
		font.setPointSize(12);
		label->setFont(font);
		label_2 = new QLabel(pForm);
		label_2->setObjectName(QString::fromUtf8("label_2"));
		label_2->setGeometry(QRect(240, 10, 71, 22));
		label_2->setFont(font);
		XMinSB = new QDoubleSpinBox(pForm);
		XMinSB->setObjectName(QString::fromUtf8("XMinSB"));
		XMinSB->setGeometry(QRect(50, 37, 62, 27));
		XMinSB->setMinimum(-30);
		XMinSB->setMaximum(100000);
		XMinSB->setValue(0);
		label_3 = new QLabel(pForm);
		label_3->setObjectName(QString::fromUtf8("label_3"));
		label_3->setGeometry(QRect(28, 40, 20, 21));
		QFont font1;
		font1.setPointSize(10);
		label_3->setFont(font1);
		YMinSB = new QDoubleSpinBox(pForm);
		YMinSB->setObjectName(QString::fromUtf8("YMinSB"));
		YMinSB->setGeometry(QRect(280, 37, 62, 27));
		YMinSB->setMinimum(-1e+06);
		YMinSB->setMaximum(1e+06);
		YMinSB->setValue(-15e+3);
		label_4 = new QLabel(pForm);
		label_4->setObjectName(QString::fromUtf8("label_4"));
		label_4->setGeometry(QRect(255, 40, 21, 21));
		label_4->setFont(font1);
		label_5 = new QLabel(pForm);
		label_5->setObjectName(QString::fromUtf8("label_5"));
		label_5->setGeometry(QRect(353, 40, 25, 21));
		label_5->setFont(font1);
		YMaxSB = new QDoubleSpinBox(pForm);
		YMaxSB->setObjectName(QString::fromUtf8("YMaxSB"));
		YMaxSB->setGeometry(QRect(380, 37, 62, 27));
		YMaxSB->setMinimum(-1e+06);
		YMaxSB->setMaximum(1e+06);
		YMaxSB->setValue(15e+03);
		XMaxSB = new QDoubleSpinBox(pForm);
		XMaxSB->setObjectName(QString::fromUtf8("XMaxSB"));
		XMaxSB->setGeometry(QRect(150, 37, 62, 27));
		XMaxSB->setMinimum(0);
		XMaxSB->setMaximum(100000);
		XMaxSB->setValue(25e+3);
		label_6 = new QLabel(pForm);
		label_6->setObjectName(QString::fromUtf8("label_6"));
		label_6->setGeometry(QRect(124, 40, 25, 21));
		label_6->setFont(font1);
		WavePVList = new QListWidget(pForm);
		WavePVList->setObjectName(QString::fromUtf8("WavePVList"));
		WavePVList->setGeometry(QRect(10, 120, 261, 291));
		QFont font2;
		font2.setPointSize(9);
		WavePVList->setFont(font2);
		label_7 = new QLabel(pForm);
		label_7->setObjectName(QString::fromUtf8("label_7"));
		label_7->setGeometry(QRect(290, 130, 71, 21));
		label_7->setFont(font1);
		LineWidthSB = new QSpinBox(pForm);
		LineWidthSB->setObjectName(QString::fromUtf8("LineWidthSB"));
		LineWidthSB->setGeometry(QRect(370, 130, 51, 27));
		LineWidthSB->setMinimum(1);
		LineWidthSB->setMaximum(10);
		label_8 = new QLabel(pForm);
		label_8->setObjectName(QString::fromUtf8("label_8"));
		label_8->setGeometry(QRect(290, 185, 71, 21));
		label_8->setFont(font1);
		label_9 = new QLabel(pForm);
		label_9->setObjectName(QString::fromUtf8("label_9"));
		label_9->setGeometry(QRect(290, 240, 71, 21));
		label_9->setFont(font1);
		WaveColorPB = new QPushButton(pForm);
		WaveColorPB->setObjectName(QString::fromUtf8("WaveColorPB"));
		WaveColorPB->setGeometry(QRect(370, 185, 80, 27));
		ShowHidePB = new QPushButton(pForm);
		ShowHidePB->setObjectName(QString::fromUtf8("ShowHidePB"));
		ShowHidePB->setGeometry(QRect(370, 240, 80, 27));
		ShowHidePB->setCheckable(true);
		ShowHidePB->setChecked(true);
		label_10 = new QLabel(pForm);
		label_10->setObjectName(QString::fromUtf8("label_10"));
		label_10->setGeometry(QRect(10, 90, 111, 22));
		label_10->setFont(font);
		retranslateUi(pForm);
		ConnectSet();
		show();

	} // setupUi

	void ConnectSet();

	void retranslateUi(QWidget *Form)
	{
		Form->setWindowTitle("Form");
		label->setText("X-Range");
		label_2->setText("Y-Range");
		label_3->setText("Min");
		label_4->setText("Min");
		label_5->setText("Max");
		label_6->setText("Max");
		label_7->setText("Line Width");
		label_8->setText("Line Color");
		label_9->setText("Hide/Show");
		WaveColorPB->setText("Color");
		ShowHidePB->setText("Hide");
		label_10->setText("Wave PV List");
		Q_UNUSED(Form);
	} // retranslateUi

	void SetParent(CAMultiwaveplot *pPlot);
	void DeleteForm();
	void SetPVNames(vector<string>& pvnames); //{ mvecpvnames=pvnames;};

protected:
	virtual void closeEvent( QCloseEvent * /*event*/)
	{
		DeleteForm();
		delete pForm;
	};
public slots:
	void XMaxValue(double value);
	void XMinValue(double value);
	void YMaxValue(double value);
	void YMinValue(double value);
	void PenColor(bool);
	void PenWidth(int);
	void PenShowHide(bool);
	void ListItemInfo(QListWidgetItem *);


private:
	int singleton;
	CAMultiwaveplot *pWaveplot;
	//vector<string> mvecpvnames;
	void showPVList();
};

namespace WaveForm {
class CAMultiwaveplot;

class SetProperty : public SetProperty_Form 
{
public:
	SetProperty(QWidget *pWidget = NULL):SetProperty_Form(pWidget) 
	{
	};
};
}; // namespace Ui


class QtCAArrayDataThread;
class CAMultiwaveplot : public QwtPlot
{
	Q_OBJECT
	Q_ENUMS(TypeOfCall)
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString pvname1 READ getPvname1 WRITE setPvname1);
	Q_PROPERTY(QString pvname2 READ getPvname2 WRITE setPvname2);
	Q_PROPERTY(QString pvname3 READ getPvname3 WRITE setPvname3);
	Q_PROPERTY(QString pvname4 READ getPvname4 WRITE setPvname4);
	Q_PROPERTY(QString pvname5 READ getPvname5 WRITE setPvname5);
	Q_PROPERTY(QString pvname6 READ getPvname6 WRITE setPvname6);
	Q_PROPERTY(QString pvname7 READ getPvname7 WRITE setPvname7);
	Q_PROPERTY(QString pvname8 READ getPvname8 WRITE setPvname8);
	Q_PROPERTY(QString pvname9 READ getPvname9 WRITE setPvname9);
	Q_PROPERTY(QString pvname10 READ getPvname10 WRITE setPvname10);
	Q_PROPERTY(QString pvname11 READ getPvname11 WRITE setPvname11);
	Q_PROPERTY(QString pvname12 READ getPvname12 WRITE setPvname12);
	Q_PROPERTY(QString pvname13 READ getPvname13 WRITE setPvname13);
	Q_PROPERTY(QString pvname14 READ getPvname14 WRITE setPvname14);
	
    Q_PROPERTY(QColor canvasColor READ getCanvascolor WRITE setCanvascolor); 

    Q_PROPERTY(QColor majLinecolor READ getMajorlinecolor WRITE setMajorlinecolor); 
    Q_PROPERTY(QColor minLinecolor READ getMinorlinecolor WRITE setMinorlinecolor); 

	Q_PROPERTY(QString plotTitle READ getPlottitle WRITE setPlottitle);
	Q_PROPERTY(QString xTitle READ getXtitle WRITE setXtitle);
	Q_PROPERTY(QString yTitle READ getYtitle WRITE setYtitle);

	Q_PROPERTY(bool logScale READ getLogscale WRITE setLogscale);
	Q_PROPERTY(int reqElement READ getREQNOE WRITE setREQNOE);
	Q_PROPERTY(int updateTime READ getUpdateTime WRITE setUpdateTime);
	Q_PROPERTY(int refreshTime READ getRefreshTime WRITE setRefreshTime);
	Q_PROPERTY(int bufferPage READ getBufferPage WRITE setBufferPage);
	Q_PROPERTY(TypeOfCall type READ getType WRITE setType);

public:
	explicit CAMultiwaveplot(QWidget *parent = NULL);
	virtual ~CAMultiwaveplot();
	enum TypeOfCall {
		CAPendIO,
		CAPendEvent
	};


	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initMultiPlot();

	const QString &getPvname() const;
	void  setPvname(const QString &pvname);

	const QString &getPvname1() const;
	void  setPvname1(const QString &pvname);

	const QString &getPvname2() const;
	void  setPvname2(const QString &pvname);

	const QString &getPvname3() const;
	void  setPvname3(const QString &pvname);

	const QString &getPvname4() const;
	void  setPvname4(const QString &pvname);

	const QString &getPvname5() const;
	void  setPvname5(const QString &pvname);

	const QString &getPvname6() const;
	void  setPvname6(const QString &pvname);

	const QString &getPvname7() const;
	void  setPvname7(const QString &pvname);

	const QString &getPvname8() const;
	void  setPvname8(const QString &pvname);

	const QString &getPvname9() const;
	void  setPvname9(const QString &pvname);

	const QString &getPvname10() const;
	void  setPvname10(const QString &pvname);

	const QString &getPvname11() const;
	void  setPvname11(const QString &pvname);

	const QString &getPvname12() const;
	void  setPvname12(const QString &pvname);

	const QString &getPvname13() const;
	void  setPvname13(const QString &pvname);

	const QString &getPvname14() const;
	void  setPvname14(const QString &pvname);

	const bool &getLogscale() const;
	void  setLogscale(const bool setlog);

	const QColor &getCanvascolor();
	void setCanvascolor(const QColor &color);

	const QString &getPlottitle() const;
	void  setPlottitle(const QString &title);

	const QString &getXtitle() const;
	void  setXtitle(const QString &title);

	const QString &getYtitle() const;
	void  setYtitle(const QString &title);

	const QColor &getMajorlinecolor();
	void setMajorlinecolor(const QColor &color);

	const QColor &getMinorlinecolor();
	void setMinorlinecolor(const QColor &color);

	const int &getREQNOE();
	void setREQNOE(const int &reqelement);

	const int &getUpdateTime();
	void setUpdateTime(const int &msec);

	const int &getRefreshTime();
	void setRefreshTime(const int &msec);

	const int &getBufferPage();
	void setBufferPage(const int &pagecnt);

	TypeOfCall getType() const;
	void setType(TypeOfCall type);


	void PrintPvNames ();
	void Run();
	void Start();
	void Stop();
	void SetDataPtr(const int index, void *data);
	void SetData();
	void ResetChild();

	void setXRangeMax(const double max);
	void setXRangeMin(const double min);
	void setYRangeMax(const double max);
	void setYRangeMin(const double min);
	QString getColorName(const int index);
	const int getPenWidth(const int index);
	const bool getDisplayCurve(const int index);
	void IndexLabelColorChange(int index, QString colorname);
signals:
	void changeIndexColor(int index, QString colorname);

public slots:
	void changePvName (const int index, const QString &pvname);
	//void displayCurve(QwtPlotItem *item, bool on);
	void setYRange(const double min, const double max);
	void displayCurve(const bool on, const QString &pvname);
	void displayCurve(const bool show);

	void penIndex(const int index);
	void penWidth(const int width);
	void penStyle(const QString strstyle);
	void penColor(const QString color);
	void penColor(const QColor &color);
	void factor(const QString strfactor);
	void nonfactors();
	void userUpdate();
	void ShowProperty();

private:
	virtual void timerEvent(QTimerEvent *);
	bool eventFilter(QObject *o, QEvent *e);
	void getPvNames ();
	QString getTime(const int &time);
	void setDamp(double damping);
	void plotupdate();

private:
	//data member
	class PrivateData;
	PrivateData *d_data;
	QtCAArrayDataThread *mcaarraydatathread;
	//WaveForm::SetProperty *pProperty;
};

#endif
