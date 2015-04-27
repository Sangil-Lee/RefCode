#ifndef _IRTV_BMP_H
#define _IRTV_BMP_H

#include "IACFSDK.h"

#include "irtvGrabFrameData.h"

#if defined(WIN32)
#define	IRTV_IMG_BYTES		3		// for QImage::Format_RGB888
#else
#define	IRTV_IMG_BYTES		4		// for QImage::Format_RGB32
#endif

class IrtvBMP
{
public :

	IrtvBMP ();
	~IrtvBMP ();

	void init ();

	const int open (const int nShotNum, UInt16 width, UInt16 height);

	void close ();

	const int write (const char *pszBmpFileName, UInt16 *data);
	inline const double getIrigTime ();
	void makeDateTime ();

	void convertFrameToRGB (unsigned short *pFrameBuffer, unsigned char *pBits, int width, int height);
	const int saveBMPToFile (const char *fname,  unsigned char *pBits, int nWidth, int nHeight, int nPitch);

private :

	Int32		m_nShotNum;
	UInt16		m_width;
	UInt16		m_height;
	UInt16		m_nPitch;
	UInt16		m_nPageNo;
	double		m_dFirstFrameIrigTime;
	double		m_dCurrFrameIrigTime;
	char		m_szFrameStartTime[24];
	iacfSHeader	m_stFrameHeader;

	unsigned int	m_Hist[65536];
	unsigned short	m_frameBuffer[IRTV_MAX_BMP_BUF_SIZE];
};

#endif	// _IRTV_BMP_H