#ifndef PANEL_CSSETFORM_H
#define PANEL_CSSETFORM_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_cssetform;
}

class panel_cssetform : public QWidget {
    Q_OBJECT
public:
    panel_cssetform(QWidget *parent = 0);
    ~panel_cssetform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_cssetform *ui;
    AttachChannelAccess *cssetform_pattach;

private slots:
    void on_pushButton_clicked();
signals:
    void OnClickNewPv();
};

#endif // PANEL_CSSETFORM_H
