#ifndef PANEL_TFSETFORM_H
#define PANEL_TFSETFORM_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_tfsetform;
}

class panel_tfsetform : public QWidget {
    Q_OBJECT
public:
    panel_tfsetform(QWidget *parent = 0);
    ~panel_tfsetform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_tfsetform *ui;

    AttachChannelAccess *tfsetform_pattach;

private slots:
    void on_pushButton_clicked();

signals:
    void OnClickNewPv();
};

#endif // PANEL_TFSETFORM_H
