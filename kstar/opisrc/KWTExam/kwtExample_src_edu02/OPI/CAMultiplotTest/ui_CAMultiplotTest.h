/********************************************************************************
** Form generated from reading ui file 'CAMultiplotTest.ui'
**
** Created: Mon Nov 15 15:02:28 2010
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CAMULTIPLOTTEST_H
#define UI_CAMULTIPLOTTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include "caColorcheckbox.h"
#include "caMultiplot.h"

class Ui_Form
{
public:
    CAMultiplot *CAMultiplot;
    CAColorCheckBox *CAColorCheckBox_2;
    CAColorCheckBox *CAColorCheckBox_4;
    QComboBox *comboBox_3;
    QLabel *label;
    CAColorCheckBox *CAColorCheckBox;
    CAColorCheckBox *CAColorCheckBox_3;
    QLabel *label_13;
    QLabel *label_14;
    QComboBox *comboBox;
    QLabel *label_9;
    QComboBox *comboBox_4;
    QLabel *label_11;
    CAColorCheckBox *CAColorCheckBox_10;
    QLabel *label_12;
    CAColorCheckBox *CAColorCheckBox_7;
    QLabel *label_10;
    CAColorCheckBox *CAColorCheckBox_9;
    CAColorCheckBox *CAColorCheckBox_8;
    QLabel *label_15;
    CAColorCheckBox *CAColorCheckBox_6;
    QComboBox *comboBox_2;
    CAColorCheckBox *CAColorCheckBox_5;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox;
    QLabel *label_16;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_8;
    QLabel *label_7;

    void setupUi(QWidget *Form)
    {
    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));
    Form->resize(1280, 1024);
    CAMultiplot = new CAMultiplot(Form);
    CAMultiplot->setObjectName(QString::fromUtf8("CAMultiplot"));
    CAMultiplot->setGeometry(QRect(0, 40, 1021, 768));
    QPalette palette;
    QBrush brush(QColor(0, 0, 0, 255));
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::Text, brush);
    QBrush brush1(QColor(170, 85, 255, 255));
    brush1.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, brush1);
    palette.setBrush(QPalette::Inactive, QPalette::Text, brush);
    palette.setBrush(QPalette::Inactive, QPalette::ButtonText, brush1);
    QBrush brush2(QColor(115, 115, 115, 255));
    brush2.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Disabled, QPalette::Text, brush2);
    palette.setBrush(QPalette::Disabled, QPalette::ButtonText, brush2);
    CAMultiplot->setPalette(palette);
    CAMultiplot->setAutoFillBackground(true);
    CAMultiplot->setProperty("canvasColor", QVariant(QColor(255, 255, 255)));
    CAMultiplot->setProperty("majLinecolor", QVariant(QColor(255, 0, 255)));
    CAMultiplot->setProperty("minLinecolor", QVariant(QColor(160, 160, 255)));
    CAMultiplot->setProperty("logScale", QVariant(true));
    CAColorCheckBox_2 = new CAColorCheckBox(Form);
    CAColorCheckBox_2->setObjectName(QString::fromUtf8("CAColorCheckBox_2"));
    CAColorCheckBox_2->setGeometry(QRect(1040, 110, 139, 22));
    CAColorCheckBox_4 = new CAColorCheckBox(Form);
    CAColorCheckBox_4->setObjectName(QString::fromUtf8("CAColorCheckBox_4"));
    CAColorCheckBox_4->setGeometry(QRect(1040, 160, 139, 22));
    comboBox_3 = new QComboBox(Form);
    comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
    comboBox_3->setGeometry(QRect(260, 10, 72, 25));
    label = new QLabel(Form);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(1190, 80, 79, 22));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox = new CAColorCheckBox(Form);
    CAColorCheckBox->setObjectName(QString::fromUtf8("CAColorCheckBox"));
    CAColorCheckBox->setGeometry(QRect(1040, 80, 139, 22));
    CAColorCheckBox_3 = new CAColorCheckBox(Form);
    CAColorCheckBox_3->setObjectName(QString::fromUtf8("CAColorCheckBox_3"));
    CAColorCheckBox_3->setGeometry(QRect(1040, 140, 139, 22));
    label_13 = new QLabel(Form);
    label_13->setObjectName(QString::fromUtf8("label_13"));
    label_13->setGeometry(QRect(1190, 250, 79, 22));
    label_13->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    label_14 = new QLabel(Form);
    label_14->setObjectName(QString::fromUtf8("label_14"));
    label_14->setGeometry(QRect(1190, 280, 79, 22));
    label_14->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    comboBox = new QComboBox(Form);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));
    comboBox->setGeometry(QRect(0, 10, 165, 25));
    label_9 = new QLabel(Form);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setGeometry(QRect(1190, 140, 79, 22));
    label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    comboBox_4 = new QComboBox(Form);
    comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
    comboBox_4->setGeometry(QRect(330, 10, 72, 25));
    label_11 = new QLabel(Form);
    label_11->setObjectName(QString::fromUtf8("label_11"));
    label_11->setGeometry(QRect(1190, 190, 79, 22));
    label_11->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox_10 = new CAColorCheckBox(Form);
    CAColorCheckBox_10->setObjectName(QString::fromUtf8("CAColorCheckBox_10"));
    CAColorCheckBox_10->setGeometry(QRect(1040, 330, 139, 22));
    label_12 = new QLabel(Form);
    label_12->setObjectName(QString::fromUtf8("label_12"));
    label_12->setGeometry(QRect(1190, 220, 79, 22));
    label_12->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox_7 = new CAColorCheckBox(Form);
    CAColorCheckBox_7->setObjectName(QString::fromUtf8("CAColorCheckBox_7"));
    CAColorCheckBox_7->setGeometry(QRect(1040, 250, 139, 22));
    label_10 = new QLabel(Form);
    label_10->setObjectName(QString::fromUtf8("label_10"));
    label_10->setGeometry(QRect(1190, 160, 79, 22));
    label_10->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox_9 = new CAColorCheckBox(Form);
    CAColorCheckBox_9->setObjectName(QString::fromUtf8("CAColorCheckBox_9"));
    CAColorCheckBox_9->setGeometry(QRect(1040, 300, 139, 22));
    CAColorCheckBox_8 = new CAColorCheckBox(Form);
    CAColorCheckBox_8->setObjectName(QString::fromUtf8("CAColorCheckBox_8"));
    CAColorCheckBox_8->setGeometry(QRect(1040, 280, 139, 22));
    label_15 = new QLabel(Form);
    label_15->setObjectName(QString::fromUtf8("label_15"));
    label_15->setGeometry(QRect(1190, 300, 79, 22));
    label_15->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox_6 = new CAColorCheckBox(Form);
    CAColorCheckBox_6->setObjectName(QString::fromUtf8("CAColorCheckBox_6"));
    CAColorCheckBox_6->setGeometry(QRect(1040, 220, 139, 22));
    comboBox_2 = new QComboBox(Form);
    comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
    comboBox_2->setGeometry(QRect(190, 10, 72, 25));
    CAColorCheckBox_5 = new CAColorCheckBox(Form);
    CAColorCheckBox_5->setObjectName(QString::fromUtf8("CAColorCheckBox_5"));
    CAColorCheckBox_5->setGeometry(QRect(1040, 190, 139, 22));
    label_2 = new QLabel(Form);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(1190, 110, 79, 22));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    doubleSpinBox = new QDoubleSpinBox(Form);
    doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
    doubleSpinBox->setGeometry(QRect(420, 10, 101, 25));
    doubleSpinBox->setMaximum(1e+08);
    label_16 = new QLabel(Form);
    label_16->setObjectName(QString::fromUtf8("label_16"));
    label_16->setGeometry(QRect(1190, 330, 79, 22));
    label_16->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    label_3 = new QLabel(Form);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(980, 780, 54, 17));
    label_4 = new QLabel(Form);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(800, 780, 54, 17));
    label_5 = new QLabel(Form);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(610, 780, 54, 17));
    label_6 = new QLabel(Form);
    label_6->setObjectName(QString::fromUtf8("label_6"));
    label_6->setGeometry(QRect(420, 780, 54, 17));
    label_8 = new QLabel(Form);
    label_8->setObjectName(QString::fromUtf8("label_8"));
    label_8->setGeometry(QRect(50, 780, 54, 17));
    label_7 = new QLabel(Form);
    label_7->setObjectName(QString::fromUtf8("label_7"));
    label_7->setGeometry(QRect(240, 780, 54, 17));

    retranslateUi(Form);
    QObject::connect(CAMultiplot, SIGNAL(currentValue(double)), label, SLOT(setNum(double)));
    QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), CAMultiplot, SLOT(penIndex(int)));
    QObject::connect(comboBox_2, SIGNAL(currentIndexChanged(int)), CAMultiplot, SLOT(penWidth(int)));
    QObject::connect(comboBox_3, SIGNAL(currentIndexChanged(QString)), CAMultiplot, SLOT(penColor(QString)));
    QObject::connect(comboBox_4, SIGNAL(currentIndexChanged(QString)), CAMultiplot, SLOT(penStyle(QString)));
    QObject::connect(doubleSpinBox, SIGNAL(valueChanged(QString)), CAMultiplot, SLOT(factor(QString)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue1(double)), label_2, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel1(QString)), label_4, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel(QString)), label_3, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel2(QString)), label_5, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel3(QString)), label_6, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel4(QString)), label_7, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(timeLabel5(QString)), label_8, SLOT(setText(QString)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue2(double)), label_9, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue3(double)), label_10, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue4(double)), label_11, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue6(double)), label_13, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue5(double)), label_12, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue7(double)), label_14, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue9(double)), label_16, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(currentValue8(double)), label_15, SLOT(setNum(double)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel(QString,QString)), CAColorCheckBox, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel1(QString,QString)), CAColorCheckBox_2, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel2(QString,QString)), CAColorCheckBox_3, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel3(QString,QString)), CAColorCheckBox_4, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel4(QString,QString)), CAColorCheckBox_5, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel5(QString,QString)), CAColorCheckBox_6, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel6(QString,QString)), CAColorCheckBox_7, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel7(QString,QString)), CAColorCheckBox_8, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel8(QString,QString)), CAColorCheckBox_9, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAMultiplot, SIGNAL(legendLabel9(QString,QString)), CAColorCheckBox_10, SLOT(changeColorLabel(QString,QString)));
    QObject::connect(CAColorCheckBox, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_2, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_3, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_4, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_5, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_6, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_7, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_8, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_9, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_10, SIGNAL(showLabel(bool,QString)), CAMultiplot, SLOT(displayCurve(bool,QString)));

    QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
    Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    CAMultiplot->setPvname(QApplication::translate("Form", "leesiHost_sinA", 0, QApplication::UnicodeUTF8));
    CAMultiplot->setPvname1(QApplication::translate("Form", "leesiHost_sinB", 0, QApplication::UnicodeUTF8));
    CAMultiplot->setPvname2(QApplication::translate("Form", "leesiHost_sinC", 0, QApplication::UnicodeUTF8));
    CAMultiplot->setPvname3(QString());
    CAMultiplot->setPvname4(QString());
    CAMultiplot->setPvname5(QString());
    CAMultiplot->setPvname6(QString());
    CAMultiplot->setPvname7(QString());
    CAMultiplot->setPvname8(QString());
    CAMultiplot->setPvname9(QString());
    CAMultiplot->setProperty("plotTitle", QVariant(QApplication::translate("Form", "Log Scale", 0, QApplication::UnicodeUTF8)));
    CAMultiplot->setProperty("xTitle", QVariant(QApplication::translate("Form", "Time Line", 0, QApplication::UnicodeUTF8)));
    CAMultiplot->setProperty("yTitle", QVariant(QApplication::translate("Form", "Y-Value (torr)", 0, QApplication::UnicodeUTF8)));
    comboBox_3->clear();
    comboBox_3->insertItems(0, QStringList()
     << QApplication::translate("Form", "Red", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Green", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Blue", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "#b3ff8a", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "#c4e2ff", 0, QApplication::UnicodeUTF8)
    );
    label->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_13->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_14->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    comboBox->clear();
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("Form", "leesiHost_sinA", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "leesiHost_sinB", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "leesiHost_sinC", 0, QApplication::UnicodeUTF8)
    );
    label_9->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    comboBox_4->clear();
    comboBox_4->insertItems(0, QStringList()
     << QApplication::translate("Form", "Steps", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Lines", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Dots", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Sticks", 0, QApplication::UnicodeUTF8)
    );
    label_11->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_12->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_10->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_15->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    comboBox_2->clear();
    comboBox_2->insertItems(0, QStringList()
     << QApplication::translate("Form", "1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "3", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "4", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "5", 0, QApplication::UnicodeUTF8)
    );
    label_2->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_16->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_6->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_8->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    label_7->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Form);
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

#endif // UI_CAMULTIPLOTTEST_H
