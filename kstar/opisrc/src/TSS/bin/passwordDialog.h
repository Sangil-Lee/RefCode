#ifndef PASSWORD_DIALOG_H
#define PASSWRD_DIALOG_H

#include <QDialog>

#include "ui_TSS_passwdDialog.h"

class passwordDlg : public QDialog, public Ui::passwdDlg
{
    Q_OBJECT

public:
    passwordDlg(QWidget *parent = 0);

protected:
//	void closeEvent(QCloseEvent *event);

private slots:


private:



};



#endif

