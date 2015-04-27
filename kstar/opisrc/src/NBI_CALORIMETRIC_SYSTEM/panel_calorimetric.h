#ifndef PANEL_CALORIMETRIC_H
#define PANEL_CALORIMETRIC_H

#include <QWidget>
#include "qtchaccesslib.h"

namespace Ui {
    class panel_calorimetric;
}

class panel_calorimetric : public QWidget {
    Q_OBJECT
public:
    panel_calorimetric(QWidget *parent = 0);
    ~panel_calorimetric();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_calorimetric *ui;
    AttachChannelAccess *calorimetric_attach;
    AttachChannelAccess *static_attach;
    //AttachChannelAccess *tab1_attach;
    //AttachChannelAccess *tab2_attach;

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
};

#endif // PANEL_CALORIMETRIC_H
