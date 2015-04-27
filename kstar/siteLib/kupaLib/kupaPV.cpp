///////////////////////////////////////////////////////////
//  kupaPV.cpp
//  Implementation of the Class kupaPV
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////


#include "kutilObj.h"
#include "kupaPV.h"

kuLock kupaPV::m_valueLock("PV Value Lock");

kupaPV::kupaPV(string pvName, string nodeName, int caType, string description, string unit){

	setPVName (pvName);
	setNodeName (nodeName);
	setCAType (caType);
	setDescription (description);
	setUnit (unit);
	setDataType (-1);

	m_currValue[0]		= 0x00;
	m_bConnected		= 0;
	m_bValidDb			= kuFALSE;
	m_dbStatus			= 0;
	m_connectCallback	= NULL;
	m_eventCallback		= NULL;

#if (KUPA_USE_STATIC_LIB || KUPA_USE_DYNAMIC_LIB)

	if (KUPA_CA_DB == getCAType()) {
#if KUPA_USE_STATIC_LIB
		m_pdbentry		= NULL;

		if (NULL == m_pdbentry) {
			m_pdbentry = kupaDbAllocEntry();

			kupaDbInitEntry (m_pdbentry);
		}
#endif	// KUPA_USE_STATIC_LIB

#if KUPA_USE_DYNAMIC_LIB
		m_pdbAddr		= NULL;

		if (NULL == m_pdbAddr) {
			m_pdbAddr = kupaDbAllocAddr();

			if (kuOK == kupaDbInitAddr (m_pdbAddr, m_pvName.c_str())) {
				setValidDb (kuTRUE);
			}
		}
#endif	// KUPA_USE_DYNAMIC_LIB
	}

#endif	// (KUPA_USE_STATIC_LIB || KUPA_USE_DYNAMIC_LIB)
}


kupaPV::~kupaPV(){

#if KUPA_USE_STATIC_LIB
	if (NULL != m_pdbentry) {
		kupaDbFinishEntry (m_pdbentry);
		kupaDbFreeEntry (m_pdbentry);
		m_pdbentry = NULL;
	}
#endif	// KUPA_USE_STATIC_LIB

#if KUPA_USE_DYNAMIC_LIB
	if (NULL != m_pdbAddr) {
		free (m_pdbAddr);
		m_pdbAddr = NULL;
	}
#endif	// KUPA_USE_DYNAMIC_LIB
}


int kupaPV::update(){

	char buf[64];
	double dValue;
	string sValue;

#if !(KUPA_USE_STATIC_LIB || KUPA_USE_DYNAMIC_LIB)
	if (KUPA_CA_EVENT != getCAType()) {
#else
	if (KUPA_CA_GET == getCAType()) {
#endif
		ca_get (DBR_DOUBLE, getChId(), (void *)&dValue);
		ca_pend_io (0.5);
		sprintf (buf, "%g", dValue);
	}
	else if (KUPA_CA_DB == getCAType()) {
#if KUPA_USE_STATIC_LIB
		if (kuTRUE != isValidDb ()) {
			if (kuOK != kupaDbFindRecord (m_pdbentry, m_pvFieldName.c_str())) {
				setValue ("N/A");
				return (kuNOK);
			}

			setValidDb (kuTRUE);
		}

		kupaDbGetString (m_pdbentry, buf);

#elif KUPA_USE_DYNAMIC_LIB
		if (kuTRUE != kupaDbIsValidAddr (m_pdbAddr)) {
			return (kuNOK);
		}

#if 0
		// string has integer value : 12.345 => 12
		kupaDbGetField (m_pdbAddr, buf);
#else
		dValue = kupaDbGetValue (m_pdbAddr);
		sprintf (buf, "%g", dValue);
#endif
#endif
	}
	else {
		return (kuNOK);
	}

	setValue (buf);

	kuDebug (kuDEBUG, "[kupaPV::update] pv(%s) caType(%d) value(%s)\n",
			m_pvName.c_str(), getCAType(), getValue(sValue).c_str());

	return (kuOK);
}


int kupaPV::collect(){

	update ();

	// collects last value
	string sValue;
	
	getValue(sValue);;

	m_valueVector.push_back (sValue);

	kuDebug (kuDEBUG, "[kupaPV::collect] pv(%s) node(%s) size(%d) value(%s)\n",
			m_pvName.c_str(), m_nodeName.c_str(), m_valueVector.size(), sValue.c_str());

	return (kuOK);
}


int kupaPV::createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser){

	if (KUPA_CA_EVENT != getCAType()) {
		ca_create_channel (getPVName().c_str(), NULL, this, KUPA_CA_PRIORITY, &m_chid);
	}
	else {
		ca_create_channel (getPVName().c_str(), connectionCallback, this, KUPA_CA_PRIORITY, &m_chid);
	}

	if (NULL != connectionCallback) {
		setConnectHandler (connectionCallback);
	}

	if (NULL != eventCallback) {
		setEventHandler (eventCallback);
	}

	return (kuOK);
}


int kupaPV::addEvent(){

	return (addEvent (getConnectHandler(), getEventHandler(), this));
}


int kupaPV::addEvent(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser){

	if (-1 == m_dataType) {
		m_dataType = ca_field_type(m_chid);
	}

	if (-1 == m_dataType) {
		setDataType (DBR_STRING);
	}
	else if (DBF_LONG == m_dataType) {
		setDataType (DBR_LONG);			// allows a big decimal value
	}
	else if (DBF_ENUM == m_dataType) {
		setDataType (DBR_DOUBLE);
	}
	else {
		//setDataType (DBR_STRING);		// DBR_STRING's decimal is 2. So used double.
		setDataType (DBR_DOUBLE);
	}

	if (kuTRUE == isEventType()) {
		// ca_add_event was deprecated
		//ca_add_event (getDataType(), getChId(), getEventHandler(), pUser, NULL);

		// array is not allowed in this version
		//ca_create_subscription (getDataType(), 0, getChId(), DBE_VALUE, getEventHandler(), pUser, NULL);

		ca_create_subscription (getDataType(), 1, getChId(), DBE_VALUE, getEventHandler(), pUser, NULL);

		kuDebug (kuDEBUG, "[kupaPV::addEvent] ca_create_subscription : %s \n", getPVName().c_str());
	}

	return (kuOK);
}


string kupaPV::getNodeName(){

	return m_nodeName;
}


string kupaPV::getPVName(){

	return m_pvName;
}


string kupaPV::getPVFieldName(){

	return m_pvFieldName;
}


string kupaPV::getDescription(){

	return m_description;
}


string kupaPV::getUnit(){

	return m_unit;
}


int kupaPV::getValueSize(){

	return m_valueVector.size();
}


string kupaPV::getValue(){

	string str;

	getValue (str);

	return (str);
}


string & kupaPV::getValue(string &value){

	lockValue ();

	value = m_currValue;

	unlockValue ();

	return value;
}


string & kupaPV::getValue(kuUInt32 idx, string &value){

	if ( (0 <= idx) && (idx < m_valueVector.size()) ) {
		value = m_valueVector[idx];
	}

	return value;
}


int kupaPV::getValues(kuUInt32 size, kuFloat64 *pValues){

	kuUInt32	idx = 0;

	if (NULL != pValues) {
		for (idx = 0; idx < size && idx < m_valueVector.size(); idx++) {
			pValues[idx] = atof(m_valueVector[idx].c_str());
		}
	}

	return idx;
}


int kupaPV::getCAType(){

	return m_caType;
}


chid kupaPV::getChId(){

	return m_chid;
}


int kupaPV::getDataType(){

	return m_dataType;
}


int kupaPV::isConnected(){

	return m_bConnected;
}


int kupaPV::isEventType(){

	return (KUPA_CA_EVENT == getCAType());
}


int kupaPV::isValidDb(){

#if KUPA_USE_STATIC_LIB
	if (NULL == m_pdbentry) {
		return (kuFALSE);
	}
#endif	// KUPA_USE_STATIC_LIB

#if KUPA_USE_DYNAMIC_LIB
	if (NULL == m_pdbAddr) {
		return (kuFALSE);
	}
#endif	// KUPA_USE_DYNAMIC_LIB

	return (m_bValidDb);
}


kupaConnectionHandler kupaPV::getConnectHandler(){

	return m_connectCallback;
}


kupaEventHandler kupaPV::getEventHandler(){

	return m_eventCallback;
}


void kupaPV::reset(){
	m_valueVector.clear ();
}


void kupaPV::setNodeName(string newVal){

	m_nodeName = newVal;
}


void kupaPV::setPVName(string newVal){

	m_pvName		= newVal;
	m_pvFieldName	= m_pvName + ".VAL";
}


void kupaPV::setDescription(string newVal){

	m_description = newVal;
}


void kupaPV::setUnit(string newVal){

	m_unit = newVal;
}


void kupaPV::setValue(string newVal){

	lockValue ();

	strcpy (m_currValue, newVal.c_str());

	unlockValue ();
}


void kupaPV::setDataType(int newVal){

	m_dataType = newVal;
}


void kupaPV::setCAType(int newVal){

	m_caType = newVal;
}


void kupaPV::setConnected(int newVal){

	m_bConnected = newVal;
}


void kupaPV::setValidDb(int newVal){

	m_bValidDb = newVal;
}


void kupaPV::setConnectHandler(kupaConnectionHandler newVal) {

	m_connectCallback = newVal;
}


void kupaPV::setEventHandler(kupaEventHandler newVal){

	m_eventCallback = newVal;
}


void kupaPV::lockValue(){

#if KUPA_USE_VALUE_LOCK
	m_valueLock.lock();
#endif
}


void kupaPV::unlockValue(){

#if KUPA_USE_VALUE_LOCK
	m_valueLock.unlock();
#endif
}


