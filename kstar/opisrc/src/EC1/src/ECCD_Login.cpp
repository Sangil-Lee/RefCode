#include "ECCD_Login.h"

ECCD_Login::ECCD_Login(QWidget *parent)
    : QDialog(parent)
{
	codec = QTextCodec::codecForName("eucKR");

	ui.setupUi(this);
	pattach = new AttachChannelAccess(this);

	connect(ui.pushButton, SIGNAL(clicked()), this, SLOT(slot_ECCDMain_Show()));
	connect(ui.pushButton_2, SIGNAL(clicked()), this, SLOT(slot_ECCDClose()));
}

ECCD_Login::~ECCD_Login()
{
      disconnect(ui.pushButton, 0, 0, 0);
      if(ui.pushButton){ delete ui.pushButton; ui.pushButton = NULL; }
}


void ECCD_Login::slot_ECCDMain_Show(void){

	QString passwd = ui.lineEdit->text();

	if (passwd == "kstareccd")
	{	accept();
	} else {
		QMessageBox msgBox;
		msgBox.setText(codec->toUnicode("OPI LOGIN"));
		msgBox.setInformativeText(codec->toUnicode("Not matching Password. Please retry!!"));
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.setDefaultButton(QMessageBox::Ok);
		int ret = msgBox.exec();
	}
}

void ECCD_Login::slot_ECCDClose(void){
	kill(getpid(), SIGTERM);
}

void ECCD_Login::closeEvent(QCloseEvent *)
{
	kill(getpid(), SIGTERM);
}
