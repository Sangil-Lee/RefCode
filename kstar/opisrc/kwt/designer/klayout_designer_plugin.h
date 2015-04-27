/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Kwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Kwt License, Version 1.0
 *****************************************************************************/

#ifndef KLAYOUT_DESIGNER_PLUGIN_H
#define KLAYOUT_DESIGNER_PLUGIN_H

#include <qglobal.h>

#if QT_VERSION < 0x040000

#ifdef __GNUC__
#error This code is Qt4 only
#endif

#endif

#include <QDesignerCustomWidgetInterface>
#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

namespace KlayoutDesignerPlugin
{
	class CustomLayoutInterface: public QObject, 
			 public QDesignerCustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CustomLayoutInterface(QObject *parent);

			virtual bool isContainer() const;
			virtual bool isInitialized() const;
			virtual QIcon icon() const;
			virtual QString codeTemplate() const;
			virtual QString domXml() const;
			virtual QString group() const;
			virtual QString includeFile() const;
			virtual QString name() const;
			virtual QString toolTip() const;
			virtual QString whatsThis() const;
			virtual void initialize(QDesignerFormEditorInterface *);

		protected:
			QString d_name; 
			QString d_include; 
			QString d_toolTip; 
			QString d_whatsThis; 
			QString d_domXml; 
			QString d_codeTemplate;
			QIcon d_icon;

		private:
			bool d_isInitialized;
	};

	class CustomLayoutCollectionInterface: public QObject,
										   public QDesignerCustomWidgetCollectionInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

		public:
			CustomLayoutCollectionInterface(QObject *parent = NULL);

			virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

		private:
			QList<QDesignerCustomWidgetInterface*> d_plugins;
	};

	class LScaleLayoutInterface: public CustomLayoutInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			LScaleLayoutInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class TaskMenuFactory: public QExtensionFactory
	{
		Q_OBJECT

		public:
			TaskMenuFactory(QExtensionManager *parent = 0);

		protected:
			QObject *createExtension(QObject *object, 
					const QString &iid, QObject *parent) const;
	};

	class TaskMenuExtension: public QObject,
							 public QDesignerTaskMenuExtension
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerTaskMenuExtension)

		public:
			TaskMenuExtension(QWidget *widget, QObject *parent);

			QAction *preferredEditAction() const;
			QList<QAction *> taskActions() const;

			private slots:
				void editProperties();
			void applyProperties(const QString &);

		private:
			QAction *d_editAction;
			QWidget *d_widget;
	};
};

#endif
