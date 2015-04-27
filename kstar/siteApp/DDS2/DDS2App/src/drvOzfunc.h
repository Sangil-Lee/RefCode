
#ifndef drvOzfunc_H
#define drvOzfunc_H

#include "drvM6802.h"


int ozInitDevice(unsigned short vme_addr);

int ozWriteRegister(drvM6802_taskConfig *ptaskConfig);
int ozSetSamplingRate( drvM6802_taskConfig *ptaskConfig, unsigned short samplingRate );
int ozSetAIGain(drvM6802_taskConfig *ptaskConfig, int ch, int gain);
int ozSetGlobalReset(drvM6802_taskConfig *ptaskConfig);
int ozSetADsFIFOrun(drvM6802_taskConfig *ptaskConfig);
int ozSetADsFIFOreset(drvM6802_taskConfig *ptaskConfig);
int ozSetChannelMask(drvM6802_taskConfig *ptaskConfig);
int ozSetClockInterExter(drvM6802_taskConfig *ptaskConfig);
int ozSetArmOut(drvM6802_taskConfig *ptaskConfig);
int ozSetArmReset(drvM6802_taskConfig *ptaskConfig);
int ozSetTermEnable(drvM6802_taskConfig *ptaskConfig);
int ozSetTermDisable(drvM6802_taskConfig *ptaskConfig);
int ozSetMasterOrSlave(drvM6802_taskConfig *ptaskConfig);
int ozSetBoardIDBit(drvM6802_taskConfig *ptaskConfig);
int ozSetSampleClkDivider(drvM6802_taskConfig *ptaskConfig, unsigned short clockDivider );
int ozSetTriggerReset(drvM6802_taskConfig *ptaskConfig);
int ozSetTriggerRelease(drvM6802_taskConfig *ptaskConfig);
int ozSetTriggerPolarity(drvM6802_taskConfig *ptaskConfig);
int ozSetTriggerExternal(drvM6802_taskConfig *ptaskConfig);
int ozSetTriggerInternal(drvM6802_taskConfig *ptaskConfig);
int ozSetZeroCalibration(drvM6802_taskConfig *ptaskConfig,int chan,  uint16 val);

int ozSetStrobeTTL(drvM6802_taskConfig *ptaskConfig);

#endif /* drvOzfunc_H */

