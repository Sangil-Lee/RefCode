/********************************************************************************
** Form generated from reading ui file 'CAWaveplotTest.ui'
**
** Created: Tue Feb 10 14:25:20 2009
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_CAWAVEPLOTTEST_H
#define UI_CAWAVEPLOTTEST_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include "caColorcheckbox.h"
#include "caMultiwaveplot.h"

class Ui_Form
{
public:
    CAColorCheckBox *CAColorCheckBox_2;
    QComboBox *comboBox_3;
    QLabel *label;
    CAColorCheckBox *CAColorCheckBox;
    CAColorCheckBox *CAColorCheckBox_3;
    QComboBox *comboBox;
    QLabel *label_9;
    QComboBox *comboBox_4;
    QComboBox *comboBox_2;
    QLabel *label_2;
    QDoubleSpinBox *doubleSpinBox;
    CAMultiwaveplot *CAMultiwaveplot;

    void setupUi(QWidget *Form)
    {
    if (Form->objectName().isEmpty())
        Form->setObjectName(QString::fromUtf8("Form"));
    Form->resize(1012, 749);
    CAColorCheckBox_2 = new CAColorCheckBox(Form);
    CAColorCheckBox_2->setObjectName(QString::fromUtf8("CAColorCheckBox_2"));
    CAColorCheckBox_2->setGeometry(QRect(780, 170, 139, 22));
    comboBox_3 = new QComboBox(Form);
    comboBox_3->setObjectName(QString::fromUtf8("comboBox_3"));
    comboBox_3->setGeometry(QRect(400, 40, 72, 25));
    label = new QLabel(Form);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(894, 140, 79, 22));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    CAColorCheckBox = new CAColorCheckBox(Form);
    CAColorCheckBox->setObjectName(QString::fromUtf8("CAColorCheckBox"));
    CAColorCheckBox->setGeometry(QRect(780, 140, 139, 22));
    CAColorCheckBox_3 = new CAColorCheckBox(Form);
    CAColorCheckBox_3->setObjectName(QString::fromUtf8("CAColorCheckBox_3"));
    CAColorCheckBox_3->setGeometry(QRect(780, 200, 139, 22));
    comboBox = new QComboBox(Form);
    comboBox->setObjectName(QString::fromUtf8("comboBox"));
    comboBox->setGeometry(QRect(140, 40, 165, 25));
    label_9 = new QLabel(Form);
    label_9->setObjectName(QString::fromUtf8("label_9"));
    label_9->setGeometry(QRect(894, 200, 79, 22));
    label_9->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    comboBox_4 = new QComboBox(Form);
    comboBox_4->setObjectName(QString::fromUtf8("comboBox_4"));
    comboBox_4->setGeometry(QRect(470, 40, 72, 25));
    comboBox_2 = new QComboBox(Form);
    comboBox_2->setObjectName(QString::fromUtf8("comboBox_2"));
    comboBox_2->setGeometry(QRect(330, 40, 72, 25));
    label_2 = new QLabel(Form);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(894, 170, 79, 22));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
    doubleSpinBox = new QDoubleSpinBox(Form);
    doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
    doubleSpinBox->setGeometry(QRect(560, 40, 101, 25));
    doubleSpinBox->setMaximum(1e+08);
    CAMultiwaveplot = new CAMultiwaveplot(Form);
    CAMultiwaveplot->setObjectName(QString::fromUtf8("CAMultiwaveplot"));
    CAMultiwaveplot->setGeometry(QRect(20, 110, 761, 551));
    CAMultiwaveplot->setProperty("canvasColor", QVariant(QColor(249, 249, 255)));
    CAMultiwaveplot->setProperty("majLinecolor", QVariant(QColor(255, 0, 255)));
    CAMultiwaveplot->setProperty("reqElement", QVariant(512));
    CAMultiwaveplot->setUpdateTime(100);
    CAMultiwaveplot->setRefreshTime(100);
    CAMultiwaveplot->setBufferPage(1);

    retranslateUi(Form);
    QObject::connect(comboBox, SIGNAL(currentIndexChanged(int)), CAMultiwaveplot, SLOT(penIndex(int)));
    QObject::connect(comboBox_2, SIGNAL(currentIndexChanged(int)), CAMultiwaveplot, SLOT(penWidth(int)));
    QObject::connect(comboBox_3, SIGNAL(currentIndexChanged(QString)), CAMultiwaveplot, SLOT(penColor(QString)));
    QObject::connect(comboBox_4, SIGNAL(currentIndexChanged(QString)), CAMultiwaveplot, SLOT(penStyle(QString)));
    QObject::connect(CAColorCheckBox, SIGNAL(showLabel(bool,QString)), CAMultiwaveplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_2, SIGNAL(showLabel(bool,QString)), CAMultiwaveplot, SLOT(displayCurve(bool,QString)));
    QObject::connect(CAColorCheckBox_3, SIGNAL(showLabel(bool,QString)), CAMultiwaveplot, SLOT(displayCurve(bool,QString)));

    QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
    Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    CAColorCheckBox_2->setText(QApplication::translate("Form", "leesiHost_sinB_WF", 0, QApplication::UnicodeUTF8));
    comboBox_3->clear();
    comboBox_3->insertItems(0, QStringList()
     << QApplication::translate("Form", "Red", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Green", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Blue", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "#b3ff8a", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "#c4e2ff", 0, QApplication::UnicodeUTF8)
    );
    label->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    CAColorCheckBox->setText(QApplication::translate("Form", "leesiHost_sinA_WF", 0, QApplication::UnicodeUTF8));
    CAColorCheckBox_3->setText(QApplication::translate("Form", "leesiHost_sinC_WF", 0, QApplication::UnicodeUTF8));
    comboBox->clear();
    comboBox->insertItems(0, QStringList()
     << QApplication::translate("Form", "leesiHost_sinA_WF", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "leesiHost_sinB_WF", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "leesiHost_sinC_WF", 0, QApplication::UnicodeUTF8)
    );
    label_9->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    comboBox_4->clear();
    comboBox_4->insertItems(0, QStringList()
     << QApplication::translate("Form", "Steps", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Lines", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Dots", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "Sticks", 0, QApplication::UnicodeUTF8)
    );
    comboBox_2->clear();
    comboBox_2->insertItems(0, QStringList()
     << QApplication::translate("Form", "1", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "2", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "3", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "4", 0, QApplication::UnicodeUTF8)
     << QApplication::translate("Form", "5", 0, QApplication::UnicodeUTF8)
    );
    label_2->setText(QApplication::translate("Form", "TextLabel", 0, QApplication::UnicodeUTF8));
    CAMultiwaveplot->setPvname(QApplication::translate("Form", "leesiHost_sinA_WF", 0, QApplication::UnicodeUTF8));
    CAMultiwaveplot->setPvname1(QApplication::translate("Form", "leesiHost_sinB_WF", 0, QApplication::UnicodeUTF8));
    CAMultiwaveplot->setPvname2(QApplication::translate("Form", "leesiHost_sinC_WF", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(Form);
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

#endif // UI_CAWAVEPLOTTEST_H
