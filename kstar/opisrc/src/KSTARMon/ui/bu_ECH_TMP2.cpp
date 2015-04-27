/********************************************************************************
** Form generated from reading ui file 'bu_ECH_TMP2.ui'
**
** Created: Mon Nov 10 13:58:58 2008
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_BU_ECH_TMP2_H
#define UI_BU_ECH_TMP2_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>
#include "caDisplayer.h"
#include "caGraphic.h"
#include "caPushbutton.h"

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    CAPushButton *ca11;
    CAPushButton *ca03;
    CAPushButton *ca02;
    CAGraphic *ca07;
    CAGraphic *ca10;
    CAGraphic *ca15;
    CAGraphic *ca13;
    CADisplayer *ca14;
    CAPushButton *ca12;
    CAGraphic *ca01;
    CAGraphic *ca04;
    CAPushButton *ca05;
    CAGraphic *ca08;
    CAPushButton *ca06;
    CAPushButton *ca17;
    CAPushButton *ca16;
    CADisplayer *ca09;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
    if (MainWindow->objectName().isEmpty())
        MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
    MainWindow->resize(350, 370);
    centralwidget = new QWidget(MainWindow);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    label = new QLabel(centralwidget);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(0, 0, 350, 350));
    label->setPixmap(QPixmap(QString::fromUtf8(":/images/ECH_TMP2.png")));
    label->setScaledContents(true);
    ca11 = new CAPushButton(centralwidget);
    ca11->setObjectName(QString::fromUtf8("ca11"));
    ca11->setGeometry(QRect(211, 164, 60, 22));
    QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ca11->sizePolicy().hasHeightForWidth());
    ca11->setSizePolicy(sizePolicy);
    ca11->setMinimumSize(QSize(0, 22));
    ca11->setMaximumSize(QSize(60, 22));
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    ca11->setFont(font);
    ca11->setButtonType(CAPushButton::TrueButton);
    ca11->setSelColor(QColor(0, 255, 0));
    ca11->setUnselColor(QColor(211, 197, 179));
    ca11->setSelTColor(QColor(60, 76, 100));
    ca11->setUnselTColor(QColor(172, 148, 113));
    ca03 = new CAPushButton(centralwidget);
    ca03->setObjectName(QString::fromUtf8("ca03"));
    ca03->setGeometry(QRect(58, 206, 60, 22));
    sizePolicy.setHeightForWidth(ca03->sizePolicy().hasHeightForWidth());
    ca03->setSizePolicy(sizePolicy);
    ca03->setMinimumSize(QSize(0, 22));
    ca03->setMaximumSize(QSize(60, 22));
    ca03->setFont(font);
    ca03->setButtonType(CAPushButton::TrueButton);
    ca03->setSelColor(QColor(0, 255, 0));
    ca03->setUnselColor(QColor(211, 197, 179));
    ca03->setSelTColor(QColor(60, 76, 100));
    ca03->setUnselTColor(QColor(172, 148, 113));
    ca02 = new CAPushButton(centralwidget);
    ca02->setObjectName(QString::fromUtf8("ca02"));
    ca02->setGeometry(QRect(58, 186, 60, 22));
    sizePolicy.setHeightForWidth(ca02->sizePolicy().hasHeightForWidth());
    ca02->setSizePolicy(sizePolicy);
    ca02->setMinimumSize(QSize(0, 22));
    ca02->setMaximumSize(QSize(60, 22));
    ca02->setFont(font);
    ca02->setButtonType(CAPushButton::TrueButton);
    ca02->setSelColor(QColor(0, 255, 0));
    ca02->setUnselColor(QColor(211, 197, 179));
    ca02->setSelTColor(QColor(60, 76, 100));
    ca02->setUnselTColor(QColor(172, 148, 113));
    ca07 = new CAGraphic(centralwidget);
    ca07->setObjectName(QString::fromUtf8("ca07"));
    ca07->setGeometry(QRect(60, 282, 15, 15));
    ca07->setMaximumSize(QSize(15, 15));
    ca07->setGeometryType(StaticGraphic::Ellipse);
    ca07->setLineColor(QColor(60, 76, 100));
    ca07->setLineWidth(1);
    ca07->setPopup(true);
    ca07->setPvs(QStringList() << "ECH_RD26" << "ECH_RE4F" << "ECH_RE0F");
    ca10 = new CAGraphic(centralwidget);
    ca10->setObjectName(QString::fromUtf8("ca10"));
    ca10->setGeometry(QRect(195, 173, 15, 25));
    ca10->setMaximumSize(QSize(15, 25));
    ca10->setGeometryType(StaticGraphic::VSValve);
    ca10->setLineColor(QColor(60, 76, 100));
    ca10->setLineWidth(1);
    ca15 = new CAGraphic(centralwidget);
    ca15->setObjectName(QString::fromUtf8("ca15"));
    ca15->setGeometry(QRect(33, 304, 25, 25));
    ca15->setMaximumSize(QSize(25, 25));
    ca15->setGeometryType(StaticGraphic::DP);
    ca15->setLineColor(QColor(60, 76, 100));
    ca15->setLineWidth(1);
    ca13 = new CAGraphic(centralwidget);
    ca13->setObjectName(QString::fromUtf8("ca13"));
    ca13->setGeometry(QRect(291, 191, 22, 22));
    ca13->setMinimumSize(QSize(20, 20));
    ca13->setMaximumSize(QSize(22, 22));
    ca13->setGeometryType(StaticGraphic::VacuumGauge);
    ca13->setFillMode(StaticGraphic::Solid);
    ca13->setLineColor(QColor(60, 76, 100));
    ca13->setLineWidth(1);
    ca13->setPopup(false);
    ca14 = new CADisplayer(centralwidget);
    ca14->setObjectName(QString::fromUtf8("ca14"));
    ca14->setGeometry(QRect(262, 214, 80, 20));
    ca14->setMinimumSize(QSize(20, 20));
    ca14->setMaximumSize(QSize(80, 20));
    ca14->setFont(font);
    ca14->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    ca14->setProperty("lineWidth", QVariant(1));
    ca14->setProperty("framecol", QVariant(QColor(60, 76, 100)));
    ca12 = new CAPushButton(centralwidget);
    ca12->setObjectName(QString::fromUtf8("ca12"));
    ca12->setGeometry(QRect(211, 184, 60, 22));
    sizePolicy.setHeightForWidth(ca12->sizePolicy().hasHeightForWidth());
    ca12->setSizePolicy(sizePolicy);
    ca12->setMinimumSize(QSize(0, 22));
    ca12->setMaximumSize(QSize(60, 22));
    ca12->setFont(font);
    ca12->setButtonType(CAPushButton::TrueButton);
    ca12->setSelColor(QColor(0, 255, 0));
    ca12->setUnselColor(QColor(211, 197, 179));
    ca12->setSelTColor(QColor(60, 76, 100));
    ca12->setUnselTColor(QColor(172, 148, 113));
    ca01 = new CAGraphic(centralwidget);
    ca01->setObjectName(QString::fromUtf8("ca01"));
    ca01->setGeometry(QRect(32, 200, 25, 15));
    ca01->setMaximumSize(QSize(25, 15));
    ca01->setGeometryType(StaticGraphic::HSValve);
    ca01->setLineColor(QColor(60, 76, 100));
    ca01->setLineWidth(1);
    ca04 = new CAGraphic(centralwidget);
    ca04->setObjectName(QString::fromUtf8("ca04"));
    ca04->setGeometry(QRect(33, 249, 25, 25));
    ca04->setMaximumSize(QSize(25, 25));
    ca04->setGeometryType(StaticGraphic::TMP);
    ca04->setLineColor(QColor(60, 76, 100));
    ca04->setLineWidth(1);
    ca04->setPopup(false);
    ca04->setPvs(QStringList());
    ca05 = new CAPushButton(centralwidget);
    ca05->setObjectName(QString::fromUtf8("ca05"));
    ca05->setGeometry(QRect(58, 240, 60, 22));
    sizePolicy.setHeightForWidth(ca05->sizePolicy().hasHeightForWidth());
    ca05->setSizePolicy(sizePolicy);
    ca05->setMinimumSize(QSize(0, 22));
    ca05->setMaximumSize(QSize(60, 22));
    ca05->setFont(font);
    ca05->setButtonType(CAPushButton::TrueButton);
    ca05->setSelColor(QColor(0, 255, 0));
    ca05->setUnselColor(QColor(211, 197, 179));
    ca05->setSelTColor(QColor(60, 76, 100));
    ca05->setUnselTColor(QColor(172, 148, 113));
    ca08 = new CAGraphic(centralwidget);
    ca08->setObjectName(QString::fromUtf8("ca08"));
    ca08->setGeometry(QRect(36, 34, 22, 22));
    ca08->setMinimumSize(QSize(20, 20));
    ca08->setMaximumSize(QSize(22, 22));
    ca08->setGeometryType(StaticGraphic::VacuumGauge);
    ca08->setFillMode(StaticGraphic::Solid);
    ca08->setLineColor(QColor(60, 76, 100));
    ca08->setLineWidth(1);
    ca08->setPopup(false);
    ca06 = new CAPushButton(centralwidget);
    ca06->setObjectName(QString::fromUtf8("ca06"));
    ca06->setGeometry(QRect(58, 260, 60, 22));
    sizePolicy.setHeightForWidth(ca06->sizePolicy().hasHeightForWidth());
    ca06->setSizePolicy(sizePolicy);
    ca06->setMinimumSize(QSize(0, 22));
    ca06->setMaximumSize(QSize(60, 22));
    ca06->setFont(font);
    ca06->setButtonType(CAPushButton::TrueButton);
    ca06->setSelColor(QColor(0, 255, 0));
    ca06->setUnselColor(QColor(211, 197, 179));
    ca06->setSelTColor(QColor(60, 76, 100));
    ca06->setUnselTColor(QColor(172, 148, 113));
    ca17 = new CAPushButton(centralwidget);
    ca17->setObjectName(QString::fromUtf8("ca17"));
    ca17->setGeometry(QRect(153, 259, 70, 22));
    sizePolicy.setHeightForWidth(ca17->sizePolicy().hasHeightForWidth());
    ca17->setSizePolicy(sizePolicy);
    ca17->setMinimumSize(QSize(0, 22));
    ca17->setMaximumSize(QSize(70, 22));
    ca17->setFont(font);
    ca17->setButtonType(CAPushButton::TrueButton);
    ca17->setSelColor(QColor(0, 255, 0));
    ca17->setUnselColor(QColor(211, 197, 179));
    ca17->setSelTColor(QColor(60, 76, 100));
    ca17->setUnselTColor(QColor(172, 148, 113));
    ca16 = new CAPushButton(centralwidget);
    ca16->setObjectName(QString::fromUtf8("ca16"));
    ca16->setGeometry(QRect(153, 239, 70, 22));
    sizePolicy.setHeightForWidth(ca16->sizePolicy().hasHeightForWidth());
    ca16->setSizePolicy(sizePolicy);
    ca16->setMinimumSize(QSize(0, 22));
    ca16->setMaximumSize(QSize(70, 22));
    ca16->setFont(font);
    ca16->setButtonType(CAPushButton::TrueButton);
    ca16->setSelColor(QColor(0, 255, 0));
    ca16->setUnselColor(QColor(211, 197, 179));
    ca16->setSelTColor(QColor(60, 76, 100));
    ca16->setUnselTColor(QColor(172, 148, 113));
    ca09 = new CADisplayer(centralwidget);
    ca09->setObjectName(QString::fromUtf8("ca09"));
    ca09->setGeometry(QRect(59, 35, 80, 20));
    ca09->setMinimumSize(QSize(20, 20));
    ca09->setMaximumSize(QSize(80, 20));
    ca09->setFont(font);
    ca09->setProperty("nuformat", QVariant(CADisplayer::Pressure));
    ca09->setProperty("lineWidth", QVariant(1));
    ca09->setProperty("framecol", QVariant(QColor(60, 76, 100)));
    MainWindow->setCentralWidget(centralwidget);
    statusbar = new QStatusBar(MainWindow);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    MainWindow->setStatusBar(statusbar);

    retranslateUi(MainWindow);

    QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
    MainWindow->setWindowTitle(QApplication::translate("MainWindow", "TMP2 Control Panel", 0, QApplication::UnicodeUTF8));
    label->setText(QString());
    ca11->setPvname(QApplication::translate("MainWindow", "ECH_RE54", 0, QApplication::UnicodeUTF8));
    ca11->setTruelabel(QApplication::translate("MainWindow", "CLOSE", 0, QApplication::UnicodeUTF8));
    ca11->setFalselabel(QString());
    ca03->setPvname(QApplication::translate("MainWindow", "ECH_RE0F", 0, QApplication::UnicodeUTF8));
    ca03->setTruelabel(QApplication::translate("MainWindow", "OPEN", 0, QApplication::UnicodeUTF8));
    ca03->setFalselabel(QString());
    ca02->setPvname(QApplication::translate("MainWindow", "ECH_RE4F", 0, QApplication::UnicodeUTF8));
    ca02->setTruelabel(QApplication::translate("MainWindow", "CLOSE", 0, QApplication::UnicodeUTF8));
    ca02->setFalselabel(QString());
    ca07->setPvname(QApplication::translate("MainWindow", "ECH_RD4C", 0, QApplication::UnicodeUTF8));
    ca07->setFilename(QApplication::translate("MainWindow", "/usr/local/opi/ui/ECH_TMP.ui", 0, QApplication::UnicodeUTF8));
    ca10->setPvname(QApplication::translate("MainWindow", "ECH_RD26", 0, QApplication::UnicodeUTF8));
    ca15->setPvname(QApplication::translate("MainWindow", "ECH_RD46", 0, QApplication::UnicodeUTF8));
    ca13->setPvname(QApplication::translate("MainWindow", "ECH_RD2C", 0, QApplication::UnicodeUTF8));
    ca14->setPvname(QApplication::translate("MainWindow", "ECH_TPG256_P1", 0, QApplication::UnicodeUTF8));
    ca14->setUnit(QString());
    ca12->setPvname(QApplication::translate("MainWindow", "ECH_RE14", 0, QApplication::UnicodeUTF8));
    ca12->setTruelabel(QApplication::translate("MainWindow", "OPEN", 0, QApplication::UnicodeUTF8));
    ca12->setFalselabel(QString());
    ca01->setPvname(QApplication::translate("MainWindow", "ECH_RD2A", 0, QApplication::UnicodeUTF8));
    ca04->setPvname(QApplication::translate("MainWindow", "ECH_RD46", 0, QApplication::UnicodeUTF8));
    ca04->setFilename(QApplication::translate("MainWindow", "/usr/local/opi/ui/ECH_TMP.ui", 0, QApplication::UnicodeUTF8));
    ca05->setPvname(QApplication::translate("MainWindow", "ECH_RE5C", 0, QApplication::UnicodeUTF8));
    ca05->setTruelabel(QApplication::translate("MainWindow", "STOP", 0, QApplication::UnicodeUTF8));
    ca05->setFalselabel(QString());
    ca08->setPvname(QApplication::translate("MainWindow", "ECH_RD2B", 0, QApplication::UnicodeUTF8));
    ca06->setPvname(QApplication::translate("MainWindow", "ECH_RE1C", 0, QApplication::UnicodeUTF8));
    ca06->setTruelabel(QApplication::translate("MainWindow", "START", 0, QApplication::UnicodeUTF8));
    ca06->setFalselabel(QString());
    ca17->setPvname(QApplication::translate("MainWindow", "ECH_RE1C", 0, QApplication::UnicodeUTF8));
    ca17->setTruelabel(QApplication::translate("MainWindow", "REMOTE", 0, QApplication::UnicodeUTF8));
    ca17->setFalselabel(QString());
    ca16->setPvname(QApplication::translate("MainWindow", "ECH_RE5C", 0, QApplication::UnicodeUTF8));
    ca16->setTruelabel(QApplication::translate("MainWindow", "LOCAL", 0, QApplication::UnicodeUTF8));
    ca16->setFalselabel(QString());
    ca09->setPvname(QApplication::translate("MainWindow", "ECH_TPG256_P1", 0, QApplication::UnicodeUTF8));
    ca09->setUnit(QString());
    Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

#endif // UI_BU_ECH_TMP2_H
