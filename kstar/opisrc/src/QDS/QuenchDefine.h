#ifndef QUENCHDEFINE_H
#define QUENCHDEFINE_H

#include <QString>
#include <QRect>
#include <QLabel>
#include <QGroupBox>

#include "caLabel.h"

const QString BAR_STYLE_NORMAL = "background-color: rgb(0, 255, 0);";
const QString BAR_STYLE_HI = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 255, 0, 255), stop:1 rgba(255, 255, 0, 255));";
const QString BAR_STYLE_HI_INV = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 255, 0, 255), stop:1 rgba(0, 255, 0, 255));";
const QString BAR_STYLE_HIHI = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 255, 0, 255), stop:0.484444 rgba(255, 255, 0, 255), stop:1 rgba(255, 170, 0, 255));";
const QString BAR_STYLE_HIHI_INV = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 170, 0, 255), stop:0.484444 rgba(255, 255, 0, 255), stop:1 rgba(0, 255, 0, 255));";
const QString BAR_STYLE_HIHIHI = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(0, 255, 0, 255), stop:0.346667 rgba(255, 255, 0, 255), stop:0.657778 rgba(255, 170, 0, 255), stop:1 rgba(255, 0, 0, 255));";
const QString BAR_STYLE_HIHIHI_INV = "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(255, 0, 0, 255), stop:0.346667 rgba(255, 170, 0, 255), stop:0.657778 rgba(255, 255, 0, 255), stop:1 rgba(0, 255, 0, 255));";
const int BAR_HEIGHT = 15;
const int BAR_POS_Y = 35;

static void setOpiBar(CALabel *qVth, CALabel *qValue, QGroupBox *qBox, QLabel *qBar){
    float fVth = qVth->text().toFloat();
    float fValue = qValue->text().toFloat();
    QRect boxRect = qBox->geometry();

    int nCx = boxRect.width()/2;
    QRect barRect;

    if(fVth == 0)
        return;

    float fDiv = fValue / fVth;
    barRect.setY(BAR_POS_Y);
    barRect.setHeight(BAR_HEIGHT);
    int nWidth;

    if(fDiv > 1){
        nWidth = boxRect.width()/2-5;
        barRect.setX(nCx);
        qBar->setStyleSheet(BAR_STYLE_HIHIHI);
    }
    else if(fDiv > 0.75){
        nWidth = fDiv * (boxRect.width()/2-5);
        barRect.setX(nCx);
        qBar->setStyleSheet(BAR_STYLE_HIHIHI);
    }
    else if(fDiv > 0.5){
        nWidth = fDiv * (boxRect.width()/2-5);
        barRect.setX(nCx);
        qBar->setStyleSheet(BAR_STYLE_HIHI);
    }
    else if(fDiv > 0.25){
        nWidth = fDiv * (boxRect.width()/2-5);
        barRect.setX(nCx);
        qBar->setStyleSheet(BAR_STYLE_HI);
    }
    else if(fDiv > 0){
        nWidth = fDiv * (boxRect.width()/2-5);
        barRect.setX(nCx);
        qBar->setStyleSheet(BAR_STYLE_NORMAL);
    }
    else if(fDiv > -0.25){
        nWidth = fabs(fDiv) * (boxRect.width()/2-5);
        barRect.setX(nCx - nWidth + 5);
        qBar->setStyleSheet(BAR_STYLE_NORMAL);
    }
    else if(fDiv > -0.5){
        nWidth = fabs(fDiv) * (boxRect.width()/2-5);
        barRect.setX(nCx - nWidth + 5);
        qBar->setStyleSheet(BAR_STYLE_HI_INV);
    }
    else if(fDiv > -0.75){
        nWidth = fabs(fDiv) * (boxRect.width()/2-5);
        barRect.setX(nCx - nWidth + 5);
        qBar->setStyleSheet(BAR_STYLE_HIHI_INV);
    }
    else if(fDiv > -1){
        nWidth = fabs(fDiv) * (boxRect.width()/2-5);
        barRect.setX(nCx - nWidth + 5);
        qBar->setStyleSheet(BAR_STYLE_HIHIHI_INV);
    }
    else{
        nWidth = boxRect.width()/2-5;
        barRect.setX(nCx - nWidth + 5);
        qBar->setStyleSheet(BAR_STYLE_HIHIHI_INV);
    }
    barRect.setWidth(nWidth);
    qBar->setGeometry(barRect);
}

static void setOpiProgressBar(CALabel *qVth, CALabel *qValue, CALabel *qDT, QProgressBar *qProgressBar, QTimer *pTimer, int &nFlowCounter)
{
    /*
    float fDt = qDT->text().toFloat();

    int nDt = fDt;

    qProgressBar->setRange(0,nDt);

    float fValue = qValue->text().toFloat();
    float fVth = qVth->text().toFloat();

    if(fValue >= fVth || fValue <= -fVth)
    {
        if(!pTimer->isActive())
        {
            nFlowCounter = 0;
            pTimer->start(100);
        }
    }
    else
    {
        if(pTimer->isActive())
        {
            pTimer->stop();
            qProgressBar->setValue(0);
        }
    }
    */

    float fDt = qDT->text().toFloat();

    int nDt = fDt;

    qProgressBar->setRange(0,nDt);

    float fValue = qValue->text().toFloat();
    float fVth = qVth->text().toFloat();

    if(fValue >= fVth || fValue <= -fVth)
    {
        if(!pTimer->isActive())
        {
            nFlowCounter =0;
            pTimer->start(100);
        }
    }
    else
    {
        if(pTimer->isActive())
        {
            pTimer->stop();
            qProgressBar->setValue(0);
        }
    }
}

#endif // QUENCHDEFINE_H
