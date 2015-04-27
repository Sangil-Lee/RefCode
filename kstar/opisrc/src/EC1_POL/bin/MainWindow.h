#ifndef ECCD_MAIN_H
#define ECCD_MAIN_H

#include <QtGui/QWidget>
#include <signal.h>

#include "ui_EC1_POL_Main.h"
#include "qtchaccesslib.h"

#define STACKED_POLARIZATION	0
#define STACKED_TCMONITORING	1

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots:
	void slot_POLORIZATION_Show(void);
	void slot_TCMONITORING_Show(void);

	void slot_CM_Stat_Change1(bool);
	void slot_CM_Stat_Change2(bool);

	void slot_TLCM_Stat_Change1(bool);
	void slot_TLCM_Stat_Change2(bool);

	void slot_OP_Stat_Change1(bool);
	void slot_OP_Stat_Change2(bool);

	//void slot_Shot_Stat_Change1(bool);
	//void slot_Shot_Stat_Change2(bool);

	//void slot_Shot_Number_Change1(double);

	void slot_L1_Stat_Change(bool);
	void slot_L2_Stat_Change(bool);
	void slot_L3_Stat_Change(bool);
	void slot_L4_Stat_Change(bool);

private:
	void closeEvent(QCloseEvent *event);
	void setpushButton(int);

	void insertStackedWidgetPage();

	Ui::ECCDMAIN ui;

	bool	bEMode, bPMode, bTLRemoteMode, bTLLocalMode, bKstarMode, bDummyMode;	
};

#endif // MainWindow_H
 

 
