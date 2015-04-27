#ifndef INTERACTIVEPLOT_H
#define INTERACTIVEPLOT_H

#include <qwt_plot.h>
#include "colorbar.h"
#include "canvaspicker.h"
#include "scalepicker.h"
#include <qevent.h>
#if QT_VERSION < 0x040000
#include <qwhatsthis.h>
#endif
#include <qwt_plot_layout.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_scale_widget.h>
#include <qwt_wheel.h>
#include <stdlib.h>

class ColorBar;
class QwtWheel;
class CanvasPicker;
class ScalePicker;

class InteractivePlot: public QwtPlot
{
    Q_OBJECT
public:
    InteractivePlot(QWidget *parent = NULL);
    virtual bool eventFilter(QObject *, QEvent *);

	void initInteractivePlot();
	QwtData &getData() const;

public slots:
    void setCanvasColor(const QColor &);
    void insertCurve(int axis, double base);

private slots:
    void scrollLeftAxis(double);

private:
    void insertCurve(Qt::Orientation, const QColor &, double base);

    ColorBar *d_colorBar;
    QwtWheel *d_wheel;
	CanvasPicker *canvasPicker;
	ScalePicker *scalePicker;
};

#endif
