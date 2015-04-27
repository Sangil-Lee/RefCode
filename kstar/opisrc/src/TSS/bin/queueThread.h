#ifndef WR_QUEUE_THREAD_H
#define WR_QUEUE_THREAD_H

 #include <QThread>
 #include <QQueue>
 
 #include "global.h"


 class CqueueThread : public QThread
 {
//     Q_OBJECT

 public:
//     CqueueThread( QObject *parent = 0);
	CqueueThread( void *parent = 0);

     ~CqueueThread();


	void putQueueNode(ST_QUEUE_STATUS);

//	void call_pg_setup_CLTU_with_all_list();

	 

//	QQueue<ST_QUEUE_STATUS> eventQ;
//	QString text;
 

signals:
//	void temp_signla();

public slots:
	void temp_slot();

 private:
 	QQueue<ST_QUEUE_STATUS> eventQ;

protected:
	void run();

     void *proot;
		

 };

 #endif


