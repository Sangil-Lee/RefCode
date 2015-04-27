#ifndef MENU_BUTTON_H
#define MENU_BUTTON_H

#include <QtGui>
#include <QSystemTrayIcon>
#include <QWidget>
#include "MenuWindow.h"

class MenuWindow;
class MenuButton:public QPushButton
{
    Q_OBJECT
public:
	MenuButton ( QWidget * parent = 0 );

	MenuButton ( const QString & text, QWidget * parent = 0 );

	MenuButton (const QString butname, const QIcon & icon, const QString & text, QWidget * parent = 0, 
			const QStringList hlist = NULL);
	~MenuButton ();

	bool getExecutable (const QString exHostName);


signals:
	void doubleClickMessage(const QString message, const QStringList hostlist);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
private:
	QString mbutname;
	QStringList strlist;
};

#endif
