#include <QDebug>

#include "mainWindow.h"
#include "irtvMainWindow.h"
#include "irtvDataClient.h"

#if defined(WIN32)
#include <io.h>	// for access()
#endif

using namespace std;

// display on/off
bool				gbDisplayOnOff	= false;

IrtvGraphicsScene::IrtvGraphicsScene (IRTV_Interface *parent) 
{
	m_parent		= parent;
	m_mousePressed	= false;
	m_shapeItem		= NULL;
	m_brush			= new QBrush(QColor(0,255,0), Qt::NoBrush);
	m_pen			= new QPen(QColor(255,255,0), IRTV_ROI_LINE_WIDTH);

	QRectF rectF(0, 0, IRTV_MAX_WIDTH, IRTV_MAX_HEIGHT);
	setSceneRect(rectF);

	reDrawRect (rectF);
}

void IrtvGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * e) {
	if (m_mousePressed) {
		m_ePoint	= e->lastScenePos ();
	
		reDrawRect (m_sPoint, m_ePoint);
	}
}

void IrtvGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * e) {
	m_mousePressed	= true;
	m_sPoint	= e->buttonDownScenePos (e->button());
}
void IrtvGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * e) {
	m_mousePressed	= false;
	m_sPoint	= e->buttonDownScenePos (e->button());

	// ROI 설정 값 변경
	m_parent->updateWindowCoordi();
}

void IrtvGraphicsScene::reDrawRect (QRectF rectF) {
	return ( reDrawRect (rectF.x(), rectF.y(), rectF.width(), rectF.height()));
}

void IrtvGraphicsScene::reDrawRect (QPointF sPoint, QPointF ePoint) {
	qreal x, y, w, h;

	if (sPoint.rx() > ePoint.rx()) {
		x = ePoint.rx();
		w = sPoint.rx() - ePoint.rx();
	}
	else {
		x = sPoint.rx();
		w = ePoint.rx() - sPoint.rx();
	}

	if (sPoint.ry() > ePoint.ry()) {
		y = ePoint.ry();
		h = sPoint.ry() - ePoint.ry();
	}
	else {
		y = sPoint.ry();
		h = ePoint.ry() - sPoint.ry();
	}

	return ( reDrawRect (x, y, w, h));
}

void IrtvGraphicsScene::reDrawRect (qreal x, qreal y, qreal w, qreal h) {
	qreal sx, sy, ex, ey;
	sx = x < 0 ? 0 : x;
	sy = y < 0 ? 0 : y;
	ex = x + w;
	ey = y + h;
	ex = ex > IRTV_MAX_WIDTH  ? IRTV_MAX_WIDTH  : ex;
	ey = ey > IRTV_MAX_HEIGHT ? IRTV_MAX_HEIGHT : ey;

	m_xOffset	= sx;
	m_yOffset	= sy;
	m_width		= ex - sx;
	m_height	= ey - sy;

	//qDebug ("[reDrawRect] (%f,%f) (%f,%f)", x, y, w, h);
	//qDebug ("[reDrawRect] (%f,%f) (%f,%f)", m_xOffset, m_yOffset, m_width, m_height);

	// apply the resolution restrictions for window size and position
	m_xOffset	-= ((int)m_xOffset) % IRTV_WIN_RES_WIDTH;
	m_yOffset	-= ((int)m_yOffset) % IRTV_WIN_RES_HEIGTH;
	m_width		-= ((int)m_width  ) % IRTV_WIN_RES_WIDTH;
	m_height	-= ((int)m_height ) % IRTV_WIN_RES_HEIGTH;
	//qDebug ("[reDrawRect] => (%f,%f) (%f,%f)", m_xOffset, m_yOffset, m_width, m_height);

	if (m_shapeItem) {
		removeItem (m_shapeItem);
		m_shapeItem = NULL;
	}

	//m_shapeItem = addRect (m_xOffset, m_yOffset, m_width, m_height, *m_pen, *m_brush);
	m_shapeItem = addRect (m_xOffset, m_yOffset, 
		m_width - IRTV_ROI_LINE_WIDTH, m_height - IRTV_ROI_LINE_WIDTH, *m_pen, *m_brush);
	//m_shapeItem->setFlag(QGraphicsItem::ItemIsMovable);

	// ROI 설정 값 변경
	char	strx[64], stry[64], strwidth[64], strheight[64];

	sprintf (strx		, "%d", (int)m_xOffset);
	sprintf (stry		, "%d", (int)m_yOffset);
	sprintf (strwidth	, "%d", (int)m_width);
	sprintf (strheight	, "%d", (int)m_height);

	m_parent->setWindowCoordi (strx, stry, strwidth, strheight);
}

IRTV_MainWindow::IRTV_MainWindow (QWidget *parent)
{
	init ();
}

IRTV_MainWindow::~IRTV_MainWindow ()
{
	if (NULL != m_QTimer) {
		m_QTimer->stop ();
		delete (m_QTimer);

		m_QTimer	= NULL;
	}

#if defined(WIN32)
	try {
		m_grabFrameShm.close ();
	}
	//catch (IrtvGrabFrameShmException &shmErr) {
	catch (...) {
	}
#endif
}

void IRTV_MainWindow::init ()
{
	setPrintLevel (K_MON);

	m_bDataRecvThr	= false;
	m_bDisplayOn	= false;					// DisplayOff 설정
	m_nTimerMillis	= IRTV_LIVE_VIEW_TIMER;		// 100 msec

	m_QTimer		= NULL;
	m_QTimerPV		= NULL;

	m_nFrameSeqNum	= 0;

}

// Windwos : 14 bits RAW Data를 24 bits RGB 데이터로 변환
// Linux   : 14 bits RAW Data를 32 bits RGB 데이터로 변환
void IRTV_MainWindow::convertFrameToRGB (unsigned short *frameBuffer, unsigned char *pBits, int width, int height)
{
	int		i, nPixels = width * height;

	for (i = 0; i < 65536; ++i) {
		m_Hist[i] = 0;
	}

	for (i = 0; i < nPixels; ++i) {
		++m_Hist[frameBuffer[i]];
	}

	//int				nMax = (int)(nPixels*0.0003);
	int				nMax = (int)(nPixels*0.03);	// 좀 더 밝게 표시
	unsigned short	minValue=0, maxValue=65535;
	unsigned int	n;

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

	int		Pitch;
	int		nbytes	= IRTV_IMG_BYTES;
	int		nAlign	= 4;

	if (((width * nbytes) % nAlign) == 0) {
		Pitch = width * nbytes;
	}
	else {
		Pitch = (width * nbytes + (nAlign - ((width * nbytes) % nAlign)));
	}

	int				x, y;
	unsigned char	index;
	double			scale = 255.0 / (maxValue - minValue);

	for (i = 0, y = 0; y < height; ++y) {
		for (x = 0; x < width; ++x, ++i) {
			index = (unsigned char)((min(max(minValue, frameBuffer[i + width]), maxValue) - minValue) * scale);

			// TOP/LEFT
			pBits[y * Pitch + x*IRTV_IMG_BYTES + 0] = index;
			pBits[y * Pitch + x*IRTV_IMG_BYTES + 1] = index;
			pBits[y * Pitch + x*IRTV_IMG_BYTES + 2] = index;

			if (nAlign == 4) {
				pBits[y * Pitch + x*IRTV_IMG_BYTES + 3] = 0xff;
			}
		}
	}
}

int IRTV_MainWindow::writePV (const char *pvName, const char *pvValue)
{
	chid	chanId;

	ca_create_channel (pvName, NULL, NULL, 20, &chanId);
	ca_pend_io (0.5);

	ca_put (DBR_STRING, chanId, pvValue);
	ca_pend_io (0.5);

	//TODO
	ca_clear_channel (chanId);

	return (true);
}

// 참고 : KWT의 caArrayThread.cpp 파일로부터 내용을 가져옴
char *IRTV_MainWindow::val2str (const void *v, unsigned type, int index)
{
	static char str[500];
	char ch;
	void *val_ptr;
	unsigned base_type;

	base_type = type % (LAST_TYPE+1);
	//qDebug("basetype:%d, type:%d, listtype:%d, DBR_DOUBLE:%d", base_type, type, LAST_TYPE, DBR_DOUBLE);

	if (type == DBR_STSACK_STRING || type == DBR_CLASS_NAME)
		base_type = DBR_STRING;

	val_ptr = dbr_value_ptr(v, type);
	//qDebug("valptr:%p, v:%p", val_ptr, v);

	switch (base_type) {
		case DBR_STRING:
			sprintf(str, "%s",  ((dbr_string_t*) val_ptr)[index]);
			break;
		case DBR_FLOAT:
			sprintf(str, "%g", ((dbr_float_t*) val_ptr)[index]);
			break;
		case DBR_DOUBLE:
			sprintf(str, "%g", ((dbr_double_t*) val_ptr)[index]);
			break;
		case DBR_CHAR:
			ch = ((dbr_char_t*) val_ptr)[index];
			if(ch > 31 )
				sprintf(str, "%d \'%c\'",ch,ch);
			else
				sprintf(str, "%d",ch);
			break;
		case DBR_INT:
			sprintf(str, "%ld", ((dbr_int_t*) val_ptr)[index]);
			break;
		case DBR_LONG:
			sprintf(str, "%ld", ((dbr_long_t*) val_ptr)[index]);
			break;
		case DBR_ENUM:
			{
				dbr_enum_t *val = (dbr_enum_t *)val_ptr;
				if (dbr_type_is_GR(type))
					sprintf(str,"%s (%d)", 
							((struct dbr_gr_enum *)v)->strs[val[index]],val[index]);
				else if (dbr_type_is_CTRL(type))
					sprintf(str,"%s (%d)", 
							((struct dbr_ctrl_enum *)v)->strs[val[index]],val[index]);
				else
					sprintf(str, "%d", val[index]);
			}
	}

	return str;
}

void irtvConnectionCallback (struct connection_handler_args args)
{
	IRTV_CA_INFO	*pNode = (IRTV_CA_INFO *) ca_puser(args.chid);

	if (NULL == pNode) {
		return;
	}

	if ( args.op == CA_OP_CONN_UP ) {
		if (0 == pNode->bInit) {
			int size;

			pNode->nElems	= ca_element_count (pNode->chanid);
			pNode->dbfType	= ca_field_type (pNode->chanid);
			pNode->dbrType	= dbf_type_to_DBR_TIME(pNode->dbfType);
			size			= dbr_size_n (pNode->dbrType, pNode->nElems);
			pNode->value	= calloc (1, size);
			pNode->bInit	= 1;
		}

		pNode->status	= ECA_CONN;

		//qDebug ("connCB : pv(%s) chid(%p) elems(%d) type(%d/%d)", 
		//	pNode->name.c_str(), pNode->chanid, (int)pNode->nElems, (int)pNode->dbfType, (int)pNode->dbrType);
	}
	else if ( args.op == CA_OP_CONN_DOWN ) {
		pNode->status = ECA_DISCONN;

		//qDebug ("connCB : pv(%s) state(%d)", ca_name(args.chid), ca_state(args.chid));
	}
}

