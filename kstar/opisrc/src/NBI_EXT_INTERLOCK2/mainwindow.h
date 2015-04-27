#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtchaccesslib.h"
#include <signal.h>

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
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    AttachChannelAccess *main_pattach;
    AttachChannelAccess *static_pattach;

    QWidget *main_widget;


private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void on_btn_calAlmIS1_clicked();
    void on_btn_calAlmIS2_clicked();
    void on_btn_calAlmCommon_clicked();
};

#endif // MAINWINDOW_H
