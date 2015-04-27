#include "panel_tfsetform.h"
#include "ui_panel_tfsetform.h"

panel_tfsetform::panel_tfsetform(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::panel_tfsetform)
{
    ui->setupUi(this);

    tfsetform_pattach = new AttachChannelAccess(this);
}

panel_tfsetform::~panel_tfsetform()
{
    delete ui;
}

void panel_tfsetform::changeEvent(QEvent *e)
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

void panel_tfsetform::on_pushButton_clicked()
{
      emit OnClickNewPv();
}
