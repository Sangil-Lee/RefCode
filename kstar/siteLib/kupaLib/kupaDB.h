///////////////////////////////////////////////////////////
//  kupaDB.h
//  Implementation of database access
//  Created on:      09-5-2013 오후 1:41:55
//  Original author: ysw
///////////////////////////////////////////////////////////

/**
 * \file    kupaDB.h
 * \ingroup kupa
 * \brief   EPICS DB 관련 함수 정의
 * 
 * 본 파일은, EPICS에서 제공하는 Static 및 Dynamic DB 접근 라이브러리들에 대한 wrapper 함수를 정의한다.
 */

#if !defined(__KUPA_DB_H)
#define __KUPA_DB_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *	kupapDbEntry;
typedef void *	kupapDbAddr;

// ------------------------------------
// static library for database
// ------------------------------------

extern kupapDbEntry kupaDbAllocEntry ();										///< Static. dbAllocEntry
extern void kupaDbFreeEntry (kupapDbEntry pdbentry);							///< Static. dbFreeEntry
extern int kupaDbInitEntry (kupapDbEntry pdbentry);								///< Static. dbInitEntry
extern int kupaDbFinishEntry (kupapDbEntry pdbentry);							///< Static. dbFinishEntry
extern int kupaDbIsValidEntry (kupapDbEntry pdbentry);							///< Static. kuOK
extern int kupaDbFindRecord (kupapDbEntry pdbentry, const char *pRecordName);	///< Static. dbFindRecord
extern char * kupaDbGetString (kupapDbEntry pdbentry, char *pdata);				///< Static. dbGetString

// ------------------------------------
// dynamic library for database
// ------------------------------------

extern kupapDbAddr kupaDbAllocAddr ();											///< Dynamic. calloc(DBADDR)
extern void kupaDbFreeAddr (kupapDbAddr pdbaddr);								///< Dynamic. free
extern int kupaDbInitAddr (kupapDbAddr pdbaddr, const char *pRecordName);		///< Dynamic. dbNameToAddr
extern int kupaDbFinishAddr (kupapDbAddr pdbaddr);								///< Dynamic. kuOK
extern int kupaDbIsValidAddr (kupapDbAddr pdbaddr);								///< Dynamic. DBADDR 검사
extern char * kupaDbGetField (kupapDbAddr pdbaddr, char *pdata);				///< Dynamic. dbGetField(DBR_STRING)
extern double kupaDbGetValue (kupapDbAddr pdbaddr);								///< Dynamic. dbGetField(DBR_DOUBLE)

#ifdef __cplusplus
}
#endif

#endif // __KUPA_DB_H
