/*****************************************************************************
 * pxi6259-enums.h
 *
 * Common enumerations for NI PXI-6259 driver and its library.
 * Author: Janez Golob (Cosylab)
 * Copyright (C) 2010-2013 ITER Organization
 ****************************************************************************/

/**
 * @file
 * This file contains almost all important enums,
 * that are used in driver and library and can also be used in user programs.
 */

#ifndef PXI6259ENUMS_H_
#define PXI6259ENUMS_H_


/**
 * This bit selects the reload mode for the SC counter:
 *      - 0: No automatic change of the SC load register.
 *      - 1: The SC counter will switch load registers on every SC_TC.
 *      .
 * You can use setting 1 for pretrigger acquisition mode and for staged analog input.
 */
typedef enum {
        AI_SC_RELOAD_MODE_NO_CHANGE         = 0,
        AI_SC_RELOAD_MODE_SWITCH            = 1,
} ai_sc_reload_mode_t;


/**
 * If the SC counter is disarmed, this bit selects the initial SC load register:
 *      - 0: Load register A.
 *      - 1: Load register B.
 *      .
 * If the SC counter is armed, this bit has no effect.
 */
typedef enum {
        AI_SC_INITIAL_LOAD_SOURCE_LOAD_A            = 0,
        AI_SC_INITIAL_LOAD_SOURCE_LOAD_B            = 1,
} ai_sc_initial_load_source_t;


/**
 * This bitfield selects the reload mode for the SI counter:
 *      - 0:    No automatic change of the SI load register.
 *      - 4:    Alternate first period on every STOP. Use this setting to make the time interval
 *              between the START trigger and the first sample pulse different from the remaining
 *              sample intervals.
 *      - 5:    Switch load register on every STOP. Use this setting to synchronously change the
 *              sample interval at each STOP.
 *      - 6:    Alternate first period on every SC_TC. Use this setting to make the interval between
 *              the START1 trigger and the first scan different from the scan interval.
 *      - 7:    Switch load register on every SC_TC. Use this setting to synchronously change the
 *              scan interval at each SC_TC.
 *      .
 */
typedef enum {
        AI_SI_RELOAD_MODE_NO_CHANGE         = 0,
        AI_SI_RELOAD_MODE_ALTERNATE_FIRST_PERIOD_EVERY_STOP                   = 4,
        AI_SI_RELOAD_MODE_SWITCH_EVERY_STOP   = 5,
        AI_SI_RELOAD_MODE_ALTERNATE_FIRST_PERIOD_EVERY_SCTC                   = 6,
        AI_SI_RELOAD_MODE_SWITCH_EVERY_SCTC   = 7,
} ai_si_reload_mode_t;


/**
 * If the SI counter is disarmed, this bit selects the initial SI load register:
 *      - 0: Load register A.
 *      - 1: Load register B.
 *      .
 * If the SI counter is armed, writing to this bit has no effect.
 */
typedef enum {
        AI_SI_INITIAL_LOAD_SOURCE_LOAD_A            = 0,
        AI_SI_INITIAL_LOAD_SOURCE_LOAD_B            = 1,
} ai_si_initial_load_source_t;


/**
 * This bit selects the active edge of the SI source (the signal that is selected by
 * AI_SI_Source_Select):
 *      - 0: Rising edge.
 *      - 1: Falling edge.
 *      .
 * Set this bit to 0 if an internal timebase is used.
 */
typedef enum {
        AI_SI_SOURCE_POLARITY_RISING_EDGE       = 0,
        AI_SI_SOURCE_POLARITY_FALLING_EDGE      = 1,
} ai_si_source_polarity_t;


/**
 * This bit selects the reload mode for the SI2 counter:
 *      - 0: No automatic change of the SI2 load register.
 *      - 1: Alternate first period on every STOP.
 *      .
 * Set this bit to 1 in the internal CONVERT mode to make the time interval between the
 * START trigger and the first CONVERT different from the time interval between
 * CONVERTs.
 */
typedef enum {
        AI_SI2_RELOAD_MODE_NO_CHANGE         = 0,
        AI_SI2_RELOAD_MODE_ALTERNATE_FIRST_PERIOD_EVERY_STOP                   = 1,
} ai_si2_reload_mode_t;


/**
 * This bit selects the initial SI2 load register:
 *      - 0: Load register A.
 *      - 1: Load register B.
 *      .
 * Do not change this bit while the counter is counting.
 */
typedef enum {
        AI_SI2_INITIAL_LOAD_SOURCE_LOAD_A            = 0,
        AI_SI2_INITIAL_LOAD_SOURCE_LOAD_B            = 1,
} ai_si2_initial_load_source_t;


/**
 * This bit selects the SI2 source:
 *      - 0: The same signal selected as the SI source. Refer to AI_SI_Source_Select.
 *      - 1: AI_IN_TIMEBASE1.
 *      .
 */
typedef enum {
        AI_SI2_SELECT_SAME_AS_SI        = 0,
        AI_SI2_SELECT_INTIMEBASE1       = 1,
} ai_si2_select_t;


/**
 * This bit determines when the LOCALMUX_CLK output signal pulses:
 *      - 0: Pulse on every CONVERT.
 *      - 1: Pulse only on CONVERTs that occur during DIV_TC.
 *      .
 * This bit allows you to use the DIV counter for LOCALMUX_CLK signal control. This is
 * useful if one or more external multiplexers, such as an AMUX-64T or SCXI, are connected
 * to the board the PXI6259 card is on. You should set this bit to 0 if no external multiplexers are
 * present or if each external channel corresponds to one internal channel. You should set this
 * bit to 1 if one or more external multiplexers are present and if you are multiplexing more than
 * one external channel onto each internal channel. If this bit is set to 1, the DIV counter must
 * be used to determine the number of EXTMUX_CLK pulses that will correspond to one
 * LOCALMUX_CLK pulse.
 */
typedef enum {
        AI_EXTERNAL_MUX_PRESENT_EVERY_CONVERT     = 0,
        AI_EXTERNAL_MUX_PRESENT_DIV_TC_CONVERTS   = 1,
} ai_external_mux_present_t;


/**
 *This bitfield enables and selects the external gate:
 *      - 0: External gate disabled.
 *      - 1–10: PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 31: Logic low.
 *      .
 * You can use the external gate to pause an analog input operation in progress.
 */
typedef enum {
        AI_EXTERNAL_GATE_SELECT_DISABLED          = 0,
        AI_EXTERNAL_GATE_SELECT_PFI0              = 1,
        AI_EXTERNAL_GATE_SELECT_PFI1              = 2,
        AI_EXTERNAL_GATE_SELECT_PFI2              = 3,
        AI_EXTERNAL_GATE_SELECT_PFI3              = 4,
        AI_EXTERNAL_GATE_SELECT_PFI4              = 5,
        AI_EXTERNAL_GATE_SELECT_PFI5              = 6,
        AI_EXTERNAL_GATE_SELECT_PFI6              = 7,
        AI_EXTERNAL_GATE_SELECT_PFI7              = 8,
        AI_EXTERNAL_GATE_SELECT_PFI8              = 9,
        AI_EXTERNAL_GATE_SELECT_PFI9              = 10,
        AI_EXTERNAL_GATE_SELECT_RTSI0             = 11,
        AI_EXTERNAL_GATE_SELECT_RTSI1             = 12,
        AI_EXTERNAL_GATE_SELECT_RTSI2             = 13,
        AI_EXTERNAL_GATE_SELECT_RTSI3             = 14,
        AI_EXTERNAL_GATE_SELECT_RTSI4             = 15,
        AI_EXTERNAL_GATE_SELECT_RTSI5             = 16,
        AI_EXTERNAL_GATE_SELECT_RTSI6             = 17,
        AI_EXTERNAL_GATE_SELECT_STAR_TRIGGER      = 20,
        AI_EXTERNAL_GATE_SELECT_PFI10             = 21,
        AI_EXTERNAL_GATE_SELECT_PFI11             = 22,
        AI_EXTERNAL_GATE_SELECT_PFI12             = 23,
        AI_EXTERNAL_GATE_SELECT_PFI13             = 24,
        AI_EXTERNAL_GATE_SELECT_PFI14             = 25,
        AI_EXTERNAL_GATE_SELECT_PFI15             = 26,
        AI_EXTERNAL_GATE_SELECT_RTSI7             = 27,
        AI_EXTERNAL_GATE_SELECT_ANALOG_TRIGGER    = 30,
        AI_EXTERNAL_GATE_SELECT_LOW               = 31,
} ai_external_gate_select_t;


/**
 * This bit selects the polarity of the external gate signal:
 *      - 0: Active high (high enables operation).
 *      - 1: Active low (low enables operation).
 *      .
 */
typedef enum {
        AI_EXTERNAL_GATE_POLARITY_ACTIVE_HIGH       = 0,
        AI_EXTERNAL_GATE_POLARITY_ACTIVE_LOW        = 1,
} ai_external_gate_polarity_t;


/**
 * This bit selects the AI data FIFO condition on which to generate the DMA request (output
 * signal AIFREQ) and the FIFO interrupt (if the FIFO interrupt is enabled):
 *      - 0: Generate DMA request and FIFO interrupt on FIFO not empty. Keep the request and
 *           interrupt asserted while the FIFO is not empty.
 *      - 1: Generate DMA request and FIFO interrupt on FIFO more than half-full. Keep the
 *           request and interrupt asserted while the FIFO is half-full.
 *      - 2: Generate DMA request and FIFO interrupt on FIFO full. Keep the request and
 *           interrupt asserted while the FIFO is full.
 *      - 3: Generate DMA request and FIFO interrupt on FIFO more than half-full. Keep the
 *           request and interrupt asserted while the FIFO is not empty.
 *      .
 * Selection 3 will cause the request and FIFO interrupt to assert at the end of the acquisition and
 * remain asserted until the FIFO empties, provided that SHIFTIN arrives after the trailing edge
 * of the last SC_TC. The SHIFTIN signal may arrive before the trailing edge of the last SC_TC
 * if an internal CONVERT is used and the SI2 clock is slow with respect to the ADC interval.
 * In this case, you should use the SC_TC interrupt to initiate the final FIFO read at the end at
 * the acquisition.
 */
typedef enum {
        AI_FIFO_MODE_NOT_EMPTY                  = 0,
        AI_FIFO_MODE_HALF_FULL                  = 1,
        AI_FIFO_MODE_FULL                       = 2,
        AI_FIFO_MODE_HALF_FULL_UNTIL_EMPTY      = 3,
} ai_fifo_mode_t;


/**
 * This bit determines the gating mode, if gating is enabled.
 *      - 0: Free-run gating mode.
 *      - 1: Halt-gating mode.
 *      .
 */
typedef enum {
        AI_EXTERNAL_GATE_MODE_FREE_RUN          = 0,
        AI_EXTERNAL_GATE_MODE_HALT_GATING       = 1,
} ai_external_gate_mode_t;


/**
 * This bit selects the pulsewidth and assertion time of the EXTMUX_CLK output signal:
 *      - 0:    Pulsewidth is 4.5 AI_OUT_TIMEBASE periods. EXTMUX_CLK trails the
 *              LOCALMUX_CLK pulse by 0.5–1.5 AI_OUT_TIMEBASE periods.
 *      - 1:    Pulsewidth is equal to the pulsewidth of the LOCALMUX_CLK read pulse selected
 *              by AI_LOCALMUX_CLK_Pulse_Width. EXTMUX_CLK and
 *              LOCALMUX_CLK are asserted at the same time.
 *      .
 */
typedef enum {
        AI_EXTMUX_CLK_PULSE_WIDTH_45_CLK_PERIODS                        = 0,
        AI_EXTMUX_CLK_PULSE_WIDTH_EQ_LOCALMUX_CLK_PULSE_WIDTH           = 1,
} ai_extmux_clk_pulse_width_t;


/**
 * If AI_CONVERT_Pulse_Timebase is 0, this bit determines the pulsewidth of the CONVERT
 * and PFI2/CONV output signals. If AI_CONVERT_Pulse_Timebase is 1 and
 * AI_CONVERT_Original_Pulse is 0, this bit determines the maximal pulsewidth of the
 * CONVERT and PFI2/CONV signals (so that the pulsewidth is equal to the shorter of this
 * pulsewidth and the original signal pulsewidth). The pulsewidths are as follows:
 *      - 0: 1.5–2 AI_OUT_TIMEBASE periods.
 *      - 1: 0.5–1 AI_OUT_TIMEBASE periods.
 *      .
 */
typedef enum {
        AI_CONVERT_PULSE_WIDTH_2_ClOCK_PERIODS     = 0,
        AI_CONVERT_PULSE_WIDTH_1_ClOCK_PERIOD      = 1,
} ai_convert_pulse_width_t;


/**
 * This bit enables and selects the polarity of the CONVERT output signal:
 *      - 0: High Z.
 *      - 1: Ground.
 *      - 2: Enable, active low.
 *      - 3: Enable, active high.
 *      .
 * This bitfield also selects the polarity of the PFI2/CONV output signal, if enabled for output:
 *      - 0: Active low.
 *      - 1: Ground.
 *      - 2: Active low.
 *      - 3: Active high.
 *      .
 */
typedef enum {
        AI_CONVERT_OUTPUT_SELECT_HIGH_Z            = 0,
        AI_CONVERT_OUTPUT_SELECT_GROUND            = 1,
        AI_CONVERT_OUTPUT_SELECT_ACTIVE_LOW        = 2,
        AI_CONVERT_OUTPUT_SELECT_ACTIVE_HIGH       = 3,
} ai_convert_output_select_t;


/**
 * This bit enables and selects polarity for the EXTMUX_CLK output signal:
 *      - 0: High Z.
 *      - 1: Ground.
 *      - 2: Enable, active low.
 *      - 3: Enable, active high.
 *      .
 */
typedef enum {
        AI_EXTMUX_OUTPUT_SELECT_HIGH_Z            = 0,
        AI_EXTMUX_OUTPUT_SELECT_GROUND            = 1,
        AI_EXTMUX_OUTPUT_SELECT_ACTIVE_LOW        = 2,
        AI_EXTMUX_OUTPUT_SELECT_ACTIVE_HIGH       = 3,
} ai_extmux_output_select_t;


/**
 * This bitfield enables and selects the polarity of the SCAN_IN_PROG output signal:
 *      - 0: High Z.
 *      - 1: Ground.
 *      - 2: Enable, active low.
 *      - 3: Enable, active high.
 */
typedef enum {
        AI_SIP_OUTPUT_SELECT_HIGH_Z            = 0,
        AI_SIP_OUTPUT_SELECT_GROUND            = 1,
        AI_SIP_OUTPUT_SELECT_ACTIVE_LOW        = 2,
        AI_SIP_OUTPUT_SELECT_ACTIVE_HIGH       = 3,
} ai_sip_output_select_t;


/**
 * This bit selects the output source for the bidirectional pin PFI7/AI_START_Pulse if the pin
 * is configured for output:
 *      - 0:    If AI_Trigger_Length is set to 0, the pin will reflect the internal signal AD_START.
 *              If AI_Trigger_Length is set to 1, the pin will reflect the internal signal AD_START
 *              after it has been pulse stretched to be 1–2 AI_OUT_TIMEBASE periods long.
 *      - 1:    The pin will output the same signal as SCAN_IN_PROG. If SCAN_IN_PROG is
 *              configured for high impedance, the pin will output ground.
 *      .
 */
typedef enum {
        AI_SAMPLE_OUTPUT_SELECT_AD_START          = 0,
        AI_SAMPLE_OUTPUT_SELECT_SCAN_IN_PROG      = 1,
} ai_sample_output_select_t;


/**
 * This bit selects the START trigger:
 *      - 0:     The internal signal SI_TC.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 18:    Bitfield AI_START_Pulse.
 *      - 19:    The internal signal G_OUT from general-purpose counter 0.
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 28:    GPCTR1_OUT.
 *      - 29:    SCXI_TRIG1.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31:    Logic low.
 *      .
 * When you set this bit to 0, the PXI6259 card is in the internal START mode. When you select
 * any other signal as the START trigger, the PXI6259 card is in the external START mode.
 *
 */
typedef enum {
        AI_SAMPLE_SELECT_SI_TC             = 0,
        AI_SAMPLE_SELECT_PFI0              = 1,
        AI_SAMPLE_SELECT_PFI1              = 2,
        AI_SAMPLE_SELECT_PFI2              = 3,
        AI_SAMPLE_SELECT_PFI3              = 4,
        AI_SAMPLE_SELECT_PFI4              = 5,
        AI_SAMPLE_SELECT_PFI5              = 6,
        AI_SAMPLE_SELECT_PFI6              = 7,
        AI_SAMPLE_SELECT_PFI7              = 8,
        AI_SAMPLE_SELECT_PFI8              = 9,
        AI_SAMPLE_SELECT_PFI9              = 10,
        AI_SAMPLE_SELECT_RTSI0             = 11,
        AI_SAMPLE_SELECT_RTSI1             = 12,
        AI_SAMPLE_SELECT_RTSI2             = 13,
        AI_SAMPLE_SELECT_RTSI3             = 14,
        AI_SAMPLE_SELECT_RTSI4             = 15,
        AI_SAMPLE_SELECT_RTSI5             = 16,
        AI_SAMPLE_SELECT_RTSI6             = 17,
        AI_SAMPLE_SELECT_PULSE             = 18,
        AI_SAMPLE_SELECT_GPCRT0_OUT        = 19,
        AI_SAMPLE_SELECT_STAR_TRIGGER      = 20,
        AI_SAMPLE_SELECT_PFI10             = 21,
        AI_SAMPLE_SELECT_PFI11             = 22,
        AI_SAMPLE_SELECT_PFI12             = 23,
        AI_SAMPLE_SELECT_PFI13             = 24,
        AI_SAMPLE_SELECT_PFI14             = 25,
        AI_SAMPLE_SELECT_PFI15             = 26,
        AI_SAMPLE_SELECT_RTSI7             = 27,
        AI_SAMPLE_SELECT_GPCTR1_OUT        = 28,
        AI_SAMPLE_SELECT_SCXI_TRIG1        = 29,
        AI_SAMPLE_SELECT_ANALOG_TRIGGER    = 30,
        AI_SAMPLE_SELECT_LOW               = 31,
} ai_sample_select_t;


/**
 * This bit determines the polarity of Sample trigger:
 *              - 0: Active high or rising edge.
 *              - 1: Active low or falling edge.
 */
typedef enum {
        AI_SAMPLE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE                   = 0,
        AI_SAMPLE_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE                   = 1,
} ai_sample_polarity_t;


/**
 * This bitfield selects the STOP trigger:
 *      - 0: The internal signal DIV_TC or bitfield AI_STOP_Pulse.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 18:    The internal signal SI2_TC.
 *      - 19:    Signal present on the AI_STOP_IN pin.
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 28:    GPCTR1_OUT.
 *      - 29:    SCXI_TRIG1.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31: Logic low.
 *      .
 * Set this bit to 31 for single-channel operation if your board does not have configuration
 * memory.
 */
typedef enum {
        AI_STOP_SELECT_DIV_TC_OR_PULSE   = 0,
        AI_STOP_SELECT_PFI0              = 1,
        AI_STOP_SELECT_PFI1              = 2,
        AI_STOP_SELECT_PFI2              = 3,
        AI_STOP_SELECT_PFI3              = 4,
        AI_STOP_SELECT_PFI4              = 5,
        AI_STOP_SELECT_PFI5              = 6,
        AI_STOP_SELECT_PFI6              = 7,
        AI_STOP_SELECT_PFI7              = 8,
        AI_STOP_SELECT_PFI8              = 9,
        AI_STOP_SELECT_PFI9              = 10,
        AI_STOP_SELECT_RTSI0             = 11,
        AI_STOP_SELECT_RTSI1             = 12,
        AI_STOP_SELECT_RTSI2             = 13,
        AI_STOP_SELECT_RTSI3             = 14,
        AI_STOP_SELECT_RTSI4             = 15,
        AI_STOP_SELECT_RTSI5             = 16,
        AI_STOP_SELECT_RTSI6             = 17,
        AI_STOP_SELECT_SI2TC             = 18,
        AI_STOP_SELECT_IN                = 19,
        AI_STOP_SELECT_STAR_TRIGGER      = 20,
        AI_STOP_SELECT_PFI10             = 21,
        AI_STOP_SELECT_PFI11             = 22,
        AI_STOP_SELECT_PFI12             = 23,
        AI_STOP_SELECT_PFI13             = 24,
        AI_STOP_SELECT_PFI14             = 25,
        AI_STOP_SELECT_PFI15             = 26,
        AI_STOP_SELECT_RTSI7             = 27,
        AI_STOP_SELECT_ANALOG_TRIGGER    = 30,
        AI_STOP_SELECT_LOW               = 31,
} ai_stop_select_t;


/**
 * This bit determines the polarity of STOP trigger:
 *              - 0: Active high or rising edge.
 *              - 1: Active low or falling edge.
 *      .
 * Set this bit to 0 if AI_STOP_Select is set to 0. You should set this bit to 1 if AI_STOP_Select
 * is set to 31.
 */
typedef enum {
        AI_STOP_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE            = 0,
        AI_STOP_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE            = 1,
} ai_stop_polarity_t;


/* class tAnalog_Trigger_Etc : public tReg16IODirect32 */
typedef enum {
        kAnalog_Trigger_ModeLow_Window        = 0,
        kAnalog_Trigger_ModeHigh_Window       = 1,
        kAnalog_Trigger_ModeMiddle_Window     = 2,
        kAnalog_Trigger_ModeHigh_Hysteresis   = 4,
        kAnalog_Trigger_ModeLow_Hysteresis    = 6,
} tAnalog_Trigger_Mode;


typedef enum {
        kAnalog_Trigger_EnableDisabled          = 0,
        kAnalog_Trigger_EnableEnabled           = 1,
} tAnalog_Trigger_Enable;


typedef enum {
        GPFO_1_Output_SelectG_OUT             = 0,
        GPFO_1_Output_SelectDIV_TC            = 1,
} GPFO_1_Output_Select;


typedef enum {
        GPFO_0_Output_SelectG_OUT             = 0,
        GPFO_0_Output_SelectRTSI0             = 1,
        GPFO_0_Output_SelectRTSI1             = 2,
        GPFO_0_Output_SelectRTSI2             = 3,
        GPFO_0_Output_SelectRTSI3             = 4,
        GPFO_0_Output_SelectRTSI4             = 5,
        GPFO_0_Output_SelectRTSI5             = 6,
        GPFO_0_Output_SelectRTSI6             = 7,
} GPFO_0_Output_Select;

typedef enum {
        GPFO_0_Output_EnableInput             = 0,
        GPFO_0_Output_EnableOutput            = 1,
} GPFO_0_Output_Enable;

typedef enum {
        GPFO_1_Output_EnableInput             = 0,
        GPFO_1_Output_EnableOutput            = 1,
} GPFO_1_Output_Enable;

//---------------------------------------------------------------------------
// G0_Command
//---------------------------------------------------------------------------
/**
 * This bitfield selects the General Purpose Counter counting direction.
 *      - 0:    software defined to count down.
 *      - 1:    software defined to count up.
 *      - 2:    hardware defined.
 *      - 3:    hardware defined by gate.
 *      .
 */
typedef enum {
        GPC_COUNT_DIRECTION_SOFTWARE_DOWN     = 0,
        GPC_COUNT_DIRECTION_SOFTWARE_UP       = 1,
        GPC_COUNT_DIRECTION_HARDWARE          = 2,
        GPC_COUNT_DIRECTION_HARDWARE_GATE     = 3,
} gpc_count_direction_t;


/**
 * This bit enables the write switch feature of the general-purpose counter load registers.
 * Writes to load register A are:
 *      - 0:    Unconditionally directed to load register A.
 *      - 1:    Directed to the inactive load register.
 *      .
 */
typedef enum {
        GPC_WRITE_SWITCH_ALWAYS_LOAD_A     = 0,
        GPC_WRITE_SWITCH_INACTIVE_LOAD     = 1,
} gpc_write_switch_t;


/**
 * This bit enables gate synchronization to the source:
 *       - 0:   Disabled.
 *       - 1:   Enabled.
 *       .
 * You should normally set this bit to 1. You can set this bit to 0 if you know that
 * the gate signal is synchronized to the source signal.
 */
typedef enum {
        GPC_SYNCHRONIZED_GATE_DISABLED         = 0,
        GPC_SYNCHRONIZED_GATE_ENABLED          = 1,
} gpc_synchronized_gate_t;


/**
 * This bit selects the load or save register segment to be used for automatic interrupt
 * acknowledgment:
 *      - 0:    Low register.
 *      - 1:    High register.
 *      .
 */
typedef enum {
        GPC_LITTLE_BIG_ENDIAN_LOW_REGISTER      = 0,
        GPC_LITTLE_BIG_ENDIAN_HIGH_REGISTER     = 1,
} gpc_little_big_endian_t;


/**
 * This bit selects the source that controls general purpose counter i load register bank switching,
 * if bank switching is enabled:
 *      - 0:   Gate.
 *      - 1:   Software.
 *      .
 */
typedef enum {
        GPC_BANK_SWITCH_MODE_GATE              = 0,
        GPC_BANK_SWITCH_MODE_SOFTWARE          = 1,
} gpc_bank_switch_mode_t;


/**
 * If the general-purpose counter i is not armed, this bit selects the bank to which you can write:
 *      - 0:    Bank X.
 *      - 1:    Bank Y.
 *      .
 * If the general-purpose counter i is armed, this bit enables bank switching:
 *      - 0:    Disabled.
 *      - 1:    Enabled.
 *      .
 */
typedef enum {
        GPC_BANK_SWITCH_ENABLE_BANK_X            = 0,
        GPC_BANK_SWITCH_ENABLE_BANK_Y            = 1,
} gpc_bank_switch_enable_t;


//---------------------------------------------------------------------------
// GC_Mode
//---------------------------------------------------------------------------
/**
 * This bit enables and selects the counter gating mode:
 *      - 0:    Gating is disabled.
 *      - 1:    Level gating.
 *      - 2:    Edge gating:
 *              •       Rising edge if Gi_Gating_Polarity is set to 0.
 *              •       Falling edge if Gi_Gating_Polarity is set to 1.
 *      - 3:    Edge gating:
 *              •       Falling edge if Gi_Gating_Polarity is set to 0.
 *              •       Rising edge if Gi_Gating_Polarity is set to 1.
 *      .
 * When Gi_Gating_Mode is 0 (gating disabled), gate level is available only for control of
 * counting direction (up/down), and for no other purpose.
 */
typedef enum {
        GPC_GATING_MODE_GATING_DISABLED                = 0,
        GPC_GATING_MODE_LEVEL_GATING                   = 1,
        GPC_GATING_MODE_EDGE_GATING_ACTIVE_HIGH        = 2,
        GPC_GATING_MODE_EDGE_GATING_ACTIVE_LOW         = 3,
} gpc_gating_mode_t;


/**
 * This bit enables you to use both gate edges to generate the gate interrupt and/or to control
 * counter operation:
 *      - 0:    Disabled.
 *      - 1:    Enabled.
 *      .
 * This bit also affects where interrupts are generated.
 */
typedef enum {
        GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_DISABLED     = 0,
        GPC_GATE_ON_BOTH_EDGES_BOTH_EDGES_ENABLED      = 1,
} gpc_gate_on_both_edges_t;


/**
 * This bit selects the triggering mode, if gating is not disabled:
 *      - 0:    The first gate edge starts, the next stops the counting.
 *      - 1:    The first gate edge stops, the next starts the counting.
 *      - 2:    Gate edge always starts the counting, unless counting is already in progress, in
 *              which case the edge is ignored. The valid Gi_Stop_Mode settings for this selection
 *              are 1 and 2, but only the TC (not the gate) will stop the counting.
 *      - 3:    Gate is used for reload, save, or load select only, if any of those options is enabled;
 *              not for stopping.
 *      .
 * Selections 0 and 1 are valid only if Gi_Stop_Mode is set to 0 (no hardware limit on this).
 * Selections 0, 1, and 2 are valid only if Gi_Gating_Mode is set to 2 or 3. Selection 3 is valid
 * only if Gi_Gating_Mode is not set to 0.
 */
typedef enum {
        GPC_TRIGGER_MODE_FOR_EDGE_GATE_FIRST_STARTS_NEXT_STOPS    = 0,
        GPC_TRIGGER_MODE_FOR_EDGE_GATE_FIRST_STARTS_NEXT_STARTS   = 1,
        GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_STARTS_TC_STOPS       = 2,
        GPC_TRIGGER_MODE_FOR_EDGE_GATE_GATE_DOES_NOT_STOP         = 3,
} gpc_trigger_mode_for_edge_gate_t;


/**
 * This bit selects the condition on which the counter will stop:
 *      - 0:    Stop on gate condition.
 *      - 1:    Stop on gate condition or at the first TC, whichever comes first.
 *      - 2:    Stop by gate condition or at the second TC, whichever comes first.
 *      - 3:    Reserved.
 *      .
 * Notice that, regardless of this bitfield setting, you can always use the software disarm
 * command, Gi_Disarm, to stop the counter. The gate condition that stops the counter is
 * determined by Gi_Gating_Mode (in case of level gating) or by a combination of
 * Gi_Gating_Mode and Gi_Trigger_Mode_For_Edge_Gate (in case of edge gating). Selections
 * 1 and 2 are valid only if Gi_Trigger_Mode_For_Edge_Gate is set to 2 (no hardware limit on
 * this).
 *
 */
typedef enum {
        GPC_STOP_MODE_STOP_ON_GATE                        = 0,
        GPC_STOP_MODE_STOP_ON_GATE_OR_FIRST_TC            = 1,
        GPC_STOP_MODE_STOP_ON_GATE_OR_SECOND_TC           = 2,
        GPC_STOP_MODE_STOP_MODE_RESERVED                  = 3,
} gpc_stop_mode_t;


/**
 * If general-purpose counter i is disarmed, this bit selects the initial counter load register:
 *      - 0:    Load register A.
 *      - 1:    Load register B.
 *      .
 * The source for subsequent loads depends on Reload_Source_Switching. If
 * general-purpose counter i is armed, writing to this bit has no effect.
 */
typedef enum {
        GPC_LOAD_SOURCE_SELECT_LOAD_A            = 0,
        GPC_LOAD_SOURCE_SELECT_LOAD_B            = 1,
} gpc_load_source_select_t;


/**
 * Setting this bit to 1 loads the contents of the selected load register into general-purpose
 * counter. This bit is cleared automatically.
 */
typedef enum {
        GPC_LOAD_DONT_LOAD               = 0,
        GPC_LOAD_LOAD                    = 1,
} gpc_load_t;


/**
 * This bit selects the mode for the G_OUT signal:
 *      - 0:    Reserved.
 *      - 1:    TC mode. The counter TC signal appears on G_OUT.
 *      - 2:    Toggle output on TC mode. G_OUT changes state on the trailing edge of counter
 *              TC.
 *      - 3:    Toggle output on TC or gate mode. G_OUT changes state on the trailing edge of
 *              counter TC and on the active gate edge. This mode can be used for sequential
 *              scanning.
 *      .
 */
typedef enum {
        GPC_OUTPUT_MODE_RESERVED                  = 0,
        GPC_OUTPUT_MODE_PULSE                     = 1,
        GPC_OUTPUT_MODE_TOGGLE                    = 2,
        GPC_OUTPUT_MODE_TOGGLE_ON_TC_OR_GATE      = 3,
} gpc_output_mode_t;


/**
 * This bit determines whether the hardware disarms the counter when the counter stops due to
 * a hardware condition:
 *      - 0:    No hardware disarm.
 *      - 1:    Disarm at the TC that stops counting.
 *      - 2:    Disarm at the G_GATE that stops counting.
 *      - 3:    Disarm at the TC or G_GATE that stops counting, whichever comes first.
 *      .
 */
typedef enum {
        GPC_COUNTING_ONCE_NO_HW_DISARM            = 0,
        GPC_COUNTING_ONCE_DISARM_ON_TC            = 1,
        GPC_COUNTING_ONCE_DISARM_ON_GATE          = 2,
        GPC_COUNTING_ONCE_DISARM_ON_GATE_OR_TC    = 3,
} gpc_counting_once_t;


/**
 * This bit determines the counter behavior on TC:
 *      - 0:    Roll over on TC.
 *      - 1:    Reload on TC.
 *      .
 * Notice that it is legal to set both Gi_Loading_On_Gate and Gi_Loading_On_TC to 1
 * simultaneously.
 */
typedef enum {
        GPC_LOADING_ON_TC_ROLLOVER_ON_TC          = 0,
        GPC_LOADING_ON_TC_RELOAD_ON_TC            = 1,
} gpc_loading_on_tc_t;


/**
 * This bit determines whether the gate signal causes counter reload:
 *      - 0:    Gate signal does not cause counter reload.
 *      - 1:    Counter is reloaded on gate edge that stops the counter, unless edge gating is used
 *              and Gi_Trigger_Mode_For_Edge_Gate is set to 3. In the later case, counter is
 *              reloaded on every selected gate edge.
 *      .
 * Reloading occurs on active source edge. Notice that it is legal to set both
 * Loading_On_Gate and Loading_On_TC to 1 simultaneously.
 */
typedef enum {
        GPC_LOADING_ON_GATE_NO_RELOAD                     = 0,
        GPC_LOADING_ON_GATE_RELOAD_ON_STOP_GATE           = 1,
} gpc_loading_on_gate_t;


/**
 * If Gate_Select_Load_Source is set to 0, this bit enables load register selection in the
 * following manner:
 *      - 0:    Always use the same load register.
 *      - 1:    Alternate between the two load registers.
 *      .
 */
typedef enum {
        GPC_RELOAD_SOURCE_USE_SAME        = 0,
        GPC_RELOAD_SOURCE_ALTERNATE       = 1,
} gpc_reload_source_switching_t;


/**
 * Define DMA behaviour
 */
typedef enum {
        GPC_DMA_ENABLE_READ             = 0,
        GPC_DMA_ENABLE_WRITE            = 1,
} gpc_dma_enable_t;


/**
 * This bit indicates the current G_OUT state (after the polarity selection):
 *      - 0:    Low.
 *      - 1:    High.
 *      .
 */
typedef enum {
        GPC_OUTPUT_ST_LOW               = 0,
        GPC_OUTPUT_ST_HIGH              = 1,
} gpc_output_st_t;


/**
 * This bit indicates the status of the HW save register for general-purpose counter i:
 *      - 0:    HW save register is tracing the counter.
 *      - 1:    HW save register is latched for later read.
 *      .
 */
typedef enum {
        GPC_HW_SAVE_ST_TRACING           = 0,
        GPC_HW_SAVE_ST_LATCHED           = 1,
} gpc_hw_save_st_t;


/**
 * This bit indicates the detection of a permanent stale data error:
 *      - 0:    No error.
 *      - 1:    Error.
 *      .
 * A permanent stale data error occurs if Stale_Data_St was set at any time during an
 * interrupt-driven noncumulative-event counting or period-measurement operation. This is
 * useful for after-the-fact error detection.
 */
typedef enum {
        GPC_PERMANENT_STALE_DATA_ST_NO_ERROR          = 0,
        GPC_PERMANENT_STALE_DATA_ST_ERROR             = 1,
} gpc_permanent_stale_data_st_t;

typedef enum {
        kG0_Bank_StBank_X            = 0,
        kG0_Bank_StBank_Y            = 1,
} tG0_Bank_St;

typedef enum {
        kG1_Bank_StBank_X            = 0,
        kG1_Bank_StBank_Y            = 1,
} tG1_Bank_St;

typedef enum {
        kG0_Gate_StInactive          = 0,
        kG0_Gate_StActive            = 1,
} tG0_Gate_St;

typedef enum {
        kG1_Gate_StInactive          = 0,
        kG1_Gate_StActive            = 1,
} tG1_Gate_St;


/**
 * This bit selects the active edge of the general-purpose counter i source:
 *      - 0:    Rising edge.
 *      - 1:    Falling edge.
 *      .
 */
typedef enum {
        GPC_SOURCE_POLARITY_RISING_EDGE             = 0,
        GPC_SOURCE_POLARITY_FALLING_EDGE            = 1,
} gpc_source_polarity_t;



//  Possible Gi_Source_Select values:
//  0: TB1(20 MHz)
//  1-10: PFI<0..9>
//  11-17: RTSI<0..6>
//  18: TB2(100 kHz)
//  19: The G_TC signal from the other general-purpose counter
//  20: The G_GATE signal from the other counter, when G0_Src_SubSelect = 0;
//      Star_Trig when G0_Src_SubSelect = 1
//  21-26: PFI<10..15>
//  27: RTSI7
//  28: TB2(200 KHz)
//  29: PXIClk10
//  30: The internal signal TB3(80 MHz), when G0_Src_SubSelect = 0;
//      The output of the Analog Trigger module when G0_Src_SubSelect = 1
//  31: Logic low
typedef enum {
        GPC_SOURCE_SELECT_TIMEBASE1                 = 0,
        GPC_SOURCE_SELECT_PFI0                      = 1,
        GPC_SOURCE_SELECT_PFI1                      = 2,
        GPC_SOURCE_SELECT_PFI2                      = 3,
        GPC_SOURCE_SELECT_PFI3                      = 4,
        GPC_SOURCE_SELECT_PFI4                      = 5,
        GPC_SOURCE_SELECT_PFI5                      = 6,
        GPC_SOURCE_SELECT_PFI6                      = 7,
        GPC_SOURCE_SELECT_PFI7                      = 8,
        GPC_SOURCE_SELECT_PFI8                      = 9,
        GPC_SOURCE_SELECT_PFI9                      = 10,
        GPC_SOURCE_SELECT_RTSI0                     = 11,
        GPC_SOURCE_SELECT_RTSI1                     = 12,
        GPC_SOURCE_SELECT_RTSI2                     = 13,
        GPC_SOURCE_SELECT_RTSI3                     = 14,
        GPC_SOURCE_SELECT_RTSI4                     = 15,
        GPC_SOURCE_SELECT_RTSI5                     = 16,
        GPC_SOURCE_SELECT_RTSI6                     = 17,
        GPC_SOURCE_SELECT_TIMEBASE2                 = 18,
        GPC_SOURCE_SELECT_TC                        = 19,
        GPC_SOURCE_SELECT_GATE                      = 20,
        GPC_SOURCE_SELECT_PFI10                     = 21,
        GPC_SOURCE_SELECT_PFI11                     = 22,
        GPC_SOURCE_SELECT_PFI12                     = 23,
        GPC_SOURCE_SELECT_PFI13                     = 24,
        GPC_SOURCE_SELECT_PFI14                     = 25,
        GPC_SOURCE_SELECT_PFI15                     = 26,
        GPC_SOURCE_SELECT_RTSI7                     = 27,
        GPC_SOURCE_SELECT_TIMEBASE1_2               = 28,
        GPC_SOURCE_SELECT_PXIclk10                  = 29,
        GPC_SOURCE_SELECT_TB3                       = 30,
        GPC_SOURCE_SELECT_LOGIC_LOW                 = 31,
} gpc_source_select_t;


//  Possible Gi_Gate_Select values
//  0: The output of the TimeStamping Mux.
//     This signal is selected with the G0_TimeStamp_Select bitfield
//  1-10: PFI<0..9>
//  11-17: RTSI<0..6>
//  18: The internal analog input signal AI_START2
//  19: Star_Trig
//  20: The G_OUT signal from the other general-purpose counter
//  21-26: PFI<10..15>
//  27: RTSI7
//  28: The internal analog input signal AI_START1
//  29: The G_SOURCE signal from the other general-purpose counter
//  30: The output of the Analog Trigger Module
//  31: Logic low
typedef enum {
        GPC_GATE_SELECT_TIMESTAMPING_MUX_OUTPUT   = 0,
        GPC_GATE_SELECT_PFI0                      = 1,
        GPC_GATE_SELECT_PFI1                      = 2,
        GPC_GATE_SELECT_PFI2                      = 3,
        GPC_GATE_SELECT_PFI3                      = 4,
        GPC_GATE_SELECT_PFI4                      = 5,
        GPC_GATE_SELECT_PFI5                      = 6,
        GPC_GATE_SELECT_PFI6                      = 7,
        GPC_GATE_SELECT_PFI7                      = 8,
        GPC_GATE_SELECT_PFI8                      = 9,
        GPC_GATE_SELECT_PFI9                      = 10,
        GPC_GATE_SELECT_RTSI0                     = 11,
        GPC_GATE_SELECT_RTSI1                     = 12,
        GPC_GATE_SELECT_RTSI2                     = 13,
        GPC_GATE_SELECT_RTSI3                     = 14,
        GPC_GATE_SELECT_RTSI4                     = 15,
        GPC_GATE_SELECT_RTSI5                     = 16,
        GPC_GATE_SELECT_RTSI6                     = 17,
        GPC_GATE_SELECT_AI_START2                 = 18,
        GPC_GATE_SELECT_STAR_TRIGGER              = 19,
        GPC_GATE_SELECT_G_OUT                     = 20,
        GPC_GATE_SELECT_PFI10                     = 21,
        GPC_GATE_SELECT_PFI11                     = 22,
        GPC_GATE_SELECT_PFI12                     = 23,
        GPC_GATE_SELECT_PFI13                     = 24,
        GPC_GATE_SELECT_PFI14                     = 25,
        GPC_GATE_SELECT_PFI15                     = 26,
        GPC_GATE_SELECT_RTSI7                     = 27,
        GPC_GATE_SELECT_AI_START1                 = 28,
        GPC_GATE_SELECT_G_SOURCE                  = 29,
        GPC_GATE_SELECT_ANALOG_OUTPUT             = 30,
        GPC_GATE_SELECT_LOGIC_LOW                 = 31,
} gpc_gate_select_t;


/**
 * Select the type of task to use with the counter.
 *      - 0:    No task will be used.
 *      - 1:    Counter will perform as simple counter.
 *      - 2:    Use counter to generate single pulse.
 *      - 3:    Delay trigger for set period of time.
 *      .
 */
typedef enum {
        GPC_TASK_TYPE_NOT_IN_USE                        = 0,
        GPC_TASK_TYPE_SIMPLE_COUNTER,
        GPC_TASK_TYPE_SINGLE_PULSE_GENERATION,
        GPC_TASK_TYPE_DELAY_OUTPUT_TRIGGER,
} gpc_task_type_t;


/* class tAI_Trigger_Select : public tReg16IODirect32 */

/**
 * This bitfield selects the START1 trigger:
 *      - 0:     Bitfield AI_START1_Pulse.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 18:    The internal signal G_OUT from general-purpose counter 0.
 *      - 19:    The internal signal G_OUT from general-purpose counter 1.
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31:    Logic low.
 *      .
 */
typedef enum {
        AI_START_SELECT_PULSE             = 0,
        AI_START_SELECT_PFI0              = 1,
        AI_START_SELECT_PFI1              = 2,
        AI_START_SELECT_PFI2              = 3,
        AI_START_SELECT_PFI3              = 4,
        AI_START_SELECT_PFI4              = 5,
        AI_START_SELECT_PFI5              = 6,
        AI_START_SELECT_PFI6              = 7,
        AI_START_SELECT_PFI7              = 8,
        AI_START_SELECT_PFI8              = 9,
        AI_START_SELECT_PFI9              = 10,
        AI_START_SELECT_RTSI0             = 11,
        AI_START_SELECT_RTSI1             = 12,
        AI_START_SELECT_RTSI2             = 13,
        AI_START_SELECT_RTSI3             = 14,
        AI_START_SELECT_RTSI4             = 15,
        AI_START_SELECT_RTSI5             = 16,
        AI_START_SELECT_RTSI6             = 17,
        AI_START_SELECT_GPCTR0_OUT        = 18,
        AI_START_SELECT_GPCTR1_OUT        = 19,
        AI_START_SELECT_STAR_TRIGGER      = 20,
        AI_START_SELECT_PFI10             = 21,
        AI_START_SELECT_PFI11             = 22,
        AI_START_SELECT_PFI12             = 23,
        AI_START_SELECT_PFI13             = 24,
        AI_START_SELECT_PFI14             = 25,
        AI_START_SELECT_PFI15             = 26,
        AI_START_SELECT_RTSI7             = 27,
        AI_START_SELECT_ANALOG_TRIGGER    = 30,
        AI_START_SELECT_LOW               = 31,
} ai_start_select_t;


/**
 * This bit determines the polarity of the START1 trigger:
 *      - 0: Active high or rising edge.
 *      - 1: Active low or falling edge.
 *      .
 * You should set this bit to 0 if AI_START1_Select is set to 0.
 */
typedef enum {
        AI_START_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE            = 0,
        AI_START_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE            = 1,
} ai_start_polarity_t;


/**
 * This bitfield selects the REFERENCE trigger:
 *      - 0:    AI_REFERENCE_Pulse.
 *      - 1–10: PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31: Logic low.
 *      .
 */
typedef enum {
        AI_REFERENCE_SELECT_PULSE             = 0,
        AI_REFERENCE_SELECT_PFI0              = 1,
        AI_REFERENCE_SELECT_PFI1              = 2,
        AI_REFERENCE_SELECT_PFI2              = 3,
        AI_REFERENCE_SELECT_PFI3              = 4,
        AI_REFERENCE_SELECT_PFI4              = 5,
        AI_REFERENCE_SELECT_PFI5              = 6,
        AI_REFERENCE_SELECT_PFI6              = 7,
        AI_REFERENCE_SELECT_PFI7              = 8,
        AI_REFERENCE_SELECT_PFI8              = 9,
        AI_REFERENCE_SELECT_PFI9              = 10,
        AI_REFERENCE_SELECT_RTSI0             = 11,
        AI_REFERENCE_SELECT_RTSI1             = 12,
        AI_REFERENCE_SELECT_RTSI2             = 13,
        AI_REFERENCE_SELECT_RTSI3             = 14,
        AI_REFERENCE_SELECT_RTSI4             = 15,
        AI_REFERENCE_SELECT_RTSI5             = 16,
        AI_REFERENCE_SELECT_RTSI6             = 17,
        AI_REFERENCE_SELECT_STAR_TRIGGER      = 20,
        AI_REFERENCE_SELECT_PFI10             = 21,
        AI_REFERENCE_SELECT_PFI11             = 22,
        AI_REFERENCE_SELECT_PFI12             = 23,
        AI_REFERENCE_SELECT_PFI13             = 24,
        AI_REFERENCE_SELECT_PFI14             = 25,
        AI_REFERENCE_SELECT_PFI15             = 26,
        AI_REFERENCE_SELECT_RTSI7             = 27,
        AI_REFERENCE_SELECT_ANALOG_TRIGGER    = 30,
        AI_REFERENCE_SELECT_LOW               = 31,
} ai_reference_select_t;


/**
 * This bit determines the polarity of Reference trigger:
 *      - 0: Active high or rising edge.
 *      - 1: Active low or falling edge.
 *      .
 * You should set this bit to 0 if AI_REFERENCE_Select is set to 0
 */
typedef enum {
        AI_REFERENCE_POLARITY_ACTIVE_HIGH_OR_RISING_EDGE        = 0,
        AI_REFERENCE_POLARITY_ACTIVE_LOW_OR_FALLING_EDGE        = 1,
} ai_reference_polarity_t;


/**
 * This bit determines the behavior of the SC, SI, SI2, and DIV counters during SC_TC:
 *      - 0:   If AI_Pre_Trigger is 0, the counters will return to idle on the first SC_TC. If
 *              AI_Pre_Trigger is 1, the counters will return to idle on the second SC_TC.
 *      - 1:    The counters will ignore SC_TC.
 *      .
 * Set this bit to 0 to select the pretrigger or posttrigger acquisition modes if you want to acquire
 * a predetermined number of scans. Set this bit to 1 to select the continuous acquisition mode
 * if you wish to continuously acquire data or to perform staged analog input. You can use
 * AI_End_On_End_Of_Scan and AI_End_On_SC_TC to stop an analog input operation in the
 * continuous acquisition mode.
 */
typedef enum {
        AI_CONTINUOUS_DISABLE           = 0,
        AI_CONTINUOUS_ENABLE            = 1,
} ai_continuous_t;


/**
 * This bit determines the behavior of the BC, UC, and UI counters during BC_TC:
 *      - 0:    Counters will stop on BC_TC.
 *      - 1:    Counters will ignore BC_TC. The counters remain armed and generate UPDATE
 *              pulses until an AO_End_On_BC_TC or AO_End_On_UC_TC command is given,
 *              until the AOTM is reset using AO_Reset, or until an AO_Trigger_Once command
 *              is issued.
 *      .
 */
typedef enum {
        AO_CONTINUOUS_STOP_ON_BC_TC     = 0,
        AO_CONTINUOUS_IGNORE_BC_TC      = 1,
} ao_continuous_t;


/**
 * This bit selects the active edge of the UI source (the signal that is selected by
 * AO_UI_Source_Select):
 *      - 0:    Rising edge.
 *      - 1:    Falling edge.
 *      .
 */
typedef enum {
        AO_UI_SOURCE_POLARITY_RISING_EDGE       = 0,
        AO_UI_SOURCE_POLARITY_FALLING_EDGE      = 1,
} ao_ui_source_polarity_t;


/**
 * This bit selects the active edge of the UPDATE source (the signal that is selected by
 * AO_UPDATE_Source_Select):
 *      - 0:    Rising edge.
 *      - 1:    Falling edge.
 *      .
 * You must set this bit to 0 in the internal UPDATE mode.
 */
typedef enum {
        AO_UPDATE_SOURCE_POLARITY_RISING_EDGE       = 0,
        AO_UPDATE_SOURCE_POLARITY_FALLING_EDGE      = 1,
} ao_update_source_polarity_t;


/**
 * This bitfield selects the AO_UI source :
 *      - 0:     Bitfield AO_UI_SOURCE_GROUND.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 19:    Internal Timebase 2 (100kHz)
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 28:    Internal Timebase 1 (20MHz)
 *      - 29:    PXI_CLK10
 *      - 30:    ANALOG_TRIGGER.
 *      - 31:    Logic low.
 *      .
 */
typedef enum {
        AO_UI_SOURCE_SELECT_GROUND            = 0,
        AO_UI_SOURCE_SELECT_PFI0              = 1,
        AO_UI_SOURCE_SELECT_PFI1              = 2,
        AO_UI_SOURCE_SELECT_PFI2              = 3,
        AO_UI_SOURCE_SELECT_PFI3              = 4,
        AO_UI_SOURCE_SELECT_PFI4              = 5,
        AO_UI_SOURCE_SELECT_PFI5              = 6,
        AO_UI_SOURCE_SELECT_PFI6              = 7,
        AO_UI_SOURCE_SELECT_PFI7              = 8,
        AO_UI_SOURCE_SELECT_PFI8              = 9,
        AO_UI_SOURCE_SELECT_PFI9              = 10,
        AO_UI_SOURCE_SELECT_RTSI0             = 11,
        AO_UI_SOURCE_SELECT_RTSI1             = 12,
        AO_UI_SOURCE_SELECT_RTSI2             = 13,
        AO_UI_SOURCE_SELECT_RTSI3             = 14,
        AO_UI_SOURCE_SELECT_RTSI4             = 15,
        AO_UI_SOURCE_SELECT_RTSI5             = 16,
        AO_UI_SOURCE_SELECT_RTSI6             = 17,
        AO_UI_SOURCE_SELECT_IN_TIMEBASE2      = 19,
        AO_UI_SOURCE_SELECT_STAR_TRIGGER      = 20,
        AO_UI_SOURCE_SELECT_PFI10             = 21,
        AO_UI_SOURCE_SELECT_PFI11             = 22,
        AO_UI_SOURCE_SELECT_PFI12             = 23,
        AO_UI_SOURCE_SELECT_PFI13             = 24,
        AO_UI_SOURCE_SELECT_PFI14             = 25,
        AO_UI_SOURCE_SELECT_PFI15             = 26,
        AO_UI_SOURCE_SELECT_RTSI7             = 27,
        AO_UI_SOURCE_SELECT_IN_TIMEBASE1      = 28,
        AO_UI_SOURCE_SELECT_PXI_CLK10         = 29,
        AO_UI_SOURCE_SELECT_ANALOG_TRIGGER    = 30,
        AO_UI_SOURCE_SELECT_LOW               = 31,
} ao_ui_source_select_t;


/**
 * This bitfield selects the AO_UPDATE_SOURCE:
 *      - 0:     Bitfield AO_UI_SOURCE_UI_TC.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 18:    General Purpose Counter 0 Out
 *      - 19:    General Purpose Counter 1 Out
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31:    Logic low.
 *      .
 */
typedef enum {
        AO_UPDATE_SOURCE_SELECT_UI_TC             = 0,
        AO_UPDATE_SOURCE_SELECT_PFI0              = 1,
        AO_UPDATE_SOURCE_SELECT_PFI1              = 2,
        AO_UPDATE_SOURCE_SELECT_PFI2              = 3,
        AO_UPDATE_SOURCE_SELECT_PFI3              = 4,
        AO_UPDATE_SOURCE_SELECT_PFI4              = 5,
        AO_UPDATE_SOURCE_SELECT_PFI5              = 6,
        AO_UPDATE_SOURCE_SELECT_PFI6              = 7,
        AO_UPDATE_SOURCE_SELECT_PFI7              = 8,
        AO_UPDATE_SOURCE_SELECT_PFI8              = 9,
        AO_UPDATE_SOURCE_SELECT_PFI9              = 10,
        AO_UPDATE_SOURCE_SELECT_RTSI0             = 11,
        AO_UPDATE_SOURCE_SELECT_RTSI1             = 12,
        AO_UPDATE_SOURCE_SELECT_RTSI2             = 13,
        AO_UPDATE_SOURCE_SELECT_RTSI3             = 14,
        AO_UPDATE_SOURCE_SELECT_RTSI4             = 15,
        AO_UPDATE_SOURCE_SELECT_RTSI5             = 16,
        AO_UPDATE_SOURCE_SELECT_RTSI6             = 17,
        AO_UPDATE_SOURCE_SELECT_GPCTR0_Out        = 18,
        AO_UPDATE_SOURCE_SELECT_GPCTR1_Out        = 19,
        AO_UPDATE_SOURCE_SELECT_STAR_TRIGEGR      = 20,
        AO_UPDATE_SOURCE_SELECT_PFI10             = 21,
        AO_UPDATE_SOURCE_SELECT_PFI11             = 22,
        AO_UPDATE_SOURCE_SELECT_PFI12             = 23,
        AO_UPDATE_SOURCE_SELECT_PFI13             = 24,
        AO_UPDATE_SOURCE_SELECT_PFI14             = 25,
        AO_UPDATE_SOURCE_SELECT_PFI15             = 26,
        AO_UPDATE_SOURCE_SELECT_RTSI7             = 27,
        AO_UPDATE_SOURCE_SELECT_ANALOG_TRIGGER    = 30,
        AO_UPDATE_SOURCE_SELECT_LOW               = 31,
} ao_update_source_select_t;


// NOT implemented
typedef enum {
        kAO_UI_Reload_ModeNo_Change             = 0,
        kAO_UI_Reload_ModeAlternate_On_STOP     = 4,
        kAO_UI_Reload_ModeSwitch_On_STOP        = 5,
        kAO_UI_Reload_ModeAlternate_On_BC_TC    = 6,
        kAO_UI_Reload_ModeSwitch_On_BC_TC       = 7,
} tAO_UI_Reload_Mode;


/**
 * Available options for AO_BC_INITIAL_LOAD_SOURCE
 *      - 0:    Source is Register A
 *      - 1:    Source is Register B
 *      .
 */
typedef enum {
        AO_BC_INITIAL_LOAD_SOURCE_REG_A             = 0,
        AO_BC_INITIAL_LOAD_SOURCE_REG_B             = 1,
} ao_bc_initial_load_source;


/**
 * Available options for AO_UI_INITIAL_LOAD_SOURCE
 *      - 0:    Source is Register A
 *      - 1:    Source is Register B
 *      .
 */
typedef enum {
        AO_UI_INITIAL_LOAD_SOURCE_REG_A                 = 0,
        AO_UI_INITIAL_LOAD_SOURCE_REG_B                 = 1,
} ao_ui_initial_load_source;


/**
 * Available options for AO_UC_INITIAL_LOAD_SOURCE
 *      - 0:    Source is Register A
 *      - 1:    Source is Register B
 *      .
 */
typedef enum {
        AO_UC_INITIAL_LOAD_SOURCE_REG_A                 = 0,
        AO_UC_INITIAL_LOAD_SOURCE_REG_B                 = 1,
} ao_uc_initial_load_source;


/**
 * Settings for interrupt generated by AO_FIFO
 *      - 0:    Generate interrupt if FIFO empty
 *      - 1:    Generate interrupt if FIFO less than half full
 *      - 2:    Generate interrupt if FIFO less than full
 *      - 3:    Generate interrupt if FIFO half full to full
 */
typedef enum {
        AO_FIFO_MODE_EMPTY                              = 0,
        AO_FIFO_MODE_LESS_THAN_HALF_FULL                = 1,
        AO_FIFO_MODE_LESS_THAN_FULL                     = 2,
        AO_FIFO_MODE_LESS_THAN_HALF_FULL_TO_FULL        = 3,
} ao_fifo_mode_t;

/* class tAO_Output_Control : public tReg16IODirect32 */

/**
 * This bitfield enables and selects the polarity of the UPDATE output signal:
 *      - 0:    High Z.
 *      - 1:    Ground.
 *      - 2:    Enable, active low.
 *      - 3:    Enable, active high.
 * This bitfield also selects the polarity of the PFI5/UPDATE output signal, if
 * enabled for output:
 *      - 0:    Active low.
 *      - 1:    Ground.
 *      - 2:    Active low.
 *      - 3:    Active high.
 *      .
 */
typedef enum {
        AO_UPDATE_OUTPUT_SELECT_HIGH_Z            = 0,
        AO_UPDATE_OUTPUT_SELECT_GROUND            = 1,
        AO_UPDATE_OUTPUT_SELECT_ACTIVE_LOW        = 2,
        AO_UPDATE_OUTPUT_SELECT_ACTIVE_HIGH       = 3,
} ao_update_output_select_t;

// NOT used
typedef enum {
        kAO_External_Gate_PolarityActive_High       = 0,
        kAO_External_Gate_PolarityActive_Low        = 1,
} tAO_External_Gate_Polarity;

// NOT used
typedef enum {
        kAO_External_Gate_SelectDisabled          = 0,
        kAO_External_Gate_SelectPFI0              = 1,
        kAO_External_Gate_SelectPFI1              = 2,
        kAO_External_Gate_SelectPFI2              = 3,
        kAO_External_Gate_SelectPFI3              = 4,
        kAO_External_Gate_SelectPFI4              = 5,
        kAO_External_Gate_SelectPFI5              = 6,
        kAO_External_Gate_SelectPFI6              = 7,
        kAO_External_Gate_SelectPFI7              = 8,
        kAO_External_Gate_SelectPFI8              = 9,
        kAO_External_Gate_SelectPFI9              = 10,
        kAO_External_Gate_SelectRTSI0             = 11,
        kAO_External_Gate_SelectRTSI1             = 12,
        kAO_External_Gate_SelectRTSI2             = 13,
        kAO_External_Gate_SelectRTSI3             = 14,
        kAO_External_Gate_SelectRTSI4             = 15,
        kAO_External_Gate_SelectRTSI5             = 16,
        kAO_External_Gate_SelectRTSI6             = 17,
        kAO_External_Gate_SelectStar_Trigger      = 20,
        kAO_External_Gate_SelectPFI10             = 21,
        kAO_External_Gate_SelectPFI11             = 22,
        kAO_External_Gate_SelectPFI12             = 23,
        kAO_External_Gate_SelectPFI13             = 24,
        kAO_External_Gate_SelectPFI14             = 25,
        kAO_External_Gate_SelectPFI15             = 26,
        kAO_External_Gate_SelectRTSI7             = 27,
        kAO_External_Gate_SelectAnalog_Trigger    = 30,
        kAO_External_Gate_SelectLow               = 31,
} tAO_External_Gate_Select;


// NOT used
typedef enum {
        kAO_Trigger_LengthDA_START1         = 0,
        kAO_Trigger_LengthDA_ST1ED          = 1,
} tAO_Trigger_Length;


/**
 * This bit selects the signal appearing on the bidirectional pin PFI6/AO_START1 when the pin
 * is configured for output:
 *      - 0:    Output the internal signal DA_START1.
 *      - 1:    Output the internal signal DA_ST1ED after it has been pulse stretched to be
 *              1–2 AO_OUT_TIMEBASE periods long.
 *      .
 */
typedef enum {
        AO_TRIGGER_LENGTH_DA_START1         = 0,
        AO_TRIGGER_LENGTH_DA_ST1ED          = 1,
} ao_trigger_lenght;


/**
 * This bit selects the BC counter source:
 *      - 0:    UPDATE
 *      - 1:    The internal signal UC_TC
 *      .
 */
typedef enum {
        AO_BC_SOURCE_SELECT_UPDATE            = 0,
        AO_BC_SOURCE_SELECT_UC_TC             = 1,
} ao_bc_source_select_t;


/**
 * If AO_UPDATE_Pulse_Timebase is 0, this bit determines the pulsewidth of the UPDATE
 * and PFI5/UPDATE signals. If AO_UPDATE_Pulse_Timebase is 1 and
 * AO_UPDATE_Original_Pulse is 0, this bit determines the maximum pulsewidth of the
 * UPDATE and PFI5/UPDATE signals (so that the pulsewidth is equal to the shorter of this
 * pulsewidth and the original signal pulsewidth). The pulsewidths are as follows:
 *      - 0:    1–1.5 AO_OUT_TIMEBASE periods.
 *      - 1:    3–3.5 AO_OUT_TIMEBASE periods.
 *      .
 */
typedef enum {
        AO_UPDATE_PULSE_WIDTH_ABOUT_1_TIMEBASE_PERIODS          = 0,
        AO_UPDATE_PULSE_WIDTH_ABOUT_3_TIMEBASE_PERIODS          = 1,
} ao_update_pulse_width_t;


/**
 * This bit determines how the pulsewidth of the UPDATE and PFI5/UPDATE signal is
 * selected:
 *      - 0:    Selected by AO_UPDATE_Pulse_Width.
 *      - 1:    Selected by AO_UPDATE_Original_Pulse.
 *      .
 */
typedef enum {
        AO_UPDATE_PULSE_TIMEBASE_SELECT_BY_PULSE_WIDTH          = 0,
        AO_UPDATE_PULSE_TIMEBASE_SELECT_BY_ORIGINAL_PULSE       = 1,
} ao_update_pulse_timebase;


// NOT used
typedef enum {
        kAO_UPDATE_Original_PulseMax_Of_UPDATE_Pulsewidth         = 0,
        kAO_UPDATE_Original_PulseEqual_To_UPDATE_Pulsewidth       = 1,
} tAO_UPDATE_Original_Pulse;


/**
 * This bit selects the pulsewidth of the TMRDACWR, CPUDACWR, and DACWR<0..1>
 * signals:
 *      - 0:    3 AO_OUT_TIMEBASE periods.
 *      - 1:    2 AO_OUT_TIMEBASE periods.
 *      .
 */
typedef enum {
        AO_TMRDACWR_PULSE_WIDTH_ABOUT_3_TIMEBASE_PERIODS          = 0,
        AO_TMRDACWR_PULSE_WIDTH_ABOUT_2_TIMEBASE_PERIODS          = 1,
} ao_tmrdacwr_pulse_width_t;


/**
 * This bit selects the DAC mode:
 *      - 0:    Dual-DAC mode.
 *      - 1:    Single-DAC mode.
 *      .
 * The pins DACWR<0..1> pulse on each TMRDACWR and CPUDACWR. In the dual-DAC
 * mode, DACWR0 pulses on every write and DACWR1 is not used. In the single-DAC mode,
 * DACWR0 pulses when a write occurs to an even channel and DACWR1 pulses when a write
 * occurs to an odd channel. If you are using the DAQ-STC on a device with two DACs in
 * individual packages, set this bit to 1. When you make this selection, you can use pins
 * DACWR0 and DACWR1. In all other cases, set this bit to 0. When you choose this option,
 * you should use pin DACWR0 only.
 */
typedef enum {
        AO_NUMBER_OF_DAC_PACKAGES_DUAL_DAC_MODE     = 0,
        AO_NUMBER_OF_DAC_PACKAGES_SINGLE_DAC_MODE   = 1,
} ao_number_of_dac_packages_t;


typedef enum {
        kPulse             = 0,
        kPFI0              = 1,
        kPFI1              = 2,
        kPFI2              = 3,
        kPFI3              = 4,
        kPFI4              = 5,
        kPFI5              = 6,
        kPFI6              = 7,
        kPFI7              = 8,
        kPFI8              = 9,
        kPFI9              = 10,
        kRTSI0             = 11,
        kRTSI1             = 12,
        kRTSI2             = 13,
        kRTSI3             = 14,
        kRTSI4             = 15,
        kRTSI5             = 16,
        kRTSI6             = 17,
        kLow               = 31,
} tDefault;

typedef enum {
        kAO_START_PolarityActive_High_Or_Rising_Edge            = 0,
        kAO_START_PolarityActive_Low_Or_Falling_Edge            = 1,
        kAO_START_PolarityActive_High       = 0,
        kAO_START_PolarityActive_Low        = 1,
        kAO_START_PolarityRising_Edge       = 0,
        kAO_START_PolarityFalling_Edge      = 1,
} tAO_START_Polarity;

/* class tAO_Trigger_Select : public tReg16IODirect32 */


/**
 *This bitfield selects the START1 trigger:
 *      - 0:     Bitfield AO_START1_Pulse.
 *      - 1–10:  PFI<0..9>.
 *      - 11–17: RTSI_TRIGGER<0..6>.
 *      - 18:    The internal analog input signal START2.
 *      - 19:    The internal analog input signal START1.
 *      - 20:    Star Trigger.
 *      - 21-26: PFI<10..15>.
 *      - 27:    RTSI_TRIGGER7.
 *      - 30:    ANALOG_TRIGGER.
 *      - 31:    Logic low.
 *      .
 */
typedef enum {
        AO_START1_SELECT_PULSE             = 0,
        AO_START1_SELECT_PFI0              = 1,
        AO_START1_SELECT_PFI1              = 2,
        AO_START1_SELECT_PFI2              = 3,
        AO_START1_SELECT_PFI3              = 4,
        AO_START1_SELECT_PFI4              = 5,
        AO_START1_SELECT_PFI5              = 6,
        AO_START1_SELECT_PFI6              = 7,
        AO_START1_SELECT_PFI7              = 8,
        AO_START1_SELECT_PFI8              = 9,
        AO_START1_SELECT_PFI9              = 10,
        AO_START1_SELECT_RTSI0             = 11,
        AO_START1_SELECT_RTSI1             = 12,
        AO_START1_SELECT_RTSI2             = 13,
        AO_START1_SELECT_RTSI3             = 14,
        AO_START1_SELECT_RTSI4             = 15,
        AO_START1_SELECT_RTSI5             = 16,
        AO_START1_SELECT_RTSI6             = 17,
        AO_START1_SELECT_AI_START_2        = 18,
        AO_START1_SELECT_AI_START_1        = 19,
        AO_START1_SELECT_STAR_TRIGGER      = 20,
        AO_START1_SELECT_PFI10             = 21,
        AO_START1_SELECT_PFI11             = 22,
        AO_START1_SELECT_PFI12             = 23,
        AO_START1_SELECT_PFI13             = 24,
        AO_START1_SELECT_PFI14             = 25,
        AO_START1_SELECT_PFI15             = 26,
        AO_START1_SELECT_RTSI7             = 27,
        AO_START1_SELECT_ANALOG_TRIGGER    = 30,
        AO_START1_SELECT_LOW               = 31,
} ao_start1_select_t;


/**
 * This bit determines the polarity of START1 trigger:
 *      - 0:    Active high or rising edge.
 *      - 1:    Active low or falling edge.
 *      .
 * Set this bit to 0 if AO_START1_Select is set to 0.
 */
typedef enum {
        AO_START1_POLARITY_ACTIVE_HIGH       = 0,
        AO_START1_POLARITY_RISING_EDGE       = 0,
        AO_START1_POLARITY_ACTIVE_LOW        = 1,
        AO_START1_POLARITY_FALLING_EDGE      = 1,
} ao_start1_polarity_t;


/**
 * This bit selects the update mode for the LDACi output signals:
 *      - 0:    Immediate update mode. LDACi outputs an inverted version of the DAC write
 *              signals (TMRDACWR and CPUDACWR).
 *      - 1:    Timed update mode. LDACi outputs the UPDATE or UPDATE2 signal.
 *              See AO_LDACi _Source_Select.
 *      .
 */
typedef enum {
        AO_UPDATE_MODE_IMMEDIATE         = 0,
        AO_UPDATE_MODE_TIMED             = 1,
} ao_update_mode_t;


/**
 * TODO think of something
 */
typedef enum {
        AO_DAC_POLARITY_UNIPOLAR          = 0,
        AO_DAC_POLARITY_BIPOLAR           = 1,
} ao_dac_polarity_t;


/**
 * Select which set of samples is associated with AO channel
 *      - 0:    Select first order of samples.
 *      - 1:    Select second order of samples.
 *      - 2:    Select third order of samples.
 *      - 3:    Select fourth order of samples.
 *      - 0xF:  Deselect any order (static signal generation).
 *      .
 * Select 0xF when generating static signals.
 */
typedef enum {
        AO_WAVEFORM_ORDER_0             = 0,
        AO_WAVEFORM_ORDER_1             = 1,
        AO_WAVEFORM_ORDER_2             = 2,
        AO_WAVEFORM_ORDER_3             = 3,
        AO_WAVEFORM_ORDER_F             = 0xF,
} ao_waveform_order_t;


/**
 * Select type of signal to generate.
 */
typedef enum {
        AO_SIGNAL_GENERATION_STATIC     = 0,
        AO_SIGNAL_GENERATION_WAVEFORM   = 1,
} ao_signal_generation_t;


/**
 * This bit enables the local buffer mode:
 *       - 0:   Disabled.
 *       - 1:   Enabled.
 *       .
 * In the local buffer mode, the contents of the data FIFO are regenerated when the FIFO
 * empties. The AOTM accomplishes this by pulsing the AOFFRT signal when the FIFO empty
 * condition is indicated the AOFEF. You can use the local buffer mode when the FIFO is large
 * enough to hold the whole waveform to be generated and the waveform does not vary in time.
 */
typedef enum {
        AO_FIFO_RETRANSMIT_DISABLE      = 0,
        AO_FIFO_RETRANSMIT_ENABLE       = 1,
} ao_fifo_retransmit_t;


/**
 * Output direction of PFI terminal:
 *      - 0: Input.
 *      - 1: Output.
 *      .
 */
typedef enum {
        PFI_PIN_DIR_INPUT             = 0,
        PFI_PIN_DIR_OUTPUT            = 1,
} pfi_pin_dir_t;


/**
 * RTSI Board Interface output options.
 */
typedef enum {
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI0                      = 0,
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI1                      = 1,
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI2                      = 2,
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI3                      = 3,
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI4                      = 4,
        RTSI_SHARED_MUX_OUTPUT_SELECT_PFI5                      = 5,
        RTSI_SHARED_MUX_OUTPUT_SELECT_AO_GATE_RTSI              = 6,
        RTSI_SHARED_MUX_OUTPUT_SELECT_AI_GATE_RTSI              = 7,
        RTSI_SHARED_MUX_OUTPUT_SELECT_FREQ_OUT                  = 8,
        RTSI_SHARED_MUX_OUTPUT_SELECT_G1_OUT                    = 9,
        RTSI_SHARED_MUX_OUTPUT_SELECT_G1_SELECTED_GATE          = 10,
        RTSI_SHARED_MUX_OUTPUT_SELECT_G1_SELECTED_SOURCE        = 11,
        RTSI_SHARED_MUX_OUTPUT_SELECT_G1_Z                      = 12,
        RTSI_SHARED_MUX_OUTPUT_SELECT_G0_Z                      = 13,
        RTSI_SHARED_MUX_OUTPUT_SELECT_ANALOG_TRIGGER            = 14,
        RTSI_SHARED_MUX_OUTPUT_SELECT_AI_START_PULSE            = 15,
} rtsi_shared_mux_output_select_t;


/* class tRTSI_Trig_A_Output : public tReg16IODirect32 */


/**
 * RTSI terminal output options:
 *      - 0:     Internal START1 Signal without
 *               Master/Slave Synchronization.
 *      - 1:     START2 without Master/Slave Synchronization.
 *      - 2:     Internal Sample Clock.
 *      - 3:     DAC Update.
 *      - 4:     START1 without Master/Slave Sync.
 *      - 5:     G_SRC_0.
 *      - 6:     G_GATE_0.
 *      - 7:     RGOUT0.
 *      - 8-11:  RTSI Board Interface.
 *      - 12:    RTSI Oscillator Source.
 *      .
 */
typedef enum {
        RTSI_OUTPUT_SELECT_ADR_START1        = 0,
        RTSI_OUTPUT_SELECT_ADR_START2        = 1,
        RTSI_OUTPUT_SELECT_SCLKG             = 2,
        RTSI_OUTPUT_SELECT_DACUPDN           = 3,
        RTSI_OUTPUT_SELECT_DA_START1         = 4,
        RTSI_OUTPUT_SELECT_G_SRC_0           = 5,
        RTSI_OUTPUT_SELECT_G_GATE_0          = 6,
        RTSI_OUTPUT_SELECT_RGOUT0            = 7,
        RTSI_OUTPUT_SELECT_RTSI_BRD_0        = 8,
        RTSI_OUTPUT_SELECT_RTSI_BRD_1        = 9,
        RTSI_OUTPUT_SELECT_RTSI_BRD_2        = 10,
        RTSI_OUTPUT_SELECT_RTSI_BRD_3        = 11,
        RTSI_OUTPUT_SELECT_RTSI_OSC          = 12,
} rtsi_output_select_t;



typedef enum {
        kRTSI_Sub_Selection_1G_OUT_0                    = 0,
        kRTSI_Sub_Selection_1G_OUT_0_RTSI_IO_Pin        = 1,
} tRTSI_Sub_Selection_1;


/**
 * Output direction of RTSI terminal:
 *      - 0: Input.
 *      - 1: Output.
 *      .
 */
typedef enum {
        RTSI0_PIN_DIR_INPUT             = 0,
        RTSI0_PIN_DIR_OUTPUT            = 1,
} rtsi_pin_dir_t;


typedef enum {
        kSCXI_HW_Serial_Timebase100kHz            = 0,
        kSCXI_HW_Serial_Timebase1MHz              = 1,
} tSCXI_HW_Serial_Timebase;


/**
 * This bit selects the of active edge of the CONVERT source (the signal that is selected by
 * AI_CONVERT_Source_Select):
 *      - 0: Falling edge.
 *      - 1: Rising edge.
 *      .
 * You must set this bit to 0 in the internal CONVERT mode.
 */
typedef enum {
        AI_CONVERT_POLARITY_FALLING_EDGE      = 0,
        AI_CONVERT_POLARITY_RISING_EDGE       = 1,
} ai_convert_polarity_t;


/**
 * Setting this bit to 1 disarms the SC, SI, SI2, and DIV counters at the next SC_TC. You can
 * use this bit to stop the acquisition in continuous acquisition mode. This bit is cleared
 * automatically.
 */
typedef enum {
        AI_END_ON_SC_DISABLE      = 0,
        AI_END_ON_SC_ENABLE       = 1,
} ai_end_on_sc_t;


/**
 * Setting this bit to 1 disarms the SC, SI, SI2, and DIV counters at the next STOP. You can use
 * this bit to stop the acquisition in continuous acquisition mode. This bit is cleared
 * automatically.
 */
typedef enum {
        AI_END_ON_END_OF_SCAN_DISABLE      = 0,
        AI_END_ON_END_OF_SCAN_ENABLE       = 1,
} ai_end_on_end_of_scan_t;


/**
 * This bitfield selects the SI source:
 *      - 0:            AI_IN_TIMEBASE1
 *      - 1–10:         PFI<0..9>
 *      - 11–17:        RTSI_TRIGGER<0..6>
 *      - 19:           IN_TIMEBASE2
 *      - 20:           Star Trigger
 *      - 21-26:        PFI<10..15>
 *      - 27:           RTSI_TRIGGER7
 *      - 28:           INTIMEBASE1
 *      - 29:           PXI_CLK10
 *      - 30:           ANALOG_TRIGGER
 *      - 31:           Logic low
 *      .
 */
typedef enum {
        AI_SI_SELECT_GROUND            = 0,
        AI_SI_SELECT_PFI0              = 1,
        AI_SI_SELECT_PFI1              = 2,
        AI_SI_SELECT_PFI2              = 3,
        AI_SI_SELECT_PFI3              = 4,
        AI_SI_SELECT_PFI4              = 5,
        AI_SI_SELECT_PFI5              = 6,
        AI_SI_SELECT_PFI6              = 7,
        AI_SI_SELECT_PFI7              = 8,
        AI_SI_SELECT_PFI8              = 9,
        AI_SI_SELECT_PFI9              = 10,
        AI_SI_SELECT_RTSI0             = 11,
        AI_SI_SELECT_RTSI1             = 12,
        AI_SI_SELECT_RTSI2             = 13,
        AI_SI_SELECT_RTSI3             = 14,
        AI_SI_SELECT_RTSI4             = 15,
        AI_SI_SELECT_RTSI5             = 16,
        AI_SI_SELECT_RTSI6             = 17,
        AI_SI_SELECT_INTIMEBASE2       = 19,
        AI_SI_SELECT_STAR_TRIGGER      = 20,
        AI_SI_SELECT_PFI10             = 21,
        AI_SI_SELECT_PFI11             = 22,
        AI_SI_SELECT_PFI12             = 23,
        AI_SI_SELECT_PFI13             = 24,
        AI_SI_SELECT_PFI14             = 25,
        AI_SI_SELECT_PFI15             = 26,
        AI_SI_SELECT_RTSI7             = 27,
        AI_SI_SELECT_INTIMEBASE1       = 28,
        AI_SI_SELECT_PXI_CLK10         = 29,
        AI_SI_SELECT_ANALOG_TRIGGER    = 30,
        AI_SI_SELECT_LOW               = 31,
} ai_si_select_t;


/**
 * This bitfield selects the CONVERT source:
 *      - 0:            The internal signal SI2_TC, inverted.
 *      - 1–10:         PFI<0..9>
 *      - 11–17:        RTSI_TRIGGER<0..6>.
 *      - 19:           The internal signal GOUT from general-purpose counter 0.
 *      - 20:           Star Trigger.
 *      - 21-26:        PFI<10..15>.
 *      - 27:           RTSI_TRIGGER7.
 *      - 30:           ANALOG_TRIGGER.
 *      - 31:           Logic low.
 *      .
 * When you set this bit to 0, the PXI6259 card is in the internal CONVERT mode. When you select
 * any other signal as the CONVERT source, the PXI6259 card is in the external CONVERT mode.
 */
typedef enum {
        AI_CONVERT_SELECT_SI2TC             = 0,
        AI_CONVERT_SELECT_PFI0              = 1,
        AI_CONVERT_SELECT_PFI1              = 2,
        AI_CONVERT_SELECT_PFI2              = 3,
        AI_CONVERT_SELECT_PFI3              = 4,
        AI_CONVERT_SELECT_PFI4              = 5,
        AI_CONVERT_SELECT_PFI5              = 6,
        AI_CONVERT_SELECT_PFI6              = 7,
        AI_CONVERT_SELECT_PFI7              = 8,
        AI_CONVERT_SELECT_PFI8              = 9,
        AI_CONVERT_SELECT_PFI9              = 10,
        AI_CONVERT_SELECT_RTSI0             = 11,
        AI_CONVERT_SELECT_RTSI1             = 12,
        AI_CONVERT_SELECT_RTSI2             = 13,
        AI_CONVERT_SELECT_RTSI3             = 14,
        AI_CONVERT_SELECT_RTSI4             = 15,
        AI_CONVERT_SELECT_RTSI5             = 16,
        AI_CONVERT_SELECT_RTSI6             = 17,
        AI_CONVERT_SELECT_GPCRT1_OUT        = 18,
        AI_CONVERT_SELECT_GPCRT0_OUT        = 19,
        AI_CONVERT_SELECT_STAR_TRIGGER      = 20,
        AI_CONVERT_SELECT_PFI10             = 21,
        AI_CONVERT_SELECT_PFI11             = 22,
        AI_CONVERT_SELECT_PFI12             = 23,
        AI_CONVERT_SELECT_PFI13             = 24,
        AI_CONVERT_SELECT_PFI14             = 25,
        AI_CONVERT_SELECT_PFI15             = 26,
        AI_CONVERT_SELECT_RTSI7             = 27,
        AI_CONVERT_SELECT_ANALOG_TRIGGER    = 30,
        AI_CONVERT_SELECT_LOW               = 31,
} ai_convert_select_t;


typedef enum {
        kSCXI_Dedicated_Output_EnableInput             = 0,
        kSCXI_Dedicated_Output_EnableOutput            = 1,
} tSCXI_Dedicated_Output_Enable;

typedef enum {
        kSCXI_Trig1_Output_EnableInput             = 0,
        kSCXI_Trig1_Output_EnableOutput            = 1,
} tSCXI_Trig1_Output_Enable;

typedef enum {
        kSCXI_Trig0_Output_EnableInput             = 0,
        kSCXI_Trig0_Output_EnableOutput            = 1,
} tSCXI_Trig0_Output_Enable;

typedef enum {
        kSCXI_Trig1_Output_SelectPFI0              = 0,
        kSCXI_Trig1_Output_SelectPFI1              = 1,
        kSCXI_Trig1_Output_SelectPFI2              = 2,
        kSCXI_Trig1_Output_SelectPFI3              = 3,
        kSCXI_Trig1_Output_SelectPFI4              = 4,
        kSCXI_Trig1_Output_SelectPFI5              = 5,
        kSCXI_Trig1_Output_SelectPFI6              = 6,
        kSCXI_Trig1_Output_SelectPFI7              = 7,
        kSCXI_Trig1_Output_SelectPFI8              = 8,
        kSCXI_Trig1_Output_SelectPFI9              = 9,
        kSCXI_Trig1_Output_SelectPFI10             = 10,
        kSCXI_Trig1_Output_SelectPFI11             = 11,
        kSCXI_Trig1_Output_SelectPFI12             = 12,
        kSCXI_Trig1_Output_SelectPFI13             = 13,
        kSCXI_Trig1_Output_SelectPFI14             = 14,
        kSCXI_Trig1_Output_SelectPFI15             = 15,
        kSCXI_Trig1_Output_SelectRTSI0             = 16,
        kSCXI_Trig1_Output_SelectRTSI1             = 17,
        kSCXI_Trig1_Output_SelectRTSI2             = 18,
        kSCXI_Trig1_Output_SelectRTSI3             = 19,
        kSCXI_Trig1_Output_SelectRTSI4             = 20,
        kSCXI_Trig1_Output_SelectRTSI5             = 21,
        kSCXI_Trig1_Output_SelectRTSI6             = 22,
        kSCXI_Trig1_Output_SelectRTSI7             = 23,
        kSCXI_Trig1_Output_SelectAI_Start          = 24,
        kSCXI_Trig1_Output_SelectLow               = 31,
} tSCXI_Trig1_Output_Select;

/* class tSCXI_DIO_Enable : public tReg8IODirect32 */
typedef enum {
        kSCXI_DIO_MOSI_EnableInput             = 0,
        kSCXI_DIO_MOSI_EnableOutput            = 1,
} tSCXI_DIO_MOSI_Enable;

typedef enum {
        kSCXI_DIO_D_A_EnableInput             = 0,
        kSCXI_DIO_D_A_EnableOutput            = 1,
} tSCXI_DIO_D_A_Enable;

typedef enum {
        kSCXI_DIO_Intr_EnableInput             = 0,
        kSCXI_DIO_Intr_EnableOutput            = 1,
} tSCXI_DIO_Intr_Enable;

/* class tAnalog_Trigger_Control : public tReg8IODirect32 */

typedef enum {
        kAnalog_Trigger_SelectAI_Chan           = 0,
        kAnalog_Trigger_SelectAPFI0             = 1,
        kAnalog_Trigger_SelectAPFI1             = 2,
        kAnalog_Trigger_SelectGround            = 3,
} tAnalog_Trigger_Select;


/**
 * AI can differ several different channels:
 *      - 0: Calibration
 *      - 1: Differential
 *      - 2: Non-Referenced Single-Ended (NRSE)
 *      - 3: Referenced Single-Ended (RSE)
 *      - 4: AUX
 *      - 5: GHOST
 *      .
 */
typedef enum {
        AI_CHANNEL_TYPE_CALIBRATION       = 0,
        AI_CHANNEL_TYPE_DIFFERENTIAL      = 1,
        AI_CHANNEL_TYPE_NRSE              = 2,
        AI_CHANNEL_TYPE_RSE               = 3,
        AI_CHANNEL_TYPE_AUX               = 5,
        AI_CHANNEL_TYPE_GHOST             = 7,
} ai_channel_type_t;


/**
 * This bit define polarity of an AI channel
 *      - 0: Bipolar.
 *      - 1: Unipolar.
 *      .
 */
typedef enum {
        AI_POLARITY_BIPOLAR           = 0,
        AI_POLARITY_UNIPOLAR          = 1,
} ai_polarity_t;


typedef enum {
        kIntExtTrigTRIG1             = 0,
        kIntExtTrigPGIA2             = 1,
} tIntExtTrig;


/**
 * This bit arms the SC counter. The counter remains armed (and the bit remains set) until it is
 * disarmed, either by hardware or by setting AI_Disarm to 1
 */
typedef enum {
        kAI_Arm_SC_Disable            = 0,
        kAI_Arm_SC_Enable             = 1,
} tAI_ARM_SC;


/**
 * Setting this bit to 1 arms the SI counter. The counter remains armed (and the bit remains set)
 * until it is disarmed, either by hardware or by setting AI_Disarm to 1.
 */
typedef enum {
        kAI_Arm_SI_Disable            = 0,
        kAI_Arm_SI_Enable             = 1,
} tAI_ARM_SI;


/**
 * Setting this bit to 1 arms the SI2 counter. The counter remains armed and the bit remains set
 * until it is disarmed, either by hardware or by setting AI_Disarm to 1
 */
typedef enum {
        kAI_Arm_SI2_Disable           = 0,
        kAI_Arm_SI2_Enable            = 1,
} tAI_ARM_SI2;


/**
 * Available output possibilities for PFI terminals.
 */
typedef enum
{
        PFI_DEFAULT_OUT                 = 0,
        AI_START1_PFI                   = 1,
        AI_START2_PFI                   = 2,
        AI_CONVERT                      = 3,
        G1_SELECTED_SRC                 = 4,
        G1_SELECTED_GATE                = 5,
        AO_UPDATE_N                     = 6,
        AO_START1_PFI                   = 7,
        AI_START_PULSE                  = 8,
        G0_SELECTED_SRC                 = 9,
        G0_SELECTED_GATE                = 10,
        EXT_STROBE                      = 11,
        AI_EXTERNAL_MUX_CLK             = 12,
        G0_OUT                          = 13,
        G1_OUT                          = 14,
        FREQ_OUT                        = 15,
        PFI_DO                          = 16,
        I_ATRIG                         = 17,
        RTSI_PIN0                       = 18,
        RTSI_PIN1                       = 19,
        RTSI_PIN2                       = 20,
        RTSI_PIN3                       = 21,
        RTSI_PIN4                       = 22,
        RTSI_PIN5                       = 23,
        RTSI_PIN6                       = 24,
        RTSI_PIN7                       = 25,
        Star_TRIG_IN                    = 26,
        SCXI_TRIG1                      = 27,
        DIO_CHANGE_DETECT_RTSI  = 28,
        CDI_SAMPLE                      = 29,
        CDO_UPDATE                      = 30,
} pfi_output_select_t;


/* class tPFI_Filter : public tReg32IODirect32 */
typedef enum {
        kPFI_Filter_SelectNoFilter              = 0,
        kPFI_Filter_SelectSmallFilter           = 1,
        kPFI_Filter_SelectMediumFilter          = 2,
        kPFI_Filter_SelectLargeFilter           = 3,
        kPFI_Filter_SelectIllegalFilterSetting  = 4,
} tPFI_Filter_Select;


/* class tRTSI_Filter : public tReg32IODirect32 */
typedef enum {
        kRTSI_Filter_SelectNoFilter                     = 0,
        kRTSI_Filter_SelectSmallFilter                  = 1,
        kRTSI_Filter_SelectMediumFilter                 = 2,
        kRTSI_Filter_SelectLargeFilter                  = 3,
        kRTSI_Filter_SelectIllegalFilterSetting     = 4,
} tRTSI_Filter_Select;


/* class tClock_And_Fout2 : public tReg16IODirect32 */
typedef enum {
        kPLL_In_Source_SelectOld_RTSI7         = 0,
        kPLL_In_Source_SelectRTSI0             = 11,
        kPLL_In_Source_SelectRTSI1             = 12,
        kPLL_In_Source_SelectRTSI2             = 13,
        kPLL_In_Source_SelectRTSI3             = 14,
        kPLL_In_Source_SelectRTSI4             = 15,
        kPLL_In_Source_SelectRTSI5             = 16,
        kPLL_In_Source_SelectRTSI6             = 17,
        kPLL_In_Source_SelectStar_Trigger      = 20,
        kPLL_In_Source_SelectRTSI7             = 27,
        kPLL_In_Source_SelectPXI_Clock10       = 29,
} tPLL_In_Source_Select;


typedef enum {
        kTB1_SelectSelect_OSC        = 0,
        kTB1_SelectSelect_PLL        = 1,
} tTB1_Select;


typedef enum {
        kTB3_SelectSelect_OSC        = 0,
        kTB3_SelectSelect_PLL        = 1,
} tTB3_Select;


typedef enum {
        kRTSI_Clock_Freq20MHz        = 0,
        kRTSI_Clock_Freq10MHz        = 1,
} tRTSI_Clock_Freq;


typedef enum {
        kCDI_Sample_Source_SelectGround            = 0,
        kCDI_Sample_Source_SelectPFI0              = 1,
        kCDI_Sample_Source_SelectPFI1              = 2,
        kCDI_Sample_Source_SelectPFI2              = 3,
        kCDI_Sample_Source_SelectPFI3              = 4,
        kCDI_Sample_Source_SelectPFI4              = 5,
        kCDI_Sample_Source_SelectPFI5              = 6,
        kCDI_Sample_Source_SelectPFI6              = 7,
        kCDI_Sample_Source_SelectPFI7              = 8,
        kCDI_Sample_Source_SelectPFI8              = 9,
        kCDI_Sample_Source_SelectPFI9              = 10,
        kCDI_Sample_Source_SelectRTSI0             = 11,
        kCDI_Sample_Source_SelectRTSI1             = 12,
        kCDI_Sample_Source_SelectRTSI2             = 13,
        kCDI_Sample_Source_SelectRTSI3             = 14,
        kCDI_Sample_Source_SelectRTSI4             = 15,
        kCDI_Sample_Source_SelectRTSI5             = 16,
        kCDI_Sample_Source_SelectRTSI6             = 17,
        kCDI_Sample_Source_SelectAI_Start          = 18,
        kCDI_Sample_Source_SelectAI_Convert        = 19,
        kCDI_Sample_Source_SelectPXI_Star_Trigger  = 20,
        kCDI_Sample_Source_SelectPFI10             = 21,
        kCDI_Sample_Source_SelectPFI11             = 22,
        kCDI_Sample_Source_SelectPFI12             = 23,
        kCDI_Sample_Source_SelectPFI13             = 24,
        kCDI_Sample_Source_SelectPFI14             = 25,
        kCDI_Sample_Source_SelectPFI15             = 26,
        kCDI_Sample_Source_SelectRTSI7             = 27,
        kCDI_Sample_Source_SelectG0_Out            = 28,
        kCDI_Sample_Source_SelectG1_Out            = 29,
        kCDI_Sample_Source_SelectAnalog_Trigger    = 30,
        kCDI_Sample_Source_SelectAO_Update         = 31,
        kCDI_Sample_Source_SelectFreq_Out          = 32,
        kCDI_Sample_Source_SelectDIO_Change_Detect_Irq       = 33,
} tCDI_Sample_Source_Select;


typedef enum {
        kCDI_PolarityRising            = 0,
        kCDI_PolarityFalling           = 1,
} tCDI_Polarity;


typedef enum {
        kCDI_FIFO_ModeFIFO_Not_Empty    = 0,
        kCDI_FIFO_ModeFIFO_Half_Full    = 1,
} tCDI_FIFO_Mode;


typedef enum {
        kCDI_Data_LaneCDIO_15_0         = 0,
        kCDI_Data_LaneCDIO_31_16        = 1,
        kCDI_Data_LaneCDIO_7_0          = 0,
        kCDI_Data_LaneCDIO_15_8         = 1,
        kCDI_Data_LaneCDIO_23_16        = 2,
        kCDI_Data_LaneCDIO_31_24        = 3,
} tCDI_Data_Lane;


typedef enum {
        kCDO_Update_Source_SelectGround            = 0,
        kCDO_Update_Source_SelectPFI0              = 1,
        kCDO_Update_Source_SelectPFI1              = 2,
        kCDO_Update_Source_SelectPFI2              = 3,
        kCDO_Update_Source_SelectPFI3              = 4,
        kCDO_Update_Source_SelectPFI4              = 5,
        kCDO_Update_Source_SelectPFI5              = 6,
        kCDO_Update_Source_SelectPFI6              = 7,
        kCDO_Update_Source_SelectPFI7              = 8,
        kCDO_Update_Source_SelectPFI8              = 9,
        kCDO_Update_Source_SelectPFI9              = 10,
        kCDO_Update_Source_SelectRTSI0             = 11,
        kCDO_Update_Source_SelectRTSI1             = 12,
        kCDO_Update_Source_SelectRTSI2             = 13,
        kCDO_Update_Source_SelectRTSI3             = 14,
        kCDO_Update_Source_SelectRTSI4             = 15,
        kCDO_Update_Source_SelectRTSI5             = 16,
        kCDO_Update_Source_SelectRTSI6             = 17,
        kCDO_Update_Source_SelectAI_Start          = 18,
        kCDO_Update_Source_SelectAI_Convert        = 19,
        kCDO_Update_Source_SelectPXI_Star_Trigger  = 20,
        kCDO_Update_Source_SelectPFI10             = 21,
        kCDO_Update_Source_SelectPFI11             = 22,
        kCDO_Update_Source_SelectPFI12             = 23,
        kCDO_Update_Source_SelectPFI13             = 24,
        kCDO_Update_Source_SelectPFI14             = 25,
        kCDO_Update_Source_SelectPFI15             = 26,
        kCDO_Update_Source_SelectRTSI7             = 27,
        kCDO_Update_Source_SelectG0_Out            = 28,
        kCDO_Update_Source_SelectG1_Out            = 29,
        kCDO_Update_Source_SelectAnalog_Trigger    = 30,
        kCDO_Update_Source_SelectAO_Update         = 31,
        kCDO_Update_Source_SelectFreq_Out          = 32,
        kCDO_Update_Source_SelectDIO_Change_Detect_Irq       = 33,
} tCDO_Update_Source_Select;


typedef enum {
        kCDO_PolarityRising            = 0,
        kCDO_PolarityFalling           = 1,
} tCDO_Polarity;


typedef enum {
        kCDO_FIFO_ModeFIFO_Not_Full     = 0,
        kCDO_FIFO_ModeFIFO_Half_Full    = 1,
} tCDO_FIFO_Mode;


typedef enum {
        kCDO_Data_LaneCDIO_15_0         = 0,
        kCDO_Data_LaneCDIO_31_16        = 1,
        kCDO_Data_LaneCDIO_7_0          = 0,
        kCDO_Data_LaneCDIO_15_8         = 1,
        kCDO_Data_LaneCDIO_23_16        = 2,
        kCDO_Data_LaneCDIO_31_24        = 3,
} tCDO_Data_Lane;


typedef enum {
        kSCXI_Force_AI_EXTMUX_CLK_Width250ns             = 0,
        kSCXI_Force_AI_EXTMUX_CLK_Width500ns             = 1,
} tSCXI_Force_AI_EXTMUX_CLK_Width;


/*
 * DIO port selection
 *      - 0:    Port 0 - <0.0 .. 0.31>
 *      - 1:    Port 1 - <1.0 .. 1.7> - <PFI0..PFI7>
 *      - 2:    Port 2 - <2.0 .. 2.7> - <PFI7..PFI15>
 */
typedef enum {
        DIO_PORT_SELECT_PORT0           = 0,
        DIO_PORT_SELECT_PORT1           = 1,
        DIO_PORT_SELECT_PORT2           = 2,
} dio_port_select_t;


typedef enum {
        SCALE_CONVERTER_SELECT_AI       = 0,
        SCALE_CONVERTER_SELECT_AO0      = 0,
        SCALE_CONVERTER_SELECT_AO1      = 1,
        SCALE_CONVERTER_SELECT_AO2      = 2,
        SCALE_CONVERTER_SELECT_AO3      = 3,
} scale_converter_select_t;


#endif /* PXI6259ENUMS_H_ */
