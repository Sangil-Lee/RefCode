#ifndef _IRTV_CAM_FILE_H
#define _IRTV_CAM_FILE_H

#include "irtvFile.h"

#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

#define	IRTV_BASE_EPOCH		2208988800L		// 1900/1/1 ~ 1970/1/1 까지의 초
#define	IRTV_LOCAL_EPOCH	(9 * 60 * 60)	// local time 반영 문제로 인하여, 9시간이 상이함

typedef struct {
	char	*data;
	char	*pos;
} DIRLIST;

class IrtvCamFile : public IrtvFile
{
public :

	static const int IFS_BLOCK_SIZE	= 128;

	IrtvCamFile () {
		m_resHandle		= NULL;
		m_szDirName[0]	= 0x00;
		m_szExtName[0]	= 0x00;
	}

	IrtvCamFile (const resHandle h, const char *dir, const char *ext) {
		setInfo (h, dir, ext);
	}

	~IrtvCamFile () {
		m_vFileName.clear();
	}

	int getNumOfFile () {
		return (m_vFileName.size());
	}

	void setInfo (const resHandle h, const char *dir, const char *ext = NULL) {
		m_resHandle	= h;

		//	Convert all / to \.
		convertDelimiter(dir, m_szDirName, '/', '\\');

		if (ext) {
			strcpy (m_szExtName, ext);
		}
		else {
			m_szExtName[0]	= 0x00;
		}
	}

	vector<string> &getFileList () {
		return (m_vFileName);
	}


	int makeFileList () {
		return (makeFileList (m_resHandle, m_szDirName, m_szExtName[0] ? m_szExtName : NULL));
	}

	int makeFileList (resHandle h, char *dir, char *ext = NULL) {
		resDIRLIST	*dl = NULL;
		const char	*fname;
		tcUInt8		attrib;
		tcUInt32	size;
		tcUInt64	timeLastModified;
		time_t		epochTime;
		char		bufTime[64];
		char		name[64];
		char		str[64];
		char		*ptr;
		struct tm	*tp = NULL;
		
		m_vFileName.clear ();

		if (bhpListNUCs (h, &dl) == reserrOk) {
			while (resDirRead(dl, &fname, &attrib, &size, &timeLastModified) == reserrOk) {
				epochTime	= timeLastModified / 100000000;
				tp = localtime (&epochTime);

				sprintf (bufTime, "%4d/%02d/%02d %02d:%02d", 
					tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min);

				debug (4, "%s : size(%d) time(%s)\n", fname, size, bufTime);

				strcpy (name, fname);

				if (NULL != (ptr = strrchr (name, '.'))) {
					name[strlen(name)-strlen(ptr)]	= 0x00;
				}

				sprintf (str, "%s,%s", name, bufTime);

				m_vFileName.push_back (str);
			}

			resDirClose(dl);
			dl = NULL;
		}

		return (m_vFileName.size());
	}

protected :

	resHandle	m_resHandle;
	char		m_szDirName[128];
	char		m_szExtName[64];

	vector<string>	m_vFileName;

};

#endif	// _IRTV_CAM_FILE_H