#include <QtGui>

#include "queueThread.h"

#include "mainWindow.h"


QueueThread::QueueThread( void *parent)
{
	pm = (MainWindow*)parent;
}
/*
QueueThread::QueueThread(const QQueue<ST_QUEUE_STATUS> &myqueue, QObject *parent)
     : QThread(parent), eventQ(myqueue)
{
	pm = (MainWindow*)parent;
}
*/

QueueThread::~QueueThread()
{

}

void QueueThread::run()
{
	MainWindow *pWnd = (MainWindow *)pm;
	ST_QUEUE_STATUS queueNode;
	forever { 

		while( !eventQ.isEmpty() ) {
// 	   		printf("i'm in run thread function\n");
//   			cout << eventQ.dequeue() << endl;
			queueNode = eventQ.dequeue();

//			if( queueNode.id == QUEUE_THREAD_DDS2 ) {
//				printf("queueNode.status: %d\n", queueNode.status );
//				pWnd->setDDS2StateFromPV(queueNode.status);
//				}
//			else 
				pWnd->setDDS1StateFromPV(queueNode);
			
			usleep(10000);
    		}
	    	usleep(500000);
    }
    printf("out of thread function\n");

}

void QueueThread::putQueue(ST_QUEUE_STATUS val)	
{
//	ST_QUEUE_STATUS myval;
//	myval.id = val.id;
//	myval.status = val.status;
//	printf("befor queue \n");
	eventQ.enqueue(val);
//	printf("after queue \n");	
}

