
#include "cadef.h"

#include "kutilObj.h"
#include "kupaManager.h"
#include "kupaCA.h"
#include "kupaPV.h"


string & kupaGetStringFromEvent (struct event_handler_args eha, string &valueStr)
{
	kupaPV	*pPV = (kupaPV *)ca_puser(eha.chid);
	char	buf[256];
	double	*pdValue = NULL;
	long	*plValue = NULL;

	switch (pPV->getDataType()) {
		case DBR_LONG :
			plValue = (long *)dbr_value_ptr (eha.dbr, pPV->getDataType());
			sprintf (buf, "%ld", plValue[0]);
			valueStr = buf;
			break;

		case DBR_DOUBLE :
			pdValue = (double *)dbr_value_ptr (eha.dbr, pPV->getDataType());
			sprintf (buf, "%g", pdValue[0]);
			valueStr = buf;
			break;

		default :
			valueStr = (char *)eha.dbr;
			break;
	}

	return (valueStr);
}

static void kupaPrintChidInfo (chid chid, char *message)
{
	if (chid) {
		kuDebug (kuDEBUG, "[%s] pv: %s  type(%d) nelements(%ld) host(%s) read(%d) write(%d) state(%d)\n",
				message,
				ca_name(chid),ca_field_type(chid),ca_element_count(chid), ca_host_name(chid),
				ca_read_access(chid), ca_write_access(chid), ca_state(chid));
	}
	else {
		kuDebug (kuDEBUG, "[%s] chid is invalid !!!\n", message);
	}
}

void kupaExceptionCallback (struct exception_handler_args args)
{
    chid		chid = args.chid;
    long		stat = args.stat; /* Channel access status code*/
    const char	*channel;
    static char	*noname = "unknown";

    channel = (chid ? ca_name(chid) : noname);

    if (chid) kupaPrintChidInfo (chid, "kupaExceptionCB");

    kuDebug (kuMON, "[kupaExceptionCallback] stat %s channel %s\n", ca_message(stat),channel);
}

void kupaConnectionCallback (struct connection_handler_args args)
{
    kupaPrintChidInfo (args.chid, "kupaConnCB");

	kupaPV *pPV = (kupaPV *)ca_puser(args.chid);

	if (CA_OP_CONN_UP == args.op) {
		pPV->setConnected (kuTRUE);

		if (-1 == pPV->getDataType()) {
			pPV->addEvent ();
		}
	}
	else {
		pPV->setConnected (kuFALSE);
	}
}

void kupaAccessRightsCallback(struct access_rights_handler_args args)
{
    kupaPrintChidInfo (args.chid, "kupaAccessCB");
}

void kupaEventCallback(struct event_handler_args eha)
{
    if (eha.status!=ECA_NORMAL) {
		kupaPrintChidInfo (eha.chid, "kupaEventCB");
    } else {
		string valueStr;
		kupaGetStringFromEvent (eha, valueStr);
		kupaManager::getInstance()->updatePV ((char *)ca_name(eha.chid), (char *)valueStr.c_str());
    }
}

void kupaTriggerCallback(struct event_handler_args eha)
{
    if (eha.status!=ECA_NORMAL) {
		kupaPrintChidInfo (eha.chid, "kupaTriggerCB");
    } else {
		string valueStr;
		kupaGetStringFromEvent (eha, valueStr);
		kupaManager::getInstance()->processTrigger ((char *)ca_name(eha.chid), atof((char *)valueStr.c_str()));
    }
}

