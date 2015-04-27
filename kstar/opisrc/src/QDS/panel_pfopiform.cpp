#include "panel_pfopiform.h"
#include "ui_panel_pfopiform.h"
#include "QuenchDefine.h"

panel_pfopiform::panel_pfopiform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_pfopiform)
{
    ui->setupUi(this);

    pfopiform_pattach = new AttachChannelAccess(this);

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

     //Tab 02
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
     ui->caLabel_169->hide();
     ui->caLabel_170->hide();
     ui->caLabel_171->hide();
     ui->caLabel_175->hide();
     ui->caLabel_176->hide();
     ui->caLabel_177->hide();
     ui->caLabel_181->hide();
     ui->caLabel_182->hide();
     ui->caLabel_183->hide();
     ui->caLabel_187->hide();
     ui->caLabel_188->hide();
     ui->caLabel_189->hide();
     ui->caLabel_193->hide();
     ui->caLabel_194->hide();
     ui->caLabel_195->hide();
     ui->caLabel_199->hide();
     ui->caLabel_200->hide();
     ui->caLabel_201->hide();

     //Tab 03
     ui->caLabel_205->hide();
     ui->caLabel_206->hide();
     ui->caLabel_207->hide();
     ui->caLabel_211->hide();
     ui->caLabel_212->hide();
     ui->caLabel_213->hide();
     ui->caLabel_217->hide();
     ui->caLabel_218->hide();
     ui->caLabel_219->hide();
     ui->caLabel_223->hide();
     ui->caLabel_224->hide();
     ui->caLabel_225->hide();
     ui->caLabel_229->hide();
     ui->caLabel_230->hide();
     ui->caLabel_231->hide();
     ui->caLabel_235->hide();
     ui->caLabel_236->hide();
     ui->caLabel_237->hide();
     ui->caLabel_241->hide();
     ui->caLabel_242->hide();
     ui->caLabel_243->hide();
     ui->caLabel_247->hide();
     ui->caLabel_248->hide();
     ui->caLabel_249->hide();
     ui->caLabel_253->hide();
     ui->caLabel_254->hide();
     ui->caLabel_255->hide();
     ui->caLabel_259->hide();
     ui->caLabel_260->hide();
     ui->caLabel_261->hide();
     ui->caLabel_265->hide();
     ui->caLabel_266->hide();
     ui->caLabel_267->hide();
     ui->caLabel_271->hide();
     ui->caLabel_272->hide();
     ui->caLabel_273->hide();
     ui->caLabel_301->hide();
     ui->caLabel_302->hide();
     ui->caLabel_303->hide();
     ui->caLabel_307->hide();
     ui->caLabel_308->hide();
     ui->caLabel_309->hide();

     //Tab 04
     ui->caLabel_313->hide();
     ui->caLabel_314->hide();
     ui->caLabel_315->hide();
     ui->caLabel_319->hide();
     ui->caLabel_320->hide();
     ui->caLabel_321->hide();
     ui->caLabel_325->hide();
     ui->caLabel_326->hide();
     ui->caLabel_327->hide();
     ui->caLabel_331->hide();
     ui->caLabel_332->hide();
     ui->caLabel_333->hide();
     ui->caLabel_337->hide();
     ui->caLabel_338->hide();
     ui->caLabel_339->hide();
     ui->caLabel_343->hide();
     ui->caLabel_344->hide();
     ui->caLabel_345->hide();
     ui->caLabel_349->hide();
     ui->caLabel_350->hide();
     ui->caLabel_351->hide();
     ui->caLabel_355->hide();
     ui->caLabel_356->hide();
     ui->caLabel_357->hide();
     ui->caLabel_361->hide();
     ui->caLabel_362->hide();
     ui->caLabel_363->hide();
     ui->caLabel_367->hide();
     ui->caLabel_368->hide();
     ui->caLabel_369->hide();
     ui->caLabel_373->hide();
     ui->caLabel_374->hide();
     ui->caLabel_375->hide();
     ui->caLabel_379->hide();
     ui->caLabel_380->hide();
     ui->caLabel_381->hide();
     ui->caLabel_385->hide();
     ui->caLabel_386->hide();
     ui->caLabel_387->hide();
     ui->caLabel_391->hide();
     ui->caLabel_392->hide();
     ui->caLabel_393->hide();
     ui->caLabel_397->hide();
     ui->caLabel_398->hide();
     ui->caLabel_399->hide();
     ui->caLabel_403->hide();
     ui->caLabel_404->hide();
     ui->caLabel_405->hide();
     ui->caLabel_409->hide();
     ui->caLabel_410->hide();
     ui->caLabel_411->hide();
     ui->caLabel_415->hide();
     ui->caLabel_416->hide();
     ui->caLabel_417->hide();
     ui->caLabel_421->hide();
     ui->caLabel_422->hide();
     ui->caLabel_423->hide();
     ui->caLabel_427->hide();
     ui->caLabel_428->hide();
     ui->caLabel_429->hide();
     ui->caLabel_433->hide();
     ui->caLabel_434->hide();
     ui->caLabel_435->hide();
     ui->caLabel_439->hide();
     ui->caLabel_440->hide();
     ui->caLabel_441->hide();
     ui->caLabel_445->hide();
     ui->caLabel_446->hide();
     ui->caLabel_447->hide();
     ui->caLabel_451->hide();
     ui->caLabel_452->hide();
     ui->caLabel_453->hide();
     ui->caLabel_457->hide();
     ui->caLabel_458->hide();
     ui->caLabel_459->hide();



     // pf coil
     m_pPFC01LBHV22GrpWindow = new QMainWindow();
     m_pPFC01LCHV10GrpWindow = new QMainWindow();
     m_pPFC01UBHV21GrpWindow = new QMainWindow();
     m_pPFC01UCHV09GrpWindow = new QMainWindow();
     m_pPFC02LBHV24GrpWindow = new QMainWindow();
     m_pPFC02LCHV15GrpWindow = new QMainWindow();
     m_pPFC02UBHV23GrpWindow = new QMainWindow();
     m_pPFC02UCHV14GrpWindow = new QMainWindow();
     m_pPFC03LBHV26GrpWindow = new QMainWindow();
     m_pPFC03LCHV20GrpWindow = new QMainWindow();
     m_pPFC03UBHV25GrpWindow = new QMainWindow();
     m_pPFC03UCHV19GrpWindow = new QMainWindow();
     m_pPFC04LBHV28GrpWindow = new QMainWindow();
     m_pPFC04LCHV26GrpWindow = new QMainWindow();
     m_pPFC04UBHV27GrpWindow = new QMainWindow();
     m_pPFC04UCHV25GrpWindow = new QMainWindow();
     m_pPFC05LBHV30GrpWindow = new QMainWindow();
     m_pPFC05LCHV32GrpWindow = new QMainWindow();
     m_pPFC05UBHV29GrpWindow = new QMainWindow();
     m_pPFC05UCHV31GrpWindow = new QMainWindow();
     m_pPFC06LLBHV35GrpWindow = new QMainWindow();
     m_pPFC06LTBHV36GrpWindow = new QMainWindow();
     m_pPFC06LUBHV34GrpWindow = new QMainWindow();
     m_pPFC06TBHV37GrpWindow = new QMainWindow();
     m_pPFC06ULBHV32GrpWindow = new QMainWindow();
     m_pPFC06UTBHV33GrpWindow = new QMainWindow();
     m_pPFC06UUBHV31GrpWindow = new QMainWindow();
     m_pPFC07LBHV39GrpWindow = new QMainWindow();
     m_pPFC07TBHV40GrpWindow = new QMainWindow();
     m_pPFC07UBHV38GrpWindow = new QMainWindow();

     // pf bus
     m_pPFB01ICHV13GrpWindow = new QMainWindow();
     m_pPFB01LPCHV12GrpWindow = new QMainWindow();
     m_pPFB01UNCHV11GrpWindow = new QMainWindow();
     m_pPFB02ICHV18GrpWindow = new QMainWindow();
     m_pPFB02LNCHV17GrpWindow = new QMainWindow();
     m_pPFB02UPCHV16GrpWindow = new QMainWindow();
     m_pPFB03LNCHV24GrpWindow = new QMainWindow();
     m_pPFB03LPCHV23GrpWindow = new QMainWindow();
     m_pPFB03UNCHV22GrpWindow = new QMainWindow();
     m_pPFB03UPCHV21GrpWindow = new QMainWindow();
     m_pPFB04LNCHV30GrpWindow = new QMainWindow();
     m_pPFB04LPCHV29GrpWindow = new QMainWindow();
     m_pPFB04UNCHV28GrpWindow = new QMainWindow();
     m_pPFB04UPCHV27GrpWindow = new QMainWindow();
     m_pPFB05LNCHV36GrpWindow = new QMainWindow();
     m_pPFB05LPCHV35GrpWindow = new QMainWindow();
     m_pPFB05UNCHV34GrpWindow = new QMainWindow();
     m_pPFB05UPCHV33GrpWindow = new QMainWindow();
     m_pPFB06LNCHV40GrpWindow = new QMainWindow();
     m_pPFB06LPCHV39GrpWindow = new QMainWindow();
     m_pPFB06UNCHV38GrpWindow = new QMainWindow();
     m_pPFB06UPCHV37GrpWindow = new QMainWindow();
     m_pPFB07ICHV43GrpWindow = new QMainWindow();
     m_pPFB07LNCHV42GrpWindow = new QMainWindow();
     m_pPFB07UPCHV41GrpWindow = new QMainWindow();

     // pf new
     m_pPFC0100CHVN01GrpWindow = new QMainWindow();
     m_pPFC0200CHVN03GrpWindow = new QMainWindow();
     m_pPFC0300CHVN05GrpWindow = new QMainWindow();
     m_pPFC0400CHVN07GrpWindow = new QMainWindow();
     m_pPFC0500CHVN09GrpWindow = new QMainWindow();
     m_pPFC06L0BHVN12GrpWindow = new QMainWindow();
     m_pPFC06U0BHVN11GrpWindow = new QMainWindow();
     m_pPFC0700BHVN15GrpWindow = new QMainWindow();

     // pf coil
     m_pPFC01LBHV22GrpWindow->setGeometry(30,30,540,400);
     m_pPFC01LCHV10GrpWindow->setGeometry(30,30,540,400);
     m_pPFC01UBHV21GrpWindow->setGeometry(30,30,540,400);
     m_pPFC01UCHV09GrpWindow->setGeometry(30,30,540,400);
     m_pPFC02LBHV24GrpWindow->setGeometry(30,30,540,400);
     m_pPFC02LCHV15GrpWindow->setGeometry(30,30,540,400);
     m_pPFC02UBHV23GrpWindow->setGeometry(30,30,540,400);
     m_pPFC02UCHV14GrpWindow->setGeometry(30,30,540,400);
     m_pPFC03LBHV26GrpWindow->setGeometry(30,30,540,400);
     m_pPFC03LCHV20GrpWindow->setGeometry(30,30,540,400);
     m_pPFC03UBHV25GrpWindow->setGeometry(30,30,540,400);
     m_pPFC03UCHV19GrpWindow->setGeometry(30,30,540,400);
     m_pPFC04LBHV28GrpWindow->setGeometry(30,30,540,400);
     m_pPFC04LCHV26GrpWindow->setGeometry(30,30,540,400);
     m_pPFC04UBHV27GrpWindow->setGeometry(30,30,540,400);
     m_pPFC04UCHV25GrpWindow->setGeometry(30,30,540,400);
     m_pPFC05LBHV30GrpWindow->setGeometry(30,30,540,400);
     m_pPFC05LCHV32GrpWindow->setGeometry(30,30,540,400);
     m_pPFC05UBHV29GrpWindow->setGeometry(30,30,540,400);
     m_pPFC05UCHV31GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06LLBHV35GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06LTBHV36GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06LUBHV34GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06TBHV37GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06ULBHV32GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06UTBHV33GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06UUBHV31GrpWindow->setGeometry(30,30,540,400);
     m_pPFC07LBHV39GrpWindow->setGeometry(30,30,540,400);
     m_pPFC07TBHV40GrpWindow->setGeometry(30,30,540,400);
     m_pPFC07UBHV38GrpWindow->setGeometry(30,30,540,400);

     // pf bus
     m_pPFB01ICHV13GrpWindow->setGeometry(30,30,540,400);
     m_pPFB01LPCHV12GrpWindow->setGeometry(30,30,540,400);
     m_pPFB01UNCHV11GrpWindow->setGeometry(30,30,540,400);
     m_pPFB02ICHV18GrpWindow->setGeometry(30,30,540,400);
     m_pPFB02LNCHV17GrpWindow->setGeometry(30,30,540,400);
     m_pPFB02UPCHV16GrpWindow->setGeometry(30,30,540,400);
     m_pPFB03LNCHV24GrpWindow->setGeometry(30,30,540,400);
     m_pPFB03LPCHV23GrpWindow->setGeometry(30,30,540,400);
     m_pPFB03UNCHV22GrpWindow->setGeometry(30,30,540,400);
     m_pPFB03UPCHV21GrpWindow->setGeometry(30,30,540,400);
     m_pPFB04LNCHV30GrpWindow->setGeometry(30,30,540,400);
     m_pPFB04LPCHV29GrpWindow->setGeometry(30,30,540,400);
     m_pPFB04UNCHV28GrpWindow->setGeometry(30,30,540,400);
     m_pPFB04UPCHV27GrpWindow->setGeometry(30,30,540,400);
     m_pPFB05LNCHV36GrpWindow->setGeometry(30,30,540,400);
     m_pPFB05LPCHV35GrpWindow->setGeometry(30,30,540,400);
     m_pPFB05UNCHV34GrpWindow->setGeometry(30,30,540,400);
     m_pPFB05UPCHV33GrpWindow->setGeometry(30,30,540,400);
     m_pPFB06LNCHV40GrpWindow->setGeometry(30,30,540,400);
     m_pPFB06LPCHV39GrpWindow->setGeometry(30,30,540,400);
     m_pPFB06UNCHV38GrpWindow->setGeometry(30,30,540,400);
     m_pPFB06UPCHV37GrpWindow->setGeometry(30,30,540,400);
     m_pPFB07ICHV43GrpWindow->setGeometry(30,30,540,400);
     m_pPFB07LNCHV42GrpWindow->setGeometry(30,30,540,400);
     m_pPFB07UPCHV41GrpWindow->setGeometry(30,30,540,400);

     // pf new
     m_pPFC0100CHVN01GrpWindow->setGeometry(30,30,540,400);
     m_pPFC0200CHVN03GrpWindow->setGeometry(30,30,540,400);
     m_pPFC0300CHVN05GrpWindow->setGeometry(30,30,540,400);
     m_pPFC0400CHVN07GrpWindow->setGeometry(30,30,540,400);
     m_pPFC0500CHVN09GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06L0BHVN12GrpWindow->setGeometry(30,30,540,400);
     m_pPFC06U0BHVN11GrpWindow->setGeometry(30,30,540,400);
     m_pPFC0700BHVN15GrpWindow->setGeometry(30,30,540,400);

     m_pPFC01LBHV22GrpForm = NULL;
     m_pPFC01LCHV10GrpForm = NULL;
     m_pPFC01UBHV21GrpForm = NULL;
     m_pPFC01UCHV09GrpForm = NULL;
     m_pPFC02LBHV24GrpForm = NULL;
     m_pPFC02LCHV15GrpForm = NULL;
     m_pPFC02UBHV23GrpForm = NULL;
     m_pPFC02UCHV14GrpForm = NULL;
     m_pPFC03LBHV26GrpForm = NULL;
     m_pPFC03LCHV20GrpForm = NULL;
     m_pPFC03UBHV25GrpForm = NULL;
     m_pPFC03UCHV19GrpForm = NULL;
     m_pPFC04LBHV28GrpForm = NULL;
     m_pPFC04LCHV26GrpForm = NULL;
     m_pPFC04UBHV27GrpForm = NULL;
     m_pPFC04UCHV25GrpForm = NULL;
     m_pPFC05LBHV30GrpForm = NULL;
     m_pPFC05LCHV32GrpForm = NULL;
     m_pPFC05UBHV29GrpForm = NULL;
     m_pPFC05UCHV31GrpForm = NULL;
     m_pPFC06LLBHV35GrpForm = NULL;
     m_pPFC06LTBHV36GrpForm = NULL;
     m_pPFC06LUBHV34GrpForm = NULL;
     m_pPFC06TBHV37GrpForm = NULL;
     m_pPFC06ULBHV32GrpForm = NULL;
     m_pPFC06UTBHV33GrpForm = NULL;
     m_pPFC06UUBHV31GrpForm = NULL;
     m_pPFC07LBHV39GrpForm = NULL;
     m_pPFC07TBHV40GrpForm = NULL;
     m_pPFC07UBHV38GrpForm = NULL;

     m_pPFB01ICHV13GrpForm = NULL;
     m_pPFB01LPCHV12GrpForm = NULL;
     m_pPFB01UNCHV11GrpForm = NULL;
     m_pPFB02ICHV18GrpForm = NULL;
     m_pPFB02LNCHV17GrpForm = NULL;
     m_pPFB02UPCHV16GrpForm = NULL;
     m_pPFB03LNCHV24GrpForm = NULL;
     m_pPFB03LPCHV23GrpForm = NULL;
     m_pPFB03UNCHV22GrpForm = NULL;
     m_pPFB03UPCHV21GrpForm = NULL;
     m_pPFB04LNCHV30GrpForm = NULL;
     m_pPFB04LPCHV29GrpForm = NULL;
     m_pPFB04UNCHV28GrpForm = NULL;
     m_pPFB04UPCHV27GrpForm = NULL;
     m_pPFB05LNCHV36GrpForm = NULL;
     m_pPFB05LPCHV35GrpForm = NULL;
     m_pPFB05UNCHV34GrpForm = NULL;
     m_pPFB05UPCHV33GrpForm = NULL;
     m_pPFB06LNCHV40GrpForm = NULL;
     m_pPFB06LPCHV39GrpForm = NULL;
     m_pPFB06UNCHV38GrpForm = NULL;
     m_pPFB06UPCHV37GrpForm = NULL;
     m_pPFB07ICHV43GrpForm = NULL;
     m_pPFB07LNCHV42GrpForm = NULL;
     m_pPFB07UPCHV41GrpForm = NULL;

     m_pPFC0100CHVN01GrpForm = NULL;
     m_pPFC0200CHVN03GrpForm = NULL;
     m_pPFC0300CHVN05GrpForm = NULL;
     m_pPFC0400CHVN07GrpForm = NULL;
     m_pPFC0500CHVN09GrpForm = NULL;
     m_pPFC06L0BHVN12GrpForm = NULL;
     m_pPFC06U0BHVN11GrpForm = NULL;
     m_pPFC0700BHVN15GrpForm = NULL;

     for(int i=0;i!=PF_OPI_COUNT;++i){
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

     connect(m_pTimer[28],SIGNAL(timeout()),this,SLOT(OnUpdateTimer29()));
     connect(m_pTimer[29],SIGNAL(timeout()),this,SLOT(OnUpdateTimer30()));
     connect(m_pTimer[30],SIGNAL(timeout()),this,SLOT(OnUpdateTimer31()));
     connect(m_pTimer[31],SIGNAL(timeout()),this,SLOT(OnUpdateTimer32()));
     connect(m_pTimer[32],SIGNAL(timeout()),this,SLOT(OnUpdateTimer33()));
     connect(m_pTimer[33],SIGNAL(timeout()),this,SLOT(OnUpdateTimer34()));
     connect(m_pTimer[34],SIGNAL(timeout()),this,SLOT(OnUpdateTimer35()));
     connect(m_pTimer[35],SIGNAL(timeout()),this,SLOT(OnUpdateTimer36()));
     connect(m_pTimer[36],SIGNAL(timeout()),this,SLOT(OnUpdateTimer37()));
     connect(m_pTimer[37],SIGNAL(timeout()),this,SLOT(OnUpdateTimer38()));
     connect(m_pTimer[38],SIGNAL(timeout()),this,SLOT(OnUpdateTimer39()));
     connect(m_pTimer[39],SIGNAL(timeout()),this,SLOT(OnUpdateTimer40()));
     connect(m_pTimer[40],SIGNAL(timeout()),this,SLOT(OnUpdateTimer41()));
     connect(m_pTimer[41],SIGNAL(timeout()),this,SLOT(OnUpdateTimer42()));
     connect(m_pTimer[42],SIGNAL(timeout()),this,SLOT(OnUpdateTimer43()));
     connect(m_pTimer[43],SIGNAL(timeout()),this,SLOT(OnUpdateTimer44()));
     connect(m_pTimer[44],SIGNAL(timeout()),this,SLOT(OnUpdateTimer45()));
     connect(m_pTimer[45],SIGNAL(timeout()),this,SLOT(OnUpdateTimer46()));
     connect(m_pTimer[46],SIGNAL(timeout()),this,SLOT(OnUpdateTimer47()));
     connect(m_pTimer[47],SIGNAL(timeout()),this,SLOT(OnUpdateTimer48()));
     connect(m_pTimer[48],SIGNAL(timeout()),this,SLOT(OnUpdateTimer49()));
     connect(m_pTimer[49],SIGNAL(timeout()),this,SLOT(OnUpdateTimer50()));
     connect(m_pTimer[50],SIGNAL(timeout()),this,SLOT(OnUpdateTimer51()));
     connect(m_pTimer[51],SIGNAL(timeout()),this,SLOT(OnUpdateTimer52()));
     connect(m_pTimer[52],SIGNAL(timeout()),this,SLOT(OnUpdateTimer53()));
     connect(m_pTimer[53],SIGNAL(timeout()),this,SLOT(OnUpdateTimer54()));
     connect(m_pTimer[54],SIGNAL(timeout()),this,SLOT(OnUpdateTimer55()));

     connect(m_pTimer[55],SIGNAL(timeout()),this,SLOT(OnUpdateTimer56()));
     connect(m_pTimer[56],SIGNAL(timeout()),this,SLOT(OnUpdateTimer57()));
     connect(m_pTimer[57],SIGNAL(timeout()),this,SLOT(OnUpdateTimer58()));
     connect(m_pTimer[58],SIGNAL(timeout()),this,SLOT(OnUpdateTimer59()));
     connect(m_pTimer[59],SIGNAL(timeout()),this,SLOT(OnUpdateTimer60()));
     connect(m_pTimer[60],SIGNAL(timeout()),this,SLOT(OnUpdateTimer61()));
     connect(m_pTimer[61],SIGNAL(timeout()),this,SLOT(OnUpdateTimer62()));
     connect(m_pTimer[62],SIGNAL(timeout()),this,SLOT(OnUpdateTimer63()));
     connect(m_pTimer[63],SIGNAL(timeout()),this,SLOT(OnUpdateTimer64()));
     connect(m_pTimer[64],SIGNAL(timeout()),this,SLOT(OnUpdateTimer65()));
     connect(m_pTimer[65],SIGNAL(timeout()),this,SLOT(OnUpdateTimer66()));
     connect(m_pTimer[66],SIGNAL(timeout()),this,SLOT(OnUpdateTimer67()));
     connect(m_pTimer[67],SIGNAL(timeout()),this,SLOT(OnUpdateTimer68()));
     connect(m_pTimer[68],SIGNAL(timeout()),this,SLOT(OnUpdateTimer69()));
     connect(m_pTimer[69],SIGNAL(timeout()),this,SLOT(OnUpdateTimer70()));
     connect(m_pTimer[70],SIGNAL(timeout()),this,SLOT(OnUpdateTimer71()));
     connect(m_pTimer[71],SIGNAL(timeout()),this,SLOT(OnUpdateTimer72()));
     connect(m_pTimer[72],SIGNAL(timeout()),this,SLOT(OnUpdateTimer73()));

     ui->tabWidget->setStyle(new QWindowsStyle);

}

panel_pfopiform::~panel_pfopiform()
{
    delete ui;
}

void panel_pfopiform::changeEvent(QEvent *e)
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

void panel_pfopiform::initConnect()
{
    connect(m_pMainOpiForm,SIGNAL(UpdatePFOperation(QString)),this,SLOT(UpdatePFOperation(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFStop(QString)),this,SLOT(UpdatePFStop(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFPrepare(QString)),this,SLOT(UpdatePFPrepare(QString)));
    connect(m_pMainOpiForm,SIGNAL(UpdatePFQuench(QString)),this,SLOT(UpdatePFQuench(QString)));
}



//Tab 01

void panel_pfopiform::OnUpdateTimer1()
{
    ui->progressBar_1->setValue(++m_nFlowCounter[0]*100);
}
void panel_pfopiform::OnUpdateTimer2()
{
    ui->progressBar_2->setValue(++m_nFlowCounter[1]*100);
}
void panel_pfopiform::OnUpdateTimer3()
{
    ui->progressBar_3->setValue(++m_nFlowCounter[2]*100);
}
void panel_pfopiform::OnUpdateTimer4()
{
    ui->progressBar_4->setValue(++m_nFlowCounter[3]*100);
}
void panel_pfopiform::OnUpdateTimer5()
{
    ui->progressBar_5->setValue(++m_nFlowCounter[4]*100);
}
void panel_pfopiform::OnUpdateTimer6()
{
    ui->progressBar_6->setValue(++m_nFlowCounter[5]*100);
}
void panel_pfopiform::OnUpdateTimer7()
{
    ui->progressBar_7->setValue(++m_nFlowCounter[6]*100);
}
void panel_pfopiform::OnUpdateTimer8()
{
    ui->progressBar_8->setValue(++m_nFlowCounter[7]*100);
}
void panel_pfopiform::OnUpdateTimer9()
{
    ui->progressBar_9->setValue(++m_nFlowCounter[8]*100);
}
void panel_pfopiform::OnUpdateTimer10()
{
    ui->progressBar_10->setValue(++m_nFlowCounter[9]*100);
}
void panel_pfopiform::OnUpdateTimer11()
{
    ui->progressBar_11->setValue(++m_nFlowCounter[10]*100);
}
void panel_pfopiform::OnUpdateTimer12()
{
    ui->progressBar_12->setValue(++m_nFlowCounter[11]*100);
}
void panel_pfopiform::OnUpdateTimer13()
{
    ui->progressBar_13->setValue(++m_nFlowCounter[12]*100);
}
void panel_pfopiform::OnUpdateTimer14()
{
    ui->progressBar_14->setValue(++m_nFlowCounter[13]*100);
}
void panel_pfopiform::OnUpdateTimer15()
{
    ui->progressBar_15->setValue(++m_nFlowCounter[14]*100);
}
void panel_pfopiform::OnUpdateTimer16()
{
    ui->progressBar_16->setValue(++m_nFlowCounter[15]*100);
}
void panel_pfopiform::OnUpdateTimer17()
{
    ui->progressBar_17->setValue(++m_nFlowCounter[16]*100);
}
void panel_pfopiform::OnUpdateTimer18()
{
    ui->progressBar_18->setValue(++m_nFlowCounter[17]*100);
}

void panel_pfopiform::on_caLabel_1_valueChanged(double value)
{
    setOpiBar(ui->caLabel_2, ui->caLabel_1, ui->groupBox_1, ui->label_96);
    setOpiProgressBar(ui->caLabel_2
                      , ui->caLabel_1
                      , ui->caLabel_3
                      , ui->progressBar_1
                      , m_pTimer[0]
                      , m_nFlowCounter[0]);
}

void panel_pfopiform::on_caLabel_8_valueChanged(double value)
{
    setOpiBar(ui->caLabel_7, ui->caLabel_8, ui->groupBox_2, ui->label_97);
    setOpiProgressBar(ui->caLabel_7
                      , ui->caLabel_8
                      , ui->caLabel_9
                      , ui->progressBar_2
                      , m_pTimer[1]
                      , m_nFlowCounter[1]);
}

void panel_pfopiform::on_caLabel_14_valueChanged(double value)
{
    setOpiBar(ui->caLabel_13, ui->caLabel_14, ui->groupBox_3, ui->label_98);
    setOpiProgressBar(ui->caLabel_13
                      , ui->caLabel_14
                      , ui->caLabel_15
                      , ui->progressBar_3
                      , m_pTimer[2]
                      , m_nFlowCounter[2]);
}

void panel_pfopiform::on_caLabel_20_valueChanged(double value)
{
    setOpiBar(ui->caLabel_19, ui->caLabel_20, ui->groupBox_4, ui->label_99);
    setOpiProgressBar(ui->caLabel_19
                      , ui->caLabel_20
                      , ui->caLabel_21
                      , ui->progressBar_4
                      , m_pTimer[3]
                      , m_nFlowCounter[3]);
}

void panel_pfopiform::on_caLabel_26_valueChanged(double value)
{
    setOpiBar(ui->caLabel_25, ui->caLabel_26, ui->groupBox_5, ui->label_100);
    setOpiProgressBar(ui->caLabel_25
                      , ui->caLabel_26
                      , ui->caLabel_27
                      , ui->progressBar_5
                      , m_pTimer[4]
                      , m_nFlowCounter[4]);
}

void panel_pfopiform::on_caLabel_32_valueChanged(double value)
{
    setOpiBar(ui->caLabel_31, ui->caLabel_32, ui->groupBox_6, ui->label_101);
    setOpiProgressBar(ui->caLabel_31
                      , ui->caLabel_32
                      , ui->caLabel_33
                      , ui->progressBar_6
                      , m_pTimer[5]
                      , m_nFlowCounter[5]);
}

void panel_pfopiform::on_caLabel_38_valueChanged(double value)
{
    setOpiBar(ui->caLabel_37, ui->caLabel_38, ui->groupBox_7, ui->label_102);
    setOpiProgressBar(ui->caLabel_37
                      , ui->caLabel_38
                      , ui->caLabel_39
                      , ui->progressBar_7
                      , m_pTimer[6]
                      , m_nFlowCounter[6]);
}

void panel_pfopiform::on_caLabel_44_valueChanged(double value)
{
    setOpiBar(ui->caLabel_43, ui->caLabel_44, ui->groupBox_8, ui->label_103);
    setOpiProgressBar(ui->caLabel_43
                      , ui->caLabel_44
                      , ui->caLabel_45
                      , ui->progressBar_8
                      , m_pTimer[7]
                      , m_nFlowCounter[7]);
}

void panel_pfopiform::on_caLabel_50_valueChanged(double value)
{
    setOpiBar(ui->caLabel_49, ui->caLabel_50, ui->groupBox_9, ui->label_104);
    setOpiProgressBar(ui->caLabel_49
                      , ui->caLabel_50
                      , ui->caLabel_51
                      , ui->progressBar_9
                      , m_pTimer[8]
                      , m_nFlowCounter[8]);
}

void panel_pfopiform::on_caLabel_56_valueChanged(double value)
{
    setOpiBar(ui->caLabel_55, ui->caLabel_56, ui->groupBox_10, ui->label_105);
    setOpiProgressBar(ui->caLabel_55
                      , ui->caLabel_56
                      , ui->caLabel_57
                      , ui->progressBar_10
                      , m_pTimer[9]
                      , m_nFlowCounter[9]);
}

void panel_pfopiform::on_caLabel_62_valueChanged(double value)
{
    setOpiBar(ui->caLabel_61, ui->caLabel_62, ui->groupBox_11, ui->label_106);
    setOpiProgressBar(ui->caLabel_61
                      , ui->caLabel_62
                      , ui->caLabel_63
                      , ui->progressBar_11
                      , m_pTimer[10]
                      , m_nFlowCounter[10]);
}

void panel_pfopiform::on_caLabel_68_valueChanged(double value)
{
    setOpiBar(ui->caLabel_67, ui->caLabel_68, ui->groupBox_12, ui->label_107);
    setOpiProgressBar(ui->caLabel_67
                      , ui->caLabel_68
                      , ui->caLabel_69
                      , ui->progressBar_12
                      , m_pTimer[11]
                      , m_nFlowCounter[11]);
}

void panel_pfopiform::on_caLabel_74_valueChanged(double value)
{
    setOpiBar(ui->caLabel_73, ui->caLabel_74, ui->groupBox_13, ui->label_108);
    setOpiProgressBar(ui->caLabel_73
                      , ui->caLabel_74
                      , ui->caLabel_75
                      , ui->progressBar_13
                      , m_pTimer[12]
                      , m_nFlowCounter[12]);
}

void panel_pfopiform::on_caLabel_80_valueChanged(double value)
{
    setOpiBar(ui->caLabel_79, ui->caLabel_80, ui->groupBox_14, ui->label_109);
    setOpiProgressBar(ui->caLabel_79
                      , ui->caLabel_80
                      , ui->caLabel_81
                      , ui->progressBar_14
                      , m_pTimer[13]
                      , m_nFlowCounter[13]);
}

void panel_pfopiform::on_caLabel_86_valueChanged(double value)
{
    setOpiBar(ui->caLabel_85, ui->caLabel_86, ui->groupBox_15, ui->label_110);
    setOpiProgressBar(ui->caLabel_85
                      , ui->caLabel_86
                      , ui->caLabel_87
                      , ui->progressBar_15
                      , m_pTimer[14]
                      , m_nFlowCounter[14]);
}

void panel_pfopiform::on_caLabel_92_valueChanged(double value)
{
    setOpiBar(ui->caLabel_91, ui->caLabel_92, ui->groupBox_16, ui->label_111);
    setOpiProgressBar(ui->caLabel_91
                      , ui->caLabel_92
                      , ui->caLabel_93
                      , ui->progressBar_16
                      , m_pTimer[15]
                      , m_nFlowCounter[15]);
}

void panel_pfopiform::on_caLabel_98_valueChanged(double value)
{
    setOpiBar(ui->caLabel_97, ui->caLabel_98, ui->groupBox_17, ui->label_112);
    setOpiProgressBar(ui->caLabel_97
                      , ui->caLabel_98
                      , ui->caLabel_99
                      , ui->progressBar_17
                      , m_pTimer[16]
                      , m_nFlowCounter[16]);
}

void panel_pfopiform::on_caLabel_104_valueChanged(double value)
{
    setOpiBar(ui->caLabel_103, ui->caLabel_104, ui->groupBox_18, ui->label_113);
    setOpiProgressBar(ui->caLabel_103
                      , ui->caLabel_104
                      , ui->caLabel_105
                      , ui->progressBar_18
                      , m_pTimer[17]
                      , m_nFlowCounter[17]);
}

void panel_pfopiform::on_pushButton_1_clicked()
{
    if(m_pPFC01UCHV09GrpForm == NULL){
        m_pPFC01UCHV09GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01UCHV09.ui",1);
        if(m_pPFC01UCHV09GrpForm)   m_pPFC01UCHV09GrpWindow->setCentralWidget(m_pPFC01UCHV09GrpForm->GetWidget());
    }

    m_pPFC01UCHV09GrpWindow->show();
}

void panel_pfopiform::on_pushButton_2_clicked()
{
    if(m_pPFC01LCHV10GrpForm == NULL){
        m_pPFC01LCHV10GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01LCHV10.ui",1);
        if(m_pPFC01LCHV10GrpForm)   m_pPFC01LCHV10GrpWindow->setCentralWidget(m_pPFC01LCHV10GrpForm->GetWidget());
    }

    m_pPFC01LCHV10GrpWindow->show();
}

void panel_pfopiform::on_pushButton_3_clicked()
{
    if(m_pPFC0100CHVN01GrpForm == NULL){
        m_pPFC0100CHVN01GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/PFC0100CHVN01.ui",1);
        if(m_pPFC0100CHVN01GrpForm)   m_pPFC0100CHVN01GrpWindow->setCentralWidget(m_pPFC0100CHVN01GrpForm->GetWidget());
    }

    m_pPFC0100CHVN01GrpWindow->show();
}

void panel_pfopiform::on_pushButton_4_clicked()
{
    if(m_pPFC02UCHV14GrpForm == NULL){
        m_pPFC02UCHV14GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02UCHV14.ui",1);
        if(m_pPFC02UCHV14GrpForm)   m_pPFC02UCHV14GrpWindow->setCentralWidget(m_pPFC02UCHV14GrpForm->GetWidget());
    }

    m_pPFC02UCHV14GrpWindow->show();
}

void panel_pfopiform::on_pushButton_5_clicked()
{
    if(m_pPFC02LCHV15GrpForm == NULL){
        m_pPFC02LCHV15GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02LCHV15.ui",1);
        if(m_pPFC02LCHV15GrpForm)   m_pPFC02LCHV15GrpWindow->setCentralWidget(m_pPFC02LCHV15GrpForm->GetWidget());
    }

    m_pPFC02LCHV15GrpWindow->show();
}

void panel_pfopiform::on_pushButton_6_clicked()
{
    if(m_pPFC0200CHVN03GrpForm == NULL){
        m_pPFC0200CHVN03GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/PFC0200CHVN03.ui",1);
        if(m_pPFC0200CHVN03GrpForm)   m_pPFC0200CHVN03GrpWindow->setCentralWidget(m_pPFC0200CHVN03GrpForm->GetWidget());
    }

    m_pPFC0200CHVN03GrpWindow->show();
}

void panel_pfopiform::on_pushButton_7_clicked()
{
    if(m_pPFC03UCHV19GrpForm == NULL){
        m_pPFC03UCHV19GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03UCHV19.ui",1);
        if(m_pPFC03UCHV19GrpForm)   m_pPFC03UCHV19GrpWindow->setCentralWidget(m_pPFC03UCHV19GrpForm->GetWidget());
    }

    m_pPFC03UCHV19GrpWindow->show();
}

void panel_pfopiform::on_pushButton_8_clicked()
{
    if(m_pPFC03LCHV20GrpForm == NULL){
        m_pPFC03LCHV20GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03LCHV20.ui",1);
        if(m_pPFC03LCHV20GrpForm)   m_pPFC03LCHV20GrpWindow->setCentralWidget(m_pPFC03LCHV20GrpForm->GetWidget());
    }

    m_pPFC03LCHV20GrpWindow->show();
}

void panel_pfopiform::on_pushButton_9_clicked()
{
    if(m_pPFC03LCHV20GrpForm == NULL){
        m_pPFC03LCHV20GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03LCHV20.ui",1);
        if(m_pPFC03LCHV20GrpForm)   m_pPFC03LCHV20GrpWindow->setCentralWidget(m_pPFC03LCHV20GrpForm->GetWidget());
    }

    m_pPFC03LCHV20GrpWindow->show();
}

void panel_pfopiform::on_pushButton_10_clicked()
{
    if(m_pPFC04UCHV25GrpForm == NULL){
        m_pPFC04UCHV25GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04UCHV25.ui",1);
        if(m_pPFC04UCHV25GrpForm)   m_pPFC04UCHV25GrpWindow->setCentralWidget(m_pPFC04UCHV25GrpForm->GetWidget());
    }

    m_pPFC04UCHV25GrpWindow->show();
}

void panel_pfopiform::on_pushButton_11_clicked()
{
    if(m_pPFC04LCHV26GrpForm == NULL){
        m_pPFC04LCHV26GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04LCHV26.ui",1);
        if(m_pPFC04LCHV26GrpForm)   m_pPFC04LCHV26GrpWindow->setCentralWidget(m_pPFC04LCHV26GrpForm->GetWidget());
    }

    m_pPFC04LCHV26GrpWindow->show();
}

void panel_pfopiform::on_pushButton_12_clicked()
{
    if(m_pPFC04LCHV26GrpForm == NULL){
        m_pPFC04LCHV26GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04LCHV26.ui",1);
        if(m_pPFC04LCHV26GrpForm)   m_pPFC04LCHV26GrpWindow->setCentralWidget(m_pPFC04LCHV26GrpForm->GetWidget());
    }

    m_pPFC04LCHV26GrpWindow->show();
}

void panel_pfopiform::on_pushButton_13_clicked()
{
    if(m_pPFC05UCHV31GrpForm == NULL){
        m_pPFC05UCHV31GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05UCHV31.ui",1);
        if(m_pPFC05UCHV31GrpForm)   m_pPFC05UCHV31GrpWindow->setCentralWidget(m_pPFC05UCHV31GrpForm->GetWidget());
    }

    m_pPFC05UCHV31GrpWindow->show();
}

void panel_pfopiform::on_pushButton_14_clicked()
{
    if(m_pPFC05LCHV32GrpForm == NULL){
        m_pPFC05LCHV32GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05LCHV32.ui",1);
        if(m_pPFC05LCHV32GrpForm)   m_pPFC05LCHV32GrpWindow->setCentralWidget(m_pPFC05LCHV32GrpForm->GetWidget());
    }

    m_pPFC05LCHV32GrpWindow->show();
}

void panel_pfopiform::on_pushButton_15_clicked()
{
    if(m_pPFC05LCHV32GrpForm == NULL){
        m_pPFC05LCHV32GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05LCHV32.ui",1);
        if(m_pPFC05LCHV32GrpForm)   m_pPFC05LCHV32GrpWindow->setCentralWidget(m_pPFC05LCHV32GrpForm->GetWidget());
    }

    m_pPFC05LCHV32GrpWindow->show();
}

void panel_pfopiform::on_pushButton_16_clicked()
{
    if(m_pPFC06U0BHVN11GrpForm == NULL){
        m_pPFC06U0BHVN11GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/PFC06U0BHVN11.ui",1);
        if(m_pPFC06U0BHVN11GrpForm)   m_pPFC06U0BHVN11GrpWindow->setCentralWidget(m_pPFC06U0BHVN11GrpForm->GetWidget());
    }

    m_pPFC06U0BHVN11GrpWindow->show();
}

void panel_pfopiform::on_pushButton_17_clicked()
{
    if(m_pPFC06L0BHVN12GrpForm == NULL){
        m_pPFC06L0BHVN12GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/PFC06L0BHVN12.ui",1);
        if(m_pPFC06L0BHVN12GrpForm)   m_pPFC06L0BHVN12GrpWindow->setCentralWidget(m_pPFC06L0BHVN12GrpForm->GetWidget());
    }

    m_pPFC06L0BHVN12GrpWindow->show();
}

void panel_pfopiform::on_pushButton_18_clicked()
{
    if(m_pPFC0700BHVN15GrpForm == NULL){
        m_pPFC0700BHVN15GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfnew/PFC0700BHVN15.ui",1);
        if(m_pPFC0700BHVN15GrpForm)   m_pPFC0700BHVN15GrpWindow->setCentralWidget(m_pPFC0700BHVN15GrpForm->GetWidget());
    }

    m_pPFC0700BHVN15GrpWindow->show();
}
//Tab 01

//Tab 02
void panel_pfopiform::OnUpdateTimer19()
{
    ui->progressBar_19->setValue(++m_nFlowCounter[18]*100);
}
void panel_pfopiform::OnUpdateTimer20()
{
    ui->progressBar_20->setValue(++m_nFlowCounter[19]*100);
}
void panel_pfopiform::OnUpdateTimer21()
{
    ui->progressBar_21->setValue(++m_nFlowCounter[20]*100);
}
void panel_pfopiform::OnUpdateTimer22()
{
    ui->progressBar_22->setValue(++m_nFlowCounter[21]*100);
}
void panel_pfopiform::OnUpdateTimer23()
{
    ui->progressBar_23->setValue(++m_nFlowCounter[22]*100);
}
void panel_pfopiform::OnUpdateTimer24()
{
    ui->progressBar_24->setValue(++m_nFlowCounter[23]*100);
}
void panel_pfopiform::OnUpdateTimer25()
{
    ui->progressBar_25->setValue(++m_nFlowCounter[24]*100);
}
void panel_pfopiform::OnUpdateTimer26()
{
    ui->progressBar_26->setValue(++m_nFlowCounter[25]*100);
}
void panel_pfopiform::OnUpdateTimer27()
{
    ui->progressBar_27->setValue(++m_nFlowCounter[26]*100);
}
void panel_pfopiform::OnUpdateTimer28()
{
    ui->progressBar_28->setValue(++m_nFlowCounter[27]*100);
}
void panel_pfopiform::OnUpdateTimer29()
{
    ui->progressBar_29->setValue(++m_nFlowCounter[28]*100);
}
void panel_pfopiform::OnUpdateTimer30()
{
    ui->progressBar_30->setValue(++m_nFlowCounter[29]*100);
}
void panel_pfopiform::OnUpdateTimer31()
{
    ui->progressBar_31->setValue(++m_nFlowCounter[30]*100);
}
void panel_pfopiform::OnUpdateTimer32()
{
    ui->progressBar_32->setValue(++m_nFlowCounter[31]*100);
}
void panel_pfopiform::OnUpdateTimer33()
{
    ui->progressBar_33->setValue(++m_nFlowCounter[32]*100);
}
void panel_pfopiform::OnUpdateTimer34()
{
    ui->progressBar_34->setValue(++m_nFlowCounter[33]*100);
}


void panel_pfopiform::on_caLabel_110_valueChanged(double value)
{
    setOpiBar(ui->caLabel_109, ui->caLabel_110, ui->groupBox_19, ui->label_114);
    setOpiProgressBar(ui->caLabel_109
                      , ui->caLabel_110
                      , ui->caLabel_111
                      , ui->progressBar_19
                      , m_pTimer[18]
                      , m_nFlowCounter[18]);
}

void panel_pfopiform::on_caLabel_116_valueChanged(double value)
{
    setOpiBar(ui->caLabel_115, ui->caLabel_116, ui->groupBox_20, ui->label_115);
    setOpiProgressBar(ui->caLabel_115
                      , ui->caLabel_116
                      , ui->caLabel_117
                      , ui->progressBar_20
                      , m_pTimer[19]
                      , m_nFlowCounter[19]);
}

void panel_pfopiform::on_caLabel_122_valueChanged(double value)
{
    setOpiBar(ui->caLabel_121, ui->caLabel_122, ui->groupBox_21, ui->label_116);
    setOpiProgressBar(ui->caLabel_121
                      , ui->caLabel_122
                      , ui->caLabel_123
                      , ui->progressBar_21
                      , m_pTimer[20]
                      , m_nFlowCounter[20]);
}

void panel_pfopiform::on_caLabel_128_valueChanged(double value)
{
    setOpiBar(ui->caLabel_127, ui->caLabel_128, ui->groupBox_22, ui->label_117);
    setOpiProgressBar(ui->caLabel_127
                      , ui->caLabel_128
                      , ui->caLabel_129
                      , ui->progressBar_22
                      , m_pTimer[21]
                      , m_nFlowCounter[21]);
}

void panel_pfopiform::on_caLabel_134_valueChanged(double value)
{
    setOpiBar(ui->caLabel_133, ui->caLabel_134, ui->groupBox_23, ui->label_118);
    setOpiProgressBar(ui->caLabel_133
                      , ui->caLabel_134
                      , ui->caLabel_135
                      , ui->progressBar_23
                      , m_pTimer[22]
                      , m_nFlowCounter[22]);
}

void panel_pfopiform::on_caLabel_140_valueChanged(double value)
{
    setOpiBar(ui->caLabel_139, ui->caLabel_140, ui->groupBox_24, ui->label_119);
    setOpiProgressBar(ui->caLabel_139
                      , ui->caLabel_140
                      , ui->caLabel_141
                      , ui->progressBar_24
                      , m_pTimer[23]
                      , m_nFlowCounter[23]);
}

void panel_pfopiform::on_caLabel_146_valueChanged(double value)
{
    setOpiBar(ui->caLabel_145, ui->caLabel_146, ui->groupBox_25, ui->label_120);
    setOpiProgressBar(ui->caLabel_145
                      , ui->caLabel_146
                      , ui->caLabel_147
                      , ui->progressBar_25
                      , m_pTimer[24]
                      , m_nFlowCounter[24]);
}

void panel_pfopiform::on_caLabel_152_valueChanged(double value)
{
    setOpiBar(ui->caLabel_151, ui->caLabel_152, ui->groupBox_26, ui->label_121);
    setOpiProgressBar(ui->caLabel_151
                      , ui->caLabel_152
                      , ui->caLabel_153
                      , ui->progressBar_26
                      , m_pTimer[25]
                      , m_nFlowCounter[25]);
}

void panel_pfopiform::on_caLabel_158_valueChanged(double value)
{
    setOpiBar(ui->caLabel_157, ui->caLabel_158, ui->groupBox_27, ui->label_122);
    setOpiProgressBar(ui->caLabel_157
                      , ui->caLabel_158
                      , ui->caLabel_159
                      , ui->progressBar_27
                      , m_pTimer[26]
                      , m_nFlowCounter[26]);
}

void panel_pfopiform::on_caLabel_164_valueChanged(double value)
{
    setOpiBar(ui->caLabel_163, ui->caLabel_164, ui->groupBox_28, ui->label_123);
    setOpiProgressBar(ui->caLabel_163
                      , ui->caLabel_164
                      , ui->caLabel_165
                      , ui->progressBar_28
                      , m_pTimer[27]
                      , m_nFlowCounter[27]);
}

void panel_pfopiform::on_caLabel_170_valueChanged(double value)
{
    setOpiBar(ui->caLabel_169, ui->caLabel_170, ui->groupBox_29, ui->label_124);
    setOpiProgressBar(ui->caLabel_169
                      , ui->caLabel_170
                      , ui->caLabel_171
                      , ui->progressBar_29
                      , m_pTimer[28]
                      , m_nFlowCounter[28]);
}

void panel_pfopiform::on_caLabel_176_valueChanged(double value)
{
    setOpiBar(ui->caLabel_175, ui->caLabel_176, ui->groupBox_30, ui->label_125);
    setOpiProgressBar(ui->caLabel_175
                      , ui->caLabel_176
                      , ui->caLabel_177
                      , ui->progressBar_30
                      , m_pTimer[29]
                      , m_nFlowCounter[29]);
}

void panel_pfopiform::on_caLabel_182_valueChanged(double value)
{
    setOpiBar(ui->caLabel_181, ui->caLabel_182, ui->groupBox_31, ui->label_126);
    setOpiProgressBar(ui->caLabel_181
                      , ui->caLabel_182
                      , ui->caLabel_183
                      , ui->progressBar_31
                      , m_pTimer[30]
                      , m_nFlowCounter[30]);
}

void panel_pfopiform::on_caLabel_188_valueChanged(double value)
{
    setOpiBar(ui->caLabel_187, ui->caLabel_188, ui->groupBox_32, ui->label_127);
    setOpiProgressBar(ui->caLabel_187
                      , ui->caLabel_188
                      , ui->caLabel_189
                      , ui->progressBar_32
                      , m_pTimer[31]
                      , m_nFlowCounter[31]);
}

void panel_pfopiform::on_caLabel_194_valueChanged(double value)
{
    setOpiBar(ui->caLabel_193, ui->caLabel_194, ui->groupBox_33, ui->label_128);
    setOpiProgressBar(ui->caLabel_193
                      , ui->caLabel_194
                      , ui->caLabel_195
                      , ui->progressBar_33
                      , m_pTimer[32]
                      , m_nFlowCounter[32]);
}

void panel_pfopiform::on_caLabel_200_valueChanged(double value)
{
    setOpiBar(ui->caLabel_199, ui->caLabel_200, ui->groupBox_34, ui->label_129);
    setOpiProgressBar(ui->caLabel_199
                      , ui->caLabel_200
                      , ui->caLabel_201
                      , ui->progressBar_34
                      , m_pTimer[33]
                      , m_nFlowCounter[33]);
}

void panel_pfopiform::on_pushButton_19_clicked()
{
    if(m_pPFC01UCHV09GrpForm == NULL){
        m_pPFC01UCHV09GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01UCHV09.ui",1);
        if(m_pPFC01UCHV09GrpForm)   m_pPFC01UCHV09GrpWindow->setCentralWidget(m_pPFC01UCHV09GrpForm->GetWidget());
    }

    m_pPFC01UCHV09GrpWindow->show();
}

void panel_pfopiform::on_pushButton_20_clicked()
{
    if(m_pPFC03UCHV19GrpForm == NULL){
        m_pPFC03UCHV19GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03UCHV19.ui",1);
        if(m_pPFC03UCHV19GrpForm)   m_pPFC03UCHV19GrpWindow->setCentralWidget(m_pPFC03UCHV19GrpForm->GetWidget());
    }

    m_pPFC03UCHV19GrpWindow->show();
}

void panel_pfopiform::on_pushButton_21_clicked()
{
    if(m_pPFC01UBHV21GrpForm == NULL){
        m_pPFC01UBHV21GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01UBHV21.ui",1);
        if(m_pPFC01UBHV21GrpForm)   m_pPFC01UBHV21GrpWindow->setCentralWidget(m_pPFC01UBHV21GrpForm->GetWidget());
    }

    m_pPFC01UBHV21GrpWindow->show();
}

void panel_pfopiform::on_pushButton_22_clicked()
{
    if(m_pPFC03UBHV25GrpForm == NULL){
        m_pPFC03UBHV25GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03UBHV25.ui",1);
        if(m_pPFC03UBHV25GrpForm)   m_pPFC03UBHV25GrpWindow->setCentralWidget(m_pPFC03UBHV25GrpForm->GetWidget());
    }

    m_pPFC03UBHV25GrpWindow->show();
}

void panel_pfopiform::on_pushButton_23_clicked()
{
    if(m_pPFC01LCHV10GrpForm == NULL){
        m_pPFC01LCHV10GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01LCHV10.ui",1);
        if(m_pPFC01LCHV10GrpForm)   m_pPFC01LCHV10GrpWindow->setCentralWidget(m_pPFC01LCHV10GrpForm->GetWidget());
    }

    m_pPFC01LCHV10GrpWindow->show();
}

void panel_pfopiform::on_pushButton_24_clicked()
{
    if(m_pPFC03LCHV20GrpForm == NULL){
        m_pPFC03LCHV20GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03LCHV20.ui",1);
        if(m_pPFC03LCHV20GrpForm)   m_pPFC03LCHV20GrpWindow->setCentralWidget(m_pPFC03LCHV20GrpForm->GetWidget());
    }

    m_pPFC03LCHV20GrpWindow->show();
}

void panel_pfopiform::on_pushButton_25_clicked()
{
    if(m_pPFC01LBHV22GrpForm == NULL){
        m_pPFC01LBHV22GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC01LBHV22.ui",1);
        if(m_pPFC01LBHV22GrpForm)   m_pPFC01LBHV22GrpWindow->setCentralWidget(m_pPFC01LBHV22GrpForm->GetWidget());
    }

    m_pPFC01LBHV22GrpWindow->show();
}

void panel_pfopiform::on_pushButton_26_clicked()
{
    if(m_pPFC03LBHV26GrpForm == NULL){
        m_pPFC03LBHV26GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC03LBHV26.ui",1);
        if(m_pPFC03LBHV26GrpForm)   m_pPFC03LBHV26GrpWindow->setCentralWidget(m_pPFC03LBHV26GrpForm->GetWidget());
    }

    m_pPFC03LBHV26GrpWindow->show();
}

void panel_pfopiform::on_pushButton_27_clicked()
{
    if(m_pPFC02UCHV14GrpForm == NULL){
        m_pPFC02UCHV14GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02UCHV14.ui",1);
        if(m_pPFC02UCHV14GrpForm)   m_pPFC02UCHV14GrpWindow->setCentralWidget(m_pPFC02UCHV14GrpForm->GetWidget());
    }

    m_pPFC02UCHV14GrpWindow->show();
}

void panel_pfopiform::on_pushButton_28_clicked()
{
    if(m_pPFC04UCHV25GrpForm == NULL){
        m_pPFC04UCHV25GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04UCHV25.ui",1);
        if(m_pPFC04UCHV25GrpForm)   m_pPFC04UCHV25GrpWindow->setCentralWidget(m_pPFC04UCHV25GrpForm->GetWidget());
    }

    m_pPFC04UCHV25GrpWindow->show();
}

void panel_pfopiform::on_pushButton_29_clicked()
{
    if(m_pPFC02UBHV23GrpForm == NULL){
        m_pPFC02UBHV23GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02UBHV23.ui",1);
        if(m_pPFC02UBHV23GrpForm)   m_pPFC02UBHV23GrpWindow->setCentralWidget(m_pPFC02UBHV23GrpForm->GetWidget());
    }

    m_pPFC02UBHV23GrpWindow->show();
}

void panel_pfopiform::on_pushButton_30_clicked()
{
    if(m_pPFC04UBHV27GrpForm == NULL){
        m_pPFC04UBHV27GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04UBHV27.ui",1);
        if(m_pPFC04UBHV27GrpForm)   m_pPFC04UBHV27GrpWindow->setCentralWidget(m_pPFC04UBHV27GrpForm->GetWidget());
    }

    m_pPFC04UBHV27GrpWindow->show();
}

void panel_pfopiform::on_pushButton_31_clicked()
{
    if(m_pPFC02LCHV15GrpForm == NULL){
        m_pPFC02LCHV15GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02LCHV15.ui",1);
        if(m_pPFC02LCHV15GrpForm)   m_pPFC02LCHV15GrpWindow->setCentralWidget(m_pPFC02LCHV15GrpForm->GetWidget());
    }

    m_pPFC02LCHV15GrpWindow->show();
}

void panel_pfopiform::on_pushButton_32_clicked()
{
    if(m_pPFC04LCHV26GrpForm == NULL){
        m_pPFC04LCHV26GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04LCHV26.ui",1);
        if(m_pPFC04LCHV26GrpForm)   m_pPFC04LCHV26GrpWindow->setCentralWidget(m_pPFC04LCHV26GrpForm->GetWidget());
    }

    m_pPFC04LCHV26GrpWindow->show();
}

void panel_pfopiform::on_pushButton_33_clicked()
{
    if(m_pPFC02LBHV24GrpForm == NULL){
        m_pPFC02LBHV24GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC02LBHV24.ui",1);
        if(m_pPFC02LBHV24GrpForm)   m_pPFC02LBHV24GrpWindow->setCentralWidget(m_pPFC02LBHV24GrpForm->GetWidget());
    }

    m_pPFC02LBHV24GrpWindow->show();
}

void panel_pfopiform::on_pushButton_34_clicked()
{
    if(m_pPFC04LBHV28GrpForm == NULL){
        m_pPFC04LBHV28GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC04LBHV28.ui",1);
        if(m_pPFC04LBHV28GrpForm)   m_pPFC04LBHV28GrpWindow->setCentralWidget(m_pPFC04LBHV28GrpForm->GetWidget());
    }

    m_pPFC04LBHV28GrpWindow->show();
}

//Tab 02

//Tab 03

void panel_pfopiform::OnUpdateTimer35()
{
    ui->progressBar_35->setValue(++m_nFlowCounter[34]*100);
}
void panel_pfopiform::OnUpdateTimer36()
{
    ui->progressBar_36->setValue(++m_nFlowCounter[35]*100);
}
void panel_pfopiform::OnUpdateTimer37()
{
    ui->progressBar_37->setValue(++m_nFlowCounter[36]*100);
}
void panel_pfopiform::OnUpdateTimer38()
{
    ui->progressBar_38->setValue(++m_nFlowCounter[37]*100);
}
void panel_pfopiform::OnUpdateTimer39()
{
    ui->progressBar_39->setValue(++m_nFlowCounter[38]*100);
}
void panel_pfopiform::OnUpdateTimer40()
{
    ui->progressBar_40->setValue(++m_nFlowCounter[39]*100);
}
void panel_pfopiform::OnUpdateTimer41()
{
    ui->progressBar_41->setValue(++m_nFlowCounter[40]*100);
}
void panel_pfopiform::OnUpdateTimer42()
{
    ui->progressBar_42->setValue(++m_nFlowCounter[41]*100);
}
void panel_pfopiform::OnUpdateTimer43()
{
    ui->progressBar_43->setValue(++m_nFlowCounter[42]*100);
}
void panel_pfopiform::OnUpdateTimer44()
{
    ui->progressBar_44->setValue(++m_nFlowCounter[43]*100);
}
void panel_pfopiform::OnUpdateTimer45()
{
    ui->progressBar_45->setValue(++m_nFlowCounter[44]*100);
}
void panel_pfopiform::OnUpdateTimer46()
{
    ui->progressBar_46->setValue(++m_nFlowCounter[45]*100);
}
void panel_pfopiform::OnUpdateTimer47()
{
    ui->progressBar_47->setValue(++m_nFlowCounter[46]*100);
}
void panel_pfopiform::OnUpdateTimer48()
{
    ui->progressBar_48->setValue(++m_nFlowCounter[47]*100);
}
void panel_pfopiform::on_caLabel_206_valueChanged(double value)
{
    setOpiBar(ui->caLabel_205, ui->caLabel_206, ui->groupBox_35, ui->label_130);
    setOpiProgressBar(ui->caLabel_205
                      , ui->caLabel_206
                      , ui->caLabel_207
                      , ui->progressBar_35
                      , m_pTimer[34]
                      , m_nFlowCounter[34]);
}

void panel_pfopiform::on_caLabel_212_valueChanged(double value)
{
    setOpiBar(ui->caLabel_211, ui->caLabel_212, ui->groupBox_36, ui->label_131);
    setOpiProgressBar(ui->caLabel_211
                      , ui->caLabel_212
                      , ui->caLabel_213
                      , ui->progressBar_36
                      , m_pTimer[35]
                      , m_nFlowCounter[35]);
}

void panel_pfopiform::on_caLabel_218_valueChanged(double value)
{
    setOpiBar(ui->caLabel_217, ui->caLabel_218, ui->groupBox_37, ui->label_132);
    setOpiProgressBar(ui->caLabel_217
                      , ui->caLabel_218
                      , ui->caLabel_219
                      , ui->progressBar_37
                      , m_pTimer[36]
                      , m_nFlowCounter[36]);
}

void panel_pfopiform::on_caLabel_224_valueChanged(double value)
{
    setOpiBar(ui->caLabel_223, ui->caLabel_224, ui->groupBox_38, ui->label_133);
    setOpiProgressBar(ui->caLabel_223
                      , ui->caLabel_224
                      , ui->caLabel_225
                      , ui->progressBar_38
                      , m_pTimer[37]
                      , m_nFlowCounter[37]);
}

void panel_pfopiform::on_caLabel_230_valueChanged(double value)
{
    setOpiBar(ui->caLabel_229, ui->caLabel_230, ui->groupBox_39, ui->label_134);
    setOpiProgressBar(ui->caLabel_229
                      , ui->caLabel_230
                      , ui->caLabel_231
                      , ui->progressBar_39
                      , m_pTimer[38]
                      , m_nFlowCounter[38]);
}

void panel_pfopiform::on_caLabel_236_valueChanged(double value)
{
    setOpiBar(ui->caLabel_235, ui->caLabel_236, ui->groupBox_40, ui->label_135);
    setOpiProgressBar(ui->caLabel_235
                      , ui->caLabel_236
                      , ui->caLabel_237
                      , ui->progressBar_40
                      , m_pTimer[39]
                      , m_nFlowCounter[39]);
}

void panel_pfopiform::on_caLabel_242_valueChanged(double value)
{
    setOpiBar(ui->caLabel_241, ui->caLabel_242, ui->groupBox_41, ui->label_136);
    setOpiProgressBar(ui->caLabel_241
                      , ui->caLabel_242
                      , ui->caLabel_243
                      , ui->progressBar_41
                      , m_pTimer[40]
                      , m_nFlowCounter[40]);
}

void panel_pfopiform::on_caLabel_248_valueChanged(double value)
{
    setOpiBar(ui->caLabel_247, ui->caLabel_248, ui->groupBox_42, ui->label_137);
    setOpiProgressBar(ui->caLabel_247
                      , ui->caLabel_248
                      , ui->caLabel_249
                      , ui->progressBar_42
                      , m_pTimer[41]
                      , m_nFlowCounter[41]);
}

void panel_pfopiform::on_caLabel_254_valueChanged(double value)
{
    setOpiBar(ui->caLabel_253, ui->caLabel_254, ui->groupBox_43, ui->label_138);
    setOpiProgressBar(ui->caLabel_253
                      , ui->caLabel_254
                      , ui->caLabel_255
                      , ui->progressBar_43
                      , m_pTimer[42]
                      , m_nFlowCounter[42]);
}

void panel_pfopiform::on_caLabel_260_valueChanged(double value)
{
    setOpiBar(ui->caLabel_259, ui->caLabel_260, ui->groupBox_44, ui->label_139);
    setOpiProgressBar(ui->caLabel_259
                      , ui->caLabel_260
                      , ui->caLabel_261
                      , ui->progressBar_44
                      , m_pTimer[43]
                      , m_nFlowCounter[43]);
}

void panel_pfopiform::on_caLabel_266_valueChanged(double value)
{
    setOpiBar(ui->caLabel_265, ui->caLabel_266, ui->groupBox_45, ui->label_140);
    setOpiProgressBar(ui->caLabel_265
                      , ui->caLabel_266
                      , ui->caLabel_267
                      , ui->progressBar_45
                      , m_pTimer[44]
                      , m_nFlowCounter[44]);
}

void panel_pfopiform::on_caLabel_272_valueChanged(double value)
{
    setOpiBar(ui->caLabel_271, ui->caLabel_272, ui->groupBox_46, ui->label_141);
    setOpiProgressBar(ui->caLabel_271
                      , ui->caLabel_272
                      , ui->caLabel_273
                      , ui->progressBar_46
                      , m_pTimer[45]
                      , m_nFlowCounter[45]);
}

void panel_pfopiform::on_caLabel_302_valueChanged(double value)
{
    setOpiBar(ui->caLabel_301, ui->caLabel_302, ui->groupBox_47, ui->label_142);
    setOpiProgressBar(ui->caLabel_301
                      , ui->caLabel_302
                      , ui->caLabel_303
                      , ui->progressBar_47
                      , m_pTimer[46]
                      , m_nFlowCounter[46]);
}

void panel_pfopiform::on_caLabel_308_valueChanged(double value)
{
    setOpiBar(ui->caLabel_307, ui->caLabel_308, ui->groupBox_48, ui->label_143);
    setOpiProgressBar(ui->caLabel_307
                      , ui->caLabel_308
                      , ui->caLabel_309
                      , ui->progressBar_48
                      , m_pTimer[47]
                      , m_nFlowCounter[47]);
}

void panel_pfopiform::on_pushButton_35_clicked()
{
    if(m_pPFC05UCHV31GrpForm == NULL){
        m_pPFC05UCHV31GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05UCHV31.ui",1);
        if(m_pPFC05UCHV31GrpForm)   m_pPFC05UCHV31GrpWindow->setCentralWidget(m_pPFC05UCHV31GrpForm->GetWidget());
    }

    m_pPFC05UCHV31GrpWindow->show();
}

void panel_pfopiform::on_pushButton_36_clicked()
{
    if(m_pPFC06LUBHV34GrpForm == NULL){
        m_pPFC06LUBHV34GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06LUBHV34.ui",1);
        if(m_pPFC06LUBHV34GrpForm)  m_pPFC06LUBHV34GrpWindow->setCentralWidget(m_pPFC06LUBHV34GrpForm->GetWidget());
    }

    m_pPFC06LUBHV34GrpWindow->show();
}

void panel_pfopiform::on_pushButton_37_clicked()
{
    if(m_pPFC05UBHV29GrpForm == NULL){
        m_pPFC05UBHV29GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05UBHV29.ui",1);
        if(m_pPFC05UBHV29GrpForm)   m_pPFC05UBHV29GrpWindow->setCentralWidget(m_pPFC05UBHV29GrpForm->GetWidget());
    }

    m_pPFC05UBHV29GrpWindow->show();
}

void panel_pfopiform::on_pushButton_38_clicked()
{
    if(m_pPFC06LLBHV35GrpForm == NULL){
        m_pPFC06LLBHV35GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06LLBHV35.ui",1);
        if(m_pPFC06LLBHV35GrpForm)  m_pPFC06LLBHV35GrpWindow->setCentralWidget(m_pPFC06LLBHV35GrpForm->GetWidget());
    }

    m_pPFC06LLBHV35GrpWindow->show();
}

void panel_pfopiform::on_pushButton_39_clicked()
{
    if(m_pPFC05LCHV32GrpForm == NULL){
        m_pPFC05LCHV32GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05LCHV32.ui",1);
        if(m_pPFC05LCHV32GrpForm)   m_pPFC05LCHV32GrpWindow->setCentralWidget(m_pPFC05LCHV32GrpForm->GetWidget());
    }

    m_pPFC05LCHV32GrpWindow->show();
}

void panel_pfopiform::on_pushButton_40_clicked()
{
    if(m_pPFC06LTBHV36GrpForm == NULL){
        m_pPFC06LTBHV36GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06LTBHV36.ui",1);
        if(m_pPFC06LTBHV36GrpForm)  m_pPFC06LTBHV36GrpWindow->setCentralWidget(m_pPFC06LTBHV36GrpForm->GetWidget());
    }

    m_pPFC06LTBHV36GrpWindow->show();
}

void panel_pfopiform::on_pushButton_41_clicked()
{
    if(m_pPFC05LBHV30GrpForm == NULL){
        m_pPFC05LBHV30GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC05LBHV30.ui",1);
        if(m_pPFC05LBHV30GrpForm)   m_pPFC05LBHV30GrpWindow->setCentralWidget(m_pPFC05LBHV30GrpForm->GetWidget());
    }

    m_pPFC05LBHV30GrpWindow->show();
}

void panel_pfopiform::on_pushButton_42_clicked()
{
    if(m_pPFC06TBHV37GrpForm == NULL){
        m_pPFC06TBHV37GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06TBHV37.ui",1);
        if(m_pPFC06TBHV37GrpForm)   m_pPFC06TBHV37GrpWindow->setCentralWidget(m_pPFC06TBHV37GrpForm->GetWidget());
    }

    m_pPFC06TBHV37GrpWindow->show();
}

void panel_pfopiform::on_pushButton_43_clicked()
{
    if(m_pPFC06UUBHV31GrpForm == NULL){
        m_pPFC06UUBHV31GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06UUBHV31.ui",1);
        if(m_pPFC06UUBHV31GrpForm)  m_pPFC06UUBHV31GrpWindow->setCentralWidget(m_pPFC06UUBHV31GrpForm->GetWidget());
    }

    m_pPFC06UUBHV31GrpWindow->show();
}

void panel_pfopiform::on_pushButton_44_clicked()
{
    if(m_pPFC07UBHV38GrpForm == NULL){
        m_pPFC07UBHV38GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC07UBHV38.ui",1);
        if(m_pPFC07UBHV38GrpForm)   m_pPFC07UBHV38GrpWindow->setCentralWidget(m_pPFC07UBHV38GrpForm->GetWidget());
    }

    m_pPFC07UBHV38GrpWindow->show();
}

void panel_pfopiform::on_pushButton_45_clicked()
{
    if(m_pPFC06ULBHV32GrpForm == NULL){
        m_pPFC06ULBHV32GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06ULBHV32.ui",1);
        if(m_pPFC06ULBHV32GrpForm)  m_pPFC06ULBHV32GrpWindow->setCentralWidget(m_pPFC06ULBHV32GrpForm->GetWidget());
    }

    m_pPFC06ULBHV32GrpWindow->show();
}

void panel_pfopiform::on_pushButton_46_clicked()
{
    if(m_pPFC07LBHV39GrpForm == NULL){
        m_pPFC07LBHV39GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC07LBHV39.ui",1);
        if(m_pPFC07LBHV39GrpForm)   m_pPFC07LBHV39GrpWindow->setCentralWidget(m_pPFC07LBHV39GrpForm->GetWidget());
    }

    m_pPFC07LBHV39GrpWindow->show();
}


void panel_pfopiform::on_pushButton_47_clicked()
{
    if(m_pPFC06UTBHV33GrpForm == NULL){
        m_pPFC06UTBHV33GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC06UTBHV33.ui",1);
        if(m_pPFC06UTBHV33GrpForm)  m_pPFC06UTBHV33GrpWindow->setCentralWidget(m_pPFC06UTBHV33GrpForm->GetWidget());
    }

    m_pPFC06UTBHV33GrpWindow->show();
}

void panel_pfopiform::on_pushButton_48_clicked()
{
    if(m_pPFC07TBHV40GrpForm == NULL){
        m_pPFC07TBHV40GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfcoil/PFC07TBHV40.ui",1);
        if(m_pPFC07TBHV40GrpForm)   m_pPFC07TBHV40GrpWindow->setCentralWidget(m_pPFC07TBHV40GrpForm->GetWidget());
    }

    m_pPFC07TBHV40GrpWindow->show();
}

//Tab 03


//Tab 04

void panel_pfopiform::OnUpdateTimer49()
{
    ui->progressBar_49->setValue(++m_nFlowCounter[48]*100);
}
void panel_pfopiform::OnUpdateTimer50()
{
    ui->progressBar_50->setValue(++m_nFlowCounter[49]*100);
}
void panel_pfopiform::OnUpdateTimer51()
{
    ui->progressBar_51->setValue(++m_nFlowCounter[50]*100);
}
void panel_pfopiform::OnUpdateTimer52()
{
    ui->progressBar_52->setValue(++m_nFlowCounter[51]*100);
}
void panel_pfopiform::OnUpdateTimer53()
{
    ui->progressBar_53->setValue(++m_nFlowCounter[52]*100);
}
void panel_pfopiform::OnUpdateTimer54()
{
    ui->progressBar_54->setValue(++m_nFlowCounter[53]*100);
}
void panel_pfopiform::OnUpdateTimer55()
{
    ui->progressBar_55->setValue(++m_nFlowCounter[54]*100);
}
void panel_pfopiform::OnUpdateTimer56()
{
    ui->progressBar_56->setValue(++m_nFlowCounter[55]*100);
}
void panel_pfopiform::OnUpdateTimer57()
{
    ui->progressBar_57->setValue(++m_nFlowCounter[56]*100);
}
void panel_pfopiform::OnUpdateTimer58()
{
    ui->progressBar_58->setValue(++m_nFlowCounter[57]*100);
}
void panel_pfopiform::OnUpdateTimer59()
{
    ui->progressBar_59->setValue(++m_nFlowCounter[58]*100);
}
void panel_pfopiform::OnUpdateTimer60()
{
    ui->progressBar_60->setValue(++m_nFlowCounter[59]*100);
}
void panel_pfopiform::OnUpdateTimer61()
{
    ui->progressBar_61->setValue(++m_nFlowCounter[60]*100);
}
void panel_pfopiform::OnUpdateTimer62()
{
    ui->progressBar_62->setValue(++m_nFlowCounter[61]*100);
}
void panel_pfopiform::OnUpdateTimer63()
{
    ui->progressBar_63->setValue(++m_nFlowCounter[62]*100);
}
void panel_pfopiform::OnUpdateTimer64()
{
    ui->progressBar_64->setValue(++m_nFlowCounter[63]*100);
}
void panel_pfopiform::OnUpdateTimer65()
{
    ui->progressBar_65->setValue(++m_nFlowCounter[64]*100);
}
void panel_pfopiform::OnUpdateTimer66()
{
    ui->progressBar_66->setValue(++m_nFlowCounter[65]*100);
}
void panel_pfopiform::OnUpdateTimer67()
{
    ui->progressBar_67->setValue(++m_nFlowCounter[66]*100);
}
void panel_pfopiform::OnUpdateTimer68()
{
    ui->progressBar_68->setValue(++m_nFlowCounter[67]*100);
}
void panel_pfopiform::OnUpdateTimer69()
{
    ui->progressBar_69->setValue(++m_nFlowCounter[68]*100);
}
void panel_pfopiform::OnUpdateTimer70()
{
    ui->progressBar_70->setValue(++m_nFlowCounter[69]*100);
}
void panel_pfopiform::OnUpdateTimer71()
{
    ui->progressBar_71->setValue(++m_nFlowCounter[70]*100);
}
void panel_pfopiform::OnUpdateTimer72()
{
    ui->progressBar_72->setValue(++m_nFlowCounter[71]*100);
}
void panel_pfopiform::OnUpdateTimer73()
{
    ui->progressBar_73->setValue(++m_nFlowCounter[72]*100);
}
void panel_pfopiform::on_caLabel_314_valueChanged(double value)
{
    setOpiBar(ui->caLabel_313, ui->caLabel_314, ui->groupBox_49, ui->label_144);
    setOpiProgressBar(ui->caLabel_313
                      , ui->caLabel_314
                      , ui->caLabel_315
                      , ui->progressBar_49
                      , m_pTimer[48]
                      , m_nFlowCounter[48]);
}

void panel_pfopiform::on_caLabel_320_valueChanged(double value)
{
    setOpiBar(ui->caLabel_319, ui->caLabel_320, ui->groupBox_50, ui->label_145);
    setOpiProgressBar(ui->caLabel_319
                      , ui->caLabel_320
                      , ui->caLabel_321
                      , ui->progressBar_50
                      , m_pTimer[49]
                      , m_nFlowCounter[49]);
}

void panel_pfopiform::on_caLabel_326_valueChanged(double value)
{
    setOpiBar(ui->caLabel_325, ui->caLabel_326, ui->groupBox_51, ui->label_146);
    setOpiProgressBar(ui->caLabel_325
                      , ui->caLabel_326
                      , ui->caLabel_327
                      , ui->progressBar_51
                      , m_pTimer[50]
                      , m_nFlowCounter[50]);
}

void panel_pfopiform::on_caLabel_332_valueChanged(double value)
{
    setOpiBar(ui->caLabel_331, ui->caLabel_332, ui->groupBox_52, ui->label_147);
    setOpiProgressBar(ui->caLabel_331
                      , ui->caLabel_332
                      , ui->caLabel_333
                      , ui->progressBar_52
                      , m_pTimer[51]
                      , m_nFlowCounter[51]);
}

void panel_pfopiform::on_caLabel_338_valueChanged(double value)
{
    setOpiBar(ui->caLabel_337, ui->caLabel_338, ui->groupBox_53, ui->label_148);
    setOpiProgressBar(ui->caLabel_337
                      , ui->caLabel_338
                      , ui->caLabel_339
                      , ui->progressBar_53
                      , m_pTimer[52]
                      , m_nFlowCounter[52]);
}

void panel_pfopiform::on_caLabel_344_valueChanged(double value)
{
    setOpiBar(ui->caLabel_343, ui->caLabel_344, ui->groupBox_54, ui->label_149);
    setOpiProgressBar(ui->caLabel_343
                      , ui->caLabel_344
                      , ui->caLabel_345
                      , ui->progressBar_54
                      , m_pTimer[53]
                      , m_nFlowCounter[53]);
}

void panel_pfopiform::on_caLabel_350_valueChanged(double value)
{
    setOpiBar(ui->caLabel_349, ui->caLabel_350, ui->groupBox_55, ui->label_150);
    setOpiProgressBar(ui->caLabel_349
                      , ui->caLabel_350
                      , ui->caLabel_351
                      , ui->progressBar_55
                      , m_pTimer[54]
                      , m_nFlowCounter[54]);
}

void panel_pfopiform::on_caLabel_356_valueChanged(double value)
{
    setOpiBar(ui->caLabel_355, ui->caLabel_356, ui->groupBox_56, ui->label_151);
    setOpiProgressBar(ui->caLabel_355
                      , ui->caLabel_356
                      , ui->caLabel_357
                      , ui->progressBar_56
                      , m_pTimer[55]
                      , m_nFlowCounter[55]);
}

void panel_pfopiform::on_caLabel_362_valueChanged(double value)
{
    setOpiBar(ui->caLabel_361, ui->caLabel_362, ui->groupBox_57, ui->label_152);
    setOpiProgressBar(ui->caLabel_361
                      , ui->caLabel_362
                      , ui->caLabel_363
                      , ui->progressBar_57
                      , m_pTimer[56]
                      , m_nFlowCounter[56]);
}

void panel_pfopiform::on_caLabel_368_valueChanged(double value)
{
    setOpiBar(ui->caLabel_367, ui->caLabel_368, ui->groupBox_58, ui->label_153);
    setOpiProgressBar(ui->caLabel_367
                      , ui->caLabel_368
                      , ui->caLabel_369
                      , ui->progressBar_57
                      , m_pTimer[57]
                      , m_nFlowCounter[57]);
}

void panel_pfopiform::on_caLabel_374_valueChanged(double value)
{
    setOpiBar(ui->caLabel_373, ui->caLabel_374, ui->groupBox_59, ui->label_154);
    setOpiProgressBar(ui->caLabel_373
                      , ui->caLabel_374
                      , ui->caLabel_375
                      , ui->progressBar_59
                      , m_pTimer[58]
                      , m_nFlowCounter[58]);
}

void panel_pfopiform::on_caLabel_380_valueChanged(double value)
{
    setOpiBar(ui->caLabel_379, ui->caLabel_380, ui->groupBox_60, ui->label_155);
    setOpiProgressBar(ui->caLabel_379
                      , ui->caLabel_380
                      , ui->caLabel_381
                      , ui->progressBar_60
                      , m_pTimer[59]
                      , m_nFlowCounter[59]);
}

void panel_pfopiform::on_caLabel_386_valueChanged(double value)
{
    setOpiBar(ui->caLabel_385, ui->caLabel_386, ui->groupBox_61, ui->label_156);
    setOpiProgressBar(ui->caLabel_385
                      , ui->caLabel_386
                      , ui->caLabel_387
                      , ui->progressBar_61
                      , m_pTimer[60]
                      , m_nFlowCounter[60]);
}

void panel_pfopiform::on_caLabel_392_valueChanged(double value)
{
    setOpiBar(ui->caLabel_391, ui->caLabel_392, ui->groupBox_62, ui->label_157);
    setOpiProgressBar(ui->caLabel_391
                      , ui->caLabel_392
                      , ui->caLabel_393
                      , ui->progressBar_62
                      , m_pTimer[61]
                      , m_nFlowCounter[61]);
}

void panel_pfopiform::on_caLabel_398_valueChanged(double value)
{
    setOpiBar(ui->caLabel_397, ui->caLabel_398, ui->groupBox_63, ui->label_158);
    setOpiProgressBar(ui->caLabel_397
                      , ui->caLabel_398
                      , ui->caLabel_399
                      , ui->progressBar_63
                      , m_pTimer[62]
                      , m_nFlowCounter[62]);
}

void panel_pfopiform::on_caLabel_404_valueChanged(double value)
{
    setOpiBar(ui->caLabel_403, ui->caLabel_404, ui->groupBox_64, ui->label_159);
    setOpiProgressBar(ui->caLabel_403
                      , ui->caLabel_404
                      , ui->caLabel_405
                      , ui->progressBar_64
                      , m_pTimer[63]
                      , m_nFlowCounter[63]);
}

void panel_pfopiform::on_caLabel_410_valueChanged(double value)
{
    setOpiBar(ui->caLabel_409, ui->caLabel_410, ui->groupBox_65, ui->label_160);
    setOpiProgressBar(ui->caLabel_409
                      , ui->caLabel_410
                      , ui->caLabel_411
                      , ui->progressBar_65
                      , m_pTimer[64]
                      , m_nFlowCounter[64]);
}

void panel_pfopiform::on_caLabel_416_valueChanged(double value)
{
    setOpiBar(ui->caLabel_415, ui->caLabel_416, ui->groupBox_66, ui->label_161);
    setOpiProgressBar(ui->caLabel_415
                      , ui->caLabel_416
                      , ui->caLabel_417
                      , ui->progressBar_66
                      , m_pTimer[65]
                      , m_nFlowCounter[65]);
}

void panel_pfopiform::on_caLabel_422_valueChanged(double value)
{
    setOpiBar(ui->caLabel_421, ui->caLabel_422, ui->groupBox_67, ui->label_162);
    setOpiProgressBar(ui->caLabel_421
                      , ui->caLabel_422
                      , ui->caLabel_423
                      , ui->progressBar_67
                      , m_pTimer[66]
                      , m_nFlowCounter[66]);
}

void panel_pfopiform::on_caLabel_428_valueChanged(double value)
{
    setOpiBar(ui->caLabel_427, ui->caLabel_428, ui->groupBox_68, ui->label_163);
    setOpiProgressBar(ui->caLabel_427
                      , ui->caLabel_428
                      , ui->caLabel_429
                      , ui->progressBar_68
                      , m_pTimer[67]
                      , m_nFlowCounter[67]);
}

void panel_pfopiform::on_caLabel_434_valueChanged(double value)
{
    setOpiBar(ui->caLabel_433, ui->caLabel_434, ui->groupBox_69, ui->label_210);
    setOpiProgressBar(ui->caLabel_433
                      , ui->caLabel_434
                      , ui->caLabel_435
                      , ui->progressBar_69
                      , m_pTimer[68]
                      , m_nFlowCounter[68]);
}

void panel_pfopiform::on_caLabel_440_valueChanged(double value)
{
    setOpiBar(ui->caLabel_439, ui->caLabel_440, ui->groupBox_70, ui->label_211);
    setOpiProgressBar(ui->caLabel_439
                      , ui->caLabel_440
                      , ui->caLabel_441
                      , ui->progressBar_70
                      , m_pTimer[69]
                      , m_nFlowCounter[69]);
}

void panel_pfopiform::on_caLabel_446_valueChanged(double value)
{
    setOpiBar(ui->caLabel_445, ui->caLabel_446, ui->groupBox_71, ui->label_245);
    setOpiProgressBar(ui->caLabel_445
                      , ui->caLabel_446
                      , ui->caLabel_447
                      , ui->progressBar_71
                      , m_pTimer[70]
                      , m_nFlowCounter[70]);
}

void panel_pfopiform::on_caLabel_452_valueChanged(double value)
{
    setOpiBar(ui->caLabel_451, ui->caLabel_452, ui->groupBox_72, ui->label_247);
    setOpiProgressBar(ui->caLabel_451
                      , ui->caLabel_452
                      , ui->caLabel_453
                      , ui->progressBar_72
                      , m_pTimer[71]
                      , m_nFlowCounter[71]);
}

void panel_pfopiform::on_caLabel_458_valueChanged(double value)
{
    setOpiBar(ui->caLabel_457, ui->caLabel_458, ui->groupBox_73, ui->label_249);
    setOpiProgressBar(ui->caLabel_457
                      , ui->caLabel_458
                      , ui->caLabel_459
                      , ui->progressBar_73
                      , m_pTimer[72]
                      , m_nFlowCounter[72]);
}
void panel_pfopiform::UpdatePFStop(const QString strStyle)
{
    ui->label_27->setStyleSheet(strStyle);
}

void panel_pfopiform::UpdatePFPrepare(const QString strStyle)
{
    ui->label_28->setStyleSheet(strStyle);
}

void panel_pfopiform::UpdatePFOperation(const QString strStyle)
{
    ui->label_29->setStyleSheet(strStyle);
}

void panel_pfopiform::UpdatePFQuench(const QString strStyle)
{
    ui->label_30->setStyleSheet(strStyle);
}

void panel_pfopiform::on_pushButton_49_clicked()
{
    if(m_pPFB01UNCHV11GrpForm == NULL){
        m_pPFB01UNCHV11GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB01UNCHV11.ui",1);
        if(m_pPFB01UNCHV11GrpForm)      m_pPFB01UNCHV11GrpWindow->setCentralWidget(m_pPFB01UNCHV11GrpForm->GetWidget());
    }

    m_pPFB01UNCHV11GrpWindow->show();
}

void panel_pfopiform::on_pushButton_50_clicked()
{
    if(m_pPFB02ICHV18GrpForm == NULL){
        m_pPFB02ICHV18GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB02ICHV18.ui",1);
        if(m_pPFB02ICHV18GrpForm)       m_pPFB02ICHV18GrpWindow->setCentralWidget(m_pPFB02ICHV18GrpForm->GetWidget());
    }

    m_pPFB02ICHV18GrpWindow->show();
}

void panel_pfopiform::on_pushButton_51_clicked()
{
    if(m_pPFB04UPCHV27GrpForm == NULL){
        m_pPFB04UPCHV27GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB04UPCHV27.ui",1);
        if(m_pPFB04UPCHV27GrpForm)      m_pPFB04UPCHV27GrpWindow->setCentralWidget(m_pPFB04UPCHV27GrpForm->GetWidget());
    }

    m_pPFB04UPCHV27GrpWindow->show();
}

void panel_pfopiform::on_pushButton_52_clicked()
{
    if(m_pPFB05UNCHV34GrpForm == NULL){
        m_pPFB05UNCHV34GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB05UNCHV34.ui",1);
        if(m_pPFB05UNCHV34GrpForm)      m_pPFB05UNCHV34GrpWindow->setCentralWidget(m_pPFB05UNCHV34GrpForm->GetWidget());
    }

    m_pPFB05UNCHV34GrpWindow->show();
}


void panel_pfopiform::on_pushButton_53_clicked()
{
    if(m_pPFB06LPCHV39GrpForm == NULL){
        m_pPFB06LPCHV39GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB06LPCHV39.ui",1);
        if(m_pPFB06LPCHV39GrpForm)      m_pPFB06LPCHV39GrpWindow->setCentralWidget(m_pPFB06LPCHV39GrpForm->GetWidget());
    }

    m_pPFB06LPCHV39GrpWindow->show();
}

void panel_pfopiform::on_pushButton_54_clicked()
{
    if(m_pPFB01LPCHV12GrpForm == NULL){
        m_pPFB01LPCHV12GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB01LPCHV12.ui",1);
        if(m_pPFB01LPCHV12GrpForm)      m_pPFB01LPCHV12GrpWindow->setCentralWidget(m_pPFB01LPCHV12GrpForm->GetWidget());
    }

    m_pPFB01LPCHV12GrpWindow->show();
}

void panel_pfopiform::on_pushButton_55_clicked()
{
    if(m_pPFB03UPCHV21GrpForm == NULL){
        m_pPFB03UPCHV21GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB03UPCHV21.ui",1);
        if(m_pPFB03UPCHV21GrpForm)      m_pPFB03UPCHV21GrpWindow->setCentralWidget(m_pPFB03UPCHV21GrpForm->GetWidget());
    }

    m_pPFB03UPCHV21GrpWindow->show();
}

void panel_pfopiform::on_pushButton_56_clicked()
{
    if(m_pPFB04UNCHV28GrpForm == NULL){
        m_pPFB04UNCHV28GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB04UNCHV28.ui",1);
        if(m_pPFB04UNCHV28GrpForm)      m_pPFB04UNCHV28GrpWindow->setCentralWidget(m_pPFB04UNCHV28GrpForm->GetWidget());
    }

    m_pPFB04UNCHV28GrpWindow->show();
}

void panel_pfopiform::on_pushButton_57_clicked()
{
    if(m_pPFB05LPCHV35GrpForm == NULL){
        m_pPFB05LPCHV35GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB05LPCHV35.ui",1);
        if(m_pPFB05LPCHV35GrpForm)      m_pPFB05LPCHV35GrpWindow->setCentralWidget(m_pPFB05LPCHV35GrpForm->GetWidget());
    }

    m_pPFB05LPCHV35GrpWindow->show();
}

void panel_pfopiform::on_pushButton_58_clicked()
{
    if(m_pPFB06LNCHV40GrpForm == NULL){
        m_pPFB06LNCHV40GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB06LNCHV40.ui",1);
        if(m_pPFB06LNCHV40GrpForm)      m_pPFB06LNCHV40GrpWindow->setCentralWidget(m_pPFB06LNCHV40GrpForm->GetWidget());
    }

    m_pPFB06LNCHV40GrpWindow->show();
}

void panel_pfopiform::on_pushButton_59_clicked()
{
    if(m_pPFB01ICHV13GrpForm == NULL){
        m_pPFB01ICHV13GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB01ICHV13.ui",1);
        if(m_pPFB01ICHV13GrpForm)       m_pPFB01ICHV13GrpWindow->setCentralWidget(m_pPFB01ICHV13GrpForm->GetWidget());
    }

    m_pPFB01ICHV13GrpWindow->show();
}

void panel_pfopiform::on_pushButton_60_clicked()
{
    if(m_pPFB03UNCHV22GrpForm == NULL){
        m_pPFB03UNCHV22GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB03UNCHV22.ui",1);
        if(m_pPFB03UNCHV22GrpForm)      m_pPFB03UNCHV22GrpWindow->setCentralWidget(m_pPFB03UNCHV22GrpForm->GetWidget());
    }

    m_pPFB03UNCHV22GrpWindow->show();
}

void panel_pfopiform::on_pushButton_61_clicked()
{
    if(m_pPFB04LPCHV29GrpForm == NULL){
        m_pPFB04LPCHV29GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB04LPCHV29.ui",1);
        if(m_pPFB04LPCHV29GrpForm)      m_pPFB04LPCHV29GrpWindow->setCentralWidget(m_pPFB04LPCHV29GrpForm->GetWidget());
    }

    m_pPFB04LPCHV29GrpWindow->show();
}

void panel_pfopiform::on_pushButton_62_clicked()
{
    if(m_pPFB05LNCHV36GrpForm == NULL){
        m_pPFB05LNCHV36GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB05LNCHV36.ui",1);
        if(m_pPFB05LNCHV36GrpForm)      m_pPFB05LNCHV36GrpWindow->setCentralWidget(m_pPFB05LNCHV36GrpForm->GetWidget());
    }

    m_pPFB05LNCHV36GrpWindow->show();
}

void panel_pfopiform::on_pushButton_63_clicked()
{
    if(m_pPFB07UPCHV41GrpForm == NULL){
        m_pPFB07UPCHV41GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB07UPCHV41.ui",1);
        if(m_pPFB07UPCHV41GrpForm)      m_pPFB07UPCHV41GrpWindow->setCentralWidget(m_pPFB07UPCHV41GrpForm->GetWidget());
    }

    m_pPFB07UPCHV41GrpWindow->show();
}

void panel_pfopiform::on_pushButton_64_clicked()
{
    if(m_pPFB02UPCHV16GrpForm == NULL){
        m_pPFB02UPCHV16GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB02UPCHV16.ui",1);
        if(m_pPFB02UPCHV16GrpForm)      m_pPFB02UPCHV16GrpWindow->setCentralWidget(m_pPFB02UPCHV16GrpForm->GetWidget());
    }

    m_pPFB02UPCHV16GrpWindow->show();
}

void panel_pfopiform::on_pushButton_65_clicked()
{
    if(m_pPFB03LPCHV23GrpForm == NULL){
        m_pPFB03LPCHV23GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB03LPCHV23.ui",1);
        if(m_pPFB03LPCHV23GrpForm)      m_pPFB03LPCHV23GrpWindow->setCentralWidget(m_pPFB03LPCHV23GrpForm->GetWidget());
    }

    m_pPFB03LPCHV23GrpWindow->show();
}

void panel_pfopiform::on_pushButton_66_clicked()
{
    if(m_pPFB04LNCHV30GrpForm == NULL){
        m_pPFB04LNCHV30GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB04LNCHV30.ui",1);
        if(m_pPFB04LNCHV30GrpForm)      m_pPFB04LNCHV30GrpWindow->setCentralWidget(m_pPFB04LNCHV30GrpForm->GetWidget());
    }

    m_pPFB04LNCHV30GrpWindow->show();
}

void panel_pfopiform::on_pushButton_67_clicked()
{
    if(m_pPFB06UPCHV37GrpForm == NULL){
        m_pPFB06UPCHV37GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB06UPCHV37.ui",1);
        if(m_pPFB06UPCHV37GrpForm)      m_pPFB06UPCHV37GrpWindow->setCentralWidget(m_pPFB06UPCHV37GrpForm->GetWidget());
    }

    m_pPFB06UPCHV37GrpWindow->show();
}

void panel_pfopiform::on_pushButton_68_clicked()
{
    if(m_pPFB07LNCHV42GrpForm == NULL){
        m_pPFB07LNCHV42GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB07LNCHV42.ui",1);
        if(m_pPFB07LNCHV42GrpForm)      m_pPFB07LNCHV42GrpWindow->setCentralWidget(m_pPFB07LNCHV42GrpForm->GetWidget());
    }

    m_pPFB07LNCHV42GrpWindow->show();
}

void panel_pfopiform::on_pushButton_69_clicked()
{
    if(m_pPFB02LNCHV17GrpForm == NULL){
        m_pPFB02LNCHV17GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB02LNCHV17.ui",1);
        if(m_pPFB02LNCHV17GrpForm)      m_pPFB02LNCHV17GrpWindow->setCentralWidget(m_pPFB02LNCHV17GrpForm->GetWidget());
    }

    m_pPFB02LNCHV17GrpWindow->show();
}

void panel_pfopiform::on_pushButton_70_clicked()
{
    if(m_pPFB03LNCHV24GrpForm == NULL){
        m_pPFB03LNCHV24GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB03LNCHV24.ui",1);
        if(m_pPFB03LNCHV24GrpForm)      m_pPFB03LNCHV24GrpWindow->setCentralWidget(m_pPFB03LNCHV24GrpForm->GetWidget());
    }

    m_pPFB03LNCHV24GrpWindow->show();
}

void panel_pfopiform::on_pushButton_71_clicked()
{
    if(m_pPFB05UPCHV33GrpForm == NULL){
        m_pPFB05UPCHV33GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB05UPCHV33.ui",1);
        if(m_pPFB05UPCHV33GrpForm)      m_pPFB05UPCHV33GrpWindow->setCentralWidget(m_pPFB05UPCHV33GrpForm->GetWidget());
    }

    m_pPFB05UPCHV33GrpWindow->show();
}

void panel_pfopiform::on_pushButton_72_clicked()
{
    if(m_pPFB06UNCHV38GrpForm == NULL){
        m_pPFB06UNCHV38GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB06UNCHV38.ui",1);
        if(m_pPFB06UNCHV38GrpForm)      m_pPFB06UNCHV38GrpWindow->setCentralWidget(m_pPFB06UNCHV38GrpForm->GetWidget());
    }

    m_pPFB06UNCHV38GrpWindow->show();
}

void panel_pfopiform::on_pushButton_73_clicked()
{
    if(m_pPFB07ICHV43GrpForm == NULL){
        m_pPFB07ICHV43GrpForm = new AttachChannelAccess("/usr/local/opi/ui/pfbus/PFB07ICHV43.ui",1);
        if(m_pPFB07ICHV43GrpForm)       m_pPFB07ICHV43GrpWindow->setCentralWidget(m_pPFB07ICHV43GrpForm->GetWidget());
    }

    m_pPFB07ICHV43GrpWindow->show();
}
