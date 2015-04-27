#ifndef drvHTstream_H
#define drvHTstream_H

#include "drvACQ196.h"
#include "sfwDriver.h"

/* I have to change this function, I don't need deviceChannel mapping because I receive data aready changed data from target acq196  2012.8.25  */
#if 0
// TG Remove
int acq196_initMapping(DeviceMapping *mapping, int slot);
void acq196_closeMapping( DeviceMapping *mapping );
#endif
//remove TG
//int acq196_htstream(ST_STD_device *pSTdev);

int acq196_set_SlaveMode(ST_STD_device *pSTdev);
int acq196_set_MasterMode(ST_STD_device *pSTdev);
int acq196_set_SingleMode(ST_STD_device *pSTdev);

int acq196_channel_Block_Mask(ST_STD_device *pSTDdev);
int acq196_route_LEMO_to_FpgaPXI(ST_STD_device *pSTdev);
int acq196_route_LEMO_to_FPGA(ST_STD_device *pSTdev);
int acq196_route_FPGA_to_PXI(ST_STD_device *pSTdev);
int acq196_route_PXI_to_FPGA(ST_STD_device *pSTdev);
int acq196_route_FPGA_to_FPGA(ST_STD_device *pSTdev);


int acq196_stream_to_elastic_store(ST_STD_device *pSTdev);
int acq196_set_ARM(ST_STD_device *pSTdev);
int acq196_set_MODE(ST_STD_device *pSTdev);
int acq196_set_ABORT(ST_STD_device *pSTdev);
int acq196_set_TriggerConfig(ST_STD_device *pSTdev);


int acq196_setDIO_rising(ST_STD_device *pSTdev);
int acq196_setDIO_falling(ST_STD_device *pSTdev);
int acq196_setDIO_input(ST_STD_device *pSTdev);

int acq196_initialize(ST_STD_device *pSTdev);

int acq196_set_internalClk_DO0(ST_STD_device *pSTdev);
int acq196_set_externalClk_DI0(ST_STD_device *pSTdev);


// I don't need this function
//int acq196_set_maxdma(ST_STD_device *pSTdev);

#endif

