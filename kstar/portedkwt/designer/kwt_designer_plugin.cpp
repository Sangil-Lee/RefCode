/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Kwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Kwt License, Version 1.0
 *****************************************************************************/
#include <qglobal.h>
#include <qaction.h>
#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QExtensionManager>
#include <QErrorMessage>

#include "kwt_designer_plugin.h"
#include "caDisplayer.h"
#include "caPushbutton.h"
#include "caBobutton.h"
#include "caCheckbox.h"
#include "caMbbobutton.h"
#include "caComboBox.h"
#include "caLineedit.h"
#include "caMultiplot.h"
#include "caMultiwaveplot.h"
#include "caLabel.h"
#include "caListBox.h"
#include "caBlinkLabel.h"
#include "caImageMbbi.h"
#include "caWclock.h"
#include "caGraphic.h"
#include "staticGraphic.h"
#include "caColorcheckbox.h"
#include "caUitime.h"
#include "scalepicker.h"
//#include "colorbar.h"
#include "caScreenShot.h"
#include "caQtSignal.h"
#include "caFanOut.h"
#include "caScheduler.h"
#include "caSlider.h"
#include "caProgressBar.h"
//#include "colorBarScaleWidget.h"
#include "caColorTilebutton.h"
#include "caWaveformput.h"
#include "caTableList.h"
#include "caPopup.h"
//#include "canvaspicker.h"
//#include "blinkLine.h"
//#include "blinkLabel.h"


using namespace KwtDesignerPlugin;

CustomWidgetInterface::CustomWidgetInterface(QObject *parent): 
    QObject(parent),
    d_isInitialized(false)
{
}

bool CustomWidgetInterface::isContainer() const
{
    return false;
}

bool CustomWidgetInterface::isInitialized() const
{
    return d_isInitialized;
}

QIcon CustomWidgetInterface::icon() const
{
    return d_icon;
}

QString CustomWidgetInterface::codeTemplate() const
{
    return d_codeTemplate;
}

QString CustomWidgetInterface::domXml() const
{
    return d_domXml;
}

QString CustomWidgetInterface::group() const
{
    return "KSTAR Widgets";
}

QString CustomWidgetInterface::includeFile() const
{
    return d_include;
}

QString CustomWidgetInterface::name() const
{
    return d_name;
}

QString CustomWidgetInterface::toolTip() const
{
    return d_toolTip;
}

QString CustomWidgetInterface::whatsThis() const
{
    return d_whatsThis;
}

void CustomWidgetInterface::initialize(
    QDesignerFormEditorInterface *formEditor)
{
    if ( d_isInitialized )
        return;

    QExtensionManager *manager = formEditor->extensionManager();
    if ( manager )
    {
        manager->registerExtensions(new TaskMenuFactory(manager),
            Q_TYPEID(QDesignerTaskMenuExtension));
    }

    d_isInitialized = true;
}
CADisplayerInterface::CADisplayerInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CADisplayer";
    d_include = "caDisplayer.h";
    d_domXml =
        "<widget class=\"CADisplayer\" name=\"CADisplayer\">\n"
        "</widget>\n";
}

QWidget *CADisplayerInterface::createWidget(QWidget *parent)
{
    return new CADisplayer(parent);
}


CAPushButtonInterface::CAPushButtonInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAPushButton";
    d_include = "caPushbutton.h";
    d_domXml =
        "<widget class=\"CAPushButton\" name=\"caPushButton\">\n"
        "</widget>\n";
}
QWidget *CAPushButtonInterface::createWidget(QWidget *parent)
{
    return new CAPushButton(parent);
}


CABoButtonInterface::CABoButtonInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CABoButton";
    d_include = "caBobutton.h";
    d_domXml =
        "<widget class=\"CABoButton\" name=\"caBoButton\">\n"
        "</widget>\n";
}
QWidget *CABoButtonInterface::createWidget(QWidget *parent)
{
    return new CABoButton(parent);
}

CACheckBoxInterface::CACheckBoxInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CACheckBox";
    d_include = "caCheckbox.h";
    d_domXml =
        "<widget class=\"CACheckBox\" name=\"caCheckBox\">\n"
        "</widget>\n";
}
QWidget *CACheckBoxInterface::createWidget(QWidget *parent)
{
    return new CACheckBox(parent);
}

CAMbboButtonInterface::CAMbboButtonInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAMbboButton";
    d_include = "caMbbobutton.h";
    d_domXml =
        "<widget class=\"CAMbboButton\" name=\"caMbboButton\">\n"
        "</widget>\n";
}
QWidget *CAMbboButtonInterface::createWidget(QWidget *parent)
{
    return new CAMbboButton(parent);
}

CAComboBoxInterface::CAComboBoxInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAComboBox";
    d_include = "caComboBox.h";
    d_domXml =
        "<widget class=\"CAComboBox\" name=\"caComboBox\">\n"
        "</widget>\n";
}
QWidget *CAComboBoxInterface::createWidget(QWidget *parent)
{
    return new CAComboBox(parent);
}


CALineEditInterface::CALineEditInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CALineEdit";
    d_include = "caLineedit.h";
    d_domXml =
        "<widget class=\"CALineEdit\" name=\"caLineEdit\">\n"
        "</widget>\n";
}
QWidget *CALineEditInterface::createWidget(QWidget *parent)
{
    return new CALineEdit(parent);
}


CALabelInterface::CALabelInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CALabel";
    d_include = "caLabel.h";
    d_domXml =
        "<widget class=\"CALabel\" name=\"caLabel\">\n"
        "</widget>\n";
}

QWidget *CALabelInterface::createWidget(QWidget *parent)
{
    return new CALabel(parent);
}

CABlinkLabelInterface::CABlinkLabelInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "CABlinkLabel";
	d_include = "caBlinkLabel.h";
	d_domXml =
		"<widget class=\"CABlinkLabel\" name=\"caBlinkLabel\">\n"
		"</widget>\n";
}
QWidget *CABlinkLabelInterface::createWidget(QWidget *parent)
{
	return new CABlinkLabel(parent);
}

#if 0
BlinkLabelInterface::BlinkLabelInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "BlinkLabel";
	d_include = "blinkLabel.h";
	d_domXml =
		"<widget class=\"BlinkLabel\" name=\"blinkLabel\">\n"
		"</widget>\n";
}
QWidget *BlinkLabelInterface::createWidget(QWidget *parent)
{
	return new BlinkLabel(parent);
}

BlinkLineInterface::BlinkLineInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "BlinkLine";
	d_include = "blinkLine.h";
	d_domXml =
		"<widget class=\"BlinkLine\" name=\"blinkLine\">\n"
		"</widget>\n";
}
QWidget *BlinkLineInterface::createWidget(QWidget *parent)
{
	return new BlinkLine(parent);
}
#endif

CAListBoxInterface::CAListBoxInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "CAListBox";
	d_include = "caListBox.h";
	d_domXml =
		"<widget class=\"CAListBox\" name=\"caListBox\">\n"
		"</widget>\n";
}
QWidget *CAListBoxInterface::createWidget(QWidget *parent)
{
	return new CAListBox(parent);
}


CAImageMbbiInterface::CAImageMbbiInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAImageMbbi";
    d_include = "caImageMbbi.h";
    d_domXml =
        "<widget class=\"CAImageMbbi\" name=\"caImageMbbi\">\n"
        "</widget>\n";
}
QWidget *CAImageMbbiInterface::createWidget(QWidget *parent)
{
    return new CAImageMbbi(parent);
}

CAWclockInterface::CAWclockInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAWclock";
    d_include = "caWclock.h";
    d_domXml =
        "<widget class=\"CAWclock\" name=\"caWclock\">\n"
        "</widget>\n";
}
QWidget *CAWclockInterface::createWidget(QWidget *parent)
{
    return new CAWclock(parent);
}

CAGraphicInterface::CAGraphicInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAGraphic";
    d_include = "caGraphic.h";
    d_domXml =
        "<widget class=\"CAGraphic\" name=\"caGraphic\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}
QWidget *CAGraphicInterface::createWidget(QWidget *parent)
{
    return new CAGraphic(parent);
}

StaticGraphicInterface::StaticGraphicInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "StaticGraphic";
    d_include = "staticGraphic.h";
    d_domXml =
        "<widget class=\"StaticGraphic\" name=\"staticGraphic\">\n"
#if 0
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>100</width>\n"
        "   <height>100</height>\n"
        "  </rect>\n"
        " </property>\n"
#endif
        "</widget>\n";
}
QWidget *StaticGraphicInterface::createWidget(QWidget *parent)
{
    return new StaticGraphic(parent);
}


CAMultiplotInterface::CAMultiplotInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAMultiplot";
    d_include = "caMultiplot.h";
    d_domXml = 
        "<widget class=\"CAMultiplot\" name=\"caMultiplot\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>500</width>\n"
        "   <height>300</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *CAMultiplotInterface::createWidget(QWidget *parent)
{
    return new CAMultiplot(parent);
}
CAMultiwaveplotInterface::CAMultiwaveplotInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "CAMultiwaveplot";
	d_include = "caMultiwaveplot.h";
	d_domXml =
		"<widget class=\"CAMultiwaveplot\" name=\"CAMultiwaveplot\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>500</width>\n"
		"   <height>300</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QWidget *CAMultiwaveplotInterface::createWidget(QWidget *parent)
{
	return new CAMultiwaveplot(parent);
}

#ifdef MDSPLUS
CAMDSplusplotInterface::CAMDSplusplotInterface(QObject *parent):
	CustomWidgetInterface(parent)
{
	d_name = "CAMDSplusplot";
	d_include = "caMDSplusplot.h";
	d_domXml =
		"<widget class=\"CAMDSplusplot\" name=\"CAMDSplusplot\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>500</width>\n"
		"   <height>300</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QWidget *CAMDSplusplotInterface::createWidget(QWidget *parent)
{
	return new CAMDSplusplot(parent);
}
#endif

CAColorCheckBoxInterface::CAColorCheckBoxInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAColorCheckBox";
    d_include = "caColorcheckbox.h";
    d_domXml = 
        "<widget class=\"CAColorCheckBox\" name=\"caColorCheckBox\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>90</width>\n"
        "   <height>20</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *CAColorCheckBoxInterface::createWidget(QWidget *parent)
{
    return new CAColorCheckBox(parent);
}

CAUITimeInterface::CAUITimeInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAUITime";
    d_include = "caUitime.h";
    d_domXml =
        "<widget class=\"CAUITime\" name=\"CAUITime\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>20</width>\n"
        "   <height>20</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}
QWidget *CAUITimeInterface::createWidget(QWidget *parent)
{
    return new CAUITime(parent);
}

CAScreenShotInterface::CAScreenShotInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAScreenShot";
    d_include = "caScreenShot.h";
    d_domXml =
        "<widget class=\"CAScreenShot\" name=\"CAScreenShot\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>20</width>\n"
        "   <height>20</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}
QWidget *CAScreenShotInterface::createWidget(QWidget *parent)
{
    return new CAScreenShot(parent);
}

CAQtSignalInterface::CAQtSignalInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAQtSignal";
    d_include = "caQtSignal.h";
    d_domXml =
        "<widget class=\"CAQtSignal\" name=\"CAQtSignal\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>0</x>\n"
        "   <y>0</y>\n"
        "   <width>20</width>\n"
        "   <height>20</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *CAQtSignalInterface::createWidget(QWidget *parent)
{
    return new CAQtSignal(parent);
}

CAFanOutInterface::CAFanOutInterface(QObject *parent): CustomWidgetInterface(parent)
{
	d_name = "CAFanOut";
	d_include = "caFanOut.h";
	d_domXml =
		"<widget class=\"CAFanOut\" name=\"CAFanOut\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>20</width>\n"
		"   <height>20</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QWidget *CAFanOutInterface::createWidget(QWidget *parent)
{
	return new CAFanOut(parent);
}

CASchedulerInterface::CASchedulerInterface(QObject *parent): CustomWidgetInterface(parent)
{
	d_name = "CAScheduler";
	d_include = "caScheduler.h";
	d_domXml =
		"<widget class=\"CAScheduler\" name=\"caScheduler\">\n"
		" <property name=\"geometry\">\n"
		"  <rect>\n"
		"   <x>0</x>\n"
		"   <y>0</y>\n"
		"   <width>20</width>\n"
		"   <height>20</height>\n"
		"  </rect>\n"
		" </property>\n"
		"</widget>\n";
}

QWidget *CASchedulerInterface::createWidget(QWidget *parent)
{
	return new CAScheduler(parent);
}

CASliderInterface::CASliderInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CASlider";
    d_include = "caSlider.h";
    d_domXml =
        "<widget class=\"CASlider\" name=\"CASlider\">\n"
        "</widget>\n";
}

QWidget *CASliderInterface::createWidget(QWidget *parent)
{
    return new CASlider(parent);
}

CAProgressBarInterface::CAProgressBarInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAProgressBar";
    d_include = "caProgressBar.h";
    d_domXml =
        "<widget class=\"CAProgressBar\" name=\"CAPogressBar\">\n"
        "</widget>\n";
}

QWidget *CAProgressBarInterface::createWidget(QWidget *parent)
{
    return new CAProgressBar(parent);
}

#if 0
ColorBarScaleWidgetInterface::ColorBarScaleWidgetInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "ColorBarScaleWidget";
    d_include = "colorBarScaleWidget.h";
    d_domXml =
        "<widget class=\"ColorBarScaleWidget\" name=\"ColorBarScaleWidget\">\n"
        "</widget>\n";
}

QWidget *ColorBarScaleWidgetInterface::createWidget(QWidget *parent)
{
    return new ColorBarScaleWidget(parent);
}

CAColorTileButtonInterface::CAColorTileButtonInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAColorTileButton";
    d_include = "caColorTilebutton.h";
    d_domXml =
        "<widget class=\"CAColorTileButton\" name=\"CAColorTileButton\">\n"
        "</widget>\n";
}

QWidget *CAColorTileButtonInterface::createWidget(QWidget *parent)
{
    return new CAColorTileButton(parent);
}

CAWaveformPutInterface::CAWaveformPutInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAWaveformPut";
    d_include = "caWaveformput.h";
    d_domXml =
        "<widget class=\"CAWaveformPut\" name=\"CAWaveformPut\">\n"
        "</widget>\n";
}

QWidget *CAWaveformPutInterface::createWidget(QWidget *parent)
{
    return new CAWaveformPut(parent);
}
#endif

CATableListInterface::CATableListInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CATableList";
    d_include = "caTableList.h";
    d_domXml =
        "<widget class=\"CATableList\" name=\"CATableList\">\n"
        "</widget>\n";
}

QWidget *CATableListInterface::createWidget(QWidget *parent)
{
    return new CATableList(parent);
}

CAPopUpInterface::CAPopUpInterface(QObject *parent):
    CustomWidgetInterface(parent)
{
    d_name = "CAPopUp";
    d_include = "caPopup.h";
    d_domXml =
        "<widget class=\"CAPopUp\" name=\"CAPopUp\">\n"
        "</widget>\n";
}

QWidget *CAPopUpInterface::createWidget(QWidget *parent)
{
    return new CAPopUp(parent);
}

CustomWidgetCollectionInterface::CustomWidgetCollectionInterface(   
        QObject *parent): 
    QObject(parent)
{
    d_plugins.append(new CADisplayerInterface(this));
    d_plugins.append(new CAPushButtonInterface(this));
    d_plugins.append(new CABoButtonInterface(this));
    d_plugins.append(new CACheckBoxInterface(this));
    d_plugins.append(new CAMbboButtonInterface(this));
    d_plugins.append(new CAComboBoxInterface(this));
    d_plugins.append(new CALineEditInterface(this));
    d_plugins.append(new CALabelInterface(this));
    d_plugins.append(new CABlinkLabelInterface(this));
    //d_plugins.append(new BlinkLabelInterface(this));
    //d_plugins.append(new BlinkLineInterface(this));
    d_plugins.append(new CAListBoxInterface(this));
    d_plugins.append(new CAImageMbbiInterface(this));
    d_plugins.append(new CAWclockInterface(this));
    d_plugins.append(new CAGraphicInterface(this));
    d_plugins.append(new StaticGraphicInterface(this));
    d_plugins.append(new CAMultiplotInterface(this));
    d_plugins.append(new CAMultiwaveplotInterface(this));
    d_plugins.append(new CAColorCheckBoxInterface(this));
    d_plugins.append(new CAUITimeInterface(this));
    d_plugins.append(new CAScreenShotInterface(this));
    d_plugins.append(new CAQtSignalInterface(this));
    d_plugins.append(new CAFanOutInterface(this));
    d_plugins.append(new CASchedulerInterface(this));
    d_plugins.append(new CASliderInterface(this));
    d_plugins.append(new CAProgressBarInterface(this));
    //d_plugins.append(new ColorBarScaleWidgetInterface(this));
    //d_plugins.append(new CAColorTileButtonInterface(this));
    //d_plugins.append(new CAWaveformPutInterface(this));
    d_plugins.append(new CATableListInterface(this));
    d_plugins.append(new CAPopUpInterface(this));
}

QList<QDesignerCustomWidgetInterface*> 
    CustomWidgetCollectionInterface::customWidgets(void) const
{
    return d_plugins;
}

TaskMenuFactory::TaskMenuFactory(QExtensionManager *parent): 
    QExtensionFactory(parent)
{
}

QObject *TaskMenuFactory::createExtension(
    QObject *object, const QString &iid, QObject *parent) const
{
    if (iid == Q_TYPEID(QDesignerTaskMenuExtension))
    {
        if (CADisplayer *disp = qobject_cast<CADisplayer*>(object))
            return new TaskMenuExtension(disp, parent);
    }

    return QExtensionFactory::createExtension(object, iid, parent);
}


TaskMenuExtension::TaskMenuExtension(QWidget *widget, QObject *parent):
    QObject(parent),    
    d_widget(widget)
{
    d_editAction = new QAction(tr("Edit Kwt Attributes ..."), this);
    connect(d_editAction, SIGNAL(triggered()), 
        this, SLOT(editProperties()));
}

QList<QAction *> TaskMenuExtension::taskActions() const
{
    QList<QAction *> list;
    list.append(d_editAction);
    return list;
}

QAction *TaskMenuExtension::preferredEditAction() const
{
    return d_editAction;
}

void TaskMenuExtension::editProperties()
{
    const QVariant v = d_widget->property("propertiesDocument");
    if ( v.type() != QVariant::String )
        return;

    QString properties = v.toString();

/*
    if ( qobject_cast<KwtPlot*>(d_widget) )
    {
        PlotDialog dialog(properties);
        connect(&dialog, SIGNAL(edited(const QString&)), 
            SLOT(applyProperties(const QString &)));
        (void)dialog.exec();
        return;
    }
*/

    static QErrorMessage *errorMessage = NULL;
    if ( errorMessage == NULL )
        errorMessage = new QErrorMessage();
    errorMessage->showMessage("Not implemented yet.");
}

void TaskMenuExtension::applyProperties(const QString &properties)
{
    QDesignerFormWindowInterface *formWindow
        = QDesignerFormWindowInterface::findFormWindow(d_widget);
    if ( formWindow && formWindow->cursor() )
        formWindow->cursor()->setProperty("propertiesDocument", properties);
}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(KwtDesignerPlugin, CustomWidgetCollectionInterface)
#endif
