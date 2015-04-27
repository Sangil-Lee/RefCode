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

Infomation byte
char, unsigned char, signed char ----- 1 Byte
short, unsigned short ---------------- 2 Byte
int, unsigned int  ------------------- 4 Byte
long, unsigned long  ----------------- 4 Byte
float  ------------------------------- 4 Byte
double ------------------------------- 8 Byte
long double -------------------------- 8 Byte



******* Character strings
   char *message="Printing different data types";
   wchar_t *wmessage=L"Wide Character string";

****** Characters
   char ca='A';
   wchar_t wca=L'A';

****** Integer Data types
   unsigned short usa=65535; short ssb=-32768;
   unsigned int usi=4294967295; int ssi=-2147483648;
   unsigned long long int ulli=4294967295L;long long int lli=-2147483647L;

****** Floating point Data types
   float fa=1e+37,fb=1e-37;
   double da=1e+37,db=1e-37;
   long double lda=1e+37L,ldb=1e-37L;

****** Pointers
   int *p=&ssi;

****** Character strings
   printf("%s\n",message);
   printf("%ls\n\n",wmessage);

****** Character
   printf("%c\n",ca);
   printf("%lc\n\n",wca);
******** short
   printf("%hu %hi\n",usa,ssb);
   printf("%hx %hx\n\n",usa,ssb);
******** integer/long integers
   printf("%u %i\n",usi,ssi); 
   printf("%x %x\n\n",usi,ssi);
******* long long integers
   printf("%llu %lli\n",ulli,lli); 
   printf("%llx %llx\n\n",ulli,lli);
******* float
   printf("%f %f\n",fa,fb); 
******* double
   printf("%f %f\n",da,db); 
   printf("%e %e\n\n",da,db);
****** long double
   printf("%llf %llf\n",lda,ldb); 
   printf("%lle %lle\n\n",lda,ldb);

****** pointer of any data type
   printf("%p\n\n",p); 



                                    Decimal Byte
                                       Offset
                                    -----------
*/

//  Definitions of array sizes:
#define      HDRNAMEMAX   120     // Max char str length for file name
#define      USERINFOMAX  1000    // User information space
#define      COMMENTMAX   80      // User comment string max length (5 comments)
#define      LABELMAX     16      // Label string max length
#define      FILEVERMAX   16      // File version string max length
#define      DATEMAX      10      // String length of file creation date string as ddmmmyyyy\0
#define      ROIMAX       10      // Max size of roi array of structures
#define      TIMEMAX      7       // Max time store as hhmmss\0

/*
#define  ControllerVersion              0    // short  Hardware Version 
#define  LogicOutput                    2   // short   Definition of Output BNC 
#define  AmpHiCapLowNoise               4    // WORD    Amp Switching Mode 
#define  mode                           8    // short   timing mode

*/
#define  PI_exp_sec                       10    // float   alternitive exposure, in sec.
#define  PI_xDimDet                        6    // WORD    Detector x dimension of chip.

#define  PI_VChipXdim                     14    // short   Virtual Chip X dim
#define  PI_VChipYdim                     16    // short   Virtual Chip Y dim
#define  PI_yDimDet                       18    // WORD    y dimension of CCD or detector.


#define  PI_DATE                       20    // char    date
/*
#define  VirtualChipFlag               30    // short   On/Off
#define  Spare_1[2]                    32    // char    spare
#define  noscan                        34    // short   Old number of scans - should always be -1
#define  DetTemperature                36    // float   Detector Temperature Set
#define  DetType                       40    // short   CCD/DiodeArray type
#define  stdiode                       44    // short   trigger diode
*/
#define  PI_xdim                       42    // WORD    actual # of pixels on x axis
#define  PI_DELAYTIME                  46    // float   Used with Async Mode   *********************************************** We need!!!
/*
#define  ShutterControl                50    // WORD    Normal, Disabled Open, Disabled Closed
#define  AbsorbLive                    52    // short   On/Off
#define  AbsorbMode                    54    // WORD    Reference Strip or File
#define  CanDoVirtualChipFlag          56    // short   T/F Cont/Chip able to do Virtual Chip
*/
#define  PI_ThresholdMinLive           58    // short   On/Off
#define  PI_ThresholdMinVal            60    // float   Threshold Minimum Value
#define  PI_ThresholdMaxLive           64    // short   On/Off
#define  PI_ThresholdMaxVal            66    // float   Threshold Maximum Value

#define  PI_SpecAutoSpectroMode        70    // short   T/F Spectrograph Used     *********************************************** We need!!!
#define  PI_SpecCenterWlNm             72    // float   Center Wavelength in Nm   *********************************************** We need!!!
#define  PI_SpecGlueFlag               76    // short   T/F File is Glued         *********************************************** We need!!!
#define  PI_SpecGlueStartWlNm          78    // float   Starting Wavelength in Nm *********************************************** We need!!!
#define  PI_SpecGlueEndWlNm            82    // float   Ending Wavelength in Nm *********************************************** We need!!!
#define  PI_SpecGlueMinOvrlpNm         86    // float   Minimum Overlap in Nm     *********************************************** We need!!!
#define  PI_SpecGlueFinalResNm         90    // float   Final Resolution in Nm    *********************************************** We need!!!
/*
#define  PulserType                    94    // short   0=None, PG200=1, PTG=2, DG535=3
#define  CustomChipFlag                96    // short   T/F Custom Chip Used
#define  XPrePixels                    98    // short   Pre Pixels in X direction
#define  XPostPixels                  100    // short   Post Pixels in X direction
#define  YPrePixels                   102    // short   Pre Pixels in Y direction 
#define  YPostPixels                  104    // short   Post Pixels in Y direction
#define  asynen                       106    // short   asynchron enable flag  0 = off
*/
#define  PI_datatype                  108   /// * short   experiment datatype     *********************************************** We need!!!
                                             // 0 =   float (4 bytes)
                                             // 1 =   long (4 bytes)
                                             // 2 =   short (2 bytes)
                                             // 3 =   unsigned short (2 bytes) */
/*
#define  PulserMode                   110    // short   Repetitive/Sequential
#define  PulserOnChipAccums           112    // WORD    Num PTG On-Chip Accums
#define  PulserRepeatExp              114    // DWORD   Num Exp Repeats (Pulser SW Accum)
*/
#define  PI_PulseRepWidth             118    // float   Width Value for Repetitive pulse (usec) *********************************************** We need!!!
#define  PI_PulseRepDelay             122    // float   Width Value for Repetitive pulse (usec) *********************************************** We need!!!
#define  PI_PulseSeqStartWidth        126    // float   Start Width for Sequential pulse (usec) *********************************************** We need!!!
#define  PI_PulseSeqEndWidth          130    // float   End Width for Sequential pulse (usec)   *********************************************** We need!!!
#define  PI_PulseSeqStartDelay        134    // float   Start Delay for Sequential pulse (usec) *********************************************** We need!!!
#define  PI_PulseSeqEndDelay          138    // float   End Delay for Sequential pulse (usec)   *********************************************** We need!!!
/*
#define  PulseSeqIncMode              142    // short   Increments: 1=Fixed, 2=Exponential
#define  PImaxUsed                    144    // short   PI-Max type controller flag
#define  PImaxMode                    146    // short   PI-Max mode
#define  PImaxGain                    148    // short   PI-Max Gain
#define  BackGrndApplied              150    // short   1 if background subtraction done
#define  PImax2nsBrdUsed              152    // short   T/F PI-Max 2ns Board Used
#define  minblk                       154    // WORD    min. # of strips per skips
#define  numminblk                    156    // WORD    # of min-blocks before geo skps
#define  SpecMirrorLocation[2]        158    // short   Spectro Mirror Location, 0=Not Present
#define  SpecSlitLocation[4]          162    // short   Spectro Slit Location, 0=Not Present
#define  CustomTimingFlag             170    // short   T/F Custom Timing Used
#define  ExperimentTimeLocal[TIMEMAX] 172    // char    Experiment Local Time as hhmmss\0
#define  ExperimentTimeUTC[TIMEMAX]   179    // char    Experiment UTC Time as hhmmss\0
#define  ExposUnits                   186    // short   User Units for Exposure
#define  ADCoffset                    188    // WORD    ADC offset
#define  ADCrate                      190    // WORD    ADC rate
#define  ADCtype                      192    // WORD    ADC type
#define  ADCresolution                194    // WORD    ADC resolution
#define  ADCbitAdjust                 196    // WORD    ADC bit adjust
#define  gain                         198    // WORD    gain
#define  Comments[5][COMMENTMAX]      200    // char    File Comments
#define  geometric                    600    // WORD    geometric ops: rotate 0x01,
                                             // reverse 0x02, flip 0x04
#define  xlabel[LABELMAX]             602    // char    intensity display string
#define  cleans                       618    // WORD    cleans
#define  NumSkpPerCln                 620    // WORD    number of skips per clean.
#define  SpecMirrorPos[2]             622    // short   Spectrograph Mirror Positions
#define  SpecSlitPos[4]               626    // float   Spectrograph Slit Positions
#define  AutoCleansActive             642    // short   T/F
#define  UseContCleansInst            644    // short   T/F
#define  AbsorbStripNum               646    // short   Absorbance Strip Number
#define  SpecSlitPosUnits             648    // short   Spectrograph Slit Position Units
#define  SpecGrooves                  650    // float   Spectrograph Grating Grooves
#define  srccmp                       654    // short   number of source comp. diodes
*/
#define  PI_ydim                      656    // WORD    y dimension of raw data.           *********************************************** We need!!!
/*
#define  scramble                     658    // short   0=scrambled,1=unscrambled
#define  ContinuousCleansFlag         660    // short   T/F Continuous Cleans Timing Option
#define  ExternalTriggerFlag          662    // short   T/F External Trigger Timing Option
*/
#define  PI_lnoscan                   664    // long    Number of scans (Early WinX)      *********************************************** We need!!!
#define  PI_lavgexp                   668    // long    Number of Accumulations           *********************************************** We need!!!
#define  PI_ReadoutTime               672    // float   Experiment readout time           *********************************************** We need!!!
/*
#define  TriggeredModeFlag            676    // short   T/F Triggered Timing Option
#define  Spare_2[10]                  678    // char    
#define  sw_version[FILEVERMAX]       688    // char    Version of SW creating this file
#define  type                         704    // short    1 = new120 (Type II)             
                                            //  2 = old120 (Type I )           
                                            //  3 = ST130                      
                                            //  4 = ST121                      
                                            //  5 = ST138                      
                                            //  6 = DC131 (PentaMax)           
                                            //  7 = ST133 (MicroMax/SpectroMax)
                                            //  8 = ST135 (GPIB)               
                                            //  9 = VICCD                      
                                            // 10 = ST116 (GPIB)               
                                            // 11 = OMA3 (GPIB)                
                                            // 12 = OMA4                       
#define  flatFieldApplied             706   //  short   1 if flat field was applied.
#define  Spare_3[16]                  708   // char     
#define  kin_trig_mode                724   // short    Kinetics Trigger Mode
#define  dlabel[LABELMAX]             726   // char     Data label.
#define  Spare_4[436]                 742   // char    
#define  PulseFileName[HDRNAMEMAX]   1178   // char  Name of Pulser File with Pulse Widths/Delays (for Z-Slice) 
#define  AbsorbFileName[HDRNAMEMAX]  1298   // char    Name of Absorbance File (if File Mode)
#define  YT_Flag                     1426   // short    Set to 1 if this file contains YT data
#define  clkspd_us                   1428   // float    Vert Clock Speed in micro-sec
#define  HWaccumFlag                 1432   // short    set to 1 if accum done by Hardware.
#define  StoreSync                   1434   // short    set to 1 if store sync used
#define  BlemishApplied              1436   // short    set to 1 if blemish removal applied
#define  CosmicApplied               1438   // short    set to 1 if cosmic ray removal applied
#define  CosmicType                  1440   // short    if cosmic ray applied, this is type
#define  CosmicThreshold             1442   // float    Threshold of cosmic ray removal.  
*/
#define  PI_clkspd_us                1428   // float    Vert Clock Speed in micro-sec
#define  PI_NumExpRepeats            1418   // DWORD    Number of Times experiment repeated
#define  PI_NumExpAccums             1422   // DWORD    Number of Time experiment accumulated

#define  PI_NumFrames                1446   // long     number of frames in file.              *********************************************** We need!!!
/*
#define  MaxIntensity                1450   // float    max intensity of data (future)    
#define  MinIntensity                1454   // float    min intensity of data (future)    
*/
#define  PI_ylabel                      1458   // char     y axis label.                          *********************************************** We need!!!
/*
#define  ShutterType                 1474   // WORD     shutter type.                     

*/
#define  PI_shutterComp                 1476   // float    shutter compensation time.        
#define  PI_readoutMode                 1480   // WORD     readout mode, full,kinetics, etc      *********************************************** We need!!!
#define  PI_clkspd                      1484   // WORD     clock speed for kinetics & frame transfer
/*
#define  WindowSize                  1482   // WORD     window size for kinetics only.    

#define  interface_type              1486   // WORD     computer interface                
                                            //  (isa-taxi, pci, eisa, etc.)             
#define  NumROIsInExperiment         1488   // short    May be more than the 10 allowed in
                                            //  this header (if 0, assume 1)            
#define  Spare_5[16]                 1490   // char                                       
#define  controllerNum               1506   // WORD     if multiple controller system will
                                            //  have controller number data came from.  
                                            //  this is a future item.                  
#define  SWmade                      1508   // WORD     Which software package created this file 
*/
#define  PI_NumROI                   1510   // short    number of ROIs used. if 0 assume 1.      

/*                                      
-------------------------------------------------------------------------------

                        ROI entries   (1512 - 1631)
*/
#define PI_ROI_Entery                1512
/*
  struct ROIinfo                           // 
  {                                        //            
    unsigned short  startx;                           //  left x start value.               
    unsigned short  endx;                             //  right x value.                    
    unsigned short  groupx;                           //  amount x is binned/grouped in hw. 
    unsigned short  starty;                           //  top y start value.                
    unsigned short  endy;                             //  bottom y value.                   
    unsigned short  groupy;                           //  amount y is binned/grouped in hw. 
  } ROIinfoblk[ROIMAX];                     // 
*/
/*
                                           //  ROI Starting Offsets:     
                                           //       
                                           //    ROI  1  =  1512     
                                           //    ROI  2  =  1524     
                                           //    ROI  3  =  1536     
                                           //    ROI  4  =  1548     
                                           //    ROI  5  =  1560     
                                             //  ROI  6  =  1572     
                                             //  ROI  7  =  1584     
                                             //  ROI  8  =  1596     
                                             //  ROI  9  =  1608     
                                             //  ROI 10  =  1620
                                             // 
                                             // 
#define  FlatField[HDRNAMEMAX]       1632    // char    Flat field file name.       
#define  background[HDRNAMEMAX]      1752    // char    background sub. file name.  
#define  blemish[HDRNAMEMAX]         1872    // char    blemish file name.          
#define  file_header_ver             1992    // float   version of this file header 
#define  YT_Info[1000]               1996-2995 // char  Reserved for YT information
#define  WinView_id                  2996    // long    == 0x01234567L if file created by WinX
                                             // 

// START OF X CALIBRATION STRUCTURE (3000 - 3488)

#define  offset                      3000   //  double  offset for absolute data scaling
#define  factor                      3008   //  double  factor for absolute data scaling
#define  current_unit                3016   //  char    selected scaling unit           
#define  reserved1                   3017   //  char    reserved                        
#define  string[40]                  3018   //  char    special string for scaling      
#define  reserved2[40]               3058   //  char    reserved                        
#define  calib_valid                 3098   //  char    flag if calibration is valid    
#define  input_unit                  3099   //  char    current input units for         
                                            // "calib_value"                  
#define  polynom_unit                3100   //  char    linear UNIT and used            
                                            // in the "polynom_coeff"         
#define  polynom_order               3101   //  char    ORDER of calibration POLYNOM    
#define  calib_count                 3102    // char    valid calibration data pairs    
*/

#define  PI_pixel_position          3103    // double  pixel_position[10] pixel pos. of calibration data  
#define  PI_calib_value             3183    // double  calib_value[10] calibration VALUE at above pos  
#define  PI_polynom_coeff           3263    // double  polynom_coeff[6] polynom COEFFICIENTS            
#define  PI_Xlaser_position         3311    // double  laser wavenumber for relativ WN 
#define  PI_Ylaser_position         3800    // double  laser wavenumber for relativ WN 

/*
#define  reserved3                   3319    // char    reserved                        
#define  new_calib_flag              3320    // BYTE    If set to 200, valid label below
#define  calib_label[81]             3321    // char    Calibration label (NULL term'd) 
#define  expansion[87]               3402    // char    Calibration Expansion area      
                                             // 
                                             // 
//   START OF Y CALIBRATION STRUCTURE (3489 - 3977)

#define  offset                      3489   //  double  offset for absolute data scaling
#define  factor                      3497   //  double  factor for absolute data scaling
#define  current_unit                3505   //  char    selected scaling unit           
#define  reserved1                   3506   //  char    reserved                        
#define  string[40]                  3507   //  char    special string for scaling      
#define  reserved2[40]               3547   //  char    reserved                        
#define  calib_valid                 3587   //  char    flag if calibration is valid    
#define  input_unit                  3588   //  char    current input units for         
                                            // "calib_value"                   
#define  polynom_unit                3589   //  char    linear UNIT and used            
                                            // in the "polynom_coeff"          
#define  polynom_order               3590   //  char    ORDER of calibration POLYNOM    
#define  calib_count                 3591    // char    valid calibration data pairs    
#define  pixel_position[10]          3592    // double  pixel pos. of calibration data  
#define  calib_value[10]             3672    // double  calibration VALUE at above pos  
#define  polynom_coeff[6]            3752    // double  polynom COEFFICIENTS            
#define  laser_position              3800    // double  laser wavenumber for relativ WN 
#define  reserved3                   3808    // char    reserved                        
#define  new_calib_flag              3809    // BYTE    If set to 200, valid label below
#define  calib_label[81]             3810    // char    Calibration label (NULL term'd) 
#define  expansion[87]               3891    // char    Calibration Expansion area      
                                             // 
//     END OF CALIBRATION STRUCTURES

#define  Istring[40]                 3978    // char    special intensity scaling string
#define  Spare_6[25]                 4018    // char    
#define  SpecType                    4043    // BYTE    spectrometer type (acton, spex, etc.)
#define  SpecModel                   4044    // BYTE    spectrometer model (type dependent)
#define  PulseBurstUsed              4045    // BYTE    pulser burst mode on/off
#define  PulseBurstCount             4046    // DWORD   pulser triggers per burst
#define  ulseBurstPeriod             4050    // double  pulser burst period (in usec)
#define  PulseBracketUsed            4058    // BYTE    pulser bracket pulsing on/off
#define  PulseBracketType            4059    // BYTE    pulser bracket pulsing type
#define  PulseTimeConstFast          4060    // double  pulser slow exponential time constant (in usec)
#define  PulseAmplitudeFast          4068    // double  pulser fast exponential amplitude constant
#define  PulseTimeConstSlow          4076    // double  pulser slow exponential time constant (in usec)
#define  PulseAmplitudeSlow          4084    // double  pulser slow exponential amplitude constant
#define  AnalogGain;                 4092    // short   analog gain
#define  AvGainUsed                  4094    // short   avalanche gain was used
#define  AvGain                      4096    // short   avalanche gain value
#define  lastvalue                   4098    // short   Always the LAST value in the header
                                             // 
//         END OF HEADER       // 

*/
#define      PI_DATAREAD      4100    // Start of Data
/*

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
*/

int mdsplus_data_put_spe(char* fileNamePath, int shotNumber, int storeMode, float blipTime, float daqTime);
int mdsplus_Tree_create(int storeMode, int shotNumber);
