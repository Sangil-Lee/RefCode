//#include "stdafx.h"

#include <time.h>
#include <stdlib.h>

#include "IrtvFileSEQ.h"
#include "IrtvCommand.h"

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

const int IrtvFileSEQ::open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName, bhpSCalInfo * calib) 
{
	close ();

	setShotNum (nShotNum);
	setWidth (nWidth);
	setHeight (nHeight);

	CString		fname = pFileName;
	TCTCHAR		*wname = fname.GetBuffer();

	m_pSEQFile = ImagerFileFactory->CreateImagerFile (FileSystem, wname, IrtvCommand::getInstance().camera.ReduceObjects);

	if (NULL == m_pSEQFile) {
		debug (2, "[SEQ::open] create failed : %s\n", pFileName);
		return (-1);
	}

	return (0);
}

const int IrtvFileSEQ::close () 
{
	if (m_pSEQFile) {
		m_pSEQFile->Close ();
		m_pSEQFile = NULL;
	}

	init ();

	return (0);
}

// 미구현
const int IrtvFileSEQ::write (UInt16 *pFrameBuf) 
{
	return (-1);
}

// Original을 이용하여 SEQ 파일에 이미지 프레임 기록
const int IrtvFileSEQ::write (CCamera &camera, SImageData &image, UInt8 preset) 
{
	// Original Data를 이용하여 SEQ 파일 생성
	if (true != m_pSEQFile->PutFrame (image.data.pData, preset, camera.ReduceObjects)) {
		debug (2, "[SEQ::write] Error : PutFrame failed \n");
		return (-1);
	}

	return (0);
}

const int IrtvFileSEQ::convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir) 
{
	return ( convert (nWidth, nHeight, pSrcRawFilePath, pDstSEQFileDir, NULL) );
}

const int IrtvFileSEQ::convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir, bhpSCalInfo * calib) 
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

	IImagerFilePtr	pSEQFile;
	SImageData	image;
	bool		bError	= false;
	int			i;
	CString		fname = dstFileName;
	TCTCHAR		*wname = fname.GetBuffer();
	UInt8		preset;

	pSEQFile = ImagerFileFactory->CreateImagerFile (FileSystem, wname, IrtvCommand::getInstance().camera.ReduceObjects);

	if (NULL == pSEQFile) {
		free (pFrameBuf);
		fclose (pRawFp);
		debug (2, "[SEQ::convert] create failed : %s\n", dstFileName);
		return (-1);
	}

	bError	= false;
	IrtvCommand::getInstance().camera.Controller->GetActivePreset (preset);

	for (i = 0; i < nFrames; ++i) {
		// 파일로부터 RAW Data 읽기
		if (1 != fread (pFrameBuf, nImageSize, 1, pRawFp)) {
			debug (2, "[SEQ::convert] Error : fread failed\n");
			bError	= true;
			break;
		}

		// convert the raw data into original data (filtered, updates reduce objects)
		if (true != IrtvCommand::getInstance().camera.RawToOriginal (pFrameBuf, nImageSize, image, preset)) {
			debug (2, "[SEQ::convert] Error : RawToOriginal failed \n");
			bError	= true;
			break;
		}

		// Original Data를 이용하여 SEQ 파일 생성
		if (true != pSEQFile->PutFrame (image.data.pData, preset, IrtvCommand::getInstance().camera.ReduceObjects)) {
			debug (2, "[SEQ::convert] Error : PutFrame failed \n");
			bError	= true;
			break;
		}
	}

	pSEQFile->Close ();
	pSEQFile = NULL;

	fclose (pRawFp);
	pRawFp	= NULL;

	free (pFrameBuf);

	if (!bError) {
		debug (3, "[SEQ::convert] %s created !!!\n", dstFileName);
		return (0);
	}

	return (-1);
}
