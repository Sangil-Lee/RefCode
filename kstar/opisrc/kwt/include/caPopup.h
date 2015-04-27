#ifndef CA_POPUP_H
#define CA_POPUP_H

#include <QtGui>

class CAPopUp : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString filename READ getFilename WRITE setFilename);


public:
	explicit CAPopUp(QWidget *parent = NULL);
	virtual ~CAPopUp();

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

	const QString &getFilename() const;
	void setFilename(const QString &filename);
	void setShow(const int show) {bshow = show;};
	int getShow() {return bshow;};
	int  Show();

public slots:
	virtual void changeValue (const short &connstatus, const double &value);
	virtual void buttonCancel();

signals:

private:
	class PrivateData;
	PrivateData *p_data;
	int bshow;
	int getValue();
	void showSingleton ();
};
#endif

