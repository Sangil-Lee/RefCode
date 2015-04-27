/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
*     National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
*     Operator of Los Alamos National Laboratory.
* Copyright (c) 2002 Berliner Elektronenspeicherringgesellschaft fuer
*     Synchrotronstrahlung.
* EPICS BASE Versions 3.13.7
* and higher are distributed subject to a Software License Agreement found
* in file LICENSE that is included with this distribution. 
\*************************************************************************/
/* 
 *  caarrayget.h,v 1.1.2.6 2004/10/19 15:17:03 lange Exp
 *
 *  Author: Ralph Lange (BESSY)
 *
 */
#ifndef INCLtool_libh
#define INCLtool_libh
#include <epicsTime.h>
#include <cadef.h>
#include <QtGui>
#include <vector>
#include <string>
#include "caMultiwaveplot.h"

#define CA_PRIORITY 50          /* CA priority */
#define DEFAULT_TIMEOUT 1.0     /* Default CA timeout */

#define VALID_DOUBLE_DIGITS 18  /* Max usable precision for a double */
#define PEND_EVENT_SLICES 5     /* No. of pend_event slices for callback requests */

/* Different output formats */
typedef enum { plain, terse, all, specifiedDbr } OutputT;

/* Different request types */
typedef enum { get, callback } RequestT;

/* Type of timestamp */
typedef enum { absolute, relative, incremental, incrementalByChan } TimeT;

/* Structure representing one PV (= channel) */
typedef struct 
{
    char* name;
    chid  mychid;
    long  dbfType;
    long  dbrType;
    unsigned long nElems;
    unsigned long reqElems;
    int status;
    void* value;
    epicsTimeStamp tsPrevious;
    char firstStampPrinted;
    char onceConnected;
} pv;

using std::vector;
using std::string;
class CAMultiwaveplot;
class	QtCAArrayDataThread:public QThread
{
	Q_OBJECT
public:
	QtCAArrayDataThread();
	virtual	~QtCAArrayDataThread();

	char *val2str (const void *v, unsigned type, int index);
	int  create_pvs (pv *pvs, int nPvs, caCh *pCB );
	int  connect_pvs (pv *pvs, int nPvs );
	int	caget (pv *pvs, int nPvs, RequestT request, OutputT format,
			chtype dbrType, unsigned long reqElems);
	friend void event_handler (evargs args);
	void setStop(const bool stop){mbstop = stop;};
	const bool getStop(){return mbstop;};
	void SetPVNames(vector<string> pvnames){ mvecpvnames=pvnames;};
	void SetChNodes(vector<chid> chnodes){ mvecnodes=chnodes;};
	void SetReqEON(int reqeon){mreqelement=reqeon;};
	void SetParent(CAMultiwaveplot *parent){mparent=parent;};
	void SetUpdateTime(int msec){mmsec = msec;};
public slots:

signals:
protected:
	void run();

private:
	void init();
	int nConn;
	static int nRead;
	TimeT tsType;        /* Timestamp type flag (-r -i -I options) */
	int enumAsNr;        /* Used for -n option (get DBF_ENUM as number) */
	double caTimeout;    /* Wait time default (see -w option) */
	pv *mpvs;
	int mpvcnt;
	bool mbstop;
	vector<string> mvecpvnames;
	vector<chid> mvecnodes;
	int mreqelement;
	int mmsec;
	CAMultiwaveplot *mparent;
};
/*
 * no additions below this endif
 */
#endif /* ifndef INCLtool_libh */
