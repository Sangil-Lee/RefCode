#include <time.h>
#include <stdlib.h>
#include <afxwin.h>
#include <sys/timeb.h>

#include <epicsTime.h>

#include "irtvBMP.h"

IrtvBMP::IrtvBMP () 
{ 
	init ();
}

IrtvBMP::~IrtvBMP () 
{
	this->close ();
}

void IrtvBMP::init () 
{
	m_width			= 0;
	m_height		= 0;
	m_nPageNo		= 0;
}

const int IrtvBMP::open (const int nShotNum, UInt16 width, UInt16 height) 
{
	init ();

	m_nShotNum		= nShotNum;
	m_width			= width;
	m_height		= height;

	makeDateTime ();

	int		nbytes	= IRTV_IMG_BYTES;
	int		nAlign	= 4;	// IRTV_IMG_BYTES

	if (((m_width * nbytes) % nAlign) == 0) {
		m_nPitch = m_width * nbytes;
	}
	else {
		m_nPitch = (m_width * nbytes + (nAlign - ((m_width * nbytes) % nAlign)));
	}

	return (0);
}

void IrtvBMP::close () 
{
	init ();
}

void IrtvBMP::makeDateTime ()
{
	epicsTimeStamp epics_time;

	epicsTimeGetCurrent (&epics_time);
	epicsTimeToStrftime (m_szFrameStartTime, sizeof(m_szFrameStartTime), "%Y:%m:%d %H:%M:%S", &epics_time);
}

const int IrtvBMP::saveBMPToFile (const char *fname, unsigned char *pBits, int nWidth, int nHeight, int nPitch)
{
	CFile	file;

	if (!file.Open(fname, CFile::modeCreate | CFile::modeWrite)) {
		return (-1);
	}

	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmih;

	bmfh.bfType = 0x4D42;	//	"BM"
	bmfh.bfSize = (nPitch * nHeight) + 54;
	bmfh.bfReserved1 = 0;
	bmfh.bfReserved2 = 0;
	bmfh.bfOffBits = 54;

	bmih.biSize = 40;
	bmih.biWidth = nWidth;
	bmih.biHeight = nHeight;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;
	bmih.biCompression = 0;
	bmih.biSizeImage = nPitch * nHeight;
	bmih.biXPelsPerMeter = 72;
	bmih.biYPelsPerMeter = 72;
	bmih.biClrUsed = 0;
	bmih.biClrImportant = 0;

	file.Write(&bmfh.bfType, 2);
	file.Write(&bmfh.bfSize, 4);
	file.Write(&bmfh.bfReserved1, 2);
	file.Write(&bmfh.bfReserved2, 2);
	file.Write(&bmfh.bfOffBits, 4);

	file.Write(&bmih.biSize, 4);
	file.Write(&bmih.biWidth, 4);
	file.Write(&bmih.biHeight, 4);
	file.Write(&bmih.biPlanes, 2);
	file.Write(&bmih.biBitCount, 2);
	file.Write(&bmih.biCompression, 4);
	file.Write(&bmih.biSizeImage, 4);
	file.Write(&bmih.biXPelsPerMeter, 4);
	file.Write(&bmih.biYPelsPerMeter, 4);
	file.Write(&bmih.biClrUsed, 4);
	file.Write(&bmih.biClrImportant, 4);

	file.Write(pBits, nPitch * nHeight);

	file.Close();

	return (0);
}

const int IrtvBMP::write (const char *pszBmpFileName, UInt16 *data) 
{
	// Frame 내의 헤더 정보 얻기
	iacfDecodeHeader (&m_stFrameHeader, data);

	if (0 == m_nPageNo) {
		// 1st Frame의 시각 얻기
		m_dFirstFrameIrigTime	= getIrigTime ();
	}

	// Frame Number 증가
	m_nPageNo	+= 1;

	// 현재 Frame의 시각 얻기
	m_dCurrFrameIrigTime	= getIrigTime ();

	// 부가 정보 생성
	char	szShotNum[64];
	char	szDesc[100];
	char	szOffsetTime[64];
	char	szDateTime[24];
	int		nOffsetTime	= (int)(m_dCurrFrameIrigTime - m_dFirstFrameIrigTime);

	sprintf (szShotNum		, "Shot : %07d", m_nShotNum);
	sprintf (szOffsetTime	, "%01d:%02d.%03d", nOffsetTime / (1000 * 60), (nOffsetTime % (1000 * 60)) / 1000, nOffsetTime % 1000);
	sprintf (szDesc			, "Shot : %07d, Page : %05d, Time : %s, Resolution : %d x %d", m_nShotNum, m_nPageNo, szOffsetTime, m_width, m_height);
	sprintf (szDateTime		, "%-19s", szOffsetTime);

	// RGB로 변환
	convertFrameToRGB (data, (unsigned char *)m_frameBuffer, m_width, m_height);

	// BMP 파일로 저장
	saveBMPToFile (pszBmpFileName, (unsigned char *)m_frameBuffer, m_width, m_height, m_nPitch);

	return (0);
}

inline const double IrtvBMP::getIrigTime ()
{
	return ( 
		m_stFrameHeader.IRIGTime.Millisecond 
			+ m_stFrameHeader.IRIGTime.Second * 1000
			+ m_stFrameHeader.IRIGTime.Minute * 1000 * 60
			+ m_stFrameHeader.IRIGTime.Hour   * 1000 * 3600
	);
}

// 12 bits RAW Data를 24 bits RGB 데이터로 변환
void IrtvBMP::convertFrameToRGB (unsigned short *data, unsigned char *pBits, int width, int height)
{
	int		i, nPixels = width * height;

	for (i = 0; i < 65536; ++i) {
		m_Hist[i] = 0;
	}

	for (i = 0; i < nPixels; ++i) {
		++m_Hist[data[i+width]];
	}

	//int				nMax = (int)(nPixels*0.0003);
	int				nMax = (int)(nPixels*0.03);
	unsigned short	minValue=0, maxValue=65535;
	int				n;

	n = 0;
	for (i = 0; i < 65535; ++i) {
		n += m_Hist[i];

		if (n > nMax) {
			minValue = (unsigned short)(i+1);
			break;
		}
	}

	n = 0;
	for (i = 65535; i > 0; --i) {
		n += m_Hist[i];

		if (n > nMax) {
			maxValue = (unsigned short)(i-1);
			break;
		}
	}

	int				x, y;
	unsigned char	index;
	double			scale = 255.0 / (maxValue - minValue);

	for (i = 0, y = 0; y < height; ++y) {
	//for (i = 0, y = height - 1; y >= 0; --y) {
		for (x = 0; x < width; ++x, ++i) {
			index = (unsigned char)((min(max(minValue, data[i + width]), maxValue) - minValue) * scale);

			// TOP/LEFT
			pBits[y * m_nPitch + x*IRTV_IMG_BYTES + 0] = index;
			pBits[y * m_nPitch + x*IRTV_IMG_BYTES + 1] = index;
			pBits[y * m_nPitch + x*IRTV_IMG_BYTES + 2] = index;

			if (4 == IRTV_IMG_BYTES) {
				pBits[y * m_nPitch + x*IRTV_IMG_BYTES + 3] = 0xff;
			}
		}
	}
}
