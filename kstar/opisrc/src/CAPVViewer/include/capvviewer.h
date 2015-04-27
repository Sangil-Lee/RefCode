#ifndef __CAPVVIEWER_H
#define __CAPVVIEWER_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include "cadef.h"

using namespace std;

class CAPVViewer:public QWidget
{
	Q_OBJECT
public:
	CAPVViewer();
	virtual ~CAPVViewer();

public slots:
	void applyDispalyformat();

signals:

protected:
	void timerEvent(QTimerEvent *);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	void startStriptool();
	QString displayLegend(const int index, const char* svalue);

	//mbr variable
	const static int MAXALL=10;

	int			mpvcount;
	QUiLoader	m_loader;
	QWidget		*m_pwidget;
	QList<QLabel *> mall_label;
	QList<QLineEdit *> mall_ledit;
	QLineEdit *mleall[MAXALL];
	QLineEdit *mle1st[MAXALL];
	QLabel	*mlball[MAXALL];
	QLabel	*mlb1st[MAXALL];
	QLabel	*mlb1stunit[MAXALL];
	QTabWidget *mptab;
	QProcess	striptoolprc;
	QComboBox	*mcbLeindex;
	QRadioButton	*mrExp;
	QRadioButton	*mrNum;
	QRadioButton	*mrAuto;
	QSpinBox	*mspPrec;
	QPushButton *mbApply;
};
#endif
