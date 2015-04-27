/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Kwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Kwt License, Version 1.0
 *****************************************************************************/

#if defined(_MSC_VER) /* MSVC Compiler */
#pragma warning ( disable : 4786 )
#endif

#include <qglobal.h>
#include <qaction.h>
#include <QtPlugin>
#include <QDesignerFormEditorInterface>
#include <QDesignerFormWindowInterface>
#include <QDesignerFormWindowCursorInterface>
#include <QExtensionManager>
#if 1
#include <QErrorMessage>
#endif

#include "klayout_designer_plugin.h"
#include "LScaleLayout.h"

using namespace KlayoutDesignerPlugin;

CustomLayoutInterface::CustomLayoutInterface(QObject *parent): 
    QObject(parent),
    d_isInitialized(false)
{
}

bool CustomLayoutInterface::isContainer() const
{
    return false;
}

bool CustomLayoutInterface::isInitialized() const
{
    return d_isInitialized;
}

QIcon CustomLayoutInterface::icon() const
{
    return d_icon;
}

QString CustomLayoutInterface::codeTemplate() const
{
    return d_codeTemplate;
}

QString CustomLayoutInterface::domXml() const
{
    return d_domXml;
}

QString CustomLayoutInterface::group() const
{
    return "KSTAR Layouts";
}

QString CustomLayoutInterface::includeFile() const
{
    return d_include;
}

QString CustomLayoutInterface::name() const
{
    return d_name;
}

QString CustomLayoutInterface::toolTip() const
{
    return d_toolTip;
}

QString CustomLayoutInterface::whatsThis() const
{
    return d_whatsThis;
}

void CustomLayoutInterface::initialize(
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

LScaleLayoutInterface::LScaleLayoutInterface(QObject *parent):
    CustomLayoutInterface(parent)
{
    d_name = "LScaleLayout";
    d_include = "LScaleLayout.h";
    d_domXml =
        "<widget class=\"LScaleLayout\" name=\"lscaleLayout\">\n"
        " <property name=\"geometry\">\n"
        "  <rect>\n"
        "   <x>20</x>\n"
        "   <y>20</y>\n"
        "   <width>160</width>\n"
        "   <height>80</height>\n"
        "  </rect>\n"
        " </property>\n"
        "</widget>\n";
}

QWidget *LScaleLayoutInterface::createWidget(QWidget *parent)
{
	int margin=6;
	int spacing=6;
	LScaleLayout *w = new LScaleLayout(parent, margin, spacing);
	w->setMinimumSize(10, 10);
    return w;
}

CustomLayoutCollectionInterface::CustomLayoutCollectionInterface(   
        QObject *parent): 
    QObject(parent)
{
    d_plugins.append(new LScaleLayoutInterface(this));
}

QList<QDesignerCustomWidgetInterface*> 
    CustomLayoutCollectionInterface::customWidgets(void) const
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
		/*
        if (CADisplayer *disp = qobject_cast<CADisplayer*>(object))
            return new TaskMenuExtension(disp, parent);
			*/
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

Q_EXPORT_PLUGIN2(KwtDesignerPlugin, CustomLayoutCollectionInterface)
