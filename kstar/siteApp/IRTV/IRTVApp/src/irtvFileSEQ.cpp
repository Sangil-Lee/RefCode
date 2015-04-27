//#include "stdafx.h"

#include <time.h>
#include <stdlib.h>

#include "IrtvFileSEQ.h"

IrtvFileSEQ::IrtvFileSEQ () 
{ 
	init ();
}

IrtvFileSEQ::~IrtvFileSEQ () 
{
	close ();
}

void IrtvFileSEQ::init () 
{
	m_pSEQFile	= NULL;
}

const int IrtvFileSEQ::open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName) 
{
	return (open (nShotNum, nWidth, nHeight, pFileName, NULL));
}

const int IrtvFileSEQ::open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName, iacfCalib calib) 
{
	close ();

	setShotNum (nShotNum);
	setWidth (nWidth);
	setHeight (nHeight);

	if (NULL == (m_pSEQFile = iacfFileCreate (pFileName, nWidth, nHeight, calib))) {
		debug (2, "[SEQ::open] create failed : %s\n", pFileName);
		return (-1);
	}

	return (0);
}

const int IrtvFileSEQ::close () 
{
	if (m_pSEQFile) {
		iacfFileClose (m_pSEQFile);
		m_pSEQFile = NULL;
	}

	init ();

	return (0);
}

const int IrtvFileSEQ::write (UInt16 *pFrameBuf) 
{
	// Frame 내의 헤더 정보 얻기
	iacfDecodeHeader (&m_stFrameHeader, pFrameBuf);

	// RAW Data를 이용하여 SEQ 파일 생성
	if (true != iacfFileWrite (m_pSEQFile, (UInt16 *)pFrameBuf)) {
		debug (2, "[SEQ::write] Error : iacfFileWrite failed \n");
		return (-1);
	}

	return (0);
}

const int IrtvFileSEQ::convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir) 
{
	return ( convert (nWidth, nHeight, pSrcRawFilePath, pDstSEQFileDir, NULL) );
}

const int IrtvFileSEQ::convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir, iacfCalib calib) 
{
	// 파일 열기
	FILE	*pRawFp		= NULL;
	int		nImageSize	= nWidth * (nHeight + 1) * 2;
	UInt32	nFileSize;
	int		nFrames;

	debug (3, "[SEQ::convert] nImageSize(%d) nWidth(%d) nHeight(%d)\n", nImageSize, nWidth, nHeight);

	if (NULL == (pRawFp = fopen (pSrcRawFilePath, "rb"))) {
		debug (2, "[SEQ::convert] Error : open failed for %s\n", pSrcRawFilePath);
		return (-1);
	}

	fseek (pRawFp, 0, SEEK_END);
	nFileSize = (UInt32)ftell (pRawFp);
	fseek (pRawFp, 0, SEEK_SET);

	if (nFileSize % nImageSize) {
		fclose (pRawFp);
		debug (2, "[SEQ::convert] Error : file size / image size mismatch\n", pSrcRawFilePath);
		return (-1);
	}

	nFrames = nFileSize / nImageSize;

	char	*pFrameBuf	= (char *)malloc(nImageSize);
	if (!pFrameBuf) {
		fclose (pRawFp);
		debug (2, "[SEQ::convert] malloc failed\n");
		return (-1);
	}

	char	dir[256];
	char	name[256];
	char	dstFileName[256];

	dir[0] = name[0]	= 0x00;
	getDirFileName (pDstSEQFileDir, dir, name);

	if (!strcmp (name, "")) {
		getFileName (pSrcRawFilePath, name);
		sprintf (dstFileName, "%s\\%s.seq", dir, name);
	}
	else {
		strcpy (dstFileName, pDstSEQFileDir);
	}

	debug (3, "[SEQ::convert] RAW(%s)\n", pSrcRawFilePath);
	debug (3, "[SEQ::convert] SEQ(%s)\n", dstFileName);

	iacfFile	pSEQFile;
	bool		bError	= false;
	int			i;

	if (NULL == (pSEQFile = iacfFileCreate (dstFileName, nWidth, nHeight, calib))) {
		fclose (pRawFp);
		debug (2, "[SEQ::convert] create failed : %s\n", dstFileName);
		return (-1);
	}

	bError	= false;

	for (i = 0; i < nFrames; ++i) {
		// 파일로부터 RAW Data 읽기
		if (1 != fread (pFrameBuf, nImageSize, 1, pRawFp)) {
			debug (2, "[SEQ::convert] Error : fread failed\n");
			bError	= true;
			break;
		}

		// RAW Data를 이용하여 SEQ 파일 생성
		if (true != iacfFileWrite (pSEQFile, (UInt16 *)pFrameBuf)) {
			debug (2, "[SEQ::convert] Error : iacfFileWrite failed \n");
			bError	= true;
			break;
		}
	}

	iacfFileClose (pSEQFile);

	fclose (pRawFp);
	pRawFp	= NULL;

	if (!bError) {
		debug (3, "[SEQ::convert] %s created !!!\n", dstFileName);
		return (0);
	}

	return (0);
}
