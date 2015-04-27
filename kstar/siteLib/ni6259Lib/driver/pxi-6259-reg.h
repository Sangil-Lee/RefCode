/*****************************************************************************
 * pxi-6259-reg.h
 *
 * Register definitions for the NI PXI-6259 driver.
 * Author: Janez Golob (Cosylab)
 * Copyright (C) 2010-2013 ITER Organization
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file COPYING in the main directory of the Linux
 * distribution for more details.
 ****************************************************************************/

#ifndef _PXI6682_REG_H
#define _PXI6682_REG_H

#define BAR0    0
#define BAR1    1
#define BAR2    2
#define BAR3    3
#define BAR4    4
#define BAR5    5

#define _bit31          0x80000000
#define _bit30          0x40000000
#define _bit29          0x20000000
#define _bit28          0x10000000
#define _bit27          0x08000000
#define _bit26          0x04000000
#define _bit25          0x02000000
#define _bit24          0x01000000
#define _bit23          0x00800000
#define _bit22          0x00400000
#define _bit21          0x00200000
#define _bit20          0x00100000
#define _bit19          0x00080000
#define _bit18          0x00040000
#define _bit17          0x00020000
#define _bit16          0x00010000
#define _bit15          0x00008000
#define _bit14          0x00004000
#define _bit13          0x00002000
#define _bit12          0x00001000
#define _bit11          0x00000800
#define _bit10          0x00000400
#define _bit9           0x00000200
#define _bit8           0x00000100
#define _bit7           0x00000080
#define _bit6           0x00000040
#define _bit5           0x00000020
#define _bit4           0x00000010
#define _bit3           0x00000008
#define _bit2           0x00000004
#define _bit1           0x00000002
#define _bit0           0x00000001

#define NUM_PFI_OUTPUT_SELECT_REGS 6

/* Registers in the National Instruments DAQ-STC chip */

//#define Interrupt_A_Ack_Register      2
#define G0_Gate_Interrupt_Ack                   _bit15
#define G0_TC_Interrupt_Ack                     _bit14
#define AI_Error_Interrupt_Ack                  _bit13
#define AI_STOP_Interrupt_Ack                   _bit12
#define AI_START_Interrupt_Ack                  _bit11
#define AI_START2_Interrupt_Ack                 _bit10
#define AI_START1_Interrupt_Ack                 _bit9
#define AI_SC_TC_Interrupt_Ack                  _bit8
#define AI_SC_TC_Error_Confirm                  _bit7
#define G0_TC_Error_Confirm                     _bit6
#define G0_Gate_Error_Confirm                   _bit5

//#define AI_Status_1_Register          2
#define Interrupt_A_St                          0x8000
#define AI_FIFO_Full_St                         0x4000
#define AI_FIFO_Half_Full_St                    0x2000
#define AI_FIFO_Empty_St                        0x1000
#define AI_Overrun_St                           0x0800
#define AI_Overflow_St                          0x0400
#define AI_SC_TC_Error_St                       0x0200
#define AI_START2_St                            0x0100
#define AI_START1_St                            0x0080
#define AI_SC_TC_St                             0x0040
#define AI_START_St                             0x0020
#define AI_STOP_St                              0x0010
#define G0_TC_St                                0x0008
#define G0_Gate_Interrupt_St                    0x0004
#define AI_FIFO_Request_St                      0x0002
#define Pass_Thru_0_Interrupt_St                0x0001

//#define AI_Status_2_Register          5


//#define Interrupt_B_Ack_Register      3
#define G1_Gate_Error_Confirm                   _bit1
#define G1_TC_Error_Confirm                     _bit2
#define AO_BC_TC_Trigger_Error_Confirm          _bit3
#define AO_BC_TC_Error_Confirm                  _bit4
#define AO_UI2_TC_Error_Confrim                 _bit5
#define AO_UI2_TC_Interrupt_Ack                 _bit6
#define AO_UC_TC_Interrupt_Ack                  _bit7
#define AO_BC_TC_Interrupt_Ack                  _bit8
#define AO_START1_Interrupt_Ack                 _bit9
#define AO_UPDATE_Interrupt_Ack                 _bit10
#define AO_START_Interrupt_Ack                  _bit11
#define AO_STOP_Interrupt_Ack                   _bit12
#define AO_Error_Interrupt_Ack                  _bit13
#define G1_TC_Interrupt_Ack                     _bit14
#define G1_Gate_Interrupt_Ack                   _bit15

//#define AO_Status_1_Register          3
#define Interrupt_B_St                          _bit15
#define AO_FIFO_Full_St                         _bit14
#define AO_FIFO_Half_Full_St                    _bit13
#define AO_FIFO_Empty_St                        _bit12
#define AO_BC_TC_Error_St                       _bit11
#define AO_START_St                             _bit10
#define AO_Overrun_St                           _bit9
#define AO_START1_St                            _bit8
#define AO_BC_TC_St                             _bit7
#define AO_UC_TC_St                             _bit6
#define AO_UPDATE_St                            _bit5
#define AO_UI2_TC_St                            _bit4
#define G1_TC_St                                _bit3
#define G1_Gate_Interrupt_St                    _bit2
#define AO_FIFO_Request_St                      _bit1
#define Pass_Thru_1_Interrupt_St                _bit0

//#define AI_Command_2_Register         4
#define AI_End_On_SC_TC                         _bit15
#define AI_End_On_End_Of_Scan                   _bit14
#define AI_START1_Disable                       _bit11
#define AI_SC_Save_Trace                        _bit10
#define AI_SI_Switch_Load_On_SC_TC              _bit9
#define AI_SI_Switch_Load_On_STOP               _bit8
#define AI_SI_Switch_Load_On_TC                 _bit7
#define AI_SC_Switch_Load_On_TC                 _bit4
#define AI_STOP_Pulse                           _bit3
#define AI_START_Pulse                          _bit2
#define AI_START2_Pulse                         _bit1
#define AI_START1_Pulse                         _bit0

//#define AO_Command_2_Register         5
#define AO_End_On_BC_TC(x)                      (((x) & 0x3) << 14)
#define AO_End_On_BC_TC_Mask                    (0x3 << 14)
#define AO_Start_Stop_Gate_Enable               _bit13
#define AO_UC_Save_Trace                        _bit12
#define AO_BC_Gate_Enable                       _bit11
#define AO_BC_Save_Trace                        _bit10
#define AO_UI_Switch_Load_On_BC_TC              _bit9
#define AO_UI_Switch_Load_On_Stop               _bit8
#define AO_UI_Switch_Load_On_TC                 _bit7
#define AO_UC_Switch_Load_On_BC_TC              _bit6
#define AO_UC_Switch_Load_On_TC                 _bit5
#define AO_BC_Switch_Load_On_TC                 _bit4
#define AO_Mute_B                               _bit3
#define AO_Mute_A                               _bit2
#define AO_UPDATE2_Pulse                        _bit1
#define AO_START1_Pulse                         _bit0

//#define AO_Status_2_Register          6

//#define DIO_Parallel_Input_Register   7

//#define AI_Command_1_Register         8
#define AI_Analog_Trigger_Reset                 _bit14
#define AI_Disarm                               _bit13
#define AI_SI2_Arm                              _bit12
#define AI_SI2_Load                             _bit11
#define AI_SI_Arm                               _bit10
#define AI_SI_Load                              _bit9
#define AI_DIV_Arm                              _bit8
#define AI_DIV_Load                             _bit7
#define AI_SC_Arm                               _bit6
#define AI_SC_Load                              _bit5
#define AI_SCAN_IN_PROG_Pulse                   _bit4
#define AI_EXTMUX_CLK_Pulse                     _bit3
#define AI_LOCALMUX_CLK_Pulse                   _bit2
#define AI_SC_TC_Pulse                          _bit1
#define AI_CONVERT_Pulse                        _bit0

//#define AO_Command_1_Register         9
#define AO_Analog_Trigger_Reset                 _bit15
#define AO_START_Pulse                          _bit14
#define AO_Disarm                               _bit13
#define AO_UI2_Arm_Disarm                       _bit12
#define AO_UI2_Load                             _bit11
#define AO_UI_Arm                               _bit10
#define AO_UI_Load                              _bit9
#define AO_UC_Arm                               _bit8
#define AO_UC_Load                              _bit7
#define AO_BC_Arm                               _bit6
#define AO_BC_Load                              _bit5
#define AO_DAC1_Update_Mode                     _bit4
#define AO_LDAC1_Source_Select                  _bit3
#define AO_DAC0_Update_Mode                     _bit2
#define AO_LDAC0_Source_Select                  _bit1
#define AO_UPDATE_Pulse                         _bit0

//#define DIO_Output_Register           10
#define DIO_Parallel_Data_Out(a)                ((a)&0xff)
#define DIO_Parallel_Data_Mask                  0xff
#define DIO_SDOUT                               _bit0
#define DIO_SDIN                                _bit4
#define DIO_Serial_Data_Out(a)                  (((a)&0xff)<<8)
#define DIO_Serial_Data_Mask                    0xff00

//#define DIO_Control_Register          11
#define DIO_Software_Serial_Control             _bit11
#define DIO_HW_Serial_Timebase                  _bit10
#define DIO_HW_Serial_Enable                    _bit9
#define DIO_HW_Serial_Start                     _bit8
#define DIO_Pins_Dir(a)                         ((a)&0xff)
#define DIO_Pins_Dir_Mask                       0xff

//#define AI_Mode_1_Register            12
#define AI_CONVERT_Source_Select(a)             (((a) & 0x1f) << 11)
#define AI_CONVERT_Source_Select_Mask           (0x1f << 11)
#define AI_SI_Source_select(a)                  (((a) & 0x1f) << 6)
#define AI_SI_Source_select_Mask                (0x1f << 6)
#define AI_CONVERT_Source_Polarity              _bit5
#define AI_SI_Source_Polarity                   _bit4
#define AI_Start_Stop                           _bit3
#define AI_Mode_1_Reserved                      _bit2
#define AI_Continuous                           _bit1
#define AI_Trigger_Once                         _bit0

//#define AI_Mode_2_Register            13
#define AI_SC_Gate_Enable                       _bit15
#define AI_Start_Stop_Gate_Enable               _bit14
#define AI_Pre_Trigger                          _bit13
#define AI_External_MUX_Present                 _bit12
#define AI_SI2_Initial_Load_Source              _bit9
#define AI_SI2_Reload_Mode                      _bit8
#define AI_SI_Initial_Load_Source               _bit7
#define AI_SI_Reload_Mode(a)                    (((a) & 0x7)<<4)
#define AI_SI_Reload_Mode_Mask                  (0x7<<4)
#define AI_SI_Write_Switch                      _bit3
#define AI_SC_Initial_Load_Source               _bit2
#define AI_SC_Reload_Mode                       _bit1
#define AI_SC_Write_Switch                      _bit0

//#define AI_SI_Load_A_Registers                14
//#define AI_SI_Load_B_Registers                16
//#define AI_SC_Load_A_Registers                18
//#define AI_SC_Load_B_Registers                20
//#define AI_SI_Save_Registers          64
//#define AI_SC_Save_Registers          66

//#define AI_SI2_Load_A_Register                23
//#define AI_SI2_Load_B_Register                25

//#define Joint_Status_1_Register         27
#define DIO_Serial_IO_In_Progress_St            _bit12

//#define DIO_Serial_Input_Register       28

//#define Joint_Status_2_Register         29
#define AO_TMRDACWRs_In_Progress_St             _bit5

//#define AO_Mode_1_Register            38
#define AO_UPDATE_Source_Select(x)              (((x)&0x1f)<<11)
#define AO_UPDATE_Source_Select_Mask            (0x1f<<11)
#define AO_UI_Source_Select(x)                  (((x)&0x1f)<<6)
#define AO_UI_Source_Select_Mask                (0x1f<<6)
#define AO_Multiple_Channels                    _bit5
#define AO_UPDATE_Source_Polarity               _bit4
#define AO_UI_Source_Polarity                   _bit3
#define AO_UC_Switch_Load_Every_TC              _bit2
#define AO_Continuous                           _bit1
#define AO_Trigger_Once                         _bit0

//#define AO_Mode_2_Register            39
#define AO_FIFO_Mode(x)                        (((x) & 0x3) << 14)
#define AO_FIFO_Mode_Mask                       (0x3 << 14)
#define AO_FIFO_Retransmit_Enable               _bit13
#define AO_START1_Disable                       _bit12
#define AO_UC_Initial_Load_Source               _bit11
#define AO_UC_Write_Switch                      _bit10
#define AO_UI2_Initial_Load_Source              _bit9
#define AO_UI2_Reload_Mode                      _bit8
#define AO_UI_Initial_Load_Source               _bit7
#define AO_UI_Reload_Mode(x)                    (((x) & 0x7) << 4)
#define AO_UI_Reload_Mode_Mask                  (0x7 << 4)
#define AO_UI_Write_Switch                      _bit3
#define AO_BC_Initial_Load_Source               _bit2
#define AO_BC_Reload_Mode                       _bit1
#define AO_BC_Write_Switch                      _bit0

// AO_FIFO_Clear
#define AO_FIFO_Clear                           _bit0

//#define AO_UI_Load_A_Register         40
//#define AO_UI_Load_A_Register_High    40
//#define AO_UI_Load_A_Register_Low     41
//#define AO_UI_Load_B_Register         42
//#define AO_UI_Save_Registers          16
//#define AO_BC_Load_A_Register         44
//#define AO_BC_Load_A_Register_High    44
//#define AO_BC_Load_A_Register_Low     45
//#define AO_BC_Load_B_Register         46
//#define AO_BC_Load_B_Register_High    46
//#define AO_BC_Load_B_Register_Low     47
//#define AO_BC_Save_Registers          18
//#define AO_UC_Load_A_Register         48
//#define AO_UC_Load_A_Register_High    48
//#define AO_UC_Load_A_Register_Low     49
//#define AO_UC_Load_B_Register         50
//#define AO_UC_Save_Registers          20

//define Clock_and_Fout2_Register

#define RTSI_Clock_Freq         _bit7
#define TB3_Select                      _bit6
#define TB1_Select                      _bit5
#define PLL_In_Source_Select(x) ((x) & 0x1f)
#define PLL_In_Source_Select_Mask       (0x1f)


//define Clock_and_FOUT_Register                56
enum Clock_and_FOUT_bits {
        FOUT_Enable = _bit15,
        FOUT_Timebase_Select = _bit14,
        DIO_Serial_Out_Divide_By_2 = _bit13,
        Slow_Internal_Time_Divide_By_2 = _bit12,
        Slow_Internal_Timebase = _bit11,
        G_Source_Divide_By_2 = _bit10,
        Clock_To_Board_Divide_By_2 = _bit9,
        Clock_To_Board = _bit8,
        AI_Output_Divide_By_2 = _bit7,
        AI_Source_Divide_By_2 = _bit6,
        AO_Output_Divide_By_2 = _bit5,
        AO_Source_Divide_By_2 = _bit4,
        FOUT_Divider_mask = 0xf
};
static inline unsigned FOUT_Divider(unsigned divider)
{
        return (divider & FOUT_Divider_mask);
}

//define IO_Bidirection_Pin_Register    57
//define        RTSI_Trig_Direction_Register    58

#define RTSI_Pin_Dir(chan)      (1 << (8 + ((chan) & 0x7)))
#define RTSI_Pin_Dir_Mask(chan) (1 << (8 + ((chan) & 0x7)))

//define Interrupt_Control_Register     59
#define Interrupt_B_Enable                      _bit15
#define Interrupt_B_Output_Select(x)            ((x)<<12)
#define Interrupt_B_Output_Select_Mask          (0x7<<12)
#define Interrupt_A_Enable                      _bit11
#define Interrupt_A_Output_Select(x)            ((x)<<8)
#define Interrupt_A_Output_Select_Mask          (0x7<<8)
#define Pass_Thru_0_Interrupt_Polarity          _bit3
#define Pass_Thru_1_Interrupt_Polarity          _bit2
#define Interrupt_Output_On_3_Pins              _bit1
#define Interrupt_Output_Polarity               _bit0

//define AI_Output_Control_Register     60
#define AI_START_Output_Select                  _bit10
#define AI_SCAN_IN_PROG_Output_Select(x)        (((x) & 0x3) << 8)
#define AI_SCAN_IN_PROG_Output_Select_Mask      (0x3 << 8)
#define AI_EXTMUX_CLK_Output_Select(x)          (((x) & 0x3) << 6)
#define AI_EXTMUX_CLK_Output_Select_Mask        (0x3 << 6)
#define AI_LOCALMUX_CLK_Output_Select(x)        ((x)<<4)
#define AI_LOCALMUX_CLK_Output_Select_Mask      (0x3<<4)
#define AI_SC_TC_Output_Select(x)               ((x)<<2)
#define AI_SC_TC_Output_Select_Mask             (0x3<<2)
#define AI_CONVERT_Output_Select(sel)           ((sel) & 0x3)
#define AI_CONVERT_Output_Select_Mask           (0x3)

//#define AI_START_STOP_Select_Register 62
#define AI_START_Polarity                       _bit15
#define AI_STOP_Polarity                        _bit14
#define AI_STOP_Sync                            _bit13
#define AI_STOP_Edge                            _bit12
#define AI_STOP_Select(a)                       (((a) & 0x1f)<<7)
#define AI_STOP_Select_Mask                     (0x1f<<7)
#define AI_START_Sync                           _bit6
#define AI_START_Edge                           _bit5
#define AI_START_Select(a)                      ((a) & 0x1f)
#define AI_START_Select_Mask                    (0x1f)

//#define AI_Trigger_Select_Register    63
#define AI_START1_Polarity                      _bit15
#define AI_START2_Polarity                      _bit14
#define AI_START2_Sync                          _bit13
#define AI_START2_Edge                          _bit12
#define AI_START2_Select(a)                     (((a) & 0x1f) << 7)
#define AI_START2_Select_Mask                   (0x1f << 7)
#define AI_START1_Sync                          _bit6
#define AI_START1_Edge                          _bit5
#define AI_START1_Select(a)                     ((a) & 0x1f)
#define AI_START1_Select_Mask                   (0x1f)

//#define AI_DIV_Load_A_Register        64

//#define AO_Start_Select_Register      66
#define AO_UI2_Software_Gate                    _bit15
#define AO_UI2_External_Gate_Polarity           _bit14
#define AO_START_Polarity                       _bit13
#define AO_AOFREQ_Enable                        _bit12
#define AO_UI2_External_Gate_Select(a)          (((a) & 0x1f) << 7)
#define AO_UI2_External_Gate_Select_Mask        (0x1f << 7)
#define AO_START_Sync                           _bit6
#define AO_START_Edge                           _bit5
#define AO_START_Select(a)                      ((a) & 0x1f)
#define AO_START_Select_Mask                    (0x1f)

//#define AO_Trigger_Select_Register    67
#define AO_UI2_External_Gate_Enable             _bit15
#define AO_Delayed_START1                       _bit14
#define AO_START1_Polarity                      _bit13
#define AO_UI2_Source_Polarity                  _bit12
#define AO_UI2_Source_Select(x)                 (((x)&0x1f)<<7)
#define AO_UI2_Source_Select_Mask               (0x1f<<7)
#define AO_START1_Sync                          _bit6
#define AO_START1_Edge                          _bit5
#define AO_START1_Select(x)                     (((x)&0x1f)<<0)
#define AO_START1_Select_Mask                   (0x1f<<0)

//#define AO_Mode_3_Register            70
#define AO_UI2_Switch_Load_Next_TC              _bit13
#define AO_UC_Switch_Load_Every_BC_TC           _bit12
#define AO_Trigger_Length                       _bit11
#define AO_Stop_On_Overrun_Error                _bit5
#define AO_Stop_On_BC_TC_Trigger_Error          _bit4
#define AO_Stop_On_BC_TC_Error                  _bit3
#define AO_Not_An_UPDATE                        _bit2
#define AO_Software_Gate                        _bit1
#define AO_Last_Gate_Disable                    _bit0   /* M Series only */

//#define Joint_Reset_Register          72
#define Software_Reset                          _bit11
#define AO_Configuration_End                    _bit9
#define AI_Configuration_End                    _bit8
#define AO_Configuration_Start                  _bit5
#define AI_Configuration_Start                  _bit4
#define G1_Reset                                _bit3
#define G0_Reset                                _bit2
#define AO_Reset                                _bit1
#define AI_Reset                                _bit0

//#define Interrupt_A_Enable_Register   73
#define Pass_Thru_0_Interrupt_Enable            _bit9
#define G0_Gate_Interrupt_Enable                _bit8
#define AI_FIFO_Interrupt_Enable                _bit7
#define G0_TC_Interrupt_Enable                  _bit6
#define AI_Error_Interrupt_Enable               _bit5
#define AI_STOP_Interrupt_Enable                _bit4
#define AI_START_Interrupt_Enable               _bit3
#define AI_START2_Interrupt_Enable              _bit2
#define AI_START1_Interrupt_Enable              _bit1
#define AI_SC_TC_Interrupt_Enable               _bit0

//#define Interrupt_B_Enable_Register   75
#define Pass_Thru_1_Interrupt_Enable            _bit11
#define G1_Gate_Interrupt_Enable                _bit10
#define G1_TC_Interrupt_Enable                  _bit9
#define AO_FIFO_Interrupt_Enable                _bit8
#define AO_UI2_TC_Interrupt_Enable              _bit7
#define AO_UC_TC_Interrupt_Enable               _bit6
#define AO_Error_Interrupt_Enable               _bit5
#define AO_STOP_Interrupt_Enable                _bit4
#define AO_START_Interrupt_Enable               _bit3
#define AO_UPDATE_Interrupt_Enable              _bit2
#define AO_START1_Interrupt_Enable              _bit1
#define AO_BC_TC_Interrupt_Enable               _bit0

//define Second_IRQ_A_Enable_Register   74
enum Second_IRQ_A_Enable_Bits {
        AI_SC_TC_Second_Irq_Enable = _bit0,
        AI_START1_Second_Irq_Enable = _bit1,
        AI_START2_Second_Irq_Enable = _bit2,
        AI_START_Second_Irq_Enable = _bit3,
        AI_STOP_Second_Irq_Enable = _bit4,
        AI_Error_Second_Irq_Enable = _bit5,
        G0_TC_Second_Irq_Enable = _bit6,
        AI_FIFO_Second_Irq_Enable = _bit7,
        G0_Gate_Second_Irq_Enable = _bit8,
        Pass_Thru_0_Second_Irq_Enable = _bit9
};

//#define Second_IRQ_B_Enable_Register  76
enum Second_IRQ_B_Enable_Bits {
        AO_BC_TC_Second_Irq_Enable = _bit0,
        AO_START1_Second_Irq_Enable = _bit1,
        AO_UPDATE_Second_Irq_Enable = _bit2,
        AO_START_Second_Irq_Enable = _bit3,
        AO_STOP_Second_Irq_Enable = _bit4,
        AO_Error_Second_Irq_Enable = _bit5,
        AO_UC_TC_Second_Irq_Enable = _bit6,
        AO_UI2_TC_Second_Irq_Enable = _bit7,
        AO_FIFO_Second_Irq_Enable = _bit8,
        G1_TC_Second_Irq_Enable = _bit9,
        G1_Gate_Second_Irq_Enable = _bit10,
        Pass_Thru_1_Second_Irq_Enable = _bit11
};

//#define AI_Personal_Register          77
#define AI_SHIFTIN_Pulse_Width                  _bit15
#define AI_EOC_Polarity                         _bit14
#define AI_SOC_Polarity                         _bit13
#define AI_SHIFTIN_Polarity                     _bit12
#define AI_CONVERT_Pulse_Timebase               _bit11
#define AI_CONVERT_Pulse_Width                  _bit10
#define AI_CONVERT_Original_Pulse               _bit9
#define AI_FIFO_Flags_Polarity                  _bit8
#define AI_Overrun_Mode                         _bit7
#define AI_EXTMUX_CLK_Pulse_Width               _bit6
#define AI_LOCALMUX_CLK_Pulse_Width             _bit5
#define AI_AIFREQ_Polarity                      _bit4

//#define AO_Personal_Register                  78
#define AO_Multiple_DACS_Per_Package            _bit15          // m-series only
#define AO_Number_Of_DAC_Packages               _bit14          // 1 for "single" mode, 0 for "dual"
#define AO_TMRDACWR_Pulse_Width                 _bit12
#define AO_FIFO_Enable                          _bit10
#define AO_UPDATE_Original_Pulse                _bit7
#define AO_UPDATE_Pulse_Timebase                _bit6
#define AO_UPDATE_Pulse_Width                   _bit5
#define AO_BC_Source_Select                     _bit4
#define AO_Interval_Buffer_Mode                 _bit3




//#define       RTSI_Trig_A_Output_Register     79
//#define       RTSI_Trig_B_Output_Register     80

#define RTSI_Trig_Output(chan, source)          ((source & 0xf) << ((chan % 4) * 4))
#define RTSI_Trig_Output_Mask(chan)             (0xf << ((chan % 4) * 4))

// inverse to RTSI_Trig_Output()
#define RTSI_Trig_Output_Source(chan, bits)     ((bits >> ((rtsi_channel % 4) * 4)) & 0xf)

//#define       RTSI_Board_Register             81
#define Write_Strobe_0_Register         82
#define Write_Strobe_1_Register         83
#define Write_Strobe_2_Register         84
#define Write_Strobe_3_Register         85

/* AO_Calibration_Register */
#define AO_RefGround                            _bit0

//#define AO_Output_Control_Register    86
#define AO_External_Gate_Enable                 _bit15
#define AO_External_Gate_Select(x)              (((x)&0x1f)<<10)
#define AO_External_Gate_Select_Mask            (0x1f<<10)
#define AO_Number_Of_Channels(x)                (((x)&0xf)<<6)
#define AO_Number_Of_Channels_Mask              (0xf<<6)
#define AO_UPDATE2_Output_Select(x)             (((x)&0x3)<<4)
#define AO_UPDATE2_Output_Select_Mask           (0x3<<4)
#define AO_External_Gate_Polarity               _bit3
#define AO_UPDATE2_Output_Toggle                _bit2
#define AO_UPDATE_Output_Select(x)              (x & 0x3)
#define AO_UPDATE_Output_Select_Mask             (0x3)


//#define AI_Mode_3_Register            87
#define AI_Trigger_Length                       _bit15
#define AI_Delay_START                          _bit14
#define AI_Software_Gate                        _bit13
#define AI_SI_Special_Trigger_Delay             _bit12
#define AI_SI2_Source_Select                    _bit11
#define AI_Delayed_START2                       _bit10
#define AI_Delayed_START1                       _bit9
#define AI_External_Gate_Mode                   _bit8
#define AI_FIFO_Mode_Mask                       (3<<6)
#define AI_FIFO_Mode_HF_to_E                    (3<<6)
#define AI_FIFO_Mode_F                          (2<<6)
#define AI_FIFO_Mode_HF                         (1<<6)
#define AI_FIFO_Mode_NE                         (0<<6)
#define AI_External_Gate_Polarity               _bit5
#define AI_External_Gate_Select(a)              ((a) & 0x1f)
#define AI_External_Gate_Select_Mask            (0x1f)

/*  */
#define G_Autoincrement_Register(a)     (68+(a))
#define G_Command_Register(a)           (6+(a))
#define G_HW_Save_Register(a)           (8+(a)*2)
#define G_HW_Save_Register_High(a)      (8+(a)*2)
#define G_HW_Save_Register_Low(a)       (9+(a)*2)
#define G_Input_Select_Register(a)      (36+(a))
#define G_Load_A_Register(a)            (28+(a)*4)
#define G_Load_A_Register_High(a)       (28+(a)*4)
#define G_Load_A_Register_Low(a)        (29+(a)*4)
#define G_Load_B_Register(a)            (30+(a)*4)
#define G_Load_B_Register_High(a)       (30+(a)*4)
#define G_Load_B_Register_Low(a)        (31+(a)*4)
#define G_Mode_Register(a)              (26+(a))
#define G_Save_Register(a)              (12+(a)*2)
#define G_Save_Register_High(a)         (12+(a)*2)
#define G_Save_Register_Low(a)          (13+(a)*2)
#define G_Status_Register               4

/* command register */
#define G_Disarm_Copy                   _bit15  /* strobe */
#define G_Save_Trace_Copy               _bit14
#define G_Arm_Copy                      _bit13  /* strobe */
#define G_Bank_Switch_Enable            _bit12
#define G_Bank_Switch_Mode              _bit11
#define G_Bank_Switch_Start             _bit10  /* strobe */
#define G_Little_Big_Endian             _bit9
#define G_Synchronized_Gate             _bit8
#define G_Write_Switch                  _bit7
#define G_Up_Down(a)                    (((a)&0x03)<<5)
#define G_Up_Down_Mask                  (0x03<<5)
#define G_Disarm                        _bit4   /* strobe */
#define G_Analog_Trigger_Reset          _bit3   /* strobe */
#define G_Load                          _bit2   /* strobe */
#define G_Save_Trace                    _bit1
#define G_Arm                           _bit0   /* strobe */

/* input select register */
#define G_Gate_Select(a)                (((a)&0x1f)<<7)
#define G_Gate_Select_Mask              (0x1f<<7)
#define G_Source_Select(a)              (((a)&0x1f)<<2)
#define G_Source_Select_Mask            (0x1f<<2)
#define G_Write_Acknowledges_Irq        _bit1
#define G_Read_Acknowledges_Irq         _bit0

/* same input select register, but with channel agnostic names */
#define G_Source_Polarity               _bit15
#define G_Output_Polarity               _bit14
#define G_OR_Gate                       _bit13
#define G_Gate_Select_Load_Source       _bit12

/* mode register */
#define G_Reload_Source_Switching               _bit15
#define G_Loading_On_Gate                       _bit14
#define G_Gate_Polarity                         _bit13
#define G_Loading_On_TC                         _bit12
#define G_Output_Mode(a)                        (((a)&0x03)<<8)
#define G_Output_Mode_Mask                      (0x03<<8)
#define G_Load_Source_Select                    _bit7
#define G_Trigger_Mode_For_Edge_Gate(a)         (((a)&0x03)<<3)
#define G_Trigger_Mode_For_Edge_Gate_Mask       (0x03<<3)
#define G_Gating_Mode(a)                        (((a)&0x03)<<0)
#define G_Gating_Mode_Mask                      (0x03<<0)

/* same input mode register, but with channel agnostic names */


#define G_Counting_Once(a)              (((a)&0x03)<<10)
#define G_Counting_Once_Mask            (0x03<<10)
#define G_Stop_Mode(a)                  (((a)&0x03)<<5)
#define G_Stop_Mode_Mask                (0x03<<5)
#define G_Gate_On_Both_Edges            _bit2

/* G_Status_Register */
#define G1_Gate_Error_St                _bit15
#define G0_Gate_Error_St                _bit14
#define G1_TC_Error_St                  _bit13
#define G0_TC_Error_St                  _bit12
#define G1_No_Load_Between_Gates_St     _bit11
#define G0_No_Load_Between_Gates_St     _bit10
#define G1_Armed_St                     _bit9
#define G0_Armed_St                     _bit8
#define G1_Stale_Data_St                _bit7
#define G0_Stale_Data_St                _bit6
#define G1_Next_Load_Source_St          _bit5
#define G0_Next_Load_Source_St          _bit4
#define G1_Counting_St                  _bit3
#define G0_Counting_St                  _bit2
#define G1_Save_St                      _bit1
#define G0_Save_St                      _bit0

/* general purpose counter timer */
#define G_Autoincrement(a)              ((a)<<0)
#define G_Autoincrement_Mask            (0xFFFF)        // TODO check this

/*Analog_Trigger_Etc_Register*/
#define Analog_Trigger_Mode(x)                  ((x) & 0x7)
#define Analog_Trigger_Mode_Mask                (0x7)
#define Analog_Trigger_Enable                   _bit3
#define Analog_Trigger_Reset                    _bit5
#define Misc_Counter_TCs_Output_Enable  _bit6
#define GPFO_1_Output_Select                    _bit7
#define GPFO_0_Output_Select(a)                 ((a & 0x7)<<11)
#define GPFO_0_Output_Select_Mask               (0x7<<11)
#define GPFO_0_Output_Enable                    _bit14
#define GPFO_1_Output_Enable                    _bit15

/* Gi_DMA_Config_Register */
#define G_DMA_BankSW_Error                      _bit4
#define G_DMA_Reset                             _bit3
#define G_DMA_Int_Enable                        _bit2
#define G_DMA_Write                             _bit1
#define G_DMA_Enable                            _bit0



// AI_Config_FIFO_Data_Register

#define AI_Config_Last_Channel          _bit14
#define AI_Config_Dither                        _bit13
#define AI_Config_Polarity                      _bit12
#define AI_Config_Gain(a)                       ((a & 0x7) << 9)
#define AI_Config_Gain_Mask                     ((0x7) << 9)
#define AI_Config_Channel_Type(a)       ((a & 0x7) << 6)
#define AI_Config_Channel_Type_Mask     ((0x7) << 6)
#define AI_Config_Bank(a)               ((a & 0x3) << 4)
#define AI_Config_Bank_Mask             ((0x3) << 4)
#define AI_Config_Channel(a)            (a & 0xF)
#define AI_Config_Channel_Mask          (0xF)


//AI_Config_FIFO_Bypass Register
#define AI_Bypass_Config_FIFO           _bit31
#define AI_Bypass_Polarity              _bit22
#define AI_Bypass_Dither                _bit21
#define AI_Bypass_Gain(x)               ((x & 0x3)<<18)
#define AI_Bypass_Gain_Mask             ((0x3)<<18)
#define AO_Bypass_AO_Cal_Sel(x)         ((x & 0x7)<<15)
#define AO_Bypass_AO_Cal_Sel_Mask       ((0x7)<<15)
#define AI_Bypass_Mode_Mux(x)           ((x & 0x3)<<13)
#define AI_Bypass_Mode_Mux_Mask         ((0x3)<<13)
#define AI_Bypass_Cal_Sel_Neg(x)        ((x & 0x7)<<10)
#define AI_Bypass_Cal_Sel_Neg_Mask      ((0x7)<<10)
#define AI_Bypass_Cal_Sel_Pos(x)        ((x & 0x7)<<7)
#define AI_Bypass_Cal_Sel_Pos_Mask      ((0x7)<<7)
#define AI_Bypass_Bank(x)               ((x & 0xF)<<3)
#define AI_Bypass_Bank_Mask             ((0xF)<<3)
#define AI_Bypass_Channel(x)            (x & 0x7)
#define AI_Bypass_Channel_Mask          (0x7)

// IO_Bidirection_Pin_Register
#define PFI_Pin_Dir(chan)               (1<<(chan))
#define PFI_Pin_Dir_Mask(chan)          (1<<(chan))

//PFI_Output_Select_1_Register
#define PFI_Output_Select(chan, sel)    (((sel) & 0x1f) << (((chan) % 3) * 5))
#define PFI_Output_Select_Mask(chan)    ((0x1f) << (((chan) % 3) * 5))


// PLL_Control
#define PLL_VCO_Mode(a)                 (((a) & 0x3) << 13)
#define PLL_VCO_Mode_Mask               ((0x3) << 13)

#define PLL_Enable                      _bit12
#define PLL_Divisor(a)                  (((a) & 0xf) << 8)
#define PLL_Divisor_Mask                ((0xf) << 8)
#define PLL_Multiplier(a)               ((a) & 0x7f)
#define PLL_Multiplier_Mask             (0x7f)


// Analog_Trigger_Control
#define Analog_Trigger_Select(x)        ((x) & 0x3)
#define Analog_Trigger_Select_Mask      (0x3)


#define RTSI_Shared_MUX_Output_Select(chan, sel)  (((sel) & 0xf) << (((chan) % 4) * 4))
#define RTSI_Shared_MUX_Output_Select_Mask(chan)  ((0xf) << ((chan % 4) * 4))


/* DIO section */

/* DIO pin direction */
#define DIO_Pin_Dir(chan)               (1 << (chan))
#define DIO_Pin_Dir_Mask(chan)          (1 << (chan))


/* AO_Config_Bank_i_Register            133..137 */
#define AO_DAC_Offset_Select(x)         ((x) & 0x7)
#define AO_DAC_Offset_Select_Mask       (0x7)
#define AO_DAC_Reference_Select(x)      (((x) & 0x7) << 3)
#define AO_DAC_Reference_Select_Mask    ((0x7) << 3)
#define AO_Update_Mode                  _bit6
#define AO_DAC_Polarity                 _bit7


#define MAX_M_OFFSET 0x500

enum pxi6259_register {
        CDIO_DMA_Select_Register,       // pxi6259_write
        SCXI_Status_Register,   // pxi6259_read
        AI_AO_Select_Register,  // pxi6259_write, same offset as e-series
        SCXI_Serial_Data_In_Register,   // pxi6259_read
        G0_G1_Select_Register,  // pxi6259_write, same offset as e-series
        Misc_Command_Register,
        SCXI_Serial_Data_Out_Register,
        SCXI_Control_Register,
        SCXI_Output_Enable_Register,
        AI_FIFO_Data_Register,
        Static_Digital_Output_Register, // pxi6259_write
        Static_Digital_Input_Register,  // pxi6259_read
        DIO_Direction_Register,
        Cal_PWM_Register,
        AI_Config_FIFO_Data_Register, // 0x5e,
        Interrupt_C_Enable_Register, // 0x88,   // pxi6259_write
        Interrupt_C_Status_Register, // 0x88,   // pxi6259_read
        Analog_Trigger_Control_Register, // 0x8c,
        AO_Serial_Interrupt_Enable_Register, // 0xa0,
        AO_Serial_Interrupt_Ack_Register, // 0xa1,      // pxi6259_write
        AO_Serial_Interrupt_Status_Register, // 0xa1,   // pxi6259_read
        AO_Calibration_Register, // 0xa3,
        AO_FIFO_Data_Register, // 0xa4,
        PFI_Filter_Register, // 0xb0,
        RTSI_Filter_Register, // 0xb4,
        SCXI_Legacy_Compatibility_Register, // 0xbc,
        Interrupt_A_Ack_Register, // 0x104,     // pxi6259_write
        AI_Status_1_Register, // 0x104, // pxi6259_read
        Interrupt_B_Ack_Register, // 0x106,     // pxi6259_write
        AO_Status_1_Register, // 0x106, // pxi6259_read
        AI_Command_2_Register, // 0x108,        // pxi6259_write
        G01_Status_Register, // 0x108,  // pxi6259_read
        AO_Command_2_Register, // 0x10a,
        AO_Status_2_Register, // 0x10c, // pxi6259_read
        G0_Command_Register, // 0x10c,  // pxi6259_write
        G1_Command_Register, // 0x10e,  // pxi6259_write
        G0_HW_Save_Register, // 0x110,
        G0_HW_Save_High_Register, // 0x110,
        AI_Command_1_Register, // 0x110,
        G0_HW_Save_Low_Register, // 0x112,
        AO_Command_1_Register, // 0x112,
        G1_HW_Save_Register, // 0x114,
        G1_HW_Save_High_Register, // 0x114,
        G1_HW_Save_Low_Register, // 0x116,
        AI_Mode_1_Register, // 0x118,
        G0_Save_Register, // 0x118,
        G0_Save_High_Register, // 0x118,
        AI_Mode_2_Register, // 0x11a,
        G0_Save_Low_Register, // 0x11a,
        AI_SI_Load_A_Register, // 0x11c,
        G1_Save_Register, // 0x11c,
        G1_Save_High_Register, // 0x11c,
        G1_Save_Low_Register, // 0x11e,
        AI_SI_Load_B_Register, // 0x120,        // pxi6259_write
        AO_UI_Save_Register, // 0x120,  // pxi6259_read
        AI_SC_Load_A_Register, // 0x124,        // pxi6259_write
        AO_BC_Save_Register, // 0x124,  // pxi6259_read
        AI_SC_Load_B_Register, // 0x128,        // pxi6259_write
        AO_UC_Save_Register, // 0x128,  //pxi6259_read
        AI_SI2_Load_A_Register, // 0x12c,
        AI_SI2_Load_B_Register, // 0x130,
        G0_Mode_Register, // 0x134,
        G1_Mode_Register, // 0x136,     // pxi6259_write
        Joint_Status_1_Register, // 0x136,      // pxi6259_read
        G0_Load_A_Register, // 0x138,
        Joint_Status_2_Register, // 0x13a,
        G0_Load_B_Register, // 0x13c,
        G1_Load_A_Register, // 0x140,
        G1_Load_B_Register, // 0x144,
        G0_Input_Select_Register, // 0x148,
        G1_Input_Select_Register, // 0x14a,
        AO_Mode_1_Register, // 0x14c,
        AO_Mode_2_Register, // 0x14e,
        AO_UI_Load_A_Register, // 0x150,
        AO_UI_Load_B_Register, // 0x154,
        AO_BC_Load_A_Register, // 0x158,
        AO_BC_Load_B_Register, // 0x15c,
        AO_UC_Load_A_Register, // 0x160,
        AO_UC_Load_B_Register, // 0x164,
        Clock_and_FOUT_Register, // 0x170,
        IO_Bidirection_Pin_Register, // 0x172,
        RTSI_Trig_Direction_Register, // 0x174,
        Interrupt_Control_Register, // 0x176,
        AI_Output_Control_Register, // 0x178,
        Analog_Trigger_Etc_Register, // 0x17a,
        AI_START_STOP_Select_Register, // 0x17c,
        AI_Trigger_Select_Register, // 0x17e,
        AI_SI_Save_Register, // 0x180,  // pxi6259_read
        AI_DIV_Load_A_Register, // 0x180,       // pxi6259_write
        AI_SC_Save_Register, // 0x184,  // pxi6259_read
        AO_Start_Select_Register, // 0x184,     // pxi6259_write
        AO_Trigger_Select_Register, // 0x186,
        AO_Mode_3_Register, // 0x18c,
        G0_Autoincrement_Register, // 0x188,
        G1_Autoincrement_Register, // 0x18a,
        Joint_Reset_Register, // 0x190,
        Interrupt_A_Enable_Register, // 0x192,
        Interrupt_B_Enable_Register, // 0x196,
        AI_Personal_Register, // 0x19a,
        AO_Personal_Register, // 0x19c,
        RTSI_Trig_A_Output_Register, // 0x19e,
        RTSI_Trig_B_Output_Register, // 0x1a0,
        RTSI_Shared_MUX_Register, // 0x1a2,
        AO_Output_Control_Register, // 0x1ac,
        AI_Mode_3_Register, // 0x1ae,
        Configuration_Memory_Clear_Register, // 0x1a4,
        AI_FIFO_Clear_Register, // 0x1a6,
        AO_FIFO_Clear_Register, // 0x1a8,
        G0_Counting_Mode_Register, // 0x1b0,
        G1_Counting_Mode_Register, // 0x1b2,
        G0_Second_Gate_Register, // 0x1b4,
        G1_Second_Gate_Register, // 0x1b6,
        G0_DMA_Config_Register, // 0x1b8,       // pxi6259_write
        G0_DMA_Status_Register, // 0x1b8,       // pxi6259_read
        G1_DMA_Config_Register, // 0x1ba,       // pxi6259_write
        G1_DMA_Status_Register, // 0x1ba,       // pxi6259_read
        G0_MSeries_ABZ_Register, // 0x1c0,
        G1_MSeries_ABZ_Register, // 0x1c2,
        Clock_and_Fout2_Register, // 0x1c4,
        PLL_Control_Register, // 0x1c6,
        PLL_Status_Register, // 0x1c8,
        PFI_Output_Select_1_Register, // 0x1d0,
        PFI_Output_Select_2_Register, // 0x1d2,
        PFI_Output_Select_3_Register, // 0x1d4,
        PFI_Output_Select_4_Register, // 0x1d6,
        PFI_Output_Select_5_Register, // 0x1d8,
        PFI_Output_Select_6_Register, // 0x1da,
        PFI_DI_Register, // 0x1dc,
        PFI_DO_Register, // 0x1de,
        AI_Config_FIFO_Bypass_Register, // 0x218,
        SCXI_DIO_Enable_Register, // 0x21c,
        CDI_FIFO_Data_Register, // 0x220,       // pxi6259_read
        CDO_FIFO_Data_Register, // 0x220,       // pxi6259_write
        CDIO_Status_Register, // 0x224, // pxi6259_read
        CDIO_Command_Register, // 0x224,        // pxi6259_write
        CDI_Mode_Register, // 0x228,
        CDO_Mode_Register, // 0x22c,
        CDI_Mask_Enable_Register, // 0x230,
        CDO_Mask_Enable_Register, // 0x234,
        Static_AI_Control_0_Register, // 0x64,
        Static_AI_Control_1_Register, // 0x261,
        Static_AI_Control_2_Register, // 0x262,
        Static_AI_Control_3_Register, // 0x263,
        Static_AI_Control_4_Register, // 0x264,
        Static_AI_Control_5_Register, // 0x265,
        Static_AI_Control_6_Register, // 0x266,
        Static_AI_Control_7_Register, // 0x267,
        Gen_PWM_0_Register, // 0x44,
        Gen_PWM_1_Register, // 0x46,
        Gen_PWM_2_Register, // 0x48,
        Gen_PWM_3_Register, // 0x4a,
        Gen_PWM_4_Register, // 0x4c,
        Gen_PWM_5_Register, // 0x4e,
        Gen_PWM_6_Register, // 0x50,
        Gen_PWM_7_Register, // 0x52,
        AO_Config_Bank_0_Register, // 0xc3,
        AO_Config_Bank_1_Register, // 0xc7,
        AO_Config_Bank_2_Register, // 0xcb,
        AO_Config_Bank_3_Register, // 0xcf,
        AO_Waveform_Order_0_Register, // 0xc2
        AO_Waveform_Order_1_Register, // 0xc6
        AO_Waveform_Order_2_Register, // 0xca
        AO_Waveform_Order_3_Register, // 0xce
        AO_Waveform_Order_4_Register, // 0xd2
        AO_Waveform_Order_5_Register, // 0xd6
        AO_Waveform_Order_6_Register, // 0xda
        AO_Waveform_Order_7_Register, // 0xde
        AO_Waveform_Order_8_Register, // 0xe2
        AO_Waveform_Order_9_Register, // 0xe6
        AO_Waveform_Order_10_Register, // 0xea
        AO_Waveform_Order_11_Register, // 0xee
        AO_Waveform_Order_12_Register, // 0xf2
        AO_Waveform_Order_13_Register, // 0xf6
        AO_Waveform_Order_14_Register, // 0xfa
        AO_Waveform_Order_15_Register, // 0xfe
        DAC_Direct_Data_0_Register, // 0xc0
        DAC_Direct_Data_1_Register, // 0xc4
        DAC_Direct_Data_2_Register, // 0xc8
        DAC_Direct_Data_3_Register, // 0xcc
        DAC_Direct_Data_4_Register, // 0xd0
        DAC_Direct_Data_5_Register, // 0xd4
        DAC_Direct_Data_6_Register, // 0xd8
        DAC_Direct_Data_7_Register, // 0xdc
        DAC_Direct_Data_8_Register, // 0xe0
        DAC_Direct_Data_9_Register, // 0xe4
        DAC_Direct_Data_10_Register, // 0xe8
        DAC_Direct_Data_11_Register, // 0xec
        DAC_Direct_Data_12_Register, // 0xf0
        DAC_Direct_Data_13_Register, // 0xf4
        DAC_Direct_Data_14_Register, // 0xf8
        DAC_Direct_Data_15_Register, // 0xfc
};

/* TODO change masks */
static uint32_t flush_mask [] = {
                /*CDIO_DMA_Select */ 0xffffffff,        // pxi6259_write
                /*SCXI_Status */ 0xffffffff,    // pxi6259_read
                /*AI_AO_Select */ 0xffffffff,   // pxi6259_write, same offset as e-series
                /*SCXI_Serial_Data_In */0xffffffff,     // pxi6259_read
                /*G0_G1_Select */ 0xffffffff,   // pxi6259_write, same offset as e-series
                /*Misc_Command */ 0xffffffff,
                /*SCXI_Serial_Data_Out */ 0xffffffff,
                /*SCXI_Control */ 0xffffffff,
                /*SCXI_Output_Enable */ 0xffffffff,
                /*AI_FIFO_Data */ 0xffffffff,
                /*Static_Digital_Output */ 0xffffffff,  // pxi6259_write
                /*Static_Digital_Input */ 0xffffffff,   // pxi6259_read
                /*DIO_Direction */ 0xffffffff,
                /*Cal_PWM */ 0xffffffff,
                /*AI_Config_FIFO_Data */ 0xffffffff,
                /*Interrupt_C_Enable */ 0xffffffff,     // pxi6259_write
                /*Interrupt_C_Status */ 0xffffffff,     // pxi6259_read
                /*Analog_Trigger_Control */ 0xffffffff,
                /*AO_Serial_Interrupt_Enable */ 0xffffffff,
                /*AO_Serial_Interrupt_Ack */ 0xffffffff,        // pxi6259_write
                /*AO_Serial_Interrupt_Status */ 0xffffffff,     // pxi6259_read
                /*AO_Calibration */ 0xffffffff,
                /*AO_FIFO_Data */ 0xffffffff,
                /*PFI_Filter */ 0xffffffff,
                /*RTSI_Filter */ 0xffffffff,
                /*SCXI_Legacy_Compatibility */ 0xffffffff,
                /*Interrupt_A_Ack */ 0xffff001f,        // pxi6259_write
                /*AI_Status_1 */ 0xffffffff,    // pxi6259_read
                /*Interrupt_B_Ack */ 0xffffffff,        // pxi6259_write
                /*AO_Status_1 */ 0xffffffff,    // pxi6259_read
                /*AI_Command_2 */ 0xffff3c00,   // pxi6259_write
                /*G01_Status */ 0xffffffff,     // pxi6259_read
                /*AO_Command_2 */ 0xffffffff,
                /*AO_Status_2 */ 0xffffffff,    // pxi6259_read
                /*G0_Command */ 0xffffffff,     // pxi6259_write
                /*G1_Command */ 0xffffffff,
                /*G0_HW_Save */ 0xffffffff,
                /*G0_HW_Save_High */ 0xffffffff,
                /*AI_Command_1 */ 0xffff0000,
                /*G0_HW_Save_Low */ 0xffffffff,
                /*AO_Command_1 */ 0xffff181e,
                /*G1_HW_Save */ 0xffffffff,
                /*G1_HW_Save_High */ 0xffffffff,
                /*G1_HW_Save_Low */ 0xffffffff,
                /*AI_Mode_1 */ 0xffffffff,
                /*G0_Save */ 0xffffffff,
                /*G0_Save_High */ 0xffffffff,
                /*AI_Mode_2 */ 0xffffffff,
                /*G0_Save_Low */ 0xffffffff,
                /*AI_SI_Load_A */ 0xffffffff,
                /*G1_Save */ 0xffffffff,
                /*G1_Save_High */ 0xffffffff,
                /*G1_Save_Low */ 0xffffffff,
                /*AI_SI_Load_B */ 0xffffffff,   // pxi6259_write
                /*AO_UI_Save */ 0xffffffff,     // pxi6259_read
                /*AI_SC_Load_A */ 0xffffffff,   // pxi6259_write
                /*AO_BC_Save */ 0xffffffff,     // pxi6259_read
                /*AI_SC_Load_B */ 0xffffffff,   // pxi6259_write
                /*AO_UC_Save */ 0xffffffff,     //pxi6259_read
                /*AI_SI2_Load_A */ 0xffffffff,
                /*AI_SI2_Load_B */ 0xffffffff,
                /*G0_Mode */ 0xffffffff,
                /*G1_Mode */ 0xffffffff,        // pxi6259_write
                /*Joint_Status_1 */ 0xffffffff, // pxi6259_read
                /*G0_Load_A */ 0xffffffff,
                /*Joint_Status_2 */ 0xffffffff,
                /*G0_Load_B */ 0xffffffff,
                /*G1_Load_A */ 0xffffffff,
                /*G1_Load_B */ 0xffffffff,
                /*G0_Input_Select */ 0xffffffff,
                /*G1_Input_Select */ 0xffffffff,
                /*AO_Mode_1 */ 0xffffffff,
                /*AO_Mode_2 */ 0xffffffff,
                /*AO_UI_Load_A */ 0xffffffff,
                /*AO_UI_Load_B */ 0xffffffff,
                /*AO_BC_Load_A */ 0xffffffff,
                /*AO_BC_Load_B */ 0xffffffff,
                /*AO_UC_Load_A */ 0xffffffff,
                /*AO_UC_Load_B */ 0xffffffff,
                /*Clock_and_FOUT */ 0xffffffff,
                /*IO_Bidirection_Pin */ 0xffffffff,
                /*RTSI_Trig_Direction */ 0xffffffff,
                /*Interrupt_Control */ 0xffffffff,
                /*AI_Output_Control */ 0xffffffff,
                /*Analog_Trigger_Etc */ 0xffffffdf,
                /*AI_START_STOP_Select */ 0xffffffff,
                /*AI_Trigger_Select */ 0xffffffff,
                /*AI_SI_Save */ 0xffffffff,     // pxi6259_read
                /*AI_DIV_Load_A */ 0xffffffff,  // pxi6259_write
                /*AI_SC_Save */ 0xffffffff,     // pxi6259_read
                /*AO_Start_Select */ 0xffffffff,        // pxi6259_write
                /*AO_Trigger_Select */ 0xffffffff,
                /*AO_Mode_3 */ 0xffffffff,
                /*G0_Autoincrement */ 0xffffffff,
                /*G1_Autoincrement */ 0xffffffff,
                /*Joint_Reset */ 0xfffffcc0,
                /*Interrupt_A_Enable */ 0xffffffff,
                /*Interrupt_B_Enable */ 0xffffffff,
                /*AI_Personal */ 0xffffffff,
                /*AO_Personal */ 0xffffffff,
                /*RTSI_Trig_A_Output */ 0xffffffff,
                /*RTSI_Trig_B_Output */ 0xffffffff,
                /*RTSI_Shared_MUX */ 0xffffffff,
                /*AO_Output_Control */ 0xffffffff,
                /*AI_Mode_3 */ 0xffffffff,
                /*Configuration_Memory_Clear */ 0xfffffffe,
                /*AI_FIFO_Clear */ 0xffffffff,
                /*AO_FIFO_Clear */ 0xffffffff,
                /*G0_Counting_Mode */ 0xffffffff,
                /*G1_Counting_Mode */ 0xffffffff,
                /*G0_Second_Gate */ 0xffffffff,
                /*G1_Second_Gate */ 0xffffffff,
                /*G0_DMA_Config */ 0xffffffff,  // pxi6259_write
                /*G0_DMA_Status */ 0xffffffff,  // pxi6259_read
                /*G1_DMA_Config */ 0xffffffff,  // pxi6259_write
                /*G1_DMA_Status */ 0xffffffff,  // pxi6259_read
                /*G0_MSeries_ABZ */ 0xffffffff,
                /*G1_MSeries_ABZ */ 0xffffffff,
                /*Clock_and_Fout2 */ 0xffffffff,
                /*PLL_Control */ 0xffffffff,
                /*PLL_Status */ 0xffffffff,
                /*PFI_Output_Select_1 */ 0xffffffff,
                /*PFI_Output_Select_2 */ 0xffffffff,
                /*PFI_Output_Select_3 */ 0xffffffff,
                /*PFI_Output_Select_4 */ 0xffffffff,
                /*PFI_Output_Select_5 */ 0xffffffff,
                /*PFI_Output_Select_6 */ 0xffffffff,
                /*PFI_DI */ 0xffffffff,
                /*PFI_DO */ 0xffffffff,
                /*AI_Config_FIFO_Bypass */ 0xffffffff,
                /*SCXI_DIO_Enable */ 0xffffffff,
                /*CDI_FIFO_Data */ 0xffffffff,  // pxi6259_read
                /*CDO_FIFO_Data */ 0xffffffff,  // pxi6259_write
                /*CDIO_Status */ 0xffffffff,    // pxi6259_read
                /*CDIO_Command */ 0xffffffff,   // pxi6259_write
                /*CDI_Mode */ 0xffffffff,
                /*CDO_Mode */ 0xffffffff,
                /*CDI_Mask_Enable */ 0xffffffff,
                /*CDO_Mask_Enable */ 0xffffffff,
                /*M_Offset_Static_AI_Control_0_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_1_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_2_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_3_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_4_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_5_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_6_Register */ 0xffffffff,
                /*M_Offset_Static_AI_Control_7_Register */ 0xffffffff,
                /*Gen_PWM 0*/ 0xffffffff,
                /*Gen_PWM 1*/ 0xffffffff,
                /*Gen_PWM 2*/ 0xffffffff,
                /*Gen_PWM 3*/ 0xffffffff,
                /*Gen_PWM 4*/ 0xffffffff,
                /*Gen_PWM 5*/ 0xffffffff,
                /*Gen_PWM 6*/ 0xffffffff,
                /*Gen_PWM 7*/ 0xffffffff,
                /*AO_Config_Bank_0*/ 0xff,
                /*AO_Config_Bank_1*/ 0xff,
                /*AO_Config_Bank_2*/ 0xff,
                /*AO_Config_Bank_3*/ 0xff,
                /*AO_Waveform_Order_0*/ 0xff,
                /*AO_Waveform_Order_1*/ 0xff,
                /*AO_Waveform_Order_2*/ 0xff,
                /*AO_Waveform_Order_3*/ 0xff,
                /*AO_Waveform_Order_4*/ 0xff,
                /*AO_Waveform_Order_5*/ 0xff,
                /*AO_Waveform_Order_6*/ 0xff,
                /*AO_Waveform_Order_7*/ 0xff,
                /*AO_Waveform_Order_8*/ 0xff,
                /*AO_Waveform_Order_9*/ 0xff,
                /*AO_Waveform_Order_10*/ 0xff,
                /*AO_Waveform_Order_11*/ 0xff,
                /*AO_Waveform_Order_12*/ 0xff,
                /*AO_Waveform_Order_12*/ 0xff,
                /*AO_Waveform_Order_14*/ 0xff,
                /*AO_Waveform_Order_15*/ 0xff,
                /*DAC_Direct_Data_0_Register*/ 0xffffffff,
                /*DAC_Direct_Data_1_Register*/ 0xffffffff,
                /*DAC_Direct_Data_2_Register*/ 0xffffffff,
                /*DAC_Direct_Data_3_Register*/ 0xffffffff,
                /*DAC_Direct_Data_4_Register*/ 0xffffffff,
                /*DAC_Direct_Data_5_Register*/ 0xffffffff,
                /*DAC_Direct_Data_6_Register*/ 0xffffffff,
                /*DAC_Direct_Data_7_Register*/ 0xffffffff,
                /*DAC_Direct_Data_8_Register*/ 0xffffffff,
                /*DAC_Direct_Data_9_Register*/ 0xffffffff,
                /*DAC_Direct_Data_10_Register*/ 0xffffffff,
                /*DAC_Direct_Data_11_Register*/ 0xffffffff,
                /*DAC_Direct_Data_12_Register*/ 0xffffffff,
                /*DAC_Direct_Data_13_Register*/ 0xffffffff,
                /*DAC_Direct_Data_14_Register*/ 0xffffffff,
                /*DAC_Direct_Data_15_Register*/ 0xffffffff,
};

static uint8_t reg_width [] = {
                /*CDIO_DMA_Select */ 8, // pxi6259_write
                /*SCXI_Status */ 8,     // pxi6259_read
                /*AI_AO_Select */ 8,    // pxi6259_write, same offset as e-series
                /*SCXI_Serial_Data_In */8,      // pxi6259_read
                /*G0_G1_Select */ 8,    // pxi6259_write, same offset as e-series
                /*Misc_Command */ 8,
                /*SCXI_Serial_Data_Out */ 8,
                /*SCXI_Control */ 8,
                /*SCXI_Output_Enable */ 8,
                /*AI_FIFO_Data */ 32,
                /*Static_Digital_Output */ 32,  // pxi6259_write
                /*Static_Digital_Input */ 32,   // pxi6259_read
                /*DIO_Direction */ 32,
                /*Cal_PWM */ 32,
                /*AI_Config_FIFO_Data */ 16,
                /*Interrupt_C_Enable */ 8,      // pxi6259_write
                /*Interrupt_C_Status */ 8,      // pxi6259_read
                /*Analog_Trigger_Control */ 8,
                /*AO_Serial_Interrupt_Enable */ 8,
                /*AO_Serial_Interrupt_Ack */ 8, // pxi6259_write
                /*AO_Serial_Interrupt_Status */ 8,      // pxi6259_read
                /*AO_Calibration */ 8,
                /*AO_FIFO_Data */ 32,
                /*PFI_Filter */ 32,
                /*RTSI_Filter */ 32,
                /*SCXI_Legacy_Compatibility */ 8,
                /*Interrupt_A_Ack */ 16,        // pxi6259_write
                /*AI_Status_1 */ 16,    // pxi6259_read
                /*Interrupt_B_Ack */ 16,        // pxi6259_write
                /*AO_Status_1 */ 16,    // pxi6259_read
                /*AI_Command_2 */ 16,   // pxi6259_write
                /*G01_Status */ 16,     // pxi6259_read
                /*AO_Command_2 */ 16,
                /*AO_Status_2 */ 16,    // pxi6259_read
                /*G0_Command */ 16,     // pxi6259_write
                /*G1_Command */ 16,
                /*G0_HW_Save */ 16,
                /*G0_HW_Save_High */ 16,
                /*AI_Command_1 */ 16,
                /*G0_HW_Save_Low */ 16,
                /*AO_Command_1 */ 16,
                /*G1_HW_Save */ 16,
                /*G1_HW_Save_High */ 16,
                /*G1_HW_Save_Low */ 16,
                /*AI_Mode_1 */ 16,
                /*G0_Save */ 32,
                /*G0_Save_High */ 16,
                /*AI_Mode_2 */ 16,
                /*G0_Save_Low */ 16,
                /*AI_SI_Load_A */ 32,
                /*G1_Save */ 32,
                /*G1_Save_High */ 16,
                /*G1_Save_Low */ 16,
                /*AI_SI_Load_B */ 32,   // pxi6259_write
                /*AO_UI_Save */ 32,     // pxi6259_read
                /*AI_SC_Load_A */ 32,   // pxi6259_write
                /*AO_BC_Save */ 32,     // pxi6259_read
                /*AI_SC_Load_B */ 32,   // pxi6259_write
                /*AO_UC_Save */ 32,     //pxi6259_read
                /*AI_SI2_Load_A */ 32,
                /*AI_SI2_Load_B */ 32,
                /*G0_Mode */ 16,
                /*G1_Mode */ 16,        // pxi6259_write
                /*Joint_Status_1 */ 16, // pxi6259_read
                /*G0_Load_A */ 32,
                /*Joint_Status_2 */ 16,
                /*G0_Load_B */ 32,
                /*G1_Load_A */ 32,
                /*G1_Load_B */ 32,
                /*G0_Input_Select */ 16,
                /*G1_Input_Select */ 16,
                /*AO_Mode_1 */ 16,
                /*AO_Mode_2 */ 16,
                /*AO_UI_Load_A */ 32,
                /*AO_UI_Load_B */ 32,
                /*AO_BC_Load_A */ 32,
                /*AO_BC_Load_B */ 32,
                /*AO_UC_Load_A */ 32,
                /*AO_UC_Load_B */ 32,
                /*Clock_and_FOUT */ 16,
                /*IO_Bidirection_Pin */ 16,
                /*RTSI_Trig_Direction */ 16,
                /*Interrupt_Control */ 16,
                /*AI_Output_Control */ 16,
                /*Analog_Trigger_Etc */ 16,
                /*AI_START_STOP_Select */ 16,
                /*AI_Trigger_Select */ 16,
                /*AI_SI_Save */ 32,     // pxi6259_read
                /*AI_DIV_Load_A */ 16,  // pxi6259_write
                /*AI_SC_Save */ 32,     // pxi6259_read
                /*AO_Start_Select */ 16,        // pxi6259_write
                /*AO_Trigger_Select */ 16,
                /*AO_Mode_3 */ 16,
                /*G0_Autoincrement */ 16,
                /*G1_Autoincrement */ 16,
                /*Joint_Reset */ 16,
                /*Interrupt_A_Enable */ 16,
                /*Interrupt_B_Enable */ 16,
                /*AI_Personal */ 16,
                /*AO_Personal */ 16,
                /*RTSI_Trig_A_Output */ 16,
                /*RTSI_Trig_B_Output */ 16,
                /*RTSI_Shared_MUX */ 16,
                /*AO_Output_Control */ 16,
                /*AI_Mode_3 */ 16,
                /*Configuration_Memory_Clear */ 16,
                /*AI_FIFO_Clear */ 16,
                /*AO_FIFO_Clear */ 16,
                /*G0_Counting_Mode */ 16,
                /*G1_Counting_Mode */ 16,
                /*G0_Second_Gate */ 16,
                /*G1_Second_Gate */ 16,
                /*G0_DMA_Config */ 16,  // pxi6259_write
                /*G0_DMA_Status */ 16,  // pxi6259_read
                /*G1_DMA_Config */ 16,  // pxi6259_write
                /*G1_DMA_Status */ 16,  // pxi6259_read
                /*G0_MSeries_ABZ */ 16,
                /*G1_MSeries_ABZ */ 16,
                /*Clock_and_Fout2 */ 16,
                /*PLL_Control */ 16,
                /*PLL_Status */ 16,
                /*PFI_Output_Select_1 */ 16,
                /*PFI_Output_Select_2 */ 16,
                /*PFI_Output_Select_3 */ 16,
                /*PFI_Output_Select_4 */ 16,
                /*PFI_Output_Select_5 */ 16,
                /*PFI_Output_Select_6 */ 16,
                /*PFI_DI */ 16,
                /*PFI_DO */ 16,
                /*AI_Config_FIFO_Bypass */ 32,
                /*SCXI_DIO_Enable */ 8,
                /*CDI_FIFO_Data */ 32,  // pxi6259_read
                /*CDO_FIFO_Data */ 32,  // pxi6259_write
                /*CDIO_Status */ 32,    // pxi6259_read
                /*CDIO_Command */ 32,   // pxi6259_write
                /*CDI_Mode */ 32,
                /*CDO_Mode */ 32,
                /*CDI_Mask_Enable */ 32,
                /*CDO_Mask_Enable */ 32,
                /*M_Offset_Static_AI_Control_0_Register */ 8,
                /*M_Offset_Static_AI_Control_1_Register */ 8,
                /*M_Offset_Static_AI_Control_2_Register */ 8,
                /*M_Offset_Static_AI_Control_3_Register */ 8,
                /*M_Offset_Static_AI_Control_4_Register */ 8,
                /*M_Offset_Static_AI_Control_5_Register */ 8,
                /*M_Offset_Static_AI_Control_6_Register */ 8,
                /*M_Offset_Static_AI_Control_7_Register */ 8,
                /*Gen_PWM 0*/ 16,
                /*Gen_PWM 1*/ 16,
                /*Gen_PWM 2*/ 16,
                /*Gen_PWM 3*/ 16,
                /*Gen_PWM 4*/ 16,
                /*Gen_PWM 5*/ 16,
                /*Gen_PWM 6*/ 16,
                /*Gen_PWM 7*/ 16,
                /*AO_Config_Bank_0*/ 8,
                /*AO_Config_Bank_1*/ 8,
                /*AO_Config_Bank_2*/ 8,
                /*AO_Config_Bank_3*/ 8,
                /*AO_Waveform_Order_0*/ 8,
                /*AO_Waveform_Order_1*/ 8,
                /*AO_Waveform_Order_2*/ 8,
                /*AO_Waveform_Order_3*/ 8,
                /*AO_Waveform_Order_4*/ 8,
                /*AO_Waveform_Order_5*/ 8,
                /*AO_Waveform_Order_6*/ 8,
                /*AO_Waveform_Order_7*/ 8,
                /*AO_Waveform_Order_8*/ 8,
                /*AO_Waveform_Order_9*/ 8,
                /*AO_Waveform_Order_10*/ 8,
                /*AO_Waveform_Order_11*/ 8,
                /*AO_Waveform_Order_12*/ 8,
                /*AO_Waveform_Order_12*/ 8,
                /*AO_Waveform_Order_14*/ 8,
                /*AO_Waveform_Order_15*/ 8,
                /*DAC_Direct_Data_0_Register*/ 32,
                /*DAC_Direct_Data_1_Register*/ 32,
                /*DAC_Direct_Data_2_Register*/ 32,
                /*DAC_Direct_Data_3_Register*/ 32,
                /*DAC_Direct_Data_4_Register*/ 32,
                /*DAC_Direct_Data_5_Register*/ 32,
                /*DAC_Direct_Data_6_Register*/ 32,
                /*DAC_Direct_Data_7_Register*/ 32,
                /*DAC_Direct_Data_8_Register*/ 32,
                /*DAC_Direct_Data_9_Register*/ 32,
                /*DAC_Direct_Data_10_Register*/ 32,
                /*DAC_Direct_Data_11_Register*/ 32,
                /*DAC_Direct_Data_12_Register*/ 32,
                /*DAC_Direct_Data_13_Register*/ 32,
                /*DAC_Direct_Data_14_Register*/ 32,
                /*DAC_Direct_Data_15_Register*/ 32,
};

static uint32_t reg_offsets [] = {
                /*CDIO_DMA_Select */ 0x7,       // pxi6259_write
                /*SCXI_Status */ 0x7,   // pxi6259_read
                /*AI_AO_Select */ 0x9,  // pxi6259_write, same offset as e-series
                /*SCXI_Serial_Data_In */ 0x9,   // pxi6259_read
                /*G0_G1_Select */ 0xb,  // pxi6259_write, same offset as e-series
                /*Misc_Command */ 0xf,
                /*SCXI_Serial_Data_Out */ 0x11,
                /*SCXI_Control */ 0x13,
                /*SCXI_Output_Enable */ 0x15,
                /*AI_FIFO_Data */ 0x1c,
                /*Static_Digital_Output */ 0x24,        // pxi6259_write
                /*Static_Digital_Input */ 0x24, // pxi6259_read
                /*DIO_Direction */ 0x28,
                /*Cal_PWM */ 0x40,
                /*AI_Config_FIFO_Data */ 0x5e,
                /*Interrupt_C_Enable */ 0x88,   // pxi6259_write
                /*Interrupt_C_Status */ 0x88,   // pxi6259_read
                /*Analog_Trigger_Control */ 0x8c,
                /*AO_Serial_Interrupt_Enable */ 0xa0,
                /*AO_Serial_Interrupt_Ack */ 0xa1,      // pxi6259_write
                /*AO_Serial_Interrupt_Status */ 0xa1,   // pxi6259_read
                /*AO_Calibration */ 0xa3,
                /*AO_FIFO_Data */ 0xa4,
                /*PFI_Filter */ 0xb0,
                /*RTSI_Filter */ 0xb4,
                /*SCXI_Legacy_Compatibility */ 0xbc,
                /*Interrupt_A_Ack */ 0x104,     // pxi6259_write
                /*AI_Status_1 */ 0x104, // pxi6259_read
                /*Interrupt_B_Ack */ 0x106,     // pxi6259_write
                /*AO_Status_1 */ 0x106, // pxi6259_read
                /*AI_Command_2 */ 0x108,        // pxi6259_write
                /*G01_Status */ 0x108,  // pxi6259_read
                /*AO_Command_2 */ 0x10a,
                /*AO_Status_2 */ 0x10c, // pxi6259_read
                /*G0_Command */ 0x10c,  // pxi6259_write
                /*G1_Command */ 0x10e,  // pxi6259_write
                /*G0_HW_Save */ 0x110,
                /*G0_HW_Save_High */ 0x110,
                /*AI_Command_1 */ 0x110,
                /*G0_HW_Save_Low */ 0x112,
                /*AO_Command_1 */ 0x112,
                /*G1_HW_Save */ 0x114,
                /*G1_HW_Save_High */ 0x114,
                /*G1_HW_Save_Low */ 0x116,
                /*AI_Mode_1 */ 0x118,
                /*G0_Save */ 0x118,
                /*G0_Save_High */ 0x118,
                /*AI_Mode_2 */ 0x11a,
                /*G0_Save_Low */ 0x11a,
                /*AI_SI_Load_A */ 0x11c,
                /*G1_Save */ 0x11c,
                /*G1_Save_High */ 0x11c,
                /*G1_Save_Low */ 0x11e,
                /*AI_SI_Load_B */ 0x120,        // pxi6259_write
                /*AO_UI_Save */ 0x120,  // pxi6259_read
                /*AI_SC_Load_A */ 0x124,        // pxi6259_write
                /*AO_BC_Save */ 0x124,  // pxi6259_read
                /*AI_SC_Load_B */ 0x128,        // pxi6259_write
                /*AO_UC_Save */ 0x128,  //pxi6259_read
                /*AI_SI2_Load_A */ 0x12c,
                /*AI_SI2_Load_B */ 0x130,
                /*G0_Mode */ 0x134,
                /*G1_Mode */ 0x136,     // pxi6259_write
                /*Joint_Status_1 */ 0x136,      // pxi6259_read
                /*G0_Load_A */ 0x138,
                /*Joint_Status_2 */ 0x13a,
                /*G0_Load_B */ 0x13c,
                /*G1_Load_A */ 0x140,
                /*G1_Load_B */ 0x144,
                /*G0_Input_Select */ 0x148,
                /*G1_Input_Select */ 0x14a,
                /*AO_Mode_1 */ 0x14c,
                /*AO_Mode_2 */ 0x14e,
                /*AO_UI_Load_A */ 0x150,
                /*AO_UI_Load_B */ 0x154,
                /*AO_BC_Load_A */ 0x158,
                /*AO_BC_Load_B */ 0x15c,
                /*AO_UC_Load_A */ 0x160,
                /*AO_UC_Load_B */ 0x164,
                /*Clock_and_FOUT */ 0x170,
                /*IO_Bidirection_Pin */ 0x172,
                /*RTSI_Trig_Direction */ 0x174,
                /*Interrupt_Control */ 0x176,
                /*AI_Output_Control */ 0x178,
                /*Analog_Trigger_Etc */ 0x17a,
                /*AI_START_STOP_Select */ 0x17c,
                /*AI_Trigger_Select */ 0x17e,
                /*AI_SI_Save */ 0x180,  // pxi6259_read
                /*AI_DIV_Load_A */ 0x180,       // pxi6259_write
                /*AI_SC_Save */ 0x184,  // pxi6259_read
                /*AO_Start_Select */ 0x184,     // pxi6259_write
                /*AO_Trigger_Select */ 0x186,
                /*AO_Mode_3 */ 0x18c,
                /*G0_Autoincrement */ 0x188,
                /*G1_Autoincrement */ 0x18a,
                /*Joint_Reset */ 0x190,
                /*Interrupt_A_Enable */ 0x192,
                /*Interrupt_B_Enable */ 0x196,
                /*AI_Personal */ 0x19a,
                /*AO_Personal */ 0x19c,
                /*RTSI_Trig_A_Output */ 0x19e,
                /*RTSI_Trig_B_Output */ 0x1a0,
                /*RTSI_Shared_MUX */ 0x1a2,
                /*AO_Output_Control */ 0x1ac,
                /*AI_Mode_3 */ 0x1ae,
                /*Configuration_Memory_Clear */ 0x1a4,
                /*AI_FIFO_Clear */ 0x1a6,
                /*AO_FIFO_Clear */ 0x1a8,
                /*G0_Counting_Mode */ 0x1b0,
                /*G1_Counting_Mode */ 0x1b2,
                /*G0_Second_Gate */ 0x1b4,
                /*G1_Second_Gate */ 0x1b6,
                /*G0_DMA_Config */ 0x1b8,       // pxi6259_write
                /*G0_DMA_Status */ 0x1b8,       // pxi6259_read
                /*G1_DMA_Config */ 0x1ba,       // pxi6259_write
                /*G1_DMA_Status */ 0x1ba,       // pxi6259_read
                /*G0_MSeries_ABZ */ 0x1c0,
                /*G1_MSeries_ABZ */ 0x1c2,
                /*Clock_and_Fout2 */ 0x1c4,
                /*PLL_Control */ 0x1c6,
                /*PLL_Status */ 0x1c8,
                /*PFI_Output_Select_1 */ 0x1d0,
                /*PFI_Output_Select_2 */ 0x1d2,
                /*PFI_Output_Select_3 */ 0x1d4,
                /*PFI_Output_Select_4 */ 0x1d6,
                /*PFI_Output_Select_5 */ 0x1d8,
                /*PFI_Output_Select_6 */ 0x1da,
                /*PFI_DI */ 0x1dc,
                /*PFI_DO */ 0x1de,
                /*AI_Config_FIFO_Bypass */ 0x218,
                /*SCXI_DIO_Enable */ 0x21c,
                /*CDI_FIFO_Data */ 0x220,       // pxi6259_read
                /*CDO_FIFO_Data */ 0x220,       // pxi6259_write
                /*CDIO_Status */ 0x224, // pxi6259_read
                /*CDIO_Command */ 0x224,        // pxi6259_write
                /*CDI_Mode */ 0x228,
                /*CDO_Mode */ 0x22c,
                /*CDI_Mask_Enable */ 0x230,
                /*CDO_Mask_Enable */ 0x234,
                /*M_Offset_Static_AI_Control_0_Register */ 0x64,
                /*M_Offset_Static_AI_Control_1_Register */ 0x261,
                /*M_Offset_Static_AI_Control_2_Register */ 0x262,
                /*M_Offset_Static_AI_Control_3_Register */ 0x263,
                /*M_Offset_Static_AI_Control_4_Register */ 0x264,
                /*M_Offset_Static_AI_Control_5_Register */ 0x265,
                /*M_Offset_Static_AI_Control_6_Register */ 0x266,
                /*M_Offset_Static_AI_Control_7_Register */ 0x267,
                /*Gen_PWM 0*/ 0x44,
                /*Gen_PWM 1*/ 0x46,
                /*Gen_PWM 2*/ 0x48,
                /*Gen_PWM 3*/ 0x4a,
                /*Gen_PWM 4*/ 0x4c,
                /*Gen_PWM 5*/ 0x4e,
                /*Gen_PWM 6*/ 0x50,
                /*Gen_PWM 7*/ 0x52,
                /*AO_Config_Bank_0*/ 0xc3,
                /*AO_Config_Bank_1*/ 0xc7,
                /*AO_Config_Bank_2*/ 0xcb,
                /*AO_Config_Bank_3*/ 0xcf,
                /*AO_Waveform_Order_0*/ 0xc2,
                /*AO_Waveform_Order_1*/ 0xc6,
                /*AO_Waveform_Order_2*/ 0xca,
                /*AO_Waveform_Order_3*/ 0xce,
                /*AO_Waveform_Order_4*/ 0xd2,
                /*AO_Waveform_Order_5*/ 0xd6,
                /*AO_Waveform_Order_6*/ 0xda,
                /*AO_Waveform_Order_7*/ 0xde,
                /*AO_Waveform_Order_8*/ 0xe2,
                /*AO_Waveform_Order_9*/ 0xe6,
                /*AO_Waveform_Order_10*/ 0xea,
                /*AO_Waveform_Order_11*/ 0xee,
                /*AO_Waveform_Order_12*/ 0xf2,
                /*AO_Waveform_Order_12*/ 0xf6,
                /*AO_Waveform_Order_14*/ 0xfa,
                /*AO_Waveform_Order_15*/ 0xfe,
                /*DAC_Direct_Data_0_Register*/ 0xc0,
                /*DAC_Direct_Data_1_Register*/ 0xc4,
                /*DAC_Direct_Data_2_Register*/ 0xc8,
                /*DAC_Direct_Data_3_Register*/ 0xcc,
                /*DAC_Direct_Data_4_Register*/ 0xd0,
                /*DAC_Direct_Data_5_Register*/ 0xd4,
                /*DAC_Direct_Data_6_Register*/ 0xd8,
                /*DAC_Direct_Data_7_Register*/ 0xdc,
                /*DAC_Direct_Data_8_Register*/ 0xe0,
                /*DAC_Direct_Data_9_Register*/ 0xe4,
                /*DAC_Direct_Data_10_Register*/ 0xe8,
                /*DAC_Direct_Data_11_Register*/ 0xec,
                /*DAC_Direct_Data_12_Register*/ 0xf0,
                /*DAC_Direct_Data_13_Register*/ 0xf4,
                /*DAC_Direct_Data_14_Register*/ 0xf8,
                /*DAC_Direct_Data_15_Register*/ 0xfc,
};

/* MITE register offsets */
#define ChannelOperation_Register 0x0
#define ChannelControl_Register 0x4
#define ChannelStatus_Register 0x3c
#define BaseCount_Register 0x2c
#define TransferCount_Register 0x8
#define FifoCount_Register 0x40
#define MemoryConfig_Register 0xc
#define DeviceConfig_Register 0x14
#define BaseAddress_Register 0x28
#define MemoryAddress_Register 0x10
#define DeviceAddress_Register 0x18

#endif
