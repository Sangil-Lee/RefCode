#include "panel_rfcon_klystron.h"
#include "ui_LH1_rfcon_klystron.h"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <QMessageBox>

panel_rfcon_klystron::panel_rfcon_klystron(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_rfcon_klystron)
{
    ui->setupUi(this);

    klystron_attach = new AttachChannelAccess(this);

    ui->caLineEdit_12->setText("0");
    ui->caLineEdit_12->valueEntered();
 	
   
}

panel_rfcon_klystron::~panel_rfcon_klystron()
{
    delete ui;
}

void panel_rfcon_klystron::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void panel_rfcon_klystron::on_pushButton_clicked()
{

    for(int i =0;i< 21;i++)
    {
        d_x[i] = 0;
        d_y[i] = 0;
    }


    //d_x[0] = ui->caLineEdit_12->text().toDouble();
    d_x[0] = ui->caLabel_12->text().toDouble();
    //d_y[0] = (dbPout * ui->caLineEdit_24->text().toDouble())/100;
    //d_y[0] = ui->caLineEdit_24->text().toDouble();
    d_y[0] = ui->caLabel_52->text().toDouble();

    //d_x[1] = ui->caLineEdit_13->text().toDouble();
    //d_y[1] = (dbPout * ui->caLineEdit_23->text().toDouble())/100;
    //d_y[1] = ui->caLineEdit_23->text().toDouble();
    d_x[1] = ui->caLabel_32->text().toDouble();
    d_y[1] = ui->caLabel_65->text().toDouble();


    //d_x[2] = ui->caLineEdit_14->text().toDouble();
    //d_y[2] = ui->caLineEdit_29->text().toDouble();
    d_x[2] = ui->caLabel_33->text().toDouble();
    d_y[2] = ui->caLabel_68->text().toDouble();

    //d_x[3] = ui->caLineEdit_15->text().toDouble();
    //d_y[3] = ui->caLineEdit_27->text().toDouble();
    d_x[3] = ui->caLabel_34->text().toDouble();
    d_y[3] = ui->caLabel_64->text().toDouble();

    //d_x[4] = ui->caLineEdit_16->text().toDouble();
    //d_y[4] = ui->caLineEdit_25->text().toDouble();
    d_x[4] = ui->caLabel_35->text().toDouble();
    d_y[4] = ui->caLabel_58->text().toDouble();

    //d_x[5] = ui->caLineEdit_17->text().toDouble();
    //d_y[5] = ui->caLineEdit_28->text().toDouble();
    d_x[5] = ui->caLabel_36->text().toDouble();
    d_y[5] = ui->caLabel_67->text().toDouble();

    //d_x[6] = ui->caLineEdit_18->text().toDouble();
    //d_y[6] = ui->caLineEdit_31->text().toDouble();
    d_x[6] = ui->caLabel_37->text().toDouble();
    d_y[6] = ui->caLabel_71->text().toDouble();

    //d_x[7] = ui->caLineEdit_19->text().toDouble();
    //d_y[7] = ui->caLineEdit_30->text().toDouble();
    d_x[7] = ui->caLabel_38->text().toDouble();
    d_y[7] = ui->caLabel_59->text().toDouble();

    //d_x[8] = ui->caLineEdit_20->text().toDouble();
    //d_y[8] = ui->caLineEdit_22->text().toDouble();
    d_x[8] = ui->caLabel_39->text().toDouble();
    d_y[8] = ui->caLabel_53->text().toDouble();

    //d_x[9] = ui->caLineEdit_21->text().toDouble();
    //d_y[9] = ui->caLineEdit_26->text().toDouble();
    d_x[9] = ui->caLabel_40->text().toDouble();
    d_y[9] = ui->caLabel_63->text().toDouble();

    //d_x[10] = ui->caLineEdit_34->text().toDouble();
    //d_y[10] = ui->caLineEdit_43->text().toDouble();
    d_x[10] = ui->caLabel_41->text().toDouble();
    d_y[10] = ui->caLabel_66->text().toDouble();

    //d_x[11] = ui->caLineEdit_49->text().toDouble();
    //d_y[11] = ui->caLineEdit_51->text().toDouble();
    d_x[11] = ui->caLabel_42->text().toDouble();
    d_y[11] = ui->caLabel_55->text().toDouble();

    //d_x[12] = ui->caLineEdit_38->text().toDouble();
    //d_y[12] = ui->caLineEdit_46->text().toDouble();
    d_x[12] = ui->caLabel_43->text().toDouble();
    d_y[12] = ui->caLabel_57->text().toDouble();

    //d_x[13] = ui->caLineEdit_40->text().toDouble();
    //d_y[13] = ui->caLineEdit_53->text().toDouble();
    d_x[13] = ui->caLabel_44->text().toDouble();
    d_y[13] = ui->caLabel_56->text().toDouble();

    //d_x[14] = ui->caLineEdit_50->text().toDouble();
    //d_y[14] = ui->caLineEdit_48->text().toDouble();
    d_x[14] = ui->caLabel_45->text().toDouble();
    d_y[14] = ui->caLabel_60->text().toDouble();

    //d_x[15] = ui->caLineEdit_42->text().toDouble();
    //d_y[15] = ui->caLineEdit_35->text().toDouble();
    d_x[15] = ui->caLabel_46->text().toDouble();
    d_y[15] = ui->caLabel_69->text().toDouble();

    //d_x[16] = ui->caLineEdit_47->text().toDouble();
    //d_y[16] = ui->caLineEdit_41->text().toDouble();
    d_x[16] = ui->caLabel_47->text().toDouble();
    d_y[16] = ui->caLabel_62->text().toDouble();

    //d_x[17] = ui->caLineEdit_45->text().toDouble();
    //d_y[17] = ui->caLineEdit_44->text().toDouble();
    d_x[17] = ui->caLabel_48->text().toDouble();
    d_y[17] = ui->caLabel_70->text().toDouble();

    //d_x[18] = ui->caLineEdit_39->text().toDouble();
    //d_y[18] = ui->caLineEdit_36->text().toDouble();
    d_x[18] = ui->caLabel_49->text().toDouble();
    d_y[18] = ui->caLabel_61->text().toDouble();

    //d_x[19] = ui->caLineEdit_52->text().toDouble();
    //d_y[19] = ui->caLineEdit_37->text().toDouble();
    d_x[19] = ui->caLabel_50->text().toDouble();
    d_y[19] = ui->caLabel_72->text().toDouble();

    //d_x[20] = ui->caLineEdit_62->text().toDouble();
    //d_y[20] = ui->caLineEdit_61->text().toDouble();
    d_x[20] = ui->caLabel_51->text().toDouble();
    d_y[20] = ui->caLabel_54->text().toDouble();



    QwtPlotCurve *curve1 = new QwtPlotCurve("Curve");

    curve1->setPen(QPen(Qt::red));
    curve1->setStyle(QwtPlotCurve::Steps);

    curve1->setRawData(d_x,d_y,PLOT_SIZE);
    curve1->attach(ui->qwtPlot);
    ui->qwtPlot->replot();
}


void panel_rfcon_klystron::on_pushButton_clicked(bool checked)
{

}

void panel_rfcon_klystron::on_pushButton_2_clicked()
{
	klyrfgraph_attach = new AttachChannelAccess("/usr/local/opi/ui/LH1_klyrfgraph.ui",1);
	klyrfgraph_attach->GetWidget();    
}

void panel_rfcon_klystron::on_pushButton_3_clicked()
{
        klyantgraph_attach = new AttachChannelAccess("/usr/local/opi/ui/LH1_antrfgraph.ui",1);
        klyantgraph_attach->GetWidget();
}

void panel_rfcon_klystron::on_pushButton_4_clicked()
{
        tlrfgraph_attach = new AttachChannelAccess("/usr/local/opi/ui/LH1_tlrfgraph.ui",1);
        tlrfgraph_attach->GetWidget();
}

void panel_rfcon_klystron::on_pushButton_5_clicked()
{
        antfwdgraph_attach = new AttachChannelAccess("/usr/local/opi/ui/LH1_antfwdrfgraph.ui",1);
        antfwdgraph_attach->GetWidget();
}

void panel_rfcon_klystron::on_pushButton_6_clicked()
{
        antrevgraph_attach = new AttachChannelAccess("/usr/local/opi/ui/LH1_antrevrfgraph.ui",1);
        antrevgraph_attach->GetWidget();
}


void panel_rfcon_klystron::on_caLineEdit_13_enteredValue(double value)
{
    //double dbValue = ui->caLineEdit_12->text().toDouble();
    //double dbcaLineEdit_13 = ui->caLineEdit_13->text().toDouble();
    //if(dbcaLineEdit_13 <= dbValue)
    //{
    //    QMessageBox::information(this, "Value Check","Set Value Check");
    //}
}

void panel_rfcon_klystron::on_caLineEdit_13_returnPressed()
{

}

void panel_rfcon_klystron::on_caLineEdit_13_changedValue(double value)
{

}

void panel_rfcon_klystron::on_caLineEdit_12_changedValue(double value)
{

}

void panel_rfcon_klystron::on_caLineEdit_12_enteredValue(double value)
{

}
