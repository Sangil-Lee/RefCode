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
	const int open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *pFileName, iacfCalib calib);

	const int write (UInt16 *pFrameBuf);

	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir);
	const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir, iacfCalib calib);

	const int close ();

private :

	iacfFile	m_pSEQFile;

};

#endif	// _IRTV_FILE_SEQ_H