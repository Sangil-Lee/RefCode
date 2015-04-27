#ifndef PANEL_PFOPIFORM_H
#define PANEL_PFOPIFORM_H

#include <QWidget>
#include <signal.h>
#include <QTimer>

#include "qtchaccesslib.h"
#include "panel_mainopiform.h"

const int PF_OPI_COUNT = 73;

namespace Ui {
    class panel_pfopiform;
}

class panel_pfopiform : public QWidget {
    Q_OBJECT
public:
    panel_pfopiform(QWidget *parent = 0);
    ~panel_pfopiform();

    panel_mainopiform *m_pMainOpiForm;
    void initConnect();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_pfopiform *ui;

    AttachChannelAccess *pfopiform_pattach;

    int m_nFlowCounter[PF_OPI_COUNT];
    QTimer *m_pTimer[PF_OPI_COUNT];

    // pf coil
    AttachChannelAccess *m_pPFC01LBHV22GrpForm;
    AttachChannelAccess *m_pPFC01LCHV10GrpForm;
    AttachChannelAccess *m_pPFC01UBHV21GrpForm;
    AttachChannelAccess *m_pPFC01UCHV09GrpForm;
    AttachChannelAccess *m_pPFC02LBHV24GrpForm;
    AttachChannelAccess *m_pPFC02LCHV15GrpForm;
    AttachChannelAccess *m_pPFC02UBHV23GrpForm;
    AttachChannelAccess *m_pPFC02UCHV14GrpForm;
    AttachChannelAccess *m_pPFC03LBHV26GrpForm;
    AttachChannelAccess *m_pPFC03LCHV20GrpForm;
    AttachChannelAccess *m_pPFC03UBHV25GrpForm;
    AttachChannelAccess *m_pPFC03UCHV19GrpForm;
    AttachChannelAccess *m_pPFC04LBHV28GrpForm;
    AttachChannelAccess *m_pPFC04LCHV26GrpForm;
    AttachChannelAccess *m_pPFC04UBHV27GrpForm;
    AttachChannelAccess *m_pPFC04UCHV25GrpForm;
    AttachChannelAccess *m_pPFC05LBHV30GrpForm;
    AttachChannelAccess *m_pPFC05LCHV32GrpForm;
    AttachChannelAccess *m_pPFC05UBHV29GrpForm;
    AttachChannelAccess *m_pPFC05UCHV31GrpForm;
    AttachChannelAccess *m_pPFC06LLBHV35GrpForm;
    AttachChannelAccess *m_pPFC06LTBHV36GrpForm;
    AttachChannelAccess *m_pPFC06LUBHV34GrpForm;
    AttachChannelAccess *m_pPFC06TBHV37GrpForm;
    AttachChannelAccess *m_pPFC06ULBHV32GrpForm;
    AttachChannelAccess *m_pPFC06UTBHV33GrpForm;
    AttachChannelAccess *m_pPFC06UUBHV31GrpForm;
    AttachChannelAccess *m_pPFC07LBHV39GrpForm;
    AttachChannelAccess *m_pPFC07TBHV40GrpForm;
    AttachChannelAccess *m_pPFC07UBHV38GrpForm;

    QMainWindow *m_pPFC01LBHV22GrpWindow;
    QMainWindow *m_pPFC01LCHV10GrpWindow;
    QMainWindow *m_pPFC01UBHV21GrpWindow;
    QMainWindow *m_pPFC01UCHV09GrpWindow;
    QMainWindow *m_pPFC02LBHV24GrpWindow;
    QMainWindow *m_pPFC02LCHV15GrpWindow;
    QMainWindow *m_pPFC02UBHV23GrpWindow;
    QMainWindow *m_pPFC02UCHV14GrpWindow;
    QMainWindow *m_pPFC03LBHV26GrpWindow;
    QMainWindow *m_pPFC03LCHV20GrpWindow;
    QMainWindow *m_pPFC03UBHV25GrpWindow;
    QMainWindow *m_pPFC03UCHV19GrpWindow;
    QMainWindow *m_pPFC04LBHV28GrpWindow;
    QMainWindow *m_pPFC04LCHV26GrpWindow;
    QMainWindow *m_pPFC04UBHV27GrpWindow;
    QMainWindow *m_pPFC04UCHV25GrpWindow;
    QMainWindow *m_pPFC05LBHV30GrpWindow;
    QMainWindow *m_pPFC05LCHV32GrpWindow;
    QMainWindow *m_pPFC05UBHV29GrpWindow;
    QMainWindow *m_pPFC05UCHV31GrpWindow;
    QMainWindow *m_pPFC06LLBHV35GrpWindow;
    QMainWindow *m_pPFC06LTBHV36GrpWindow;
    QMainWindow *m_pPFC06LUBHV34GrpWindow;
    QMainWindow *m_pPFC06TBHV37GrpWindow;
    QMainWindow *m_pPFC06ULBHV32GrpWindow;
    QMainWindow *m_pPFC06UTBHV33GrpWindow;
    QMainWindow *m_pPFC06UUBHV31GrpWindow;
    QMainWindow *m_pPFC07LBHV39GrpWindow;
    QMainWindow *m_pPFC07TBHV40GrpWindow;
    QMainWindow *m_pPFC07UBHV38GrpWindow;

    // pf bus
    AttachChannelAccess *m_pPFB01ICHV13GrpForm;
    AttachChannelAccess *m_pPFB01LPCHV12GrpForm;
    AttachChannelAccess *m_pPFB01UNCHV11GrpForm;
    AttachChannelAccess *m_pPFB02ICHV18GrpForm;
    AttachChannelAccess *m_pPFB02LNCHV17GrpForm;
    AttachChannelAccess *m_pPFB02UPCHV16GrpForm;
    AttachChannelAccess *m_pPFB03LNCHV24GrpForm;
    AttachChannelAccess *m_pPFB03LPCHV23GrpForm;
    AttachChannelAccess *m_pPFB03UNCHV22GrpForm;
    AttachChannelAccess *m_pPFB03UPCHV21GrpForm;
    AttachChannelAccess *m_pPFB04LNCHV30GrpForm;
    AttachChannelAccess *m_pPFB04LPCHV29GrpForm;
    AttachChannelAccess *m_pPFB04UNCHV28GrpForm;
    AttachChannelAccess *m_pPFB04UPCHV27GrpForm;
    AttachChannelAccess *m_pPFB05LNCHV36GrpForm;
    AttachChannelAccess *m_pPFB05LPCHV35GrpForm;
    AttachChannelAccess *m_pPFB05UNCHV34GrpForm;
    AttachChannelAccess *m_pPFB05UPCHV33GrpForm;
    AttachChannelAccess *m_pPFB06LNCHV40GrpForm;
    AttachChannelAccess *m_pPFB06LPCHV39GrpForm;
    AttachChannelAccess *m_pPFB06UNCHV38GrpForm;
    AttachChannelAccess *m_pPFB06UPCHV37GrpForm;
    AttachChannelAccess *m_pPFB07ICHV43GrpForm;
    AttachChannelAccess *m_pPFB07LNCHV42GrpForm;
    AttachChannelAccess *m_pPFB07UPCHV41GrpForm;

    QMainWindow *m_pPFB01ICHV13GrpWindow;
    QMainWindow *m_pPFB01LPCHV12GrpWindow;
    QMainWindow *m_pPFB01UNCHV11GrpWindow;
    QMainWindow *m_pPFB02ICHV18GrpWindow;
    QMainWindow *m_pPFB02LNCHV17GrpWindow;
    QMainWindow *m_pPFB02UPCHV16GrpWindow;
    QMainWindow *m_pPFB03LNCHV24GrpWindow;
    QMainWindow *m_pPFB03LPCHV23GrpWindow;
    QMainWindow *m_pPFB03UNCHV22GrpWindow;
    QMainWindow *m_pPFB03UPCHV21GrpWindow;
    QMainWindow *m_pPFB04LNCHV30GrpWindow;
    QMainWindow *m_pPFB04LPCHV29GrpWindow;
    QMainWindow *m_pPFB04UNCHV28GrpWindow;
    QMainWindow *m_pPFB04UPCHV27GrpWindow;
    QMainWindow *m_pPFB05LNCHV36GrpWindow;
    QMainWindow *m_pPFB05LPCHV35GrpWindow;
    QMainWindow *m_pPFB05UNCHV34GrpWindow;
    QMainWindow *m_pPFB05UPCHV33GrpWindow;
    QMainWindow *m_pPFB06LNCHV40GrpWindow;
    QMainWindow *m_pPFB06LPCHV39GrpWindow;
    QMainWindow *m_pPFB06UNCHV38GrpWindow;
    QMainWindow *m_pPFB06UPCHV37GrpWindow;
    QMainWindow *m_pPFB07ICHV43GrpWindow;
    QMainWindow *m_pPFB07LNCHV42GrpWindow;
    QMainWindow *m_pPFB07UPCHV41GrpWindow;

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
    void on_pushButton_73_clicked();
    void on_pushButton_72_clicked();
    void on_pushButton_71_clicked();
    void on_pushButton_70_clicked();
    void on_pushButton_69_clicked();
    void on_pushButton_68_clicked();
    void on_pushButton_67_clicked();
    void on_pushButton_66_clicked();
    void on_pushButton_65_clicked();
    void on_pushButton_64_clicked();
    void on_pushButton_63_clicked();
    void on_pushButton_62_clicked();
    void on_pushButton_61_clicked();
    void on_pushButton_60_clicked();
    void on_pushButton_59_clicked();
    void on_pushButton_58_clicked();
    void on_pushButton_57_clicked();
    void on_pushButton_56_clicked();
    void on_pushButton_55_clicked();
    void on_pushButton_54_clicked();
    void on_pushButton_53_clicked();
    void on_pushButton_48_clicked();
    void on_pushButton_47_clicked();
    void on_pushButton_50_clicked();
    void on_pushButton_49_clicked();
    void on_caLabel_458_valueChanged(double value);
    void on_caLabel_452_valueChanged(double value);
    void on_caLabel_446_valueChanged(double value);
    void on_caLabel_440_valueChanged(double value);
    void on_caLabel_434_valueChanged(double value);
    void on_caLabel_428_valueChanged(double value);
    void on_caLabel_422_valueChanged(double value);
    void on_caLabel_416_valueChanged(double value);
    void on_caLabel_410_valueChanged(double value);
    void on_caLabel_404_valueChanged(double value);
    void on_caLabel_398_valueChanged(double value);
    void on_caLabel_392_valueChanged(double value);
    void on_caLabel_386_valueChanged(double value);
    void on_caLabel_380_valueChanged(double value);
    void on_caLabel_374_valueChanged(double value);
    void on_caLabel_368_valueChanged(double value);
    void on_caLabel_362_valueChanged(double value);
    void on_caLabel_356_valueChanged(double value);
    void on_caLabel_350_valueChanged(double value);
    void on_caLabel_344_valueChanged(double value);
    void on_caLabel_338_valueChanged(double value);
    void on_caLabel_332_valueChanged(double value);
    void on_caLabel_326_valueChanged(double value);
    void on_caLabel_320_valueChanged(double value);
    void on_caLabel_314_valueChanged(double value);
    void on_pushButton_52_clicked();
    void on_pushButton_51_clicked();
    void on_pushButton_46_clicked();
    void on_pushButton_45_clicked();
    void on_pushButton_44_clicked();
    void on_pushButton_43_clicked();
    void on_pushButton_42_clicked();
    void on_pushButton_41_clicked();
    void on_pushButton_40_clicked();
    void on_pushButton_39_clicked();
    void on_pushButton_38_clicked();
    void on_pushButton_37_clicked();
    void on_pushButton_36_clicked();
    void on_pushButton_35_clicked();
    void on_caLabel_308_valueChanged(double value);
    void on_caLabel_302_valueChanged(double value);
    void on_caLabel_272_valueChanged(double value);
    void on_caLabel_266_valueChanged(double value);
    void on_caLabel_260_valueChanged(double value);
    void on_caLabel_254_valueChanged(double value);
    void on_caLabel_248_valueChanged(double value);
    void on_caLabel_242_valueChanged(double value);
    void on_caLabel_236_valueChanged(double value);
    void on_caLabel_230_valueChanged(double value);
    void on_caLabel_224_valueChanged(double value);
    void on_caLabel_218_valueChanged(double value);
    void on_caLabel_212_valueChanged(double value);
    void on_caLabel_206_valueChanged(double value);
    void on_pushButton_34_clicked();
    void on_pushButton_33_clicked();
    void on_pushButton_32_clicked();
    void on_pushButton_31_clicked();
    void on_pushButton_30_clicked();
    void on_pushButton_29_clicked();
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
    void on_caLabel_200_valueChanged(double value);
    void on_caLabel_194_valueChanged(double value);
    void on_caLabel_188_valueChanged(double value);
    void on_caLabel_182_valueChanged(double value);
    void on_caLabel_176_valueChanged(double value);
    void on_caLabel_170_valueChanged(double value);
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
    void on_pushButton_18_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_15_clicked();
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
    void OnUpdateTimer29();
    void OnUpdateTimer30();
    void OnUpdateTimer31();
    void OnUpdateTimer32();
    void OnUpdateTimer33();
    void OnUpdateTimer34();
    void OnUpdateTimer35();
    void OnUpdateTimer36();
    void OnUpdateTimer37();
    void OnUpdateTimer38();
    void OnUpdateTimer39();
    void OnUpdateTimer40();
    void OnUpdateTimer41();
    void OnUpdateTimer42();
    void OnUpdateTimer43();
    void OnUpdateTimer44();
    void OnUpdateTimer45();
    void OnUpdateTimer46();
    void OnUpdateTimer47();
    void OnUpdateTimer48();
    void OnUpdateTimer49();
    void OnUpdateTimer50();
    void OnUpdateTimer51();
    void OnUpdateTimer52();
    void OnUpdateTimer53();
    void OnUpdateTimer54();
    void OnUpdateTimer55();

    void OnUpdateTimer56();
    void OnUpdateTimer57();
    void OnUpdateTimer58();
    void OnUpdateTimer59();
    void OnUpdateTimer60();
    void OnUpdateTimer61();
    void OnUpdateTimer62();
    void OnUpdateTimer63();
    void OnUpdateTimer64();
    void OnUpdateTimer65();
    void OnUpdateTimer66();
    void OnUpdateTimer67();
    void OnUpdateTimer68();
    void OnUpdateTimer69();
    void OnUpdateTimer70();
    void OnUpdateTimer71();
    void OnUpdateTimer72();
    void OnUpdateTimer73();

    void UpdatePFStop(const QString strStyle);
    void UpdatePFPrepare(const QString strStyle);
    void UpdatePFOperation(const QString strStyle);
    void UpdatePFQuench(const QString strStyle);
};

#endif // PANEL_PFOPIFORM_H
