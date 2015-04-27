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

	// �̱���
	const int write (UInt16 *pFrameBuf);

	// Original �����͸� �̿��Ͽ� SEQ ���Ͽ� �̹��� ������ ���
	const int write (CCamera &camera, SImageData &image, UInt8 preset);

	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir);

	// Raw �����͸� �̿��Ͽ� SEQ ���Ͽ� �̹��� ������ ���
	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir, bhpSCalInfo *calib);

	const int close ();

private :

	IImagerFilePtr	m_pSEQFile;

};

#endif	// _IRTV_FILE_SEQ_H