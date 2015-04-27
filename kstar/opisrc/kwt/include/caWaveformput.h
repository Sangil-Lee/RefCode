/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/


#ifndef CA_WAVEFORMPUT_H
#define CA_WAVEFORMPUT_H
#include <vector>
#include <string>

#include <QtGui>
#include <qwt_plot.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_scale_draw.h>
#include <qwt_math.h>
#include <qwt_data.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_engine.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_symbol.h>
#include <cadef.h>

using std::vector;
using std::string;

class CAWaveformPut;

class WaveformProperty_Form:public QWidget
{
	Q_OBJECT
public:
    QListWidget *XYValuelist;
    //QSpinBox *XMin;
    //QSpinBox *XMax;
    //QSpinBox *YMin;
    //QSpinBox *YMax;

    QDoubleSpinBox *XMin;
    QDoubleSpinBox *XMax;
    QDoubleSpinBox *YMin;
    QDoubleSpinBox *YMax;

    QPushButton *valueInsert;
    QPushButton *valueApply;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QPushButton *valueDelete;
    QLabel *label_7;
    QSpinBox *Penwidth;
    QSpinBox *Symbolsize;
    QLabel *label_8;
    QLabel *label_9;
    QComboBox *Pencolor;
    QLabel *label_10;
    QComboBox *Symbolcolor;

    QCheckBox *check_1;
    QCheckBox *check_2;

	QWidget *pForm;
	CAWaveformPut *mpWave;

	WaveformProperty_Form(QWidget *pWidget = NULL):QWidget(pWidget, Qt::WindowStaysOnTopHint),singleton(1)
	{
		//CAWaveformPut *mpWave = (CAWaveformPut*)pWidget;
		setupUi();
	};

	~WaveformProperty_Form()
	{
		singleton = 0;
		delete pForm;
	};

    void setupUi()
	{
		QFont font;
		font.setPointSize(10);
		pForm = new QWidget(this);
		pForm->resize(410, 410);
		XYValuelist = new QListWidget(pForm);
		XYValuelist->setObjectName(QString::fromUtf8("XYValuelist"));
		XYValuelist->setGeometry(QRect(10, 180, 256, 192));
		XYValuelist->setFont(font);
		//XYValuelist->setEditTriggers(QAbstractItemView::AllEditTriggers);
		//XMin = new QSpinBox(pForm);
		XMin = new QDoubleSpinBox(pForm);
		XMin->setObjectName(QString::fromUtf8("XMin"));
		XMin->setGeometry(QRect(35, 40, 51, 31));
		XMin->setFont(font);
		XMin->setMinimum(-50);
		XMin->setMaximum(4000);
		XMin->setValue(-20);
		//XMax = new QSpinBox(pForm);
		XMax = new QDoubleSpinBox(pForm);
		XMax->setObjectName(QString::fromUtf8("XMax"));
		XMax->setGeometry(QRect(125, 40, 51, 31));
		XMax->setFont(font);
		XMax->setMinimum(-50);
		XMax->setMaximum(5000);
		XMax->setValue(50);
		valueInsert = new QPushButton(pForm);
		valueInsert->setObjectName(QString::fromUtf8("valueInsert"));
		valueInsert->setGeometry(QRect(270, 220, 80, 27));
		valueApply = new QPushButton(pForm);
		valueApply->setObjectName(QString::fromUtf8("valueApply"));
		valueApply->setGeometry(QRect(270, 290, 80, 27));
		label = new QLabel(pForm);
		label->setObjectName(QString::fromUtf8("label"));
		label->setGeometry(QRect(0, 15, 71, 22));
		label->setFont(font);
		label_2 = new QLabel(pForm);
		label_2->setObjectName(QString::fromUtf8("label_2"));
		label_2->setGeometry(QRect(0, 45, 31, 21));
		label_2->setLayoutDirection(Qt::RightToLeft);
		label_3 = new QLabel(pForm);
		label_3->setObjectName(QString::fromUtf8("label_3"));
		label_3->setGeometry(QRect(90, 45, 31, 21));
		label_3->setLayoutDirection(Qt::RightToLeft);
		label_4 = new QLabel(pForm);
		label_4->setObjectName(QString::fromUtf8("label_4"));
		label_4->setGeometry(QRect(200, 15, 61, 21));
		label_4->setFont(font);
		//YMin = new QSpinBox(pForm);
		YMin = new QDoubleSpinBox(pForm);
		YMin->setObjectName(QString::fromUtf8("YMin"));
		YMin->setGeometry(QRect(245, 40, 51, 31));
		YMin->setFont(font);
		YMin->setMinimum(-1000);
		YMin->setMaximum(7000);
		YMin->setValue(0);
		label_5 = new QLabel(pForm);
		label_5->setObjectName(QString::fromUtf8("label_5"));
		label_5->setGeometry(QRect(300, 45, 31, 21));
		label_5->setLayoutDirection(Qt::RightToLeft);
		label_6 = new QLabel(pForm);
		label_6->setObjectName(QString::fromUtf8("label_6"));
		label_6->setGeometry(QRect(210, 45, 31, 21));
		label_6->setLayoutDirection(Qt::RightToLeft);
		//YMax = new QSpinBox(pForm);
		YMax = new QDoubleSpinBox(pForm);
		YMax->setObjectName(QString::fromUtf8("YMax"));
		YMax->setGeometry(QRect(335, 40, 51, 31));
		YMax->setFont(font);
		YMax->setMinimum(-1000);
		YMax->setMaximum(7000);
		YMax->setValue(5000);
		valueDelete = new QPushButton(pForm);
		valueDelete->setObjectName(QString::fromUtf8("valueDelete"));
		valueDelete->setGeometry(QRect(270, 255, 80, 27));
		label_7 = new QLabel(pForm);
		label_7->setObjectName(QString::fromUtf8("label_7"));
		label_7->setGeometry(QRect(20, 95, 61, 21));
		label_7->setLayoutDirection(Qt::RightToLeft);
		Penwidth = new QSpinBox(pForm);
		Penwidth->setObjectName(QString::fromUtf8("Penwidth"));
		Penwidth->setGeometry(QRect(90, 90, 71, 31));
		Penwidth->setFont(font);
		Symbolsize = new QSpinBox(pForm);
		Symbolsize->setObjectName(QString::fromUtf8("Symbolsize"));
		Symbolsize->setFont(font);
		Symbolsize->setGeometry(QRect(295, 90, 71, 31));
		label_8 = new QLabel(pForm);
		label_8->setObjectName(QString::fromUtf8("label_8"));
		label_8->setGeometry(QRect(210, 95, 76, 21));
		label_8->setLayoutDirection(Qt::RightToLeft);

		label_9 = new QLabel(pForm);
		label_9->setObjectName(QString::fromUtf8("label_9"));
		label_9->setGeometry(QRect(20, 140, 61, 21));
		label_9->setLayoutDirection(Qt::RightToLeft);

		Pencolor = new QComboBox(pForm);
		Pencolor->setObjectName(QString::fromUtf8("Pencolor"));
		Pencolor->setFont(font);
		Pencolor->setGeometry(QRect(90, 140, 71, 26));

		label_10 = new QLabel(pForm);
		label_10->setObjectName(QString::fromUtf8("label_10"));
		label_10->setGeometry(QRect(210, 140, 76, 21));
		label_10->setLayoutDirection(Qt::RightToLeft);

		Symbolcolor = new QComboBox(pForm);
		Symbolcolor->setObjectName(QString::fromUtf8("Symbolcolor"));
		Symbolcolor->setGeometry(QRect(295, 140, 71, 26));
		Symbolcolor->setFont(font);

		check_1 = new QCheckBox(pForm);
		check_1 ->setObjectName(QString::fromUtf8("check_1"));
		check_1 ->setGeometry(QRect(20, 180, 80, 25));
		check_1 ->setCheckState(Qt::Checked);

		check_2 = new QCheckBox(pForm);
		check_2 ->setObjectName(QString::fromUtf8("check_2"));
		check_2 ->setGeometry(QRect(200, 180, 80, 25));
		check_2 ->setCheckState(Qt::Checked);

		XYValuelist->setGeometry(QRect(10, 220, 256, 192));

		retranslateUi(pForm);
		ConnectSet();
		show();

	} // setupUi

	void ConnectSet()
	{
		connect(Penwidth, SIGNAL(valueChanged(int)), this, SLOT(PenWidth(int)));
		connect(Pencolor, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(PenColor(const QString &)));
		connect(XYValuelist, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(ListItemChange( QListWidgetItem *)));

		connect(XYValuelist, SIGNAL(currentRowChanged(int)), this, SLOT(CurveItemChanged(int)));

		connect(valueInsert, SIGNAL(clicked()), this, SLOT(ItemInsert()));
		connect(valueApply, SIGNAL(clicked()), this, SLOT(ItemApply()));
		connect(valueDelete, SIGNAL(clicked()), this, SLOT(ItemDelete()));
		connect(Symbolsize, SIGNAL(valueChanged(int)), this, SLOT(SymbolSize(int)));
		connect(Symbolcolor, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(SymbolColor(const QString &)));
		connect(XMax, SIGNAL(valueChanged(double)), this, SLOT(XMaxValue(double)));
		connect(XMin, SIGNAL(valueChanged(double)), this, SLOT(XMinValue(double)));
		connect(YMax, SIGNAL(valueChanged(double)), this, SLOT(YMaxValue(double)));
		connect(YMin, SIGNAL(valueChanged(double)), this, SLOT(YMinValue(double)));

		connect(check_1, SIGNAL(stateChanged(int)), this, SLOT(XNearby(int)));
		connect(check_2, SIGNAL(stateChanged(int)), this, SLOT(YNearby(int)));
	};

    void retranslateUi(QWidget *Form)
	{
		Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
		valueInsert->setText(QApplication::translate("Form", "Insert", 0, QApplication::UnicodeUTF8));
		valueApply->setText(QApplication::translate("Form", "Apply", 0, QApplication::UnicodeUTF8));
		label->setText(QApplication::translate("Form", "X-Range", 0, QApplication::UnicodeUTF8));
		label_2->setText(QApplication::translate("Form", "Min", 0, QApplication::UnicodeUTF8));
		label_3->setText(QApplication::translate("Form", "Max", 0, QApplication::UnicodeUTF8));
		label_4->setText(QApplication::translate("Form", "Y-Range", 0, QApplication::UnicodeUTF8));
		label_5->setText(QApplication::translate("Form", "Max", 0, QApplication::UnicodeUTF8));
		label_6->setText(QApplication::translate("Form", "Min", 0, QApplication::UnicodeUTF8));
		valueDelete->setText(QApplication::translate("Form", "Delete", 0, QApplication::UnicodeUTF8));
		label_7->setText(QApplication::translate("Form", "Line Width", 0, QApplication::UnicodeUTF8));
		label_8->setText(QApplication::translate("Form", "Symbol Size", 0, QApplication::UnicodeUTF8));
		label_9->setText(QApplication::translate("Form", "Line Color", 0, QApplication::UnicodeUTF8));

		check_1->setText(QApplication::translate("Form", "X-Nearby", 0, QApplication::UnicodeUTF8));
		check_2->setText(QApplication::translate("Form", "Y-Nearby", 0, QApplication::UnicodeUTF8));

		Pencolor->clear();
		Pencolor->insertItems(0, QStringList()
				<< QApplication::translate("Form", "Blue", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Red", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Yellow", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Black", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "White", 0, QApplication::UnicodeUTF8)
				);
		label_10->setText(QApplication::translate("Form", "Symbol Color", 0, QApplication::UnicodeUTF8));
		Symbolcolor->clear();
		Symbolcolor->insertItems(0, QStringList()
				<< QApplication::translate("Form", "Blue", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Red", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Yellow", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "Black", 0, QApplication::UnicodeUTF8)
				<< QApplication::translate("Form", "White", 0, QApplication::UnicodeUTF8)
				);
		Q_UNUSED(Form);
	} // retranslateUi
	int GetPropertyWindow(){return singleton;};

	//declare
	void SetParent(CAWaveformPut *pwave);
	void XYValueList();
	void XYValueItemUpdate(const int index,QwtArray<double> &xData, QwtArray<double> &yData);
	void DeleteForm();

public slots:
	void PenWidth(int width);
	void PenColor(const QString & color);
	void SymbolColor(const QString & color);
	void SymbolSize(int size);
	void ListItemChange( QListWidgetItem *);
	void ItemInsert();
	void ItemApply();
	void ItemDelete();
	void XMaxValue(double value);
	void XMinValue(double value);
	void YMaxValue(double value);
	void YMinValue(double value);

	void XNearby(int value);
	void YNearby(int value);

	void CurveItemChanged(int);

protected:
	virtual void closeEvent( QCloseEvent * /*event*/)
	{
		qDebug("*Close Window*");
		DeleteForm();
		delete pForm;
	};
	int getStrToValue(QString strvalue, double &xval, double &yval);
	void reSetSymbol();
private:
	int singleton;
};

#if 1
namespace WaveForm {

class CAWaveformPut;
class WaveformProperty : public WaveformProperty_Form 
{
public:
	WaveformProperty(QWidget *pWidget = NULL):WaveformProperty_Form(pWidget){};
#if 0
	protected:
		virtual void closeEvent( QCloseEvent *event)
		{
			qDebug("Close Event**********");
		};
#endif
};
}; // namespace Ui
#endif

class CAWaveformPut : public QwtPlot
{
	Q_OBJECT
	Q_ENUMS(ControlMode)
	Q_PROPERTY(ControlMode mode READ getMode WRITE setMode);

	Q_PROPERTY(QString ywavepvname READ getYPvname WRITE setYPvname);
	Q_PROPERTY(QString xwavepvname READ getXPvname WRITE setXPvname);

	Q_PROPERTY(QStringList xpvlist READ getXPVList WRITE setXPVList);
	Q_PROPERTY(QStringList ypvlist READ getYPVList WRITE setYPVList);

	Q_PROPERTY(QString countpv READ getCountpv WRITE setCountpv);

    Q_PROPERTY(QColor canvasColor READ getCanvascolor WRITE setCanvascolor); 

    Q_PROPERTY(QColor majLinecolor READ getMajorlinecolor WRITE setMajorlinecolor); 
    Q_PROPERTY(QColor minLinecolor READ getMinorlinecolor WRITE setMinorlinecolor); 

	Q_PROPERTY(QString plotTitle READ getPlottitle WRITE setPlottitle);
	Q_PROPERTY(QString xTitle READ getXtitle WRITE setXtitle);
	Q_PROPERTY(QString yTitle READ getYtitle WRITE setYtitle);

	// Pen proprety
	Q_PROPERTY(int penwidth READ getPenWidth WRITE setPenWidth);
    Q_PROPERTY(QColor pencolor READ getPenColor WRITE setPenColor); 

	// Symbol proprety
	Q_PROPERTY(int symbolsize READ getSymbolSize WRITE setSymbolSize);
    Q_PROPERTY(QColor symbolcolor READ getSymbolColor WRITE setSymbolColor); 

	// Range property
	Q_PROPERTY(double xrangemin READ getXRangeMin WRITE setXRangeMin);
	Q_PROPERTY(double xrangemax READ getXRangeMax WRITE setXRangeMax);

	Q_PROPERTY(double yrangemin READ getYRangeMin WRITE setYRangeMin);
	Q_PROPERTY(double yrangemax READ getYRangeMax WRITE setYRangeMax);

	Q_PROPERTY(bool xnearby READ getXNearby WRITE setXNearby);
	Q_PROPERTY(bool ynearby READ getYNearby WRITE setYNearby);

	Q_PROPERTY(QString filename READ getFilename WRITE setFilename);


public:
	explicit CAWaveformPut(QWidget *parent = NULL);
	virtual ~CAWaveformPut();

	enum ControlMode {Control=0, Monitor};

	void ResetChild() {pWavewindow = NULL;};

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

	void initWaveformPlot();

	ControlMode getMode() const;
	void setMode(ControlMode mode);

	const QString &getYPvname() const;
	void  setYPvname(const QString &pvname);

	const QString &getXPvname() const;
	void  setXPvname(const QString &pvname);

	const QStringList getXPVList() const;
	void  setXPVList(const QStringList pvlist);

	const QStringList getYPVList() const;
	void  setYPVList(const QStringList pvlist);

	const QString &getCountpv() const;
	void  setCountpv(const QString &pvname);

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

	//Pen property
	const int getPenWidth();
	void setPenWidth (const int penwidth);

	const QColor &getPenColor();
	void setPenColor(const QColor &color);

	//Symbol property
	const int getSymbolSize();
	void setSymbolSize (const int penwidth);

	const QColor &getSymbolColor();
	void setSymbolColor(const QColor &color);

	//Range Property
	const double getXRangeMin() const;
	void setXRangeMin(const double min);

	const double getXRangeMax() const;
	void setXRangeMax(const double max);

	const double getYRangeMin() const;
	void setYRangeMin(const double min);

	const double getYRangeMax() const;
	void setYRangeMax(const double max);

	const bool getXNearby() const;
	void setXNearby(const bool );

	const bool getYNearby() const;
	void setYNearby(const bool );

	const QString &getFilename() const;
	void  setFilename(const QString &filename);

	QwtPlotCurve *curve();
	void SetData(QwtArray<double> &xData, QwtArray<double> &yData );
	void RemovePoint(const int index);
	void InsertPoint(const QPoint &pos);
	double TruncVal(double orgvalue,  int n);
	void MovePoint(const QPoint &pos);
	void MoveValue(const double xval, const double yval);
	void ShowProperty(const QPoint &pos);
	void GetXYData(QwtArray<double>& xdata, QwtArray<double>& ydata);
	void SetSelectedPoint(const int index);
	int DataCheck();
	const int TransformX(const int dx);
	const int TransformY(const int dy);
	double getSelectedPointYValue();
	double getSelectedPointXValue();
	int getXFactor();
	int getYFactor();
	void SetWavePVCount(const short value);
	short GetWavePVCount();
	void SetYData(const double &dvalue);
	void SetXData(const double &dvalue);
	void SetYWaveData(const int count, const double *dvalue);
	void SetXWaveData(const int count, const double *dvalue);
	void Update();
	void Update(const int count);
	void Update(const QString nameindex, const int xytype);
	void ReSet();

signals:
	void updatesignal(const int count);
	void updatesignal(const QString nameindex, const int xytype);
	void resetsignal();
	void valuesignal(const double value);

public slots:
	void penIndex(const int index);
	void penWidth(const int width);
	void penStyle(const QString strstyle);
	void penColor(const QString color);

	void caXYArrayPut();
	void caXYArrayGet();
	void shotNumber(const int shotno);
	void savePattern();
	void openPattern();
	void saveAsPattern();
	void openAsPattern();
	void MonitorOnIP();
	void MonitorOn();
	void UpdateSlot(const int count);
	void UpdateSlot(const QString nameindex, const int xytype);
	void reSet();
	//void openPattern(const QString file);

protected:
	virtual void timerEvent (QTimerEvent * event);

private:
#if 0
	virtual bool eventFilter(QObject *, QEvent *);
	void move(const QPoint &pos);
	void select(const QPoint &pos);
#endif
	void plotupdate();
	void printdata();
	double caGet (const QString pvname);
	int caArrayPut (const int type, int count = 0);
	int caArrayGet (const int type, int &count);
	void setXRange(const double min, const double max);
	void setYRange(const double min, const double max);
	double getDistance(const QPoint &pos);
	void reSetSymbol();
	void caListPut();
	int caPut(const QString pvname, const double value);
	void movePointBy(int dx, int dy);
	int checkXList(int & index);

private:
	enum Type{XAXIS = 0, YAXIS};
	//data member
	class PrivateData;
	PrivateData *d_data;
	int mUpdate;
	WaveForm::WaveformProperty *pWavewindow;
};
#endif
