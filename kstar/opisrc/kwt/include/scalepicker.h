#ifndef SCALEPICKER_H
#define SCALEPICKER_H

#include <qobject.h>
#include <qrect.h>
#include <qevent.h>
#include <qwt_plot.h>
#include <qwt_scale_widget.h>

class QwtPlot;
class QwtScaleWidget;

class ScalePicker: public QObject
{
    Q_OBJECT
public:
    ScalePicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);

signals:
    void clicked(int axis, double value);

private:
    void mouseClicked(const QwtScaleWidget *, const QPoint &);
    QRect scaleRect(const QwtScaleWidget *) const;
};
#endif
