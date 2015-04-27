#ifndef _SINGLE_PLOT_WINDOW_H
#define _SINGLE_PLOT_WINDOW_H
#include <qapplication.h>
#include <qmainwindow.h>
#include <qwt_counter.h>
#include <qtoolbar.h>
#include <qlabel.h>
#include <qlayout.h>
#include "SinglePlot.h"

class SinglePlotWindow : public QMainWindow
{
public:
    SinglePlotWindow(const QString& pvname);
	virtual ~SinglePlotWindow();
private:
	SinglePlot *mplot;
protected:
	void closeEvent(QCloseEvent *event);
};

#endif
