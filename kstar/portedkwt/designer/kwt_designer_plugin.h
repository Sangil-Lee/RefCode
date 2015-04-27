/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Kwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Kwt License, Version 1.0
 *****************************************************************************/

#ifndef KWT_DESIGNER_PLUGIN_H
#define KWT_DESIGNER_PLUGIN_H

#include <QDesignerCustomWidgetInterface>
#include <QDesignerTaskMenuExtension>
#include <QExtensionFactory>

namespace KwtDesignerPlugin
{
	class CustomWidgetInterface: public QObject, 
			 public QDesignerCustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CustomWidgetInterface(QObject *parent);

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

	class CustomWidgetCollectionInterface: public QObject,
										   public QDesignerCustomWidgetCollectionInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
#if QT_VERSION >= 0x050000
		Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QDesignerCustomWidgetCollectionInterface" )
#endif

		public:
			CustomWidgetCollectionInterface(QObject *parent = NULL);

			virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

		private:
			QList<QDesignerCustomWidgetInterface*> d_plugins;
	};

	class CADisplayerInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CADisplayerInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAPushButtonInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAPushButtonInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CABoButtonInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CABoButtonInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CACheckBoxInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CACheckBoxInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAMbboButtonInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAMbboButtonInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAComboBoxInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAComboBoxInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CALineEditInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CALineEditInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CABlinkLabelInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CABlinkLabelInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

#if 0
	class BlinkLineInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			BlinkLineInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class BlinkLabelInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			BlinkLabelInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};
#endif

	class CAListBoxInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAListBoxInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};


	class CAMultiplotInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAMultiplotInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAMultiwaveplotInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAMultiwaveplotInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

#ifdef MDSPLUS
	class CAMDSplusplotInterface: public CustomWidgetInterface
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAMDSplusplotInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};
#endif

	class CALabelInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CALabelInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAImageMbbiInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAImageMbbiInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAWclockInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAWclockInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAColorCheckBoxInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAColorCheckBoxInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAGraphicInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAGraphicInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class StaticGraphicInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			StaticGraphicInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAUITimeInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAUITimeInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAScreenShotInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAScreenShotInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAQtSignalInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAQtSignalInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAFanOutInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAFanOutInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CASchedulerInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CASchedulerInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CASliderInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CASliderInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAProgressBarInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAProgressBarInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

#if 0
	class CAColorTileButtonInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAColorTileButtonInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};
	class ColorBarScaleWidgetInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			ColorBarScaleWidgetInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAWaveformPutInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAWaveformPutInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};
#endif

	class CATableListInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CATableListInterface(QObject *parent);
			virtual QWidget *createWidget(QWidget *parent);
	};

	class CAPopUpInterface: public CustomWidgetInterface 
	{
		Q_OBJECT
			Q_INTERFACES(QDesignerCustomWidgetInterface)

		public:
			CAPopUpInterface(QObject *parent);
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
