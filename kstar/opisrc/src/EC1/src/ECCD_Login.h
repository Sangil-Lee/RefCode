#ifndef ECCD_LOGIN_H
#define ECCD_LOGIN_H

#include <QtGui/QDialog>
#include <signal.h>

#include "ui_ECCD_LOGIN.h"
#include "qtchaccesslib.h"

// Sub Header File Include
//#include "ECCD_Main.h"

class ECCD_Login : public QDialog
{
    Q_OBJECT

public:
    ECCD_Login(QWidget *parent = 0);
    ~ECCD_Login();

// slot define
private slots:
	void slot_ECCDMain_Show(void);
	void slot_ECCDClose(void);

private:
	AttachChannelAccess *pattach;
	void closeEvent(QCloseEvent *event);
	Ui::ECCDLOGIN ui;

// Sub Form Class define   
//    	ECCD_Main		*ECCDMAIN;
	QTextCodec *codec;
};

#endif // ECCD_LOGIN_H
 

 
