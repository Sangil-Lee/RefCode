#ifndef _ARCHIVE_SHEET_H
#define _ARCHIVE_SHEET_H
#include <sys/types.h>
#include <sys/stat.h>
#include <QtGui>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <epicsTimeHelper.h>
#include "ArchiveDataClient.h"
#include "ArchiveThreadsController.h"

class ArchiveThreadsController;

class ArchiveSheetDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ArchiveSheetDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &,
        const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
        const QModelIndex &index) const;
private slots:
    void commitAndCloseEditor();
};

class ArchiveSheetItem : public QTableWidgetItem
{
public:
    ArchiveSheetItem();
    ArchiveSheetItem(const QString &text);

    QTableWidgetItem *clone() const;

    QVariant data(int role) const;
    void setData(int role, const QVariant &value);
    QVariant display() const;

    inline QString formula() const
        { return QTableWidgetItem::data(Qt::DisplayRole).toString(); }

    static QVariant computeFormula(const QString &formula,
                                   const QTableWidget *widget,
                                   const QTableWidgetItem *self = 0);

private:
    mutable bool isResolving;
};

class ArchiveSheet : public QMainWindow
{
    Q_OBJECT
public:
	static const int MAX_COUNT = 2000;
	enum A_TYPE{PLOT_TYPE=0, SHEET_TYPE=4};
    ArchiveSheet(int rows, int cols, QWidget *parent = 0);
	virtual ~ArchiveSheet();

	QTableWidget *getPvTableView() {return pvtableView;};
	QTableWidget *getTableView() {return table;};

	void setEpicsTimeStamp(const epicsTimeStamp &ts);
	bool getRowCol(){return b_rowcol;};
	void setRowCol(const bool rowcol){b_rowcol=rowcol;};
	//void setEpicTimes(const epicsTimeStamp &eptime);
	void setEpicTimes(QString &eptimestr);
	void setPvNames(const QString &pvname);
	void addPvValues(const QString &pvname, double &pvvalue);
	void updateDisplay();

	//for Test
	void TestPrint();
	void Test();
	const int Interval();

signals:
	void updateSignal();
	void nextPageSignal(int);
	void prevPageSignal(int);
		
public slots:
    void updateStatus(QTableWidgetItem *item);
    void updateColor(QTableWidgetItem *item);
    void updateLineEdit(QTableWidgetItem *item);
    void returnPressed();
    void selectColor();
    void selectFont();
    void clear();
    void showAbout();
    void getPVIndexes();
    void getArchiveValues();
    void updatePages(int index);
    void updateNextPage();
    void updatePrevPage();

    void actionSum();
    void actionSubtract();
    void actionAdd();
    void actionMultiply();
    void actionDivide();

    void viewPlot();

    void connWindow();
    void disconnWindow();
    void setupWindow();
    void openWindow();
    void saveWindow();
	void appendFile();
    void saveasWindow();

    void showPVList();
    void clearPVList();
	void addTblList();
	void delTblList();
	void updatePage();
	void setInterval(int interval);

protected:
    void setupContextMenu();
    void setupContents();

    void setupMenuBar();
    void createActions();
	bool connectToArchiver( const QString& url="http://nbti.nfrc.re.kr/archive/cgi/ArchiveDataServer.cgi");
	bool getPVNameList( const int archkey, const QString& pvname );

    void actionMath_helper(const QString &title, const QString &op);
    bool runInputDialog(const QString &title, const QString &c1Text, const QString &c2Text, const QString &opText,
                        const QString &outText, QString *cell1, QString *cell2, QString *outCell);
	void clearTable();
	void saveFile(const QString &filename);
	void openFile(QString &filename);
//	void fileToken( string &strtoken, int &row, int &col);
	void fileToken( string &strtoken, string &token, const char delim );

private:
    QToolBar *toolBar;
    QToolBar *archiveInfoBar;
    QAction *colorAction;
    QAction *fontAction;
    QAction *firstSeparator;
    QAction *cell_sumAction;
    QAction *cell_addAction;
    QAction *cell_subAction;
    QAction *cell_mulAction;
    QAction *cell_divAction;
    QAction *secondSeparator;
    QAction *view_Action;
    QAction *clearAction;
    QAction *aboutArchiveSheet;
    QAction *connAction;
    QAction *disconnAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *appendAction;
    QAction *saveasAction;
    QAction *setupAction;
    QAction *exitAction;
	QComboBox *m_arch_combo;
	QComboBox *ar_page;
    QTableWidget *table;
    ArchiveDataClient	m_arclient;
    stdVector<ArchiveDataClient::NameInfo> name_infos;
    stdVector<int> pvindexes;
	QStringList arch_list;
	QDateEdit *startdateEdit;
	QTimeEdit *starttimeEdit;
	QDateEdit *enddateEdit;
	QTimeEdit *endtimeEdit;
	QTimeEdit *ivaltimeEdit;
	QCheckBox *pivalcheck;
	QVector<double>qvecX, qvecY;
	QString strStartTime;
	QString strEndTime;
	QString	mfilepath;
	QLabel *startTime;
	QLabel *endTime;
	QLabel *curstartTime;
	QLabel *curendTime;
	QPushButton *nextButton;
	QPushButton *prevButton;
    stdVector<stdString> names;
    //QLabel *cellLabel;
    //QLineEdit *formulaInput;
	//setupWindow
    QLineEdit *pvNameLine;
    QListWidget *pvlistView;
    QTableWidget *pvtableView;
	ArchiveThreadsController *pthrcontroller;
	epicsTimeStamp	epicstime; 
	bool	b_rowcol;
	bool    isInitPage;
	double  m_pagetime;
	double  m_interval;
	int		m_setinterval;
	ArchiveThread *mpthr;
	QVector<QString> qvecEptime;
	QHash<QString, QVector<double> > qhashVal;
};
#endif
