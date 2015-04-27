#ifndef _MULTI_PLOT_MAINWINDOW_H
#define _MULTI_PLOT_MAINWINDOW_H
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <QtGui>
#include <qwt_counter.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_engine.h>
#include "MultiPlot.h"
#include "MultiPlotInfo.h"
class MultiPlot;
class DataModellerThread;
class MultiplotMainWindow:public QMainWindow
{
	Q_OBJECT
public:
    //MultiplotMainWindow(const vector<string> &pvnames);
    MultiplotMainWindow();
    virtual ~MultiplotMainWindow();
	vector<QLabel *> m_pvvalues;
	vector<QCheckBox *> m_pvdisplayers;
public slots:
    void SetLabels(int index, QString pvvalue);
	void CheckDisplayer();
	void Group();
	void Zoom();
	void ChangePen();
	void ZoomOut();
	void ScreenPrint();
	void ColorDialog();
	void ApplyPen();
	void Setup();
	void SaveWindow();
	void OpenWindow();
	void Reset();
	void SetEnableObject(); 
	void SetAutoScale(int);
	void SetLogScale(int check);
	void showValue(const vector<string> &pvnames); 
	void enableZoomMode(bool on);
signals:

private:
	void saveFile(const QString filename);
	void openFile(const QString filename);
	void groupFileLoad(const QString filename);
	void readSetInfo();
	void resetVBoxLayout();
private:
	enum ENUM_PEN{PEN_WIDTH=0, PEN_STYLE, PEN_COLOR};
	MultiPlot *m_plot;
	QwtLinearScaleEngine *mpLinearEngine;
	QwtLog10ScaleEngine	*mpLog10Engine;
	QPushButton *m_pZoom;
	QPushButton *m_pChpen;
	MultiPlotPVInfo m_pvsinfo;
	vector<MultiPlotPVInfo> mvec_pvsinfo;
    QLineEdit *m_lineEdit[MultiPlot::MAX_PV_CNT];
    QCheckBox *m_checkBox[MultiPlot::MAX_PV_CNT];
    QComboBox *m_comboBox[3][MultiPlot::MAX_PV_CNT];
    QSpinBox *m_sbh,*m_sbm,*m_sbs;
	QLabel *mdate;
	//QDoubleSpinBox *mymin, *mymax;
	QLineEdit *mymin, *mymax;
	QWidget *mHbox;
	QVBoxLayout *mVboxLayout;
	//QHBoxLayout *mHboxLayout, *mHboxLayout2, *mHboxLayout3, *mHboxLayout4,mHboxLayout5, *mHboxLayout6;
	//QHBoxLayout *mHboxLayout[10];
	QHBoxLayout *mHboxLayout[4];
    QComboBox *mpenIndexCombo, *mpenWidthCombo, *mpenStyleCombo;
    QPushButton *mcolorButton;
	QCheckBox	*mpAutoscale;
	QString mcolorname;
};
#endif
