#include <QDebug>

#include "mainWindow.h"
#include "qtchaccesslib.h"

#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_scale_draw.h>

using namespace std;

MainWindow::MainWindow (QWidget *parent) :
    QMainWindow(parent)
{
	init ();
}

MainWindow::~MainWindow ()
{
    delete m_pIrtvS1Window;
	m_pIrtvS1Window = NULL;

	delete m_pIrtvD1Window;
	m_pIrtvD1Window = NULL;
}

void MainWindow::init ()
{
	m_pIrtvS1Window	= NULL;
	m_pIrtvD1Window	= NULL;

	setupUi(this);

	insertStackedWidgetPage ();

	createActions ();
	createConnects ();

	createToolbars ();
	createStatusBar ();

	on_btnIRTV_S1_clicked ();
	on_btnIRTV_D1_clicked ();
}

void MainWindow::insertStackedWidgetPage ()
{
	// this를 사용할 경우, Windows에서 Tab 및 버튼들이 활성화 되지 않음
    //new AttachChannelAccess (this);
    new AttachChannelAccess (frame_shot_info);

	// --------------------------------------
	// for IRTV_S1
	// --------------------------------------
	QWidget *wdgIRTV = NULL;
	AttachChannelAccess *pAttach = NULL;
	
	wdgIRTV = (QWidget*)this->stackedWidget->widget(STACKED_IRTV_S1);

	if( !wdgIRTV ) {
		QMessageBox::information(0, "Kstar IRTV", tr("get StackedWidget ERROR") );
		return;
	}

	m_pIrtvS1Window = new IRTV_S1Window (wdgIRTV);
    pAttach = new AttachChannelAccess (wdgIRTV);

	stackedWidget->removeWidget (wdgIRTV);
	stackedWidget->insertWidget (STACKED_IRTV_S1, pAttach->GetWidget());

	// --------------------------------------
	// for IRTV_D1
	// --------------------------------------
	wdgIRTV = (QWidget*)this->stackedWidget->widget(STACKED_IRTV_D1);

	if( !wdgIRTV ) {
		QMessageBox::information(0, "Kstar IRTV", tr("get StackedWidget ERROR") );
		return;
	}

	m_pIrtvD1Window = new IRTV_D1Window (wdgIRTV);
    pAttach = new AttachChannelAccess (wdgIRTV);

	stackedWidget->removeWidget (wdgIRTV);
	stackedWidget->insertWidget (STACKED_IRTV_D1, pAttach->GetWidget());

	//on_btnIRTV_S1_clicked ();
	//this->stackedWidget->setCurrentIndex(STACKED_IRTV_S1);
}

void MainWindow::createActions ()
{
}

void MainWindow::createConnects ()
{
	connect (this, SIGNAL (signal_printStatus (char *)), this, SLOT (slot_printStatus (char *)));
}

void MainWindow::createToolbars()
{
#if 0	
	fileToolbar->addAction (actionNew);
	fileToolbar->addAction (actionOpen);
	fileToolbar->addAction (actionSave);
	fileToolbar->addAction (actionPrint);

	logoTB = new QToolBar(this);
	logoTB->setObjectName(QString::fromUtf8("logoToolBar"));

	logoTB->setOrientation(Qt::Horizontal);
	addToolBar(static_cast<Qt::ToolBarArea>(4), logoTB);

	QFrame *tbframe = new QFrame();
    logoTB->addWidget(tbframe);

    QHBoxLayout *tblayout = new QHBoxLayout(tbframe);
    tblayout->QLayout::setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    tblayout->setSpacing(0);
    tblayout->setMargin(0);
    tblayout->setObjectName(QString::fromUtf8("toolBarLayout"));

    QSpacerItem *tbspacer = new QSpacerItem(240, 10, QSizePolicy::Expanding, QSizePolicy::Fixed);
    QSpacerItem *tbspacer2 = new QSpacerItem(5, 5, QSizePolicy::Preferred, QSizePolicy::Fixed);


	CAGraphic *iocDDS1A = new CAGraphic();		// DDS1A
	iocDDS1A->setLineWidth(2);
	iocDDS1A->setMinimumSize(QSize(20,20));
	iocDDS1A->setMaximumSize(QSize(20,20));
	iocDDS1A->setFillColor(QColor("yellow"));
	iocDDS1A->setFillMode(StaticGraphic::Solid);
	iocDDS1A->setLineColor(QColor("black"));
	iocDDS1A->setPvname("DDS1A_HEARTBEAT");
	iocDDS1A->setFillDisplayMode(CAGraphic::ActInact);
	iocDDS1A->setObjectName("CAGraphic_iocDDS1A");

    QFont clockfont;
    clockfont.setPointSize(12);
    CAWclock *wclock1 = new CAWclock();
	wclock1->setMinimumSize(QSize(160,20));
	wclock1->setMaximumSize(QSize(160,20));
	wclock1->setPvname("CCS_SYS_TIME.RVAL");
	wclock1->setFont(clockfont);
	wclock1->setObjectName("CAWclock_wclock1");
	
    QLabel *logo = new QLabel("KSTAR logo");
    logo->setPixmap(QPixmap::fromImage(QImage(":images/DDS_kstar.png")));

    tblayout->addItem(tbspacer);
    tblayout->addWidget(wclock1);
    tblayout->addItem(tbspacer2);

	tblayout->addWidget(iocDDS1A);
	tblayout->addWidget(iocDDS1B);
	
	tblayout->addItem(tbspacer2);
	tblayout->addWidget(logo);
	
	AttachChannelAccess *pattachTLB = new AttachChannelAccess(tbframe);  
#endif
}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}

void MainWindow::changeEvent (QEvent *e)
{
	QMainWindow::changeEvent(e);

	switch (e->type()) {
		case QEvent::LanguageChange:
			m_pIrtvS1Window->retranslateUi(this);
			m_pIrtvD1Window->retranslateUi(this);
			break;
		default:
			break;
	}
}

void MainWindow::closeEvent (QCloseEvent *)
{
	qApp->quit();
}

void MainWindow::btnRelease ()
{
	btnIRTV_S1->setEnabled(true);
	btnIRTV_D1->setEnabled(true);
}

void MainWindow::on_btnIRTV_S1_clicked ()
{
	qDebug ("on_btnIRTV_S1_clicked");

	btnRelease ();
	btnIRTV_S1->setEnabled(false);
	stackedWidget->setCurrentIndex(STACKED_IRTV_S1);
}

void MainWindow::on_btnIRTV_D1_clicked ()
{
	qDebug ("on_btnIRTV_D1_clicked");

	btnRelease ();
	btnIRTV_D1->setEnabled(false);
	stackedWidget->setCurrentIndex(STACKED_IRTV_D1);
}

void MainWindow::showAbout ()
{
	QMessageBox *mbox = new QMessageBox();
#if defined(WIN32)
	mbox->setIconPixmap(QPixmap("c:/opi/images/TMS.xpm"));
#else
	mbox->setIconPixmap(QPixmap("/usr/local/opi/images/TMS.xpm"));
#endif
	mbox->setWindowTitle("About KSTAR IRTV");
	mbox->setText("KSTAR IRTV Operator Interface");
	mbox->setDetailedText("It is the operator interface for the KSTAR IRTV\n[Release 1.0.0]");
	mbox->exec();
}

void MainWindow::printStatus (const char *fmt, ... )
{
	char buf[1024];
	va_list argp;

	va_start(argp, fmt);
	vsprintf(buf, fmt, argp);
	va_end(argp);

	emit signal_printStatus (buf);
}

void MainWindow::slot_printStatus (char *buf)
{
	char strbuf[1024];
	int len;

	len = strlen(buf);
	if ( buf[len-1] == '\n' || (buf[len-1] == '\0')  ) buf[len-1] = 0L;

	QDateTime dtime = QDateTime::currentDateTime();	
	QString text = dtime.toString("[yyyy/MM/dd hh:mm:ss] ");

	strcpy (strbuf, text.toAscii().constData() );
	strcat (strbuf, buf);
	
	this->listWidget_status->addItem( strbuf );
		   
	int row = this->listWidget_status->count() ;
	this->listWidget_status->setCurrentRow (row - 1);

	if (row > 1000) {
		this->listWidget_status->clear();
	}
}

