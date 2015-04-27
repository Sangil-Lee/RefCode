#include <time.h>
#include <stdlib.h>

#include <epicsTime.h>

#include "irtvCommon.h"
#include "irtvTiff.h"

#pragma comment(lib, "libtiff.lib")

IrtvTiff::IrtvTiff () 
{ 
	init ();
}

IrtvTiff::~IrtvTiff () 
{
	this->close ();
}

void IrtvTiff::init () 
{
	m_pTiff			= NULL;
	m_width			= 0;
	m_height		= 0;
	m_bMultiPage	= false;
	m_bCompress		= true;
	m_nPageNo		= 0;
}

const int IrtvTiff::open (const int nShotNum, const char *fname, UInt16 width, UInt16 height, bool bMultiPage, bool bCompress) 
{
	init ();

	if (NULL == (m_pTiff = TIFFOpen (fname, "w"))) {
		printf ("TIFFOpen failed \n");
		return (-1);
	}

	m_nShotNum		= nShotNum;
	m_width			= width;
	m_height		= height;
	m_bMultiPage	= bMultiPage;
	m_bCompress		= bCompress;

	makeDateTime ();

	return (0);
}

void IrtvTiff::close () 
{
	if (NULL != m_pTiff) {
		TIFFClose (m_pTiff);
	}

	init ();
}

void IrtvTiff::makeDateTime ()
{
	epicsTimeStamp epics_time;

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (m_szFrameStartTime, sizeof(m_szFrameStartTime), "%Y:%m:%d %H:%M:%S", &epics_time);
}

const int IrtvTiff::write (UInt16 *data) 
{
	// Frame ���� ��� ���� ���
	bhpDecodeHeader (&m_stFrameHeader, data);

	if (0 == m_nPageNo) {
		// 1st Frame�� �ð� ���
		m_dFirstFrameIrigTime	= getIrigTime ();
	}

	// Frame Number ����
	m_nPageNo	+= 1;

	// ���� Frame�� �ð� ���
	m_dCurrFrameIrigTime	= getIrigTime ();

	char	szShotNum[64];
	char	szOffsetTime[64];
	char	szDateTime[24];
	int		nOffsetTime	= (int)(m_dCurrFrameIrigTime - m_dFirstFrameIrigTime);

	sprintf (szShotNum, "%06d", m_nShotNum);
	// MATLAB���� �ν��� �� �ֵ��� ���� ���� : HH:MM:SS.mmm
	sprintf (szOffsetTime, "00:%02d:%02d.%03d", nOffsetTime / (1000 * 60), (nOffsetTime % (1000 * 60)) / 1000, nOffsetTime % 1000);
	sprintf (szDateTime, "%-19s", szOffsetTime);

	TIFFSetField (m_pTiff, TIFFTAG_IMAGEWIDTH		, m_width);
	TIFFSetField (m_pTiff, TIFFTAG_IMAGELENGTH		, m_height);
	TIFFSetField (m_pTiff, TIFFTAG_BITSPERSAMPLE	, 16);
	TIFFSetField (m_pTiff, TIFFTAG_SAMPLESPERPIXEL	, 1);
	TIFFSetField (m_pTiff, TIFFTAG_PLANARCONFIG		, PLANARCONFIG_CONTIG);
	TIFFSetField (m_pTiff, TIFFTAG_PHOTOMETRIC		, PHOTOMETRIC_MINISBLACK);
	TIFFSetField (m_pTiff, TIFFTAG_ORIENTATION		, ORIENTATION_TOPLEFT);

	if (true == isCompress ()) {
		// LZW (Lempelziv welch) : ���ս� ���� : 642KB -> 351KB
		TIFFSetField (m_pTiff, TIFFTAG_COMPRESSION	, COMPRESSION_LZW);
	}

	// multi-page
	if (true == isMultiPage ()) {
		TIFFSetField (m_pTiff, TIFFTAG_SUBFILETYPE	, FILETYPE_PAGE);
	}
	
	TIFFSetField (m_pTiff, TIFFTAG_ARTIST			, sfwEnvGet("IRTV_TIFF_ARTIST"));		// ������
	TIFFSetField (m_pTiff, TIFFTAG_COPYRIGHT		, sfwEnvGet("IRTV_TIFF_COPYRIGHT"));	// ���۱�
	TIFFSetField (m_pTiff, TIFFTAG_HOSTCOMPUTER		, sfwEnvGet("IRTV_TIFF_HOSTCOMPUTER"));	// ��ǻ��
	TIFFSetField (m_pTiff, TIFFTAG_SOFTWARE			, sfwEnvGet("IRTV_TIFF_SOFTWARE"));		// ���α׷� �̸� & ����
	TIFFSetField (m_pTiff, TIFFTAG_MAKE				, sfwEnvGet("IRTV_TIFF_MAKE"));			// ī�޶� ������ü
	TIFFSetField (m_pTiff, TIFFTAG_MODEL			, sfwEnvGet("IRTV_TIFF_MODEL"));		// ī�޶� ��
	TIFFSetField (m_pTiff, TIFFTAG_DOCUMENTNAME		, szShotNum);							// Document Name
	TIFFSetField (m_pTiff, TIFFTAG_PAGENAME			, szOffsetTime);						// Document Page name
	TIFFSetField (m_pTiff, TIFFTAG_PAGENUMBER		, m_nPageNo);							// Page ��ȣ
	TIFFSetField (m_pTiff, TIFFTAG_DATETIME			, szDateTime);							// ���� IRIG ���� �ɼ� : NULL ���� 20�ڸ� ("HH:MM:SS.mmm")

	for (int i = 0; i < m_height; ++i) {
		TIFFWriteScanline (m_pTiff, data + m_width * i, i, 0);
	}

	if (true == isMultiPage ()) {
		TIFFWriteDirectory (m_pTiff);
	}

	return (0);
}

const bool IrtvTiff::isMultiPage () 
{
	return (m_bMultiPage); 
}

const bool IrtvTiff::isCompress () 
{
	return (m_bCompress); 
}

inline const double IrtvTiff::getIrigTime ()
{
	return ( 
		m_stFrameHeader.IRIGTime.Millisecond 
			+ m_stFrameHeader.IRIGTime.Second * 1000
			+ m_stFrameHeader.IRIGTime.Minute * 1000 * 60
			+ m_stFrameHeader.IRIGTime.Hour   * 1000 * 3600
	);
}
