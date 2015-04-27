#ifndef PV_LIST_VIWER_H
#define PV_LIST_VIWER_H
#include <QObject>
#include <QtGui>
#include <mysql++.h>
#include <cadef.h>
#include <dbDefs.h>
#include <kstardbconn.h>

using namespace std;

class PVListViewer:public QWidget
{
	Q_OBJECT
public:
	PVListViewer(QWidget *parent=NULL);
	~PVListViewer();

public slots:
	void Search();
	void SelectTableWidgetItem(int row, int);
	void NotConnPVList();
	void PVListCheck();

private:
	void makeUI();
	void saveFile(const QString &filename);

private:
	QComboBox *system_cb;
	QLineEdit *pvname_le;
	QLineEdit *pvdrag_le;
	QPushButton *search_pb;
    QPushButton *notconnlist_pb;
	QPushButton *pvcheck_pb;
	QPushButton *close_pb;
	QTableWidget *pvlist_tw;
    QLabel *listcnt;
	mysqlpp::Connection m_con;
	MySQLKSTARDB mysqlkstardb;
};
#endif
