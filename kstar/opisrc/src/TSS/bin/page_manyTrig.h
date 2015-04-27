#ifndef CLTU_MANY_TRIG_H
#define CLTU_MANY_TRIG_H

#include <QObject>
#include <QEvent>
#include <QMutex>
#include <QTableWidgetItem>

#include "epicsFunc.h"

#include "global.h"
//#include "cadef.h"



class LTU_MANY_TRIG  :  public QObject
{
    Q_OBJECT

public:
	LTU_MANY_TRIG();
	~LTU_MANY_TRIG();
	void destroyPageManyTrig();

	void InitWindow( void *);
	void Init_OPI_widget();


	void event_removeCurrentRowItem();
	void clicked_btn_mTrig_group_setup();
	void clicked_btn_mTrig_pv_setup();
	void clicked_btn_mTrig_list_clear();
	void clicked_btn_mTrig_Connect();
	void clicked_btn_mTrig_Disconnect();
	void clicked_btn_mTrig_start();
	void clicked_btn_mTrig_stop();
	
	
protected:
	
	void *pm;



private slots:
	

private:

};

#endif

