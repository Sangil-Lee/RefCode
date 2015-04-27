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
		m_nCtrlDeviceId	= -1;
		m_szDirName[0]	= 0x00;
		m_szExtName[0]	= 0x00;
	}

	IrtvCamFile (const int id, const char *dir, const char *ext) {
		setInfo (id, dir, ext);
	}

	~IrtvCamFile () {
		m_vFileName.clear();
	}

	int getNumOfFile () {
		return (m_vFileName.size());
	}

	void setInfo (const int id, const char *dir, const char *ext = NULL) {
		m_nCtrlDeviceId	= id;

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
		return (makeFileList (m_nCtrlDeviceId, m_szDirName, m_szExtName[0] ? m_szExtName : NULL));
	}

	int makeFileList (int id, char *dir, char *ext = NULL) {
		UInt32			blockSize, numBlocks, totalBytes, totalBlocks, iBlock, thisSize;
		char			*data, block[IFS_BLOCK_SIZE];
		int				error = 0, tryNum;
		DIRLIST			dl;
		char			*ent;

		m_vFileName.clear ();

		//	Assert the dir is valid and ends with \.
		if (strlen(dir) == 0)
			return (-1);

		if (dir[strlen(dir) - 1] != '\\')
			strcat(dir, "\\");

		//	Start directory listing
		if ((error = iacfSetIFSDirListHostTransferProlog (id, IFS_BLOCK_SIZE, 0, dir)) != errOk)
			return (-1);

		if ((error = iacfGetIFSDirListHostTransferProlog (id, &blockSize, &numBlocks, dir)) != errOk)
			return (-1);

		debug (4, "[makeFileList] dir(%s) blockSize(%d) numBlocks(%d) \n", dir, blockSize, numBlocks);

		if (blockSize > IFS_BLOCK_SIZE)
			blockSize = IFS_BLOCK_SIZE;

		//	Allocate data for entire listing, use ? as end of list.
		data = (char *)malloc(numBlocks * blockSize + 1);
		memset(data, '?', numBlocks * blockSize + 1);

		//	Transfer blocks
		for (iBlock = 0; iBlock < numBlocks; ++iBlock) {
			thisSize = blockSize;

			//	Receive block from camera (try up to three times)
			for (tryNum = 0; tryNum < 3; ++tryNum) {
				if ((error = iacfSetIFSDirListHostTransferDataBlock (id, thisSize, iBlock, (char *)block)) == 0)
					error = iacfGetIFSDirListHostTransferDataBlock (id, &thisSize, &iBlock, (char *)block);

				if (error == 0)
					break;
			}

			//	Bail on error
			if (error != errOk) {
				free(data);
				iacfGetIFSDirListHostTransferEpilog(id, &totalBytes, &totalBlocks);
				return (-1);
			}

			//	Copy block into data
			memcpy (data + iBlock * blockSize, block, thisSize);
		}

		//	End directory list
		iacfGetIFSDirListHostTransferEpilog(id, &totalBytes, &totalBlocks);

		debug (3, "[OpenDir] iBlock(%d) totBytes(%d) totBlocks(%d)\n", iBlock, totalBytes, totalBlocks);

		UInt32	fileSizeInBytes;
		UInt32	timeLastModified;
		UInt32	timeLastAccessed;
		char	path[64];
		char	name[64];
		char	bufTime[64];
		char	str[64];
		char	*ptr;
		time_t	epochTime;
		struct tm	*tp;

		dl.data	= data;
		dl.pos	= data;

		//	Print each entry
		while (1) {
			ent = dl.pos;
			
			//	? means end of list
			if (*ent == '?')
				break;

			//	Skip to the end of this entry, and then to the next
			for (; *(dl.pos) != 0; ++dl.pos);
			++dl.pos;

			if (NULL == ent) {
				break;
			}

			if ( (NULL == ext) || (NULL != strstr((const char *)ent, ext)) ) {
				sprintf (path, "%s/%s", "nuc", ent);

				iacfSetIFSActiveFile (id, path);
				iacfGetIFSFileInfo (id, &fileSizeInBytes, &timeLastModified, &timeLastAccessed);

				// 1900/1/1 시각을 1970/1/1 시각으로 환산한 뒤, 문자열로 변환
				epochTime	= timeLastModified - IRTV_BASE_EPOCH - IRTV_LOCAL_EPOCH;
				tp = localtime (&epochTime);

				sprintf (bufTime, "%4d/%02d/%02d %02d:%02d", 
					tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min);

				debug (3, "%s : size(%u) modified(%u) accessed(%u) time(%s)\n",
					ent, fileSizeInBytes, timeLastModified, timeLastAccessed, bufTime);

				strcpy (name, ent);
				if (NULL != (ptr = strrchr (name, '.'))) {
					name[strlen(name)-strlen(ptr)]	= 0x00;
				}

				sprintf (str, "%s,%s", name, bufTime);

				m_vFileName.push_back (str);
			}
		}

		//	free data
		free(data);

		return (m_vFileName.size());
	}

protected :

	int			m_nCtrlDeviceId;
	char		m_szDirName[128];
	char		m_szExtName[64];

	vector<string>	m_vFileName;

};

#endif	// _IRTV_CAM_FILE_H