#ifndef CALLBACK_FUNC_H
#define CALLBACK_FUNC_H

#include <QObject>
#include <QTableWidget>

#include "global.h"

class CBfunc : public QObject
{
    Q_OBJECT

public:
    CBfunc();
	 ~CBfunc();

	void InitWindow( void *);



	void startMonitoringPV(chid, int);

/*	void startMonitoringPV_DDS2(chid, int); */
	
	void stopMonitoringPV();

//	void patchMessage(chid *, unsigned int );
	


protected:
	void *proot;

	evid evidMain[MAX_EVENT_PV];
	int cntEvidMain;	

private slots:


private:

	
	

};

#endif

