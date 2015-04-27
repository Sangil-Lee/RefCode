#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtchaccesslib.h"


#define DIBD_TCP_PORT   1470

typedef unsigned char           u08;
typedef unsigned short int      u16;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    AttachChannelAccess *pAttachView;

    int led1_sock;
    int led2_sock;
    int led3_sock;
    int led4_sock;
    int led5_sock;

    int led5_sel;
    int timer_inc;
    int timer_id;

    int set_up_tcp(char *ip_address);
protected:
    virtual void timerEvent(QTimerEvent *e);
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

private slots:
    void on_pushButton_10_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
};

#endif // MAINWINDOW_H
