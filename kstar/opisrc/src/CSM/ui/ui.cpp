/********************************************************************************
** Form generated from reading ui file 'ICS_Server_TMON_1.ui'
**
** Created: Fri Apr 22 18:41:16 2011
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_ICS_SERVER_TMON_1_H
#define UI_ICS_SERVER_TMON_1_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "caDisplayer.h"

class Ui_Form
{
public:
    QLabel *titleLabel;
    QLabel *label_52;
    QWidget *layoutWidget;
    QVBoxLayout *vboxLayout;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_13;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_9;
    QLabel *label_17;
    QWidget *layoutWidget1;
    QVBoxLayout *vboxLayout1;
    CADisplayer *CADisplayer;
    CADisplayer *CADisplayer_2;
    CADisplayer *CADisplayer_3;
    CADisplayer *CADisplayer_4;
    CADisplayer *CADisplayer_5;
    CADisplayer *CADisplayer_6;
    CADisplayer *CADisplayer_7;
    CADisplayer *CADisplayer_8;
    CADisplayer *CADisplayer_9;
    CADisplayer *CADisplayer_10;
    CADisplayer *CADisplayer_11;
    CADisplayer *CADisplayer_12;
    CADisplayer *CADisplayer_13;
    CADisplayer *CADisplayer_14;
    CADisplayer *CADisplayer_15;
    CADisplayer *CADisplayer_16;
    QWidget *layoutWidget2;
    QVBoxLayout *vboxLayout2;
    QLabel *label_18;
    QLabel *label_19;
    QLabel *label_20;
    QLabel *label_21;
    QLabel *label_22;
    QLabel *label_23;
    QLabel *label_24;
    QLabel *label_25;
    QLabel *label_26;
    QLabel *label_27;
    QLabel *label_28;
    QLabel *label_29;
    QLabel *label_30;
    QLabel *label_31;
    QLabel *label_32;
    QLabel *label_33;
    QDialogButtonBox *buttonBox;

    void setupUi(QWidget *Form)
    {
    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));
    Form->setWindowModality(Qt::WindowModal);
    Form->resize(328, 562);
    titleLabel = new QLabel(Form);
    titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
    titleLabel->setGeometry(QRect(40, 0, 251, 34));
    QPalette palette;
    QBrush brush(QColor(0, 170, 0, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    QBrush brush1(QColor(255, 255, 255, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Base, brush1);
    QBrush brush2(QColor(255, 170, 127, 100));
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Window, brush2);
    palette.setBrush(QPalette::Active, QPalette::Shadow, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::Base, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::Window, brush2);
    palette.setBrush(QPalette::Inactive, QPalette::Shadow, brush);
    QBrush brush3(QColor(127, 125, 123, 255));
    brush3.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush3);
    palette.setBrush(QPalette::Disabled, QPalette::Base, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::Window, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::Shadow, brush);
    titleLabel->setPalette(palette);
    QFont font;
    font.setPointSize(17);
    titleLabel->setFont(font);
    titleLabel->setAutoFillBackground(true);
    titleLabel->setFrameShape(QFrame::WinPanel);
    titleLabel->setAlignment(Qt::AlignCenter);
    label_52 = new QLabel(Form);
    label_52->setObjectName(QString::fromUtf8("label_52"));
    label_52->setGeometry(QRect(136, 521, 155, 24));
    QFont font1;
    font1.setPointSize(12);
    label_52->setFont(font1);
    label_52->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    layoutWidget = new QWidget(Form);
    layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
    layoutWidget->setGeometry(QRect(5, 45, 146, 471));
    vboxLayout = new QVBoxLayout(layoutWidget);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    vboxLayout->setContentsMargins(0, 0, 0, 0);
    label_4 = new QLabel(layoutWidget);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setFont(font1);

    vboxLayout->addWidget(label_4);

    label_2 = new QLabel(layoutWidget);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setFont(font1);

    vboxLayout->addWidget(label_2);

    label_3 = new QLabel(layoutWidget);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setFont(font1);

    vboxLayout->addWidget(label_3);

    label_5 = new QLabel(layoutWidget);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setFont(font1);

    vboxLayout->addWidget(label_5);

    label_6 = new QLabel(layoutWidget);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setFont(font1);

    vboxLayout->addWidget(label_6);

    label_7 = new QLabel(layoutWidget);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setFont(font1);

    vboxLayout->addWidget(label_7);

    label_8 = new QLabel(layoutWidget);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setFont(font1);

    vboxLayout->addWidget(label_8);

    label_10 = new QLabel(layoutWidget);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    label_10->setFont(font1);

    vboxLayout->addWidget(label_10);

    label_11 = new QLabel(layoutWidget);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setFont(font1);

    vboxLayout->addWidget(label_11);

    label_12 = new QLabel(layoutWidget);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setFont(font1);

    vboxLayout->addWidget(label_12);

    label_13 = new QLabel(layoutWidget);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setFont(font1);

    vboxLayout->addWidget(label_13);

    label_14 = new QLabel(layoutWidget);
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setFont(font1);

    vboxLayout->addWidget(label_14);

    label_15 = new QLabel(layoutWidget);
    label_15->setObjectName(QString::fromUtf8("label_15"));
    label_15->setFont(font1);

    vboxLayout->addWidget(label_15);

    label_16 = new QLabel(layoutWidget);
    label_16->setObjectName(QString::fromUtf8("label_16"));
    label_16->setFont(font1);

    vboxLayout->addWidget(label_16);

    label_9 = new QLabel(layoutWidget);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setFont(font1);

    vboxLayout->addWidget(label_9);

    label_17 = new QLabel(layoutWidget);
    label_17->setObjectName(QString::fromUtf8("label_17"));
    label_17->setFont(font1);

    vboxLayout->addWidget(label_17);

    layoutWidget1 = new QWidget(Form);
    layoutWidget1->setObjectName(QString::fromUtf8("layoutWidget1"));
    layoutWidget1->setGeometry(QRect(170, 45, 111, 471));
    vboxLayout1 = new QVBoxLayout(layoutWidget1);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
    vboxLayout1->setContentsMargins(0, 0, 0, 0);
    CADisplayer = new CADisplayer(layoutWidget1);
    CADisplayer->setObjectName(QString::fromUtf8("CADisplayer"));
    CADisplayer->setFont(font1);
    CADisplayer->setAutoFillBackground(false);
    CADisplayer->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer);

    CADisplayer_2 = new CADisplayer(layoutWidget1);
    CADisplayer_2->setObjectName(QString::fromUtf8("CADisplayer_2"));
    CADisplayer_2->setFont(font1);
    CADisplayer_2->setAutoFillBackground(false);
    CADisplayer_2->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_2);

    CADisplayer_3 = new CADisplayer(layoutWidget1);
    CADisplayer_3->setObjectName(QString::fromUtf8("CADisplayer_3"));
    CADisplayer_3->setFont(font1);
    CADisplayer_3->setAutoFillBackground(false);
    CADisplayer_3->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_3);

    CADisplayer_4 = new CADisplayer(layoutWidget1);
    CADisplayer_4->setObjectName(QString::fromUtf8("CADisplayer_4"));
    CADisplayer_4->setFont(font1);
    CADisplayer_4->setAutoFillBackground(false);
    CADisplayer_4->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_4);

    CADisplayer_5 = new CADisplayer(layoutWidget1);
    CADisplayer_5->setObjectName(QString::fromUtf8("CADisplayer_5"));
    CADisplayer_5->setFont(font1);
    CADisplayer_5->setAutoFillBackground(false);
    CADisplayer_5->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_5);

    CADisplayer_6 = new CADisplayer(layoutWidget1);
    CADisplayer_6->setObjectName(QString::fromUtf8("CADisplayer_6"));
    CADisplayer_6->setFont(font1);
    CADisplayer_6->setAutoFillBackground(false);
    CADisplayer_6->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_6);

    CADisplayer_7 = new CADisplayer(layoutWidget1);
    CADisplayer_7->setObjectName(QString::fromUtf8("CADisplayer_7"));
    CADisplayer_7->setFont(font1);
    CADisplayer_7->setAutoFillBackground(false);
    CADisplayer_7->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_7);

    CADisplayer_8 = new CADisplayer(layoutWidget1);
    CADisplayer_8->setObjectName(QString::fromUtf8("CADisplayer_8"));
    CADisplayer_8->setFont(font1);
    CADisplayer_8->setAutoFillBackground(false);
    CADisplayer_8->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_8);

    CADisplayer_9 = new CADisplayer(layoutWidget1);
    CADisplayer_9->setObjectName(QString::fromUtf8("CADisplayer_9"));
    CADisplayer_9->setFont(font1);
    CADisplayer_9->setAutoFillBackground(false);
    CADisplayer_9->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_9);

    CADisplayer_10 = new CADisplayer(layoutWidget1);
    CADisplayer_10->setObjectName(QString::fromUtf8("CADisplayer_10"));
    CADisplayer_10->setFont(font1);
    CADisplayer_10->setAutoFillBackground(false);
    CADisplayer_10->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_10);

    CADisplayer_11 = new CADisplayer(layoutWidget1);
    CADisplayer_11->setObjectName(QString::fromUtf8("CADisplayer_11"));
    CADisplayer_11->setFont(font1);
    CADisplayer_11->setAutoFillBackground(false);
    CADisplayer_11->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_11);

    CADisplayer_12 = new CADisplayer(layoutWidget1);
    CADisplayer_12->setObjectName(QString::fromUtf8("CADisplayer_12"));
    CADisplayer_12->setFont(font1);
    CADisplayer_12->setAutoFillBackground(false);
    CADisplayer_12->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_12);

    CADisplayer_13 = new CADisplayer(layoutWidget1);
    CADisplayer_13->setObjectName(QString::fromUtf8("CADisplayer_13"));
    CADisplayer_13->setFont(font1);
    CADisplayer_13->setAutoFillBackground(false);
    CADisplayer_13->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_13);

    CADisplayer_14 = new CADisplayer(layoutWidget1);
    CADisplayer_14->setObjectName(QString::fromUtf8("CADisplayer_14"));
    CADisplayer_14->setFont(font1);
    CADisplayer_14->setAutoFillBackground(false);
    CADisplayer_14->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_14);

    CADisplayer_15 = new CADisplayer(layoutWidget1);
    CADisplayer_15->setObjectName(QString::fromUtf8("CADisplayer_15"));
    CADisplayer_15->setFont(font1);
    CADisplayer_15->setAutoFillBackground(false);
    CADisplayer_15->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_15);

    CADisplayer_16 = new CADisplayer(layoutWidget1);
    CADisplayer_16->setObjectName(QString::fromUtf8("CADisplayer_16"));
    CADisplayer_16->setFont(font1);
    CADisplayer_16->setAutoFillBackground(false);
    CADisplayer_16->setProperty("nuformat", QVariant(CADisplayer::RealTwo));

    vboxLayout1->addWidget(CADisplayer_16);

    layoutWidget2 = new QWidget(Form);
    layoutWidget2->setObjectName(QString::fromUtf8("layoutWidget2"));
    layoutWidget2->setGeometry(QRect(280, 45, 43, 471));
    vboxLayout2 = new QVBoxLayout(layoutWidget2);
    vboxLayout2->setObjectName(QString::fromUtf8("vboxLayout2"));
    vboxLayout2->setContentsMargins(0, 0, 0, 0);
    label_18 = new QLabel(layoutWidget2);
    label_18->setObjectName(QString::fromUtf8("label_18"));
    QFont font2;
    font2.setPointSize(9);
    label_18->setFont(font2);

    vboxLayout2->addWidget(label_18);

    label_19 = new QLabel(layoutWidget2);
    label_19->setObjectName(QString::fromUtf8("label_19"));
    label_19->setFont(font2);

    vboxLayout2->addWidget(label_19);

    label_20 = new QLabel(layoutWidget2);
    label_20->setObjectName(QString::fromUtf8("label_20"));
    label_20->setFont(font2);

    vboxLayout2->addWidget(label_20);

    label_21 = new QLabel(layoutWidget2);
    label_21->setObjectName(QString::fromUtf8("label_21"));
    label_21->setFont(font2);

    vboxLayout2->addWidget(label_21);

    label_22 = new QLabel(layoutWidget2);
    label_22->setObjectName(QString::fromUtf8("label_22"));
    label_22->setFont(font2);

    vboxLayout2->addWidget(label_22);

    label_23 = new QLabel(layoutWidget2);
    label_23->setObjectName(QString::fromUtf8("label_23"));
    label_23->setFont(font2);

    vboxLayout2->addWidget(label_23);

    label_24 = new QLabel(layoutWidget2);
    label_24->setObjectName(QString::fromUtf8("label_24"));
    label_24->setFont(font2);

    vboxLayout2->addWidget(label_24);

    label_25 = new QLabel(layoutWidget2);
    label_25->setObjectName(QString::fromUtf8("label_25"));
    label_25->setFont(font2);

    vboxLayout2->addWidget(label_25);

    label_26 = new QLabel(layoutWidget2);
    label_26->setObjectName(QString::fromUtf8("label_26"));
    label_26->setFont(font2);

    vboxLayout2->addWidget(label_26);

    label_27 = new QLabel(layoutWidget2);
    label_27->setObjectName(QString::fromUtf8("label_27"));
    label_27->setFont(font2);

    vboxLayout2->addWidget(label_27);

    label_28 = new QLabel(layoutWidget2);
    label_28->setObjectName(QString::fromUtf8("label_28"));
    label_28->setFont(font2);

    vboxLayout2->addWidget(label_28);

    label_29 = new QLabel(layoutWidget2);
    label_29->setObjectName(QString::fromUtf8("label_29"));
    label_29->setFont(font2);

    vboxLayout2->addWidget(label_29);

    label_30 = new QLabel(layoutWidget2);
    label_30->setObjectName(QString::fromUtf8("label_30"));
    label_30->setFont(font2);

    vboxLayout2->addWidget(label_30);

    label_31 = new QLabel(layoutWidget2);
    label_31->setObjectName(QString::fromUtf8("label_31"));
    label_31->setFont(font2);

    vboxLayout2->addWidget(label_31);

    label_32 = new QLabel(layoutWidget2);
    label_32->setObjectName(QString::fromUtf8("label_32"));
    label_32->setFont(font2);

    vboxLayout2->addWidget(label_32);

    label_33 = new QLabel(layoutWidget2);
    label_33->setObjectName(QString::fromUtf8("label_33"));
    label_33->setFont(font2);

    vboxLayout2->addWidget(label_33);

    buttonBox = new QDialogButtonBox(Form);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setGeometry(QRect(155, 525, 166, 32));
    buttonBox->setStandardButtons(QDialogButtonBox::Close);

    retranslateUi(Form);
    QObject::connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), Form, SLOT(close()));

    QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
    Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    titleLabel->setText(QApplication::translate("Form", "SERVER", 0, QApplication::UnicodeUTF8));
    label_52->setText(QString());
    label_4->setText(QApplication::translate("Form", "CCS RACK", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("Form", "SIS RACK", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("Form", "RMS RACK", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("Form", "PSI RACK", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("Form", "PCS RACK", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("Form", "DIAG RACK", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("Form", "Outdoor Temp", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("Form", "OPI RACK#1", 0, QApplication::UnicodeUTF8));
    label_11->setText(QApplication::translate("Form", "OPI RACK#2", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("Form", "OPI RACK#3", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("Form", "OPI RACK#4", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("Form", "DLP Temp", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("Form", "Audio Rack", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("Form", "Video Rack", 0, QApplication::UnicodeUTF8));
    label_9->setText(QApplication::translate("Form", "MCR(Dev) Temp", 0, QApplication::UnicodeUTF8));
    label_17->setText(QApplication::translate("Form", "MCR(Desk) Temp", 0, QApplication::UnicodeUTF8));
    CADisplayer->setPvname(QApplication::translate("Form", "TMON_CCS_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_2->setPvname(QApplication::translate("Form", "TMON_SIS_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_3->setPvname(QApplication::translate("Form", "TMON_RMS_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_4->setPvname(QApplication::translate("Form", "TMON_PSI_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_5->setPvname(QApplication::translate("Form", "TMON_PCS_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_6->setPvname(QApplication::translate("Form", "TMON_DIAG_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_7->setPvname(QApplication::translate("Form", "TMON_EXT", 0, QApplication::UnicodeUTF8));
    CADisplayer_8->setPvname(QApplication::translate("Form", "TMON_OPI_RACK1", 0, QApplication::UnicodeUTF8));
    CADisplayer_9->setPvname(QApplication::translate("Form", "TMON_OPI_RACK2", 0, QApplication::UnicodeUTF8));
    CADisplayer_10->setPvname(QApplication::translate("Form", "TMON_OPI_RACK3", 0, QApplication::UnicodeUTF8));
    CADisplayer_11->setPvname(QApplication::translate("Form", "TMON_OPI_RACK4", 0, QApplication::UnicodeUTF8));
    CADisplayer_12->setPvname(QApplication::translate("Form", "TMON_DLP", 0, QApplication::UnicodeUTF8));
    CADisplayer_13->setPvname(QApplication::translate("Form", "TMON_AUDIO_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_14->setPvname(QApplication::translate("Form", "TMON_VIDEO_RACK", 0, QApplication::UnicodeUTF8));
    CADisplayer_15->setPvname(QApplication::translate("Form", "TMON_MCR_DEV", 0, QApplication::UnicodeUTF8));
    CADisplayer_16->setPvname(QApplication::translate("Form", "TMON_MCS", 0, QApplication::UnicodeUTF8));
    label_18->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_19->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_20->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_21->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_22->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_23->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_24->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_25->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_26->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_27->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_28->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_29->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_30->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_31->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_32->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    label_33->setText(QApplication::translate("Form", "Celsius", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Form);
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

#endif // UI_ICS_SERVER_TMON_1_H
