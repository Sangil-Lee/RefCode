#include "MenuButton.h"

MenuButton::MenuButton(QWidget *parent):QPushButton(parent)
{
}

MenuButton::MenuButton ( const QString & text, QWidget * parent):QPushButton(text, parent)
{
}

MenuButton::MenuButton (const QString butname, const QIcon & icon, const QString & text, QWidget * parent, const QStringList hlist):
			QPushButton(icon, text, parent)
{
	mbutname = butname;
	strlist = hlist;
	MenuWindow *pParent = (MenuWindow*)parentWidget();
	connect(this, SIGNAL(doubleClickMessage(const QString, const QStringList)), pParent, SLOT(execution(const QString, const QStringList)) );
}

MenuButton::~MenuButton()
{
}

void
MenuButton::mouseDoubleClickEvent(QMouseEvent *event)
{
	//qDebug("MenuButton Double Clicked");
	emit doubleClickMessage(mbutname, strlist);
}

