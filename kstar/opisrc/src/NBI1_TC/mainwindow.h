#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtchaccesslib.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QWidget *neu11_widget;
    QWidget *neu12_widget;
    QWidget *bls_widget;
    QWidget *bd_widget;
    QWidget *bm_widget;
    QWidget *cal_widget;
    QWidget *baf_widget;
    QWidget *shi_widget;
    QWidget *id_widget;
    QWidget *tile_widget;

    AttachChannelAccess *bls_pattach;
    AttachChannelAccess *bd_pattach;
    AttachChannelAccess *bm_pattach;
    AttachChannelAccess *cal_pattach;
    AttachChannelAccess *baf_pattach;
    AttachChannelAccess *shi_pattach;
    AttachChannelAccess *id_pattach;
    AttachChannelAccess *neu11_pattach;
    AttachChannelAccess *neu12_pattach;
    AttachChannelAccess *tile_pattach;
    AttachChannelAccess *up_pattach;


protected:
    void changeEvent(QEvent *e);
     void closeEvent(QCloseEvent *);


private:
    Ui::MainWindow *ui;

private slots:
    void on_btTCtp_clicked();
    void on_btTCcb_clicked();
    void on_btTCcs_clicked();
    void on_btTCbd_clicked();
    void on_btTCbls_clicked();
    void on_btTCcal_clicked();
    void on_btTCid_clicked();
    void on_btTCbm_clicked();
    void on_btTCne2_clicked();
    void on_btTCne1_clicked();
};

#endif // MAINWINDOW_H
