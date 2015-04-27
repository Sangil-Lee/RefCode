#include "panel_tfopiform.h"
#include "ui_panel_tfopiform.h"
#include "QuenchDefine.h"

panel_tfopiform::panel_tfopiform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_tfopiform)
{
    ui->setupUi(this);

    tfopiform_pattach = new AttachChannelAccess(this);

    ui->tabWidget->setCurrentIndex(0);

    //Tab 01
    ui->caLabel_1->hide();
    ui->caLabel_2->hide();
    ui->caLabel_3->hide();
    ui->caLabel_7->hide();
    ui->caLabel_8->hide();
    ui->caLabel_9->hide();
    ui->caLabel_13->hide();
    ui->caLabel_14->hide();
    ui->caLabel_15->hide();
    ui->caLabel_19->hide();
    ui->caLabel_20->hide();
    ui->caLabel_21->hide();
    ui->caLabel_25->hide();
    ui->caLabel_26->hide();
    ui->caLabel_27->hide();
    ui->caLabel_31->hide();
    ui->caLabel_32->hide();
    ui->caLabel_33->hide();
    ui->caLabel_37->hide();
    ui->caLabel_38->hide();
    ui->caLabel_39->hide();
    ui->caLabel_43->hide();
    ui->caLabel_44->hide();
    ui->caLabel_45->hide();
    ui->caLabel_49->hide();
    ui->caLabel_50->hide();
    ui->caLabel_51->hide();
    ui->caLabel_55->hide();
    ui->caLabel_56->hide();
    ui->caLabel_57->hide();
    ui->caLabel_61->hide();
    ui->caLabel_62->hide();
    ui->caLabel_63->hide();
    ui->caLabel_67->hide();
    ui->caLabel_68->hide();
    ui->caLabel_69->hide();
    ui->caLabel_73->hide();
    ui->caLabel_74->hide();
    ui->caLabel_75->hide();
    ui->caLabel_79->hide();
    ui->caLabel_80->hide();
    ui->caLabel_81->hide();

    //Tab 02
    ui->caLabel_85->hide();
    ui->caLabel_86->hide();
    ui->caLabel_87->hide();
    ui->caLabel_91->hide();
    ui->caLabel_92->hide();
    ui->caLabel_93->hide();
    ui->caLabel_97->hide();
    ui->caLabel_98->hide();
    ui->caLabel_99->hide();
    ui->caLabel_103->hide();
    ui->caLabel_104->hide();
    ui->caLabel_105->hide();
    ui->caLabel_109->hide();
    ui->caLabel_110->hide();
    ui->caLabel_111->hide();
    ui->caLabel_115->hide();
    ui->caLabel_116->hide();
    ui->caLabel_117->hide();
    ui->caLabel_121->hide();
    ui->caLabel_122->hide();
    ui->caLabel_123->hide();
    ui->caLabel_127->hide();
    ui->caLabel_128->hide();
    ui->caLabel_129->hide();
    ui->caLabel_133->hide();
    ui->caLabel_134->hide();
    ui->caLabel_135->hide();
    ui->caLabel_139->hide();
    ui->caLabel_140->hide();
    ui->caLabel_141->hide();
    ui->caLabel_145->hide();
    ui->caLabel_146->hide();
    ui->caLabel_147->hide();
    ui->caLabel_151->hide();
    ui->caLabel_152->hide();
    ui->caLabel_153->hide();
    ui->caLabel_157->hide();
    ui->caLabel_158->hide();
    ui->caLabel_159->hide();
    ui->caLabel_163->hide();
    ui->caLabel_164->hide();
    ui->caLabel_165->hide();

    m_pTFBNBUSCHV03AGrpWindow = new QMainWindow();
    m_pTFBNBUSCHV04BGrpWindow = new QMainWindow();
    m_pTFBNRTBCHV07AGrpWindow = new QMainWindow();
    m_pTFBNRTBCHV08BGrpWindow = new QMainWindow();
    m_pTFBPBUSCHV01AGrpWindow = new QMainWindow();
    m_pTFBPBUSCHV02BGrpWindow = new QMainWindow();
    m_pTFBPRTBCHV05AGrpWindow = new QMainWindow();
    m_pTFBPRTBCHV06BGrpWindow = new QMainWindow();
    m_pTFC0102BHV01AGrpWindow = new QMainWindow();
    m_pTFC0102BHV01BGrpWindow = new QMainWindow();
    m_pTFC0304BHV02AGrpWindow = new QMainWindow();
    m_pTFC0304BHV02BGrpWindow = new QMainWindow();
    m_pTFC0506BHV03AGrpWindow = new QMainWindow();
    m_pTFC0506BHV03BGrpWindow = new QMainWindow();
    m_pTFC0708BHV04AGrpWindow = new QMainWindow();
    m_pTFC0708BHV04BGrpWindow = new QMainWindow();
    m_pTFC0910BHV05AGrpWindow = new QMainWindow();
    m_pTFC0910BHV05BGrpWindow = new QMainWindow();
    m_pTFC1112BHV06AGrpWindow = new QMainWindow();
    m_pTFC1112BHV06BGrpWindow = new QMainWindow();
    m_pTFC1314BHV07AGrpWindow = new QMainWindow();
    m_pTFC1314BHV07BGrpWindow = new QMainWindow();
    m_pTFC1516BHV08AGrpWindow = new QMainWindow();
    m_pTFC1516BHV08BGrpWindow = new QMainWindow();
    m_pTFM0512BHV19AGrpWindow = new QMainWindow();
    m_pTFM0512BHV20BGrpWindow = new QMainWindow();
    m_pTFM1304BHV17AGrpWindow = new QMainWindow();
    m_pTFM1304BHV18BGrpWindow = new QMainWindow();

    m_pTFBNBUSCHV03AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBNBUSCHV04BGrpWindow->setGeometry(30,30,540,400);
    m_pTFBNRTBCHV07AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBNRTBCHV08BGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPBUSCHV01AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPBUSCHV02BGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPRTBCHV05AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPRTBCHV06BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0102BHV01AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0102BHV01BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0304BHV02AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0304BHV02BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0506BHV03AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0506BHV03BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0708BHV04AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0708BHV04BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0910BHV05AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0910BHV05BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1112BHV06AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1112BHV06BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1314BHV07AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1314BHV07BGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1516BHV08AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1516BHV08BGrpWindow->setGeometry(30,30,540,400);
    m_pTFM0512BHV19AGrpWindow->setGeometry(30,30,540,400);
    m_pTFM0512BHV20BGrpWindow->setGeometry(30,30,540,400);
    m_pTFM1304BHV17AGrpWindow->setGeometry(30,30,540,400);
    m_pTFM1304BHV18BGrpWindow->setGeometry(30,30,540,400);

    m_pTFBNBUSCHV03AGrpForm = NULL;
    m_pTFBNBUSCHV04BGrpForm = NULL;
    m_pTFBNRTBCHV07AGrpForm = NULL;
    m_pTFBNRTBCHV08BGrpForm = NULL;
    m_pTFBPBUSCHV01AGrpForm = NULL;
    m_pTFBPBUSCHV02BGrpForm = NULL;
    m_pTFBPRTBCHV05AGrpForm = NULL;
    m_pTFBPRTBCHV06BGrpForm = NULL;
    m_pTFC0102BHV01AGrpForm = NULL;
    m_pTFC0102BHV01BGrpForm = NULL;
    m_pTFC0304BHV02AGrpForm = NULL;
    m_pTFC0304BHV02BGrpForm = NULL;
    m_pTFC0506BHV03AGrpForm = NULL;
    m_pTFC0506BHV03BGrpForm = NULL;
    m_pTFC0708BHV04AGrpForm = NULL;
    m_pTFC0708BHV04BGrpForm = NULL;
    m_pTFC0910BHV05AGrpForm = NULL;
    m_pTFC0910BHV05BGrpForm = NULL;
    m_pTFC1112BHV06AGrpForm = NULL;
    m_pTFC1112BHV06BGrpForm = NULL;
    m_pTFC1314BHV07AGrpForm = NULL;
    m_pTFC1314BHV07BGrpForm = NULL;
    m_pTFC1516BHV08AGrpForm = NULL;
    m_pTFC1516BHV08BGrpForm = NULL;
    m_pTFM0512BHV19AGrpForm = NULL;
    m_pTFM0512BHV20BGrpForm = NULL;
    m_pTFM1304BHV17AGrpForm = NULL;
    m_pTFM1304BHV18BGrpForm = NULL;


    for(int i=0;i!=TF_OPI_COUNT;++i){
        m_nFlowCounter[i] = 0;
        m_pTimer[i] = new QTimer();
    }

    connect(m_pTimer[0],SIGNAL(timeout()),this,SLOT(OnUpdateTimer1()));
    connect(m_pTimer[1],SIGNAL(timeout()),this,SLOT(OnUpdateTimer2()));
    connect(m_pTimer[2],SIGNAL(timeout()),this,SLOT(OnUpdateTimer3()));
    connect(m_pTimer[3],SIGNAL(timeout()),this,SLOT(OnUpdateTimer4()));
    connect(m_pTimer[4],SIGNAL(timeout()),this,SLOT(OnUpdateTimer5()));
    connect(m_pTimer[5],SIGNAL(timeout()),this,SLOT(OnUpdateTimer6()));
    connect(m_pTimer[6],SIGNAL(timeout()),this,SLOT(OnUpdateTimer7()));
    connect(m_pTimer[7],SIGNAL(timeout()),this,SLOT(OnUpdateTimer8()));
    connect(m_pTimer[8],SIGNAL(timeout()),this,SLOT(OnUpdateTimer9()));
    connect(m_pTimer[9],SIGNAL(timeout()),this,SLOT(OnUpdateTimer10()));
    connect(m_pTimer[10],SIGNAL(timeout()),this,SLOT(OnUpdateTimer11()));
    connect(m_pTimer[11],SIGNAL(timeout()),this,SLOT(OnUpdateTimer12()));
    connect(m_pTimer[12],SIGNAL(timeout()),this,SLOT(OnUpdateTimer13()));
    connect(m_pTimer[13],SIGNAL(timeout()),this,SLOT(OnUpdateTimer14()));
    connect(m_pTimer[14],SIGNAL(timeout()),this,SLOT(OnUpdateTimer15()));
    connect(m_pTimer[15],SIGNAL(timeout()),this,SLOT(OnUpdateTimer16()));
    connect(m_pTimer[16],SIGNAL(timeout()),this,SLOT(OnUpdateTimer17()));
    connect(m_pTimer[17],SIGNAL(timeout()),this,SLOT(OnUpdateTimer18()));
    connect(m_pTimer[18],SIGNAL(timeout()),this,SLOT(OnUpdateTimer19()));
    connect(m_pTimer[19],SIGNAL(timeout()),this,SLOT(OnUpdateTimer20()));
    connect(m_pTimer[20],SIGNAL(timeout()),this,SLOT(OnUpdateTimer21()));
    connect(m_pTimer[21],SIGNAL(timeout()),this,SLOT(OnUpdateTimer22()));
    connect(m_pTimer[22],SIGNAL(timeout()),this,SLOT(OnUpdateTimer23()));
    connect(m_pTimer[23],SIGNAL(timeout()),this,SLOT(OnUpdateTimer24()));
    connect(m_pTimer[24],SIGNAL(timeout()),this,SLOT(OnUpdateTimer25()));
    connect(m_pTimer[25],SIGNAL(timeout()),this,SLOT(OnUpdateTimer26()));
    connect(m_pTimer[26],SIGNAL(timeout()),this,SLOT(OnUpdateTimer27()));
    connect(m_pTimer[27],SIGNAL(timeout()),this,SLOT(OnUpdateTimer28()));

    ui->tabWidget->setStyle(new QWindowsStyle);
}

panel_tfopiform::~panel_tfopiform()
{
    delete ui;
}

void panel_tfopiform::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void panel_tfopiform::initConnect()
{
    connect(m_pMainOpiForm,SIGNAL(UpdatePFOperation(QString)),this,SLOT(UpdatePFOperation(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFStop(QString)),this,SLOT(UpdatePFStop(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFPrepare(QString)),this,SLOT(UpdatePFPrepare(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFQuench(QString)),this,SLOT(UpdatePFQuench(QString)));
}

void panel_tfopiform::OnUpdateTimer1()
{
    ui->progressBar_1->setValue(++m_nFlowCounter[0]*100);
}
void panel_tfopiform::OnUpdateTimer2()
{
    ui->progressBar_2->setValue(++m_nFlowCounter[1]*100);
}
void panel_tfopiform::OnUpdateTimer3()
{
    ui->progressBar_3->setValue(++m_nFlowCounter[2]*100);
}
void panel_tfopiform::OnUpdateTimer4()
{
    ui->progressBar_4->setValue(++m_nFlowCounter[3]*100);
}
void panel_tfopiform::OnUpdateTimer5()
{
    ui->progressBar_5->setValue(++m_nFlowCounter[4]*100);
}
void panel_tfopiform::OnUpdateTimer6()
{
    ui->progressBar_6->setValue(++m_nFlowCounter[5]*100);
}
void panel_tfopiform::OnUpdateTimer7()
{
    ui->progressBar_7->setValue(++m_nFlowCounter[6]*100);
}
void panel_tfopiform::OnUpdateTimer8()
{
    ui->progressBar_8->setValue(++m_nFlowCounter[7]*100);
}
void panel_tfopiform::OnUpdateTimer9()
{
    ui->progressBar_9->setValue(++m_nFlowCounter[8]*100);
}
void panel_tfopiform::OnUpdateTimer10()
{
    ui->progressBar_10->setValue(++m_nFlowCounter[9]*100);
}
void panel_tfopiform::OnUpdateTimer11()
{
    ui->progressBar_11->setValue(++m_nFlowCounter[10]*100);
}
void panel_tfopiform::OnUpdateTimer12()
{
    ui->progressBar_12->setValue(++m_nFlowCounter[11]*100);
}
void panel_tfopiform::OnUpdateTimer13()
{
    ui->progressBar_13->setValue(++m_nFlowCounter[12]*100);
}
void panel_tfopiform::OnUpdateTimer14()
{
    ui->progressBar_14->setValue(++m_nFlowCounter[13]*100);
}
void panel_tfopiform::OnUpdateTimer15()
{
    ui->progressBar_15->setValue(++m_nFlowCounter[14]*100);
}
void panel_tfopiform::OnUpdateTimer16()
{
    ui->progressBar_16->setValue(++m_nFlowCounter[15]*100);
}
void panel_tfopiform::OnUpdateTimer17()
{
    ui->progressBar_17->setValue(++m_nFlowCounter[16]*100);
}
void panel_tfopiform::OnUpdateTimer18()
{
    ui->progressBar_18->setValue(++m_nFlowCounter[17]*100);
}
void panel_tfopiform::OnUpdateTimer19()
{
    ui->progressBar_19->setValue(++m_nFlowCounter[18]*100);
}
void panel_tfopiform::OnUpdateTimer20()
{
    ui->progressBar_20->setValue(++m_nFlowCounter[19]*100);
}
void panel_tfopiform::OnUpdateTimer21()
{
    ui->progressBar_21->setValue(++m_nFlowCounter[20]*100);
}
void panel_tfopiform::OnUpdateTimer22()
{
    ui->progressBar_22->setValue(++m_nFlowCounter[21]*100);
}
void panel_tfopiform::OnUpdateTimer23()
{
    ui->progressBar_23->setValue(++m_nFlowCounter[22]*100);
}
void panel_tfopiform::OnUpdateTimer24()
{
    ui->progressBar_24->setValue(++m_nFlowCounter[23]*100);
}
void panel_tfopiform::OnUpdateTimer25()
{
    ui->progressBar_25->setValue(++m_nFlowCounter[24]*100);
}
void panel_tfopiform::OnUpdateTimer26()
{
    ui->progressBar_26->setValue(++m_nFlowCounter[25]*100);
}
void panel_tfopiform::OnUpdateTimer27()
{
    ui->progressBar_27->setValue(++m_nFlowCounter[26]*100);
}
void panel_tfopiform::OnUpdateTimer28()
{
    ui->progressBar_28->setValue(++m_nFlowCounter[27]*100);
}


void panel_tfopiform::on_caLabel_1_valueChanged(double value)
{
    setOpiBar(ui->caLabel_2, ui->caLabel_1, ui->groupBox_1, ui->label_100);
    setOpiProgressBar(ui->caLabel_2
                      , ui->caLabel_1
                      , ui->caLabel_3
                      , ui->progressBar_1
                      , m_pTimer[0]
                      , m_nFlowCounter[0]);
}

void panel_tfopiform::on_caLabel_8_valueChanged(double value)
{
    setOpiBar(ui->caLabel_7, ui->caLabel_8, ui->groupBox_2, ui->label_101);
    setOpiProgressBar(ui->caLabel_7
                      , ui->caLabel_8
                      , ui->caLabel_9
                      , ui->progressBar_2
                      , m_pTimer[1]
                      , m_nFlowCounter[1]);
}

void panel_tfopiform::on_caLabel_14_valueChanged(double value)
{
    setOpiBar(ui->caLabel_13, ui->caLabel_14, ui->groupBox_3, ui->label_102);
    setOpiProgressBar(ui->caLabel_13
                      , ui->caLabel_14
                      , ui->caLabel_15
                      , ui->progressBar_3
                      , m_pTimer[2]
                      , m_nFlowCounter[2]);
}

void panel_tfopiform::on_caLabel_20_valueChanged(double value)
{
    setOpiBar(ui->caLabel_19, ui->caLabel_20, ui->groupBox_4, ui->label_103);
    setOpiProgressBar(ui->caLabel_19
                      , ui->caLabel_20
                      , ui->caLabel_21
                      , ui->progressBar_4
                      , m_pTimer[3]
                      , m_nFlowCounter[3]);
}

void panel_tfopiform::on_caLabel_26_valueChanged(double value)
{
    setOpiBar(ui->caLabel_25, ui->caLabel_26, ui->groupBox_5, ui->label_104);
    setOpiProgressBar(ui->caLabel_25
                      , ui->caLabel_26
                      , ui->caLabel_27
                      , ui->progressBar_5
                      , m_pTimer[4]
                      , m_nFlowCounter[4]);
}

void panel_tfopiform::on_caLabel_32_valueChanged(double value)
{
    setOpiBar(ui->caLabel_31, ui->caLabel_32, ui->groupBox_6, ui->label_105);
    setOpiProgressBar(ui->caLabel_31
                      , ui->caLabel_32
                      , ui->caLabel_33
                      , ui->progressBar_6
                      , m_pTimer[5]
                      , m_nFlowCounter[5]);
}

void panel_tfopiform::on_caLabel_38_valueChanged(double value)
{
    setOpiBar(ui->caLabel_37, ui->caLabel_38, ui->groupBox_7, ui->label_106);
    setOpiProgressBar(ui->caLabel_37
                      , ui->caLabel_38
                      , ui->caLabel_39
                      , ui->progressBar_7
                      , m_pTimer[6]
                      , m_nFlowCounter[6]);
}

void panel_tfopiform::on_caLabel_44_valueChanged(double value)
{
    setOpiBar(ui->caLabel_43, ui->caLabel_44, ui->groupBox_8, ui->label_107);
    setOpiProgressBar(ui->caLabel_43
                      , ui->caLabel_44
                      , ui->caLabel_45
                      , ui->progressBar_8
                      , m_pTimer[7]
                      , m_nFlowCounter[7]);
}

void panel_tfopiform::on_caLabel_50_valueChanged(double value)
{
    setOpiBar(ui->caLabel_49, ui->caLabel_50, ui->groupBox_9, ui->label_108);
    setOpiProgressBar(ui->caLabel_49
                      , ui->caLabel_50
                      , ui->caLabel_51
                      , ui->progressBar_9
                      , m_pTimer[8]
                      , m_nFlowCounter[8]);
}

void panel_tfopiform::on_caLabel_56_valueChanged(double value)
{
    setOpiBar(ui->caLabel_55, ui->caLabel_56, ui->groupBox_10, ui->label_109);
    setOpiProgressBar(ui->caLabel_55
                      , ui->caLabel_56
                      , ui->caLabel_57
                      , ui->progressBar_10
                      , m_pTimer[9]
                      , m_nFlowCounter[9]);
}

void panel_tfopiform::on_caLabel_62_valueChanged(double value)
{
    setOpiBar(ui->caLabel_61, ui->caLabel_62, ui->groupBox_11, ui->label_110);
    setOpiProgressBar(ui->caLabel_61
                      , ui->caLabel_62
                      , ui->caLabel_63
                      , ui->progressBar_11
                      , m_pTimer[10]
                      , m_nFlowCounter[10]);
}

void panel_tfopiform::on_caLabel_68_valueChanged(double value)
{
    setOpiBar(ui->caLabel_67, ui->caLabel_68, ui->groupBox_12, ui->label_111);
    setOpiProgressBar(ui->caLabel_67
                      , ui->caLabel_68
                      , ui->caLabel_69
                      , ui->progressBar_12
                      , m_pTimer[11]
                      , m_nFlowCounter[11]);
}

void panel_tfopiform::on_caLabel_74_valueChanged(double value)
{
    setOpiBar(ui->caLabel_73, ui->caLabel_74, ui->groupBox_13, ui->label_112);
    setOpiProgressBar(ui->caLabel_73
                      , ui->caLabel_74
                      , ui->caLabel_75
                      , ui->progressBar_13
                      , m_pTimer[12]
                      , m_nFlowCounter[12]);
}

void panel_tfopiform::on_caLabel_80_valueChanged(double value)
{
    setOpiBar(ui->caLabel_79, ui->caLabel_80, ui->groupBox_14, ui->label_113);
    setOpiProgressBar(ui->caLabel_79
                      , ui->caLabel_80
                      , ui->caLabel_81
                      , ui->progressBar_14
                      , m_pTimer[13]
                      , m_nFlowCounter[13]);
}

void panel_tfopiform::on_pushButton_1_clicked()
{
    if(m_pTFC0102BHV01AGrpForm == NULL){
        m_pTFC0102BHV01AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0102bhv01a.ui",1);
        if(m_pTFC0102BHV01AGrpForm)
            m_pTFC0102BHV01AGrpWindow->setCentralWidget(m_pTFC0102BHV01AGrpForm->GetWidget());
    }

    m_pTFC0102BHV01AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_2_clicked()
{
    if(m_pTFC1516BHV08AGrpForm == NULL){
        m_pTFC1516BHV08AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1516bhv08a.ui",1);
        if(m_pTFC1516BHV08AGrpForm)
            m_pTFC1516BHV08AGrpWindow ->setCentralWidget(m_pTFC1516BHV08AGrpForm->GetWidget());
    }

    m_pTFC1516BHV08AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_3_clicked()
{
    if(m_pTFC0304BHV02AGrpForm == NULL){
        m_pTFC0304BHV02AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0304bhv02a.ui",1);
        if(m_pTFC0304BHV02AGrpForm)
            m_pTFC0304BHV02AGrpWindow->setCentralWidget(m_pTFC0304BHV02AGrpForm->GetWidget());
    }

    m_pTFC0304BHV02AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_4_clicked()
{
    if(m_pTFM1304BHV17AGrpForm == NULL){
        m_pTFM1304BHV17AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfm1304bhv17a.ui",1);
        if(m_pTFM1304BHV17AGrpForm)
            m_pTFM1304BHV17AGrpWindow ->setCentralWidget(m_pTFM1304BHV17AGrpForm->GetWidget());
    }

    m_pTFM1304BHV17AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_5_clicked()
{
    if(m_pTFC0506BHV03AGrpForm == NULL){
        m_pTFC0506BHV03AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0506bhv03a.ui",1);
        if(m_pTFC0506BHV03AGrpForm)
            m_pTFC0506BHV03AGrpWindow->setCentralWidget(m_pTFC0506BHV03AGrpForm->GetWidget());
    }

    m_pTFC0506BHV03AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_6_clicked()
{
    if(m_pTFM0512BHV19AGrpForm == NULL){
        m_pTFM0512BHV19AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfm0512bhv19a.ui",1);
        if(m_pTFM0512BHV19AGrpForm)
            m_pTFM0512BHV19AGrpWindow ->setCentralWidget(m_pTFM0512BHV19AGrpForm->GetWidget());
    }

    m_pTFM0512BHV19AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_7_clicked()
{
    if(m_pTFC0708BHV04AGrpForm == NULL){
        m_pTFC0708BHV04AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0708bhv04a.ui",1);
        if(m_pTFC0708BHV04AGrpForm)
            m_pTFC0708BHV04AGrpWindow->setCentralWidget(m_pTFC0708BHV04AGrpForm->GetWidget());
    }

    m_pTFC0708BHV04AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_8_clicked()
{
    if(m_pTFBPBUSCHV01AGrpForm == NULL){
        m_pTFBPBUSCHV01AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbpbuschv01a.ui",1);
        if(m_pTFBPBUSCHV01AGrpForm)
            m_pTFBPBUSCHV01AGrpWindow->setCentralWidget(m_pTFBPBUSCHV01AGrpForm->GetWidget());
    }

    m_pTFBPBUSCHV01AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_9_clicked()
{
    if(m_pTFC0910BHV05AGrpForm == NULL){
        m_pTFC0910BHV05AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0910bhv05a.ui",1);
        if(m_pTFC0910BHV05AGrpForm)
            m_pTFC0910BHV05AGrpWindow->setCentralWidget(m_pTFC0910BHV05AGrpForm->GetWidget());
    }

    m_pTFC0910BHV05AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_10_clicked()
{
    if(m_pTFBNBUSCHV03AGrpForm == NULL){
        m_pTFBNBUSCHV03AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnbuschv03a.ui",1);
        if(m_pTFBNBUSCHV03AGrpForm)
            m_pTFBNBUSCHV03AGrpWindow->setCentralWidget(m_pTFBNBUSCHV03AGrpForm->GetWidget());
    }

    m_pTFBNBUSCHV03AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_11_clicked()
{
    if(m_pTFC1112BHV06AGrpForm == NULL){
        m_pTFC1112BHV06AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1112bhv06a.ui",1);
        if(m_pTFC1112BHV06AGrpForm)
            m_pTFC1112BHV06AGrpWindow ->setCentralWidget(m_pTFC1112BHV06AGrpForm->GetWidget());
    }

    m_pTFC1112BHV06AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_12_clicked()
{
    if(m_pTFBPRTBCHV05AGrpForm == NULL){
        m_pTFBPRTBCHV05AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbprtbchv05a.ui",1);
        if(m_pTFBPRTBCHV05AGrpForm)
            m_pTFBPRTBCHV05AGrpWindow->setCentralWidget(m_pTFBPRTBCHV05AGrpForm->GetWidget());
    }

    m_pTFBPRTBCHV05AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_13_clicked()
{
    if(m_pTFC1314BHV07AGrpForm == NULL){
        m_pTFC1314BHV07AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1314bhv07a.ui",1);
        if(m_pTFC1314BHV07AGrpForm)
            m_pTFC1314BHV07AGrpWindow ->setCentralWidget(m_pTFC1314BHV07AGrpForm->GetWidget());
    }

    m_pTFC1314BHV07AGrpWindow->show();
}

void panel_tfopiform::on_pushButton_14_clicked()
{
    if(m_pTFBNRTBCHV07AGrpForm == NULL){
        m_pTFBNRTBCHV07AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnrtbchv07a.ui",1);
        if(m_pTFBNRTBCHV07AGrpForm)
            m_pTFBNRTBCHV07AGrpWindow->setCentralWidget(m_pTFBNRTBCHV07AGrpForm->GetWidget());
    }

    m_pTFBNRTBCHV07AGrpWindow->show();
}


//Tab 01


//Tab 02

void panel_tfopiform::on_caLabel_86_valueChanged(double value)
{
    setOpiBar(ui->caLabel_85, ui->caLabel_86, ui->groupBox_15, ui->label_119);
    setOpiProgressBar(ui->caLabel_85
                      , ui->caLabel_86
                      , ui->caLabel_87
                      , ui->progressBar_15
                      , m_pTimer[14]
                      , m_nFlowCounter[14]);
}

void panel_tfopiform::on_caLabel_92_valueChanged(double value)
{
    setOpiBar(ui->caLabel_91, ui->caLabel_92, ui->groupBox_16, ui->label_120);
    setOpiProgressBar(ui->caLabel_91
                      , ui->caLabel_92
                      , ui->caLabel_93
                      , ui->progressBar_16
                      , m_pTimer[15]
                      , m_nFlowCounter[15]);
}

void panel_tfopiform::on_caLabel_98_valueChanged(double value)
{
    setOpiBar(ui->caLabel_97, ui->caLabel_98, ui->groupBox_17, ui->label_121);
    setOpiProgressBar(ui->caLabel_97
                      , ui->caLabel_98
                      , ui->caLabel_99
                      , ui->progressBar_17
                      , m_pTimer[16]
                      , m_nFlowCounter[16]);
}

void panel_tfopiform::on_caLabel_104_valueChanged(double value)
{
    setOpiBar(ui->caLabel_103, ui->caLabel_104, ui->groupBox_18, ui->label_122);
    setOpiProgressBar(ui->caLabel_103
                      , ui->caLabel_104
                      , ui->caLabel_105
                      , ui->progressBar_18
                      , m_pTimer[17]
                      , m_nFlowCounter[17]);
}

void panel_tfopiform::on_caLabel_110_valueChanged(double value)
{
    setOpiBar(ui->caLabel_109, ui->caLabel_110, ui->groupBox_19, ui->label_123);
    setOpiProgressBar(ui->caLabel_109
                      , ui->caLabel_110
                      , ui->caLabel_111
                      , ui->progressBar_19
                      , m_pTimer[18]
                      , m_nFlowCounter[18]);
}

void panel_tfopiform::on_caLabel_116_valueChanged(double value)
{
    setOpiBar(ui->caLabel_115, ui->caLabel_116, ui->groupBox_20, ui->label_124);
    setOpiProgressBar(ui->caLabel_115
                      , ui->caLabel_116
                      , ui->caLabel_117
                      , ui->progressBar_20
                      , m_pTimer[19]
                      , m_nFlowCounter[19]);
}

void panel_tfopiform::on_caLabel_122_valueChanged(double value)
{
    setOpiBar(ui->caLabel_121, ui->caLabel_122, ui->groupBox_21, ui->label_125);
    setOpiProgressBar(ui->caLabel_121
                      , ui->caLabel_122
                      , ui->caLabel_123
                      , ui->progressBar_21
                      , m_pTimer[20]
                      , m_nFlowCounter[20]);
}

void panel_tfopiform::on_caLabel_128_valueChanged(double value)
{
    setOpiBar(ui->caLabel_127, ui->caLabel_128, ui->groupBox_22, ui->label_126);
    setOpiProgressBar(ui->caLabel_127
                      , ui->caLabel_128
                      , ui->caLabel_129
                      , ui->progressBar_22
                      , m_pTimer[21]
                      , m_nFlowCounter[21]);
}

void panel_tfopiform::on_caLabel_134_valueChanged(double value)
{
    setOpiBar(ui->caLabel_133, ui->caLabel_134, ui->groupBox_23, ui->label_127);
    setOpiProgressBar(ui->caLabel_133
                      , ui->caLabel_134
                      , ui->caLabel_135
                      , ui->progressBar_23
                      , m_pTimer[22]
                      , m_nFlowCounter[22]);
}

void panel_tfopiform::on_caLabel_140_valueChanged(double value)
{
    setOpiBar(ui->caLabel_139, ui->caLabel_140, ui->groupBox_24, ui->label_128);
    setOpiProgressBar(ui->caLabel_139
                      , ui->caLabel_140
                      , ui->caLabel_141
                      , ui->progressBar_24
                      , m_pTimer[23]
                      , m_nFlowCounter[23]);
}

void panel_tfopiform::on_caLabel_146_valueChanged(double value)
{
    setOpiBar(ui->caLabel_145, ui->caLabel_146, ui->groupBox_25, ui->label_129);
    setOpiProgressBar(ui->caLabel_145
                      , ui->caLabel_146
                      , ui->caLabel_147
                      , ui->progressBar_25
                      , m_pTimer[24]
                      , m_nFlowCounter[24]);
}

void panel_tfopiform::on_caLabel_152_valueChanged(double value)
{
    setOpiBar(ui->caLabel_151, ui->caLabel_152, ui->groupBox_26, ui->label_130);
    setOpiProgressBar(ui->caLabel_151
                      , ui->caLabel_152
                      , ui->caLabel_153
                      , ui->progressBar_26
                      , m_pTimer[25]
                      , m_nFlowCounter[25]);
}

void panel_tfopiform::on_caLabel_158_valueChanged(double value)
{
    setOpiBar(ui->caLabel_157, ui->caLabel_158, ui->groupBox_27, ui->label_131);
    setOpiProgressBar(ui->caLabel_157
                      , ui->caLabel_158
                      , ui->caLabel_159
                      , ui->progressBar_27
                      , m_pTimer[26]
                      , m_nFlowCounter[26]);
}

void panel_tfopiform::on_caLabel_164_valueChanged(double value)
{
    setOpiBar(ui->caLabel_163, ui->caLabel_164, ui->groupBox_28, ui->label_132);
    setOpiProgressBar(ui->caLabel_163
                      , ui->caLabel_164
                      , ui->caLabel_165
                      , ui->progressBar_28
                      , m_pTimer[27]
                      , m_nFlowCounter[27]);
}

void panel_tfopiform::on_pushButton_15_clicked()
{
    if(m_pTFC0102BHV01BGrpForm == NULL){
        m_pTFC0102BHV01BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0102bhv01b.ui",1);
        if(m_pTFC0102BHV01BGrpForm)
            m_pTFC0102BHV01BGrpWindow->setCentralWidget(m_pTFC0102BHV01BGrpForm->GetWidget());
    }

    m_pTFC0102BHV01BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_16_clicked()
{
    if(m_pTFC1516BHV08BGrpForm == NULL){
        m_pTFC1516BHV08BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1516bhv08b.ui",1);
        if(m_pTFC1516BHV08BGrpForm)
            m_pTFC1516BHV08BGrpWindow ->setCentralWidget(m_pTFC1516BHV08BGrpForm->GetWidget());
    }

    m_pTFC1516BHV08BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_17_clicked()
{
    if(m_pTFC0304BHV02BGrpForm == NULL){
        m_pTFC0304BHV02BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0304bhv02b.ui",1);
        if(m_pTFC0304BHV02BGrpForm)
            m_pTFC0304BHV02BGrpWindow->setCentralWidget(m_pTFC0304BHV02BGrpForm->GetWidget());
    }

    m_pTFC0304BHV02BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_18_clicked()
{
    if(m_pTFM1304BHV18BGrpForm == NULL){
        m_pTFM1304BHV18BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfm1304bhv18b.ui",1);
        if(m_pTFM1304BHV18BGrpForm)
            m_pTFM1304BHV18BGrpWindow ->setCentralWidget(m_pTFM1304BHV18BGrpForm->GetWidget());
    }

    m_pTFM1304BHV18BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_19_clicked()
{
    if(m_pTFC0506BHV03BGrpForm == NULL){
        m_pTFC0506BHV03BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0506bhv03b.ui",1);
        if(m_pTFC0506BHV03BGrpForm)
            m_pTFC0506BHV03BGrpWindow->setCentralWidget(m_pTFC0506BHV03BGrpForm->GetWidget());
    }

    m_pTFC0506BHV03BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_20_clicked()
{
    if(m_pTFM0512BHV20BGrpForm == NULL){
        m_pTFM0512BHV20BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfm0512bhv20b.ui",1);
        if(m_pTFM0512BHV20BGrpForm)
            m_pTFM0512BHV20BGrpWindow ->setCentralWidget(m_pTFM0512BHV20BGrpForm->GetWidget());
    }

    m_pTFM0512BHV20BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_21_clicked()
{
    if(m_pTFC0708BHV04BGrpForm == NULL){
        m_pTFC0708BHV04BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0708bhv04b.ui",1);
        if(m_pTFC0708BHV04BGrpForm)
            m_pTFC0708BHV04BGrpWindow->setCentralWidget(m_pTFC0708BHV04BGrpForm->GetWidget());
    }

    m_pTFC0708BHV04BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_22_clicked()
{
    if(m_pTFBPBUSCHV02BGrpForm == NULL){
        m_pTFBPBUSCHV02BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbpbuschv02b.ui",1);
        if(m_pTFBPBUSCHV02BGrpForm)
            m_pTFBPBUSCHV02BGrpWindow->setCentralWidget(m_pTFBPBUSCHV02BGrpForm->GetWidget());
    }

    m_pTFBPBUSCHV02BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_23_clicked()
{
    if(m_pTFC0910BHV05BGrpForm == NULL){
        m_pTFC0910BHV05BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0910bhv05b.ui",1);
        if(m_pTFC0910BHV05BGrpForm)
            m_pTFC0910BHV05BGrpWindow ->setCentralWidget(m_pTFC0910BHV05BGrpForm->GetWidget());
    }

    m_pTFC0910BHV05BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_24_clicked()
{
    if(m_pTFBNBUSCHV04BGrpForm == NULL){
        m_pTFBNBUSCHV04BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnbuschv04b.ui",1);
        if(m_pTFBNBUSCHV04BGrpForm)
            m_pTFBNBUSCHV04BGrpWindow->setCentralWidget(m_pTFBNBUSCHV04BGrpForm->GetWidget());
    }

    m_pTFBNBUSCHV04BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_25_clicked()
{
    if(m_pTFC1112BHV06BGrpForm == NULL){
        m_pTFC1112BHV06BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1112bhv06b.ui",1);
        if(m_pTFC1112BHV06BGrpForm)
            m_pTFC1112BHV06BGrpWindow ->setCentralWidget(m_pTFC1112BHV06BGrpForm->GetWidget());
    }

    m_pTFC1112BHV06BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_26_clicked()
{
    if(m_pTFBPRTBCHV06BGrpForm == NULL){
        m_pTFBPRTBCHV06BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbprtbchv06b.ui",1);
        if(m_pTFBPRTBCHV06BGrpForm)
            m_pTFBPRTBCHV06BGrpWindow->setCentralWidget(m_pTFBPRTBCHV06BGrpForm->GetWidget());
    }

    m_pTFBPRTBCHV06BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_27_clicked()
{
    if(m_pTFC1314BHV07BGrpForm == NULL){
        m_pTFC1314BHV07BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1314bhv07b.ui",1);
        if(m_pTFC1314BHV07BGrpForm)
            m_pTFC1314BHV07BGrpWindow ->setCentralWidget(m_pTFC1314BHV07BGrpForm->GetWidget());
    }

    m_pTFC1314BHV07BGrpWindow->show();
}

void panel_tfopiform::on_pushButton_28_clicked()
{
    if(m_pTFBNRTBCHV08BGrpForm == NULL){
        m_pTFBNRTBCHV08BGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnrtbchv08b.ui",1);
        if(m_pTFBNRTBCHV08BGrpForm)
            m_pTFBNRTBCHV08BGrpWindow->setCentralWidget(m_pTFBNRTBCHV08BGrpForm->GetWidget());
    }

    m_pTFBNRTBCHV08BGrpWindow->show();
}

void panel_tfopiform::UpdatePFStop(const QString strStyle)
{
    ui->label_118->setStyleSheet(strStyle);
}

void panel_tfopiform::UpdatePFPrepare(const QString strStyle)
{
    ui->label_115->setStyleSheet(strStyle);
}

void panel_tfopiform::UpdatePFOperation(const QString strStyle)
{
    ui->label_117->setStyleSheet(strStyle);
}

void panel_tfopiform::UpdatePFQuench(const QString strStyle)
{
    ui->label_116->setStyleSheet(strStyle);
}
