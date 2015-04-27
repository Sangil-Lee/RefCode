///////////////////////////////////////////////////////////
//  kupaExportCSV.cpp
//  Implementation of the Class kupaExportCSV
//  Created on:      23-4-2013 ¿ÀÈÄ 1:41:50
//  Original author: ysw
///////////////////////////////////////////////////////////

#include <string>

#include "kutilObj.h"

#include "kupaExportCSV.h"
#include "kupaCollectPV.h"
#include "kupaManager.h"

using namespace std;

kupaExportCSV::kupaExportCSV(kupaCollectInterface * pCollect, char *fileName, char *path,
		char* valueOrder, char* format, char* writeMode, char* arg6, char* arg7, char* arg8, char* arg9){

	m_kupaCollectInterface	= pCollect;

	m_fileName	= fileName;
	m_pathName	= path;

	setValueOrder (valueOrder);
	setFormat (format);
	setWriteMode (writeMode);
}


kupaExportCSV::~kupaExportCSV(){

}

void kupaExportCSV::setValueOrder (char *valueOrder) {

	if (0 == strcasecmp(valueOrder, "H")) {
		m_valueOrder	= KUPA_CSV_ORDER_H;
	}
	else {
		m_valueOrder	= KUPA_CSV_ORDER_V;
	}
}

void kupaExportCSV::setFormat (char *format) {

	if (0 == strcasecmp(format, "DOS")) {
		m_format	= KUPA_CSV_FORMAT_DOS;
	}
	else {
		m_format	= KUPA_CSV_FORMAT_UNIX;
	}
}

void kupaExportCSV::setWriteMode (char *writeMode) {

	if (0 == strcasecmp(writeMode, "APPEND")) {
		m_writeMode	= KUPA_CSV_WRITE_APPEND;
	}
	else {
		m_writeMode	= KUPA_CSV_WRITE_NEW;
	}
}


kupaCollectInterface * kupaExportCSV::getCollectInterface(){

	return (m_kupaCollectInterface);
}


int kupaExportCSV::store() {

	if (KUPA_CSV_WRITE_APPEND == getWriteMode()) {
		return (append ());
	}
	else {
		return (storeToNewFile ());
	}
}


int kupaExportCSV::storeToNewFile() {

	kupaManager	*pManager	= kupaManager::getInstance();

	kuDebug (kuMON, "[kupaExportCSV::store] ----------------------------------------\n");
	kuDebug (kuMON, "[kupaExportCSV::store] shot(%ld) stime(%g) duration(%g) rate(%d)\n",
				pManager->getShotNum(), pManager->getStartTime(), pManager->getDuration(), pManager->getSamplingRate());

	// opens file

	FILE	*fp		= NULL;
	char	fname[256];

	snprintf (fname, sizeof(fname)-1, "%s/%s-%05ld%s",
			getPathName().c_str(), getFileName().c_str(), pManager->getShotNum(), KUPA_CSV_EXT_STR);

	if (NULL == (fp = fopen (fname, "w+"))) {
		kuDebug (kuERR, "[kupaExportCSV::store] %s open failed\n", fname);
		return (kuNOK);
	}

	// --------------------------------------
	// information
	// --------------------------------------

	fprintf (fp, "Date, %s\n", kuDateTime::getCurrDateTimeStr());
	fprintf (fp, "Shot Number, %ld\n", pManager->getShotNum());
	fprintf (fp, "Start Time(s), %g\n", pManager->getStartTime());
	fprintf (fp, "Duration(s), %g\n", pManager->getDuration());
	fprintf (fp, "Sampling Rate(Hz), %d\n", pManager->getSamplingRate());
	fprintf (fp, "\n");

	if (KUPA_CSV_ORDER_H == getValueOrder()) {
		storeHorizontal (fp);
	}
	else {
		storeVertical (fp);
	}

	fclose (fp);

	kuDebug (kuMON, "[kupaExportCSV::store] file(%s) was created!!!\n", fname);

	return (kuOK);
}

int kupaExportCSV::storeVertical(FILE *fp) {

	kupaManager					*pManager	= kupaManager::getInstance();
	kupaCollectPVMap				&pvMap		= ((kupaCollectPV *)m_kupaCollectInterface)->getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	// --------------------------------------
	// header
	// --------------------------------------

	fprintf (fp, "Time, ");
	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		fprintf (fp, "%s, ", pos->second->getPVName().c_str());
	}
	fprintf (fp, "\n");

	// --------------------------------------
	// body
	// --------------------------------------

	int	size = 0;
	double timeGap = 1.0 / pManager->getSamplingRate();
	string value;

	if (pvMap.end() != (pos = pvMap.begin())) {
		size = pos->second->getValueSize();
	}

	for (int i = 0; i < size; i++) {
		fprintf (fp, "%g, ", pManager->getStartTime() + i * timeGap);

		for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
			fprintf (fp, "%s, ", pos->second->getValue(i, value).c_str());
		}
		fprintf (fp, "\n");
	}

	return (kuOK);
}

int kupaExportCSV::storeHorizontal(FILE *fp) {

	kupaManager					*pManager	= kupaManager::getInstance();
	kupaCollectPVMap			&pvMap		= ((kupaCollectPV *)m_kupaCollectInterface)->getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	// --------------------------------------
	// header
	// --------------------------------------

	int i;
	int	size = 0;
	double timeGap = 1.0 / pManager->getSamplingRate();

	if (pvMap.end() != (pos = pvMap.begin())) {
		size = pos->second->getValueSize();
	}

	fprintf (fp, "PV Name, Description, Unit, ");

	if (1 == size) {
		fprintf (fp, "Value");
	}
	else {
		for (i = 0; i < size; i++) {
			fprintf (fp, "%g, ", pManager->getStartTime() + i * timeGap);
		}
	}
	fprintf (fp, "\n");

	// --------------------------------------
	// body
	// --------------------------------------

	string value;

	for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
		fprintf (fp, "%s, %s, %s, ",
				pos->second->getPVName().c_str(), pos->second->getDescription().c_str(), pos->second->getUnit().c_str());

		for (i = 0; i < size; i++) {
			fprintf (fp, "%s, ", pos->second->getValue(i, value).c_str());
		}
		fprintf (fp, "\n");
	}

	return (kuOK);
}


int kupaExportCSV::append() {

	kupaManager					*pManager	= kupaManager::getInstance();
	kupaCollectPVMap			&pvMap		= ((kupaCollectPV *)m_kupaCollectInterface)->getkupaCollectPVMap();
	kupaCollectPVMap::iterator	pos;

	kuDebug (kuMON, "[kupaExportCSV::append] ----------------------------------------\n");
	kuDebug (kuMON, "[kupaExportCSV::append] shot(%ld) stime(%g) duration(%g) rate(%d)\n",
				pManager->getShotNum(), pManager->getStartTime(), pManager->getDuration(), pManager->getSamplingRate());

	// opens file

	FILE	*fp		= NULL;
	char	fname[256];
	int		bExist	= kuTRUE;

	snprintf (fname, sizeof(fname)-1, "%s/%s%s", getPathName().c_str(), getFileName().c_str(), KUPA_CSV_EXT_STR);

	bExist	= kuFile::isExist (fname);

	if (NULL == (fp = fopen (fname, "a+"))) {
		kuDebug (kuERR, "[kupaExportCSV::append] %s open failed\n", fname);
		return (kuNOK);
	}

	if (kuTRUE != bExist) {
		// --------------------------------------
		// header
		// --------------------------------------

		// PV Names
		fprintf (fp, "Date, Shot Number, Time, ");
		for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
			fprintf (fp, "%s, ", pos->second->getPVName().c_str());
		}
		fprintf (fp, "\n");

		// Descriptions
		fprintf (fp, ", , , ");
		for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
			fprintf (fp, "%s, ", pos->second->getDescription().c_str());
		}
		fprintf (fp, "\n");

		// Units
		fprintf (fp, ", , , ");
		for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
			fprintf (fp, "%s, ", pos->second->getUnit().c_str());
		}
		fprintf (fp, "\n");
	}

	// --------------------------------------
	// body
	// --------------------------------------

	int	size = 0;
	double timeGap = 1.0 / pManager->getSamplingRate();

	if (pvMap.end() != (pos = pvMap.begin())) {
		size = pos->second->getValueSize();
	}

	string	dateStr	= kuDateTime::getCurrDateTimeStr();
	string	value;

	for (int i = 0; i < size; i++) {
		fprintf (fp, "%s, ", dateStr.c_str());
		fprintf (fp, "%ld, ", pManager->getShotNum());
		fprintf (fp, "%g, ", pManager->getStartTime() + i * timeGap);

		for (pos = pvMap.begin(); pos != pvMap.end(); ++pos ) {
			fprintf (fp, "%s, ", pos->second->getValue(i, value).c_str());
		}
		fprintf (fp, "\n");
	}

	fclose (fp);

	kuDebug (kuMON, "[kupaExportCSV::append] file(%s) was created!!!\n", fname);

	return (kuOK);
}

