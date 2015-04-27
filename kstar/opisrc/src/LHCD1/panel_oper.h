#ifndef PANEL_OPER_H
#define PANEL_OPER_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_oper;
}

class panel_oper : public QWidget {
    Q_OBJECT
public:
    panel_oper(QWidget *parent = 0);
    ~panel_oper();

    AttachChannelAccess *oper_attach;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_oper *ui;

//private slots:
//    void on_pushButton_2_clicked();
};

#endif // PANEL_OPER_H
