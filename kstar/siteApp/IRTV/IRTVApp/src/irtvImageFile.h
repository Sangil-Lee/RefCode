#ifndef _IRTV_IMAGE_FILE_H
#define _IRTV_IMAGE_FILE_H

#include "irtvFile.h"

class IrtvImageFile : public IrtvFile
{
public :

	IrtvImageFile () {
		m_nShotNum	= 0;
		m_nWidth	= 0;
		m_nHeight	= 0;
	}

	virtual ~IrtvImageFile () {
		printf ("[IrtvFile::~IrtvFile] Please implement code\n");
	}

	virtual const int open (const int nShotNum, UInt16 nWidth, UInt16 nHeight, const char *fname) {
		printf ("[IrtvFile::open] Please implement code\n");
		return (-1);
	}

	virtual const int write (UInt16 *data) {
		printf ("[IrtvFile::write] Please implement code\n");
		return (-1);
	}

	virtual const int convert (int nWidth, int nHeight, char *pSrcRawFilePath, char *pDstSEQFileDir) {
		printf ("[IrtvFile::convert] Please implement code\n");
		return (-1);
	}

	virtual const int close () {
		printf ("[IrtvFile::close] Please implement code\n");
		return (-1);
	}

	inline const double getIrigTime (iacfSHeader stFrameHeader) {
		return ( 
			stFrameHeader.IRIGTime.Millisecond 
				+ stFrameHeader.IRIGTime.Second * 1000
				+ stFrameHeader.IRIGTime.Minute * 1000 * 60
				+ stFrameHeader.IRIGTime.Hour   * 1000 * 3600
		);
	}

	void makeDateTime () {
		getDateTime (m_szFrameStartTime, sizeof(m_szFrameStartTime));
	}

	const Int32 getShotNum () { return (m_nShotNum); }
	void setShotNum (const Int32 nShotNum) { m_nShotNum = nShotNum; }

	const UInt16 getWidth () { return (m_nWidth); }
	void setWidth (const UInt16 nWidth) { m_nWidth = nWidth; }

	const UInt16 getHeight () { return (m_nHeight); }
	void setHeight (const UInt16 nHeight) { m_nHeight = nHeight; }

protected :

	Int32		m_nShotNum;
	UInt16		m_nWidth;
	UInt16		m_nHeight;

	double		m_dFirstFrameIrigTime;
	double		m_dCurrFrameIrigTime;
	char		m_szFrameStartTime[24];
	iacfSHeader	m_stFrameHeader;
};

#endif	// _IRTV_IMAGE_FILE_H