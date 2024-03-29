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
};

#endif // MAINWINDOW_H
