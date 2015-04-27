#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <signal.h>
#include "qtchaccesslib.h"
#include "caMultiplot.h"

#include "panel_rfcon_klystron.h"
#include "panel_oper.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;

    void makeUI();
    void timerDone();
    void on_up_pannel_load();

    AttachChannelAccess *a_pattach;
    AttachChannelAccess *powersupply_pattach;
    AttachChannelAccess *c_pattach;
    AttachChannelAccess *up_pattach;
    AttachChannelAccess *interlock_pattach;
    AttachChannelAccess *vswr_pattach;
    AttachChannelAccess *graph_pattach;
    AttachChannelAccess *graph2_pattach;
    AttachChannelAccess *ltudaq_pattach;
    AttachChannelAccess *calorimetric_pattach;
    AttachChannelAccess *monitor_pattach;
    AttachChannelAccess *klystron_pattach;
    AttachChannelAccess *watercooling_pattach;
    AttachChannelAccess *tc_pattach;
    AttachChannelAccess *graph_pattach_popup;
    AttachChannelAccess *graph2_pattach_popup;
    AttachChannelAccess *antenna_pattach;
    AttachChannelAccess *mdsnode_pattach;
    AttachChannelAccess *oper_pattach;

    panel_rfcon_klystron *klystron;
    panel_oper *oper;

   // QWidget *centralwidget;
    CAMultiplot *a;
    QWidget *a_widget;
    QWidget *powersupply_widget;
    QWidget *c_widget;
    QWidget *up_widget;
    QWidget *interlock_widget;
    QWidget *vswr_widget;
    QWidget *graph_widget;
    QWidget *ltudaq_widget;
    QWidget *calorimetric_widget;
    QWidget *graph2_widget;
    QWidget *monitor_widget;
    QWidget *klystron_widget;
    QWidget *watercooling_widget;
    QWidget *tc_widget;
    QWidget *antenna_widget;
    QWidget *mdsnode_widget;
    QWidget *oper_widget;

    QMainWindow *m_klystrongraph;
    QMainWindow *m_antennagraph;

    QTimer *m_pTimer;

     void closeEvent(QCloseEvent *);

private slots:
    void on_pushButton_12_clicked();
    //void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_11_clicked();
    //void on_pushButton_10_clicked();
    //void on_pushButton_9_clicked();
    //void on_pushButton_8_clicked();
    void on_pushButton_4_clicked();
    //void on_pushButton_7_clicked();
    //void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_1_clicked();
     void OnTimeOut();
};

#endif // MAINWINDOW_H
