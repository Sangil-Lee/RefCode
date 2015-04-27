#ifndef _IRTV_GRAB_FRAME_DATA_H
#define _IRTV_GRAB_FRAME_DATA_H

#include <string.h>

#define	IRTV_RAW_TYPE_SIZE			2		// sizeof(unsigned short)
#define	IRTV_BMP_TYPE_SIZE			4		// sizeof(unsigned int)

#define	IRTV_MAX_WIDTH				640
#define	IRTV_MAX_HEIGHT				512
#define	IRTV_MAX_HEIGHT_HDR			(IRTV_MAX_HEIGHT + 1)
#define	IRTV_MAX_BUF_SIZE			(IRTV_MAX_WIDTH * IRTV_MAX_HEIGHT_HDR * IRTV_RAW_TYPE_SIZE)
#define	IRTV_MAX_BMP_BUF_SIZE		(IRTV_MAX_WIDTH * IRTV_MAX_HEIGHT_HDR * IRTV_BMP_TYPE_SIZE)

#define IRTV_GET_HEADER_SIZE(cols)	(cols * 2)

#if defined(WIN32)
#define	IRTV_LIVE_GRAB_FRAME_PATH		"D:\\IRTV\\LiveData\\IRTV_Live.tif"
#define	IRTV_LIVE_GRAB_FRAME_TMP_PATH	"D:\\IRTV\\LiveData\\IRTV_Live_Tmp.tif"
#define	IRTV_LIVE_GRAB_RAW_PATH			"D:\\IRTV\\LiveData\\IRTV_Live.raw"
#define	IRTV_LIVE_GRAB_RAW_TMP_PATH		"D:\\IRTV\\LiveData\\IRTV_Live_Tmp.raw"
#else
#define	IRTV_LIVE_GRAB_FRAME_PATH		"../images/IRTV_Live.tif"
#define	IRTV_LIVE_GRAB_RAW_PATH			"../images/IRTV_Live.raw"
#endif

// Frame의 헤더 및 Raw 데이터 정보
class GrabFrameData
{
public :
	GrabFrameData () {
		init ();
	};

	~GrabFrameData () {};

	void init () {
		m_nWidth			= 0;
		m_nHeight			= 0;
		m_nFrameSize		= 0;
		m_nFrameSeqNum		= 0;
	}

	const unsigned short getWidth () { return (m_nWidth); }
	void setWidth (unsigned short nWidth) { m_nWidth = nWidth; }

	const unsigned short getHeight () { return (m_nHeight); }
	void setHeight (unsigned short nHeight) { m_nHeight = nHeight; }

	const unsigned int getFrameSize () { return (m_nFrameSize); }
	void setFrameSize (const unsigned int nFrameSize) { m_nFrameSize = nFrameSize; }

	const unsigned int getTotalSize () {
		return (sizeof(GrabFrameData) - IRTV_MAX_BUF_SIZE + getFrameSize ());
	}

	const unsigned char *getFrameData () { return (m_data); }
	const unsigned char *getFrameData (unsigned char *ptr, unsigned int &size) {
		if (NULL != ptr) {
			size = getFrameSize ();
			memcpy (ptr, m_data, size);
		}

		return (ptr); 
	}

	void setFrameData (unsigned short nWidth, unsigned short nHeight, unsigned char *ptr) {
		setWidth (nWidth);
		setHeight (nHeight);
		setFrameData (ptr, (nWidth * nHeight * 2));

		++m_nFrameSeqNum;
	}

	const epicsFloat64 getFrameSeqNum () { return (m_nFrameSeqNum); }

	const char *getFrameDataPtr () { return ((char *)m_data); }

private :

	void setFrameData (const unsigned char *ptr, const int size) {
		if (size <= IRTV_MAX_BUF_SIZE) {
			memcpy (m_data, ptr, size);
			setFrameSize (size);
		}
	}

private :

	// Header
	epicsFloat64		m_nFrameSeqNum;			// Sequence number : to check that data was updated
    unsigned short		m_nWidth;
	unsigned short		m_nHeight;

	// Data
	unsigned int		m_nFrameSize;
	unsigned char		m_data[IRTV_MAX_BUF_SIZE];

};

// Live Data
extern GrabFrameData	gLiveFrameData;

#endif // _IRTV_GRAB_FRAME_DATA_H
