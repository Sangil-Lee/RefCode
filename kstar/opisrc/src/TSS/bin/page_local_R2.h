#ifndef CLTU_TESTCAL_R2_H
#define CLTU_TESTCAL_R2_H

#include <QObject>
#include <QEvent>
#include <QMutex>
#include <QTableWidgetItem>


#include "global.h"
//#include "cadef.h"

#define STR_R2_FREE_RUN  "Stand-alone"
#define STR_R2_SYNC_RUN  "Release"


class LocalTestR2  :  public QObject
{
    Q_OBJECT

public:
	LocalTestR2();
	~LocalTestR2();
	void destroyPageTestcal_R2();

	void InitWindow( void *);


	void itemChanged_tw_local_R2(QTableWidgetItem *);
	

	void clicked_btn_showinfo_R2();
	void clicked_btn_showTime_R2();
	void clicked_btn_showTLK_R2();
	void clicked_btn_showLog_R2();
	void clicked_btn_test_setup_R2();
	void clicked_btn_test_shotStart_R2();
	void clicked_btn_test_shotStop_R2();
	void clicked_btn_R2_open();
	void clicked_btn_R2_save();

	void clicked_btn_R2_setMode();
	void clicked_btn_R2_insert_item();

	void stateChanged_check_calib_admin(int );
	void stateChanged_check_TSS_cal_mode(int);
	void clicked_btn_put_LTU_calib_value();

	void Init_OPI_widget();
	void event_removeCurrentRowItem();
	void caput_removeGivenRowItem(int curSys, int port);
	void cb_thread_calibration(ST_QUEUE_STATUS );

	
protected:
	
	void *pm;

	int nPVblockMode; 


private slots:
	

private:

};

#endif

