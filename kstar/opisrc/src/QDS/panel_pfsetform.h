#ifndef PANEL_PFSETFORM_H
#define PANEL_PFSETFORM_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_pfsetform;
}

class panel_pfsetform : public QWidget {
    Q_OBJECT
public:
    panel_pfsetform(QWidget *parent = 0);
    ~panel_pfsetform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_pfsetform *ui;

    AttachChannelAccess *pfsetform_pattach;

private slots:
    void on_pushButton_clicked();

signals:
    void OnClickNewPv();
};

#endif // PANEL_PFSETFORM_H
