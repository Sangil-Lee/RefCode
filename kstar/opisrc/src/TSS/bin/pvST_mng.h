#ifndef WR_PVST_MNG_H
#define WR_PVST_MNG_H

 #include "global.h"

 class Cpvst_mng
 {
//     Q_OBJECT

 public:
//     CqueueThread( QObject *parent = 0);
	Cpvst_mng( void *parent = 0);

	~Cpvst_mng();


//	void putQueueNode(ST_QUEUE_STATUS);

//	void call_pg_setup_CLTU_with_all_list();
	void InitWindow( void *);
 

//	QQueue<ST_QUEUE_STATUS> eventQ;
//	QString text;

	void print_return_msg(ST_QUEUE_STATUS qnode);
	int check_is_first_event(ST_ARG_EVENT_CB *pEventArg);
	int get_pv_status_flag(ST_ARG_EVENT_CB *pEventArg);
	void set_pv_status_delete_clear(ST_ARG_EVENT_CB * pEventArg);

 

signals:
//	void temp_signla();

public slots:
	void temp_slot();

 private:
// 	QQueue<ST_QUEUE_STATUS> eventQ;

protected:
//	void run();

     void *proot;
		

 };

 #endif


