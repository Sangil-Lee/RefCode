/********************************************************************************
** Form generated from reading ui file 'KSTARMon.ui'
**
** Created: Fri Jul 1 16:30:44 2011
**      by: Qt User Interface Compiler version 4.3.2
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_KSTARMON_H
#define UI_KSTARMON_H

#include <QtGui>
#include <QUiLoader>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

class KSTARMonForm:public QWidget
{
	Q_OBJECT
public:
    QLabel *label;
    QComboBox *systemCB;
    //QDialogButtonBox *closeBT;
    QPushButton *closeBT;
    QPushButton *showPagePB;
    QListWidget *pagesLV;
	QWidget *pForm;
	QStringList tmspages;
	QStringList vmspages;
	QStringList hdspages;
	QStringList hdsvibpages;
	QStringList echpages;
	QStringList fuelpages;
	QStringList clspages;
	QStringList icrhpages;
	QStringList gcdspages;
	QStringList pmspages;
	QStringList mpspages;
	QStringList icppages;
	QStringList nb1calpages;
	QStringList nb1vacpages;
	QStringList scspages;

	KSTARMonForm(QWidget *pWidget);
	virtual ~KSTARMonForm();

public slots:
	void updateListSlot(int index);
	void showPageSlot();
	void showPageByWidgetDoubleclick( QListWidgetItem * item );

private:
	enum SYSTEM {TMS=0,VMS,HDS,ECH,Fuel,CLS,ICRH,ICP,GCDS,PMS,MPS,NB1_CAL,NB1_VAC,SCS,HDS_VIB};
	void connectSignal();
    void setupUi(QWidget *Form);
	void retranslateUi(QWidget *Form);
	int showPage(const int system, const int pageindex);
	QString openPage(const int system, const int pageindex);
};

namespace KSTARMON_UI {
    class KSTARMon: public KSTARMonForm 
	{
		public:
			KSTARMon(QWidget *pWidget):KSTARMonForm(pWidget){};
	};
} // namespace Ui

#endif // UI_KSTARMON_H
