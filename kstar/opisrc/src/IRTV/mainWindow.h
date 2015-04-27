#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <qwt_plot.h>

#include "qtchaccesslib.h"

#include "ui_DDS_IRTVMain.h"
#include "irtvS1Window.h"
#include "irtvD1Window.h"

#define	STACKED_IRTV_S1	0
#define	STACKED_IRTV_D1	1

class MainWindow : public QMainWindow, public Ui::qtMainWindow
{
    Q_OBJECT
public:
    MainWindow (QWidget *parent = 0);
    ~MainWindow ();

	void printStatus (const char *fmt, ...);	

private:
	void init ();
	void insertStackedWidgetPage ();
	void createActions ();
	void createConnects ();
	void createToolbars ();
	void createStatusBar ();

    void changeEvent(QEvent *e);
    void closeEvent (QCloseEvent *);

	void btnRelease ();

signals:
	void signal_printStatus (char *fmt);
	void signal_updateFilePath ();

private slots:
	void slot_printStatus (char *fmt);

	// ¹öÆ°
    void on_btnIRTV_S1_clicked ();
    void on_btnIRTV_D1_clicked ();

	// Menu
	void showAbout ();				// About

private:
    IRTV_S1Window	*m_pIrtvS1Window;
    IRTV_D1Window	*m_pIrtvD1Window;

};

#endif // MAINWINDOW_H
