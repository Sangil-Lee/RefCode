/********************************************************************************
** Form generated from reading ui file 'SetProperty.ui'
**
** Created: Mon Aug 12 11:05:49 2013
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SETPROPERTY_H
#define UI_SETPROPERTY_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

class SetProperty_Form:public QWidget
{
	Q_OBJECT
public:
    QLabel *label;
    QLabel *label_2;
    QDoubleSpinBox *XMinSB;
    QLabel *label_3;
    QDoubleSpinBox *YMinSB;
    QLabel *label_4;
    QLabel *label_5;
    QDoubleSpinBox *YMaxSB;
    QDoubleSpinBox *XMaxSB;
    QLabel *label_6;
    QListWidget *WavePVList;
    QLabel *label_7;
    QSpinBox *LineWidthSB;
    QLabel *label_8;
    QLabel *label_9;
    QPushButton *WaveColorPB;
    QPushButton *ShowHidePB;
    QLabel *label_10;

	QWidget *pForm;

	SetProperty_Form(QWidget *pWidget = NULL):QWdiget(pWidget, Qt::WindowStayOnTopHint),singleton(1)
	{
		setupUi();
	}
	~SetProperty_Form()
	{
		singlton = 0;
		delete pForm;
	}

	void setupUi(QWidget *Form)
	{
		if (Form->objectName().isEmpty())
			Form->setObjectName(QString::fromUtf8("Form"));
		Form->resize(455, 419);
		label = new QLabel(Form);
		label->setObjectName(QString::fromUtf8("label"));
		label->setGeometry(QRect(10, 10, 71, 22));
		QFont font;
		font.setPointSize(12);
		label->setFont(font);
		label_2 = new QLabel(Form);
		label_2->setObjectName(QString::fromUtf8("label_2"));
		label_2->setGeometry(QRect(240, 10, 71, 22));
		label_2->setFont(font);
		XMinSB = new QDoubleSpinBox(Form);
		XMinSB->setObjectName(QString::fromUtf8("XMinSB"));
		XMinSB->setGeometry(QRect(50, 37, 62, 27));
		XMinSB->setMinimum(-30);
		XMinSB->setMaximum(100000);
		label_3 = new QLabel(Form);
		label_3->setObjectName(QString::fromUtf8("label_3"));
		label_3->setGeometry(QRect(28, 40, 20, 21));
		QFont font1;
		font1.setPointSize(10);
		label_3->setFont(font1);
		YMinSB = new QDoubleSpinBox(Form);
		YMinSB->setObjectName(QString::fromUtf8("YMinSB"));
		YMinSB->setGeometry(QRect(280, 37, 62, 27));
		YMinSB->setMinimum(-1e+06);
		YMinSB->setMaximum(1e+06);
		label_4 = new QLabel(Form);
		label_4->setObjectName(QString::fromUtf8("label_4"));
		label_4->setGeometry(QRect(255, 40, 21, 21));
		label_4->setFont(font1);
		label_5 = new QLabel(Form);
		label_5->setObjectName(QString::fromUtf8("label_5"));
		label_5->setGeometry(QRect(353, 40, 25, 21));
		label_5->setFont(font1);
		YMaxSB = new QDoubleSpinBox(Form);
		YMaxSB->setObjectName(QString::fromUtf8("YMaxSB"));
		YMaxSB->setGeometry(QRect(380, 37, 62, 27));
		YMaxSB->setMinimum(-1e+06);
		YMaxSB->setMaximum(1e+06);
		XMaxSB = new QDoubleSpinBox(Form);
		XMaxSB->setObjectName(QString::fromUtf8("XMaxSB"));
		XMaxSB->setGeometry(QRect(150, 37, 62, 27));
		XMaxSB->setMinimum(0);
		XMaxSB->setMaximum(100000);
		label_6 = new QLabel(Form);
		label_6->setObjectName(QString::fromUtf8("label_6"));
		label_6->setGeometry(QRect(124, 40, 25, 21));
		label_6->setFont(font1);
		WavePVList = new QListWidget(Form);
		WavePVList->setObjectName(QString::fromUtf8("WavePVList"));
		WavePVList->setGeometry(QRect(10, 120, 261, 291));
		QFont font2;
		font2.setPointSize(11);
		WavePVList->setFont(font2);
		label_7 = new QLabel(Form);
		label_7->setObjectName(QString::fromUtf8("label_7"));
		label_7->setGeometry(QRect(290, 130, 71, 21));
		label_7->setFont(font1);
		LineWidthSB = new QSpinBox(Form);
		LineWidthSB->setObjectName(QString::fromUtf8("LineWidthSB"));
		LineWidthSB->setGeometry(QRect(370, 130, 51, 27));
		LineWidthSB->setMinimum(1);
		LineWidthSB->setMaximum(10);
		label_8 = new QLabel(Form);
		label_8->setObjectName(QString::fromUtf8("label_8"));
		label_8->setGeometry(QRect(290, 185, 71, 21));
		label_8->setFont(font1);
		label_9 = new QLabel(Form);
		label_9->setObjectName(QString::fromUtf8("label_9"));
		label_9->setGeometry(QRect(290, 240, 71, 21));
		label_9->setFont(font1);
		WaveColorPB = new QPushButton(Form);
		WaveColorPB->setObjectName(QString::fromUtf8("WaveColorPB"));
		WaveColorPB->setGeometry(QRect(370, 185, 80, 27));
		ShowHidePB = new QPushButton(Form);
		ShowHidePB->setObjectName(QString::fromUtf8("ShowHidePB"));
		ShowHidePB->setGeometry(QRect(370, 240, 80, 27));
		ShowHidePB->setCheckable(true);
		ShowHidePB->setChecked(false);
		label_10 = new QLabel(Form);
		label_10->setObjectName(QString::fromUtf8("label_10"));
		label_10->setGeometry(QRect(10, 90, 111, 22));
		label_10->setFont(font);

		retranslateUi(Form);

		QMetaObject::connectSlotsByName(Form);
	} // setupUi

	void retranslateUi(QWidget *Form)
	{
		Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
		label->setText(QApplication::translate("Form", "X-Range", 0, QApplication::UnicodeUTF8));
		label_2->setText(QApplication::translate("Form", "Y-Range", 0, QApplication::UnicodeUTF8));
		label_3->setText(QApplication::translate("Form", "Min", 0, QApplication::UnicodeUTF8));
		label_4->setText(QApplication::translate("Form", "Min", 0, QApplication::UnicodeUTF8));
		label_5->setText(QApplication::translate("Form", "Max", 0, QApplication::UnicodeUTF8));
		label_6->setText(QApplication::translate("Form", "Max", 0, QApplication::UnicodeUTF8));
		label_7->setText(QApplication::translate("Form", "Line Width", 0, QApplication::UnicodeUTF8));
		label_8->setText(QApplication::translate("Form", "Line Color", 0, QApplication::UnicodeUTF8));
		label_9->setText(QApplication::translate("Form", "Hide/Show", 0, QApplication::UnicodeUTF8));
		WaveColorPB->setText(QApplication::translate("Form", "Color", 0, QApplication::UnicodeUTF8));
		ShowHidePB->setText(QApplication::translate("Form", "Show", 0, QApplication::UnicodeUTF8));
		label_10->setText(QApplication::translate("Form", "Wave PV List", 0, QApplication::UnicodeUTF8));
		Q_UNUSED(Form);
	} // retranslateUi

};

namespace WaveForm {
class SetProperty : public SetProperty_Form 
{
	Setproperty(QWidget *pWidget = NULL):SetProperty_Form(pWidget) {};
};
}; // namespace Ui

#endif // UI_SETPROPERTY_H
