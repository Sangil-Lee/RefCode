///////////////////////////////////////////////////////////
//  kupaCollectPV.cpp
//  Implementation of the Class kupaCollectPV
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:58
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kutilObj.h"
#include "kupaCollectPV.h"


kupaCollectPV::kupaCollectPV(){

}


kupaCollectPV::~kupaCollectPV(){

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		if (NULL != pos->second) {
			delete (pos->second);
			pos->second	= NULL;
		}
	}

	pvMap.clear ();
}


int kupaCollectPV::collect(){

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	kuDebug (kuINFO, "[kupaCollectPV::collect] %d PV(s) are collecting ...\n", pvMap.size());

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		if (NULL != pos->second) {
			pos->second->collect ();
		}
	}

	return (kuOK);
}


int kupaCollectPV::update(){

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	kuDebug (kuINFO, "[kupaCollectPV::update] %d PV(s) are updating ...\n", pvMap.size());

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		if (NULL != pos->second) {
			pos->second->update ();
		}
	}

	return (kuOK);
}


void kupaCollectPV::reset(){

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		if (NULL != pos->second) {
			pos->second->reset ();
		}
	}
}


kupaCollectPVMap & kupaCollectPV::getkupaCollectPVMap() {

	return (m_kupaCollectPVMap);
}


int kupaCollectPV::addPV(string pvName, string nodeName, int caType, string description, string unit){

	kuDebug (kuTRACE, "[kupaCollectPV::addPV] pvName(%s) node(%s) caType(%d) desc(%s) unit(%s)\n",
			pvName.c_str(), nodeName.c_str(), caType, description.c_str(), unit.c_str());

	kupaPV *	pvRec	= new kupaPV(pvName, nodeName, caType, description, unit);

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	pos = pvMap.find (pvName);

	if (pos != pvMap.end()) {
		kuDebug (kuWARN, "[kupaCollectPV::addPV] overwirte : pv(%s) node(%s)\n",
			pos->second->getPVName().c_str(), pos->second->getNodeName().c_str());

		delete (pos->second);
		pvMap.erase (pos);

	}
	else {
		kuDebug (kuINFO, "[kupaCollectPV::addPV] new : pv(%s) node(%s)\n", pvName.c_str(), nodeName.c_str());
	}

	pvMap[pvName]	= pvRec;

	return (kuOK);
}


int kupaCollectPV::updatePV(string pvName, string value){

	kuDebug (kuDEBUG, "[kupaCollectPV::updatePV] pvName(%s) value(%s)\n", pvName.c_str(), value.c_str());

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	pos = pvMap.find (pvName);

	if (pos != pvMap.end()) {
		pos->second->setValue (value);
	}

	return (kuOK);
}


string & kupaCollectPV::getValue(string pvName, string &value){

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	pos = pvMap.find (pvName);

	if (pos != pvMap.end()) {
		pos->second->getValue (value);
	}

	kuDebug (kuDEBUG, "[kupaCollectPV::getValue] pvName(%s) value(%s)\n", pvName.c_str(), value.c_str());

	return (value);
}


int kupaCollectPV::createChannel(kupaConnectionHandler connectionCallback, kupaEventHandler eventCallback, void *pUser){

	kuDebug (kuTRACE, "[kupaCollectPV::createChannel] ...\n");

	kupaCollectPVMap			&pvMap	= getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		if (NULL != pos->second) {
			pos->second->createChannel (connectionCallback, eventCallback, pUser);
		}
	}

	return (kuOK);
}


