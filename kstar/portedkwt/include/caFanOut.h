#ifndef CA_FANOUT_H
#define CA_FANOUT_H

#include <QtGui>
#include <QWidget>

class CAFanOut : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QStringList pvlist READ getPVs WRITE setPVs);

public:
	explicit CAFanOut(QWidget *parent = NULL);
	virtual ~CAFanOut();

	const QStringList getPVs() const;
	void setPVs(const QStringList pvlist);
	int caPut(const QString pvname, const double &value, int type);
	int caPutString(const QString pvname, const QString value);

public slots:
	virtual void fanOutValue (const double &value);
	virtual void fanOutValue (const int &value);
	virtual void fanOutValue (const QString value);

signals:
#if 0
	void fanOutValue(double value); 
	void fanOutValue(int value); 
	void fanOutValue(QString &strvalue); 
#endif

private:
	class PrivateData;
	PrivateData *p_data;
};
#endif

