#ifndef FILE_CONTROL_H
#define FILE_CONTROL_H

#include <QObject>
#include <QTableWidget>

#include "global.h"

class FileCTRL : public QObject
{
    Q_OBJECT

public:
    FileCTRL();
	 ~FileCTRL();

	void InitWindow( void *);

	void saveACQ196ConfigFile(QTableWidget *pwidget, int num);
//	void saveM6802ConfigFile(QTableWidget *pwidget, int num);

protected:
	void *proot;
	

private slots:


private:

	
	

};

#endif

