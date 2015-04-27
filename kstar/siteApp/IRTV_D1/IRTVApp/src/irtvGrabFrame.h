#ifndef _IRTV_GRAB_FRAME_H
#define _IRTV_GRAB_FRAME_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <direct.h>

#include "irtvCommon.h"
#include "irtvCamInterface.h"
#include "irtvGrabFrameData.h"
#include "irtvGrabFrameShm.h"
#include "irtvGrabStatus.h"
#include "irtvTiff.h"
#include "irtvBMP.h"
#include "irtvCommand.h"

#include "irtvFile.h"
#include "irtvFileSEQ.h"

#include <vector>
#include <algorithm>

#define	IRTV_AVI_PATH			"D:\\IRTV\\AVI"
#define	IRTV_BMP_PATH			"D:\\IRTV\\BMP"

#define	IRTV_AVI_VDUB_HOME		"C:\\epics\\siteApp\\IRTV\\vendor\\VirtualDub"
#define	IRTV_AVI_WPUT_HOME		"C:\\epics\\siteApp\\IRTV\\vendor\\wput"
#define	IRTV_AVI_PLAYER_HOME	"C:\\epics\\siteApp\\IRTV\\vendor\\kmplayer"

using namespace std;

class IrtvGrabFrameMemInfo
{
public :
	IrtvGrabFrameMemInfo () {
		m_nSize	= 0;
		m_pData	= NULL;
	}

	~IrtvGrabFrameMemInfo () {
		//if (NULL != m_pData) {
		//	free (m_pData);
		//	m_pData	= NULL;
		//}
	}

	unsigned int	m_nSize;
	void *			m_pData;
};

class IrtvGrabFrame
{
public :
	IrtvGrabFrame ();

	IrtvGrabFrame (ST_IRTV *pIRTV);

	~IrtvGrabFrame ();

	const int init ();
	const int setup ();
	const int run ();
	const int stop ();
	void clear ();

	const int setupConfig ();
	const int setupGrabber ();
	const int createCamera ();
	const int setupBuffer ();

	const int startGrabFrame ();

	const int loadGrabFrame (unsigned char *data, unsigned int size);

	const bool isLiveRefresh ();

	const int storeGrabFrame (unsigned char *data, unsigned int size);

	const int generateImageFiles ();

	const int convertRawToTIFF ();
	const int convertMemToTIFF ();

	const int convertRawToSEQ ();
	const int convertMemToSEQ ();

	const int convertRawToAVI ();
	const int convertMemToAVI ();

	const int createFile (const bool bCreateRawFile = false);

	const int makeDir (const char *pDirName, bool bForced);

	void setIRTVAddr (ST_IRTV *pIRTV);

	//static const int processLiveData (unsigned short cols, unsigned rows, void *data);

	static const int saveToTIFF (void *data, unsigned short cols, unsigned rows, bool bCompress = true);
	static const int saveToRAW (void *data, unsigned short cols, unsigned rows);

	static void setCameraStatus (ST_IRTV *pIRTV, void *data);

	static void setCameraStatusWithHeader (ST_IRTV *pIRTV, bhpSHeader *pHeader);

	static void printCameraHeader (bhpSHeader *pHeader);

	const int getTotSamps () { return (m_pIRTV->totalRead); };

private :

    unsigned short		m_width;
	unsigned short		m_height;
	UInt32				m_pixelType;
	unsigned int		m_frameSize;
	unsigned short		lDecimationX;
	unsigned short		lDecimationY;
	unsigned short		lBinningX;
	unsigned short		lBinningY;

	GrabFrameData		m_grabFrameData;
	ST_IRTV				*m_pIRTV;
	FILE				*m_rawFp;
	unsigned char		m_convertBuf[IRTV_MAX_BUF_SIZE];
	int					m_nGrabFrameCnt;

	// Camera로부터 취득한 데이터를 파일 대신 메모리에 임시 저장 (속도 개선을 위해)
	vector<IrtvGrabFrameMemInfo>	m_vGrabFrameMemData;

private :

	const bool isRunGrabFrame ();
	void setRunGrabFrame (const bool bRunGrabFrame);

	void setGrabFrameCnt (const unsigned int nGrabFrameCnt); 
	void incGrabFrameCnt ();
	const int getGrabFrameCnt ();

	void updateGrabStatus (unsigned char *data, unsigned int size);
	
	//TODO++
	//CObjectParametersReduceObject *getObjParam ();
	int applyCalibration (const int width, const int height, unsigned char *data);

	inline int calcBuffSize (const int width, const int height) {
		return ( width * (height + 1) * IRTV_RAW_TYPE_SIZE );
	}

	// 이미지 저장 경로 : RAW, TIFF
	char *makeShotImagePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum, const char *extName=NULL);

	// RAW 파일명
	char *makeRawFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum);

	// TIFF 파일명
	char *makeTiffFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum, const int pageNo);

	// SEQ 파일명
	char *makeSeqFilePath (char *pathName, const char *imagePath, const int shotNumber, const int seqNum);

	// AVI 파일명
	char *makeAviPath (char *pathName, const int shotNumber);
	char *makeAviFilePath (char *pathName, const int shotNumber);

	// BMP 파일명
	char *makeBMPPath (char *pathName, const int shotNumber);
	char *makeBMPFilePath (char *pathName, const int shotNumber, const int seqNum);

	// Postfix를 포함한 파일명 생성
	char *makeFileNameWithPostfix (const int shotNumber);

	const bool isStoreGrabFrame ();

	void releaseGrabFrameMem ();

};

#endif // _IRTV_GRAB_FRAME_H
