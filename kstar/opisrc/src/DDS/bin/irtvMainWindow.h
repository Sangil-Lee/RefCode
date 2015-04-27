#ifndef IRTV_MAINWINDOW_H
#define IRTV_MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <qwt_plot.h>

// Channel Access for EPICS
#include "cadef.h"
#include "dbDefs.h"

#include "irtvGrabFrameData.h"

#if defined(WIN32)
#include "irtvGrabFrameShm.h"
#define	IRTV_USE_SHM_DATA
#endif

#include <iostream>
#include <vector>
#include <string>
#include <deque>
#include <map>
#include <algorithm>

using namespace std;

#if defined(WIN32)
#define	IRTV_FILE_DIALOG_PATH		"D:\\"
#define	IRTV_ROI_IMAGE_PATH			"images/DDS_IRTV_ROI.png"
#else
#define	IRTV_FILE_DIALOG_PATH		"/home"
#define	IRTV_ROI_IMAGE_PATH			"/usr/local/opi/images/DDS_IRTV_ROI.png"
#endif

const int IRTV_WIN_RES_WIDTH		= 16;
const int IRTV_WIN_RES_HEIGTH		= 4;
const int IRTV_PV_UPDATE_TIMER_MSEC	= 3000;

#if defined(WIN32)
#define	IRTV_IMG_FORMAT		QImage::Format_RGB888
#define	IRTV_IMG_BYTES		3
#else
#define	IRTV_IMG_FORMAT		QImage::Format_RGB32
#define	IRTV_IMG_BYTES		4
#endif

#define	IRTV_LIVE_VIEW_TIMER	100		// msec

#define	IRTV_MAX_WIDTH		640
#define	IRTV_MAX_HEIGHT		512
#define	IRTV_ROI_LINE_WIDTH	3

/* Structure representing one PV (= channel) */
typedef struct {
    string			name;
    chid			chanid;
    long			dbfType;
    long			dbrType;
    unsigned long	nElems;
    unsigned long	reqElems;
    int				status;
	void			*value;
	float			fval;
    vector<string>	wval;	// waveform
    epicsTimeStamp	tsPrev;
    epicsTimeStamp	tsCurr;
    char			firstStampPrinted;
    char			bInit;
} IRTV_CA_INFO;

class IRTV_Interface
{
public :
	IRTV_Interface() {};
	virtual ~IRTV_Interface() {};

	virtual void updateWindowCoordi () = 0;
	virtual void setWindowCoordi (char *x, char *y, char *width, char *height) = 0;
};

// 마우스 이벤트를 얻기 위하여 서브 클래스를 재정의함
class IrtvGraphicsScene : public QGraphicsScene
{
Q_OBJECT

public:
	IrtvGraphicsScene (IRTV_Interface *parent);

	int getXOffset () { return ((int)m_xOffset); }
	void setXOffset (int xOffset) { m_xOffset = xOffset; }

	int getYOffset () { return ((int)m_yOffset); }
	void setYOffset (int yOffset) { m_yOffset = yOffset; }

	int getWidth ()   { return ((int)m_width); }
	void setWidth (int width)   { m_width = width; }

	int getHeight ()  { return ((int)m_height); }
	void setHeight (int height)  { m_height = height; }

	void get (qreal &xOffset, qreal &yOffset, qreal &width, qreal &height) {
		xOffset	= m_xOffset;
		yOffset	= m_yOffset;
		width	= m_width;
		height	= m_height;
	}

	void set (qreal xOffset, qreal yOffset, qreal width, qreal height) {
		m_xOffset	= xOffset;
		m_yOffset	= yOffset;
		m_width		= width;
		m_height	= height;
	}

private :
	QAbstractGraphicsShapeItem *m_shapeItem;
	QBrush			*m_brush;
	QPen			*m_pen;
	IRTV_Interface	*m_parent;

	bool			m_mousePressed;
	QPointF			m_sPoint;
	QPointF			m_ePoint;

	qreal			m_xOffset;
	qreal			m_yOffset;
	qreal			m_width;
	qreal			m_height;

protected:
	void mouseMoveEvent (QGraphicsSceneMouseEvent * e);
	void mousePressEvent (QGraphicsSceneMouseEvent * e);
	void mouseReleaseEvent (QGraphicsSceneMouseEvent * e);

public :
	void reDrawRect (QRectF rectF);
	void reDrawRect (QPointF sPoint, QPointF ePoint);
	void reDrawRect (qreal x, qreal y, qreal w, qreal h);
};

class IRTV_MainWindow
{
public :
    IRTV_MainWindow (QWidget *parent = 0);
    ~IRTV_MainWindow ();

	void init ();

protected:
	void convertFrameToRGB (unsigned short *pFrameBuffer, unsigned char *pBits, int width, int height);

	int writePV (const char *pvName, const char *pvValue);
	char *val2str (const void *v, unsigned type, int index);

protected :
	QTimer	*m_QTimer;
	QTimer	*m_QTimerPV;

#if defined(WIN32)
	IrtvGrabFrameShm	m_grabFrameShm;
#endif

	GrabFrameData		m_grabFrameData;
	epicsFloat64		m_nFrameSeqNum;

	unsigned int		m_Hist[65536];
	unsigned short		m_frameBuffer[IRTV_MAX_BMP_BUF_SIZE];

	// Display On/Off status
	bool				m_bDisplayOn;
	bool				m_bDataRecvThr;

	// timer
	int					m_nTimerMillis;

	// Channel Access
	vector<IRTV_CA_INFO>	m_vCAInfoList;

	// ROI 선택
	IrtvGraphicsScene	*m_canvas;
	QGraphicsView		*m_grapicsView;
	QAbstractGraphicsShapeItem	*m_shapeItem;
};

extern void irtvConnectionCallback (struct connection_handler_args args);

#endif // IRTV_MAINWINDOW_H
