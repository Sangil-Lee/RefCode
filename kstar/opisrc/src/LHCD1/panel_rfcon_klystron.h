#ifndef PANEL_RFCON_KLYSTRON_H
#define PANEL_RFCON_KLYSTRON_H

#include <QWidget>
#include <qwt_plot.h>
#include "qtchaccesslib.h"

const int PLOT_SIZE=21;

namespace Ui {
    class panel_rfcon_klystron;
}

class panel_rfcon_klystron : public QWidget {
    Q_OBJECT
public:
    panel_rfcon_klystron(QWidget *parent = 0);
    ~panel_rfcon_klystron();

    AttachChannelAccess *klystron_attach;
    AttachChannelAccess *klyrfgraph_attach;
    AttachChannelAccess *klyantgraph_attach;
    AttachChannelAccess *tlrfgraph_attach;
    AttachChannelAccess *antfwdgraph_attach;
    AttachChannelAccess *antrevgraph_attach;


    double d_x[PLOT_SIZE];
    double d_y[PLOT_SIZE];

protected:
    void changeEvent(QEvent *e);

private:
    Ui::panel_rfcon_klystron *ui;


private slots:
    void on_caLineEdit_12_enteredValue(double value);
    void on_caLineEdit_12_changedValue(double value);
    void on_caLineEdit_13_changedValue(double value);
    void on_caLineEdit_13_returnPressed();
    void on_caLineEdit_13_enteredValue(double value);
    void on_pushButton_clicked(bool checked);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
  
	
};

#endif // PANEL_RFCON_KLYSTRON_H
