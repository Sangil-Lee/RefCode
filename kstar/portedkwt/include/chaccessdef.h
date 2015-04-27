#ifndef __CHACCESS_DEF_
#define __CHACCESS_DEF_
#include "cadef.h"
#include <QtGui>
#include <QTableWidgetItem>

using std::vector;
using std::string;

//typedef struct ChAccess
struct ChAccess
{
	unsigned int chindex;
	string		objname;
	string		pvname;
	chid		ch_id;
	double		pvvalue;
	/* shbaek added strvalue */
	QString		strvalue;
	QWidget		*pobj;
	QTableWidgetItem *pItem;
	short		prec;
	short		severity;
	short		connstatus;
	int         dbrequest;
};
typedef vector < ChAccess > VecChacc;

#endif
