#ifndef PANEL_TFOPIFORM_H
#define PANEL_TFOPIFORM_H

#include <QWidget>
#include <signal.h>
#include <QTimer>

#include "qtchaccesslib.h"
#include "panel_mainopiform.h"

const int TF_OPI_COUNT = 28;

namespace Ui {
    class panel_tfopiform;
}

class panel_tfopiform : public QWidget {
    Q_OBJECT
public:
    panel_tfopiform(QWidget *parent = 0);
    ~panel_tfopiform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_tfopiform *ui;

    panel_mainopiform *m_pMainOpiForm;
    void initConnect();
    AttachChannelAccess *tfopiform_pattach;
    int m_nFlowCounter[TF_OPI_COUNT];
    QTimer *m_pTimer[TF_OPI_COUNT];

    AttachChannelAccess *m_pTFBNBUSCHV03AGrpForm;
    AttachChannelAccess *m_pTFBNBUSCHV04BGrpForm;
    AttachChannelAccess *m_pTFBNRTBCHV07AGrpForm;
    AttachChannelAccess *m_pTFBNRTBCHV08BGrpForm;
    AttachChannelAccess *m_pTFBPBUSCHV01AGrpForm;
    AttachChannelAccess *m_pTFBPBUSCHV02BGrpForm;
    AttachChannelAccess *m_pTFBPRTBCHV05AGrpForm;
    AttachChannelAccess *m_pTFBPRTBCHV06BGrpForm;
    AttachChannelAccess *m_pTFC0102BHV01AGrpForm;
    AttachChannelAccess *m_pTFC0102BHV01BGrpForm;
    AttachChannelAccess *m_pTFC0304BHV02AGrpForm;
    AttachChannelAccess *m_pTFC0304BHV02BGrpForm;
    AttachChannelAccess *m_pTFC0506BHV03AGrpForm;
    AttachChannelAccess *m_pTFC0506BHV03BGrpForm;
    AttachChannelAccess *m_pTFC0708BHV04AGrpForm;
    AttachChannelAccess *m_pTFC0708BHV04BGrpForm;
    AttachChannelAccess *m_pTFC0910BHV05AGrpForm;
    AttachChannelAccess *m_pTFC0910BHV05BGrpForm;
    AttachChannelAccess *m_pTFC1112BHV06AGrpForm;
    AttachChannelAccess *m_pTFC1112BHV06BGrpForm;
    AttachChannelAccess *m_pTFC1314BHV07AGrpForm;
    AttachChannelAccess *m_pTFC1314BHV07BGrpForm;
    AttachChannelAccess *m_pTFC1516BHV08AGrpForm;
    AttachChannelAccess *m_pTFC1516BHV08BGrpForm;
    AttachChannelAccess *m_pTFM0512BHV19AGrpForm;
    AttachChannelAccess *m_pTFM0512BHV20BGrpForm;
    AttachChannelAccess *m_pTFM1304BHV17AGrpForm;
    AttachChannelAccess *m_pTFM1304BHV18BGrpForm;

    QMainWindow *m_pTFBNBUSCHV03AGrpWindow;
    QMainWindow *m_pTFBNBUSCHV04BGrpWindow;
    QMainWindow *m_pTFBNRTBCHV07AGrpWindow;
    QMainWindow *m_pTFBNRTBCHV08BGrpWindow;
    QMainWindow *m_pTFBPBUSCHV01AGrpWindow;
    QMainWindow *m_pTFBPBUSCHV02BGrpWindow;
    QMainWindow *m_pTFBPRTBCHV05AGrpWindow;
    QMainWindow *m_pTFBPRTBCHV06BGrpWindow;
    QMainWindow *m_pTFC0102BHV01AGrpWindow;
    QMainWindow *m_pTFC0102BHV01BGrpWindow;
    QMainWindow *m_pTFC0304BHV02AGrpWindow;
    QMainWindow *m_pTFC0304BHV02BGrpWindow;
    QMainWindow *m_pTFC0506BHV03AGrpWindow;
    QMainWindow *m_pTFC0506BHV03BGrpWindow;
    QMainWindow *m_pTFC0708BHV04AGrpWindow;
    QMainWindow *m_pTFC0708BHV04BGrpWindow;
    QMainWindow *m_pTFC0910BHV05AGrpWindow;
    QMainWindow *m_pTFC0910BHV05BGrpWindow;
    QMainWindow *m_pTFC1112BHV06AGrpWindow;
    QMainWindow *m_pTFC1112BHV06BGrpWindow;
    QMainWindow *m_pTFC1314BHV07AGrpWindow;
    QMainWindow *m_pTFC1314BHV07BGrpWindow;
    QMainWindow *m_pTFC1516BHV08AGrpWindow;
    QMainWindow *m_pTFC1516BHV08BGrpWindow;
    QMainWindow *m_pTFM0512BHV19AGrpWindow;
    QMainWindow *m_pTFM0512BHV20BGrpWindow;
    QMainWindow *m_pTFM1304BHV17AGrpWindow;
    QMainWindow *m_pTFM1304BHV18BGrpWindow;



private slots:
    void on_pushButton_28_clicked();
    void on_pushButton_27_clicked();
    void on_pushButton_26_clicked();
    void on_pushButton_25_clicked();
    void on_pushButton_24_clicked();
    void on_pushButton_23_clicked();
    void on_pushButton_22_clicked();
    void on_pushButton_21_clicked();
    void on_pushButton_20_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_18_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_15_clicked();
    void on_caLabel_164_valueChanged(double value);
    void on_caLabel_158_valueChanged(double value);
    void on_caLabel_152_valueChanged(double value);
    void on_caLabel_146_valueChanged(double value);
    void on_caLabel_140_valueChanged(double value);
    void on_caLabel_134_valueChanged(double value);
    void on_caLabel_128_valueChanged(double value);
    void on_caLabel_122_valueChanged(double value);
    void on_caLabel_116_valueChanged(double value);
    void on_caLabel_110_valueChanged(double value);
    void on_caLabel_104_valueChanged(double value);
    void on_caLabel_98_valueChanged(double value);
    void on_caLabel_92_valueChanged(double value);
    void on_caLabel_86_valueChanged(double value);
    void on_pushButton_14_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_1_clicked();
    void on_caLabel_80_valueChanged(double value);
    void on_caLabel_74_valueChanged(double value);
    void on_caLabel_68_valueChanged(double value);
    void on_caLabel_62_valueChanged(double value);
    void on_caLabel_56_valueChanged(double value);
    void on_caLabel_50_valueChanged(double value);
    void on_caLabel_44_valueChanged(double value);
    void on_caLabel_38_valueChanged(double value);
    void on_caLabel_32_valueChanged(double value);
    void on_caLabel_26_valueChanged(double value);
    void on_caLabel_20_valueChanged(double value);
    void on_caLabel_14_valueChanged(double value);
    void on_caLabel_8_valueChanged(double value);
    void on_caLabel_1_valueChanged(double value);



    void OnUpdateTimer1();
    void OnUpdateTimer2();
    void OnUpdateTimer3();
    void OnUpdateTimer4();
    void OnUpdateTimer5();
    void OnUpdateTimer6();
    void OnUpdateTimer7();
    void OnUpdateTimer8();
    void OnUpdateTimer9();
    void OnUpdateTimer10();
    void OnUpdateTimer11();
    void OnUpdateTimer12();
    void OnUpdateTimer13();
    void OnUpdateTimer14();
    void OnUpdateTimer15();
    void OnUpdateTimer16();
    void OnUpdateTimer17();
    void OnUpdateTimer18();
    void OnUpdateTimer19();
    void OnUpdateTimer20();
    void OnUpdateTimer21();
    void OnUpdateTimer22();
    void OnUpdateTimer23();
    void OnUpdateTimer24();
    void OnUpdateTimer25();
    void OnUpdateTimer26();
    void OnUpdateTimer27();
    void OnUpdateTimer28();

    void UpdatePFStop(const QString strStyle);
    void UpdatePFPrepare(const QString strStyle);
    void UpdatePFOperation(const QString strStyle);
    void UpdatePFQuench(const QString strStyle);
};

#endif // PANEL_TFOPIFORM_H
