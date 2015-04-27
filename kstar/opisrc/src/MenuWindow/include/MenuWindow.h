#ifndef MENU_WINDOW_H
#define MENU_WINDOW_H

#include <vector>
#include <QtGui>
#include <QSystemTrayIcon>
#include <QWidget>
#include <QHostInfo>
#include "MenuButton.h"
#define MENUWINDOW_VERSION "1.0"

using std::vector;
class MenuButton;

class MenuWindow : public QWidget
{
    Q_OBJECT
public:
    MenuWindow();
    void setVisible(bool visible);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void messageClicked();
    void menuExit();
	void execution(const QString message, const QStringList hostlist);

private:
	void init();
	int  initSignalHandler ();
    void createIconGroupBox();
    void createActions();
    void createTrayIcon();
	void processManager(const QString program);

    QGroupBox *iconGroupBox1;
    QGridLayout *iconLayout1;
    QGroupBox *iconGroupBox2;
    QGridLayout *iconLayout2;
    QGroupBox *iconGroupBox3;
    QGridLayout *iconLayout3;
    QGroupBox *iconGroupBox4;
    QGridLayout *iconLayout4;
    MenuButton *tmsButton;
    MenuButton *pcsButton;
    MenuButton *vmsButton;
    MenuButton *scsButton;
    MenuButton *fuelButton;
    MenuButton *ddsButton;
    MenuButton *qdsButton;
    MenuButton *icrhButton;

    //QAction *minimizeAction;
    //QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
	QStringList iconlist;
	QStringList applist;
	QProcess *myProcess;
	vector <QProcess *> vec_process;
	QHostInfo *hostinfo;
	QString hostname;
	QStringList pcslist;
	QStringList tsslist;
	QStringList ddslist;
	QStringList ccslist;
	QStringList mpslist;
	QStringList qdslist;
	QStringList echlist;
};

#endif
