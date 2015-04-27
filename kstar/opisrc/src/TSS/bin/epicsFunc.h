#ifndef EPICS_FNC_H
#define EPICS_FNC_H

#include <epicsStdlib.h>
#include "cadef.h"
#include <QString>

#define MSG_BUFFER_SIZE		128
#ifndef MAX_PV_NAME_LEN 
#define MAX_PV_NAME_LEN 60
#endif


typedef void ( *VOIDFUNCPTR ) (...);

typedef struct {
	char name[MAX_PV_NAME_LEN];
	chid cid;
	evid eid;
	char flag_cid;
	char flag_eid;
//	int nIndex;
//	int touch_flag;
//	int first_event;
	unsigned int status_flag;
}ST_PV_NODE;

typedef void (*EXECFUNCQUEUE) (connection_handler_args args);
typedef struct {
	EXECFUNCQUEUE     pFunc;
} callbackData;


int ca_Connect(  char *strPVname, chid &chidChannelToPV);
int ca_ConnectCB( char *strPVname, chid &chidChannelToPV, callbackData cbd);
int ca_Connect_STPV(ST_PV_NODE &myPV, VOIDFUNCPTR fptr = 0);

int ca_Disconnect( chid &chidChannelToPV );
int ca_Disconnect_STPV( ST_PV_NODE &myPV);



int ca_Get_QStr(chid &chidChannelToPV, QString &qstrVal);
int ca_Put_QStr(chid &chidChannelToPV, QString qstrVal);

int ca_Get_StrVal(chid &chidChannelToPV,  char *strVal);
int ca_Get_Double(chid &chidChannelToPV, double &dval);



int ca_Put_StrVal(chid &chidChannelToPV,  const char *strVal);
int ca_Put_Double(chid &chidChannelToPV, double dval);



int ca_PutST_QStr(ST_PV_NODE &myPV, QString qstrVal);
int ca_PutST_StrVal(ST_PV_NODE &myPV,  const char *strVal);
int ca_PutST_Double(ST_PV_NODE &myPV, double dval);

int ca_GetST_QStr(ST_PV_NODE &myPV, QString qstrVal);
int ca_GetST_StrVal(ST_PV_NODE &myPV,  char *strVal);
int ca_GetST_Double(ST_PV_NODE &myPV, double &dval);





int cvt_Double_to_String(double dval, char *strVal);

#endif


