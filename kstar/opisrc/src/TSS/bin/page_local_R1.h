#ifndef CLTU_TESTCAL_H
#define CLTU_TESTCAL_H

#include <QObject>
#include <QEvent>
#include <QMutex>

#include "global.h"
//#include "cadef.h"

class LocalTestR1  :  public QObject
{
    Q_OBJECT

public:
	LocalTestR1();
	~LocalTestR1();
	void destroyPageTestcal();

	void InitWindow( void *);

	void setup_single_port(int sysID,int port);
	int setup_all_port(int sysID, ST_CLTU_config cfg_CLTU);
	
	void clicked_btn_test_set();
	void clicked_btn_test_shotStart();

	void clicked_btn_cal_calStart();

	
protected:
	void Initialize();
	
	void *pm;


private slots:
	

private:

};

#endif

