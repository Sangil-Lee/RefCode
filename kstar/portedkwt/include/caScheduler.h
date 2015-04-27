#ifndef CA_SCHEDULER_H
#define CA_SCHEDULER_H

#include <QtGui>
#include <QApplication>
#include <QDialogButtonBox>
#include <QListWidget> 
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QRadioButton>
#include <QTimeEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QDialog>

class QDialogButtonBox;
class QListWidget; 
class QDateTimeEdit;
class QLineEdit;
class QRadioButton;
class QTimeEdit;
class QComboBox;
class QSpinBox;
class QPushButton;
class QLabel;
class QDialog;
class CAScheduler;
//class Ui_Dialog:public QWidget
class Ui_Dialog:public QDialog
{
	Q_OBJECT
protected slots:
	void Schedule();
	void DeleteSchedule();
	void StartSchedule(bool);
	void EditSchedule();
	void Accept();
	void Reject();
	void Display(QListWidgetItem*);
public:
	QDialogButtonBox *buttonBox;
	QListWidget *LW_Schedule;
	QDateTimeEdit *DTE_Datetime;
	QLineEdit *LE_PVname;
	QLineEdit *LE_REValue;
	QRadioButton *RB_Datetime;
	QRadioButton *RB_Day;
	QRadioButton *RB_Secs;
	QTimeEdit *TE_Time;
	QComboBox *CB_Daily;
	QSpinBox *SB_Secs;
	QPushButton *PB_Add;
	QPushButton *PB_Delete;
	QPushButton *PB_Start;
	QPushButton *PB_Edit;
	QLabel *label;
	QLabel *label_2;
	QDialog *Dialog;
	CAScheduler *pScheduler;

	//Ui_Dialog(QWidget *pWidget = NULL):QWidget(pWidget)
	Ui_Dialog(QWidget *pWidget = NULL):QDialog(pWidget)
	{
		pScheduler = (CAScheduler*)pWidget;
		setupUi();
	};

	~Ui_Dialog()
	{
		qDebug("Deleted Dialog**************");
		delete Dialog;
	};

	void setupUi()
	{
		Dialog = new QDialog();
		Dialog->setObjectName(QString::fromUtf8("Dialog"));
		//Dialog->setWindowModality(Qt::ApplicationModal);
		//Dialog->setModal(true);
		Dialog->resize(476, 343);
		buttonBox = new QDialogButtonBox(Dialog);
		buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
		buttonBox->setGeometry(QRect(110, 310, 341, 32));
		buttonBox->setOrientation(Qt::Horizontal);
		buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);
		LW_Schedule = new QListWidget(Dialog);
		LW_Schedule->setObjectName(QString::fromUtf8("LW_Schedule"));
		LW_Schedule->setGeometry(QRect(0, 120, 461, 181));
		DTE_Datetime = new QDateTimeEdit(Dialog);
		DTE_Datetime->setObjectName(QString::fromUtf8("DTE_Datetime"));
		DTE_Datetime->setGeometry(QRect(310, 10, 151, 27));
		LE_PVname = new QLineEdit(Dialog);
		LE_PVname->setObjectName(QString::fromUtf8("LE_PVname"));
		LE_PVname->setGeometry(QRect(80, 10, 113, 27));
		LE_REValue = new QLineEdit(Dialog);
		LE_REValue->setObjectName(QString::fromUtf8("LE_REValue"));
		LE_REValue->setGeometry(QRect(80, 50, 113, 27));
		RB_Datetime = new QRadioButton(Dialog);
		RB_Datetime->setObjectName(QString::fromUtf8("RB_Datetime"));
		RB_Datetime->setGeometry(QRect(210, 10, 90, 23));
		RB_Datetime->setChecked(true);
		RB_Day = new QRadioButton(Dialog);
		RB_Day->setObjectName(QString::fromUtf8("RB_Day"));
		RB_Day->setGeometry(QRect(210, 40, 90, 23));
		TE_Time = new QTimeEdit(Dialog);
		TE_Time->setObjectName(QString::fromUtf8("TE_Time"));
		TE_Time->setGeometry(QRect(390, 40, 71, 27));
		CB_Daily = new QComboBox(Dialog);
		CB_Daily->setObjectName(QString::fromUtf8("CB_Daily"));
		CB_Daily->setGeometry(QRect(310, 40, 71, 29));
		RB_Secs = new QRadioButton(Dialog);
		RB_Secs->setObjectName(QString::fromUtf8("RB_Secs"));
		RB_Secs->setGeometry(QRect(210, 70, 90, 23));
		SB_Secs = new QSpinBox(Dialog);
		SB_Secs->setObjectName(QString::fromUtf8("SB_Secs"));
		SB_Secs->setGeometry(QRect(310, 70, 61, 27));
		SB_Secs->setMinimum(1);
		SB_Secs->setMaximum(86400);
		PB_Add = new QPushButton(Dialog);
		PB_Add->setObjectName(QString::fromUtf8("PB_Add"));
		PB_Add->setGeometry(QRect(20, 90, 80, 27));
		PB_Delete = new QPushButton(Dialog);
		PB_Delete->setObjectName(QString::fromUtf8("PB_Delete"));
		PB_Delete->setGeometry(QRect(110, 90, 80, 27));
		PB_Start = new QPushButton(Dialog);
		PB_Start->setObjectName(QString::fromUtf8("PB_Start"));
		PB_Start->setGeometry(QRect(200, 90, 80, 27));
		PB_Start->setCheckable(true);
		PB_Edit = new QPushButton(Dialog);
		PB_Edit->setObjectName(QString::fromUtf8("PB_Edit"));
		PB_Edit->setGeometry(QRect(290, 90, 80, 27));
		label = new QLabel(Dialog);
		label->setObjectName(QString::fromUtf8("label"));
		label->setGeometry(QRect(5, 14, 54, 18));
		label_2 = new QLabel(Dialog);
		label_2->setObjectName(QString::fromUtf8("label_2"));
		label_2->setGeometry(QRect(4, 49, 54, 36));
		label_2->setAlignment(Qt::AlignCenter);
		retranslateUi(Dialog);
		Connect();
		//QMetaObject::connectSlotsByName(Dialog);
		ReadSchedule();
		Dialog->show();
	} // setupUi

	void Connect()
	{
		//QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
		//QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));
		QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(Accept()));
		QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(Reject()));
		QObject::connect(PB_Add, SIGNAL(clicked()), this, SLOT(Schedule()));
		QObject::connect(PB_Delete, SIGNAL(clicked()), this, SLOT(DeleteSchedule()));
		QObject::connect(PB_Start, SIGNAL(clicked(bool)), this, SLOT(StartSchedule(bool)));
		QObject::connect(PB_Edit, SIGNAL(clicked()), this, SLOT(EditSchedule()));
		QObject::connect(LW_Schedule, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(Display(QListWidgetItem*)));
	};

	void retranslateUi(QDialog *Dialog)
	{
		Dialog->setWindowTitle("Dialog");
		DTE_Datetime->setDisplayFormat("yyyy/MM/dd hh-mm-ss");
		RB_Datetime->setText("Datetime");
		RB_Day->setText("Day");
		TE_Time->setDisplayFormat("HH:mm:ss");
		CB_Daily->clear();
		CB_Daily->insertItems(0, QStringList()
				<< "Everyday" << "Monday" << "Tuesday" << "Wednesday" << "Thursday"
				<< "Friday" << "Saturday" << "Sunday");
		RB_Secs->setText("Every secs");
		PB_Add->setText("Add");
		PB_Delete->setText("Delete");
		PB_Start->setText("Start");
		PB_Edit->setText("Edit");
		label->setText("PV Name");
		label_2->setText("Reserved \n Value");
		Q_UNUSED(Dialog);
	} // retranslateUi

protected:
	void ReadSchedule();
	QString schedlist();

};

namespace Ui {
	class Dialog : public Ui_Dialog 
	{
	public:
		Dialog(QWidget *pWidget = NULL):Ui_Dialog(pWidget){};
	};
}; // namespace Ui

//class CAScheduler : public QWidget
class CAScheduler : public QLabel
{
	Q_OBJECT
	Q_PROPERTY(QStringList schedule READ getSchedules WRITE setSchedules);
	Q_PROPERTY(QStringList pvlist READ getPVs WRITE setPVs);
	Q_PROPERTY(QStringList values READ getValues WRITE setValues);

public:
	explicit CAScheduler(QWidget *parent = NULL);
	virtual ~CAScheduler();

	const QStringList getSchedules() const;
	void setSchedules(const QStringList schedulelist);

	const QStringList getPVs() const;
	void setPVs(const QStringList pvlist);

	const QStringList getValues() const;
	void setValues(const QStringList values);

	int caPut(const QString pvname, const double &value, int type);
	int caPutString(const QString pvname, const QString value);

	void ShowDialog();

	void Start();
	void Stop();
	bool IsTimerRun();

public slots:

signals:

protected:
	virtual void timerEvent (QTimerEvent *event);
	//virtual bool eventFilter(QObject *, QEvent *);

private:
	class PrivateData;
	PrivateData *p_data;
	Ui::Dialog *pdlg;
};
#endif

