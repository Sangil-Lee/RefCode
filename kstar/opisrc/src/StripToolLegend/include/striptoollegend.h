#ifndef __STRIPTOOLLEGEND_H
#define __STRIPTOOLLEGEND_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>
#include <mqueue.h>
#include <sys/stat.h>

//using std::string;
//using std::vector;
using namespace std;

typedef struct STRIPINFO_S
{
	char pvname[64];
	char svalue[32];
	char egu[12];
	int  index;
	unsigned int r;
	unsigned int g;
	unsigned int b;
	unsigned int pvbit;
	//double dvalue;
	//unsigned int pixel;
}STRIPINFO;

typedef struct INDEXINFO_S
{
	int type;	//0=Exponential, 1=Numeric
	int precision;
}INDEXINFO;

class StripToolLegend:public QWidget
{
	Q_OBJECT
public:
	StripToolLegend();
	virtual ~StripToolLegend();

public slots:
	//void getValue();
	void applyDispalyformat();

signals:

protected:
	const string msgqname;
	void timerEvent(QTimerEvent *);

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	void startStriptool();
	void bitOR ( unsigned int &src, unsigned int org );
	void bitAND ( unsigned int &src, unsigned int org );
	void bitRSet ( unsigned int &src, const int pos, int state );
	void bitLSet ( unsigned int &src, const int pos, int state );
	int bitLGet(unsigned int &src, const int pos);
	int bitRGet(unsigned int &src, const int pos);
	void print_binary ( unsigned int u );
	int bitCount(unsigned int &src);
	QString displayLegend(const int index, const char* svalue);

	//mbr variable
	const static int maxmsgsize=100000;
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
	mqd_t m_msgqID;
    STRIPINFO  stripinfo;
	INDEXINFO  indexinfo;
	struct mq_attr m_msgqAttr;
	QProcess	striptoolprc;
	QComboBox	*mcbLeindex;
	QRadioButton	*mrExp;
	QRadioButton	*mrNum;
	QRadioButton	*mrAuto;
	QSpinBox	*mspPrec;
	QPushButton *mbApply;
	vector<INDEXINFO> vecIndex;
	//vector<STRIPINFO> vecStrip;
};
#endif
