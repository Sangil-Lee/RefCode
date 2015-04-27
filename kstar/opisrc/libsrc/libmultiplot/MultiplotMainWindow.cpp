#include "MultiplotMainWindow.h"
#include "Screenshot.h"
class QwtPlotLayout;

//MultiplotMainWindow::MultiplotMainWindow(const vector<string> &pvnames)
MultiplotMainWindow::MultiplotMainWindow()
{
	hide();
	//setWindowModality(Qt::ApplicationModal);
	//setMaximumSize(1280,1000);
	setMinimumSize(400,300);
	QToolBar *toolBar = new QToolBar(this);
	toolBar->setFixedHeight(45);
	toolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	QToolBar *toolBar1 = new QToolBar(this);
	//toolBar1->setFloatable(false);
	//toolBar1->setAllowedAreas(Qt::NoToolBarArea);
	toolBar1->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);

	QWidget *hbox = new QWidget(toolBar);
	QLabel *ymaxlabel = new QLabel("Y-Max", hbox);

	mymax = new QLineEdit(hbox);
	mymax->setMinimumSize(60,27);

	QLabel *yminlabel = new QLabel("Y-Min", hbox);
	mymin = new QLineEdit(hbox);
	mymin->setMinimumSize(60,27);

	mdate = new QLabel("Date", hbox);

	QPushButton *pGrp = new QPushButton("Group", hbox);
	pGrp->setMinimumSize(60,27);
	m_pZoom = new QPushButton("Zoom", hbox);
	m_pZoom->setCheckable(true);
	m_pZoom->setMinimumSize(60,27);
	m_pChpen = new QPushButton("Change Pen", hbox);
	m_pChpen->setMinimumSize(80,27);
	QPushButton *pScrprt = new QPushButton("Screen Print", hbox);
	pScrprt->setMinimumSize(80,27);
	QPushButton *pSetup = new QPushButton("Setup", hbox);
	pSetup->setMinimumSize(60,27);
	mpAutoscale = new QCheckBox("Auto Scale",hbox);

	QCheckBox *pLogscale = new QCheckBox("Log Scale",hbox);
	pLogscale->setCheckState(Qt::Checked);

	QHBoxLayout *hboxLayout = new QHBoxLayout(hbox);

	hboxLayout->setSpacing(3);
	hboxLayout->setMargin(3);

	hboxLayout->addWidget(pGrp);
	hboxLayout->addWidget(m_pZoom);
	hboxLayout->addWidget(m_pChpen);
	hboxLayout->addWidget(pScrprt);
	hboxLayout->addWidget(pSetup);
	hboxLayout->addWidget(mpAutoscale);
	hboxLayout->addWidget(pLogscale);
	hboxLayout->addWidget(ymaxlabel);
	hboxLayout->addWidget(mymax);
	hboxLayout->addWidget(yminlabel);
	hboxLayout->addWidget(mymin);
	hboxLayout->addWidget(new QWidget(hbox),1); // spacer;
	hboxLayout->addWidget(mdate);

	toolBar->addWidget(hbox);
	addToolBar(toolBar);

	m_plot = new MultiPlot(this);
	setCentralWidget(m_plot);

	connect(pGrp    , SIGNAL(clicked()), this, SLOT(Group()) );
	connect(m_pZoom , SIGNAL(clicked()), this, SLOT(Zoom()) );
	connect(m_pChpen , SIGNAL(clicked()), this, SLOT(ChangePen()) );
	connect(pScrprt , SIGNAL(clicked()), this, SLOT(ScreenPrint()) );
	connect(pSetup  , SIGNAL(clicked()), this, SLOT(Setup()) );
	connect(m_plot, SIGNAL(updateValue(int, QString)), this, SLOT(SetLabels(int, QString)));
	connect(m_plot, SIGNAL(updateDate(QString )), mdate, SLOT(setText(QString)) );
	connect(mymax, SIGNAL(returnPressed()), m_plot, SLOT(setYMaxSpan()) );
	connect(mymin, SIGNAL(returnPressed()), m_plot, SLOT(setYMinSpan()) );

    enableZoomMode(false);
    connect(m_pZoom, SIGNAL(toggled(bool)), SLOT(enableZoomMode(bool)));
	connect(mpAutoscale, SIGNAL(stateChanged(int)), SLOT(SetAutoScale(int)) );
	connect(pLogscale, SIGNAL(stateChanged(int)), SLOT(SetLogScale(int)) );
	mpAutoscale->setCheckState(Qt::Checked);
	pLogscale->setCheckState(Qt::Unchecked);

	show();

	mpLinearEngine = new QwtLinearScaleEngine;
	mpLog10Engine = new QwtLog10ScaleEngine;

	mHbox = new QWidget(toolBar1);
	mVboxLayout = new QVBoxLayout(mHbox);
	mVboxLayout->setSpacing(6);
	mVboxLayout->setMargin(0);
	for(int i = 0;i<4;i++)
	{
		mHboxLayout[i] = new QHBoxLayout();
		mHboxLayout[i]->setSpacing(6);
		mHboxLayout[i]->setMargin(0);
		mVboxLayout->addLayout(mHboxLayout[i]);
	};
	toolBar1->addWidget(mHbox);
	addToolBar(Qt::BottomToolBarArea, toolBar1);

};
MultiplotMainWindow::~MultiplotMainWindow()
{
	m_pvvalues.clear();
	m_pvdisplayers.clear();
}
void
MultiplotMainWindow::SetLogScale(int check)
{
	if(check == Qt::Checked)
	{
		//m_plot->setAxisScaleEngine(QwtPlot::yLeft, mpLog10Engine);
		m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLog10ScaleEngine);
	}
	else if(check == Qt::Unchecked)
	{
		//m_plot->setAxisScaleEngine(QwtPlot::yLeft, mpLinearEngine);
		m_plot->setAxisScaleEngine(QwtPlot::yLeft, new QwtLinearScaleEngine);
	};
}
void
MultiplotMainWindow::SetAutoScale(int check)
{
	if(check == Qt::Checked)
	{
		m_plot->SetAutoScale(true);
		m_plot->setAxisAutoScale(QwtPlot::yLeft);
		mymin->setDisabled(true);
		mymax->setDisabled(true);
	}
	else if(check == Qt::Unchecked)
	{
		mymin->setDisabled(false);
		mymax->setDisabled(false);
		m_plot->SetAutoScale(false);
	};
}
void
MultiplotMainWindow::SetEnableObject()
{
	for (int i = 0; i < MultiPlot::MAX_PV_CNT; i++)
	{
		if(m_checkBox[i]->checkState() == Qt::Checked)
		{
			m_lineEdit[i]->setDisabled(false);
			m_comboBox[PEN_WIDTH][i]->setDisabled(false);
			m_comboBox[PEN_STYLE][i]->setDisabled(false);
			m_comboBox[PEN_COLOR][i]->setDisabled(false);
		}
		else if(m_checkBox[i]->checkState() == Qt::Unchecked)
		{
			m_lineEdit[i]->setDisabled(true);
			m_comboBox[PEN_WIDTH][i]->setDisabled(true);
			m_comboBox[PEN_STYLE][i]->setDisabled(true);
			m_comboBox[PEN_COLOR][i]->setDisabled(true);
		};
	};
};
void
MultiplotMainWindow::CheckDisplayer()
{
#if 0
	QList<QCheckBox *> checkBoxes = findChildren<QCheckBox *>();
	int index = 0;
	for (int i = 0; i<checkBoxes.size(); ++i) 
	{
		QCheckBox *checkbox = (QCheckBox*)checkBoxes.at(i);
		if(checkbox->objectName().compare("") == 0) continue;
		index = checkbox->objectName().toInt();
		if(checkbox->checkState()==Qt::Unchecked)
		{
			m_plot->getCurve(index)->setVisible(false);
		}
		else if(checkbox->checkState()==Qt::Checked)
		{
			m_plot->getCurve(index)->setVisible(true);
		};
	};
#endif
	QCheckBox *checkBox = qobject_cast<QCheckBox *>(sender());
	int index = checkBox->objectName().toInt();
	if(checkBox->checkState()==Qt::Unchecked)
	{
		m_plot->getCurve(index)->setVisible(false);
	}
	else if(checkBox->checkState()==Qt::Checked)
	{
		m_plot->getCurve(index)->setVisible(true);
	};
	m_plot->replot();
}
void
MultiplotMainWindow::SetLabels(int index, QString pvvalue)
{
	m_pvvalues.at(index)->setText(pvvalue);
}
void
MultiplotMainWindow::Group()
{
	QString filename;
    QStringList files = QFileDialog::getOpenFileNames( this, tr("Group File Load"), filename,
                                tr("Text Files (*.grp)"));
    if (files.count()) 
	{
        filename = files[0];
		groupFileLoad(filename);
    };
}
void
MultiplotMainWindow::groupFileLoad(const QString filename)
{
	ifstream file(filename.toStdString().c_str());
	string strToken;
	if ( file.fail () ) return;

	vector<string> pvnames;
	int line = 0;
	mvec_pvsinfo.clear();
	while (!file.eof ())
	{
		getline (file, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str,"\t"))) continue;

		int pvnamesize = strlen(pch); 
		char pvname[pvnamesize];
		strncpy(pvname, pch, pvnamesize-1);

		qDebug("%s", pvname);
		pvnames.push_back(pvname); //PV Name

		int index = 0;
		int width = 0, style = 0;
		QString strColor, strStyle;
		while (pch != NULL)
		{
			if(!(pch = strtok (NULL,"\t"))) continue;
			qDebug("%s", pch);
			switch (index)
			{
				case PEN_WIDTH:
					width = QString(pch).toInt();	//Width
					break;
				case PEN_STYLE:
					style=0;
					strStyle = QString(pch).toLower();
					if(strStyle.compare("step")==0)			style = 0;
					else if(strStyle.compare("line")==0)	style = 1;
					else if(strStyle.compare("scatter")==0) style = 2;
					else if(strStyle.compare("stick")==0)	style = 3;
					break;
				case PEN_COLOR:
					strColor = QString(pch);
					break;
			};
			index++;
		};
		m_pvsinfo.check = Qt::Checked;
		m_pvsinfo.index = line; 
		m_pvsinfo.penwidth = width-1; 
		m_pvsinfo.penstyle = style; 
		m_pvsinfo.pencolor = strColor.toStdString();
		m_pvsinfo.pvname = pvname;
		mvec_pvsinfo.push_back(m_pvsinfo);
		line++;
	};
	file.close ();
	m_plot->MakePlotCurve(pvnames);
	MultiPlotPVInfo *pvsinfo = 0;
	for (size_t i = 0; i < mvec_pvsinfo.size(); i++)
	{
		pvsinfo = &mvec_pvsinfo.at(i);
		int idx = pvsinfo->index;
		m_plot->SetPenWidth (idx, pvsinfo->penwidth);
		m_plot->SetLineStyle(idx, pvsinfo->penstyle);
		m_plot->SetPenColor (idx, pvsinfo->pencolor.c_str());
	};
	showValue(pvnames);
}
void
MultiplotMainWindow::Zoom()
{
	QString freezestate = m_pZoom->text();
	if ( freezestate.compare("Zoom") == 0 )
	{
		m_plot->setZoom(true);
		m_pZoom->setText("Normal");
	}
	else if ( freezestate.compare("Normal") == 0 )
	{
		m_plot->BufferSwapData();
		m_plot->setZoom(false);
		m_pZoom->setText("Zoom");
	};
}
void
MultiplotMainWindow::ChangePen()
{
    QDialog setupPenDialog(this);
    setupPenDialog.setWindowTitle("MultiPlot - Change Pen");
	setupPenDialog.setMinimumSize(480, 245);
	setupPenDialog.setMaximumSize(480, 245);

    QWidget *widget;
    QHBoxLayout *hboxLayout;

    QPushButton *okButton;
    QPushButton *cancelButton;
    QPushButton *applyButton;

    QLabel *label;
    QLabel *label_5;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;

    widget = new QWidget(&setupPenDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(220, 190, 239, 29));
    hboxLayout = new QHBoxLayout(widget);
    hboxLayout->setSpacing(6);
    hboxLayout->setMargin(0);
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));

    applyButton = new QPushButton(widget);
    applyButton->setObjectName(QString::fromUtf8("applyButton"));
	connect(applyButton, SIGNAL(clicked()), SLOT(ApplyPen()));
    hboxLayout->addWidget(applyButton);


    okButton = new QPushButton(widget);
    okButton->setObjectName(QString::fromUtf8("okButton"));
	connect(okButton, SIGNAL(clicked()), &setupPenDialog, SLOT(accept()));
    hboxLayout->addWidget(okButton);
    cancelButton = new QPushButton(widget);
    cancelButton->setObjectName(QString::fromUtf8("cancelButton"));
	connect(cancelButton, SIGNAL(clicked()), &setupPenDialog, SLOT(reject()));
    hboxLayout->addWidget(cancelButton);

    mpenIndexCombo = new QComboBox(&setupPenDialog);
    mpenIndexCombo->setObjectName(QString::fromUtf8("IndexCombo"));
    mpenIndexCombo->setGeometry(QRect(1, 110, 150, 25));
	size_t indexcount = mvec_pvsinfo.size();

	QString strIdx;
	MultiPlotPVInfo *pvinfo = 0;
	for (size_t i = 0; i<indexcount; i++)
	{
		pvinfo = &mvec_pvsinfo.at(i);
		mpenIndexCombo->addItem(pvinfo->pvname.c_str());
	};

    mpenWidthCombo = new QComboBox(&setupPenDialog);
    mpenWidthCombo->setObjectName(QString::fromUtf8("WidthCombo"));
    mpenWidthCombo->setGeometry(QRect(180, 110, 72, 25));

    mpenWidthCombo->addItem("1");
    mpenWidthCombo->addItem("2");
    mpenWidthCombo->addItem("3");
    mpenWidthCombo->addItem("4");
    mpenWidthCombo->addItem("5");

    mpenStyleCombo = new QComboBox(&setupPenDialog);
    mpenStyleCombo->setObjectName(QString::fromUtf8("StyleCombo"));
    mpenStyleCombo->setGeometry(QRect(285, 110, 72, 25));

    mpenStyleCombo->addItem("Steps");
    mpenStyleCombo->addItem("Lines");
    mpenStyleCombo->addItem("Scatters");
    mpenStyleCombo->addItem("Sticks");

    mcolorButton = new QPushButton(&setupPenDialog);
    mcolorButton->setObjectName(QString::fromUtf8("colorButton"));
    mcolorButton->setGeometry(QRect(390, 110, 72, 25));
	QPalette pal(mcolorButton->palette());
	if( indexcount != 0 )
	{
		pvinfo = &mvec_pvsinfo.at(0);
		pal.setBrush(QPalette::Active,  QPalette::Button, QColor(pvinfo->pencolor.c_str()));
		pal.setBrush(QPalette::Inactive,QPalette::Button, QColor(pvinfo->pencolor.c_str()));
		mcolorButton->setPalette(pal);
	}
	connect(mcolorButton, SIGNAL(clicked()), SLOT(ColorDialog()));

    label = new QLabel(&setupPenDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(0, 10, 471, 41));
    QFont font;
    font.setPointSize(20);
    font.setBold(false);
    font.setWeight(50);
    label->setFont(font);
    label->setAlignment(Qt::AlignCenter);
    label_2 = new QLabel(&setupPenDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(42, 82, 80, 17));
    label_3 = new QLabel(&setupPenDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(180, 82, 80, 17));
    label_4 = new QLabel(&setupPenDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(285, 82, 80, 17));
    label_5 = new QLabel(&setupPenDialog);
    label_5->setObjectName(QString::fromUtf8("label_5"));
    label_5->setGeometry(QRect(390, 82, 80, 17));

    setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
    okButton->setText(QApplication::translate("Form", "&O K", 0, QApplication::UnicodeUTF8));
    cancelButton->setText(QApplication::translate("Form", "&CANCEL", 0, QApplication::UnicodeUTF8));
    applyButton->setText(QApplication::translate("Form", "&APPLY", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("Form", "SETUP PEN ", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("Form", "Pen Index", 0, QApplication::UnicodeUTF8));
    label_3->setText(QApplication::translate("Form", "Pen Width", 0, QApplication::UnicodeUTF8));
    label_4->setText(QApplication::translate("Form", "Pen Style", 0, QApplication::UnicodeUTF8));
    label_5->setText(QApplication::translate("Form", "Pen Color", 0, QApplication::UnicodeUTF8));
    if(setupPenDialog.exec()) 
	{
		ApplyPen();
	};
}
void
MultiplotMainWindow::ZoomOut()
{
	QMessageBox::information(this, "ZoomOut", "ZoomOut Module will be called");
}
void
MultiplotMainWindow::ApplyPen()
{
	if (m_pvdisplayers.empty() == true) return;
    int index = mpenIndexCombo->currentIndex();
	int width = mpenWidthCombo->currentIndex();
	int style = mpenStyleCombo->currentIndex();
	QString strcolor = mcolorButton->palette().color(QPalette::Button).name();
	m_plot->SetPenWidth (index, width);
	m_plot->SetLineStyle(index, style);
	m_plot->SetPenColor (index, strcolor);
	QPalette pal(m_pvdisplayers.at(index)->palette());
	pal.setBrush(QPalette::Active,  QPalette::WindowText, QColor(strcolor));
	pal.setBrush(QPalette::Inactive,QPalette::WindowText, QColor(strcolor));
	m_pvdisplayers.at(index)->setPalette(pal);

#if 0
	MultiPlotPVInfo *pvinfo = &mvec_pvsinfo.at(index);
	mpenIndexCombo->addItem(pvinfo->pvname.c_str());
	pvinfo->penwidth = width; 
	pvinfo->penstyle = style; 
	pvinfo->pencolor = strcolor.toStdString();
#endif
}
void
MultiplotMainWindow::ColorDialog()
{
	QPushButton *colorButton = qobject_cast<QPushButton *>(sender());
    QColor color = QColorDialog::getColor(Qt::green, this);
    if (color.isValid()) 
	{
        colorButton->setPalette(QPalette(color));
        colorButton->setAutoFillBackground(true);
    }
}
void
MultiplotMainWindow::ScreenPrint()
{
	//QMessageBox::information(this, "ScreenPrint", "ScreenPrint Module will be called");
	Screenshot *screenshot = new Screenshot();
	screenshot -> show();
}
void
MultiplotMainWindow::SaveWindow()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("SaveFile"),"",
                                tr("Text Files (*.grp)"));
	if(!fileName.isEmpty())
	{
		fileName.append(".grp");
		saveFile(fileName);
	};
}
void
MultiplotMainWindow::Reset()
{
	for(int i = 0; i<MultiPlot::MAX_PV_CNT;i++)
	{
		m_checkBox[i]->setCheckState(Qt::Unchecked);
		m_lineEdit[i]->setText("");
		m_lineEdit[i]->setDisabled(true);
	};
	for(int i=0; i<3; i++)
	{
		for (int j=0; j<MultiPlot::MAX_PV_CNT; j++)
		{
			m_comboBox[i][j]->setCurrentIndex(0);
			m_comboBox[i][j]->setDisabled(true);
		};
	};
}
void
MultiplotMainWindow::OpenWindow()
{
	QString filename;
    QStringList files = QFileDialog::getOpenFileNames( this, tr("OpenFile"), filename,
                                tr("Text Files (*.grp)"));
    if (files.count()) 
	{
        filename = files[0];
		openFile(filename);
    };
}
void
MultiplotMainWindow::openFile(const QString filename)
{
	ifstream file(filename.toStdString().c_str());
	string strToken;
	if ( file.fail () ) return;

	int line = 0;
	while (!file.eof ())
	{
		getline (file, strToken);
		if(strToken[0] == '#' || strToken.empty()==true) continue;

		char str[strToken.size()];
		strcpy (str, strToken.c_str());

		char *pch = 0;
		if(!(pch = strtok (str,"\t"))) continue;

		int pvnamesize = strlen(pch); 
		char pvname[pvnamesize];
		strncpy(pvname, pch, pvnamesize-1);

		m_lineEdit[line]->setText(pvname);
		m_checkBox[line]->setCheckState(Qt::Checked);

		qDebug("%s", pvname);
		int index = 0;
		while (pch != NULL)
		{
			if(!(pch = strtok (NULL,"\t"))) continue;
			qDebug("%s", pch);
			switch (index)
			{
				case PEN_WIDTH:
					{
						int width = m_comboBox[index][line]->findText(pch);
						m_comboBox[index][line]->setCurrentIndex(width);
					}
					break;
				case PEN_STYLE:
					{
						int style = m_comboBox[index][line]->findText(pch);
						m_comboBox[index][line]->setCurrentIndex(style);
					}
					break;
				case PEN_COLOR:
					{
						int color = m_comboBox[index][line]->findText(pch);
						m_comboBox[index][line]->setCurrentIndex(color);
					}
					break;
			};
			index++;
		};
		line++;
	};
	file.close ();
}
void
MultiplotMainWindow::saveFile(const QString filename)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly|QIODevice::Text);
	QString linestr = "#PV name:\tLine width\tLine style\tLine color";
	int len=qstrlen(linestr.toStdString().c_str());
	file.write(linestr.toStdString().c_str(), len);
	file.write("\n",1);
	QString strCombo;
	for (int i = 0; i < MultiPlot::MAX_PV_CNT; i++)
	{
		linestr="";
		if(m_checkBox[i]->checkState() == Qt::Checked)
		{
			linestr = m_lineEdit[i]->text();
			linestr.append(":\t");
			strCombo = m_comboBox[PEN_WIDTH][i]->currentText();
			linestr.append(strCombo);
			linestr.append("\t");
			strCombo = m_comboBox[PEN_STYLE][i]->currentText();
			linestr.append(strCombo);
			linestr.append("\t");
			strCombo = m_comboBox[PEN_COLOR][i]->currentText();
			linestr.append(strCombo);
		};
		len = qstrlen(linestr.toStdString().c_str());
		file.write(linestr.toStdString().c_str(), len);
		file.write("\n", 1);
	};
	file.close();
};
void
MultiplotMainWindow::Setup()
{
    QDialog setupDialog(this);
    setupDialog.setWindowTitle("MultiPlot - Setup");
	setupDialog.setMinimumSize(680, 600);
	setupDialog.setMaximumSize(680, 600);

    QWidget *widget = new QWidget(&setupDialog);
    widget->setObjectName(QString::fromUtf8("widget"));
    widget->setGeometry(QRect(150, 90, 150, 411));

    QVBoxLayout *vboxLayout = new QVBoxLayout(widget);
    vboxLayout->setSpacing(6);
    vboxLayout->setMargin(0);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));

	QString objname;
	for(int i = 0; i<MultiPlot::MAX_PV_CNT;i++)
	{
		m_lineEdit[i] = new QLineEdit(widget);
		objname = QString("lineEdit_%1").arg(i);
		m_lineEdit[i]->setObjectName(objname);
		m_lineEdit[i]->setDisabled(true);
		vboxLayout->addWidget(m_lineEdit[i]);
	};

    QWidget *widget1 = new QWidget(&setupDialog);
    widget1->setObjectName(QString::fromUtf8("widget1"));
    widget1->setGeometry(QRect(125, 83, 25, 421));

    QVBoxLayout *vboxLayout1 = new QVBoxLayout(widget1);
    vboxLayout1->setSpacing(6);
    vboxLayout1->setMargin(0);
    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));

	for(int i = 0; i<MultiPlot::MAX_PV_CNT;i++)
	{
		m_checkBox[i] = new QCheckBox(widget);
		objname = QString("checkBox_%1").arg(i);
		m_checkBox[i]->setObjectName(objname);
		connect(m_checkBox[i], SIGNAL(stateChanged(int)), this, SLOT(SetEnableObject()) );
		vboxLayout1->addWidget(m_checkBox[i]);
	};

    QFont font;
    font.setPointSize(10);
    font.setBold(false);
    font.setWeight(50);
    QLabel *label;
    label = new QLabel(&setupDialog);
    label->setObjectName(QString::fromUtf8("label"));
    label->setGeometry(QRect(150, 60, 111, 25));
    label->setFont(font);
    label->setLayoutDirection(Qt::LeftToRight);
    label->setFrameShape(QFrame::Box);
    label->setFrameShadow(QFrame::Sunken);
    label->setAlignment(Qt::AlignCenter);
    label->setText("PV Name");

    QLabel *label_2;
    label_2 = new QLabel(&setupDialog);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setGeometry(QRect(291, 61, 76, 23));
    label_2->setFont(font);
    label_2->setLayoutDirection(Qt::LeftToRight);
    label_2->setFrameShape(QFrame::Box);
    label_2->setFrameShadow(QFrame::Sunken);
    label_2->setAlignment(Qt::AlignCenter);
    label_2->setText("Pen Width");

    QLabel *label_3;
    label_3 = new QLabel(&setupDialog);
    label_3->setObjectName(QString::fromUtf8("label_3"));
    label_3->setGeometry(QRect(373, 61, 75, 23));
    label_3->setFont(font);
    label_3->setLayoutDirection(Qt::LeftToRight);
    label_3->setFrameShape(QFrame::Box);
    label_3->setFrameShadow(QFrame::Sunken);
    label_3->setAlignment(Qt::AlignCenter);
    label_3->setText("Pen Style");

    QLabel *label_4;
    label_4 = new QLabel(&setupDialog);
    label_4->setObjectName(QString::fromUtf8("label_4"));
    label_4->setGeometry(QRect(454, 61, 76, 23));
    label_4->setFont(font);
    label_4->setLayoutDirection(Qt::LeftToRight);
    label_4->setFrameShape(QFrame::Box);
    label_4->setFrameShadow(QFrame::Sunken);
    label_4->setAlignment(Qt::AlignCenter);
    label_4->setText("Pen Color");

    QWidget *widget2 = new QWidget(&setupDialog);
    widget2->setObjectName(QString::fromUtf8("widget2"));
    widget2->setGeometry(QRect(310, 89, 231, 411));

    QGridLayout *gridLayout = new QGridLayout(widget2);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

	for(int i=0; i<3; i++)
	{
		for (int j=0; j<MultiPlot::MAX_PV_CNT; j++)
		{
			m_comboBox[i][j]=new QComboBox(widget2);
			objname = QString("comboBox_%1").arg(i);
			m_comboBox[i][j]->setObjectName(objname);
			gridLayout->addWidget(m_comboBox[i][j], j, i, 1, 1);
			QString strItem="";
			if (i == 0)
			{
				for(int k=0;k<5;k++)
				{
					strItem = QString("%1").arg(k+1);
					m_comboBox[i][j]->addItem(strItem);
				};
			}
			else if (i == 1)
			{
				m_comboBox[i][j]->addItem("Step");
				m_comboBox[i][j]->addItem("Line");
				m_comboBox[i][j]->addItem("Scatter");
			}
			else if (i == 2)
			{
				m_comboBox[i][j]->addItem("White");
				m_comboBox[i][j]->addItem("Black");
				m_comboBox[i][j]->addItem("Red");
				m_comboBox[i][j]->addItem("Dark red");
				m_comboBox[i][j]->addItem("Green");
				m_comboBox[i][j]->addItem("Dark green");
				m_comboBox[i][j]->addItem("Blue");
				m_comboBox[i][j]->addItem("Dark Blue");
				m_comboBox[i][j]->addItem("Cyan");
				m_comboBox[i][j]->addItem("Dark Cyan");
				m_comboBox[i][j]->addItem("Magenta");
				m_comboBox[i][j]->addItem("Dark Magenta");
				m_comboBox[i][j]->addItem("Yellow");
				m_comboBox[i][j]->addItem("Dark Yellow");
				m_comboBox[i][j]->addItem("Gray");
				m_comboBox[i][j]->addItem("Dark Gray");
			};
			m_comboBox[i][j]->setDisabled(true);
		};
	};

    QLabel *label_time;
    label_time = new QLabel(&setupDialog);
    label_time->setGeometry(QRect(150, 510, 130, 25));
    label_time->setFont(font);
    label_time->setLayoutDirection(Qt::LeftToRight);
    label_time->setAlignment(Qt::AlignCenter);
    label_time->setText("Time Interval(h,m,s): ");

	m_sbh = new QSpinBox(&setupDialog);
	m_sbh->setObjectName("HourInterval");
	m_sbh->setRange(0, 23);
	m_sbh->setValue(0);
    m_sbh->setGeometry(QRect(285, 510, 50, 25));

	m_sbm = new QSpinBox(&setupDialog);
	m_sbm->setObjectName("MinInterval");
	m_sbm->setRange(0, 59);
	m_sbm->setValue(0);
    m_sbm->setGeometry(QRect(340, 510, 50, 25));

	m_sbs = new QSpinBox(&setupDialog);
	m_sbs->setObjectName("SecInterval");
	m_sbs->setRange(0, 59);
	m_sbs->setValue(1);
    m_sbs->setGeometry(QRect(395, 510, 50, 25));

    QPushButton *saveButton;
    saveButton = new QPushButton(&setupDialog);
    saveButton->setObjectName("saveButton");
    saveButton->setGeometry(QRect(110, 550, 91, 31));
    saveButton->setText("&SaveFile");
	connect(saveButton, SIGNAL(clicked()), this, SLOT(SaveWindow()));

    QPushButton *openButton;
    openButton = new QPushButton(&setupDialog);
    openButton->setObjectName("openButton");
    openButton->setGeometry(QRect(210, 550, 91, 31));
    openButton->setText("&OpenFile");
	connect(openButton, SIGNAL(clicked()), this, SLOT(OpenWindow()));

    QPushButton *resetButton;
    resetButton = new QPushButton(&setupDialog);
    resetButton->setObjectName("resetButton");
    resetButton->setGeometry(QRect(420, 550, 91, 31));
    resetButton->setText("&Reset");
	connect(resetButton, SIGNAL(clicked()), this, SLOT(Reset()));

    QPushButton *okButton;
    okButton = new QPushButton(&setupDialog);
    okButton->setObjectName(QString::fromUtf8("okButton"));
    okButton->setGeometry(QRect(320, 550, 91, 31));
    okButton->setText("&O K");
	connect(okButton, SIGNAL(clicked()), &setupDialog, SLOT(accept()));

    QPushButton *closeButton;
    closeButton = new QPushButton(&setupDialog);
    closeButton->setObjectName("closeButton");
    closeButton->setGeometry(QRect(560, 550, 91, 31));
    closeButton->setText("&Close");
	connect(closeButton, SIGNAL(clicked()), &setupDialog, SLOT(reject()));

	vector<string> pvnames;
	QWidget *chboxlayw = new QWidget(this);
	chboxlayw->setGeometry(QRect(0, 950, 1280, 35));
	QVBoxLayout *vcheckboxlay;
	vcheckboxlay = new QVBoxLayout(chboxlayw);
	vcheckboxlay->setSpacing(6);
	vcheckboxlay->setMargin(0);

	QHBoxLayout *hcheckboxlay = new QHBoxLayout();
	hcheckboxlay->setSpacing(6);
	hcheckboxlay->setMargin(0);

	QHBoxLayout *hcheckboxlay2 = new QHBoxLayout();
	hcheckboxlay2->setSpacing(6);
	hcheckboxlay2->setMargin(0);

	readSetInfo();
    if(setupDialog.exec()) 
	{
		mvec_pvsinfo.clear();
		for (int i = 0;i<MultiPlot::MAX_PV_CNT; i++)
		{
			if(m_checkBox[i]->checkState()==Qt::Checked)
			{
				//PV Name
				pvnames.push_back(m_lineEdit[i]->text().toStdString());
			};
		};
		m_plot->MakePlotCurve(pvnames);

		int hour = m_sbh->value();
		int min = m_sbm->value();
		int sec = m_sbs->value();
		int timeinterval = hour*3600+min*60+sec;
		m_plot->SetTimeInterval(timeinterval);
		for (int index = 0;index<MultiPlot::MAX_PV_CNT; index++)
		{
			if(m_checkBox[index]->checkState()==Qt::Checked)
			{
				int width = m_comboBox[PEN_WIDTH][index]->currentIndex(); // Width
				int style = m_comboBox[PEN_STYLE][index]->currentIndex(); // Style
				QString strcolor = m_comboBox[PEN_COLOR][index]->currentText(); // Colorstr
				m_plot->SetPenWidth (index, width);
				m_plot->SetLineStyle(index, style);
				m_plot->SetPenColor (index, strcolor);
				m_pvsinfo.check = Qt::Checked;
				m_pvsinfo.index = index; 
				m_pvsinfo.penwidth = width; 
				m_pvsinfo.penstyle = style; 
				m_pvsinfo.pencolor = strcolor.toStdString();
				m_pvsinfo.pvname = m_lineEdit[index]->text().toStdString();
				mvec_pvsinfo.push_back(m_pvsinfo);
			};
		};
		showValue(pvnames);
    };
}

void
MultiplotMainWindow::readSetInfo()
{
	size_t pvinfsize = mvec_pvsinfo.size();
	if (pvinfsize == 0) return;

	MultiPlotPVInfo *pvsinfo = 0;
	for (size_t i = 0; i < pvinfsize; i++)
	{
		pvsinfo = &mvec_pvsinfo.at(i);
		int idx = pvsinfo->index;
		m_checkBox[idx]->setCheckState(Qt::Checked);
		m_comboBox[PEN_WIDTH][idx]->setCurrentIndex(pvsinfo->penwidth);	// Width
		m_comboBox[PEN_STYLE][idx]->setCurrentIndex(pvsinfo->penstyle);	// Style
		int colindex = m_comboBox[PEN_COLOR][idx]->findText(pvsinfo->pencolor.c_str());
		m_comboBox[PEN_COLOR][idx]->setCurrentIndex(colindex);	// Color
		m_lineEdit[idx]->setText(pvsinfo->pvname.c_str());		//PV Name
	};
}
void
MultiplotMainWindow::showValue(const vector<string> &pvnames)
{
#if 1
	resetVBoxLayout();
	QString strCheckbox;
	MultiPlotPVInfo *pvsinfo = 0;
	for ( size_t i = 0; i < pvnames.size(); i++)
	{
		m_pvdisplayers.push_back(new QCheckBox(pvnames.at(i).c_str()));
		m_pvdisplayers.at(i)->setFont(QFont("Sans Serif", 7));
		m_pvdisplayers.at(i)->setCheckState(Qt::Checked);
		pvsinfo = &mvec_pvsinfo.at(i);
		QPalette pal(m_pvdisplayers.at(i)->palette());
		pal.setBrush(QPalette::Active,  QPalette::WindowText, QColor(pvsinfo->pencolor.c_str()));
		pal.setBrush(QPalette::Inactive,QPalette::WindowText, QColor(pvsinfo->pencolor.c_str()));
		m_pvdisplayers.at(i)->setPalette(pal);
		strCheckbox = QString("%1").arg(i);
		m_pvdisplayers.at(i)->setObjectName(strCheckbox);

		connect(m_pvdisplayers.at(i), SIGNAL(stateChanged(int)), this, SLOT(CheckDisplayer()) );
		if ( i <= 4)
		{
			mHboxLayout[0]->addWidget(m_pvdisplayers.at(i));
			m_pvvalues.push_back(new QLabel(""));
			m_pvvalues.at(i)->setFont(QFont("Sans Serif", 9));
			mHboxLayout[1]->addWidget(m_pvvalues.at(i));

		}
		else
		{
			mHboxLayout[2]->addWidget(m_pvdisplayers.at(i));
			m_pvvalues.push_back(new QLabel(""));
			m_pvvalues.at(i)->setFont(QFont("Sans Serif", 9));
			mHboxLayout[3]->addWidget(m_pvvalues.at(i));
		};
	};
	mHbox->show();
#endif
	m_plot->Execute(pvnames);
}
void
MultiplotMainWindow::resetVBoxLayout()
{
	size_t cnt = m_pvdisplayers.size();
	if (cnt == 0) return;
	for ( size_t i = 0; i < cnt; i++)
	{
		//mHboxLayout->removeWidget (m_pvdisplayers.at(i));
		//mHboxLayout2->removeWidget(m_pvvalues.at(i));
		delete m_pvdisplayers.at(i);
		delete m_pvvalues.at(i);
	};
	m_pvdisplayers.clear();
	m_pvvalues.clear();
}
void
MultiplotMainWindow::enableZoomMode(bool on)
{
    m_plot->Zoomer()->setEnabled(on);
    m_plot->Zoomer()->zoom(0);
};
