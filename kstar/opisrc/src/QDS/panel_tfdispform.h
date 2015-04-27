#ifndef PANEL_TFDISPFORM_H
#define PANEL_TFDISPFORM_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_tfdispform;
}

class panel_tfdispform : public QWidget {
    Q_OBJECT
public:
    panel_tfdispform(QWidget *parent = 0);
    ~panel_tfdispform();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_tfdispform *ui;

    AttachChannelAccess *tfdispform_pattach;

private slots:
    void on_pushButton_clicked();

signals:
    void OnClickNewPv();
};

#endif // PANEL_TFDISPFORM_H
