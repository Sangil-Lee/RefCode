#ifndef IRTV_S1_WINDOW_H
#define IRTV_S1_WINDOW_H

#include "irtvMainWindow.h"
#include "ui_DDS_IRTV_S1.h"

class IRTV_S1Window : public QObject, 
	public IRTV_MainWindow, public IRTV_Interface, public Ui::IrtvS1Window
{
    Q_OBJECT
public:
    IRTV_S1Window (QWidget *parent = 0);
    ~IRTV_S1Window ();

	void init ();
	void updateWindowCoordi ();
	void setWindowCoordi (char *x, char *y, char *width, char *height);

private:
	void createActions ();
	void createConnects ();

	void startTimer ();
	void stopTimer ();
	void startTimer_PV ();
	void displayGrabFrame ();
	void updateGrabFrameData ();

	void initCA ();
	void getCA ();

signals:
	void signal_updateFilePath ();

private slots:
	// Timer
	void timerCallback ();
	void timerCallback_PV ();

public slots:
	void on_btn_FileDialog_clicked ();
	void on_btn_RefreshTime_changed ();
	void on_btn_DisplayOnOff_clicked ();
	void on_btn_UpdateROIView_clicked ();

	void on_btn_ROIFullScale_clicked ();
	void on_edit_ROI_Width_changed ();
	void on_edit_ROI_Height_changed ();
	void on_edit_ROI_X_changed ();
	void on_edit_ROI_Y_changed ();

	void on_edit_FrameRate_changed ();
	void on_slider_FrameRate_changed ();

	void on_table_StoredNUC_cellClickd (int row, int col);

	// Sync Source
	void on_radio_SyncSource_Free_clicked ();
	void on_radio_SyncSource_SWTrigger_clicked ();
	void on_radio_SyncSource_External_clicked ();
	
	// External Sync Polarity
	void on_radio_PolarityFalling_clicked ();
	void on_radio_PolarityRising_clicked ();

	// FPA Auto Off/On
	void on_radio_FPA_Disable_clicked ();
	void on_radio_FPA_Enable_clicked ();
	void on_radio_FPA_Auto_clicked ();
	
	// Preset Selection
	void on_radio_Preset0_clicked ();
	void on_radio_Preset1_clicked ();
	void on_radio_Preset2_clicked ();
	void on_radio_Preset3_clicked ();
	
	// NUC Action
	void on_radio_Action_Perform_clicked ();
	void on_radio_Action_Load_clicked ();
	void on_radio_Action_Unload_clicked ();
	void on_radio_Action_Save_clicked ();
	
private:

};

#endif // IRTV_S1_WINDOW_H
