///////////////////////////////////////////////////////////
//  kupaManager.cpp
//  Implementation of the Class kupaManager
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kutilObj.h"

#include "kupaManager.h"
#include "kupaCollectPV.h"
#include "kupaExportCSV.h"
#include "kupaExportMDS.h"
#include "kupaCA.h"


kupaManager* kupaManager::m_kupaManagerInstance	= NULL;


kupaManager::kupaManager(){

	m_kupaCollectInterface	= NULL;

	m_shotNum	= 0;
	m_shotNumPV	= NULL;
	m_armPV		= NULL;
	m_runPV		= NULL;
	m_opModePV	= NULL;

	m_startEventId	= NULL;
	m_msgQueueId	= NULL;
	m_mdsLock		= new kuLock ("kupaManager.mdsLock");
}


kupaManager::kupaManager(const kupaManager &rhs){

}


kupaManager::~kupaManager(){

	for (unsigned int i = 0; i < m_kupaExportInterfaceVec.size(); i++) {
		delete (m_kupaExportInterfaceVec[i]);
	}

	if (m_kupaCollectInterface) {
		delete (m_kupaCollectInterface);
		m_kupaCollectInterface	= NULL;
	}

	if (m_shotNumPV) {
		delete (m_shotNumPV);
		m_shotNumPV = NULL;
	}

	if (m_armPV) {
		delete (m_armPV);
		m_armPV = NULL;
	}

	if (m_runPV) {
		delete (m_runPV);
		m_runPV = NULL;
	}

	if (m_opModePV) {
		delete (m_opModePV);
		m_opModePV = NULL;
	}

	if (m_mdsLock) {
		delete (m_mdsLock);
		m_mdsLock = NULL;
	}
}


/**
 * returns a unique instance
 */
kupaManager * kupaManager::getInstance(){

	if (NULL == m_kupaManagerInstance) {
		m_kupaManagerInstance = new kupaManager();
	}

	return  m_kupaManagerInstance;
}


kupaCollectInterface* kupaManager::getkupaCollectInterface(){

	return m_kupaCollectInterface;
}


kupaExportInterfaceVec& kupaManager::getkupaExportInterfaceVec(){

	return m_kupaExportInterfaceVec;
}


kupaManager* kupaManager::getkupaManager(){

	return m_kupaManagerInstance;
}


string kupaManager::getManagerName(){

	return m_managerName;
}


string kupaManager::getArmPvName(){

	return m_armPvName;
}


string kupaManager::getRunPvName(){

	return m_runPvName;
}


string kupaManager::getOpModePvName(){

	return m_opModePvName;
}


string kupaManager::getPostActionCmd(){

	return m_postActionCmd;
}


string kupaManager::getShotNumPvName(){

	return m_shotNumPvName;
}


long kupaManager::getShotNum(){

	return m_shotNum;
}


int kupaManager::getSamplingRate(){

	return m_sampingRate;
}


int kupaManager::getOpMode(){

	return m_opMode;
}


double kupaManager::getStartTime(){

	return m_startTime;
}


double kupaManager::getDuration(){
	return m_duration;
}


double kupaManager::getDelaySec(){

	return m_delaySec;
}


int kupaManager::getSampleCnt(){

	return ((int)(getDuration() * getSamplingRate()));
}


epicsEventId kupaManager::getStartEventId(){

	return m_startEventId;
}


epicsMessageQueueId kupaManager::getMsgQueueId(){

	return m_msgQueueId;
}


int kupaManager::initManager(char* name, char* shotNumPvName, char* opModePvName, char* runPvName,
		int samplingRate, double startTime, double duration, double delaySec){

	setManagerName (name);
	setShotNumPvName (shotNumPvName);
	setOpModePvName (opModePvName);
	setRunPvName (runPvName);
	setSamplingRate (samplingRate);
	setStartTime (startTime);
	setDuration (duration);
	setDelaySec (delaySec);

	kuDebug (kuTRACE, "[kupaManager::initManager] name(%s) shot(%s) opmode(%s) run(%s) rate(%d) stime(%f) duration(%f) delay(%f)\n",
			getManagerName().c_str(), getShotNumPvName().c_str(), getOpModePvName().c_str(), getRunPvName().c_str(), 
			getSamplingRate(), getStartTime(), getDuration(), getDelaySec());

	// creates PV collector
	if (NULL == (m_kupaCollectInterface = new kupaCollectPV ())) {
		kuDebug (kuERR, "kupaManager::initManager] kupaCollectPV create failed\n");
		return (kuNOK);
	}

	return (kuOK);
}


int kupaManager::addExport(char *type, char* name, char *path, 
		char* arg3, char* arg4, char* arg5, char* arg6, char* arg7, char* arg8, char* arg9){

	if (!type || !name || !path) {
		kuDebug (kuERR, "[kupaManager::addExport] input is invalid\n");
		return (kuNOK);
	}

	kuDebug (kuTRACE, "[kupaManager::addExport] type(%s) name(%s) path(%s)\n", type, name, path);

	kupaExportInterface * pInterface	= NULL;

	if (0 == strncasecmp (KUPA_EXPORT_TYPE_CSV, type, strlen(type))) {
		pInterface	= new kupaExportCSV (getkupaCollectInterface(), name, path, arg3, arg4, arg5, arg6, arg7, arg8, arg9);

	}
	else if (0 == strncasecmp (KUPA_EXPORT_TYPE_MDS, type, strlen(type))) {
		pInterface	= new kupaExportMDS (getkupaCollectInterface(), name, path, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
	}

	if (NULL == pInterface) {
		kuDebug (kuERR, "kupaManager::addExport] cannot create kupaExport Instance\n");
		return (kuNOK);
	}

	m_kupaExportInterfaceVec.push_back (pInterface);

	return (kuOK);
}


int kupaManager::addPostAction(char* cmd){

	if (!cmd) {
		kuDebug (kuERR, "[kupaManager::addPostAction] input is invalid\n");
		return (kuNOK);
	}

	setPostActionCmd (cmd);

	return (kuOK);
}


int kupaManager::addPV(char* pvName, char* nodeName, int caType, char *description, char *unit){

	if (!pvName || !nodeName) {
		kuDebug (kuERR, "[kupaManager::addPV] input is invalid\n");
		return (kuNOK);
	}

	return (getkupaCollectInterface()->addPV (pvName, nodeName, caType, description, unit));
}


int kupaManager::updatePV(char* pvName, char* data){

	if (!pvName || !data) {
		kuDebug (kuERR, "[kupaManager::updatePV] input is invalid\n");
		return (kuNOK);
	}

	return (getkupaCollectInterface()->updatePV (pvName, data));
}


string & kupaManager::getValue(char* pvName, string &value){

	return (getkupaCollectInterface()->getValue (pvName, value));
}


int kupaManager::processTrigger(char* pvName, double value){

	static int bFirstRunEvent = 1;

	if (!pvName) {
		kuDebug (kuERR, "[kupaManager::processTrigger] input is invalid\n");
		return (kuNOK);
	}

	if (pvName == getShotNumPvName()) {
		// sets shot information
		setShotNum ((long)value);

		kuDebug (kuMON, "[kupaManager::processTrigger] shot(%f)\n", value);
	}
	else if (pvName == getArmPvName()) {
		if (1 == value) {
			kuDebug (kuMON, "[kupaManager::processTrigger] ARMING ...\n");
		}
	}
	else if (pvName == getRunPvName()) {
		if (1 == bFirstRunEvent) {
			kuDebug (kuMON, "[kupaManager::processTrigger] %s : This is first event. ignore it.\n", pvName);

			bFirstRunEvent = 0;
		}
		else if (1 == value) {
			kuDebug (kuMON, "[kupaManager::processTrigger] RUN ...\n");

			sendStartEvent ();
		}
	}
	else if (pvName == getOpModePvName()) {
		setOpMode ((int)value);
		kuDebug (kuMON, "[kupaManager::processTrigger] opmode(%f)\n", value);
	}

	return (kuOK);
}


void kupaRunThread(void* arg){

	if (NULL != arg) {
		kupaManager* pManager = (kupaManager *)arg;
		pManager->run ();
	}
}


int kupaManager::runManager(){

	// creates run thread
	epicsThreadCreate (
			"kupaManager",
			epicsThreadPriorityMedium,
			epicsThreadGetStackSize(epicsThreadStackMedium),
			(EPICSTHREADFUNC)kupaRunThread,
			this);

	// waits for completion of creating thread during some time 
	epicsThreadSleep (2.0);

	return (kuOK);
}


int kupaManager::performCollect(){

	kuDebug (kuDEBUG, "[kupaManager::performCollect] collect ... \n");

	getkupaCollectInterface()->collect();

	return (kuOK);
}


int kupaManager::performUpdate(){

	kuDebug (kuDEBUG, "[kupaManager::performUpdate] update ... \n");

	getkupaCollectInterface()->update();

	return (kuOK);
}


int kupaManager::performExport(){

	kupaExportInterfaceVec	&vecExport	= getkupaExportInterfaceVec();

	kuDebug (kuMON, "[kupaManager::performExport] opmode(%d) shot(%ld) stime(%g) duration(%g) rate(%d) size(%d)\n",
			getOpMode(), getShotNum(), getStartTime(), getDuration(), getSamplingRate(), vecExport.size());

	for (unsigned int i = 0; i < vecExport.size(); i++) {
		vecExport[i]->store ();
	}

	if ("" != getPostActionCmd()) {
		kuDebug (kuMON, "[kupaManager::performExport] cmd(%s)\n", getPostActionCmd().c_str());

		system (getPostActionCmd().c_str());
	}

	return (kuOK);
}


int kupaManager::performReset(){

	kuDebug (kuDEBUG, "[kupaManager::performReset] reset ... \n");

	getkupaCollectInterface()->reset();

	return (kuOK);
}


int kupaManager::sendStartEvent(){

	kuDebug (kuTRACE, "[kupaManager::sendStartEvent] ... \n");

#if 0
	epicsEventSignal (getStartEventId());
#else
	kupaMessage	svcMessage (KUPA_SVC_START);

	epicsMessageQueueSend (getMsgQueueId(), &svcMessage, sizeof(kupaMessage));
#endif

	return (kuOK);
}


int kupaManager::sendUpdateEvent(){

	kuDebug (kuTRACE, "[kupaManager::sendUpdateEvent] ... \n");

	kupaMessage	svcMessage (KUPA_SVC_UPDATE);

	epicsMessageQueueSend (getMsgQueueId(), &svcMessage, sizeof(kupaMessage));

	return (kuOK);
}


int kupaManager::initChannelAccess(){

	kuDebug (kuTRACE, "[kupaManager::initChannelAccess] ... \n");

	// dbContext: preemptive callback required for direct in memory interfacing of CA channels to the DB
	//SEVCHK (ca_context_create (ca_disable_preemptive_callback), "ca_context_create");

	SEVCHK (ca_context_create (ca_enable_preemptive_callback), "ca_context_create");
	SEVCHK (ca_add_exception_event (kupaExceptionCallback, NULL), "ca_add_exception_event");

	if ("" != getShotNumPvName()) {
		m_shotNumPV	= new kupaPV (getShotNumPvName(), "", kupaPV::KUPA_CA_EVENT, "", "");
		m_shotNumPV->createChannel (kupaConnectionCallback, kupaTriggerCallback, this);
	}

	if ("" != getArmPvName()) {
		m_armPV	= new kupaPV (getArmPvName(), "", kupaPV::KUPA_CA_EVENT, "", "");
		m_armPV->createChannel (kupaConnectionCallback, kupaTriggerCallback, this);
	}

	if ("" != getOpModePvName()) {
		m_opModePV	= new kupaPV (getOpModePvName(), "", kupaPV::KUPA_CA_EVENT, "", "");
		m_opModePV->createChannel (kupaConnectionCallback, kupaTriggerCallback, this);
	}

	if ("" != getRunPvName()) {
		m_runPV	= new kupaPV (getRunPvName(), "", kupaPV::KUPA_CA_EVENT, "Start Trigger", "");
		m_runPV->createChannel (kupaConnectionCallback, kupaTriggerCallback, this);
	}

	getkupaCollectInterface()->createChannel (kupaConnectionCallback, kupaEventCallback, this);

	return (kuOK);
}


int kupaManager::run(){

	kuDebug (kuTRACE, "[kupaManager::run] ... \n");

	int		iCollectCnt;
	int		iSampleCnt;

	// initializes channel access event
	initChannelAccess ();

	// creates an event to start archiving
	m_startEventId	= epicsEventCreate(epicsEventEmpty);
	m_msgQueueId	= epicsMessageQueueCreate(100, sizeof(kupaMessage));

	while (kuTRUE) {
		// ----------------------------------------------------
		// initialize
		// ----------------------------------------------------

		performReset ();

		// ----------------------------------------------------
		// waits an event to start collection of value of PVs
		// ----------------------------------------------------
		kuDebug (kuINFO, "[kupaManager::run] waiting event to start collection\n");

#if 0
		// waits that SYS_RUN is changed to 1
		epicsEventWait (getStartEventId());
#else
		epicsMessageQueueReceive (getMsgQueueId(), &m_svcMessage, sizeof(kupaMessage));
#endif

		switch (m_svcMessage.getSvcType()) {
			case KUPA_SVC_START :
				kuDebug (kuMON, "[kupaManager::run] collection will be started after %g second(s)\n", getDelaySec());

				// waits for some time before to start
				doStartDelay ();

				iCollectCnt	= 0;
				iSampleCnt	= getSampleCnt();

				while (iCollectCnt < iSampleCnt) {
					// collects data
					performCollect ();

					iCollectCnt++;

					//TODO : timer ???
					doSamplingDelay();
				}

				// ----------------------------------------------------
				// exports data
				// ----------------------------------------------------

				performExport ();

				break;

			case KUPA_SVC_UPDATE :
				kuDebug (kuMON, "[kupaManager::run] PVs will be updated ...\n", getDelaySec());

				performUpdate ();

				break;

			default :
				kuDebug (kuERR, "[kupaManager::run] svcType(%d) is invalid \n", m_svcMessage.getSvcType());
				break;
		}
	}

	return 0;
}


void kupaManager::doStartDelay(){

	if (getDelaySec() > 0) {
		epicsThreadSleep (getDelaySec());
	}
}


void kupaManager::doSamplingDelay(){

	if (getSamplingRate() > 0) {
		epicsThreadSleep (1.0 / getSamplingRate());
	}
	else {
		epicsThreadSleep (1.0);
	}
}


void kupaManager::setkupaCollectInterface(kupaCollectInterface* newVal){

	m_kupaCollectInterface = newVal;
}


void kupaManager::setkupaExportInterfaceVec(kupaExportInterfaceVec& newVal){

	m_kupaExportInterfaceVec = newVal;
}


void kupaManager::setkupaManager(kupaManager* newVal){

	m_kupaManagerInstance = newVal;
}


void kupaManager::setManagerName(string newVal){

	m_managerName = newVal;
}


void kupaManager::setShotNumPvName(string newVal){

	m_shotNumPvName = newVal;
}


void kupaManager::setArmPvName(string newVal){

	m_armPvName = newVal;
}


void kupaManager::setRunPvName(string newVal){

	m_runPvName = newVal;
}


void kupaManager::setOpModePvName(string newVal){

	m_opModePvName = newVal;
}


void kupaManager::setPostActionCmd(string newVal){

	m_postActionCmd = newVal;
}


void kupaManager::setOpMode(int newVal){

	m_opMode = newVal;
}


void kupaManager::setShotNum(long newVal){

	m_shotNum = newVal;
}


void kupaManager::setSamplingRate(int newVal){

	m_sampingRate = newVal;
}


void kupaManager::setStartTime(double newVal){

	m_startTime = newVal;
}


void kupaManager::setDuration(double newVal){

	m_duration = newVal;
}


void kupaManager::setDelaySec(double newVal){

	m_delaySec = newVal;
}


epicsMutexId kupaManager::getLockId() {

	return m_mdsLock->getLockId();
}

void kupaManager::setLockId(epicsMutexId newVal){

	m_mdsLock->setLockId (newVal);
}


int kupaManager::lock (){
	return (m_mdsLock->lock ());
}


int kupaManager::unlock (){
	return (m_mdsLock->unlock ());
}


// ------------------------------------------------------ 

static void kupaInitManagerCallFunc (const iocshArgBuf * args) {
	kupaManager::getInstance()->initManager (
			args[0].sval, args[1].sval, args[2].sval, args[3].sval, atoi(args[4].sval),
			atof(args[5].sval), atof(args[6].sval), atof(args[7].sval)
	);
}

static void kupaAddExportCallFunc (const iocshArgBuf * args) {
	kupaManager::getInstance()->addExport (
			args[0].sval, args[1].sval, args[2].sval, args[3].sval, args[4].sval,
			args[5].sval, args[6].sval, args[7].sval, args[8].sval, args[9].sval
	);
}

static void kupaAddPVCallFunc (const iocshArgBuf * args) {
	kupaManager::getInstance()->addPV (
			args[0].sval, args[1].sval, atoi(args[2].sval), args[3].sval, args[4].sval
	);
}

static void kupaAddPostActCallFunc (const iocshArgBuf * args) {
	kupaManager::getInstance()->addPostAction (args[0].sval);
}

static void kupaRunManagerCallFunc (const iocshArgBuf * args) {
	kupaManager::getInstance()->runManager ();
}

static const iocshArg kupaInitManagerArgList[] = {
	{ "Manager Name"	, iocshArgString },
	{ "Shot Num PV Name", iocshArgString },
	//{ "Arming PV Name"	, iocshArgString },
	{ "OpMode PV Name"	, iocshArgString },
	{ "Runing PV Name"	, iocshArgString },
	{ "Sampling Rate"	, iocshArgString },
	{ "Start Time"		, iocshArgString },
	{ "Duration"		, iocshArgString },
	{ "Start Dealy(ms)"	, iocshArgString },
};

static const iocshArg* const kupaInitManagerArgs[] = {
	&kupaInitManagerArgList[0], &kupaInitManagerArgList[1], &kupaInitManagerArgList[2],
	&kupaInitManagerArgList[3], &kupaInitManagerArgList[4], &kupaInitManagerArgList[5],
	&kupaInitManagerArgList[6], &kupaInitManagerArgList[7],
};

static const iocshArg kupaAddExportArgList[] = {
	{ "Type of export"	, iocshArgString },		// type : CSV, MDS, ???
	{ "File or tree"	, iocshArgString },		// file or tree
	{ "Path or addr"	, iocshArgString },		// path or MDSip address
	{ "arg3 or event"	, iocshArgString },		// parameter : 
	{ "arg4 or eventip"	, iocshArgString },		// parameter : 
	{ "arg5 or opmode"	, iocshArgString },		// parameter : 
	{ "arg6"			, iocshArgString },		// parameter : 
	{ "arg7"			, iocshArgString },		// parameter : 
	{ "arg8"			, iocshArgString },		// parameter : 
	{ "arg9"			, iocshArgString },		// parameter : 
};

static const iocshArg* const kupaAddExportArgs[] = {
	&kupaAddExportArgList[0], &kupaAddExportArgList[1], &kupaAddExportArgList[2],
	&kupaAddExportArgList[3], &kupaAddExportArgList[4], &kupaAddExportArgList[5],
	&kupaAddExportArgList[6], &kupaAddExportArgList[7], &kupaAddExportArgList[8],
	&kupaAddExportArgList[9],
};

static const iocshArg kupaAddPVArgList[] = {
	{ "PV Name"			, iocshArgString },
	{ "Node Name"		, iocshArgString },
	{ "CA Type"			, iocshArgString },
	{ "Description"		, iocshArgString },
	{ "Unit"			, iocshArgString },
};

static const iocshArg* const kupaAddPVArgs[] = {
	&kupaAddPVArgList[0], &kupaAddPVArgList[1], &kupaAddPVArgList[2],
	&kupaAddPVArgList[3], &kupaAddPVArgList[4],
};

static const iocshArg kupaAddPostActArgList[] = {
	{ "Post Action Cmd"	, iocshArgString },
};

static const iocshArg* const kupaAddPostActArgs[] = {
	&kupaAddPostActArgList[0], 
};

static const iocshFuncDef kupaInitManagerFuncDef= { "kupaInitManager", KU_DIM(kupaInitManagerArgs), kupaInitManagerArgs };
static const iocshFuncDef kupaAddExportFuncDef	= { "kupaAddExport"	 , KU_DIM(kupaAddExportArgs)  , kupaAddExportArgs	};
static const iocshFuncDef kupaAddPVFuncDef		= { "kupaAddPV"		 , KU_DIM(kupaAddPVArgs)	  , kupaAddPVArgs		};
static const iocshFuncDef kupaAddPostActFuncDef	= { "kupaAddPostAct" , KU_DIM(kupaAddPostActArgs) , kupaAddPostActArgs	};
static const iocshFuncDef kupaRunManagerFuncDef	= { "kupaRunManager" , 0						  , NULL				};

static void kupaManagerRegister() {
	iocshRegister (&kupaInitManagerFuncDef	, kupaInitManagerCallFunc);
	iocshRegister (&kupaAddExportFuncDef	, kupaAddExportCallFunc);
	iocshRegister (&kupaAddPVFuncDef		, kupaAddPVCallFunc);
	iocshRegister (&kupaAddPostActFuncDef	, kupaAddPostActCallFunc);
	iocshRegister (&kupaRunManagerFuncDef	, kupaRunManagerCallFunc);
}

epicsExportRegistrar(kupaManagerRegister);

