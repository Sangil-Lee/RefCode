#ifndef QUEUE_THREAD_H
#define QUEUE_THREAD_H

 #include <QThread>
 #include <QQueue>
 
 #include "global.h"

 class QueueThread : public QThread
 {
//     Q_OBJECT

 public:
//     QueueThread( const QQueue<ST_QUEUE_STATUS> &myqueue, QObject *parent = 0);
     QueueThread(  void *parent = 0);	 
     ~QueueThread();

     void run();
     
     void *pm;
     void putQueue(ST_QUEUE_STATUS);

//	QQueue<ST_QUEUE_STATUS> eventQ;
//	QString text;

 signals:

 private:
 	QQueue<ST_QUEUE_STATUS> eventQ;
 	QString text;

 };

 #endif


