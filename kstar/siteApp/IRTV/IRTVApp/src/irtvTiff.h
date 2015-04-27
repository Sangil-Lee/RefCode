#ifndef _IRTV_TIFF_H
#define _IRTV_TIFF_H

#include "IACFSDK.h"

#include "tiffio.h"

class IrtvTiff
{
public :

	IrtvTiff ();
	~IrtvTiff ();

	void init ();

	const int open (const int nShotNum, const char *fname, UInt16 width, UInt16 height, bool bMultiPage, bool bCompress);

	void close ();

	const int write (UInt16 *data);
	const bool isMultiPage ();
	const bool isCompress ();
	inline const double getIrigTime ();
	void makeDateTime ();

private :

	TIFF *		m_pTiff;
	bool		m_bMultiPage;
	bool		m_bCompress;
	Int32		m_nShotNum;
	UInt16		m_width;
	UInt16		m_height;
	UInt16		m_nPageNo;
	double		m_dFirstFrameIrigTime;
	double		m_dCurrFrameIrigTime;
	char		m_szFrameStartTime[24];
	iacfSHeader	m_stFrameHeader;
};

#endif	// _IRTV_TIFF_H