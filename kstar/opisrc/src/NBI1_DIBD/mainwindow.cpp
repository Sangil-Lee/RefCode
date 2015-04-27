#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>

char g_msg_head[32] = {0x10, 0x02, 0x00, /* head */
                           0x00, 0x2A,       /* Data Length : cmd + msg */
                           0x54,       /* CMD */
                           0x00, 0x01, 0xF0, 0x91, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, /* parameter 1 */
                           0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02}; /* font color */
const char g_msg_tail[2] = {0x10, 0x03};

const char g_led1_name[6] = {'B', '/', 'D', ' ', ':', ' '};
const char g_led2_name[6] = {'B', 'O', 'X', ' ', ':', ' '};
const char g_led3_name[6] = {'I', 'S', '1', ' ', ':', ' '};
const char g_led4_name[6] = {'I', 'S', '2', ' ', ':', ' '};
const char g_led51_name[4] = {'C', 'R', 'B', ' '};
const char g_led52_name[4] = {'C', 'R', 'P', ' '};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pAttachView = NULL;
    pAttachView = new AttachChannelAccess(this);

    ui->stLedSt1->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt2->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt3->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt4->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt5->setProperty("fillColor", QVariant(QColor(255,0,0)));

    led5_sel = 0;
    timer_inc = 0;
}

MainWindow::~MainWindow()
{
    if(timer_id >= 0) {
        killTimer(timer_id);
    }

    ::close(led1_sock);
    ::close(led2_sock);
    ::close(led3_sock);
    ::close(led4_sock);
    ::close(led5_sock);

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
    kill(getpid(), SIGTERM);
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

int MainWindow::set_up_tcp(char *ip_address)
{
    int sfd;
    struct sockaddr_in server;
    int connect_stat;

    sfd = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );

    server.sin_family = AF_INET;
    server.sin_port = htons( DIBD_TCP_PORT );
    server.sin_addr.s_addr = inet_addr(ip_address);

    if( sfd >= 0 )
    {
        connect_stat = ::connect( sfd, (struct sockaddr *)&server,
                        sizeof(struct sockaddr_in) );

        if( connect_stat < 0 )
        {
            ::close( sfd );
            sfd = -1;
        }
    }

    return( sfd );
}

// START Button
void MainWindow::on_pushButton_11_clicked()
{
    led1_sock = set_up_tcp("192.168.0.201");    
    led2_sock = set_up_tcp("192.168.0.202");
    led3_sock = set_up_tcp("192.168.0.203");
    led4_sock = set_up_tcp("192.168.0.204");
    led5_sock = set_up_tcp("192.168.0.205");

    if(led1_sock != -1) {
        ui->stLedSt1->setProperty("fillColor", QVariant(QColor(0,255,0)));
    }
    else {
        ui->stLedSt1->setProperty("fillColor", QVariant(QColor(255,0,0)));
    }

    if(led2_sock != -1) {
        ui->stLedSt2->setProperty("fillColor", QVariant(QColor(0,255,0)));
    }
    else {
        ui->stLedSt2->setProperty("fillColor", QVariant(QColor(255,0,0)));
    }

    if(led3_sock != -1) {
        ui->stLedSt3->setProperty("fillColor", QVariant(QColor(0,255,0)));
    }
    else {
        ui->stLedSt3->setProperty("fillColor", QVariant(QColor(255,0,0)));
    }

    if(led4_sock != -1) {
        ui->stLedSt4->setProperty("fillColor", QVariant(QColor(0,255,0)));
    }
    else {
        ui->stLedSt4->setProperty("fillColor", QVariant(QColor(255,0,0)));
    }

    if(led5_sock != -1) {
        ui->stLedSt5->setProperty("fillColor", QVariant(QColor(0,255,0)));
    }
    else {
        ui->stLedSt5->setProperty("fillColor", QVariant(QColor(255,0,0)));
    }

    timer_id = startTimer(1000);
}

void MainWindow::on_pushButton_12_clicked()
{
    if(timer_id >= 0) {
        killTimer(timer_id);
    }

    ::close(led1_sock);
    ::close(led2_sock);
    ::close(led3_sock);
    ::close(led4_sock);
    ::close(led5_sock);

    ui->stLedSt1->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt2->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt3->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt4->setProperty("fillColor", QVariant(QColor(255,0,0)));
    ui->stLedSt5->setProperty("fillColor", QVariant(QColor(255,0,0)));
}

// LED1 PW ON
void MainWindow::on_pushButton_clicked()
{
    unsigned char pw_on[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x01, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led1_sock > 0)
    {
        ::send(led1_sock, pw_on, 10, 0);

        ::recv(led1_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    unsigned char pw_off[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x00, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led1_sock > 0)
    {
        ::send(led1_sock, pw_off, 10, 0);

        ::recv(led1_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    unsigned char pw_on[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x01, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led2_sock > 0)
    {
        ::send(led2_sock, pw_on, 10, 0);

        ::recv(led2_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    unsigned char pw_off[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x00, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led2_sock > 0)
    {
        ::send(led2_sock, pw_off, 10, 0);

        ::recv(led2_sock, buff, 13, 0);
    }
}

// LED 3 PW
void MainWindow::on_pushButton_5_clicked()
{
    unsigned char pw_on[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x01, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led3_sock > 0)
    {
        ::send(led3_sock, pw_on, 10, 0);

        ::recv(led3_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    unsigned char pw_off[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x00, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led3_sock > 0)
    {
        ::send(led3_sock, pw_off, 10, 0);

        ::recv(led3_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    unsigned char pw_on[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x01, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led4_sock > 0)
    {
        ::send(led4_sock, pw_on, 10, 0);

        ::recv(led4_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_8_clicked()
{
    unsigned char pw_off[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x00, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led4_sock > 0)
    {
        ::send(led4_sock, pw_off, 10, 0);

        ::recv(led4_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    unsigned char pw_on[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x01, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led5_sock > 0)
    {
        ::send(led5_sock, pw_on, 10, 0);

        ::recv(led5_sock, buff, 13, 0);
    }
}

void MainWindow::on_pushButton_10_clicked()
{
    unsigned char pw_off[] = {0x10, 0x02, 0x00, 0x00, 0x03, 0x41, 0x00, 0x00, 0x10, 0x03};
    unsigned char buff[13];

    if(led5_sock > 0)
    {
        ::send(led5_sock, pw_off, 10, 0);

        ::recv(led5_sock, buff, 13, 0);
    }
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    //char buff[100];
    double val;
    int len, i;
    char temp[2];

    temp[0] = 0x20;

    if(led1_sock > 0)
    {
        QString str(ui->caLabel->text());

        val = str.toDouble();
        str = QString("%1").arg(val, 1, 'e', 2);

        //QString str(ui->caDisplay->text());
        QByteArray baStr;

        baStr = str.toLocal8Bit();

        //len = baStr.length() + 33;
        //g_msg_head[4] = (unsigned char)len&0xFF;
        ::send(led1_sock, g_msg_head, sizeof(g_msg_head), 0);
        ::send(led1_sock, g_led1_name, sizeof(g_led1_name), 0);
        ::send(led1_sock, baStr.data(), baStr.length(), 0);
        len = 15 - (baStr.length() + 6);
        for(i=0; i<len; i++) {
            ::send(led1_sock, temp, 1, 0);
        }
        ::send(led1_sock, g_msg_tail, sizeof(g_msg_tail), 0);
    }
    if(led2_sock > 0)
    {
        QString str(ui->caLabel_2->text());
        val = str.toDouble();
        str = QString("%1").arg(val, 1, 'e', 2);

        QByteArray baStr;

        baStr = str.toLocal8Bit();

        //len = baStr.length() + 33;
        //g_msg_head[4] = (unsigned char)len&0xFF;
        ::send(led2_sock, g_msg_head, sizeof(g_msg_head), 0);
        ::send(led2_sock, g_led2_name, sizeof(g_led2_name), 0);
        ::send(led2_sock, baStr.data(), baStr.length(), 0);
        len = 15 - (baStr.length() + 6);
        for(i=0; i<len; i++) {
            ::send(led2_sock, temp, 1, 0);
        }
        ::send(led2_sock, g_msg_tail, sizeof(g_msg_tail), 0);
    }
    if(led3_sock > 0)
    {
        QString str(ui->caLabel_3->text());
        val = str.toDouble();
        str = QString("%1").arg(val, 1, 'e', 2);

        QByteArray baStr;

        baStr = str.toLocal8Bit();

        //len = baStr.length() + 33;
        //g_msg_head[4] = (unsigned char)len&0xFF;
        ::send(led3_sock, g_msg_head, sizeof(g_msg_head), 0);
        ::send(led3_sock, g_led3_name, sizeof(g_led3_name), 0);
        ::send(led3_sock, baStr.data(), baStr.length(), 0);
        len = 15 - (baStr.length() + 6);
        for(i=0; i<len; i++) {
            ::send(led3_sock, temp, 1, 0);
        }
        ::send(led3_sock, g_msg_tail, sizeof(g_msg_tail), 0);
    }
    if(led4_sock > 0)
    {
        QString str(ui->caLabel_6->text());
        val = str.toDouble();
        str = QString("%1").arg(val, 1, 'e', 2);

        QByteArray baStr;

        baStr = str.toLocal8Bit();

        //len = baStr.length() + 33;
        //g_msg_head[4] = (unsigned char)len&0xFF;
        ::send(led4_sock, g_msg_head, sizeof(g_msg_head), 0);
        ::send(led4_sock, g_led4_name, sizeof(g_led4_name), 0);
        ::send(led4_sock, baStr.data(), baStr.length(), 0);
        len = 15 - (baStr.length() + 6);
        for(i=0; i<len; i++) {
            ::send(led4_sock, temp, 1, 0);
        }
        ::send(led4_sock, g_msg_tail, sizeof(g_msg_tail), 0);
    }
    if(led5_sock > 0)
    {
        QString str1, str2;
        QByteArray baStr1, baStr2;

        str1 = ui->caLabel_5->text();
        str2 = ui->caLabel_4->text();

        baStr1 = str1.toLocal8Bit();
        baStr2 = str2.toLocal8Bit();

        //len = baStr.length() + 33;
        //g_msg_head[4] = (unsigned char)len&0xFF;
        ::send(led5_sock, g_msg_head, sizeof(g_msg_head), 0);

//        if(led5_sel == 1)
//            ::send(led5_sock, g_led52_name, sizeof(g_led52_name), 0);
//        else
//            ::send(led5_sock, g_led51_name, sizeof(g_led51_name), 0);

        ::send(led5_sock, g_led51_name, sizeof(g_led51_name), 0);
        ::send(led5_sock, baStr1.data(), baStr1.length(), 0);
        ::send(led5_sock, temp, 1, 0);
        ::send(led5_sock, g_led52_name, sizeof(g_led52_name), 0);
        ::send(led5_sock, baStr2.data(), baStr2.length(), 0);

//        ::send(led5_sock, " K", 2, 0);
        len = 15 - (baStr1.length() + baStr2.length() + 9);
        for(i=0; i<len; i++) {
            ::send(led5_sock, temp, 1, 0);
        }
        ::send(led5_sock, g_msg_tail, sizeof(g_msg_tail), 0);

        if(led5_sel == 1)
            led5_sel = 0;
        else
            led5_sel = 1;
    }
}
