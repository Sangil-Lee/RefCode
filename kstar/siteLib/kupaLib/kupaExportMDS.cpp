///////////////////////////////////////////////////////////
//  kupaExportMDS.cpp
//  Implementation of the Class kupaExportMDS
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:52
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kutilObj.h"

#include "kupaMDS.h"
#include "kupaExportMDS.h"
#include "kupaCollectPV.h"
#include "kupaManager.h"

kupaExportMDS::kupaExportMDS(kupaCollectInterface * pCollect, 
		char *treeName, char *mdsAddr, char *eventName, char *eventAddr, 
		char *opModeName, char* arg6, char* arg7, char* arg8, char* arg9){

	m_kupaCollectInterface	= pCollect;

	setTreeName (treeName);
	setMdsAddr (mdsAddr);
	setEventName (eventName);
	setEventAddr (eventAddr);
	setOpModeName (opModeName);
}


kupaExportMDS::~kupaExportMDS(){

}


int kupaExportMDS::store(){

	kupaManager					*pManager	= kupaManager::getInstance();
	kupaCollectPVMap			&pvMap		= ((kupaCollectPV *)m_kupaCollectInterface)->getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	kuDebug (kuMON, "[kupaExportMDS::store] ----------------------------------------\n");
	kuDebug (kuMON, "[kupaExportMDS::store] Tree(%s) Addr(%s) OpMode(%s)\n",
			getTreeName().c_str(), getMdsAddr().c_str(), getOpModeName().c_str());

	// checks whether operation mode is match
	if ( (1 == pManager->getOpMode()) && (getOpModeName() == KUPA_EXPORT_MDS_LOCAL) ) {
		kuDebug (kuWARN, "[kupaExportMDS::store] operation mode is not match : skip ...\n");
		return (kuOK);
	}

	if ( (1 != pManager->getOpMode()) && (getOpModeName() == KUPA_EXPORT_MDS_REMOTE) ) {
		kuDebug (kuWARN, "[kupaExportMDS::store] operation mode is not match : skip ...\n");
		return (kuOK);
	}

	pManager->lock ();

	// --------------------------------
	// creates and opens tree
	// --------------------------------

	kupaMDS		mdsAccess;

	mdsAccess.set (getTreeName(), getMdsAddr(), getEventName(), getEventAddr());
	mdsAccess.connect ();
	mdsAccess.open (pManager->getShotNum());

	// --------------------------------
	// stores data
	// --------------------------------

	kuUInt32	size = 0;
	kuFloat64	timeGap = 1.0 / pManager->getSamplingRate();
	kuFloat64	*pValues = NULL;
	kupaPV		*pPV = NULL;

	if (pvMap.end() != (pos = pvMap.begin())) {
		size = pos->second->getValueSize();

		pValues = (kuFloat64 *)malloc(sizeof(kuFloat64)*size);
	}

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos) {
		pPV = pos->second;

		if ("" == pPV->getNodeName()) {
			continue;
		}

		// gets double values
		pPV->getValues (size, pValues);

		// puts data to MDSplus
		mdsAccess.write (pPV->getNodeName().c_str(), pPV->getUnit().c_str(),
				pValues, DTYPE_DOUBLE, size, pManager->getStartTime(), timeGap);
	}

	if (NULL != pValues) {
		free (pValues);
		pValues = NULL;
	}

	// --------------------------------
	// closes tree
	// --------------------------------

	mdsAccess.close ();
	mdsAccess.disConnect ();

	// --------------------------------
	// notifies event
	// --------------------------------

	mdsAccess.sendEvent ();

	pManager->unlock ();

	return (kuOK);
}

