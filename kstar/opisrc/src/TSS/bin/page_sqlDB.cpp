#include <QtGui>

#include "page_sqlDB.h"
#include "mainWindow.h"



LTU_SQL_DB::LTU_SQL_DB()
{
	pm = NULL;
}

LTU_SQL_DB::~LTU_SQL_DB()
{

}

void LTU_SQL_DB::destroyPageSQL()
{
//	printf("destroy page_testcal\n");

}

void LTU_SQL_DB::InitWindow(void *parent)
{
	pm = (MainWindow*)parent;
	MainWindow *pWnd = (MainWindow*)pm;


	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_SYS, 30);  // enclosed
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_SYS, WIDTH_V2_SYS); // system
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_DESC, WIDTH_V2_DESC); // description
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_CH, WIDTH_V2_CH); // port
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_ONOFF, WIDTH_V2_ONOFF); // onoff
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_MODE, WIDTH_V2_MODE); // mode: Clock, Trigger
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_POL, WIDTH_V2_POL); // trig level
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_1_T0, WIDTH_V2_T0); // sec1 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_1_T1, WIDTH_V2_T1); // sec1 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_1_CLK, WIDTH_V2_CLK); // sec1 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_2_T0, WIDTH_V2_T0); // sec2 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_2_T1, WIDTH_V2_T1); // sec2 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_2_CLK, WIDTH_V2_CLK); // sec2 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_3_T0, WIDTH_V2_T0); // sec3 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_3_T1, WIDTH_V2_T1); // sec3 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_3_CLK, WIDTH_V2_CLK); // sec3 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_4_T0, WIDTH_V2_T0); // sec4 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_4_T1, WIDTH_V2_T1); // sec4 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_4_CLK, WIDTH_V2_CLK); // sec4 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_5_T0, WIDTH_V2_T0); // sec5 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_5_T1, WIDTH_V2_T1); // sec5 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_5_CLK, WIDTH_V2_CLK); // sec5 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_6_T0, WIDTH_V2_T0); // sec6 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_6_T1, WIDTH_V2_T1); // sec6 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_6_CLK, WIDTH_V2_CLK); // sec6 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_7_T0, WIDTH_V2_T0); // sec7 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_7_T1, WIDTH_V2_T1); // sec7 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_7_CLK, WIDTH_V2_CLK); // sec7 clock
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_8_T0, WIDTH_V2_T0); // sec8 start time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_8_T1, WIDTH_V2_T1); // sec8 stop time
	pWnd->tw_ltu_sql_db->horizontalHeader()->resizeSection(COL_VIEW_8_CLK, WIDTH_V2_CLK); // sec8 clock
	

}

void LTU_SQL_DB::Init_OPI_widget()
{
	MainWindow *pWnd = (MainWindow*)pm;




}





