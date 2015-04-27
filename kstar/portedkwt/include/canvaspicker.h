#ifndef CANVASPICKER_H
#define CANVASPICKER_H
#include <qobject.h>
#include <qapplication.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
//#include <qwt_data.h>

class QPoint;
class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;

class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    CanvasPicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);

    virtual bool event(QEvent *);
	QwtData &getData();

private:
    void select(const QPoint &);
    void move(const QPoint &);
    void moveBy(int dx, int dy);

    void release();

    void showCursor(bool enable);
    void shiftPointCursor(bool up);
    void shiftCurveCursor(bool up);

    QwtPlot *plot() { return (QwtPlot *)parent(); }
    const QwtPlot *plot() const { return (QwtPlot *)parent(); }

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
	//QwtData m_data;
};
#endif
