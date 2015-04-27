#ifndef _IRTV_FILE_SEQ_H
#define _IRTV_FILE_SEQ_H

#include "irtvImageFile.h"

class IrtvFileSEQ : public IrtvImageFile
{
public :

	IrtvFileSEQ ();
	virtual ~IrtvFileSEQ ();

	void init ();

	const int open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName);
	const int open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName, bhpSCalInfo *calib);

	// 미구현
	const int write (UInt16 *pFrameBuf);

	// Original 데이터를 이용하여 SEQ 파일에 이미지 프레임 기록
	const int write (CCamera &camera, SImageData &image, UInt8 preset);

	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir);

	// Raw 데이터를 이용하여 SEQ 파일에 이미지 프레임 기록
	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir, bhpSCalInfo *calib);

	const int close ();

private :

	IImagerFilePtr	m_pSEQFile;

};

#endif	// _IRTV_FILE_SEQ_H