/*                   WINHEAD.TXT

                            $Date: 3/23/04 11:36 $

                Header Structure For WinView/WinSpec (WINX) Files

  The current data file used for WINX files consists of a 4100 (1004 Hex)
  byte header followed by the data.

  Beginning with Version 2.5, many more items were added to the header to
  make it a complete as possible record of the data collection.  This includes
  spectrograph and pulser information.  Much of these additions were accomplished
  by recycling old information which had not been used in many versions.
  All data files created under previous 2.x versions of WinView/WinSpec CAN
  still be read correctly.  HOWEVER, files created under the new versions
  (2.5 and higher) CANNOT be read by previous versions of WinView/WinSpec
  OR by the CSMA software package.

Custom Data Types Used in the structure

BYTE = unsigned char;
WORD = unsigned short;
DWORD = unsigned long;
            ***************************************************

                                    Decimal Byte
                                       Offset
                                    -----------
*/
  short   ControllerVersion              0  Hardware Version
  short   LogicOutput                    2  Definition of Output BNC
  WORD    AmpHiCapLowNoise               4  Amp Switching Mode
  WORD    xDimDet                        6  Detector x dimension of chip.
  short   mode                           8  timing mode
  float   exp_sec                       10  alternitive exposure, in sec.
  short   VChipXdim                     14  Virtual Chip X dim
  short   VChipYdim                     16  Virtual Chip Y dim
  WORD    yDimDet                       18  y dimension of CCD or detector.
  char    date[DATEMAX]                 20  date
  short   VirtualChipFlag               30  On/Off
  char    Spare_1[2]                    32
  short   noscan                        34  Old number of scans - should always be -1
  float   DetTemperature                36  Detector Temperature Set
  short   DetType                       40  CCD/DiodeArray type
  WORD    xdim                          42  actual # of pixels on x axis
  short   stdiode                       44  trigger diode
  float   DelayTime                     46  Used with Async Mode
  WORD    ShutterControl                50  Normal, Disabled Open, Disabled Closed
  short   AbsorbLive                    52  On/Off
  WORD    AbsorbMode                    54  Reference Strip or File
  short   CanDoVirtualChipFlag          56  T/F Cont/Chip able to do Virtual Chip
  short   ThresholdMinLive              58  On/Off
  float   ThresholdMinVal               60  Threshold Minimum Value
  short   ThresholdMaxLive              64  On/Off
  float   ThresholdMaxVal               66  Threshold Maximum Value
  short   SpecAutoSpectroMode           70  T/F Spectrograph Used
  float   SpecCenterWlNm                72  Center Wavelength in Nm
  short   SpecGlueFlag                  76  T/F File is Glued
  float   SpecGlueStartWlNm             78  Starting Wavelength in Nm
  float   SpecGlueEndWlNm               82  Starting Wavelength in Nm
  float   SpecGlueMinOvrlpNm            86  Minimum Overlap in Nm
  float   SpecGlueFinalResNm            90  Final Resolution in Nm
  short   PulserType                    94  0=None, PG200=1, PTG=2, DG535=3
  short   CustomChipFlag                96  T/F Custom Chip Used
  short   XPrePixels                    98  Pre Pixels in X direction
  short   XPostPixels                  100  Post Pixels in X direction
  short   YPrePixels                   102  Pre Pixels in Y direction 
  short   YPostPixels                  104  Post Pixels in Y direction
  short   asynen                       106  asynchron enable flag  0 = off
  short   datatype                     108  experiment datatype
                                             0 =   float (4 bytes)
                                             1 =   long (4 bytes)
                                             2 =   short (2 bytes)
                                             3 =   unsigned short (2 bytes)
  short   PulserMode                   110  Repetitive/Sequential
  WORD    PulserOnChipAccums           112  Num PTG On-Chip Accums
  DWORD   PulserRepeatExp              114  Num Exp Repeats (Pulser SW Accum)
  float   PulseRepWidth                118  Width Value for Repetitive pulse (usec)
  float   PulseRepDelay                122  Width Value for Repetitive pulse (usec)
  float   PulseSeqStartWidth           126  Start Width for Sequential pulse (usec)
  float   PulseSeqEndWidth             130  End Width for Sequential pulse (usec)
  float   PulseSeqStartDelay           134  Start Delay for Sequential pulse (usec)
  float   PulseSeqEndDelay             138  End Delay for Sequential pulse (usec)
  short   PulseSeqIncMode              142  Increments: 1=Fixed, 2=Exponential
  short   PImaxUsed                    144  PI-Max type controller flag
  short   PImaxMode                    146  PI-Max mode
  short   PImaxGain                    148  PI-Max Gain
  short   BackGrndApplied              150  1 if background subtraction done
  short   PImax2nsBrdUsed              152  T/F PI-Max 2ns Board Used
  WORD    minblk                       154  min. # of strips per skips
  WORD    numminblk                    156  # of min-blocks before geo skps
  short   SpecMirrorLocation[2]        158  Spectro Mirror Location, 0=Not Present
  short   SpecSlitLocation[4]          162  Spectro Slit Location, 0=Not Present
  short   CustomTimingFlag             170  T/F Custom Timing Used
  char    ExperimentTimeLocal[TIMEMAX] 172  Experiment Local Time as hhmmss\0
  char    ExperimentTimeUTC[TIMEMAX]   179  Experiment UTC Time as hhmmss\0
  short   ExposUnits                   186  User Units for Exposure
  WORD    ADCoffset                    188  ADC offset
  WORD    ADCrate                      190  ADC rate
  WORD    ADCtype                      192  ADC type
  WORD    ADCresolution                194  ADC resolution
  WORD    ADCbitAdjust                 196  ADC bit adjust
  WORD    gain                         198  gain
  char    Comments[5][COMMENTMAX]      200  File Comments
  WORD    geometric                    600  geometric ops: rotate 0x01,
                                             reverse 0x02, flip 0x04
  char    xlabel[LABELMAX]             602  intensity display string
  WORD    cleans                       618  cleans
  WORD    NumSkpPerCln                 620  number of skips per clean.
  short   SpecMirrorPos[2]             622  Spectrograph Mirror Positions
  float   SpecSlitPos[4]               626  Spectrograph Slit Positions
  short   AutoCleansActive             642  T/F
  short   UseContCleansInst            644  T/F
  short   AbsorbStripNum               646  Absorbance Strip Number
  short   SpecSlitPosUnits             648  Spectrograph Slit Position Units
  float   SpecGrooves                  650  Spectrograph Grating Grooves
  short   srccmp                       654  number of source comp. diodes
  WORD    ydim                         656  y dimension of raw data.
  short   scramble                     658  0=scrambled,1=unscrambled
  short   ContinuousCleansFlag         660  T/F Continuous Cleans Timing Option
  short   ExternalTriggerFlag          662  T/F External Trigger Timing Option
  long    lnoscan                      664  Number of scans (Early WinX)
  long    lavgexp                      668  Number of Accumulations
  float   ReadoutTime                  672  Experiment readout time
  short   TriggeredModeFlag            676  T/F Triggered Timing Option
  char    Spare_2[10]                  678  
  char    sw_version[FILEVERMAX]       688  Version of SW creating this file
  short   type                         704   1 = new120 (Type II)             
                                             2 = old120 (Type I )           
                                             3 = ST130                      
                                             4 = ST121                      
                                             5 = ST138                      
                                             6 = DC131 (PentaMax)           
                                             7 = ST133 (MicroMax/SpectroMax)
                                             8 = ST135 (GPIB)               
                                             9 = VICCD                      
                                            10 = ST116 (GPIB)               
                                            11 = OMA3 (GPIB)                
                                            12 = OMA4                       
  short   flatFieldApplied             706  1 if flat field was applied.
  char    Spare_3[16]                  708  
  short   kin_trig_mode                724  Kinetics Trigger Mode
  char    dlabel[LABELMAX]             726  Data label.
  char    Spare_4[436]                 742
  char    PulseFileName[HDRNAMEMAX]   1178  Name of Pulser File with
                                             Pulse Widths/Delays (for Z-Slice)
  char    AbsorbFileName[HDRNAMEMAX]  1298 Name of Absorbance File (if File Mode)
  DWORD   NumExpRepeats               1418  Number of Times experiment repeated
  DWORD   NumExpAccums                1422  Number of Time experiment accumulated
  short   YT_Flag                     1426  Set to 1 if this file contains YT data
  float   clkspd_us                   1428  Vert Clock Speed in micro-sec
  short   HWaccumFlag                 1432  set to 1 if accum done by Hardware.
  short   StoreSync                   1434  set to 1 if store sync used
  short   BlemishApplied              1436  set to 1 if blemish removal applied
  short   CosmicApplied               1438  set to 1 if cosmic ray removal applied
  short   CosmicType                  1440  if cosmic ray applied, this is type
  float   CosmicThreshold             1442  Threshold of cosmic ray removal.  
  long    NumFrames                   1446  number of frames in file.         
  float   MaxIntensity                1450  max intensity of data (future)    
  float   MinIntensity                1454  min intensity of data (future)    
  char    ylabel[LABELMAX]            1458  y axis label.                     
  WORD    ShutterType                 1474  shutter type.                     
  float   shutterComp                 1476  shutter compensation time.        
  WORD    readoutMode                 1480  readout mode, full,kinetics, etc  
  WORD    WindowSize                  1482  window size for kinetics only.    
  WORD    clkspd                      1484  clock speed for kinetics & frame transfer
  WORD    interface_type              1486  computer interface                
                                             (isa-taxi, pci, eisa, etc.)             
  short   NumROIsInExperiment         1488  May be more than the 10 allowed in
                                             this header (if 0, assume 1)            
  char    Spare_5[16]                 1490                                    
  WORD    controllerNum               1506  if multiple controller system will
                                             have controller number data came from.  
                                             this is a future item.                  
  WORD    SWmade                      1508  Which software package created this file 
  short   NumROI                      1510  number of ROIs used. if 0 assume 1.      

                                      
-------------------------------------------------------------------------------

                        ROI entries   (1512 - 1631)

  struct ROIinfo                           
  {                                                   
    WORD  startx                            left x start value.               
    WORD  endx                              right x value.                    
    WORD  groupx                            amount x is binned/grouped in hw. 
    WORD  starty                            top y start value.                
    WORD  endy                              bottom y value.                   
    WORD  groupy                            amount y is binned/grouped in hw. 
  } ROIinfoblk[ROIMAX]                   
                                            ROI Starting Offsets:     
                                                 
                                              ROI  1  =  1512     
                                              ROI  2  =  1524     
                                              ROI  3  =  1536     
                                              ROI  4  =  1548     
                                              ROI  5  =  1560     
                                              ROI  6  =  1572     
                                              ROI  7  =  1584     
                                              ROI  8  =  1596     
                                              ROI  9  =  1608     
                                              ROI 10  =  1620

-------------------------------------------------------------------------------

  char    FlatField[HDRNAMEMAX]       1632  Flat field file name.       
  char    background[HDRNAMEMAX]      1752  background sub. file name.  
  char    blemish[HDRNAMEMAX]         1872  blemish file name.          
  float   file_header_ver             1992  version of this file header 
  char    YT_Info[1000]               1996-2995  Reserved for YT information
  long    WinView_id                  2996  == 0x01234567L if file created by WinX

-------------------------------------------------------------------------------

                        START OF X CALIBRATION STRUCTURE (3000 - 3488)

  double  offset                      3000  offset for absolute data scaling
  double  factor                      3008  factor for absolute data scaling
  char    current_unit                3016  selected scaling unit           
  char    reserved1                   3017  reserved                        
  char    string[40]                  3018  special string for scaling      
  char    reserved2[40]               3058  reserved                        
  char    calib_valid                 3098  flag if calibration is valid    
  char    input_unit                  3099  current input units for         
                                            "calib_value"                  
  char    polynom_unit                3100  linear UNIT and used            
                                            in the "polynom_coeff"         
  char    polynom_order               3101  ORDER of calibration POLYNOM    
  char    calib_count                 3102  valid calibration data pairs    
  double  pixel_position[10]          3103  pixel pos. of calibration data  
  double  calib_value[10]             3183  calibration VALUE at above pos  
  double  polynom_coeff[6]            3263  polynom COEFFICIENTS            
  double  laser_position              3311  laser wavenumber for relativ WN 
  char    reserved3                   3319  reserved                        
  BYTE    new_calib_flag              3320  If set to 200, valid label below
  char    calib_label[81]             3321  Calibration label (NULL term'd) 
  char    expansion[87]               3402  Calibration Expansion area      

-------------------------------------------------------------------------------

                        START OF Y CALIBRATION STRUCTURE (3489 - 3977)

  double  offset                      3489  offset for absolute data scaling
  double  factor                      3497  factor for absolute data scaling
  char    current_unit                3505  selected scaling unit           
  char    reserved1                   3506  reserved                        
  char    string[40]                  3507  special string for scaling      
  char    reserved2[40]               3547  reserved                        
  char    calib_valid                 3587  flag if calibration is valid    
  char    input_unit                  3588  current input units for         
                                            "calib_value"                   
  char    polynom_unit                3589  linear UNIT and used            
                                            in the "polynom_coeff"          
  char    polynom_order               3590  ORDER of calibration POLYNOM    
  char    calib_count                 3591  valid calibration data pairs    
  double  pixel_position[10]          3592  pixel pos. of calibration data  
  double  calib_value[10]             3672  calibration VALUE at above pos  
  double  polynom_coeff[6]            3752  polynom COEFFICIENTS            
  double  laser_position              3800  laser wavenumber for relativ WN 
  char    reserved3                   3808  reserved                        
  BYTE    new_calib_flag              3809  If set to 200, valid label below
  char    calib_label[81]             3810  Calibration label (NULL term'd) 
  char    expansion[87]               3891  Calibration Expansion area      

                         END OF CALIBRATION STRUCTURES

-------------------------------------------------------------------------------

  char    Istring[40]                 3978  special intensity scaling string
  char    Spare_6[25]                 4018  
  BYTE    SpecType                    4043  spectrometer type (acton, spex, etc.)
  BYTE    SpecModel                   4044  spectrometer model (type dependent)
  BYTE    PulseBurstUsed              4045  pulser burst mode on/off
  DWORD   PulseBurstCount             4046  pulser triggers per burst
  double  ulseBurstPeriod             4050  pulser burst period (in usec)
  BYTE    PulseBracketUsed            4058  pulser bracket pulsing on/off
  BYTE    PulseBracketType            4059  pulser bracket pulsing type
  double  PulseTimeConstFast          4060  pulser slow exponential time constant (in usec)
  double  PulseAmplitudeFast          4068  pulser fast exponential amplitude constant
  double  PulseTimeConstSlow          4076  pulser slow exponential time constant (in usec)
  double  PulseAmplitudeSlow          4084  pulser slow exponential amplitude constant
  short   AnalogGain;                 4092  analog gain
  short   AvGainUsed                  4094  avalanche gain was used
  short   AvGain                      4096  avalanche gain value
  short   lastvalue                   4098  Always the LAST value in the header

                         END OF HEADER

-------------------------------------------------------------------------------

                                      4100  Start of Data



        ***************************** E.O.F.*****************************




  Definitions of array sizes:
  ---------------------------

    HDRNAMEMAX  = 120     Max char str length for file name
    USERINFOMAX = 1000    User information space
    COMMENTMAX  = 80      User comment string max length (5 comments)
    LABELMAX    = 16      Label string max length
    FILEVERMAX  = 16      File version string max length
    DATEMAX     = 10      String length of file creation date string as ddmmmyyyy\0
    ROIMAX      = 10      Max size of roi array of structures
    TIMEMAX     = 7       Max time store as hhmmss\0



  Custom Data Types used in the structure:
  ----------------------------------------

    BYTE = unsigned char
    WORD = unsigned short
    DWORD = unsigned long




  READING DATA:
  -------------

    The data follows the header beginning at offset 4100.

    Data is stored as sequential points.
    
    The X, Y and Frame dimensions are determined by the header.

      The X dimension of the stored data is in "xdim" ( Offset 42  ).
      The Y dimension of the stored data is in "ydim" ( Offset 656 ).
      The number of frames of data stored is in "NumFrames" ( Offset 1446 ).

    The size of a frame (in bytes) is:

      One frame size = xdim x ydim x (datatype Offset 108)
