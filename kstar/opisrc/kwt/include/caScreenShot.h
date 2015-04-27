#ifndef CA_SCREENSHOT_H
#define CA_SCREENSHOT_H

#include <QtGui>

class CAScreenShot : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString prefix READ getPrefix WRITE setPrefix);
	Q_PROPERTY(QString pvname READ getPvname WRITE setPvname);

public:
	explicit CAScreenShot(QWidget *parent = NULL);
	virtual ~CAScreenShot();

	const QString &getPrefix() const;
	void  setPrefix(const QString &prefix);

	const QString &getPvname() const;
	void  setPvname(const QString &prefix);

public slots:
	virtual void fileNameDateTime();
	virtual void fileName(const QString &filename);
	virtual void screenShot (const int shotno);
	virtual void fileNamePostfix(const int number);
	virtual void fileNamePostfix(const double number);
	virtual void changeValue (const short &connstatus, const double &value);

private:
	class PrivateData;
	PrivateData *p_data;
};
#endif

