#include <QtGui>

#include "queueThread.h"

#include "mainWindow.h"



CqueueThread::CqueueThread( void *parent)
{
	proot = (MainWindow*)parent;

}

/*
CqueueThread::CqueueThread( QObject *parent)
     : QThread(parent)
{
	proot = (MainWindow*)parent;
}
*/
CqueueThread::~CqueueThread()
{

}
#if 1
void CqueueThread::run()
{
	ST_QUEUE_STATUS queueNode;
	MainWindow *pMain = (MainWindow *)proot;
	
	forever { 

		while( !eventQ.isEmpty() ) {
//   			cout << eventQ.dequeue() << endl;
			queueNode = eventQ.dequeue();

			if( queueNode.caller == QUEUE_ID_MAINWND) {
/*				QMessageBox::warning(0, "Kstar TSS", queueNode.buf ); */
				pMain->pg_main.cb_thread_updateMainStack( queueNode );
			} 
			else if( queueNode.caller == QUEUE_ID_TIMESET) {
//				pMain->pg_timeset.cb_thread_updateTimingTable( queueNode ); // 2012. 2. 14
//				usleep(170000);
//				printf("in QUEUE_ID_TIMESET\n");
			}
			else if (queueNode.caller == QUEUE_ID_CALIBRATION) {
				pMain->pg_localR2.cb_thread_calibration( queueNode );
			}
			else if( queueNode.caller == QUEUE_ID_SET_ZERO ) {
				printf("in QUEUE_ID_SET_ZERO\n");
//				pMain->pg_timeset.caput_reset_all_sys();
			}
			else if( queueNode.caller == QUEUE_ID_SETUP_ALL_LIST) {
				printf("in QUEUE_ID_SETUP_ALL_LIST\n");
//				pMain->pg_timeset.pg_setup_CLTU_with_all_list();
				printf("out out call\n");
			}

//			pMain->setDDS1StateFromPV(queueNode);
//			usleep(300000);
    		}
//		printf("i'm in run thread function\n");
	    	usleep(1000);
    }
    printf("out of thread function\n");

}
#endif



void CqueueThread::putQueueNode(ST_QUEUE_STATUS val)	
{
	eventQ.enqueue(val);
}

void CqueueThread::temp_slot()
{
	printf("this is temp_slot() \n");
}
/*
void CqueueThread::call_pg_setup_CLTU_with_all_list()
{
	MainWindow *pMain = (MainWindow *)proot;

	printf("in QUEUE_ID_SETUP_ALL_LIST\n");
//	pMain->pg_timeset.pg_setup_CLTU_with_all_list();
	printf("out out call\n");
}
*/

