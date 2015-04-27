#ifndef __SERVICECHECKER_H
#define __SERVICECHECKER_H
#include <QtGui>
#include <QUiLoader>
#include <QObject>
#include <vector>
#include <string>

//using std::string;
//using std::vector;

using namespace std;

class Servicechecker:public QWidget
{
	Q_OBJECT
public:
	Servicechecker();

public slots:
	void checkHost();
	void startService();

signals:

protected:

private:
	//mbr function
	void init();
	void getUiObject();
	void registerSignal();
	void parserMsg(string msg);
	void startChArchive();
	void startMdsplus();
	void startData();
	void startWeb();
	void startBackup();
	void startOPIs();
	void remoteStartCmd(const string &svrname, const string &svcname, const string &arglist);
	QComboBox* getCBObject(QString objname);

	//mbr variable
	QUiLoader m_loader;
	QWidget	*m_pwidget;
	QTableWidget *m_ptable;
	QLineEdit	*mleresult, *mleprechktime, *mlecurchktime;
	QList<QComboBox *> mallcbBox;
	vector<string> strhostlist;
	bool	bchk;
	//static const string muifile = "/usr/local/opi/ui/";
};
#endif
