#ifndef CLTU_SQL_DB_H
#define CLTU_SQL_DB_H

#include <QObject>
#include <QEvent>
#include <QMutex>
#include <QTableWidgetItem>


#include "global.h"
//#include "cadef.h"

#define COL_VIEW_ENCLOSED	0
#define COL_VIEW_SYS	1
#define COL_VIEW_DESC	2
#define COL_VIEW_CH	 	3
 
#define COL_VIEW_ONOFF	4
#define COL_VIEW_MODE	5
#define COL_VIEW_POL		6
#define COL_VIEW_1_T0	7
#define COL_VIEW_1_T1	8
#define COL_VIEW_1_CLK	9
#define COL_VIEW_2_T0	10
#define COL_VIEW_2_T1	11
#define COL_VIEW_2_CLK	12
#define COL_VIEW_3_T0	13
#define COL_VIEW_3_T1	14
#define COL_VIEW_3_CLK	15
#define COL_VIEW_4_T0	16
#define COL_VIEW_4_T1	17
#define COL_VIEW_4_CLK	18
#define COL_VIEW_5_T0	19
#define COL_VIEW_5_T1	20
#define COL_VIEW_5_CLK	21
#define COL_VIEW_6_T0	22
#define COL_VIEW_6_T1	23
#define COL_VIEW_6_CLK	24
#define COL_VIEW_7_T0	25
#define COL_VIEW_7_T1	26
#define COL_VIEW_7_CLK	27
#define COL_VIEW_8_T0	28
#define COL_VIEW_8_T1	29
#define COL_VIEW_8_CLK	30


class LTU_SQL_DB  :  public QObject
{
    Q_OBJECT

public:
	LTU_SQL_DB();
	~LTU_SQL_DB();
	void destroyPageSQL();

	void InitWindow( void *);
	void Init_OPI_widget();


	
	
protected:
	
	void *pm;



private slots:
	

private:

};

#endif

