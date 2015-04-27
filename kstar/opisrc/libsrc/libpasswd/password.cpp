/********************************************************************************
** Form generated from reading ui file 'password.ui'
**
** Created: Mon Jun 11 17:33:33 2007
**      by: Qt User Interface Compiler version 4.2.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/
#include <password.h>

Password::Password(QString word, QWidget *widget):QDialog(widget)
{
	passwd = word;
	status = false;
	makeUI();
}
Password::~Password()
{
}
	
void 
Password::makeUI()
{
	setWindowTitle("Password!");
	setMinimumSize(400, 300);
	setMaximumSize(400, 300);

	QFont font;
	font.setPointSize(20);
	label = new QLabel(this);
	label->setObjectName(QString::fromUtf8("label"));
	label->setGeometry(QRect(90, 40, 151, 61));
	label->setFont(font);
    label->setText("Password");

	lineEdit = new QLineEdit(this);
	lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
	lineEdit->setGeometry(QRect(80, 100, 171, 41));
	font.setPointSize(16);
	font.setBold(true);
	font.setWeight(75);
	lineEdit->setFont(font);
	lineEdit->setEchoMode(QLineEdit::Password);
	pushButton = new QPushButton(this);
	pushButton->setObjectName(QString::fromUtf8("pushButton"));
	pushButton->setGeometry(QRect(220, 220, 151, 51));
	font.setPointSize(18);
	pushButton->setFont(font);
    pushButton->setText("Confirm");

	//connect(pushButton, SIGNAL(clicked()), this, SLOT(accept()) );
	connect(pushButton, SIGNAL(clicked()), this, SLOT(Check()) );

	if (exec())
	{
		//
		// accept
		setStatus(true);

	}

}
void 
Password::setStatus(bool set)
{
	status = set;
};
bool 
Password::getStatus()
{
	return status;
};
void 
Password::Check()
{
	if(passwd.compare(lineEdit->text()) == 0 )
	{
		accept();
	}
	else
	{
		QMessageBox::information(this, "Password", "Mismatch!");
	}
}
