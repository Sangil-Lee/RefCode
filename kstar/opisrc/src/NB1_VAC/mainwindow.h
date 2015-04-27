#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <signal.h>

#include "qtchaccesslib.h"
#include "qwt_plot_curve.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    AttachChannelAccess *m_pAttachMain;
    AttachChannelAccess *m_pAttachView;

protected:
    void changeEvent(QEvent *e);
    virtual void timerEvent(QTimerEvent *t);		// 타이머 이벤트 함수

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
