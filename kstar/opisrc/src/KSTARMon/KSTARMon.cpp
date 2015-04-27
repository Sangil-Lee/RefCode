#include "KSTARMon.h"
#include "MainWindow.h"

KSTARMonForm::KSTARMonForm(QWidget *pWidget):QWidget(pWidget)
{
	qDebug("Called setupUI");
	pForm = pWidget;
	setupUi(pWidget);
};

KSTARMonForm::~KSTARMonForm()
{
	qDebug("deleted form");
};

void KSTARMonForm::connectSignal()
{
	connect(systemCB,SIGNAL(currentIndexChanged(int)),this,SLOT(updateListSlot(int)));
	connect(showPagePB,SIGNAL(clicked()),this,SLOT(showPageSlot()));
	connect(closeBT,SIGNAL(clicked()),qApp,SLOT(quit()));
	connect(pagesLV,SIGNAL(itemDoubleClicked(QListWidgetItem *)),this,SLOT(showPageByWidgetDoubleclick(QListWidgetItem * )));

	systemCB->setCurrentIndex(0);
}

void  KSTARMonForm::showPageByWidgetDoubleclick( QListWidgetItem * pItem )
{
	int curindex = systemCB->currentIndex();
	int curpage = pagesLV->currentRow();

	if(!pItem || pItem->isSelected() == 0) return;
	showPage(curindex, curpage);
}

void KSTARMonForm::showPageSlot()
{
	if(!systemCB||!pagesLV) return;

	int curindex = systemCB->currentIndex();
	int curpage = pagesLV->currentRow();
	QListWidgetItem *pItem = pagesLV->currentItem();

	if(!pItem || pItem->isSelected() == 0) return;

	//qDebug("curIndex:%d, curPage:%d, isSelected:%d", curindex, curpage,pItem->isSelected());
	showPage(curindex, curpage);
};

QString KSTARMonForm::openPage(const int system, const int pageindex)
{
	QString pagename;

	if(system == TMS && pageindex == 0)
		pagename = "TMS_Main.ui";
	else if(system == TMS && pageindex == 1)
		pagename = "TMS_TF_Heline.ui";
	else if(system == TMS && pageindex == 2)
		pagename = "TMS_TF_Intercoil_Busline.ui";
	else if(system == TMS && pageindex == 3)
		pagename = "TMS_TF_Structure_u1.ui";
	else if(system == TMS && pageindex == 4)
		pagename = "TMS_TF_Structure_u2.ui";
	else if(system == TMS && pageindex == 5)
		pagename = "TMS_PF_Heline.ui";
	else if(system == TMS && pageindex == 6)
		pagename = "TMS_PF_Busline.ui";
	else if(system == TMS && pageindex == 7)
		pagename = "TMS_PF_Structure.ui";
	else if(system == TMS && pageindex == 8)
		pagename = "TMS_CS_Structure.ui";
	else if(system == TMS && pageindex == 9)
		pagename = "TMS_Gravity_Support.ui";
	else if(system == TMS && pageindex == 10)
		pagename = "TMS_Thermal_Shield.ui";
	else if(system == TMS && pageindex == 11)
		pagename = "TMS_Bus_Support.ui";
	else if(system == TMS && pageindex == 12)
		pagename = "TMS_Leaf_Spring.ui";
	else if(system == HDS && pageindex == 0)
		pagename = "HDS_Vacuum_Status.ui";
	else if(system == HDS && pageindex == 1)
		pagename = "HDS_VPS_rtchart.ui";
	else if(system == HDS && pageindex == 2)
		pagename = "HDS_HCS_TF.ui";
	else if(system == HDS && pageindex == 3)
		pagename = "HDS_HCS_PF.ui";
	else if(system == HDS && pageindex == 4)
		pagename = "HDS_HCS_TS.ui";
	else if(system == HDS && pageindex == 5)
		pagename = "HDS_HCS_ShieldTemp.ui";
	else if(system == HDS && pageindex == 6)
		pagename = "HDS_HCS_PFCoil.ui";
	else if(system == HDS && pageindex == 7)
		pagename = "HDS_HCS_PFBus.ui";
	else if(system == PMS && pageindex == 0)
		pagename = "PMS_Divertor.ui";
	else if(system == PMS && pageindex == 1)
		pagename = "PMS_Limiter.ui";
	else if(system == PMS && pageindex == 2)
		pagename = "PMS_NBarmor.ui";
	else if(system == PMS && pageindex == 3)
		pagename = "PMS_IVCC.ui";
	else if(system == PMS && pageindex == 4)
		pagename = "PMS_In_Board.ui";
	else if(system == PMS && pageindex == 5)
		pagename = "PMS_Out_Board.ui";
	else if(system == VMS && pageindex == 0)
		pagename = "VMS_vacuum_main.ui";
	else if(system == VMS && pageindex == 1)
		pagename = "VMS_cr_status.ui";
	else if(system == VMS && pageindex == 2)
		pagename = "VMS_cr_rtchart.ui";
	else if(system == VMS && pageindex == 3)
		pagename = "VMS_cr_control.ui";
	else if(system == VMS && pageindex == 4)
		pagename = "VMS_vv_status.ui";
	else if(system == VMS && pageindex == 5)
		pagename = "VMS_vv_rtchart.ui";
	else if(system == VMS && pageindex == 6)
		pagename = "VMS_vv_control.ui";
	else if(system == VMS && pageindex == 7)
		pagename = "VMS_crp_detail.ui";
	else if(system == VMS && pageindex == 8)
		pagename = "VMS_jacket_heater.ui";
	else if(system == VMS && pageindex == 9)
		pagename = "VMS_RGA.ui";
	else if(system == CLS && pageindex == 0)
		pagename = "CLS_vacuum_status.ui";
	else if(system == CLS && pageindex == 1)
		pagename = "CLS_vacuum_rtchart.ui";
	else if(system == CLS && pageindex == 2)
		pagename = "CLS_tfhcs_status.ui";
	else if(system == CLS && pageindex == 3)
		pagename = "CLS_pfhcs_status.ui";
	else if(system == CLS && pageindex == 4)
		pagename = "CLS_ts_status.ui";
	else if(system == CLS && pageindex == 5)
		pagename = "CLS_heater_control.ui";
	else if(system == CLS && pageindex == 6)
		pagename = "CLS_tf_valve_control.ui";
	else if(system == CLS && pageindex == 7)
		pagename = "CLS_pf_valve_control.ui";
	else if(system == Fuel && pageindex == 0)
		pagename = "Fuel_main.ui";
	else if(system == Fuel && pageindex == 1)
		pagename = "Fuel_piezo_status.ui";
	else if(system == Fuel && pageindex == 2)
		pagename = "Fuel_DAQ.ui";
	else if(system == Fuel && pageindex == 3)
		pagename = "GDC_main.ui";
	else if(system == Fuel && pageindex == 4)
		pagename = "Fuel_SMBI_MGI.ui";
	else if(system == Fuel && pageindex == 5)
		pagename = "Fuel_SMBI_DAQ.ui";
	else if(system == Fuel && pageindex == 6)
		pagename = "Fuel_MGI_status.ui";
	else if(system == ECH && pageindex == 0)
		pagename = "ECH_Status.ui";
	else if(system == ECH && pageindex == 1)
		pagename = "ECH_Fault_List.ui";
	else if(system == ECH && pageindex == 2)
		pagename = "ECH_NIDAQcontrol.ui";
	else if(system == ECH && pageindex == 3)
		pagename = "ECH_Waveform2.ui";
	else if(system == ICRH && pageindex == 0)
		pagename = "ICRH_main.ui";
	else if(system == ICRH && pageindex == 1)
		pagename = "ICRH_Status.ui";
	else if(system == ICRH && pageindex == 2)
		pagename = "ICRH_DAQ.ui";
	else if(system == ICP && pageindex == 0)
		pagename = "icp.ui";
	else if(system == ICP && pageindex == 1)
		pagename = "hds_vibrate.ui";
	else if(system == GCDS && pageindex == 0)
		pagename = "GCDS_DAQ.ui";
	else if(system == MPS && pageindex == 0)
		pagename = "MPS_Monitoring.ui";
	else if(system == MPS && pageindex == 1)
		pagename = "MPS_Operation.ui";
	else if(system == NB1_CAL && pageindex == 0)
		pagename = "NB1_Calorimetric.ui";
	else if(system == NB1_VAC && pageindex == 0)
		pagename = "nb1_vac.ui";
	else if(system == SCS && pageindex == 0)
		pagename = "SCS_Machine_Status.ui";
	else if(system == HDS_VIB && pageindex == 0)
		pagename = "tf_circuit.ui";
	else if(system == HDS_VIB && pageindex == 1)
		pagename = "pf_circuit.ui";
	else if(system == HDS_VIB && pageindex == 2)
		pagename = "transfer_line.ui";
	else if(system == HDS_VIB && pageindex == 3)
		pagename = "icp.ui";
	return pagename;
};

int KSTARMonForm::showPage(const int system, const int pageindex)
{
	QString uipage = openPage(system,pageindex);
	//QString uifile = "/usr/local/opi/ui/"+uipage;
	QString uifile = "../ui/"+uipage;
	QFile filecheck(uifile);
	if(filecheck.exists()==false) 
	{
		QMessageBox::information(this, "File Open Error!!", "File Not Existed!");
		return -1;
	};
	MainWindow *pWin = new MainWindow(uipage);
	pWin->show();
	//MainWindow mainwindow(uifile);
	//mainwindow.show();
	return 0;
};

void KSTARMonForm::updateListSlot(int index)
{
    pagesLV->clear();
	switch(index)
	{
		case TMS:
			pagesLV->addItems(tmspages);
			break;
		case VMS:
			pagesLV->addItems(vmspages);
			break;
		case HDS:
			pagesLV->addItems(hdspages);
			break;
		case ECH:
			pagesLV->addItems(echpages);
			break;
		case Fuel:
			pagesLV->addItems(fuelpages);
			break;
		case CLS:
			pagesLV->addItems(clspages);
			break;
		case ICRH:
			pagesLV->addItems(icrhpages);
			break;
		case ICP:
			pagesLV->addItems(icppages);
			break;
		case GCDS:
			pagesLV->addItems(gcdspages);
			break;
		case PMS:
			pagesLV->addItems(pmspages);
			break;
		case MPS:
			pagesLV->addItems(mpspages);
			break;
		case NB1_CAL:
			pagesLV->addItems(nb1calpages);
			break;
		case NB1_VAC:
			pagesLV->addItems(nb1vacpages);
			break;
		case SCS:
			pagesLV->addItems(scspages);
			break;
		case HDS_VIB:
			pagesLV->addItems(hdsvibpages);
			break;
		default:
			break;
	};
}

void KSTARMonForm::retranslateUi(QWidget *Form)
{
	Form->setWindowTitle(QApplication::translate("Form", "Form", 0, QApplication::UnicodeUTF8));
	label->setText(QApplication::translate("Form", "KSTAR Monitoring Tool", 0, QApplication::UnicodeUTF8));
	systemCB->clear();
	systemCB->insertItems(0, QStringList()
			<< QApplication::translate("Form", "TMS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "VMS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "HDS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "ECH", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "Fuel", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "CLS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "ICRH", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "ICP", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "GCDS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "PMS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "MPS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "NB1_CAL", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "NB1_VAC", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "SCS", 0, QApplication::UnicodeUTF8)
			<< QApplication::translate("Form", "HDS_VIB", 0, QApplication::UnicodeUTF8)
			);
	showPagePB->setText(QApplication::translate("Form", "Show", 0, QApplication::UnicodeUTF8));
	closeBT->setText(QApplication::translate("Form", "Close", 0, QApplication::UnicodeUTF8));
	Q_UNUSED(Form);
} 

void KSTARMonForm::setupUi(QWidget *Form)
{
	if (Form->objectName().isEmpty())
		Form->setObjectName(QString::fromUtf8("Form"));
	Form->resize(477, 301);
	label = new QLabel(Form);
	label->setObjectName(QString::fromUtf8("label"));
	label->setGeometry(QRect(110, 10, 251, 41));
	QFont font;
	font.setPointSize(17);
	label->setFont(font);
	label->setAlignment(Qt::AlignCenter);
	systemCB = new QComboBox(Form);
	systemCB->setObjectName(QString::fromUtf8("systemCB"));
	systemCB->setGeometry(QRect(10, 90, 101, 26));
	QFont font1;
	font1.setPointSize(12);
	systemCB->setFont(font1);
	//closeBT = new QDialogButtonBox(Form);
	closeBT = new QPushButton(Form);
	closeBT->setObjectName(QString::fromUtf8("closeBT"));
	closeBT->setGeometry(QRect(360, 260, 81, 32));
	QFont font2;
	font2.setPointSize(10);
	font2.setBold(false);
	font2.setWeight(50);
	closeBT->setFont(font2);
	//closeBT->setLayoutDirection(Qt::RightToLeft);
	//closeBT->setStandardButtons(QDialogButtonBox::Close);
	showPagePB = new QPushButton(Form);
	showPagePB->setObjectName(QString::fromUtf8("showPagePB"));
	showPagePB->setGeometry(QRect(360, 90, 80, 31));
	showPagePB->setFont(font2);
	pagesLV = new QListWidget(Form);
	pagesLV->setObjectName(QString::fromUtf8("pagesLV"));
	pagesLV->setGeometry(QRect(120, 90, 221, 161));
	pagesLV->setFont(font1);
	pagesLV->setSelectionMode(QAbstractItemView::SingleSelection);
	pagesLV->setSelectionBehavior(QAbstractItemView::SelectItems);

	tmspages << "TMS Main" << 
		"TMS TF Heluim Line" << "TMS TF InterCoil Bus Line" << "TMS TF Structure Upper 1" <<
		"TMS TF Structure Upper 2" << "TMS PF Heluim Line" << "TMS PF Bus Line" << "TMS PF Structure" <<
		"TMS CS Structure" << "TMS Gravity Support" << "TMS Thermal Shield" << "TMS Bus Support" << "TMS Leaf Spring";

	vmspages << "VMS Main" << "VMS CR Status"<<"VMS CR RTChart"<<"VMS CR Control" 
			 << "VMS VV Status" <<"VMS VV RTChart"<<"VMS VV Control" 
			 << "VMS CR Pump Detail" << "VMS Jacket Heater" <<"VMS RGA";

	clspages << "CLS Vacuum Status" <<"CLS Vacuum RTChart"<< "CLS TF HCS Status" << "CLS PF HCS Status" << "CLS TS Status" <<
				"CLS Heater Contorl" <<"CLS TF Valve Control"<<"CLS PF Valve Control";

	hdspages << "HDS Vacuum Status"<<"HDS VPS RTChart" << "HDS HDS TF" << "HDS HDS PF" << "HDS HCS TS" << "HDS HCS ShieldTemp" <<
		"HDS HCS PFCoil" <<"HDS HCS PFBus";

	echpages << "ECH Status" << "ECH Fault List" <<"ECH NIDAQcontrol"<<"ECH Waveform";
	fuelpages << "Fuel Main" << "Fuel Piezo Status"<<"Fuel DAQ"<<"GDC Main"<<"Fuel SMBI MGI"<<"Fuel SMBI DAQ" <<"Fuel MGI Status";
	pmspages << "PMS Divertor" << "PMS Limiter" << "PMS NBarmor" << "PMS IVCC" <<"PMS In Board" <<"PMS Out Board";
	icrhpages<< "ICRH Main" << "ICRH Status" << "ICRH DAQ";
	gcdspages<< "GCDS DAQ";
	mpspages<<"MPS Monitoring"<<"MPS Operation";
	icppages<<"ICP Monitoring"<<"HDS Vibrate";
	nb1calpages<<"NB1 Calorimetric";
	nb1vacpages<<"NB1 Vacuum";
	scspages<<"Machine Status";

	hdsvibpages << "TF Circuit" << "PF Circuit" << "TF Line" << "ICP";

	connectSignal();
	Form->show();
	retranslateUi(Form);


	QMetaObject::connectSlotsByName(Form);
} // setupUi
