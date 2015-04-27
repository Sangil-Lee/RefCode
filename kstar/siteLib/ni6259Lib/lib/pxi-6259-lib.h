/*************************************************************************
 * $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/lib/pxi-6259-lib.h $
 * $Id: pxi-6259-lib.h 34216 2013-02-20 09:00:23Z cesnikt $
 *
 * Project       : CODAC Core System
 *
 * Description   : National Instruments PXI-6259 library.
 *
 * Author        : Janez Golob (Cosylab d.d)
 *
 * Copyright (c) : 2010-2013 ITER Organization,
 *                 CS 90 046
 *                 13067 St. Paul-lez-Durance Cedex
 *                 France
 *
 * This file is part of ITER CODAC software.
 * For the terms and conditions of redistribution or use of this software
 * refer to the file ITER-LICENSE.TXT located in the top level directory
 * of the distribution package.
 ************************************************************************/

/**
 * @mainpage PXI-6259 API introduction
 *
 * The PXI-6259 API provides abstraction layer from hardware PXI-6259 implementation.
 *
 * A key concept of the PXI-6259 API is to provide applications means to communicate
 * with PXI-6259 device driver.
 */

/**
 * @file
 * This file contains almost all important function definitions, that encapsulate and ease communication with PXI-6259 device driver.
 */

#ifndef PXI6259LIB_H_
#define PXI6259LIB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <pxi6259.h>
#include <pxi6259-enums.h>

typedef struct pxi6259_ai_conf pxi6259_ai_conf_t;
typedef struct pxi6259_ao_conf pxi6259_ao_conf_t;
typedef struct pxi6259_dio_conf pxi6259_dio_conf_t;
typedef struct pxi6259_gpc_conf pxi6259_gpc_conf_t;


/********************************************/
/* CONFIGURATION AND CONTROL FUNCTIONS      */
/********************************************/

/**
 * Reset PXI-6259 device.
 *
 * This function resets the PXI-6259 device to the known state. All the hardware
 * segments are also reseted.
 *
 * On error, -1 is returned, and errno may be one of the following:
 *  - EBUSY
 *    Device is busy.
 *
 * @param fd            The pxi6259 file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_reset(int fd);

/**
 * Reset AI segment on PXI-6259 device.
 *
 * This function resets the AI segment and AI triggers of PXI-6259 device to the known state.
 *
 * On error, -1 is returned, and errno may be one of the following:
 *  - EBUSY
 *    Device is busy.
 *
 * @param fd            The AI segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_reset_ai(int fd);


/**
 * Reset AO segment on PXI-6259 device.
 *
 * This function resets the AO segment of PXI-6259 device to the known state.
 *
 * On error, -1 is returned, and errno may be one of the following:
 *  - EBUSY
 *    Device is busy.
 *
 * @param fd            The AO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_reset_ao(int fd);

/**
 * Reset DIO segment on PXI-6259 device.
 *
 * This function resets the DIO segment of PXI-6259 device to the known state.
 *
 * On error, -1 is returned, and errno may be one of the following:
 *  - EBUSY
 *    Device is busy.
 *
 * @param fd            The DIO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_reset_dio(int fd);

/**
 * Reset GPC segment on PXI-6259 device.
 *
 * This function resets the GPC segment of PXI-6259 device to the known state.
 *
 * On error, -1 is returned, and errno may be one of the following:
 *  - EBUSY
 *    Device is busy.
 *
 * @param fd            The GPC segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_reset_gpc(int fd);



/**
 * Create an analog input segment configuration.
 *
 * Function creates the configuration structure. This configuration structure can then
 * be loaded to configure multiple cards. The same structure can then be used to
 * retrieve/set various attributes.
 *
 * Note that the following default values are set:
 *      - AI_CONVERT_OUTPUT_SELECT to AI_CONVERT_OUTPUT_SELECT_ACTIVE_LOW
 *      - AI_SIP_OUTPUT_SELECT to AI_SIP_OUTPUT_SELECT_ACTIVE_HIGH
 *      - AI_CONVERT_PULSE_WIDTH to AI_CONVERT_PULSE_WIDTH_1_ClOCK_PERIOD
 *      - AI_EXTERNAL_MUX_PRESENT to AI_EXTERNAL_MUX_PRESENT_EVERY_CONVERT
 *      - AI_EXTMUX_OUTPUT_SELECT to AI_EXTMUX_OUTPUT_SELECT_GROUND
 *      - AI_EXTERNAL_GATE_SELECT to AI_EXTERNAL_GATE_SELECT_DISABLED
 *      - AI_START_SELECT to AI_START_SELECT_PULSE
 *      - AI_START_POLARITY to AI_START_POLARITY_RISING_EDGE
 *      - AI_REFERENCE_SELECT to AI_REFERENCE_SELECT_PULSE
 *      - AI_REFERENCE_POLARITY to AI_REFERENCE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE
 *      - AI_STOP_SELECT to AI_STOP_SELECT_IN
 *      - AI_STOP_POLARITY to AI_STOP_POLARITY_ACTIVE_HIGH
 *      - AI_NO_OF_POST_TRIG_SAMPLES to 1
 *      - AI_NO_OF_PRE_TRIG_SAMPLES to 0
 *      - AI_SAMPLE_SELECT to AI_SAMPLE_SELECT_PULSE
 *      - AI_SAMPLE_POLARITY to AI_SAMPLE_POLARITY_RISING_EDGE
 *      - AI_SAMPLE_DELAY_DEVISOR to 3
 *      - AI_SAMPLE_PERIOD_DEVISOR to 20
 *      - AI_CONVERT_SELECT to AI_CONVERT_SELECT_SI2TC
 *      - AI_CONVERT_POLARITY to AI_CONVERT_POLARITY_FALLING_EDGE
 *      - AI_CONVERT_DELAY_DEVISOR to 3
 *      - AI_CONVERT_PERIOD_DEVISOR to 20
 *      .
 * It should be also noted there is no channels added.
 *
 * @return              A configuration structure.
 */
pxi6259_ai_conf_t pxi6259_create_ai_conf();

/**
 * Create an analog output segment configuration.
 *
 * Function creates the configuration structure. This configuration structure can then
 * be loaded to configure multiple cards. The same structure be used to
 * retrieve/set various attributes.
 *
 * Note that the following default values are set:
 *      - AO_SIGNAL_GENERATION to AO_SIGNAL_GENERATION_STATIC
 *      - AO_CONTINUOUS to 0
 *      - AO_NUMBER_OF_BUFFERS to 1
 *      - AO_NUMBER_OF_SAMPLES to 1
 *      .
 *
 * It should be also noted there is no channels added.
 *
 * @return              A configuration structure.
 */
pxi6259_ao_conf_t pxi6259_create_ao_conf();

/**
 * Create a digital input/output segment configuration.
 *
 * Function creates the configuration structure. This configuration structure can then
 * be loaded to configure multiple cards. The same structure can then be used to
 * retrieve/set various attributes.
 *
 * It should be also noted there is no ports added.
 *
 * @return              A configuration structure.
 */
pxi6259_dio_conf_t pxi6259_create_dio_conf();

/**
 * Create an general purpose counter segment configuration.
 *
 * Function creates the configuration structure. This configuration structure can then
 * be loaded to configure multiple cards. The same structure can then be used to
 * retrieve/set various attributes.
 *
 * Note that the following default values are set:
 *      - GPC_SOURCE_SELECT to GPC_SOURCE_SELECT_TIMEBASE1
 *      - GPC_COUNT_DIRECTION to GPC_COUNT_DIRECTION_SOFTWARE_DOWN
 *      - GPC_SELECT_TYPE to GPC_TASK_TYPE_SIMPLE_COUNTER
 *      .
 *
 * @return              A configuration structure.
 */
pxi6259_gpc_conf_t pxi6259_create_gpc_conf();

/**
 * Load a configuration.
 *
 * This function loads configuration into the NI PXI 6259 AI segment. When the
 * configuration is loaded the AI segment is put into CONFIGURED state.
 *
 * It is important to notice that you cannot load new configuration until all
 * the file descriptors using AI channels are closed. This is necessarily otherwise
 * the kernel will never be able to unload the module if the reference counter
 * doesn't drop to zero. If one ore more AI channel file descriptors are opened
 * this function returns -1 and errno is set to EBUSY.
 *
 * On error, -1 is returned, and errno is set appropriately.
 *
 * @param fd            The AI segment file descriptor.
 * @param conf          The configuration to load.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_load_ai_conf(int fd, pxi6259_ai_conf_t *conf);

/**
 * Load a configuration.
 *
 * This function loads configuration into the NI PXI 6259 AO segment. When the
 * configuration is loaded the AO segment is put into CONFIGURED state.
 *
 * It is important to notice that you cannot load new configuration until all
 * the file descriptors using AO channels are closed. This is necessarily otherwise
 * the kernel will never be able to unload the module if the reference counter
 * doesn't drop to zero. If one ore more AI channel file descriptors are opened
 * this function returns -1 and errno is set to EBUSY.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EBUSY
 *   Segment is busy.
 * - EINVAL
 *   Invalid configuration or configuration structure is not initialized.
 *
 * @param fd            The AO segment file descriptor.
 * @param conf          The configuration to load.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_load_ao_conf(int fd, pxi6259_ao_conf_t *conf);

/**
 * Load a configuration.
 *
 * This function loads configuration into the NI PXI 6259 DIO segment. When the
 * configuration is loaded the DIO segment is put into CONFIGURED state.
 *
 * It is important to notice that you cannot load new configuration until all
 * the file descriptors using DIO channels are closed. This is necessarily otherwise
 * the kernel will never be able to unload the module if the reference counter
 * doesn't drop to zero.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EBUSY
 *   Segment is busy.
 * - EINVAL
 *   Invalid configuration or configuration structure is not initialized.
 *
 * @param fd            The DIO segment file descriptor.
 * @param conf          The configuration to load.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_load_dio_conf(int fd, pxi6259_dio_conf_t *conf);

/**
 * Load a configuration.
 *
 * This function loads configuration into the NI PXI 6259 GPC0 segment. When the
 * configuration is loaded the GPC0 segment is put into CONFIGURED state.
 *
 * It is important to notice that you cannot load new configuration until all
 * the file descriptors using GPC channels are closed. This is necessarily otherwise
 * the kernel will never be able to unload the module if the reference counter
 * doesn't drop to zero. If one ore more AI channel file descriptors are opened
 * this function returns -1 and errno is set to EBUSY.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EBUSY
 *   Segment is busy.
 * - EINVAL
 *   Invalid configuration or configuration structure is not initialized.
 *
 * @param fd            The GPC0 segment file descriptor.
 * @param conf          The configuration to load.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_load_gpc0_conf(int fd, pxi6259_gpc_conf_t *conf);

/**
 * Load a configuration.
 *
 * This function loads configuration into the NI PXI 6259 GPC1 segment. When the
 * configuration is loaded the GPC1 segment is put into CONFIGURED state.
 *
 * It is important to notice that you cannot load new configuration until all
 * the file descriptors using GPC channels are closed. This is necessarily otherwise
 * the kernel will never be able to unload the module if the reference counter
 * doesn't drop to zero. If one ore more AI channel file descriptors are opened
 * this function returns -1 and errno is set to EBUSY.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EBUSY
 *   Segment is busy.
 * - EINVAL
 *   Invalid configuration or configuration structure is not initialized.
 *
 * @param fd            The GPC1 segment file descriptor.
 * @param conf          The configuration to load.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_load_gpc1_conf(int fd, pxi6259_gpc_conf_t *conf);

/**
 * Read a configuration.
 *
 * This function reads existing configuration from NI PXI 6259 AI segment.
 * The configuration structure can then be used to retrieve/set various
 * attributes.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid pointer.
 *
 * @param fd            The segment file descriptor.
 * @param conf          The configuration to read;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_read_ai_conf(int fd, pxi6259_ai_conf_t *conf);

/**
 * Read a configuration.
 *
 * This function reads existing configuration from NI PXI 6259 AO segment.
 * The configuration structure can then be used to retrieve/set various
 * attributes.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid pointer.
 *
 * @param fd            The segment file descriptor.
 * @param conf          The configuration to read;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_read_ao_conf(int fd, pxi6259_ao_conf_t *conf);

/**
 * Read a configuration.
 *
 * This function reads existing configuration from NI PXI 6259 DIO segment.
 * The configuration structure can then be used to retrieve/set various
 * attributes.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid pointer.
 *
 * @param fd            The segment file descriptor.
 * @param conf          The configuration to read;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_read_dio_conf(int fd, pxi6259_dio_conf_t *conf);

/**
 * Read a configuration.
 *
 * This function reads existing configuration from NI PXI 6259 GPC0 segment.
 * The configuration structure can then be used to retrieve/set various
 * attributes.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid pointer.
 *
 * @param fd            The segment file descriptor.
 * @param conf          The configuration to read;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_read_gpc0_conf(int fd, pxi6259_gpc_conf_t *conf);

/**
 * Read a configuration.
 *
 * This function reads existing configuration from NI PXI 6259 GPC1 segment.
 * The configuration structure can then be used to retrieve/set various
 * attributes.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid pointer.
 *
 * @param fd            The segment file descriptor.
 * @param conf          The configuration to read;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_read_gpc1_conf(int fd, pxi6259_gpc_conf_t *conf);


/**
 * Set configuration attribute.
 *
 * This helper function sets configuration attribute. Use this function to
 * define how the pxi6259 card will operate. Most of the available attributes
 * are preset when the configuration is created. Some attributes need to be
 * set by user. 
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid parameter.
 *
 * @param conf          The configuration to change.
 * @param attribute     Attribute identifier. Valid values:
 *                              - AI_NUMBER_OF_CHANNELS
 *                              - AI_NO_OF_PRE_TRIG_SAMPLES,
 *                              - AI_NO_OF_POST_TRIG_SAMPLES,
 *                              - AI_CONTINUOUS,
 *                              - AI_EXTERNAL_CONVERT_CLOCK,
 *                              - AI_SIP_OUTPUT_SELECT,
 *                              - AI_EXTERNAL_MUX_PRESENT,
 *                              - AI_EXTMUX_OUTPUT_SELECT,
 *                              - AI_EXTERNAL_GATE_SELECT,
 *                              - AI_START_SELECT,
 *                              - AI_START_POLARITY,
 *                              - AI_REFERENCE_SELECT,
 *                              - AI_REFERENCE_POLARITY,
 *                              - AI_STOP_SELECT
 *                              - AI_STOP_POLARITY
 *                              - AI_SAMPLE_SELECT,
 *                              - AI_SAMPLE_POLARITY,
 *                              - AI_SAMPLE_PERIOD_DEVISOR,
 *                              - AI_SAMPLE_DELAY_DEVISOR,
 *                              - AI_CONVERT_SELECT,
 *                              - AI_CONVERT_POLARITY,
 *                              - AI_CONVERT_PERIOD_DEVISOR,
 *                              - AI_CONVERT_DELAY_DEVISOR,
 *                              - AI_CONVERT_OUTPUT_SELECT,
 *                              - AI_CONVERT_PULSE_WIDTH,
 *                              - AI_SI_SOURCE_SELECT,
 *                              .
 * @param value         The attribute variable. Must be of the appropriate type:
 *                              - uint32_t (AI_NUMBER_OF_CHANNELS)
 *                              - uint32_t (AI_NO_OF_PRE_TRIG_SAMPLES)
 *                              - uint32_t (AI_NO_OF_POST_TRIG_SAMPLES)
 *                              - uint8_t (AI_CONTINUOUS)
 *                              - uint8_t (AI_EXTERNAL_CONVERT_CLOCK)
 *                              - ai_sip_output_select_t (AI_SIP_OUTPUT_SELECT)
 *                              - ai_external_mux_present_t (AI_EXTERNAL_MUX_PRESENT)
 *                              - ai_extmux_output_select_t (AI_EXTMUX_OUTPUT_SELECT)
 *                              - ai_external_gate_select_t (AI_EXTERNAL_GATE_SELECT)
 *                              - ai_start_select_t (AI_START_SELECT)
 *                              - ai_start_polarity_t (AI_START_POLARITY)
 *                              - ai_reference_select_t (AI_REFERENCE_SELECT)
 *                              - ai_reference_polarity_t (AI_REFERENCE_POLARITY)
 *                              - ai_stop_select_t (AI_STOP_SELECT)
 *                              - ai_stop_polarity_t (AI_STOP_POLARITY)
 *                              - ai_sample_select_t (AI_SAMPLE_SELECT)
 *                              - ai_sample_polarity_t (AI_SAMPLE_POLARITY)
 *                              - uint32_t (AI_SAMPLE_PERIOD_DEVISOR)
 *                              - uint32_t (AI_SAMPLE_DELAY_DEVISOR)
 *                              - ai_convert_source_select_t (AI_CONVERT_SELECT)
 *                              - ai_convert_source_polarity_t (AI_CONVERT_POLARITY)
 *                              - uint32_t (AI_CONVERT_PERIOD_DEVISOR)
 *                              - uint32_t (AI_CONVERT_DELAY_DEVISOR)
 *                              - ai_convert_output_select_t (AI_CONVERT_OUTPUT_SELECT)
 *                              - ai_convert_pulse_width_t (AI_CONVERT_PULSE_WIDTH)
 *                              .
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ai_attribute(pxi6259_ai_conf_t *conf,
                enum pxi6259_segment_attribute attribute, ...);

/**
 * Set configuration attribute.
 *
 * This helper function sets configuration attribute. Use this functions to
 * define how the pxi6259 card will operate.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid parameter.
 *
 * @param conf          The configuration to change.
 * @param attribute     Attribute identifier. Valid values:
 *                              - AO_CONTINUOUS,
 *                              - AO_SIGNAL_GENERATION,
 *                              - AO_START1_SOURCE_SELECT,
 *                              - AO_START1_POLARITY_SELECT,
 *                              - AO_UPDATE_SOURCE_SELECT,
 *                              - AO_UPDATE_POLARITY_SELECT,
 *                              - AO_UPDATE_PERIOD_DIVISOR,
 *                              - AO_UI_SOURCE_SELECT,
 *                              - AO_UI_SOURCE_POLARITY_SELECT,
 *                              - AO_NUMBER_OF_SAMPLES,
 *                              - AO_NUMBER_OF_BUFFERS,
 *                              - AO_FIFO_RETRANSMIT,
 *                              .
 * @param value         The attribute variable. Must be of the appropriate type:
 *                              - ao_continuous_t (AO_CONTINUOUS)
 *                              - ao_signal_generation_t (AO_SIGNAL_GENERATION)
 *                              - ao_start1_select_t (AO_START1_SOURCE_SELECT)
 *                              - ao_start1_polarity_t (AO_START1_POLARITY_SELECT)
 *                              - ao_update_source_select_t (AO_UPDATE_SOURCE_SELECT)
 *                              - ao_update_source_polarity_t (AO_UPDATE_POLARITY_SELECT)
 *                              - uint32_t (AO_UPDATE_PERIOD_DIVISOR)
 *                              - ao_ui_source_select_t (AO_UI_SOURCE_SELECT)
 *                              - ao_ui_source_polarity_t (AO_UI_SOURCE_POLARITY_SELECT)
 *                              - uint32_t (AO_NUMBER_OF_SAMPLES)
 *                              - uint32_t (AO_NUMBER_OF_BUFFERS)
 *                              - ao_fifo_retransmit_t (AO_FIFO_RETRANSMIT)
 *                              .
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ao_attribute(pxi6259_ao_conf_t *conf,
                enum pxi6259_segment_attribute attribute, ...);

/**
 * Set configuration attribute.
 *
 * This helper function sets configuration attribute. Use this functions to
 * define how the pxi6259 card will operate.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid parameter.
 *
 * @param conf          The configuration to change.
 * @param attribute     Attribute identifier. Valid values:
 *                              - GPC_INITIAL_VALUE,
 *                              - GPC_SOURCE_SELECT,
 *                              - GPC_GATE_SELECT,
 *                              - GPC_COUNT_DIRECTION,
 *                              - GPC_SELECT_TYPE,
 *                              .
 * @param value         The attribute variable. Must be of the appropriate type:
 *                              - uint32_t (GPC_INITIAL_VALUE)
 *                              - gpc_source_select_t (GPC_SOURCE_SELECT)
 *                              - gpc_gate_select_t (GPC_GATE_SELECT)
 *                              - gpc_count_direction_t (GPC_COUNT_DIRECTION)
 *                              - gpc_task_type_t (GPC_SELECT_TYPE)
 *                              .
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_gpc_attribute(pxi6259_gpc_conf_t *conf,
                enum pxi6259_segment_attribute attribute, ...);

/**
 * Get AI attribute
 *
 * Function returns the selected attribute from configuration.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf          The configuration to change.
 * @param attribute     Select the attribute to return. Allowed values:
 *                              - AI_NUMBER_OF_CHANNELS
 *                              - AI_NO_OF_PRE_TRIG_SAMPLES,
 *                              - AI_NO_OF_POST_TRIG_SAMPLES,
 *                              - AI_CONTINUOUS,
 *                              - AI_EXTERNAL_CONVERT_CLOCK,
 *                              - AI_SIP_OUTPUT_SELECT,
 *                              - AI_EXTERNAL_MUX_PRESENT,
 *                              - AI_EXTMUX_OUTPUT_SELECT,
 *                              - AI_EXTERNAL_GATE_SELECT,
 *                              - AI_START_SELECT,
 *                              - AI_START_POLARITY,
 *                              - AI_REFERENCE_SELECT,
 *                              - AI_REFERENCE_POLARITY,
 *                              - AI_STOP_SELECT
 *                              - AI_STOP_POLARITY
 *                              - AI_SAMPLE_SELECT,
 *                              - AI_SAMPLE_POLARITY,
 *                              - AI_SAMPLE_PERIOD_DEVISOR,
 *                              - AI_SAMPLE_DELAY_DEVISOR,
 *                              - AI_CONVERT_SELECT,
 *                              - AI_CONVERT_POLARITY,
 *                              - AI_CONVERT_PERIOD_DEVISOR,
 *                              - AI_CONVERT_DELAY_DEVISOR,
 *                              - AI_CONVERT_OUTPUT_SELECT,
 *                              - AI_CONVERT_PULSE_WIDTH,
 *                              - AI_SI_SOURCE_SELECT,
 *                              .
 *
 * @return                      0 if function succeeds otherwise error code.
 */
__must_check int pxi6259_get_ai_attribute(pxi6259_ai_conf_t *conf,
                enum pxi6259_segment_attribute attribute, ...);

/**
 * Configure number of samples.
 *
 * This helper function sets the number of samples and updates the configuration
 * structure. This function can be used instead of pxi6259_set_segment_attribute
 * function.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param post_trigger_samples  The number of post-trigger samples.
 * @param pre_trigger_samples   The number of pre-trigger samples.
 * @param continuous            If >0 continuous otherwise not.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ai_number_of_samples(pxi6259_ai_conf_t *conf,
                uint32_t post_trigger_samples, uint32_t pre_trigger_samples, uint8_t continuous);

/**
 * Configure convert clock.
 *
 * This helper function sets the convert clock and updates the configuration
 * structure. This function can be used instead of pxi6259_set_segment_attribute
 * function.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param period_divisor        Period divisor. This value is taken into account only
 *                              when the AI_CONVERT_SELECT_SI2TC source is selected.
 *                              Note that minimum convert divisor fur single channel is 16
 *                              and for multiple channels 20.
 * @param delay_divisor         Delay divisor. This value is taken into account only
 *                              when the AI_CONVERT_SELECT_SI2TC source is selected.
 *                              Note that minimum delay divisor is 3.
 * @param source		Source of the convert clock. Valid values:
 *				- AI_CONVERT_SELECT_SI2TC The internal signal SI2_TC, inverted.
 *				- AI_CONVERT_SELECT_PFI<0..9> PFI<0..9>
 *				- AI_CONVERT_SELECT_RTSI<0..17> RTSI_TRIGGER<0..17>
 *				- AI_CONVERT_SELECT_GPCRT1_OUT The internal signal GOUT from
 *					general-purpose counter 1.
 *				- AI_CONVERT_SELECT_GPCRT0_OUT The internal signal GOUT from
 *					general-purpose counter 0.
 *				- AI_CONVERT_SELECT_STAR_TRIGGER Star Trigger.
 *				- AI_CONVERT_SELECT_ANALOG_TRIGGER Analog Trigger.
 *				- AI_CONVERT_SELECT_LOW Logic Low.
 *				.
 * @param polarity              Select the trigger event of convert clock signal. Valid values:
 *                              - AI_CONVERT_POLARITY_FALLING_EDGE Trigger on falling edge of signal.
 *                              - AI_CONVERT_POLARITY_RISING_EDGE Trigger on rising edge of signal.
 *                              .
 *
 *                              When you set the convert source to AI_CONVERT_SELECT_SI2TC,
 *                              the PXI6259 card is in the internal CONVERT mode. When you select
 *                              any other signal as the CONVERT source, the PXI6259 card is in
 *                              the external CONVERT mode.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ai_convert_clk(pxi6259_ai_conf_t *conf, uint32_t period_divisor,
                uint32_t delay_divisor, ai_convert_select_t source, ai_convert_polarity_t polarity);

/**
 * Configure sample clock.
 *
 * This helper function sets the sample clock and updates the configuration
 * structure. This function can be used instead of pxi6259_set_segment_attribute
 * function.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param period_divisor        Period divisor. This value is taken into account only
 *                              when the AI_SAMPLE_SELECT_SI_TC source is selected.
 * @param delay_divisor         Delay divisor. This value is taken into account only
 *                              when the AI_SAMPLE_SELECT_SI_TC source is selected.
 * @param source                Source of the sample clock. Valid values:
 *                               - AI_SAMPLE_SELECT_SI_TC The internal signal SI_TC.
 *                               - AI_SAMPLE_SELECT_PFI<0..15> PFI<0..9>
 *                               - AI_SAMPLE_SELECT_RTSI<0..17> RTSI_TRIGGER<0..17>
 *                               - AI_SAMPLE_SELECT_PULSE AI_START_Pulse which can be generated
 *                               with software.
 *                               - AI_SAMPLE_SELECT_GPCRT0_OUT The internal signal GOUT from
 *                               general-purpose counter 0.
 *                               - AI_SAMPLE_SELECT_GPCTR1_OUT The internal signal GOUT from
 *                               general-purpose counter 1.
 *                               - AI_SAMPLE_SELECT_STAR_TRIGGER Star Trigger.
 *                               - AI_SAMPLE_SELECT_ANALOG_TRIGGER Analog Trigger.
 *                               - AI_SAMPLE_SELECT_LOW Logic Low.
 *                               - AI_SAMPLE_SELECT_SCXI_TRIG1
 *                               .
 * @param polarity              Select the trigger event of sample clock signal. Valid values:
 *                              - AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE
 *                              - AI_SAMPLE_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE
 *                              - AI_SAMPLE_POLARITY_ACTIVE_HIGH
 *                              - AI_SAMPLE_POLARITY_ACTIVE_LOW
 *                              - AI_SAMPLE_POLARITY_RISING_EDGE
 *                              - AI_SAMPLE_POLARITY_FALLING_EDGE
 *                              .
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ai_sample_clk(pxi6259_ai_conf_t *conf, uint32_t period_divisor,
                uint32_t delay_divisor, ai_sample_select_t source, ai_sample_polarity_t polarity);

/**
 * Generate AI pulses
 *
 * Function generates pulses to trigger AI acquisition. Sample clock must be
 * set to AI_SAMPLE_SELECT_PULSE. If this is not true, error is reported.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param fd            The segment file descriptor.
 * @param num_pulses    Number of pulses to generate.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_generate_ai_pulse(int fd, uint32_t num_pulses);

/**
 * Configure analog output samples setup.
 *
 * This helper function sets number of samples and buffers values and determines if 
 * AO signal generation is continuous.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param number_of_samples     Number of samples to generate.
 * @param number_of_buffers     Number of buffers.
 * @param continuous            Continuous. Sets the type of generation.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ao_count(pxi6259_ao_conf_t *conf, uint32_t number_of_samples,
                uint32_t number_of_buffers, uint8_t continuous);

/**
 * Configure analog output update clock.
 *
 * This helper function sets update clock settings to user defined values.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param source                Update source. Valid values are:
 *                              - AO_UPDATE_SOURCE_SELECT_UI_TC
 *                              - AO_UPDATE_SOURCE_SELECT_PFI<0..15>
 *                              - AO_UPDATE_SOURCE_SELECT_RTSI<0..7>
 *                              - AO_UPDATE_SOURCE_SELECT_GPCTR0_Out
 *                              - AO_UPDATE_SOURCE_SELECT_GPCTR1_Out
 *                              - AO_UPDATE_SOURCE_SELECT_STAR_TRIGEGR
 *                              - AO_UPDATE_SOURCE_SELECT_ANALOG_TRIGGER
 *                              - AO_UPDATE_SOURCE_SELECT_LOW
 *                              .
 * @param polarity              Update polarity. Valid values:
 *                              - AO_UPDATE_SOURCE_POLARITY_RISING_EDGE
 *                              - AO_UPDATE_SOURCE_POLARITY_FALLING_EDGE
 *                              .
 * @param period_devisor        Period divisor. Value is used only when internal
 *                              pulse generation is used.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ao_update_clk(pxi6259_ao_conf_t *conf, ao_update_source_select_t source,
                ao_update_source_polarity_t polarity, uint32_t period_devisor);

/**
 * Configure analog output waveform settings.
 *
 * This helper function sets waveform settings to default values and
 * retransmit mode to user defined value.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param retransmit            Enable or disable retransmit mode.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_ao_waveform_generation(pxi6259_ao_conf_t *conf, uint8_t retransmit);

/**
 * Configure frequency of general purpose counter pulse generation.
 *
 * This helper function sets the selected general purpose counter so its output generates pulses
 * in user defined frequency.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  The configuration to change.
 * @param frequency             Frequency of the GPC output.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_set_gpc_trigger_frequency(pxi6259_gpc_conf_t *conf, uint32_t frequency);

/******************************************************/
/***        Segment Manipulation                    ***/
/******************************************************/

/**
 * Start an AI segment.
 *
 * This function transitions the segment state to the STARTED state, which begins
 * measurement of AI channels.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to start acquisition.
 *
 * @param fd            The AI segment file descriptor;
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_start_ai(int fd);

/**
 * Start an AO segment.
 *
 * This function transitions the segment state to the STARTED state, which enables
 * analog signal generation.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to start generation.
 *
 * @param fd            The AO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_start_ao(int fd);

/**
 * Start a DIO segment.
 *
 * This function transitions the segment state to the STARTED state, which enables
 * digital measurement or generation.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to start segment.
 *
 * @param fd            The DIO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_start_dio(int fd);

/**
 * Start a GPC0 segment.
 *
 * This function transitions the segment state to the STARTED state, which begins
 * counting the selected source signal pulses.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to start segment.
 *
 * @param fd            THE GPC0 segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_start_gpc0(int fd);

/**
 * Start a GPC1 segment.
 *
 * This function transitions the segment state to the STARTED state, which begins
 * counting the selected source signal pulses.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to start segment.
 *
 * @param fd            The GPC0 segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_start_gpc1(int fd);


/**
 * Stop AI segment.
 *
 * This function transitions the segment state to the CONFIGURED state, After this
 * the segment can be started again.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to stop segment.
 *
 * @param fd            The AI segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_stop_ai(int fd);

/**
 * Stop AO segment.
 *
 * This function transitions the segment state to the CONFIGURED state, after this
 * the segment can be started again.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to stop segment.
 *
 * @param fd            The AO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_stop_ao(int fd);

/**
 * Stop DIO segment.
 *
 * This function transitions the segment state to the CONFIGURED state, after this
 * the segment can be started again.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to stop segment.
 *
 * @param fd            The DIO segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_stop_dio(int fd);

/**
 * Stop GPC0 segment.
 *
 * This function transitions the segment state to the CONFIGURED state, After this
 * the segment can be started again.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to stop segment.
 *
 * @param fd            The GPC0 segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_stop_gpc0(int fd);

/**
 * Stop GPC1 segment.
 *
 * This function transitions the segment state to the CONFIGURED state, After this
 * the segment can be started again.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Segment not configured.
 * - EIO
 *   Failed to stop segment.
 *
 * @param fd            The GPC1 segment file descriptor.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_stop_gpc1(int fd);

/******************************************************/
/***        Channel Configuration/Creation          ***/
/******************************************************/

/**
 * Add channel to AI configuration.
 *
 * This function adds channel to the configuration. Number of available channels is
 * determined by type of channels used. Differential channels use two channels at
 * once (e.g. channels 3 and 11). If user adds a channel that is already used 
 * error is reported. 
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  Configuration of the segment.
 * @param channel_number        The name of the physical channel. Valid values:
 *  				        - 0..31
 * 				        .
 * @param polarity              Polarity of the adding channel. Valid values:
 *                                      - AI_POLARITY_BIPOLAR
 *                                      - AI_POLARITY_UNIPOLAR
 *                                      .
 * @param gain                  Gain of the adding channel. Valid values:
 *                                      - 1: +/- 10V
 *                                      - 2: +/- 5V
 *                                      - 3: +/- 2V
 *                                      - 4: +/- 1V
 *                                      - 5: +/- 500mV
 *                                      - 6: +/- 200mV
 *                                      - 7: +/- 100mV
 *                                      .
 * @param channel_type          Select the type of the channel:
 *                                      - AI_CHANNEL_TYPE_CALIBRATION
 *                                      - AI_CHANNEL_TYPE_DIFFERENTIAL
 *                                      - AI_CHANNEL_TYPE_NRSE
 *                                      - AI_CHANNEL_TYPE_RSE
 *                                      - AI_CHANNEL_TYPE_AUX
 *                                      - AI_CHANNEL_TYPE_GHOST
 *                                      .
 * @param dither                Select dither value. Valid values:
 *                                      - 0
 *                                      - 1
 *                                      .
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_add_ai_channel(pxi6259_ai_conf_t *conf, uint8_t channel_number,
                ai_polarity_t polarity, uint8_t gain, ai_channel_type_t channel_type, uint8_t dither);

/**
 * Remove channel from AI configuration.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  Configuration of the segment.
 * @param channel_number        Number of the physical channel. Valid values:
 * 				        - 0..31
 * 				        .
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_remove_ai_channel(pxi6259_ai_conf_t *conf, uint8_t channel_number);

/**
 * Add channel to AO configuration.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  Configuration of the segment.
 * @param channel_number        The name of the physical channel. Valid values:
 *                                      - 0..3
 *                                      .
 * @param polarity              Polarity of the channel. Legal values:
 *                                      - AO_DAC_POLARITY_UNIPOLAR
 *                                      - AO_DAC_POLARITY_BIPOLAR
 *                                      .
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_add_ao_channel(pxi6259_ao_conf_t *conf,
                uint8_t channel_number, ao_dac_polarity_t polarity);

/**
 * Remove channel from AO configuration.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  Configuration of the segment.
 * @param channel_number        Number of the physical channel. Valid values:
 *                                      - 0..3
 *                                      .
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_remove_ao_channel(pxi6259_ao_conf_t *conf, uint8_t channel_number);

/**
 * Add channel to DIO configuration.
 *
 * Adds a port to configuration and determines which terminals are inputs (mask 
 * value 0) and which are outputs (mask value 1).
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf          Configuration of the segment.
 * @param port_number   The port number. Valid values
 *                              - 0..2
 *                              .
 * @param port_mask     Mask which select which pins are inputs and which outputs.
 *
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_add_dio_channel(pxi6259_dio_conf_t *conf,
                uint8_t port_number, uint32_t port_mask);

/**
 * Remove channel from DIO configuration.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf                  Configuration of the segment.
 * @param port_number           Number of the physical port. Valid values:
 *                                      - 0..2
 *                                      .
 * @return              0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_remove_dio_channel(pxi6259_dio_conf_t *conf, uint8_t port_number);


/******************************************************/
/***         Triggering    & Timing                 ***/
/******************************************************/

/**
 * Connect terminals.
 *
 * This function routes source terminal (phisical input) to the
 * destination terminal (phisical output) or vice verse.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param fd                    segment file descriptor.
 * @param src_terminal          Source terminal.
 * @param dest_terminal         Destination terminal.
 * @param signal_modifiers	
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_connect_trigger_terminals(int fd,
                int src_terminal, int dest_terminal, int32_t signal_modifiers);

/**
 * Disconnect terminals.
 *
 * This function disconnects source terminal (phisical input) from the
 * destination terminal (phisical output) or vice verse.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param fd                    segment file descriptor.
 * @param src_terminal          Source terminal.
 * @param dest_terminal         Destination terminal.
 *
 * @return                      0 if function succeeds otherwise -1.
 */
__must_check int pxi6259_disconnect_trigger_terminals(int fd,
                int src_terminal, int dest_terminal);


/**
 * Routes a control signal to the specified terminal or destination signal.
 *
 * The output terminal can reside on the device that generates the control
 * signal or on a different device. Use this function to share clocks and
 * triggers between multiple tasks, segments and devices.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf          Configuration of the segment.
 * @param signal     	the name of the trigger, clock, or event to export
 * 				- PXI6259_AI_SAMPLE_CLOCK
 *				- PXI6259_AI_CONVERT_CLOCK
 *				- PXI6259_AI_START_TRIGGER
 *				- PXI6259_AI_REFERENCE_TRIGGER
 *				.
 * @param dest_terminal the destination terminal of the exported signal
 * 				- PXI6259_RTSI_TERMINAL
 *				- PXI6259_RTSI<0..7>
 * 				- PXI6259_PFI_TERMINAL
 *				- PXI6259_PFI<0..15>
 *				.
 *
 * @return              0 if function succeeds otherwise -1
 */
__must_check int pxi6259_export_ai_signal(pxi6259_ai_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal);

/**
 * Routes a control signal to the specified terminal or destination signal.
 *
 * The output terminal can reside on the device that generates the control
 * signal or on a different device. Use this function to share clocks and
 * triggers between multiple tasks, segments and devices.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf          Configuration of the segment.
 * @param signal        the name of the trigger, clock, or event to export
 *                              - PXI6259_AO_SAMPLE_CLOCK
 *                              - PXI6259_AO_START_TRIGGER
 *                              .
 * @param dest_terminal the destination terminal of the exported signal
 *                              - PXI6259_RTSI_TERMINAL
 *                              - PXI6259_RTSI<0..7>
 *                              - PXI6259_PFI_TERMINAL
 *                              - PXI6259_PFI<0..15>
 *                              .
 *
 * @return              0 if function succeeds otherwise -1
 */
__must_check int pxi6259_export_ao_signal(pxi6259_ao_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal);

/**
 * Routes a control signal to the specified terminal or destination signal.
 *
 * The output terminal can reside on the device that generates the control
 * signal or on a different device. Use this function to share clocks and
 * triggers between multiple tasks, segments and devices.
 *
 * On error, -1 is returned, and errno may be one of the following:
 * - EINVAL
 *   Invalid argument.
 *
 * @param conf          Configuration of the segment.
 * @param signal        the name of the trigger, clock, or event to export
 *                              - PXI6259_GPC0_OUTPUT
 *                              - PXI6259_GPC1_OUTPUT
 *                              .
 * @param dest_terminal the destination terminal of the exported signal
 *                              - PXI6259_RTSI_TERMINAL
 *                              - PXI6259_RTSI<0..7>
 *                              - PXI6259_PFI_TERMINAL
 *                              - PXI6259_PFI<0..15>
 *                              .
 *
 * @return              0 if function succeeds otherwise -1
 */
__must_check int pxi6259_export_gpc_signal(pxi6259_gpc_conf_t *conf,
                enum pxi6259_signal signal, enum pxi6259_terminal dest_terminal);


/******************************************************/
/***                 Read & Write Data              ***/
/******************************************************/

/**
 * Read scaled samples.
 *
 * This function attempts to read up to num_samples samples from file descriptor chanfd
 * into the buffer starting at buf. If the num_samples are zero this function returns 0
 * and no other value. Sample is float value and represents voltage.
 * On success, the number of samples read is returned. It is not an error if this number
 * is smaller than the number of samples requested; this may happen for example because
 * fewer samples are actually available right now, or because read() which is using by
 * this function was interrupted by a signal. On error, -1 is returned, and errno is set
 * appropriately. This function has the similar semantics as read() function does.
 * It should be noted that this function also acquires scaling coefficients before attempts
 * to read any samples.
 *
 * Valid errno values:
 * - EOVERFLOW errno is set when the DMA buffer is too small for the configured sample rate
 *   and application parameters (delay between reads, number of samples to be read, optionally
 *   process priority). This is error condition which means some samples were lost.
 *   Read position has been reset to current sample automatically when this error occurs.
 *   This effectively means that next read will return samples starting at a time the error
 *   occured.
 *
 * @param chanfd                Analog input channel file descriptor.
 * @param buffer_scaled         Array of read values (volts).
 * @param num_samples           Number of samples to read.
 *
 * @return                      Number of samples read if function succeeds otherwise -1.
 */
ssize_t pxi6259_read_ai(int chanfd, float *buffer_scaled, size_t num_samples);

/**
 * Read exact number of scaled samples.
 *
 * This function behaves similar to pxi6259_read_ai() but will block until it can return
 * all of the samples requested.
 * On success, the number of samples read is returned, this will always be the same as number
 * of samples requested. On error, -1 is returned and errno is set appropriately.
 * It should be noted that this function also acquires scaling coefficients before attempts
 * to read any samples.
 *
 * Function accepts two timing parameters:
 *
 * - timeout:   specifies the maximum time in ms that a function can block for. 0 means
 *              block forever (no timeout). On timeout a -1 is returned and errno is set to
 *              ETIMEDOUT
 *
 * - read_delay:specifies the minimum time in us the function will wait between two consecutive
 *              reads. Small values mean that the function will induce a very small propagation
 *              delay from when data is available to when function returns at the cost of higher
 *              CPU load (since function will transfer small number of samples very often). Larger
 *              values will reduce CPU load but also introduce a bit of a propagation delay (a
 *              maximum of read_delay)
 *              An acceptable default value is 100-1000;
 *
 * Valid errno values:
 * - EOVERFLOW errno is set when the DMA buffer is too small for the configured sample rate
 *   and application parameters (delay between reads, number of samples to be read, optionally
 *   process priority). This is error condition which means some samples were lost.
 *   Read position has been reset to current sample automatically when this error occurs.
 *   This effectively means that next read will return samples starting at a time the error
 *   occured.
 *
 * - ETIMEDOUT errno is set if timeout occurs before all of the samples have been read. If timeout parameter is
 *   set to 0 ETIMEDOUT is never set.
 *
 * @param chanfd                Analog input channel file descriptor.
 * @param buffer_scaled         Array of read values (volts).
 * @param num_samples           Number of samples to read.
 * @param timeout               A maximum time in [ms] that a function will wait for samples
 * @param read_delay            Minimum time in [us] that a function will wait between two consecutive reads
 *                              (acceptable default value is 100-1000)
 *
 * @return                      Number of samples read if function succeeds otherwise -1. *
 */
ssize_t pxi6259_read_ai_exact(int chanfd, float *buffer_scaled, size_t num_samples, uint32_t timeout, uint32_t read_delay);

/**
 * Write scaled sample to the output line.
 *
 * This function writes up to num_samples samples to the destination channel referenced by the
 * file descriptor chanfd from the buffer starting at buf. If the num_samples are zero this
 * function returns 0 and no other value.
 * On success, the number of samples written are returned (zero indicates nothing was written).
 * It is not an error if this number is smaller than the number of samples requested; this may
 * happen for example because the internal buffer is not free right now, or because write() which
 * is used by this function was interrupted by a signal. On error, -1 is returned, and errno is
 * set appropriately.
 *
 * @param chanfd                Analog output channel file descriptor.
 * @param buf                   Array of write values (volts).
 * @param num_samples           Number of samples to write.
 *
 * @return                      Number of samples written if function succeeds otherwise -1.
 */
ssize_t pxi6259_write_ao(int chanfd, float* buf, size_t num_samples);


#ifdef __cplusplus
} /* closing brace for "C" */
#endif

#endif /* PXI6259LIB_H_ */
