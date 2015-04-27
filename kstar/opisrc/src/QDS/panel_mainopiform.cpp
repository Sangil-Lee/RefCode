#include "panel_mainopiform.h"
#include "ui_panel_mainopiform.h"
#include "QuenchDefine.h"

panel_mainopiform::panel_mainopiform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_mainopiform)
{
    ui->setupUi(this);

    mainopiform_pattach = new AttachChannelAccess(this);

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
    ui->caLabel_52->hide();
    ui->caLabel_56->hide();
    ui->caLabel_59->hide();
    ui->caLabel_60->hide();
    ui->caLabel_61->hide();
    ui->caLabel_64->hide();
    ui->caLabel_65->hide();
    ui->caLabel_66->hide();
    ui->caLabel_69->hide();
    ui->caLabel_70->hide();
    ui->caLabel_71->hide();
    ui->caLabel_74->hide();
    ui->caLabel_75->hide();
    ui->caLabel_76->hide();
    ui->caLabel_79->hide();
    ui->caLabel_80->hide();
    ui->caLabel_81->hide();
    ui->caLabel_84->hide();
    ui->caLabel_85->hide();
    ui->caLabel_86->hide();
    ui->caLabel_89->hide();
    ui->caLabel_90->hide();
    ui->caLabel_91->hide();
    ui->caLabel_95->hide();
    ui->caLabel_96->hide();
    ui->caLabel_97->hide();
    ui->caLabel_101->hide();
    ui->caLabel_102->hide();
    ui->caLabel_103->hide();
    ui->caLabel_107->hide();
    ui->caLabel_108->hide();
    ui->caLabel_109->hide();

    ui->caLabel_200->hide();
    ui->caLabel_201->hide();
    ui->caLabel_202->hide();
    ui->caLabel_203->hide();
    ui->caLabel_204->hide();
    ui->caLabel_205->hide();
    ui->caLabel_206->hide();
    ui->caLabel_207->hide();
    ui->caLabel_208->hide();
    ui->caLabel_209->hide();
    ui->caLabel_210->hide();
    ui->caLabel_211->hide();
    ui->caLabel_212->hide();
    ui->caLabel_213->hide();
    ui->caLabel_214->hide();
    ui->caLabel_215->hide();
    ui->caLabel_216->hide();
    ui->caLabel_217->hide();
    ui->caLabel_218->hide();
    ui->caLabel_219->hide();
    ui->caLabel_220->hide();
    ui->caLabel_221->hide();
    ui->caLabel_222->hide();
    ui->caLabel_223->hide();
    ui->caLabel_224->hide();
    ui->caLabel_225->hide();
    ui->caLabel_226->hide();
    ui->caLabel_227->hide();



    m_pTFBNBUSCHV03AGrpForm = NULL;
    m_pTFBNRTBCHV07AGrpForm = NULL;
    m_pTFBPBUSCHV01AGrpForm = NULL;
    m_pTFBPRTBCHV05AGrpForm = NULL;
    m_pTFC0102BHV01AGrpForm = NULL;
    m_pTFC0304BHV02AGrpForm = NULL;
    m_pTFC0506BHV03AGrpForm = NULL;
    m_pTFC0708BHV04AGrpForm = NULL;
    m_pTFC0910BHV05AGrpForm = NULL;
    m_pTFC1112BHV06AGrpForm = NULL;
    m_pTFC1314BHV07AGrpForm = NULL;
    m_pTFC1516BHV08AGrpForm = NULL;

    m_pPFC0100CHVN01GrpForm = NULL;
    m_pPFC0200CHVN03GrpForm = NULL;
    m_pPFC0300CHVN05GrpForm = NULL;
    m_pPFC0400CHVN07GrpForm = NULL;
    m_pPFC0500CHVN09GrpForm = NULL;
    m_pPFC06L0BHVN12GrpForm = NULL;
    m_pPFC06U0BHVN11GrpForm = NULL;
    m_pPFC0700BHVN15GrpForm = NULL;

    m_pPFC0100CHVN01GrpWindow = new QMainWindow();
    m_pPFC0200CHVN03GrpWindow = new QMainWindow();
    m_pPFC0300CHVN05GrpWindow = new QMainWindow();
    m_pPFC0400CHVN07GrpWindow = new QMainWindow();
    m_pPFC0500CHVN09GrpWindow = new QMainWindow();
    m_pPFC06L0BHVN12GrpWindow = new QMainWindow();
    m_pPFC06U0BHVN11GrpWindow = new QMainWindow();
    m_pPFC0700BHVN15GrpWindow = new QMainWindow();

    m_pTFBNBUSCHV03AGrpWindow = new QMainWindow();
    m_pTFBNRTBCHV07AGrpWindow = new QMainWindow();
    m_pTFBPBUSCHV01AGrpWindow = new QMainWindow();
    m_pTFBPRTBCHV05AGrpWindow = new QMainWindow();
    m_pTFC0102BHV01AGrpWindow = new QMainWindow();
    m_pTFC0304BHV02AGrpWindow = new QMainWindow();
    m_pTFC0506BHV03AGrpWindow = new QMainWindow();
    m_pTFC0708BHV04AGrpWindow = new QMainWindow();
    m_pTFC0910BHV05AGrpWindow = new QMainWindow();
    m_pTFC1112BHV06AGrpWindow = new QMainWindow();
    m_pTFC1314BHV07AGrpWindow = new QMainWindow();
    m_pTFC1516BHV08AGrpWindow = new QMainWindow();

    m_pTFBNBUSCHV03AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBNRTBCHV07AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPBUSCHV01AGrpWindow->setGeometry(30,30,540,400);
    m_pTFBPRTBCHV05AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0102BHV01AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0304BHV02AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0506BHV03AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0708BHV04AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC0910BHV05AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1112BHV06AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1314BHV07AGrpWindow->setGeometry(30,30,540,400);
    m_pTFC1516BHV08AGrpWindow->setGeometry(30,30,540,400);

    m_pPFC0100CHVN01GrpWindow->setGeometry(30,30,540,400);
    m_pPFC0200CHVN03GrpWindow->setGeometry(30,30,540,400);
    m_pPFC0300CHVN05GrpWindow->setGeometry(30,30,540,400);
    m_pPFC0400CHVN07GrpWindow->setGeometry(30,30,540,400);
    m_pPFC0500CHVN09GrpWindow->setGeometry(30,30,540,400);
    m_pPFC06L0BHVN12GrpWindow->setGeometry(30,30,540,400);
    m_pPFC06U0BHVN11GrpWindow->setGeometry(30,30,540,400);
    m_pPFC0700BHVN15GrpWindow->setGeometry(30,30,540,400);


    for(int i=0;i!=MAIN_OPI_COUNT;++i){
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

}

panel_mainopiform::~panel_mainopiform()
{
    delete ui;
}

void panel_mainopiform::changeEvent(QEvent *e)
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


void panel_mainopiform::OnUpdateTimer1()
{
    ui->progressBar_1->setValue(++m_nFlowCounter[0]*100);
}
void panel_mainopiform::OnUpdateTimer2()
{
    ui->progressBar_2->setValue(++m_nFlowCounter[1]*100);
}
void panel_mainopiform::OnUpdateTimer3()
{
    ui->progressBar_3->setValue(++m_nFlowCounter[2]*100);
}
void panel_mainopiform::OnUpdateTimer4()
{
    ui->progressBar_4->setValue(++m_nFlowCounter[3]*100);
}
void panel_mainopiform::OnUpdateTimer5()
{
    ui->progressBar_5->setValue(++m_nFlowCounter[4]*100);
}
void panel_mainopiform::OnUpdateTimer6()
{
    ui->progressBar_6->setValue(++m_nFlowCounter[5]*100);
}
void panel_mainopiform::OnUpdateTimer7()
{
    ui->progressBar_7->setValue(++m_nFlowCounter[6]*100);
}
void panel_mainopiform::OnUpdateTimer8()
{
    ui->progressBar_8->setValue(++m_nFlowCounter[7]*100);
}
void panel_mainopiform::OnUpdateTimer9()
{
    ui->progressBar_9->setValue(++m_nFlowCounter[8]*100);
}
void panel_mainopiform::OnUpdateTimer10()
{
    ui->progressBar_10->setValue(++m_nFlowCounter[9]*100);
}
void panel_mainopiform::OnUpdateTimer11()
{
    ui->progressBar_11->setValue(++m_nFlowCounter[10]*100);
}
void panel_mainopiform::OnUpdateTimer12()
{
    ui->progressBar_12->setValue(++m_nFlowCounter[11]*100);
}
void panel_mainopiform::OnUpdateTimer13()
{
    ui->progressBar_13->setValue(++m_nFlowCounter[12]*100);
}
void panel_mainopiform::OnUpdateTimer14()
{
    ui->progressBar_14->setValue(++m_nFlowCounter[13]*100);
}
void panel_mainopiform::OnUpdateTimer15()
{
    ui->progressBar_15->setValue(++m_nFlowCounter[14]*100);
}
void panel_mainopiform::OnUpdateTimer16()
{
    ui->progressBar_16->setValue(++m_nFlowCounter[15]*100);
}
void panel_mainopiform::OnUpdateTimer17()
{
    ui->progressBar_17->setValue(++m_nFlowCounter[16]*100);
}
void panel_mainopiform::OnUpdateTimer18()
{
    ui->progressBar_18->setValue(++m_nFlowCounter[17]*100);
}
void panel_mainopiform::OnUpdateTimer19()
{
    ui->progressBar_19->setValue(++m_nFlowCounter[18]*100);
}
void panel_mainopiform::OnUpdateTimer20()
{
    ui->progressBar_20->setValue(++m_nFlowCounter[19]*100);
}


void panel_mainopiform::on_caLabel_1_valueChanged(double value)
{
    setOpiBar(ui->caLabel_2, ui->caLabel_1, ui->groupBox_1, ui->label_96);
    setOpiProgressBar(ui->caLabel_2
                      , ui->caLabel_1
                      , ui->caLabel_3
                      , ui->progressBar_1
                      , m_pTimer[0]
                      , m_nFlowCounter[0]);
}

void panel_mainopiform::on_caLabel_8_valueChanged(double value)
{
    setOpiBar(ui->caLabel_7, ui->caLabel_8, ui->groupBox_2, ui->label_97);
    setOpiProgressBar(ui->caLabel_7
                      , ui->caLabel_8
                      , ui->caLabel_9
                      , ui->progressBar_2
                      , m_pTimer[1]
                      , m_nFlowCounter[1]);
}

void panel_mainopiform::on_caLabel_14_valueChanged(double value)
{
    setOpiBar(ui->caLabel_13, ui->caLabel_14, ui->groupBox_3, ui->label_98);
    setOpiProgressBar(ui->caLabel_13
                      , ui->caLabel_14
                      , ui->caLabel_15
                      , ui->progressBar_3
                      , m_pTimer[2]
                      , m_nFlowCounter[2]);
}

void panel_mainopiform::on_caLabel_20_valueChanged(double value)
{
    setOpiBar(ui->caLabel_19, ui->caLabel_20, ui->groupBox_4, ui->label_99);
    setOpiProgressBar(ui->caLabel_19
                      , ui->caLabel_20
                      , ui->caLabel_21
                      , ui->progressBar_4
                      , m_pTimer[3]
                      , m_nFlowCounter[3]);
}

void panel_mainopiform::on_caLabel_26_valueChanged(double value)
{
    setOpiBar(ui->caLabel_25, ui->caLabel_26, ui->groupBox_5, ui->label_100);
    setOpiProgressBar(ui->caLabel_25
                      , ui->caLabel_26
                      , ui->caLabel_27
                      , ui->progressBar_5
                      , m_pTimer[4]
                      , m_nFlowCounter[4]);
}

void panel_mainopiform::on_caLabel_32_valueChanged(double value)
{
    setOpiBar(ui->caLabel_31, ui->caLabel_32, ui->groupBox_6, ui->label_101);
    setOpiProgressBar(ui->caLabel_31
                      , ui->caLabel_32
                      , ui->caLabel_33
                      , ui->progressBar_6
                      , m_pTimer[5]
                      , m_nFlowCounter[5]);
}

void panel_mainopiform::on_caLabel_38_valueChanged(double value)
{
    setOpiBar(ui->caLabel_37, ui->caLabel_38, ui->groupBox_7, ui->label_102);
    setOpiProgressBar(ui->caLabel_37
                      , ui->caLabel_38
                      , ui->caLabel_39
                      , ui->progressBar_7
                      , m_pTimer[6]
                      , m_nFlowCounter[6]);
}

void panel_mainopiform::on_caLabel_44_valueChanged(double value)
{
    setOpiBar(ui->caLabel_43, ui->caLabel_44, ui->groupBox_8, ui->label_103);
    setOpiProgressBar(ui->caLabel_43
                      , ui->caLabel_44
                      , ui->caLabel_45
                      , ui->progressBar_8
                      , m_pTimer[7]
                      , m_nFlowCounter[7]);
}

void panel_mainopiform::on_caLabel_50_valueChanged(double value)
{
    setOpiBar(ui->caLabel_49, ui->caLabel_50, ui->groupBox_9, ui->label_104);
    setOpiProgressBar(ui->caLabel_49
                      , ui->caLabel_50
                      , ui->caLabel_51
                      , ui->progressBar_9
                      , m_pTimer[8]
                      , m_nFlowCounter[8]);
}

void panel_mainopiform::on_caLabel_55_valueChanged(double value)
{
    setOpiBar(ui->caLabel_52, ui->caLabel_55, ui->groupBox_10, ui->label_105);
    setOpiProgressBar(ui->caLabel_52
                      , ui->caLabel_55
                      , ui->caLabel_56
                      , ui->progressBar_10
                      , m_pTimer[9]
                      , m_nFlowCounter[9]);
}

void panel_mainopiform::on_caLabel_60_valueChanged(double value)
{
    setOpiBar(ui->caLabel_59, ui->caLabel_60, ui->groupBox_11, ui->label_106);
    setOpiProgressBar(ui->caLabel_59
                      , ui->caLabel_60
                      , ui->caLabel_61
                      , ui->progressBar_11
                      , m_pTimer[10]
                      , m_nFlowCounter[10]);
}

void panel_mainopiform::on_caLabel_65_valueChanged(double value)
{
    setOpiBar(ui->caLabel_64, ui->caLabel_65, ui->groupBox_12, ui->label_107);
    setOpiProgressBar(ui->caLabel_64
                      , ui->caLabel_65
                      , ui->caLabel_66
                      , ui->progressBar_12
                      , m_pTimer[11]
                      , m_nFlowCounter[11]);
}

void panel_mainopiform::on_caLabel_70_valueChanged(double value)
{
    setOpiBar(ui->caLabel_69, ui->caLabel_70, ui->groupBox_13, ui->label_108);
    setOpiProgressBar(ui->caLabel_69
                      , ui->caLabel_70
                      , ui->caLabel_71
                      , ui->progressBar_13
                      , m_pTimer[12]
                      , m_nFlowCounter[12]);
}

void panel_mainopiform::on_caLabel_75_valueChanged(double value)
{
    setOpiBar(ui->caLabel_74, ui->caLabel_75, ui->groupBox_14, ui->label_109);
    setOpiProgressBar(ui->caLabel_74
                      , ui->caLabel_75
                      , ui->caLabel_76
                      , ui->progressBar_14
                      , m_pTimer[13]
                      , m_nFlowCounter[13]);
}

void panel_mainopiform::on_caLabel_80_valueChanged(double value)
{
    setOpiBar(ui->caLabel_79, ui->caLabel_80, ui->groupBox_15, ui->label_110);
    setOpiProgressBar(ui->caLabel_79
                      , ui->caLabel_80
                      , ui->caLabel_81
                      , ui->progressBar_15
                      , m_pTimer[14]
                      , m_nFlowCounter[14]);
}

void panel_mainopiform::on_caLabel_85_valueChanged(double value)
{
    setOpiBar(ui->caLabel_84, ui->caLabel_85, ui->groupBox_16, ui->label_111);
    setOpiProgressBar(ui->caLabel_84
                      , ui->caLabel_85
                      , ui->caLabel_86
                      , ui->progressBar_16
                      , m_pTimer[15]
                      , m_nFlowCounter[15]);
}

void panel_mainopiform::on_caLabel_90_valueChanged(double value)
{
    setOpiBar(ui->caLabel_89, ui->caLabel_90, ui->groupBox_17, ui->label_112);
    setOpiProgressBar(ui->caLabel_89
                      , ui->caLabel_90
                      , ui->caLabel_91
                      , ui->progressBar_17
                      , m_pTimer[16]
                      , m_nFlowCounter[16]);
}

void panel_mainopiform::on_caLabel_96_valueChanged(double value)
{
    setOpiBar(ui->caLabel_95, ui->caLabel_96, ui->groupBox_18, ui->label_113);
    setOpiProgressBar(ui->caLabel_95
                      , ui->caLabel_96
                      , ui->caLabel_97
                      , ui->progressBar_18
                      , m_pTimer[17]
                      , m_nFlowCounter[17]);
}

void panel_mainopiform::on_caLabel_102_valueChanged(double value)
{
    setOpiBar(ui->caLabel_101, ui->caLabel_102, ui->groupBox_19, ui->label_114);
    setOpiProgressBar(ui->caLabel_101
                      , ui->caLabel_102
                      , ui->caLabel_103
                      , ui->progressBar_19
                      , m_pTimer[18]
                      , m_nFlowCounter[18]);
}

void panel_mainopiform::on_caLabel_108_valueChanged(double value)
{
    setOpiBar(ui->caLabel_107, ui->caLabel_108, ui->groupBox_20, ui->label_115);
    setOpiProgressBar(ui->caLabel_107
                      , ui->caLabel_108
                      , ui->caLabel_109
                      , ui->progressBar_20
                      , m_pTimer[19]
                      , m_nFlowCounter[19]);
}

void panel_mainopiform::on_pushButton_1_clicked()
{
    if(m_pTFC0102BHV01AGrpForm == NULL){
        m_pTFC0102BHV01AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0102bhv01a.ui",1);
        if(m_pTFC0102BHV01AGrpForm)
            m_pTFC0102BHV01AGrpWindow->setCentralWidget(m_pTFC0102BHV01AGrpForm->GetWidget());
    }

    m_pTFC0102BHV01AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_2_clicked()
{
    if(m_pTFC0304BHV02AGrpForm == NULL){
        m_pTFC0304BHV02AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0304bhv02a.ui",1);
        if(m_pTFC0304BHV02AGrpForm)
            m_pTFC0304BHV02AGrpWindow->setCentralWidget(m_pTFC0304BHV02AGrpForm->GetWidget());
    }

    m_pTFC0304BHV02AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_3_clicked()
{
    if(m_pTFC0506BHV03AGrpForm == NULL){
        m_pTFC0506BHV03AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0506bhv03a.ui",1);
        if(m_pTFC0506BHV03AGrpForm)
            m_pTFC0506BHV03AGrpWindow->setCentralWidget(m_pTFC0506BHV03AGrpForm->GetWidget());
    }

    m_pTFC0506BHV03AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_4_clicked()
{
    if(m_pTFC0708BHV04AGrpForm == NULL){
        m_pTFC0708BHV04AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0708bhv04a.ui",1);
        if(m_pTFC0708BHV04AGrpForm)
            m_pTFC0708BHV04AGrpWindow->setCentralWidget(m_pTFC0708BHV04AGrpForm->GetWidget());
    }

    m_pTFC0708BHV04AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_5_clicked()
{
    if(m_pTFC0910BHV05AGrpForm == NULL){
        m_pTFC0910BHV05AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc0910bhv05a.ui",1);
        if(m_pTFC0910BHV05AGrpForm)
            m_pTFC0910BHV05AGrpWindow->setCentralWidget(m_pTFC0910BHV05AGrpForm->GetWidget());
    }

    m_pTFC0910BHV05AGrpWindow->show();
}  //"/usr/local/src/Quench/ui/pfnew/PFC0700BHVN15.ui",1);

void panel_mainopiform::on_pushButton_6_clicked()
{
    if(m_pTFC1112BHV06AGrpForm == NULL){
        m_pTFC1112BHV06AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1112bhv06a.ui",1);
        if(m_pTFC1112BHV06AGrpForm)
            m_pTFC1112BHV06AGrpWindow->setCentralWidget(m_pTFC1112BHV06AGrpForm->GetWidget());
    }

    m_pTFC1112BHV06AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_7_clicked()
{
    if(m_pTFC1314BHV07AGrpForm == NULL){
        m_pTFC1314BHV07AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1314bhv07a.ui",1);
        if(m_pTFC1314BHV07AGrpForm)
            m_pTFC1314BHV07AGrpWindow->setCentralWidget(m_pTFC1314BHV07AGrpForm->GetWidget());
    }

    m_pTFC1314BHV07AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_8_clicked()
{
    if(m_pTFC1516BHV08AGrpForm == NULL){
        m_pTFC1516BHV08AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfc1516bhv08a.ui",1);
        if(m_pTFC1516BHV08AGrpForm)
            m_pTFC1516BHV08AGrpWindow->setCentralWidget(m_pTFC1516BHV08AGrpForm->GetWidget());
    }

    m_pTFC1516BHV08AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_17_clicked()
{
    if(m_pTFBPBUSCHV01AGrpForm == NULL){
        m_pTFBPBUSCHV01AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbpbuschv01a.ui",1);
        if(m_pTFBPBUSCHV01AGrpForm)
            m_pTFBPBUSCHV01AGrpWindow->setCentralWidget(m_pTFBPBUSCHV01AGrpForm->GetWidget());
    }

    m_pTFBPBUSCHV01AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_18_clicked()
{
    if(m_pTFBNBUSCHV03AGrpForm == NULL){
        m_pTFBNBUSCHV03AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnbuschv03a.ui",1);
        if(m_pTFBNBUSCHV03AGrpForm)
            m_pTFBNBUSCHV03AGrpWindow->setCentralWidget(m_pTFBNBUSCHV03AGrpForm->GetWidget());
    }

    m_pTFBNBUSCHV03AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_19_clicked()
{
    if(m_pTFBPRTBCHV05AGrpForm == NULL){
        m_pTFBPRTBCHV05AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbprtbchv05a.ui",1);
        if(m_pTFBPRTBCHV05AGrpForm)
            m_pTFBPRTBCHV05AGrpWindow->setCentralWidget(m_pTFBPRTBCHV05AGrpForm->GetWidget());
    }

    m_pTFBPRTBCHV05AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_20_clicked()
{
    if(m_pTFBNRTBCHV07AGrpForm == NULL){
        m_pTFBNRTBCHV07AGrpForm = new AttachChannelAccess("/usr/local/opi/ui/tf/tfbnrtbchv07a.ui",1);
        if(m_pTFBNRTBCHV07AGrpForm)
            m_pTFBNRTBCHV07AGrpWindow->setCentralWidget(m_pTFBNRTBCHV07AGrpForm->GetWidget());
    }

    m_pTFBNRTBCHV07AGrpWindow->show();
}

void panel_mainopiform::on_pushButton_9_clicked()
{
    if(m_pPFC0100CHVN01GrpForm == NULL){
        m_pPFC0100CHVN01GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc100chv01.ui",1);
        if(m_pPFC0100CHVN01GrpForm)   m_pPFC0100CHVN01GrpWindow->setCentralWidget(m_pPFC0100CHVN01GrpForm->GetWidget());
    }

    m_pPFC0100CHVN01GrpWindow->show();
}

void panel_mainopiform::on_pushButton_10_clicked()
{
    if(m_pPFC0200CHVN03GrpForm == NULL){
        m_pPFC0200CHVN03GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc200chvn03.ui",1);
        if(m_pPFC0200CHVN03GrpForm)   m_pPFC0200CHVN03GrpWindow->setCentralWidget(m_pPFC0200CHVN03GrpForm->GetWidget());
    }

    m_pPFC0200CHVN03GrpWindow->show();
}

void panel_mainopiform::on_pushButton_11_clicked()
{
    if(m_pPFC0300CHVN05GrpForm == NULL){
        m_pPFC0300CHVN05GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc0300chv05.ui",1);
        if(m_pPFC0300CHVN05GrpForm)   m_pPFC0300CHVN05GrpWindow->setCentralWidget(m_pPFC0300CHVN05GrpForm->GetWidget());
    }

    m_pPFC0300CHVN05GrpWindow->show();
}

void panel_mainopiform::on_pushButton_12_clicked()
{
    if(m_pPFC0400CHVN07GrpForm == NULL){
        m_pPFC0400CHVN07GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc0400chvn07.ui",1);
        if(m_pPFC0400CHVN07GrpForm)   m_pPFC0400CHVN07GrpWindow->setCentralWidget(m_pPFC0400CHVN07GrpForm->GetWidget());
    }

    m_pPFC0400CHVN07GrpWindow->show();
}

void panel_mainopiform::on_pushButton_13_clicked()
{
    if(m_pPFC0500CHVN09GrpForm == NULL){
        m_pPFC0500CHVN09GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc0500chvn09.ui",1);
        if(m_pPFC0500CHVN09GrpForm)   m_pPFC0500CHVN09GrpWindow->setCentralWidget(m_pPFC0500CHVN09GrpForm->GetWidget());
    }

    m_pPFC0500CHVN09GrpWindow->show();
}

void panel_mainopiform::on_pushButton_14_clicked()
{
    if(m_pPFC06U0BHVN11GrpForm == NULL){
        m_pPFC06U0BHVN11GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc06u0bhvn11.ui",1);
        if(m_pPFC06U0BHVN11GrpForm)   m_pPFC06U0BHVN11GrpWindow->setCentralWidget(m_pPFC06U0BHVN11GrpForm->GetWidget());
    }

    m_pPFC06U0BHVN11GrpWindow->show();
}

void panel_mainopiform::on_pushButton_15_clicked()
{

    if(m_pPFC06L0BHVN12GrpForm == NULL){
        m_pPFC06L0BHVN12GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc06l0bhvn12.ui",1);
        if(m_pPFC06L0BHVN12GrpForm)   m_pPFC06L0BHVN12GrpWindow->setCentralWidget(m_pPFC06L0BHVN12GrpForm->GetWidget());
    }

    m_pPFC06L0BHVN12GrpWindow->show();
}

void panel_mainopiform::on_pushButton_16_clicked()
{
    if(m_pPFC0700BHVN15GrpForm == NULL){
        m_pPFC0700BHVN15GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/pfc0700bhvn15.ui",1);
        if(m_pPFC0700BHVN15GrpForm)   m_pPFC0700BHVN15GrpWindow->setCentralWidget(m_pPFC0700BHVN15GrpForm->GetWidget());
    }

    m_pPFC0700BHVN15GrpWindow->show();
}

void panel_mainopiform::on_caLabel_200_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_201_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_202_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_203_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_204_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_205_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::on_caLabel_206_valueChanged(bool value)
{
    setPFStop();
}

void panel_mainopiform::setPFStop()
{
    QString strStyle;
    if(ui->caLabel_200->text() == "1"
       || ui->caLabel_201->text() == "1"
       || ui->caLabel_202->text() == "1"
       || ui->caLabel_203->text() == "1"
       || ui->caLabel_204->text() == "1"
       || ui->caLabel_205->text() == "1"
       || ui->caLabel_206->text() == "1"
       )
        strStyle = "background-color: rgb(0, 255, 0); color: rgb(255, 255, 255); border: 1px solid rgb(255, 255, 255);";
    else
        strStyle = "color: rgb(255, 255, 255); background-color: rgb(0, 0, 0);border: 1px solid rgb(255, 255, 255);";

    ui->label_27->setStyleSheet(strStyle);
    emit UpdatePFStop(strStyle);
}

void panel_mainopiform::on_caLabel_207_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_208_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_209_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_210_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_211_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_212_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::on_caLabel_213_valueChanged(bool value)
{
    setPFPrepare();
}

void panel_mainopiform::setPFPrepare()
{
    QString strStyle;
    if(ui->caLabel_207->text() == "1"
       || ui->caLabel_208->text() == "1"
       || ui->caLabel_209->text() == "1"
       || ui->caLabel_210->text() == "1"
       || ui->caLabel_211->text() == "1"
       || ui->caLabel_212->text() == "1"
       || ui->caLabel_213->text() == "1"
       )
        strStyle = "background-color: rgb(0, 255, 0); color: rgb(255, 255, 255);border: 1px solid rgb(255, 255, 255);";
    else
        strStyle = "color: rgb(255, 255, 255); background-color: rgb(0, 0, 0);border: 1px solid rgb(255, 255, 255);";

    ui->label_28->setStyleSheet(strStyle);
    emit UpdatePFPrepare(strStyle);
}

void panel_mainopiform::on_caLabel_214_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_215_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_216_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_217_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_218_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_219_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::on_caLabel_220_valueChanged(bool value)
{
    setPFOperation();
}

void panel_mainopiform::setPFOperation()
{
    QString strStyle;
    if(ui->caLabel_214->text() == "1"
       || ui->caLabel_215->text() == "1"
       || ui->caLabel_216->text() == "1"
       || ui->caLabel_217->text() == "1"
       || ui->caLabel_218->text() == "1"
       || ui->caLabel_219->text() == "1"
       || ui->caLabel_220->text() == "1"
       )
        strStyle = "background-color: rgb(0, 255, 0); color: rgb(255, 255, 255);border: 1px solid rgb(255, 255, 255);";
    else
        strStyle = "color: rgb(255, 255, 255); background-color: rgb(0, 0, 0);border: 1px solid rgb(255, 255, 255);";

    ui->label_29->setStyleSheet(strStyle);
    emit UpdatePFOperation(strStyle);
}


void panel_mainopiform::on_caLabel_221_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_222_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_223_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_224_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_225_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_226_valueChanged(bool value)
{
    setPFQuench();
}

void panel_mainopiform::on_caLabel_227_valueChanged(bool value)
{
    setPFQuench();
}
void panel_mainopiform::setPFQuench()
{
    QString strStyle;
    if(ui->caLabel_221->text() == "1"
       || ui->caLabel_222->text() == "1"
       || ui->caLabel_223->text() == "1"
       || ui->caLabel_224->text() == "1"
       || ui->caLabel_225->text() == "1"
       || ui->caLabel_226->text() == "1"
       || ui->caLabel_227->text() == "1"
       )
        strStyle = "background-color: rgb(255, 0, 0); color: rgb(255, 255, 255);border: 1px solid rgb(255, 255, 255);";
    else
        strStyle = "color: rgb(255, 255, 255); background-color: rgb(0, 0, 0);border: 1px solid rgb(255, 255, 255);";

    ui->label_30->setStyleSheet(strStyle);
    emit UpdatePFQuench(strStyle);
}
