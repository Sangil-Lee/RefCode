#include "SinglePlotWindow.h"

SinglePlotWindow::SinglePlotWindow(const QString& pvname, const int periodic):mperiodic(periodic)
{
	hide();
	//setWindowModality(Qt::ApplicationModal);
	//setMaximumSize(900,400);
	//setMinimumSize(900,400);
	resize(900,400);
	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFixedHeight(80);
	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	QWidget *hBox = new QWidget(toolBar);
	QLabel *ymaxlabel = new QLabel("Y-Max", hBox);

	//mymax = new QDoubleSpinBox(hBox);
	mymax = new QLineEdit(hBox);
	mymax -> setText("100.0");
	//mymax->setFixedWidth(100);
	//mymax->setRange(-100000000, 100000000);
	//mymax->setSingleStep(50);
	//mymax->setValue(10);
	//mymax->setDecimals(15);

	QLabel *yminlabel = new QLabel("Y-Min", hBox);
	mymin = new QLineEdit(hBox);
	mymin -> setText("0.0");
	//mymin->setFixedWidth(100);
	//mymin->setRange(-100000000, 100000000);
	//mymin->setSingleStep(50);
	//mymin->setValue(-10);
	//mymin->setDecimals(15);

	QCheckBox *autoscale = new QCheckBox("Auto Scale",hBox);
	QCheckBox *logscale = new QCheckBox("Log Scale",hBox);

	QLabel *penwidth = new QLabel("Width", hBox);
	QLabel *pencolor = new QLabel("Color", hBox);
	QLabel *linestyle = new QLabel("Style", hBox);
	QLabel *date = new QLabel("Date", hBox);

	QComboBox *penwidthcomob = new QComboBox(hBox);
	QComboBox *pencolorcomob = new QComboBox(hBox);
	QComboBox *linestylecomob = new QComboBox(hBox);
	penwidthcomob  ->setEditable(false);
	pencolorcomob  ->setEditable(false);
	linestylecomob ->setEditable(false);

	QStringList items;
	items<<"1"<<"2"<<"3"<<"4"<<"5"<<"6";
	penwidthcomob  ->addItems(items);
	items.clear();
	items<<"blue"<<"red"<<"black"<<"green"<<"yellow"<<"pink";
	pencolorcomob  ->addItems(items);
	items.clear();
	items<<"Lines"<<"Steps"<<"Scatters"<<"Sticks";
	linestylecomob ->addItems(items);
	items.clear();

	QHBoxLayout *layout = new QHBoxLayout(hBox);
	layout->addWidget(yminlabel);
	layout->addWidget(mymin);
	layout->addWidget(ymaxlabel);
	layout->addWidget(mymax);
	layout->addWidget(autoscale);
	layout->addWidget(logscale);
	layout->addWidget(penwidth);
	layout->addWidget(penwidthcomob);
	layout->addWidget(pencolor);
	layout->addWidget(pencolorcomob);
	layout->addWidget(linestyle);
	layout->addWidget(linestylecomob);
	layout->addWidget(new QWidget(hBox), 10); // spacer);
	layout->addWidget(date);

#if QT_VERSION >= 0x040000
	toolBar->addWidget(hBox);
#endif
	addToolBar(toolBar);
#if 1
	mplot = new SinglePlot( pvname, this, mperiodic );
	setCentralWidget(mplot);
#endif

#if 0
	QRect rect(25, 370, 90, 20);
	QLabel *timelabel = new QLabel("Time Display", this);
	timelabel->setAlignment(Qt::AlignLeft);
	timelabel->setGeometry(rect);

	QLabel *timelabel1 = new QLabel("Time Display", this);
	rect.setRect(193, 370, 90, 20);
	timelabel1->setGeometry(rect);

	QLabel *timelabel2 = new QLabel("Time Display", this);
	rect.setRect(362, 370, 90, 20);
	timelabel2->setGeometry(rect);

	QLabel *timelabel3 = new QLabel("Time Display", this);
	rect.setRect(530, 370, 90, 20);
	timelabel3->setGeometry(rect);

	QLabel *timelabel4 = new QLabel("Time Display", this);
	rect.setRect(695, 370, 90, 20);
	timelabel4->setGeometry(rect);

	QLabel *timelabel5 = new QLabel("Time Display", this);
	rect.setRect(810, 370, 90, 20);
	timelabel5->setAlignment(Qt::AlignRight);
	timelabel5->setGeometry(rect);

	connect(mplot, SIGNAL(updateLabel(QString)), timelabel, SLOT(setText(QString )) );
	connect(mplot, SIGNAL(updateLabel1(QString )), timelabel1, SLOT(setText(QString )) );
	connect(mplot, SIGNAL(updateLabel2(QString )), timelabel2, SLOT(setText(QString )) );
	connect(mplot, SIGNAL(updateLabel3(QString )), timelabel3, SLOT(setText(QString )) );
	connect(mplot, SIGNAL(updateLabel4(QString )), timelabel4, SLOT(setText(QString )) );
	connect(mplot, SIGNAL(updateLabel5(QString )), timelabel5, SLOT(setText(QString )) );
#endif
#if 1
	connect(mplot,  SIGNAL(updateDate(QString )), date, SLOT(setText(QString)) );
	connect(penwidthcomob,  SIGNAL(currentIndexChanged(int)), mplot, SLOT(setPenWidth(int)) );
	connect(pencolorcomob,  SIGNAL(currentIndexChanged(const QString &)), mplot, SLOT(setPenColor(const QString &)));
	connect(linestylecomob, SIGNAL(currentIndexChanged(int)), mplot, SLOT(setLineStyle(int)) );
#if 0
	//connect(mymax, SIGNAL(valueChanged(double)), mplot, SLOT(setYMaxSpan(double)) );
	//connect(mymin, SIGNAL(valueChanged(double)), mplot, SLOT(setYMinSpan(double)) );
#else
	connect(mymax, SIGNAL(returnPressed()), mplot, SLOT(setYMaxSpan()) );
	connect(mymin, SIGNAL(returnPressed()), mplot, SLOT(setYMinSpan()) );
#endif
	//mplot->SetSpan(mymin->value(), mymax->value());

	connect(autoscale, SIGNAL(stateChanged(int)), this, SLOT(setAutoScale(int)) );
	autoscale->setChecked(Qt::Checked);
	connect(logscale, SIGNAL(stateChanged(int)), this, SLOT(setLogScale(int)) );
#endif
	show();
};
void 
SinglePlotWindow::setRangeEnable(const bool check)
{
	mymax->setEnabled(check);
	mymin->setEnabled(check);
}
void 
SinglePlotWindow::setAutoScale(int check)
{
	if(check == Qt::Checked)
	{
		mplot->setAxisAutoScale(QwtPlot::yLeft);
		setRangeEnable(false);
	}
	else if(check == Qt::Unchecked)
	{
		setRangeEnable(true);
		mplot->SetSpan(mymin->text(), mymax->text());
	};
	mplot->replot();
}

void 
SinglePlotWindow::setLogScale(int check)
{
	if(check == Qt::Checked)
	{
		//Qwt 5.x
		//mplot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
		//Qwt 6.1
		mplot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLogScaleEngine);
		//mplot->SetSpan(mymin->text(), mymax->text());
	}
	else if(check == Qt::Unchecked)
	{
		mplot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
		//mplot->SetSpan(mymin->text(), mymax->text());
	};
	mplot->replot();
}
void
SinglePlotWindow::closeEvent (QCloseEvent *)
{
	mplot->GetPlotThread()->setStop(true);
	//delete mplot;
}
