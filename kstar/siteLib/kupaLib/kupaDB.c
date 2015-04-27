///////////////////////////////////////////////////////////
//  kupaDB.c
//  Wrapper for database access such as static and dynamic
//  Created on:      09-5-2013 ¿ÀÈÄ 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////


#include "kutil.h"
#include "kupaDB.h"

#include "dbAccess.h"
#include "dbStaticLib.h"

// ------------------------------------
// static library for database
// ------------------------------------

kupapDbEntry kupaDbAllocEntry ()
{
	DBENTRY *pdbentry = dbAllocEntry(pdbbase);

	if (NULL == pdbentry) {
		kuDebug (kuERR, "[kupaDbAllocEntry] dbAllocEntry() was failed \n");
	}
	else {
		kuDebug (kuDEBUG, "[kupaDbAllocEntry] dbAllocEntry() was succeed \n");
	}

	return ((kupapDbEntry) pdbentry);
}

void kupaDbFreeEntry (kupapDbEntry pdbentry)
{
	if (NULL != pdbentry) {
		dbFreeEntry ((DBENTRY *)pdbentry);
	}
}

int kupaDbInitEntry (kupapDbEntry pdbentry)
{
	if (NULL == pdbentry) {
		kuDebug (kuERR, "[kupaDbInitEntry] entry is invaild\n");
		return (kuNOK);
	}

	dbInitEntry (pdbbase, (DBENTRY *)pdbentry);

	return (kuOK);
}

int kupaDbFinishEntry (kupapDbEntry pdbentry)
{
	if (NULL != pdbentry) {
		dbFinishEntry ((DBENTRY *)pdbentry);
	}

	return (kuOK);
}

int kupaDbIsValidEntry (kupapDbEntry pdbentry)
{
	return (kuTRUE);
}

// PVNAME or PVNAME.VAL
int kupaDbFindRecord (kupapDbEntry pdbentry, const char *pRecordName)
{
	if ( (NULL != pdbentry) && (NULL != pRecordName) ) {
		if (kupaDbIsValidEntry(pdbentry)) {
			if (0 == dbFindRecord (pdbentry, pRecordName)) {
				kuDebug (kuDEBUG, "[kupaDbFindRecord] dbFindRecord(%s) was found\n", pRecordName);
				return (kuOK);
			}
			else {
				kuDebug (kuINFO, "[kupaDbFindRecord] dbFindRecord(%s) was failed\n", pRecordName);
			}
		}
	}

	return (kuNOK);
}

// return value of field
char * kupaDbGetString (kupapDbEntry pdbentry, char *pdata)
{
	if ( (NULL != pdbentry) && (NULL != pdata) ) {
		if (kupaDbIsValidEntry(pdbentry)) {
			strcpy (pdata, dbGetString (pdbentry));
			kuDebug (kuDEBUG, "[kupaDbGetString] %s\n", pdata);
		}
	}

	return (pdata);
}

// ------------------------------------
// dynamic library for database
// ------------------------------------

kupapDbAddr kupaDbAllocAddr ()
{
	return ((kupapDbAddr) calloc(1, sizeof(DBADDR)));
}

void kupaDbFreeAddr (kupapDbAddr pdbaddr)
{
	if (NULL != pdbaddr) {
		free (pdbaddr);
		pdbaddr = NULL;
	}
}

int kupaDbInitAddr (kupapDbAddr pdbaddr, const char *pRecordName)
{
	if ( (NULL != pdbaddr) && (NULL != pRecordName) ) {
		if (kupaDbIsValidAddr(pdbaddr)) {
			if (0 != dbNameToAddr (pRecordName, (DBADDR *)pdbaddr)) {
				kuDebug (kuERR, "[kupaDbInitAddr] dbNameToAddr(%s) failed\n", pRecordName);

				kupaDbFreeAddr (pdbaddr);
				return (kuNOK);
			}
		}
	}

	return (kuOK);
}

int kupaDbFinishAddr (kupapDbAddr pdbaddr)
{
	return (kuOK);
}

int kupaDbIsValidAddr (kupapDbAddr pdbaddr)
{
	return ( pdbaddr ? kuTRUE : kuFALSE );
}

// return value of field using dbGetField()
char * kupaDbGetField (kupapDbAddr pdbaddr, char *pdata)
{
	long nElements = 1;
	long options = 0;

	if ( (NULL != pdbaddr) && (NULL != pdata) ) {
		if (kupaDbIsValidAddr(pdbaddr)) {
			dbGetField (pdbaddr, DBR_STRING, pdata, &options, &nElements, NULL);
		}
	}

	return (pdata);
}

double kupaDbGetValue (kupapDbAddr pdbaddr)
{
	long nElements = 1;
	long options = 0;
	double dValue = 0;

	if (NULL != pdbaddr) {
		if (kupaDbIsValidAddr(pdbaddr)) {
			dbGetField (pdbaddr, DBR_DOUBLE, &dValue, &options, &nElements, NULL);
		}
	}

	return (dValue);
}

