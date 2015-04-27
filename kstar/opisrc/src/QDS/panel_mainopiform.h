#ifndef PANEL_MAINOPIFORM_H
#define PANEL_MAINOPIFORM_H

#include <QWidget>
#include "qtchaccesslib.h"
#include <signal.h>
#include <QTimer>

const int MAIN_OPI_COUNT = 20;

namespace Ui {
    class panel_mainopiform;
}

class panel_mainopiform : public QWidget {
    Q_OBJECT
public:
    panel_mainopiform(QWidget *parent = 0);
    ~panel_mainopiform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_mainopiform *ui;

    AttachChannelAccess *mainopiform_pattach;

    int m_nFlowCounter[MAIN_OPI_COUNT];
    QTimer *m_pTimer[MAIN_OPI_COUNT];

    void setPFStop();
    void setPFPrepare();
    void setPFOperation();
    void setPFQuench();

    AttachChannelAccess *m_pTFBNBUSCHV03AGrpForm;
    AttachChannelAccess *m_pTFBNRTBCHV07AGrpForm;
    AttachChannelAccess *m_pTFBPBUSCHV01AGrpForm;
    AttachChannelAccess *m_pTFBPRTBCHV05AGrpForm;
    AttachChannelAccess *m_pTFC0102BHV01AGrpForm;
    AttachChannelAccess *m_pTFC0304BHV02AGrpForm;
    AttachChannelAccess *m_pTFC0506BHV03AGrpForm;
    AttachChannelAccess *m_pTFC0708BHV04AGrpForm;
    AttachChannelAccess *m_pTFC0910BHV05AGrpForm;
    AttachChannelAccess *m_pTFC1112BHV06AGrpForm;
    AttachChannelAccess *m_pTFC1314BHV07AGrpForm;
    AttachChannelAccess *m_pTFC1516BHV08AGrpForm;

    QMainWindow *m_pTFBNBUSCHV03AGrpWindow;
    QMainWindow *m_pTFBNRTBCHV07AGrpWindow;
    QMainWindow *m_pTFBPBUSCHV01AGrpWindow;
    QMainWindow *m_pTFBPRTBCHV05AGrpWindow;
    QMainWindow *m_pTFC0102BHV01AGrpWindow;
    QMainWindow *m_pTFC0304BHV02AGrpWindow;
    QMainWindow *m_pTFC0506BHV03AGrpWindow;
    QMainWindow *m_pTFC0708BHV04AGrpWindow;
    QMainWindow *m_pTFC0910BHV05AGrpWindow;
    QMainWindow *m_pTFC1112BHV06AGrpWindow;
    QMainWindow *m_pTFC1314BHV07AGrpWindow;
    QMainWindow *m_pTFC1516BHV08AGrpWindow;
    //QMainWindow *m_pTFM0512BHV19AGrpWindow;
    //QMainWindow *m_pTFM1304BHV17AGrpWindow;

    // pf new
    AttachChannelAccess *m_pPFC0100CHVN01GrpForm;
    AttachChannelAccess *m_pPFC0200CHVN03GrpForm;
    AttachChannelAccess *m_pPFC0300CHVN05GrpForm;
    AttachChannelAccess *m_pPFC0400CHVN07GrpForm;
    AttachChannelAccess *m_pPFC0500CHVN09GrpForm;
    AttachChannelAccess *m_pPFC06L0BHVN12GrpForm;
    AttachChannelAccess *m_pPFC06U0BHVN11GrpForm;
    AttachChannelAccess *m_pPFC0700BHVN15GrpForm;

    QMainWindow *m_pPFC0100CHVN01GrpWindow;
    QMainWindow *m_pPFC0200CHVN03GrpWindow;
    QMainWindow *m_pPFC0300CHVN05GrpWindow;
    QMainWindow *m_pPFC0400CHVN07GrpWindow;
    QMainWindow *m_pPFC0500CHVN09GrpWindow;
    QMainWindow *m_pPFC06L0BHVN12GrpWindow;
    QMainWindow *m_pPFC06U0BHVN11GrpWindow;
    QMainWindow *m_pPFC0700BHVN15GrpWindow;

private slots:

     void on_caLabel_227_valueChanged(bool value);
     void on_caLabel_226_valueChanged(bool value);
     void on_caLabel_225_valueChanged(bool value);
     void on_caLabel_224_valueChanged(bool value);
     void on_caLabel_223_valueChanged(bool value);
     void on_caLabel_222_valueChanged(bool value);
     void on_caLabel_221_valueChanged(bool value);
     void on_caLabel_220_valueChanged(bool value);
     void on_caLabel_219_valueChanged(bool value);
     void on_caLabel_218_valueChanged(bool value);
     void on_caLabel_217_valueChanged(bool value);
     void on_caLabel_216_valueChanged(bool value);
     void on_caLabel_215_valueChanged(bool value);
     void on_caLabel_214_valueChanged(bool value);
     void on_caLabel_213_valueChanged(bool value);
     void on_caLabel_212_valueChanged(bool value);
     void on_caLabel_211_valueChanged(bool value);
     void on_caLabel_210_valueChanged(bool value);
     void on_caLabel_209_valueChanged(bool value);
     void on_caLabel_208_valueChanged(bool value);
     void on_caLabel_207_valueChanged(bool value);
     void on_caLabel_206_valueChanged(bool value);
     void on_caLabel_205_valueChanged(bool value);
     void on_caLabel_204_valueChanged(bool value);
     void on_caLabel_203_valueChanged(bool value);
     void on_caLabel_202_valueChanged(bool value);
     void on_caLabel_201_valueChanged(bool value);
     void on_caLabel_200_valueChanged(bool value);
     void on_pushButton_16_clicked();
     void on_pushButton_15_clicked();
     void on_pushButton_14_clicked();
     void on_pushButton_13_clicked();
     void on_pushButton_12_clicked();
     void on_pushButton_11_clicked();
     void on_pushButton_10_clicked();
     void on_pushButton_9_clicked();
     void on_pushButton_20_clicked();
     void on_pushButton_19_clicked();
     void on_pushButton_18_clicked();
     void on_pushButton_17_clicked();
     void on_pushButton_8_clicked();
     void on_pushButton_7_clicked();
     void on_pushButton_6_clicked();
     void on_pushButton_5_clicked();
     void on_pushButton_4_clicked();
     void on_pushButton_3_clicked();
     void on_pushButton_2_clicked();
     void on_pushButton_1_clicked();
     void on_caLabel_108_valueChanged(double value);
     void on_caLabel_102_valueChanged(double value);
     void on_caLabel_96_valueChanged(double value);
     void on_caLabel_90_valueChanged(double value);
     void on_caLabel_85_valueChanged(double value);
     void on_caLabel_80_valueChanged(double value);
     void on_caLabel_75_valueChanged(double value);
     void on_caLabel_70_valueChanged(double value);
     void on_caLabel_65_valueChanged(double value);
     void on_caLabel_60_valueChanged(double value);
     void on_caLabel_55_valueChanged(double value);
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

signals:
    void UpdatePFStop(const QString strStyle);
    void UpdatePFPrepare(const QString strStyle);
    void UpdatePFOperation(const QString strStyle);
    void UpdatePFQuench(const QString strStyle);
};

#endif // PANEL_MAINOPIFORM_H
