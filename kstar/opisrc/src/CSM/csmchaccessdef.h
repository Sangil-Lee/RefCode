#ifndef __CSM_CHACCESS_DEF_
#define __CSM_CHACCESS_DEF_
#include "cadef.h"

using std::vector;
using std::string;

typedef struct ChAccess
{
	string			objname;
	string			pvname;
	chid			ch_id;
	double			pvvalue;
	QString			strvalue;
	QWidget			*pobj;
	short			prec;
	short			severity;
	short			connstatus;
	int				dbrequest;
	unsigned int	index;
};

typedef vector < ChAccess > VecChacc;
#endif
