#ifndef _IRTV_FILE_H
#define _IRTV_FILE_H

#include "IACFSDK.h"
#include "iacfUtil.h"

#include "irtvTypes.h"

#if !defined(_WINDOWS)
#include "irtvLog.h"
#endif

#include <epicsTime.h>

extern void debugToProcessList (const char *msg);

class IrtvFile
{
public :

	IrtvFile () {
	}

	virtual ~IrtvFile () {
		printf ("[IrtvFile::~IrtvFile] Please implement code\n");
	}

	void getDateTime (char *pszTimeStr, const int nBufSize) {
		if (pszTimeStr) {
			epicsTimeStamp epics_time;

			epicsTimeGetCurrent (&epics_time);
			epicsTimeToStrftime (pszTimeStr, nBufSize, "%Y:%m:%d %H:%M:%S", &epics_time);
		}
	}

	const int getFileName (const char *pPath, char *pFileName) {
		if ( (NULL == pPath) || (NULL == pFileName) ) {
			return (-1);
		}

		getDirFileName (pPath, NULL, pFileName);

		char	*ptr = NULL;
		ptr = (char *)strrchr (pFileName, '.');

		if (NULL != ptr) {
			int		nNameLen = strlen(pFileName) - strlen(ptr);
			pFileName[nNameLen] = 0x00;
		}

		return (0);
	}

	// seperate directory and file name from path
	const int getDirFileName (const char *pPath, char *pDirName, char *pFileName) {
		if (NULL == pPath) {
			return (-1);
		}

		char	path[256];
		convertPath (pPath, path);

		char	*ptr = NULL;

		ptr = (char *)strrchr (path, '/');

		if (NULL == ptr) {
			if (pDirName)	pDirName[0] = 0x00;
			if (pFileName)	strcpy (pFileName, path);
		}
		else {
			int		nDirNameLen = strlen(path) - strlen(ptr);

			if (pDirName) {
				strncpy (pDirName, path, nDirNameLen);
				pDirName[nDirNameLen] = 0x00;
			}

			if (pFileName)	strcpy (pFileName, ptr+1);
		}

		return (0);
	}

	const char *convertPath (const char *src, char *dest) {
		return ( convertDelimiter (src, dest, '\\', '/') );
	}

	// convert path delimiter
	const char *convertDelimiter (const char *src, char *dest, const UInt8 origin, const UInt8 newer) {
		if ((NULL == src) || (NULL == dest)) {
			return (NULL);
		}

		UInt32	i;
		for (i = 0; i < strlen(src); i++) {
			dest[i]	= (origin == src[i]) ? newer : src[i];
		}
		dest[i]	= 0x00;

		return (dest);
	}

	void debug (const int nDebugLevel, const char *fmt, ... ) {
		char buf[1024], bufTime[64], msg[1024];
		va_list argp;
		epicsTimeStamp epics_time;

		va_start(argp, fmt);
		vsprintf_s(buf, sizeof(buf)-1, fmt, argp);
		va_end(argp);

		epicsTimeGetCurrent (&epics_time);
		epicsTimeToStrftime (bufTime, sizeof(bufTime), "%m/%d %H:%M:%S.%03f", &epics_time);

		sprintf (msg, "[%s] %s", bufTime, buf);

#if defined(_WINDOWS)
		debugToProcessList (msg);
#else
		kLog (nDebugLevel, msg);
#endif
	}

};

#endif	// _IRTV_FILE_H