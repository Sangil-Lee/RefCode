#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qtchaccesslib.h"
#include <signal.h>
#include <QTimer>


//#include "panel_maindispform.h"
#include "panel_mainopiform.h"
#include "panel_cssetform.h"
#include "panel_pfopiform.h"
#include "panel_pfsetform.h"
#include "panel_tfdispform.h"
#include "panel_tfopiform.h"
#include "panel_tfsetform.h"



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

    //panel_maindispform *maindispform;
   AttachChannelAccess *main_pattach;
    AttachChannelAccess *maindispform_pattach;
    AttachChannelAccess *csdispform_pattach;
    AttachChannelAccess *pfdispform_pattach;
    AttachChannelAccess *tftestform_pattach;
    AttachChannelAccess *cstestform_pattach;
    AttachChannelAccess *pftestform_pattach;
    AttachChannelAccess *pfsetnewpvform_pattach;
    AttachChannelAccess *cssetnewpvform_pattach;
    AttachChannelAccess *tfsetnewpvform_pattach;
    AttachChannelAccess *tfdispnewpvform_pattach;
    AttachChannelAccess *csgrpform_pattach;
    AttachChannelAccess *pfgrpform_pattach;
    AttachChannelAccess *tfgrpform_pattach;

    QWidget *maindispform_widget;
    QWidget *csdispform_widget;
    QWidget *pfdispform_widget;
    QWidget *tftestform_widget;
    QWidget *cstestform_widget;
    QWidget *pftestform_widget;
    QWidget *pfsetnewpvform_widget;
    QWidget *cssetnewpvform_widget;
    QWidget *tfsetnewpvform_widget;
    QWidget *tfdispnewpvform_widget;
    QMainWindow *csgrpform_widget;
    QMainWindow *pfgrpform_widget;
    QMainWindow *tfgrpform_widget;


    void TitleClear();

    panel_mainopiform *mainopiform;
    panel_cssetform *cssetform;
    panel_pfopiform *pfopiform;
    panel_pfsetform *pfsetform;
    panel_tfdispform *tfdispform;
    panel_tfopiform *tfopiform;
    panel_tfsetform *tfsetform;


private slots:
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void OnClickTFDispNewPvForm();
    void OnClickTFSetNewPvForm();
    void OnClickCSSetNewPvForm();
    void OnClickPFSetNewPvForm();
};

#endif // MAINWINDOW_H
