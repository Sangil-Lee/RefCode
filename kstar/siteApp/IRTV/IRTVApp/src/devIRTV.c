#include <sys/stat.h>
#include <sys/types.h>

#include "aiRecord.h"
#include "aoRecord.h"
#include "longinRecord.h"
#include "longoutRecord.h"
#include "biRecord.h"
#include "boRecord.h"
#include "mbbiRecord.h"
#include "mbboRecord.h"
#include "stringinRecord.h"
#include "stringoutRecord.h"
#include "waveformRecord.h"
#include "dbAccess.h"   /*#include "dbAccessDefs.h" --> S_db_badField */

#include "drvIRTV.h" 
#include "irtvCamInterface.h"

int assertNonArmEnabled (const char *name, ST_STD_device *pSTDdev)
{
	if (pSTDdev->StatusDev & TASK_ARM_ENABLED) {
		kLog (K_ERR, "[%s] %s: System is armed! \n", name, pSTDdev->taskName);
		notify_error (1, "System is armed!", pSTDdev->taskName);		
		return (FALSE);
	}
	
	return (TRUE);
}

int callSetROI (ST_STD_device *pSTDdev)
{
	ST_IRTV *pIRTV	= getDriver(pSTDdev);

	return ( irtvSetROI ((short)pIRTV->ao_roi_x, (short)pIRTV->ao_roi_y, 
				(short)pIRTV->ao_roi_width, (short)pIRTV->ao_roi_height, 
				(short)pIRTV->ao_roi_lifetime, (short)pIRTV->ao_roi_color, (short)pIRTV->bo_roi_coordi) );
}

// gap : 1(640x512 FPA sizes), 2(320x256 FPA sizes)
double calcMaxFrameRate (int nModelType, int gap, double cols, double rows, double intrMsecTime)
{
	double	fDataWidth, fIntrTime, fFrameRate;

	kLog (K_DEBUG, "[calcMaxFrameRate] model(%d) gap(%d) cols(%f) rows(%f) intTime(%f)\n", nModelType, gap, cols, rows, intrMsecTime);

	switch (nModelType) {
		//case 0 :	// SC4000/6000 for 640x512 FPA sizes
		case 1 :	// SC6100
			// Integration mode : ITR (Integration Then Read)
			fDataWidth	= (cols / 8 + 16) * (rows + 3) + 10;
			fIntrTime	= (intrMsecTime / 0.000160) + 36;
			fFrameRate	= 1000000000.0 / ((fDataWidth + fIntrTime) * 160);

			kLog (K_DEBUG, "[calcMaxFrameRate] fDataWidth(%f) fIntrTime(%f) fFrameRate(%f)\n", fDataWidth, fIntrTime, fFrameRate);
			break;

		case 0 :	// SC4000/6000 for 640x512 FPA sizes
		default :
			if ( (cols * rows) > (320 * 256) ) {
				fFrameRate	= 50000000 / ((rows + 3 + gap) * (cols + 128) + 8);
			}
			else {
				fFrameRate	= 50000000 / ((rows + 2 + gap) * (cols + 128) + 8);
			}

			kLog (K_DEBUG, "[calcMaxFrameRate] fFrameRate(%f)\n", fFrameRate);
			break;
	}

	return (fFrameRate);
}

double getMaxFrameRate ()
{
	ST_IRTV *pIRTV	= getFirstDriver();

	return ( calcMaxFrameRate (pIRTV->mbbo_cam_model_type, 
				1, pIRTV->ai_sts_width, pIRTV->ai_sts_height, pIRTV->ai_sts_int_time) );
}

static void devIRTV_SO_IMAGE_PATH(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_SO_IMAGE_PATH] so_image_path(%s)\n", pParam->setStr);

	strcpy (getFirstDriver()->so_image_path, pParam->setStr);
}

static void devIRTV_SO_SEL_NUC_NAME(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_SO_SEL_NUC_NAME] so_sel_nuc_name(%s)\n", pParam->setStr);

	strcpy (getFirstDriver()->so_sel_nuc_name, pParam->setStr);
}

static void devIRTV_BO_SOFT_TRIGGER(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_BO_SOFT_TRIGGER] bo_soft_trigger(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	getFirstDriver()->bo_soft_trigger = (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_SOFT_TRIGGER] before ... \n");

	irtvSendSoftTrigger ();

	kLog (K_MON, "[devIRTV_BO_SOFT_TRIGGER] after  ... \n");

	DBproc_put (sfwEnvGet("PV_NAME_SOFT_TRIGGER"), "0");
}

static void devIRTV_BO_LIVE_VIEW(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_BO_LIVE_VIEW] bo_live_view(%f)\n", pParam->setValue);

	getFirstDriver()->bo_live_view = (int)pParam->setValue;
}

static void devIRTV_BO_DAQ_STOP(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_BO_DAQ_STOP] bo_daq_stop(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	resetDeviceAll ();

	notify_refresh_master_status();

	scanIoRequest (get_master()->ioScanPvt_status);

	DBproc_put (sfwEnvGet("PV_NAME_DAQ_STOP"), "0");
	DBproc_put (sfwEnvGet("PV_NAME_SYS_RUN"), "0");
	DBproc_put (sfwEnvGet("PV_NAME_SYS_ARMING"), "0");
}

static void devIRTV_BO_AGC_ENABLE(ST_execParam *pParam)
{
	kLog (K_MON, "[devIRTV_BO_AGC_ENABLE] bo_agc_enable(%f)\n", pParam->setValue);

	kLog (K_MON, "[devIRTV_BO_AGC_ENABLE] before ... \n");

	irtvSetAGCEnable ((int)pParam->setValue);

	kLog (K_MON, "[devIRTV_BO_AGC_ENABLE] after  ... \n");
}

static void devIRTV_AO_GRAB_TIME(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);

	kLog (K_MON, "[devIRTV_AO_GRAB_TIME] ao_grab_time(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_GRAB_TIME", pSTDdev)) {
		return;
	}

	pIRTV->ao_grab_time	= pParam->setValue;
	setStopT1 (pIRTV, pIRTV->start_t0 + pParam->setValue);
}

static void devIRTV_AO_SYNC_SOURCE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_SYNC_SOURCE] ao_sync_source(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_SYNC_SOURCE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->ao_sync_source == pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->ao_sync_source	= pParam->setValue;

	kLog (K_MON, "[devIRTV_AO_SYNC_SOURCE] before ... \n");

	irtvSetSyncSource ((int)pParam->setValue);

	kLog (K_MON, "[devIRTV_AO_SYNC_SOURCE] after  ... \n");
}

static void devIRTV_AO_REFRESH_TIME(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_REFRESH_TIME] ao_refresh_time(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_REFRESH_TIME", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_refresh_time = pParam->setValue;
}

static void devIRTV_AO_ROI_X(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_X] ao_roi_x(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_X", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_x = pParam->setValue;
}

static void devIRTV_AO_ROI_Y(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_Y] ao_roi_y(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_Y", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_y = pParam->setValue;
}

static void devIRTV_AO_ROI_WIDTH(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_WIDTH] ao_roi_width(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_WIDTH", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_width = pParam->setValue;
}

static void devIRTV_AO_ROI_HEIGHT(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_HEIGHT] ao_roi_height(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_HEIGHT", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_height = pParam->setValue;
}

static void devIRTV_AO_ROI_LIFETIME(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_LIFETIME] ao_roi_lifetime(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_LIFETIME", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_lifetime = pParam->setValue;
}

static void devIRTV_AO_ROI_COLOR(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_ROI_COLOR] ao_roi_color(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_ROI_COLOR", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->ao_roi_color = pParam->setValue;
}

static void devIRTV_AO_GAIN_LEVEL(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_GAIN_LEVEL] ao_gain_level(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_GAIN_LEVEL", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->ao_gain_level == pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->ao_gain_level	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_AO_GAIN_LEVEL] before ... \n");

	irtvSetGainLevel (getDriver(pSTDdev)->ao_gain_level);

	kLog (K_MON, "[devIRTV_AO_GAIN_LEVEL] after  ... \n");
}

static void devIRTV_BO_INVERT_ENABLE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_INVERT_ENABLE] bo_invert_enable(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_INVERT_ENABLE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_invert_enable == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_invert_enable	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_INVERT_ENABLE] before ... \n");

	irtvSetInvertEnable (getDriver(pSTDdev)->bo_invert_enable);

	kLog (K_MON, "[devIRTV_BO_INVERT_ENABLE] after  ... \n");
}

static void devIRTV_BO_REVERT_ENABLE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_REVERT_ENABLE] bo_revert_enable(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_REVERT_ENABLE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_revert_enable == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_revert_enable	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_REVERT_ENABLE] before ... \n");

	irtvSetRevertEnable (getDriver(pSTDdev)->bo_revert_enable);

	kLog (K_MON, "[devIRTV_BO_REVERT_ENABLE] after  ... \n");
}

static void devIRTV_BO_SYNC_POLARITY(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_SYNC_POLARITY] bo_sync_polarity(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_SYNC_POLARITY", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_sync_polarity == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_sync_polarity	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_SYNC_POLARITY] before ... \n");

	irtvSetSyncPolarity (getDriver(pSTDdev)->bo_sync_polarity);

	kLog (K_MON, "[devIRTV_BO_SYNC_POLARITY] after  ... \n");
}

static void devIRTV_BO_FPA_COOLER_ON(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_FPA_COOLER_ON] bo_cooler_on(%d, %f/%d)\n", getDriver(pSTDdev)->bo_cooler_on, pParam->setValue, (int)pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_FPA_COOLER_ON", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_cooler_on == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_cooler_on	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_FPA_COOLER_ON] before ... \n");

	irtvSetCoolerOn (getDriver(pSTDdev)->bo_cooler_on);

	kLog (K_MON, "[devIRTV_BO_FPA_COOLER_ON] after  ... \n");
}

static void devIRTV_MBBO_FPA_COOLER_TYPE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_TYPE] mbbo_cooler_type(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_MBBO_FPA_COOLER_TYPE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->mbbo_cooler_type == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->mbbo_cooler_type	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_TYPE] before ... \n");

	irtvSetCoolerType (getDriver(pSTDdev)->mbbo_cooler_type);

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_TYPE] after  ... \n");
}

static void devIRTV_MBBO_FPA_COOLER_AUTO(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_AUTO] mbbo_cooler_auto(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_MBBO_FPA_COOLER_AUTO", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->mbbo_cooler_auto == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->mbbo_cooler_auto	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_AUTO] before ... \n");

	irtvSetCoolerAuto (getDriver(pSTDdev)->mbbo_cooler_auto);

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_AUTO] after  ... \n");
}

static void devIRTV_AO_FPA_OFF_TEMP(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_FPA_OFF_TEMP] ao_fpa_off_temp(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_FPA_OFF_TEMP", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->ao_fpa_off_temp == pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->ao_fpa_off_temp	= pParam->setValue;

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_TYPE] before ... \n");

	irtvSetFPAAutoOffTemp ((int)getDriver(pSTDdev)->ao_fpa_off_temp);

	kLog (K_MON, "[devIRTV_MBBO_FPA_COOLER_TYPE] after  ... \n");
}

static void devIRTV_AO_FPA_ON_TEMP(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_FPA_ON_TEMP] ao_fpa_on_temp(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_FPA_ON_TEMP", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->ao_fpa_on_temp == pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->ao_fpa_on_temp	= pParam->setValue;

	kLog (K_MON, "[devIRTV_AO_FPA_ON_TEMP] before ... \n");

	irtvSetFPAAutoOnTemp ((int)getDriver(pSTDdev)->ao_fpa_on_temp);

	kLog (K_MON, "[devIRTV_AO_FPA_ON_TEMP] after  ... \n");
}

static void devIRTV_BO_PRESET_SEQ_MODE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_PRESET_SEQ_MODE] bo_preset_seq_mode(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_PRESET_SEQ_MODE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_preset_seq_mode == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_preset_seq_mode	= (pParam->setValue ? 0 : 1);

	kLog (K_MON, "[devIRTV_BO_PRESET_SEQ_MODE] before ... \n");

	irtvSetPresetSeqMode (getDriver(pSTDdev)->bo_preset_seq_mode);

	kLog (K_MON, "[devIRTV_BO_PRESET_SEQ_MODE] after  ... \n");
}

static void devIRTV_BO_SET_ACTIVE_PRESET(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	static int		bFirstTime	= 1;

	kLog (K_MON, "[devIRTV_BO_SET_ACTIVE_PRESET] bo_set_active_preset(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_SET_ACTIVE_PRESET", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_set_active_preset == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_set_active_preset	= (int)pParam->setValue;

	if (bFirstTime) {
		bFirstTime = 0;
	}
	else {
		if (1 == getDriver(pSTDdev)->bo_set_active_preset) {
			irtvSetActivePreset (getDriver(pSTDdev)->ao_sel_preset_num);
		}
	}

	kLog (K_MON, "[devIRTV_BO_SET_ACTIVE_PRESET] after  ... \n");
}

static void devIRTV_AO_SEL_PRESET_NUM(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_AO_SEL_PRESET_NUM] ao_sel_preset_num(%f) \n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_SEL_PRESET_NUM", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->ao_sel_preset_num == pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->ao_sel_preset_num	= (int)pParam->setValue;

	if (OK != irtvSetConfigPreset (getDriver(pSTDdev)->ao_sel_preset_num)) {
		kLog (K_ERR, "[devIRTV_AO_SEL_PRESET_NUM] irtvSetConfigPreset(%d) failed \n", getDriver(pSTDdev)->ao_sel_preset_num);
		return;
	}

	if (1 == getDriver(pSTDdev)->bo_set_active_preset) {
		kLog (K_MON, "[devIRTV_AO_SEL_PRESET_NUM] active_preset_num(%d) \n", getDriver(pSTDdev)->ao_sel_preset_num);

		if (OK != irtvSetActivePreset (getDriver(pSTDdev)->ao_sel_preset_num)) {
			kLog (K_ERR, "[devIRTV_AO_SEL_PRESET_NUM] irtvSetActivePreset(%d) failed \n", getDriver(pSTDdev)->ao_sel_preset_num);
			return;
		}
	}

	kLog (K_MON, "[devIRTV_AO_SEL_PRESET_NUM] after  ... \n");
}

static void devIRTV_BO_FULL_NUC_ENABLE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_FULL_NUC_ENABLE] bo_full_nuc_enable(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_FULL_NUC_ENABLE", pSTDdev)) {
		return;
	}

	if (getDriver(pSTDdev)->bo_full_nuc_enable == (int)pParam->setValue) {
		return;
	}

	getDriver(pSTDdev)->bo_full_nuc_enable	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_FULL_NUC_ENABLE] before ... \n");

	irtvSetFullNucEnable (getDriver(pSTDdev)->bo_full_nuc_enable);

	kLog (K_MON, "[devIRTV_BO_FULL_NUC_ENABLE] after  ... \n");
}

static void devIRTV_MBBO_NUC_ACTION_TYPE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_MBBO_NUC_ACTION_TYPE] mbbo_nuc_action_type(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_MBBO_NUC_ACTION_TYPE", pSTDdev)) {
		return;
	}

	kLog (K_MON, "[devIRTV_MBBO_NUC_ACTION_TYPE] type(%d -> %f) \n", 
		getDriver(pSTDdev)->mbbo_nuc_action_type, pParam->setValue);

	getDriver(pSTDdev)->mbbo_nuc_action_type	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_MBBO_NUC_ACTION_TYPE] after  ... \n");
}

static void devIRTV_MBBO_CAM_MODEL_TYPE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_MBBO_CAM_MODEL_TYPE] mbbo_cam_model_type(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_MBBO_CAM_MODEL_TYPE", pSTDdev)) {
		return;
	}

	kLog (K_MON, "[devIRTV_MBBO_CAM_MODEL_TYPE] type(%d -> %f) \n", 
		getDriver(pSTDdev)->mbbo_cam_model_type, pParam->setValue);

	getDriver(pSTDdev)->mbbo_cam_model_type	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_MBBO_CAM_MODEL_TYPE] after  ... \n");
}

static void devIRTV_BO_NUC_ACTION_LOAD(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);

	kLog (K_MON, "[devIRTV_BO_NUC_ACTION_LOAD] bo_nuc_action_load(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_NUC_ACTION_LOAD", pSTDdev)) {
		return;
	}

	pIRTV->bo_nuc_action_load	= (int)pParam->setValue;

	kLog (K_MON, "[devIRTV_BO_NUC_ACTION_LOAD] before ... \n");

	irtvLoadNucAction (pIRTV->so_sel_nuc_name, (int)pIRTV->ai_config_preset_num, (int)pIRTV->ai_active_preset_num);

	kLog (K_MON, "[devIRTV_BO_NUC_ACTION_LOAD] after  ... \n");
}

static void devIRTV_AO_FPA_INT_TIME(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);
	int				nPreset		= pParam->n32Arg0;

	kLog (K_MON, "[devIRTV_AO_FPA_INT_TIME] ao_int_time(%f) preset(%d)\n", pParam->setValue, pParam->n32Arg0);

	if (TRUE != assertNonArmEnabled ("devIRTV_AO_FPA_INT_TIME", pSTDdev)) {
		return;
	}

	if (pIRTV->ao_preset_int_time[nPreset] == pParam->setValue) {
		return;
	}

	pIRTV->ao_preset_int_time[nPreset]	= pParam->setValue;

	kLog (K_MON, "[devIRTV_AO_FPA_INT_TIME] before ... \n");

	irtvSetFPAIntTime (nPreset, pIRTV->ao_preset_int_time[nPreset]);

	kLog (K_MON, "[devIRTV_AO_FPA_INT_TIME] after  ... \n");
}

static void devIRTV_BO_GET_STORED_NUCS(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);

	kLog (K_MON, "[devIRTV_BO_GET_STORED_NUCS] ...\n");

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_GET_STORED_NUCS", pSTDdev)) {
		return;
	}

	kLog (K_MON, "[devIRTV_BO_GET_STORED_NUCS] before ... \n");

	irtvGetStoredNUCs ();

	kLog (K_MON, "[devIRTV_BO_GET_STORED_NUCS] after  ... \n");
}

// -----------------

static void devIRTV_BO_ROI_COORDI(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();

	kLog (K_MON, "[devIRTV_BO_ROI_COORDI] bo_roi_coordi(%f)\n", pParam->setValue);

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_ROI_COORDI", pSTDdev)) {
		return;
	}

	getDriver(pSTDdev)->bo_roi_coordi = pParam->setValue;
}

static void devIRTV_BO_ROI_SAVE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);
	double	dMaxFrameRate;
	char	szMaxFrameRate[64];

	kLog (K_MON, "[devIRTV_BO_ROI_SAVE] bo_roi_save(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_ROI_SAVE", pSTDdev)) {
		return;
	}

	callSetROI (pSTDdev);

	dMaxFrameRate = getMaxFrameRate ();
	sprintf (szMaxFrameRate, "%.2f", dMaxFrameRate);

	DBproc_put (sfwEnvGet("PV_NAME_MAX_RATE"), szMaxFrameRate);
	DBproc_put (sfwEnvGet("PV_NAME_ROI_SAVE"), "0");
}

static void devIRTV_BO_ROI_FULL_SCALE(ST_execParam *pParam)
{
	ST_STD_device	*pSTDdev	= get_first_STDdev();
	ST_IRTV			*pIRTV		= getDriver(pSTDdev);

	kLog (K_MON, "[devIRTV_BO_ROI_FULL_SCALE] bo_roi_full_scale(%f)\n", pParam->setValue);

	if (0 == pParam->setValue) {
		return;
	}

	if (TRUE != assertNonArmEnabled ("devIRTV_BO_ROI_FULL_SCALE", pSTDdev)) {
		return;
	}

	pIRTV->ao_roi_x			= 0;
	pIRTV->ao_roi_y			= 0;
	pIRTV->ao_roi_width		= 640;
	pIRTV->ao_roi_height	= 512; 

	devIRTV_BO_ROI_SAVE (pParam);

	DBproc_put (sfwEnvGet("PV_NAME_ROI_FULL_SCALE"), "0");
}

static long devAoIRTV_init_record(aoRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s %s", pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAoIRTV_init_record] %s : %s\n", pSTdpvt->recordName, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (i > 1) {
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0); 
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devAoIRTVControl (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devAoIRTVControl (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp(pSTdpvt->arg0, AO_GRAB_TIME_STR)) 			{ pSTdpvt->ind = AO_GRAB_TIME; }
	else if (!strcmp(pSTdpvt->arg0, AO_SYNC_SOURCE_STR)) 	{ pSTdpvt->ind = AO_SYNC_SOURCE; }
	else if (!strcmp(pSTdpvt->arg0, AO_REFRESH_TIME_STR)) 	{ pSTdpvt->ind = AO_REFRESH_TIME; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_X_STR)) 			{ pSTdpvt->ind = AO_ROI_X; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_Y_STR)) 			{ pSTdpvt->ind = AO_ROI_Y; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_WIDTH_STR)) 		{ pSTdpvt->ind = AO_ROI_WIDTH; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_HEIGHT_STR)) 	{ pSTdpvt->ind = AO_ROI_HEIGHT; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_LIFETIME_STR)) 	{ pSTdpvt->ind = AO_ROI_LIFETIME; }
	else if (!strcmp(pSTdpvt->arg0, AO_ROI_COLOR_STR)) 		{ pSTdpvt->ind = AO_ROI_COLOR; }
	else if (!strcmp(pSTdpvt->arg0, AO_GAIN_LEVEL_STR)) 	{ pSTdpvt->ind = AO_GAIN_LEVEL; }
	else if (!strcmp(pSTdpvt->arg0, AO_FPA_OFF_TEMP_STR)) 	{ pSTdpvt->ind = AO_FPA_OFF_TEMP; }
	else if (!strcmp(pSTdpvt->arg0, AO_FPA_ON_TEMP_STR)) 	{ pSTdpvt->ind = AO_FPA_ON_TEMP; }
	else if (!strcmp(pSTdpvt->arg0, AO_SEL_PRESET_NUM_STR)) { pSTdpvt->ind = AO_SEL_PRESET_NUM; }
	else if (!strcmp(pSTdpvt->arg0, AO_FPA_INT_TIME_STR)) 	{ pSTdpvt->ind = AO_FPA_INT_TIME; }
	else {
		kLog (K_ERR, "[devAoIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;     /* don't convert */
}

static long devAoIRTV_write_ao(aoRecord *precord)
{
	ST_dpvt        *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;

	if (!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;

	qData.pFunc				= NULL;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;
	qData.param.n32Arg0		= pSTdpvt->n32Arg0;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_DEBUG, "[devAoIRTV_write_ao] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch (pSTdpvt->ind) {
			case AO_GRAB_TIME:
				qData.pFunc = devIRTV_AO_GRAB_TIME;
				break;
			case AO_SYNC_SOURCE:
				qData.pFunc = devIRTV_AO_SYNC_SOURCE;
				break;
			case AO_REFRESH_TIME:
				qData.pFunc = devIRTV_AO_REFRESH_TIME;
				break;
			case AO_ROI_X :
				qData.pFunc = devIRTV_AO_ROI_X;
				break;
			case AO_ROI_Y :
				qData.pFunc = devIRTV_AO_ROI_Y;
				break;
			case AO_ROI_WIDTH :
				qData.pFunc = devIRTV_AO_ROI_WIDTH;
				break;
			case AO_ROI_HEIGHT :
				qData.pFunc = devIRTV_AO_ROI_HEIGHT;
				break;
			case AO_ROI_LIFETIME :
				qData.pFunc = devIRTV_AO_ROI_LIFETIME;
				break;
			case AO_ROI_COLOR :
				qData.pFunc = devIRTV_AO_ROI_COLOR;
				break;

			case AO_GAIN_LEVEL :
				qData.pFunc = devIRTV_AO_GAIN_LEVEL;
				break;
			case AO_FPA_OFF_TEMP :
				qData.pFunc = devIRTV_AO_FPA_OFF_TEMP;
				break;
			case AO_FPA_ON_TEMP :
				qData.pFunc = devIRTV_AO_FPA_ON_TEMP;
				break;
			case AO_SEL_PRESET_NUM :
				qData.pFunc = devIRTV_AO_SEL_PRESET_NUM;
				break;
			case AO_FPA_INT_TIME :
				qData.pFunc = devIRTV_AO_FPA_INT_TIME;
				break;
			default :
				qData.pFunc = NULL;
				break;
		}

		if (NULL != qData.pFunc) {
			epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
					(void*) &qData,
					sizeof(ST_threadQueueData));
		}

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		kLog (K_DEBUG, "[devAoIRTV_write_ao] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 2;    /* don't convert */
	}

	return 0;
}

static long devAiIRTV_init_record(aiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s", pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devAiIRTV_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (i > 1) {
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0); 
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devAiIRTV (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devAiIRTV (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, AI_TIMESTAMP_STR)) {
		pSTdpvt->ind		= AI_TIMESTAMP;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_MAX_RATE_STR)) {
		pSTdpvt->ind		= AI_MAX_RATE;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_TEMP_STR)) {
		pSTdpvt->ind		= AI_FPA_TEMP;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_PRESET_STR)) {
		pSTdpvt->ind		= AI_PRESET;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_INT_TIME_STR)) {
		pSTdpvt->ind		= AI_INT_TIME;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_WIDTH_STR)) {
		pSTdpvt->ind		= AI_WIDTH;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_HEIGHT_STR)) {
		pSTdpvt->ind		= AI_HEIGHT;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FRAME_RATE_STR)) {
		pSTdpvt->ind		= AI_FRAME_RATE;
	}

	else if (!strcmp (pSTdpvt->arg0, AI_ACTIVE_PRESET_NUM_STR)) {
		pSTdpvt->ind		= AI_ACTIVE_PRESET_NUM;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_CONFIG_PRESET_NUM_STR)) {
		pSTdpvt->ind		= AI_CONFIG_PRESET_NUM;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_NUC_MEM_FREE_STR)) {
		pSTdpvt->ind		= AI_NUC_MEM_FREE;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_RATE_STR)) {
		pSTdpvt->ind		= AI_FPA_RATE;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_GAIN_STR)) {
		pSTdpvt->ind		= AI_FPA_GAIN;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_WIN_X_STR)) {
		pSTdpvt->ind		= AI_FPA_WIN_X;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_WIN_Y_STR)) {
		pSTdpvt->ind		= AI_FPA_WIN_Y;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_WIN_WIDTH_STR)) {
		pSTdpvt->ind		= AI_FPA_WIN_WIDTH;
	}
	else if (!strcmp (pSTdpvt->arg0, AI_FPA_WIN_HEIGHT_STR)) {
		pSTdpvt->ind		= AI_FPA_WIN_HEIGHT;
	}
	else {
		kLog (K_ERR, "[devAiIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devAiIRTV_get_ioint_info(int cmd, aiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devAiIRTV_read_ai(aiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_IRTV *pIRTV = NULL;
	int		preset = pSTdpvt->n32Arg0;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	pIRTV	= (ST_IRTV*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case AI_TIMESTAMP :
			precord->val = pIRTV->ai_timestamp;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_TIMESTAMP] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_MAX_RATE :
			//pIRTV->ai_max_rate = IRTV_GET_MAX_FRAME_RATE(1, pIRTV->ao_roi_width, pIRTV->ao_roi_height);
			precord->val = pIRTV->ai_max_rate;

			kLog (K_DATA, "[devAiIRTV_read_ai.AI_MAX_RATE] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_TEMP :
			precord->val = pIRTV->ai_sts_fpa_temp;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_TEMP] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_PRESET :
			precord->val = pIRTV->ai_sts_preset;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_PRESET] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_INT_TIME :
			precord->val = pIRTV->ai_sts_int_time;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_INT_TIME] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_WIDTH :
			precord->val = pIRTV->ai_sts_width;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_WIDTH] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_HEIGHT :
			precord->val = pIRTV->ai_sts_height;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_HEIGHT] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FRAME_RATE :
			precord->val = pIRTV->ai_sts_frame_rate;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FRAME_RATE] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_ACTIVE_PRESET_NUM :
			precord->val = pIRTV->ai_active_preset_num;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_ACTIVE_PRESET_NUM] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_CONFIG_PRESET_NUM :
			precord->val = pIRTV->ai_config_preset_num;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_CONFIG_PRESET_NUM] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_NUC_MEM_FREE :
			precord->val = pIRTV->ai_nuc_mem_free;
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_NUC_MEM_FREE] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_RATE :
			precord->val = pIRTV->ai_preset_rate[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_RATE] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_GAIN :
			precord->val = pIRTV->ai_preset_gain[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_NUC_MEM_FREE] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_WIN_X :
			precord->val = pIRTV->ai_preset_win_x[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_WIN_X] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_WIN_Y :
			precord->val = pIRTV->ai_preset_win_y[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_WIN_Y] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_WIN_WIDTH :
			precord->val = pIRTV->ai_preset_win_width[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_WIN_WIDTH] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case AI_FPA_WIN_HEIGHT :
			precord->val = pIRTV->ai_preset_win_height[preset];
			kLog (K_DATA, "[devAiIRTV_read_ai.AI_FPA_WIN_HEIGHT] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		default:
			kLog (K_ERR, "[devAiIRTV_read_ai] task(%s) name(%s) ind(%d) is invalid\n", pSTDdev->taskName, precord->name, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBiIRTV_init_record(biRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devBiIRTV_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devBiIRTV (init_record) Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devBiIRTV (init_record) Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, BI_STS_CONN_STR)) {
		pSTdpvt->ind		= BI_STS_CONN;
	}
	else if (!strcmp (pSTdpvt->arg0, BI_PRESET_CYCLING_STR)) {
		pSTdpvt->ind		= BI_PRESET_CYCLING;
	}
	else {
		kLog (K_ERR, "[devBiIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devBiIRTV_get_ioint_info(int cmd, biRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devBiIRTV_read_bi(biRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_IRTV *pIRTV = NULL;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	pIRTV	= (ST_IRTV*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case BI_STS_CONN :
			precord->val = (epicsEnum16)pIRTV->bi_sts_conn;
			kLog (K_DATA, "[devBiIRTV_read_bi.BI_STS_CONN] %s : %f\n", pSTDdev->taskName, precord->val);
			break;
		case BI_PRESET_CYCLING :
			precord->val = (epicsEnum16)pIRTV->bi_preset_cycling;
			kLog (K_DATA, "[devBiIRTV_read_bi.BI_PRESET_CYCLING] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		default:
			kLog (K_ERR, "[devBiIRTV_read_bi] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devBoIRTV_init_record(boRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devBoIRTV_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devBoIRTVControl (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devBoIRTVControl (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind = -1;

	if (!strcmp(pSTdpvt->arg0, BO_SOFT_TRIGGER_STR)) {
		pSTdpvt->ind = BO_SOFT_TRIGGER;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_LIVE_VIEW_STR)) {
		pSTdpvt->ind = BO_LIVE_VIEW;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_ROI_SAVE_STR)) {
		pSTdpvt->ind = BO_ROI_SAVE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_ROI_FULL_SCALE_STR)) {
		pSTdpvt->ind = BO_ROI_FULL_SCALE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_ROI_COORDI_STR)) {
		pSTdpvt->ind = BO_ROI_COORDI;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_DAQ_STOP_STR)) {
		pSTdpvt->ind = BO_DAQ_STOP;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_AGC_ENABLE_STR)) {
		pSTdpvt->ind = BO_AGC_ENABLE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_INVERT_ENABLE_STR)) {
		pSTdpvt->ind = BO_INVERT_ENABLE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_REVERT_ENABLE_STR)) {
		pSTdpvt->ind = BO_REVERT_ENABLE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_SYNC_POLARITY_STR)) {
		pSTdpvt->ind = BO_SYNC_POLARITY;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_FPA_COOLER_ON_STR)) {
		pSTdpvt->ind = BO_FPA_COOLER_ON;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_PRESET_SEQ_MODE_STR)) {
		pSTdpvt->ind = BO_PRESET_SEQ_MODE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_SET_ACTIVE_PRESET_STR)) {
		pSTdpvt->ind = BO_SET_ACTIVE_PRESET;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_FULL_NUC_ENABLE_STR)) {
		pSTdpvt->ind = BO_FULL_NUC_ENABLE;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_NUC_ACTION_LOAD_STR)) {
		pSTdpvt->ind = BO_NUC_ACTION_LOAD;
	} 
	else if (!strcmp(pSTdpvt->arg0, BO_GET_STORED_NUCS_STR)) {
		pSTdpvt->ind = BO_GET_STORED_NUCS;
	} 
	else {
		kLog (K_ERR, "ERROR! devBoIRTV_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devBoIRTV_write_bo(boRecord *precord)
{
	ST_dpvt 	   *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData		   qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev 				= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev 	= pSTDdev;
	qData.param.precord 	= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case BO_SOFT_TRIGGER:
				qData.pFunc = devIRTV_BO_SOFT_TRIGGER;
				break;
			case BO_LIVE_VIEW:
				qData.pFunc = devIRTV_BO_LIVE_VIEW;
				break;
			case BO_ROI_SAVE:
				qData.pFunc = devIRTV_BO_ROI_SAVE;
				break;
			case BO_ROI_FULL_SCALE:
				qData.pFunc = devIRTV_BO_ROI_FULL_SCALE;
				break;
			case BO_ROI_COORDI:
				qData.pFunc = devIRTV_BO_ROI_COORDI;
				break;
			case BO_DAQ_STOP:
				qData.pFunc = devIRTV_BO_DAQ_STOP;
				break;
			case BO_AGC_ENABLE:
				qData.pFunc = devIRTV_BO_AGC_ENABLE;
				break;
			case BO_INVERT_ENABLE:
				qData.pFunc = devIRTV_BO_INVERT_ENABLE;
				break;
			case BO_REVERT_ENABLE:
				qData.pFunc = devIRTV_BO_REVERT_ENABLE;
				break;
			case BO_SYNC_POLARITY:
				qData.pFunc = devIRTV_BO_SYNC_POLARITY;
				break;
			case BO_FPA_COOLER_ON:
				qData.pFunc = devIRTV_BO_FPA_COOLER_ON;
				break;
			case BO_PRESET_SEQ_MODE:
				qData.pFunc = devIRTV_BO_PRESET_SEQ_MODE;
				break;
			case BO_SET_ACTIVE_PRESET:
				qData.pFunc = devIRTV_BO_SET_ACTIVE_PRESET;
				break;
			case BO_FULL_NUC_ENABLE:
				qData.pFunc = devIRTV_BO_FULL_NUC_ENABLE;
				break;
			case BO_NUC_ACTION_LOAD:
				qData.pFunc = devIRTV_BO_NUC_ACTION_LOAD;
				break;
			case BO_GET_STORED_NUCS:
				qData.pFunc = devIRTV_BO_GET_STORED_NUCS;
				break;
			default: 
				qData.pFunc = NULL;
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				(void*) &qData,
				sizeof(ST_threadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		kLog (K_INFO, "db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pSTdpvt->ind) {
			default:
				break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

static long devSoIRTV_init_record(stringoutRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devSoIRTV_init_record] %s : %s\n", pSTdpvt->recordName, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devSoIRTVControl (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devSoIRTVControl (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;
			return S_db_badField;
	}

	pSTdpvt->ind = -1;

	if (!strcmp(pSTdpvt->arg0, SO_IMAGE_PATH_STR)) 			{ pSTdpvt->ind = SO_IMAGE_PATH; }
	else if (!strcmp(pSTdpvt->arg0, SO_SEL_NUC_NAME_STR)) 	{ pSTdpvt->ind = SO_SEL_NUC_NAME; }
	else {
		kLog (K_ERR, "[devSoIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 0; /*returns: (-1,0)=>(failure,success)*/
}

static long devSoIRTV_write_so(stringoutRecord *precord)
{
	ST_dpvt        *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData         qData;

	if (!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return -1; /*(-1,0)=>(failure,success)*/
	}

	pSTDdev					= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;

	qData.pFunc				= NULL;
	qData.param.pSTDdev		= pSTDdev;
	qData.param.precord		= (struct dbCommon *)precord;

	strcpy (qData.param.setStr, precord->val);

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_DEBUG, "[devSoIRTV_write_so] db processing: phase I %s (%s)\n", precord->name,
				epicsThreadGetNameSelf());

		switch (pSTdpvt->ind) {
			case SO_IMAGE_PATH:
				qData.pFunc = devIRTV_SO_IMAGE_PATH;
				break;
			case SO_SEL_NUC_NAME:
				qData.pFunc = devIRTV_SO_SEL_NUC_NAME;
				break;
			default :
				qData.pFunc = NULL;
				break;
		}

		if (NULL != qData.pFunc) {
			epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
					(void*) &qData,
					sizeof(ST_threadQueueData));
		}

		return 0;
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		kLog (K_DEBUG, "[devSoIRTV_write_so] db processing: phase II %s (%s)\n",
				precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		return 0;    /*(-1,0)=>(failure,success)*/
	}

	return -1; /*(-1,0)=>(failure,success)*/
}

static long devMbboIRTV_init_record(mbboRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->out.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->out.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devMbboIRTV_init_record] arg num: %d: %s\n",i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();
			if(!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord, 
						"devMbboIRTV_init_record (init_record) Illegal INP field: task_name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField,(void*) precord,
					"devMbboIRTV_init_record (init_record) Illegal OUT field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind = -1;

	if (!strcmp(pSTdpvt->arg0, MBBO_FPA_COOLER_TYPE_STR)) {
		pSTdpvt->ind = MBBO_FPA_COOLER_TYPE;
	} 
	else if (!strcmp(pSTdpvt->arg0, MBBO_FPA_COOLER_AUTO_STR)) {
		pSTdpvt->ind = MBBO_FPA_COOLER_AUTO;
	} 
	else if (!strcmp(pSTdpvt->arg0, MBBO_NUC_ACTION_TYPE_STR)) {
		pSTdpvt->ind = MBBO_NUC_ACTION_TYPE;
	} 
	else if (!strcmp(pSTdpvt->arg0, MBBO_CAM_MODEL_TYPE_STR)) {
		pSTdpvt->ind = MBBO_CAM_MODEL_TYPE;
	} 
	else {
		kLog (K_ERR, "ERROR! devMbboIRTV_init_record: arg0 \"%s\" \n",  pSTdpvt->arg0);
	}

	precord->udf = FALSE;
	precord->dpvt = (void*) pSTdpvt;

	return 2;	  /*returns:(0,2)=>(success,success no convert*/
}

static long devMbboIRTV_write_mbbo(mbboRecord *precord)
{
	ST_dpvt 	   *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device  *pSTDdev;
	ST_threadCfg   *pControlThreadConfig;
	ST_threadQueueData		   qData;

	if(!pSTdpvt || precord->udf == TRUE) {
		precord->pact = TRUE;
		return 0;
	}

	pSTDdev 				= pSTdpvt->pSTDdev;
	pControlThreadConfig	= pSTDdev->pST_stdCtrlThread;
	qData.param.pSTDdev 	= pSTDdev;
	qData.param.precord 	= (struct dbCommon *)precord;
	qData.param.setValue	= precord->val;

	/* db processing: phase I */
	if(precord->pact == FALSE) {	
		precord->pact = TRUE;

		kLog (K_INFO, "db processing: phase I %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		switch(pSTdpvt->ind) {
			case MBBO_FPA_COOLER_TYPE:
				qData.pFunc = devIRTV_MBBO_FPA_COOLER_TYPE;
				break;
			case MBBO_FPA_COOLER_AUTO:
				qData.pFunc = devIRTV_MBBO_FPA_COOLER_AUTO;
				break;
			case MBBO_NUC_ACTION_TYPE:
				qData.pFunc = devIRTV_MBBO_NUC_ACTION_TYPE;
				break;
			case MBBO_CAM_MODEL_TYPE:
				qData.pFunc = devIRTV_MBBO_CAM_MODEL_TYPE;
				break;
			default :
				qData.pFunc = NULL;
				break;
		}

		epicsMessageQueueSend(pControlThreadConfig->threadQueueId,
				(void*) &qData,
				sizeof(ST_threadQueueData));

		return 0; /*returns: (-1,0)=>(failure,success)*/
	}

	/* db processing: phase II -post processing */
	if(precord->pact == TRUE) {
		kLog (K_INFO, "db processing: phase II %s (%s)\n", precord->name, epicsThreadGetNameSelf());

		precord->pact = FALSE;
		precord->udf = FALSE;

		switch(pSTdpvt->ind) {
			default:
				break;
		}

		return 0;    /*returns: (-1,0)=>(failure,success)*/
	}

	return -1;
}

static long devMbbiIRTV_init_record(mbbiRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devMbbiIRTV_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"devMbbiIRTV_init_record Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"devMbbiIRTV_init_record Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, MBBI_SYNC_SRC_STR)) {
		pSTdpvt->ind		= MBBI_SYNC_SRC;
	}
	else if (!strcmp (pSTdpvt->arg0, MBBI_SYNC_POLARITY_STR)) {
		pSTdpvt->ind		= MBBI_SYNC_POLARITY;
	}
	else if (!strcmp (pSTdpvt->arg0, MBBI_NUC_ACTION_STATUS_STR)) {
		pSTdpvt->ind		= MBBI_NUC_ACTION_STATUS;
	}
	else {
		kLog (K_ERR, "[devMbbiIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devMbbiIRTV_get_ioint_info(int cmd, mbbiRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devMbbiIRTV_read_mbbi(mbbiRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_IRTV *pIRTV = NULL;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	pIRTV	= (ST_IRTV*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case MBBI_SYNC_SRC :
			precord->rval	= (epicsUInt32)pIRTV->mbbi_sts_sync_src;
			precord->val	= (epicsEnum16)pIRTV->mbbi_sts_sync_src;
			kLog (K_DATA, "[devMbbiIRTV_read_ai.MBBI_SYNC_SRC] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case MBBI_SYNC_POLARITY :
			precord->rval	= (epicsUInt32)pIRTV->mbbi_sts_sync_polarity;
			precord->val	= (epicsEnum16)pIRTV->mbbi_sts_sync_polarity;
			kLog (K_DATA, "[devMbbiIRTV_read_ai.MBBI_SYNC_PORARITY] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		case MBBI_NUC_ACTION_STATUS :
			precord->rval	= (epicsUInt32)pIRTV->mbbi_nuc_action_status;
			precord->val	= (epicsEnum16)pIRTV->mbbi_nuc_action_status;
			kLog (K_DATA, "[devMbbiIRTV_read_ai.MBBI_NUC_ACTION_STATUS] %s : %f\n", pSTDdev->taskName, precord->val);
			break;

		default:
			kLog (K_ERR, "[devMbbiIRTV_read_ai] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devSiIRTV_init_record(stringinRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s %s", pSTdpvt->arg0, pSTdpvt->arg1);

			kLog (K_INFO, "[devSiIRTV_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (i > 1) {
				pSTdpvt->n32Arg0 = strtoul(pSTdpvt->arg1, NULL, 0); 
			}

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"[devSiIRTV_init_record] Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"[devSiIRTV_init_record] Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, SI_SYNC_SRC_NAME_STR)) {
		pSTdpvt->ind		= SI_SYNC_SRC_NAME;
	}
	else if (!strcmp (pSTdpvt->arg0, SI_SYNC_POLARITY_NAME_STR)) {
		pSTdpvt->ind		= SI_SYNC_POLARITY_NAME;
	}
	else if (!strcmp (pSTdpvt->arg0, SI_FPA_NUC_NAME_STR)) {
		pSTdpvt->ind		= SI_FPA_NUC_NAME;
		pSTdpvt->n32Arg0	= strtoul(pSTdpvt->arg1, NULL, 0); 
	}
	else {
		kLog (K_ERR, "[devSiIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devSiIRTV_get_ioint_info(int cmd, stringinRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devSiIRTV_read_si(stringinRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_IRTV *pIRTV = NULL;
	int	nPreset;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	pIRTV	= (ST_IRTV*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case SI_SYNC_SRC_NAME :
			if (0 == pIRTV->mbbi_sts_sync_src)		strcpy (precord->val, "Free");
			else if (1 == pIRTV->mbbi_sts_sync_src)	strcpy (precord->val, "External");
			else if (2 == pIRTV->mbbi_sts_sync_src)	strcpy (precord->val, "Video");
			else if (3 == pIRTV->mbbi_sts_sync_src)	strcpy (precord->val, "SW");
			else strcpy (precord->val, "N/A");

			kLog (K_DATA, "[devSiIRTV_read_si.SI_STS_SYNC_SRC_NAME] %s : %s\n", pSTDdev->taskName, precord->val);
			break;

		case SI_SYNC_POLARITY_NAME :
			if (0 == pIRTV->mbbi_sts_sync_polarity)			strcpy (precord->val, "Falling");
			else if (1 == pIRTV->mbbi_sts_sync_polarity)	strcpy (precord->val, "Rising");
			else strcpy (precord->val, "N/A");

			kLog (K_DATA, "[devSiIRTV_read_si.SI_SYNC_POLARITY_NAME] %s : %s\n", pSTDdev->taskName, precord->val);
			break;

		case SI_FPA_NUC_NAME :
			nPreset	= pSTdpvt->n32Arg0;
			strcpy (precord->val, pIRTV->si_preset_nuc_name[nPreset]);

			kLog (K_DATA, "[devSiIRTV_read_si.SI_FPA_NUC_NAME] %s : %s\n", pSTDdev->taskName, precord->val);
			break;

		default:
			kLog (K_ERR, "[devSiIRTV_read_si] task(%s) name(%s) ind(%d) is invalid\n", pSTDdev->taskName, precord->name, pSTdpvt->ind); 
			break;
	}

	return (2);
}

static long devWaveIRTV_init_record(waveformRecord *precord)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) malloc(sizeof(ST_dpvt));
	int i;

	switch(precord->inp.type) {
		case INST_IO:
			strcpy(pSTdpvt->recordName, precord->name);
			i = sscanf(precord->inp.value.instio.string, "%s", pSTdpvt->arg0);

			kLog (K_INFO, "[devWaveIRTV_init_record] %d: %s\n", i, pSTdpvt->arg0);

			pSTdpvt->pSTDdev = get_first_STDdev();

			if (!pSTdpvt->pSTDdev) {
				recGblRecordError(S_db_badField, (void*) precord,
						"[devWaveIRTV_init_record] Illegal INP field: task name");
				free(pSTdpvt);
				precord->udf = TRUE;
				precord->dpvt = NULL;
				return S_db_badField;
			}

			break;

		default:
			recGblRecordError(S_db_badField, (void*) precord,
					"[devWaveIRTV_init_record] Illegal INP field");
			free(pSTdpvt);
			precord->udf = TRUE;
			precord->dpvt = NULL;

			return S_db_badField;
	}

	pSTdpvt->ind	= -1;

	if (!strcmp (pSTdpvt->arg0, WAVE_STORED_NUC_LIST_STR)) {
		pSTdpvt->ind		= WAVE_STORED_NUC_LIST;
	}
	else {
		kLog (K_ERR, "[devWaveIRTV_init_record] %s not found ...\n", pSTdpvt->arg0);
	}

	precord->udf	= FALSE;
	precord->dpvt	= (void*) pSTdpvt;

	return 2;    /* don't convert */ 
}

static long devWaveIRTV_get_ioint_info(int cmd, waveformRecord *precord, IOSCANPVT *ioScanPvt)
{
	ST_dpvt *pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;

	if(!pSTdpvt) {
		ioScanPvt = NULL;
		return 0;
	}

	pSTDdev = pSTdpvt->pSTDdev;

	*ioScanPvt	= pSTDdev->ioScanPvt_userCall;

	return 0;
}

static long devWaveIRTV_read_array(waveformRecord *precord)
{ 
	ST_dpvt	*pSTdpvt = (ST_dpvt*) precord->dpvt;
	ST_STD_device *pSTDdev;
	ST_IRTV *pIRTV = NULL;
	int		i;
	int		offset;

	if(!pSTdpvt) return 0;

	pSTDdev	= pSTdpvt->pSTDdev;
	pIRTV	= (ST_IRTV*)pSTDdev->pUser;

	switch (pSTdpvt->ind) {
		case WAVE_STORED_NUC_LIST :
			for (i = 0; i < pIRTV->num_stored_nucs; i++) {
				offset	= i * MAX_STRING_SIZE;
				memcpy ((char *)precord->bptr + offset, pIRTV->wave_stored_nucs[i], MAX_STRING_SIZE);
			}

			precord->nord = pIRTV->num_stored_nucs;

			kLog (K_DEBUG, "[devWaveIRTV_read_array.WAVE_STORED_NUC_LIST] %s : %s\n", pSTDdev->taskName, precord->bptr);
			break;

		default:
			kLog (K_ERR, "[devWaveIRTV_read_array] task(%s) ind(%d) is invalid\n", pSTDdev->taskName, pSTdpvt->ind); 
			break;
	}

	return (2);
}

BINARYDSET	devSoIRTVControl = { 6, NULL, NULL, devSoIRTV_init_record, NULL, devSoIRTV_write_so, NULL };
BINARYDSET	devAoIRTVControl = { 6, NULL, NULL, devAoIRTV_init_record, NULL, devAoIRTV_write_ao, NULL };
BINARYDSET	devBoIRTVControl = { 5, NULL, NULL, devBoIRTV_init_record, NULL, devBoIRTV_write_bo };
BINARYDSET	devMbboIRTVControl = { 5, NULL, NULL, devMbboIRTV_init_record, NULL, devMbboIRTV_write_mbbo };
BINARYDSET	devAiIRTVRead    = { 6, NULL, NULL, devAiIRTV_init_record, devAiIRTV_get_ioint_info, devAiIRTV_read_ai, NULL };
BINARYDSET	devBiIRTVRead    = { 5, NULL, NULL, devBiIRTV_init_record, devBiIRTV_get_ioint_info, devBiIRTV_read_bi };
BINARYDSET	devSiIRTVRead    = { 5, NULL, NULL, devSiIRTV_init_record, devSiIRTV_get_ioint_info, devSiIRTV_read_si };
BINARYDSET	devMbbiIRTVRead  = { 5, NULL, NULL, devMbbiIRTV_init_record, devMbbiIRTV_get_ioint_info, devMbbiIRTV_read_mbbi };
BINARYDSET	devWaveIRTVRead  = { 5, NULL, NULL, devWaveIRTV_init_record, devWaveIRTV_get_ioint_info, devWaveIRTV_read_array };

epicsExportAddress(dset, devAoIRTVControl);
epicsExportAddress(dset, devBoIRTVControl);
epicsExportAddress(dset, devSoIRTVControl);
epicsExportAddress(dset, devMbboIRTVControl);
epicsExportAddress(dset, devAiIRTVRead);
epicsExportAddress(dset, devBiIRTVRead);
epicsExportAddress(dset, devSiIRTVRead);
epicsExportAddress(dset, devMbbiIRTVRead);
epicsExportAddress(dset, devWaveIRTVRead);
