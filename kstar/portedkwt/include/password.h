#ifndef PASSWORD_H
#define PASSWORD_H

#include <QObject>
#include <QtGui>
#include <QtGlobal>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>

class Password : public QDialog
{
	Q_OBJECT 
public:
	Password(QString word, QWidget *parent=NULL);
	~Password();
    QLabel *label;
    QLineEdit *lineEdit;
    QPushButton *pushButton;

	void setStatus( bool set);
	bool getStatus();

public slots:
	void Check();
	
private:
	void makeUI();
	QString passwd;
	bool status;
};
#endif // UI_PASSWORD_H

