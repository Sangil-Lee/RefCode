///////////////////////////////////////////////////////////
//  kupaExportFile.cpp
//  Implementation of the Class kupaExportFile
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:53
//  Original author: ysw
///////////////////////////////////////////////////////////

#include "kutilObj.h"

#include "kupaExportFile.h"


kupaExportFile::kupaExportFile(kupaCollectInterface * pCollect){

	m_kupaCollectInterface = pCollect;
}





kupaExportFile::~kupaExportFile(){

}





int kupaExportFile::store(){

	kuDebug (kuINFO, "[kupaExportMDS::store] not implemented yet\n");

	return (kuNOK);
}
