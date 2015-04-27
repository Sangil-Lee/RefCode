#ifndef CA_QTSIGNAL_H
#define CA_QTSIGNAL_H

#include <QtGui>

class CAQtSignal : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);
	Q_PROPERTY(QString condition READ getCondition WRITE setCondition);

public:
	explicit CAQtSignal(QWidget *parent = NULL);
	virtual ~CAQtSignal();

	const QString &getPrefix() const;
	void  setPrefix(const QString &prefix);

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

	const QString &getCondition() const;
	void  setCondition(const QString &condition);

public slots:
	virtual void changeValue (const short &connstatus, const double &value);

signals:
	void caQtSignal(double value); 
	void setEnable(bool enable); 
	void changeLabel(QString label);

private:
	class PrivateData;
	PrivateData *p_data;
};
#endif

