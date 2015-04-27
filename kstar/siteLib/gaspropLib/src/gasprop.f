      SUBROUTINE CALC (IDID, PROP, OVAR, INPAR1, VALU1, INPAR2, VALU2,
     +      COFNAM, IOUNIT)
* (C) Copyright (1993), Cryodata Inc.; all rights reserved.
C
C-----Version 3.30, March 3, 1998
C All revisions from 3.0 are prefixed with the line
C-----Version 3.xx  (date and information)
C********************************************* IDID
C     IDID = 1   single phase in PROP(I,0) only
C            2   sat liquid and vapor in PROP(I,1) and PROP(I,2) only;
C            3   mixture properties in PROP(I,0) + [IDID=2]
C            4   sat liquid in PROP(I,1) only;
C            5   sat vapor  in PROP(I,2) only;
C********************************************* PROP(I,J) Indices
C     DIMENSION PROP (0:31, 0:2)
C
C from EOS:   X,  P,  T,  D,  S,  H,  U,  dP/dD, dP/dT, Cv (=dU/dT (V))
C             0,  1   2   3   4   5   6   7      8      9
C-----Versions < 3.20: [Prop(0,0)=quality, (0,1)=dP/dTs, (0,2)= Latent Heat]
C-----Version 3.20: Prop(0,0), (0,1), (0,2) all refer to quality;
C                   dP/dTsat and Latent Ht moved to 28 and 29 respectively.
C
C from DERIV: Cp, Gamma, Alpha, Gruneisen,  Kt,  c,   JTcoef  dH/dV*V
C             10  11     12     13          14   15   16      17
C
C             dH/dT   dS/dT     << at constant D    c2*gmolwt
C             18      19                            20
C
C from VISC:  Viscosity
C             21
C
C from COND:  Conductivity     Thermal diffusivity     Prandtl number
C             22               23                      24
C
C from SURFTN Surface Tension
C             25
C
C from DIEL   Dielectric constant (real)
C             26
C
C-----Version 3.13:
C             27 = PV/RT
C-----Version 3.20
C             28 = dP/dT sat
C             29 = Latent Heat
C
C PROP(21:26)= (viscosity to dielectric constant) are calculated only when
C              [a] requested, and [b] available.
C              Equations are available when the corresponding IDX(j)
C              in /INDEXX/ are non-zero.  The positive numerical value
C              of IDX can distinguish between alternative equations
C              if necessary.
C
C-----Version 3.30
C PROP(30)   = Fugacity / Pressure, a dimensionless quantity
C
C-----Version 3.141
C PROP (31,2) is used on input to signal a special case:
C     (PROP array contains consistent P, T, D, and IDID from the previous
C     call, but not all output parameters have been calculated.  Input
C     processing is skipped. Jumps to output calculations. See GASFUN
C     for details. Not for general use!)
C
C---------------------------------------------------------------
C PROP(I,0) = single phase fluid OR liquid-vapor mixture property
C PROP(I,1) = saturated liquid property
C PROP(I,2) = saturated vapor property
C             Liquid-vapor mixture properties which cannot be defined
C             will be zero.  these are:
C                dP/dD, Cp, Gamma, Alpha, Kt, viscosity, conductivty,
C                diffusivity, Prandtl No.
C             Liquid-vapor properties which can be defined, but usually
C             are ignored, are: Cv, Gruneisen, and c
C             We also set these = zero (in this version).
C
C********************************************* OVAR
C If OVAR = '**'   all properties are calculated
C           'NT'   transport properties are not calculated
C           'TR'   transport but not dielectric or surface tension props
C           (anything else) only PROP(0-20), & PROP(27-29) are calculated.
C*********************************************
C
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (NIV=7, NPRP=31, NCF=65)
      DIMENSION PROP (0:31,0:2), P1D(0:31), AIN(NIV)
      CHARACTER*1  INPAR1, INPAR2
      CHARACTER*2  OVAR
      CHARACTER*11 INLIST
      CHARACTER*64 COFNAM, COFFIL
      CHARACTER*32 FLUNAM
      COMMON/PRCOF/  COFFIL
      COMMON/PRNAME/ FLUNAM
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
C-----Version 3.30: 3 terms added in next line; common UFXCVF added
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/UFXCVF/ FIXCVF(NCF)
      COMMON/PRECSN/ E(0:15)
      COMMON/INDEXX/ IDX(12)
      COMMON/UUNITS/ KUNITS(0:NPRP)
      COMMON/UCONVF/ CONVF(NCF)
C-----Version 3.20: KIN defined and used (following 2 lines + 2 other lines)
      DIMENSION JMIN(5), JMAX(5), KIN(7)
C-----Version 3.30: JPLUS defined; two lines.
      DIMENSION JPLUS(9)
      DATA JPLUS /24, 25, 27, 28, 29, 31, 61, 63, 64/
      DATA KIN /1,2,3,4,5,6,0/
      DATA INLIST  /'PTDSHUXM2LV'/
      DATA JMIN /0, 1, 1, 1, 2/, JMAX /0, 2, 2, 1, 2/
*
* Check that the correct fluid constants are being used.
*
C-----Version 3.141, 3.20: Use JDID for this call, so that input IDID is not
C     overwritten; READCF resets JDID to zero.
      JDID = 1
      IF (COFNAM .NE. COFFIL) CALL READCF (COFNAM, JDID)
      IF (JDID .LT. 0) THEN
         IDID = JDID
         RETURN
      ENDIF
*
* Identify the two input parameters;
*
      J1 = INDEX (INLIST, INPAR1)
      J2 = INDEX (INLIST, INPAR2)
      IF (J1*J2 .EQ. 0) THEN
C-----Version 3.1  : note IDID change from 3.03
         IDID = -11
         RETURN
      ENDIF
*
* Check for valid thermodynamic parameter pair
*
      JIN = JM (J1, J2)
      IF (JIN .LE. 0) THEN
C-----Version 3.1  note IDID change from 3.03
         IDID = -12
         RETURN
      ENDIF
*
* Reset precision and units as needed
*
C-----Version 3.201, March 19, 1995: Comment out the following line
C      IF (JPRECS .NE. NINT(E(0))) CALL PRECIS (JPRECS)
C-----Version 3.20: units check.
      IF (IOUNIT .NE. KUNITS(NPRP)) CALL UNILIB (IOUNIT)
C
C Place input values into the array and convert to GASPAK units
C (Note: input temperatures must be K or R, not C or F)
C
      IF (J1 .LE. NIV) AIN(J1) = VALU1 * CONVF (KUNITS(KIN(J1)))
      IF (J2 .LE. NIV) AIN(J2) = VALU2 * CONVF (KUNITS(KIN(J2)))
C
C-----Version 3.141: special feature used for GASFUN (spreadsheets)
C     IDID=0 relocated.
C-----Version 3.30: If loop change.
      IDID = 0
      IF (PROP(31,2) .EQ. -262144.D0)THEN
         GMOLWT = SMOLWT
         GO TO 500
      ELSE
         GMOLWT = XMOLWT
      ENDIF
C
*
* Set the output array to zero
*     Property 31 is not used for output.
*     Prop(31) is used as input by the DLL version.
*     Do not erase.
C-----Version 3.13: limit changed from 26 to 27.
C-----Version 3.20: limit changed from 27 to 30.
      DO 404 K1 = 0, 30
         PROP (K1,0) = 0.D0
         PROP (K1,1) = 0.D0
         PROP (K1,2) = 0.D0
  404 CONTINUE
*
* Do the calculation specified; return answers in PROP(i,j)
*
      GO TO (110, 120, 130, 140, 150, 160, 170, 180, 190, 200,
     +       210, 220, 230, 240, 240, 240, 260, 270, 280, 290) JIN
*
*-----(P,T) Input
110   CALL DFPT (IDID, PROP, AIN(1), AIN(2))
      GO TO 500
*
*-----(D,T) Input
120   CALL FDT (IDID, PROP, AIN(3), AIN(2))
      IF (IDID .EQ. 1) THEN
         CALL DERIV (PROP)
      ELSE IF (IDID .EQ. 3) THEN
         CALL DERIV (PROP(0,1))
         CALL DERIV (PROP(0,2))
      ENDIF
      GO TO 500
*
*-----(D,P) Input
130   CALL TFDP (IDID, PROP, AIN(3), AIN(1))
      GO TO 500
*
*-----(P,H) Input
140   CALL DTFPY (IDID, PROP, AIN(1), AIN(5), 5)
      GO TO 500
*
*-----(D,H) Input
150   CALL TFDY (IDID, PROP, AIN(3), AIN(5), 5)
      GO TO 500
*
*-----(P,S) Input
160   CALL DTFPY (IDID, PROP, AIN(1), AIN(4), 4)
      GO TO 500
*
*-----(T,S) Input
170   CALL DFST (IDID, PROP, AIN(4), AIN(2))
      GO TO 500
*
*-----(D,S) Input
180   CALL TFDY (IDID, PROP, AIN(3), AIN(4), 4)
      GO TO 500
*
*-----(H,S) Input
190   CALL DTFHS (IDID, PROP, AIN(5), AIN(4))
      GO TO 500
*
*-----(P,U) Input
200   CALL DTFPY (IDID, PROP, AIN(1), AIN(6), 6)
      GO TO 500
*
*-----(D,U) Input
210   CALL TFDY (IDID, PROP, AIN(3), AIN(6), 6)
      GO TO 500
*
*-----Saturation Pressure Input, or Quality and Pressure
220   CONTINUE
      IF (IBTWNR (PTP, AIN(1), PCC) .LT. 0) THEN
         IDID = -111
         GO TO 500
      ENDIF
      CALL TSIP (AIN(2), DPDT, AIN(1))
      GO TO 235
*
*-----Saturation Temperature Input, or Quality and Temperature
230   CONTINUE
C-----Version 3.11:  TTP changed to TLL in following line
      IF (IBTWNR (TLL, AIN(2), TCC) .LT. 0) THEN
         IDID = -112
         GO TO 500
      ENDIF
      CALL PSFT (AIN(1), DPDT, AIN(2))
*
235   CONTINUE
      IF ((J1-11)*(J2-11) .EQ. 0) THEN
         IDID = 5
         PROP(3,1) = DSATL (AIN(2))
      ELSE
         CALL SLFPT (JDID, PROP(0,1), AIN(1), AIN(2))
         IF (JDID .LT. 0) THEN
C-----Version 3.1   IDID changed.
            IDID = -112
            GO TO 500
         ENDIF
         CALL DERIV (PROP(0,1))
      ENDIF
      IF ((J1-10)*(J2-10) .EQ. 0) THEN
         IDID = 4
         PROP(3,2) = DSATV (AIN(2))
      ELSE
         CALL SVFPT (JDID, PROP(0,2), AIN(1), AIN(2))
         IF (JDID .LT. 0) THEN
C-----Version 3.1   IDID changed
            IDID = -112
            GO TO 500
         ENDIF
         CALL DERIV (PROP(0,2))
      ENDIF
      IF (IDID .LT. 4) THEN
         IF ((J1-7)*(J2-7) .EQ. 0) THEN
*--------Quality input
C-----Version 3.13: IDID=2 branch elminated to avoid spreadsheet problems
CV313       IF (AIN(7)*(AIN(7)-1.D0) .LT. 0.D0) THEN
            IF (AIN(7)*(AIN(7)-1.D0) .LE. 0.D0) THEN
               CALL MIXPRP (PROP(0,0), PROP(0,1), PROP (0,2),
     +               AIN(7), DPDT)
               IDID = 3
CV313       ELSE IF (AIN(7)*(AIN(7)-1.D0) .EQ. 0.D0) THEN
CV313          IDID = 2
            ELSE
               IDID = -140
               GO TO 500
            ENDIF
         ELSE
            IDID = 2
         ENDIF
      ENDIF
C-----Version 3.20: location of DPDT and Latent heat changed
C-old PROP(0,1) = DPDT
C-old PROP(0,2) = DPDT*AIN(2)*(1.D0/PROP(3,2) - 1.D0/PROP(3,1))
      PROP(28,1) = DPDT
      PROP(28,2) = DPDT
      PROP(29,1) = DPDT*AIN(2)*(1.D0/PROP(3,2) - 1.D0/PROP(3,1))
      PROP(29,2) = PROP(29,1)
      GO TO 500
*
*---- Saturation S, H, or U (2, L, or V)
240   CONTINUE
C     JIN = 14 for S, 15 for H, 16 for U
      JY = JIN - 10
      IERR = -103 - JIN
      CALL SATFY (IDID, P1D, AIN(JY), JY)
C     Note: P1D(0) = dP/dTsat
      IF (IDID .LT. 0) THEN
         IDID = IERR
         GO TO 500
      ENDIF
      GO TO 265
*
* --- (sat or satL or satV) Density input
260   CALL SATFD (IDID, P1D, AIN(3))
C     Note: P1D(0) = dP/dTsat
      IF (IDID .LT. 0) GO TO 500
      IERR = -113
*
*---- Following applies to sat S, H, U, and D
265   CONTINUE
*     IDID = 1 in liquid, 2 in vapor
      DO 406 J = 1, 9
         PROP (J, IDID) = P1D(J)
  406 CONTINUE
      CALL DERIV (PROP(0, IDID))
      IDID = IDID + 3
*
      IF ((J1-10)*(J2-10) .EQ. 0) THEN
*     Failure if the input requests satL, but satV was found.
         IF (IDID .EQ. 5) IDID = IERR
C-----Version 3.14: vapor density calculated, for Clausis-Clapeyron
         PROP(3,2) = DSATV (PROP(2,1))
      ELSE IF ((J1-11)*(J2-11) .EQ. 0) THEN
*     Failure if the input requests satV, but satL was found.
         IF (IDID .EQ. 4) IDID = IERR
C-----Version 3.14: liquid density calculated, for Clausis-Clapeyron
         PROP(3,1) = DSATL (PROP(2,2))
      ELSE IF ((J1-9)*(J2-9) .EQ. 0) THEN
*     If the input requests "sat" rather than just "satL" or "satV",
*     the other phase must be calculated.
         IF (IDID .EQ. 4) THEN
            CALL SVFPT (IDID, PROP(0,2), P1D(1), P1D(2))
            CALL DERIV (PROP(0,2))
         ELSE
            CALL SLFPT (IDID, PROP(0,1), P1D(1), P1D(2))
            CALL DERIV (PROP(0,1))
         ENDIF
         IDID = 2
      ENDIF
C-----Version 3.14 If loop added.
      IF (IDID .GT. 0) THEN
C-----Version 3.13, 3.01: Following 2 lines have been relocated from just
C     before the above endif.
C-----Version 3.20: new location in PROP array
C-old    PROP(0,1) = P1D(0)
C-old    PROP(0,2) = P1D(0)*PROP(2,1)*(1.D0/PROP(3,2) - 1.D0/PROP(3,1))
         PROP(28,1) = P1D(0)
         PROP(28,2) = P1D(0)
         PROP(29,1) = P1D(0)*PROP(2,1)*(1.D0/PROP(3,2) - 1.D0/PROP(3,1))
         PROP(29,2) = PROP(29,1)
      ENDIF
      GO TO 500
*
*-----Melting Pressure input
270   CALL MELTFP (IDID, PROP, AIN(1))
      IF (IDID .GT. 0) CALL DERIV (PROP)
      GO TO 500
*
*-----Melting Temperature input
280   CALL MELTFT (IDID, PROP, AIN(2))
      IF (IDID .GT. 0) CALL DERIV (PROP)
      GO TO 500
*
*-----Melting Density input
290   CALL MELTFD (IDID, PROP, AIN(3))
      IF (IDID .GT. 0) CALL DERIV (PROP)
*     GO TO 500
*
500   CONTINUE
* All calls come thru here; check for error
* Check that calculated pressure and temperature are within bounds
      IF ((IDID .GE. 0) .AND. (JIN .LE. 11)) THEN
         IF (PROP(2,0) .LT. TLL) THEN
            IDID = -131
         ELSE IF (PROP(2,0)  .GT. TUL+1.D0) THEN
            IDID = -132
         ELSE IF (PROP(1,0) .GT. PUL*1.02) THEN
            IDID = -134
         ELSE IF (PROP(1,0) .LT. PLL) THEN
            IDID = -133
         ENDIF
      ENDIF
C-----Version 3.1
C      IF (IDID .LT. 0) RETURN
C-----Version 3.30, Reset Molecular weight when it has been reset
C     during a disassociation calculation.  RDM, 11/21/96
      IF (IDID .LT. 0) THEN
         GMOLWT=XMOLWT
         RETURN
      END IF
*
* Set quality in single phase region, or
* Latent Heat = delH in the two-phase region
*
      IF (IDID .EQ. 1) THEN
         IF (PROP(3,0) .GE. DCC) THEN
            PROP(0,0) = -1.D0
         ELSE IF (GMOLWT .NE. XMOLWT) THEN
            PROP(0,0) = XDIS
         ELSE
            PROP(0,0) = 2.D0
         ENDIF
C-----Version 3.01   Following elseif has been added;  Dec. 8, 1991
      ELSE IF ((IDID-2)*(IDID-3) .EQ. 0) THEN
C-----Version 3.20 (6 Fortran lines)
         PROP(29,2) = PROP(5,2) - PROP(5,1)
         PROP(29,0) = PROP(29,2)*(1.D0 - PROP(0,0))
C        To avoid inconsistent sat. pressures near Tc, average:
         PROP(1,1) = 0.5D0*(PROP(1,1) + PROP(1,2))
         PROP(1,2) = PROP(1,1)
         PROP(0,1) = 0.D0
         PROP(0,2) = 1.D0
C-----Version 3.141
      ELSE IF (IDID .EQ. 5) THEN
         PROP(0,2) = 1.D0
      ENDIF
C-----Version 3.13:  Calculate PV/RT  (6 lines)
      DUM = PROP(2,0)*PROP(3,0)
      IF (DUM .GT. 0.D0) PROP(27,0) = PROP(1,0)/(RCON*DUM)
      DUM = PROP(2,1)*PROP(3,1)
      IF (DUM .GT. 0.D0) PROP(27,1) = PROP(1,1)/(RCON*DUM)
      DUM = PROP(2,2)*PROP(3,2)
      IF (DUM .GT. 0.D0) PROP(27,2) = PROP(1,2)/(RCON*DUM)
C-----Version 3.30: calculate fugacity      
      CALL FUGAC (IDID, PROP)
*
* Transport properties
*
      IF ((IDX(8).GT.0).AND.((OVAR.EQ.'**').OR.(OVAR.EQ.'TR'))) THEN
      DO 408 J = (JMIN(IDID)), (JMAX(IDID))
         CALL VISC (PROP(0,J))
         CALL COND (PROP(0,J))
C-----Version 3.01   Following 2 lines replace 2 calculations performed
C     previously within subroutine CALC.  Nov. 14, 1991
         CALL DIFFUS (PROP(0,J))
         CALL PRNDTL (PROP(0,J))
  408 CONTINUE
      ENDIF
*
* Dielectric constant and surface tension
*
      IF ((OVAR.EQ.'**') .OR. (OVAR.EQ.'NT')) THEN
        DO 410 J = (JMIN(IDID)), (JMAX(IDID))
C-----Version 3.1;  call surface tension only in 2-phase region
            IF (IDID .GT. 1)   CALL SURFTN (PROP(0,J))
            IF (IDX(9) .GT. 0) CALL DIEL (PROP(0,J))
  410    CONTINUE
      ENDIF
C
C-----Version 3.20: units conversion
C-----Version 3.30, 9 lines: Make correction to molecular wt. used
C     in disassociation calculation.  RDM 
      IF(GMOLWT .NE. XMOLWT) THEN
         CONVF (16) = FIXCVF(16) / GMOLWT
         CONVF (17) = FIXCVF(17) / GMOLWT
         CONVF (19) = FIXCVF(19) / GMOLWT
         DO 428 J = 1, 9
            NJ = JPLUS(J)
            CONVF(NJ) = FIXCVF(NJ) * GMOLWT
  428    CONTINUE
      ENDIF
C
      IF (KUNITS(NPRP) .NE. 3) THEN
         DO 420 J = 1, (NPRP-1)
            CVFJ = 1./CONVF(KUNITS(J))
            PROP(J,0) = PROP(J,0)*CVFJ
            PROP(J,1) = PROP(J,1)*CVFJ
            PROP(J,2) = PROP(J,2)*CVFJ
  420    CONTINUE
      ENDIF
C-----Version 3.30; one line
      GMOLWT = XMOLWT
      RETURN
      END
C
C
C
      FUNCTION JM (J1, J2)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
*
* Symmetric matrix of response to input variables
C-----Version 3.13  table corrected: H,T = invalid combination.
C-----Version 3.14  table corrected: (S, H, U) & (2, L, V)
      DIMENSION KM (11, 11)
*               P   T   D   S   H   U   X   M   2   L   V
      DATA KM / 0,  1,  3,  6,  4, 10, 12, 18, 12, 12, 12,
     T          1,  0,  2,  7,  0,  0, 13, 19, 13, 13, 13,
     D          3,  2,  0,  8,  5, 11,  0, 20, 17, 17, 17,
     S          6,  7,  8,  0,  9,  0,  0,  0, 14, 14, 14,
     H          4,  0,  5,  9,  0,  0,  0,  0, 15, 15, 15,
     U         10,  0, 11,  0,  0,  0,  0,  0, 16, 16, 16,
     X         12, 13,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     M         18, 19, 20,  0,  0,  0,  0,  0,  0,  0,  0,
     2         12, 13, 17, 14, 15, 16,  0,  0,  0,  0,  0,
     L         12, 13, 17, 14, 15, 16,  0,  0,  0,  0,  0,
     V         12, 13, 17, 14, 15, 16,  0,  0,  0,  0,  0/
      JM = KM (J1, J2)
      END
C
C
C
      SUBROUTINE GASERR (ERRMSG, IDID )
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Error messages, when the program returns with IDID < zero
C-----Version 3.1
C Input values of IDID:
C   -1 to   -9: invalid coefficient file
C  -10 TO  -99: external coding errors.  User should investigate.
C -100 to -199: input is outside of the valid EOS range.
C -200 to -299: unexpected iteration convergence failure, probably
C               due to input out of range.  Such an error should have been 
C               trapped by the Fortran front end, but could easily come
C               from untested user program input.  
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C-----Version 3.1
C-----Version 3.20  Modified message concerning iteration failure.
C-----Version 3.30 NLINES increased to 37; message 26 added (IDID=-5)
      PARAMETER (NLINES=37)
      CHARACTER*60 LINE (NLINES)
      CHARACTER*72 ERRMSG
      DIMENSION JIDID (NLINES)
      DATA (LINE(J), J=1,19) /
     1' Input Pressure outside of valid range.                     ',
     2' Input Temperature outside of valid range.                  ',
     3' Input Density outside of valid range.                      ',
     4' Solid phase encountered.                                   ',
     5' Saturation pressure outside of valid range.                ',
     6' Saturation temperature outside of valid range.             ',
     7' Saturation density outside of valid range.                 ',
     8' Saturation entropy outside of valid range.                 ',
     9' Saturation enthalpy out of valid range.                    ',
     A' Saturation energy outside of valid range.                  ',
     1' Melting pressure must be > triple point pressure.          ',
     2' Melting density must be > triple point liquid density.     ',
     3' Melting temperature too high, P exceeds valid range.       ',
     4' Entropy out of range.                                      ',
     5' Enthalpy out of range.                                     ',
     6' Energy out of range.                                       ',
     7' Calculated temperature > maximum valid temperature.        ',
     8' Calculated temperature < minimum valid temperature.        ',
     9' Calculated pressure <= zero.                               '/
C-----Version 3.1
      DATA (LINE(J), J=20, 37) /
     B' Calculated pressure > maximum valid pressure.              ',
     1' Input quality outside of valid range (0 to 1)              ',
     2' Fluid coefficient file not found.                          ',
     3' Reading error on the fluid coefficient file.               ',
     4' Unexpected end-of-file on the fluid coefficient file.      ',
     5' Invalid data on fluid coefficient file.                    ',
     6' Fluid coefficient file invalid for this Gaspak version.    ',
     7' Unidentified input parameter(s).                           ',
     8' Invalid combination of input parameters.                   ',
     9' Copyright 1992, Cryodata, Inc., all rights reserved.       ',
     C' Valid output: single phase fluid state.                    ',
     1' Valid output: both saturated liquid and vapor states.      ',
     2' Valid output: liquid-vapor mixture & saturation states.    ',
     3' Valid output: Saturated liquid state only.                 ',
     4' Valid output: Saturated vapor state only.                  ',
     5' Invalid index on input to GASFUN.                          ',
     6' Calculation successful but invalid fluid phase requested.  ',
     7' Iteration failure.  Input data out of range (?).           '/
C-----Version 3.20; two GASFUN messages added. 
      DATA JIDID / -101, -102, -103, -110, -111, -112, -113, -117,
     A -118, -119, -115, -114, -116, -121, -122, -123, -132, -131,
     B -133, -134, -140,   -1,   -2,   -3,   -4,   -5,  -11,  -12, 
     C    0,    1,    2,    3,    4,    5,  -13,   -15,  9789/
C
      ERRMSG = '   '
      IF (IDID .LT. -202) THEN
         JDID = 9789
      ELSE
         JDID = IDID
      ENDIF
      DO 414 J = 1, NLINES
         IF (JIDID (J) .EQ. JDID) GO TO 420
414   CONTINUE
      ERRMSG = ' Gaspak program error'
      RETURN
420   CONTINUE
      ERRMSG = LINE(J)
      RETURN
      END
C
C
C
      SUBROUTINE PRECIS (J)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Set precision for use in iterative routines.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (NF=3, NE=15)
      DIMENSION A(0:NE), FACT(NF)
      COMMON/PRECSN/ E(0:NE)
      DATA A /0.D0, 1.D-1, 3.D-2, 1.D-2, 3.D-3, 1.D-3, 3.D-4, 1.D-4,
     +       3.D-5, 1.D-5, 3.D-6, 1.D-6, 3.D-7, 1.D-7, 3.D-8, 1.D-8/
C-----Version 3.1 ; change factors; remove write statements
      DATA FACT /2.D+1, 0.5D+0, 2.D-3/
      N = MIN (NF, MAX(J, 1))
      R    = FACT(N)
      E(0) = N
      DO 416 K = 1, NE
         E(K) = R*A(K)
  416 CONTINUE
      RETURN
      END
C
C
C
C-----Version 3.13; Block data added.
C-----Version 3.20; Block data expanded.
      BLOCK DATA V320
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (NPRP=31, NCF=65)
      CHARACTER*11 UNITS
      COMMON/PRECSN/ E(0:15)
      COMMON/VERSON/ VERS
      COMMON/VALID/  IVALID
      COMMON/UUNITS/ KUNITS(0:NPRP)
      COMMON/UFXCVF/ FIXCVF(NCF)
      COMMON/UNITLB/ UNITS (NCF)
C Default precison, in case it is never set by the main program, and
C the computer system does not initialize its memory to zero.
C
      DATA E /0.D0, 3.D-2, 1.D-2, 3.D-3, 1.D-3, 3.D-4, 1.D-4, 3.D-5,
     +      1.D-5, 3.D-6, 1.D-6, 3.D-7, 1.D-7, 3.D-8, 1.D-8, 3.D-9/
C Following is the Version number; update when necessary
      DATA VERS /3.30D0/
C IVALID will be set = 1 when valid coefficients have been read in READCF
C                    = (negative number) when READCF failed.
      DATA IVALID /0/
C Following are the default (native) units.  This block will be overwritten
C     when the user selects other units.
      DATA KUNITS /
     e  1,  2, 12, 18, 26, 30, 30, 62, 21, 26, 26, 1, 57, 1, 6, 32,
     f  35, 30, 26, 1, 1, 40, 38, 44, 1, 60, 1, 1, 21, 30, 2, 3/
      DATA FIXCVF /
     1     1.D0,          1.D0,          1.D+03,        1.D+02,
     2     1.01325D+02,   6894.757D-03,  1.D0,          1.D-03,
     3     1.D-02,        9.86923D-03,   1.45037D-01,   1.D0,
     4     0.555555556D0, 1.D0,          1.8D0,         1.D0,
     5     1.D+03,        1.D0,          16.01846D0,    1.D-03,
     6     1.D0,          1.D+03,        12.410563D0,   1.D-03,
     7     1.D0,          1.D0,          4.1868D0,      1.D-03,
     8     1.D0,          1.D0,          2.326D0,       1.D0,
     9     0.3048D0,      1.D+03,        1.D0,          1.D-02,
     A     8.05765D-02,   1.D0,          1.730735D0,    1.D-06,
     1     1.D-03,        1.488164D0,    1.D0,          1.D-04,
     2     9.290304D-2,   0.555555556D0, 1.D0,          1.D-03,
     3     1.D+3,         1.D0,          1.D0,          1.D0,
     4     1.D0,          1.D0,          1.D0,          1.D0,
     5     1.D0,          1.D0,          1.D0,          1.D0,
     6     1.D-03,        1.D0,          1.D0,          0.4304257D0,
     7     1.D0/
      DATA UNITS /
     1   ' [-]       ', ' [KPa]     ', ' [MPa]     ', ' [bar]     ',
     2   ' [atmos]   ', ' [psi]     ', ' [1/KPa]   ', ' [1/MPa]   ',
     3   ' [1/bar]   ', ' [1/atmos] ', ' [1/psi]   ', ' [K]       ',
     4   ' [R]       ', ' [1/K]     ', ' [1/R]     ', ' [Kg/m3]   ',
     5   ' [g/cm3]   ', ' [mol/L]   ', ' [Lb/ft3]  ', ' [Pa/K]    ',
     6   ' [KPa/K]   ', ' [MPa/K]   ', ' [psi/R]   ', ' [J/Kg-K]  ',
     7   ' [J/g-K]   ', ' [J/mol-K] ', ' [BTU/Lb-R]', ' [J/Kg]    ',
     8   ' [J/g]     ', ' [J/mol]   ', ' [BTU/Lb]  ', ' [m/s]     ',
     9   ' [ft/s]    ', ' [K/Pa]    ', ' [K/KPa]   ', ' [K/bar]   ',
     A   ' [R/psi]   ', ' [W/m-K]   ', ' BTU/HrFtR ', ' [uPa-s]   ',
     1   ' [cP]      ', ' [Lbm/Ft-s]', ' [m2/s]    ', ' [cm2/s]   ',
     2   ' [ft2/s]   ', ' [F]       ', ' [C]       ', ' [Pa]      ',
     3   ' [1/Pa]    ', ' [Pa-s]    ', ' [P/Pc]    ', ' [T/Tc]    ',
     4   ' [D/Dc]    ', ' [TdP/PdT] ', ' [../R]    ', ' [../RTc]  ',
     5   ' [TdV/VdT] ', ' [PdD/DdP] ', ' [PdT/TdP] ', ' [N/m]     ',
     6   ' [Pa-m3/Kg]', ' [KPaL/mol]', ' [KPam3/Kg]', ' psi-ft3/lb',
     7   ' [DdP/PdD] '/
      END
C
C
C
      SUBROUTINE UNILIB (NK)
* (C) Copyright (1993), Cryodata Inc.; all rights reserved.
C Output is the array KUNITS in common /UUNITS/;
C    the library array KULIB (j,NK) is copied onto KUNITS.
C KUNITS is used in subroutine CALC.  Sequence is that of PROP array.
      PARAMETER (NKMAX=5, NPRP=31)
      DIMENSION KULIB (0:NPRP, NKMAX) 
      COMMON /UUNITS/ KUNITS(0:NPRP)
C-----Version 3.30: fugacity units allocated      
      DATA KULIB /
     a  1, 48, 12, 16, 24, 28, 28, 61, 20, 24, 24, 1, 57, 1, 49, 32,
     b  34, 28, 24, 1, 1, 50, 38, 43, 1, 60, 1, 1, 20, 28, 1, 1,
     c  1,  2, 12, 17, 25, 29, 29, 63, 21, 25, 25, 1, 57, 1, 7, 32,
     d  35, 29, 25, 1, 1, 40, 38, 44, 1, 60, 1, 1, 21, 29, 1, 2,
     e  1,  2, 12, 18, 26, 30, 30, 62, 21, 26, 26, 1, 57, 1, 7, 32,
     f  35, 30, 26, 1, 1, 40, 38, 44, 1, 60, 1, 1, 21, 30, 1, 3,
     g  1,  6, 13, 19, 27, 31, 31, 64, 23, 27, 27, 1, 57, 1, 11, 33,
     h  37, 31, 27, 1, 1, 42, 39, 45, 1, 60, 1, 1, 23, 31, 1, 4,
     i  1, 51, 52, 53, 55, 56, 56, 62, 21, 55, 55, 1, 57, 1, 7, 32,
     j  35, 30, 26, 1, 1, 40, 38, 44, 1, 60, 1, 1, 21, 30, 1, 5/
C Note: KULIB (31,j) is the unit set number, = NK
      IF ((NK .GT. 5) .OR. (NK .LT. 1)) THEN
C Use default units (Si-molar) if input is invalid.
         N = 3
      ELSE
C N=1 for SI units, =2 for SI-cgs, =4 for Engineering, =5 for dimensionless
         N = NK
      ENDIF
      DO 20 J = 0, NPRP
   20 KUNITS(J) = KULIB (J, N)
      END
C
C
C-----Version 3.20 Subroutine Gunlbl added
      SUBROUTINE GUNLBL (UARRAY, IOUNIT)
C Output: UARRAY = array of units labels for PROP array, Character*11
C Input:  IOUNIT = choice of units (1 - 5)
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (NCF=65, NPRP=31)
      CHARACTER*11 UARRAY(0:31), UNITS
      COMMON/UUNITS/ KUNITS(0:NPRP)
      COMMON/UNITLB/ UNITS (NCF)
C
      IF (IOUNIT .NE. KUNITS(NPRP)) CALL UNILIB (IOUNIT)
      DO 10 J = 0, 31
         UARRAY(J) = UNITS (KUNITS(J))
10    CONTINUE
      RETURN
      END
C
C
C
      SUBROUTINE READCF (COFNAM, IDID)
C-----Version 3.1 ; note IDID added to calling sequence
C-----Version 3.13 ; No pauses, no writing, reduced no. input parameters.
C
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Read coefficients for a particular fluid from the file [COFNAM]
C Version 3.1, Feb. 10, 1992; indices and constants for hydrogen (IDX(4)=2)
C differ from earlier versions.   Fluid coefficient files xxx.GAS
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
C-----Version 3.20: NCF added to parameter list
      PARAMETER (NCOFIL=25, NDIM=50, RNDOFF=1.D-05, PMIN=1.D-06,
     +      DMIN = 1.D-06, NCF=65)
C-----Version 3.1 ; PAWS added; otherwise no functional change
C-----Version 3.13 ; PAWS removed; length of COFNAM, COFFIL increased
C                    common VALID added
      CHARACTER LINE*72, FLUNAM*32, COFNAM*64, COFFIL*64
      COMMON/VALID/  IVALID
      COMMON/INDEXX/ IDX(12)
      COMMON/SIZINX/ NDMAX, NTMIN, NTERMS
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/REDUC/  DSTAR, TSTAR, GAMMA, RCONTC
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/ZEROS/  P0, T0, S0, H0, U0
      COMMON/VPCOF/  VP(6)
      COMMON/MELT/   CMELT(6)
C-----Version 3.12  9/21/92; GG added
C-----Version 3.20   RM and NDF added, next two lines    
      COMMON/RCOEF/  RN(NDIM), C(NDIM), GG(NDIM), RM(NDIM)
      COMMON/ICOEF/  M(NDIM), K(NDIM), N(NDIM), NTF(NDIM), NDF(NDIM)
C-----Version 3.1 2/27/92      
      COMMON/IDGAS/  GASID(11), GL(11), GM(11), GH(11)
      COMMON/CUBIC/  COFD2, QF3, RFACT
      COMMON/SRFTEN/ SURF, PSURF, ACENT
      COMMON/DENSAT/ DS(21)
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/DIELCF/ BX(6)
C-----Version 3.20: Old variable RM renamed RMTR2, here and one other line.
      COMMON/TRAN2/  EM,EOK,RMTR2,TC,DC,X,PC,SIG
      COMMON/TRAN5/  BETA,XO,DELTA,E1,E2,AGAM
      COMMON/TRAN6/  GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/TRAN10/ GMW,SEOK,SSIG
      COMMON/TRAN11/ SE1,G1,B1,DE,BK,D1,XZ,ZZ,X1,X2,X3,X4
      COMMON/PRNAME/ FLUNAM
      COMMON/PRCOF/  COFFIL
      COMMON/PRECSN/ EPRCS(0:15)
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1      TPLPRP(0:31), TPVPRP(0:31)
C-----Version 3.20: /SAVEOS/ modified
      COMMON/SAVEOS/ DSAVE, TSAVE, TFR(4), GASPRP(4), DARRAY(25),
     +      TARRAY(53), DFR(2)
C-----Version 3.20: four lines added
      COMMON/UCONVF/ CONVF(NCF)
      COMMON/UFXCVF/ FIXCVF(NCF)
C-----Version 3.30: common VERSON added here
      COMMON/VERSON/ VERS
      DIMENSION JPLUS(9)
      DATA JPLUS /24, 25, 27, 28, 29, 31, 61, 63, 64/
*
      OPEN (NCOFIL, FILE=COFNAM, STATUS='OLD', ERR=80)
* Store the name of the fluid coefficient file in common /PRCOF/
* This will be used to detect when the user changes fluids
      COFFIL= COFNAM
      DSAVE = -1.D0
      TSAVE = -1.D0
C-----Version 3.20: four lines added
      DARRAY(1) = 1.D0
      TARRAY(51) = 1.D0
      TFR(1)= 1.D0
      DFR(1)= 1.D0
* Read indices
      READ (NCOFIL, *, ERR=90, END=95) (IDX(J), J = 1, 12)
*-------------------------------------------------------------
C If IDX(2) >= 2, then this routine reads
c          SE1,G1,B1,DE,BK,D1, XZ,ZZ,X1,X2,X3,X4, SEOK,SSIG
c     and calculates viscosity with different equations     
C-----Version 3.20  7/18/93; IDX(4) = 2 for hydrogens,
C                                   = 3 for more than one gamma
C-----Version 3.30 Nitrogen added to list
C                                   = 4 for nitrogen
C                                   = 1 for most fluids   
* IDX meaning:
* 1=#lines text, 2=transp, 3=melt, 4=special fluid eq., 5=NDmax, 
C 6=NTmin, 7=NTerms 8=transp, 9=dielectric, 10=surface tens, 
C 11=special DSATV eq., 12=Psat eq. 
C-----Version 3.20 uses IDX(11) to select the sat vapor equation
C     IDX(11)=0: use the fitted equation (further defined by AV(14)).
C            =1: use the fitted equation down to T/Tc = 0.5; 
C                use "ideal gas" below that.
C            =2: use "ideal gas" always (for preliminary tests, etc.) 
* ------------------------------------------------------------
* Transfer EOS indices to special commons for faster execution
      NDMAX = IDX(5)
      NTMIN = IDX(6)
      NTERMS= IDX(7)
c-----Version 3.30; next 2 statements added
C Initialize high temperature data for hydrogen and nitrogen
      IF(IDX(4).EQ.2 .OR. IDX(4).EQ.4) CALL DISCOEF (IDX(4))
*
* Read and write fluid reference data
C-----Version 3.1 ; remove write statments; uses IDID for errors
      IF (IDX(1) .LE. 0) THEN
         IDID = -4
C-----Version 3.13 ; set IVALID
         IVALID= -4
         RETURN
      ENDIF
      DO 418 J = 1, (IDX(1))
         READ (NCOFIL, '(1X,A72)', ERR=90, END=95) LINE
         IF (J .EQ. 1)  FLUNAM = LINE(1:32)
C-----Version 3.13; Write statment eliminated
  418 CONTINUE
C-----Version 3.1 ; note END=95 added to READ statements
C-----Version 3.13; Pause eliminated
* Read gas constant and Molecular weight
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
C-----Version 3.30 One line changed, two lines added
      READ (NCOFIL, *, ERR=90, END=95) RCON, XMOLWT
      GMOLWT = XMOLWT
      SMOLWT = XMOLWT
* Read Critical point data
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) PCC, TCC, DCC, GPVN
C-----Version 3.30: following 7 lines: Check for required Gaspak version
C     Note: On coefficient files created prior to March 1998, the
C     variable GPVN was always a number less than 1.
      IF ((VERS - GPVN + 1.D-05) .LT. 0.) THEN
         IDID = -5
         IVALID = -5
         RETURN
      ENDIF           
* Read triple point data
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) PTP, TTP, DTPL, DTPV
* Read range of validity for EOS
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) PUL, TLL, TUL
* Read zeros for S, H and U (S requires P0)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) P0, T0, S0, H0, U0
* Read vapor pressure coefficients
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (VP(J), J=1,6)
* Density and temperature at intersection of melting line and PUL
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) DMM, TMM
* Melting line coefficients
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (CMELT(J), J=1,6)
* Read paramters for near-critical saturation densities
*     and temperature of the max vapor enthalpy
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) TNRC1, TNRC2, THVM
* Read saturation density coefficients
* 1-13 are for vapor, 15-21 are for liquid.
* DS(14) = the old NW parameter (0 OR 1)
*
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95)  (DS(J), J=1, 21)
* Read ideal gas coefficients
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
C-----Version 3.1 2/27/92
      IF (IDX(4) .EQ. 2) THEN
         READ (NCOFIL,*,ERR=90,END=95) (GL(J),GM(J),GH(J),J=1,11)
      ELSE
         READ (NCOFIL,*,ERR=90,END=95) (GASID(J),DUM1,DUM2, J=1,11)
      ENDIF
  405 CONTINUE
*
* Read parameters for non-dimensional state equation
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) DSTAR, TSTAR, GAMMA, RCONTC
*
* Read EOS coefficients
      DO 420 J = 1, NTERMS
C-----Version 3.12  9/21/92; JJ changed to GG(J)
         READ (NCOFIL,*,ERR=90, END=95) GG(J), RM(J), K(J), RN(J), C(J)
* RN is the exponent on T**RN in the EOS.
* This program assumes the decimal part of RN is .00, .25, .50, or .75
         N(J) = NINT (RN(J) - 0.45D0)
         DELR = RN(J) - DBLE(N(J))
         IF (ABS (DELR) .LT. RNDOFF) THEN
            NTF(J) = 0
         ELSE IF (ABS (DELR - 0.5D0) .LT. RNDOFF) THEN
            NTF(J) = 1
         ELSE IF (ABS (DELR - 0.25D0) .LT. RNDOFF) THEN
            NTF(J) = 2
         ELSE IF (ABS (DELR - 0.75D0) .LT. RNDOFF) THEN
            NTF(J) = 3
         ELSE
C-----Version 3.1
            IDID = -4
C-----Version 3.13 ; set IVALID
            IVALID = -4
            RETURN
         ENDIF
C-----Version 3.20  Allow for fraction exponents on D (required for R152)
* RM is the exponent on D**RM in the EOS.
* This program assumes the decimal part of RM is .00 or .50
         M(J) = NINT (RM(J) - 0.45D0)
         DELR = RM(J) - DBLE(M(J))
         IF (ABS (DELR) .LT. RNDOFF) THEN
            NDF(J) = 0
         ELSE IF (ABS (DELR - 0.5D0) .LT. RNDOFF) THEN
            NDF(J) = 1
         ELSE
            IDID = -4
            IVALID = -4
            RETURN
         ENDIF
  420 CONTINUE
C-----Version 3.12  9/21/92; If loop added
C-----Version 3.20  7/18/93: changes the following line
      IF (IDX(4) .NE. 3) THEN
         DO 425 J = 1, NTERMS
  425    GG(J) = 1.D0
      ENDIF
* Read or calculate surface tension parameters
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) SURF, PSURF
C-----Version 3.03 January 12, 1992  .GT. changed to .EQ. in next line.
C-----Version 3.13 10/14/92: ACENT calculated in all cases.
      CALL PSFT (PSAT7, DUM, (0.7D0*TCC))
      ACENT = LOG10 (PCC/PSAT7) - 1.D0
* Read transport property coefficients
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (GV(J), J=1,9)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (GT(J), J=1,9)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (FV(J), J=1,4)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (FT(J), J=1,4)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (EV(J), J=1,8)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (ET(J), J=1,8)
* Dielectric constant coefficients (6)
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      READ (NCOFIL, *, ERR=90, END=95) (BX(J), J=1,6)
* Read transport parameters
      READ (NCOFIL, '(A72)', ERR=90, END=95) LINE
      DC = DCC*GMOLWT*1.D-03
      PC = PCC*1.D-03
      TC = TCC
      GMW= GMOLWT
      EM = GMW
      READ (NCOFIL, *, ERR=90, END=95) SIG,XO,BETA,DELTA,E1,E2,AGAM
      READ (NCOFIL, *, ERR=90, END=95) EM,EOK,RMTR2,X
* Read additional fluid transport parameters if required
      IF (IDX(2) .GT. 1) THEN
         READ (NCOFIL, *, ERR=90, END=95) SE1,G1,B1,DE,BK,D1
         READ (NCOFIL, *, ERR=90, END=95) XZ,ZZ,X1,X2,X3,X4
         READ (NCOFIL, *, ERR=90, END=95) SEOK,SSIG
      ENDIF
*
* --------- Miscellaneous setup factors -----------
*
      XLL = 0.D0
      PLL = 1.D-08
      DLL = 1.D-08
      XUL = 1.D0
* Use default precision if the user has not set it.
      IF (EPRCS(0) .LE. 0.D0) CALL PRECIS (2)
*
* Factors required for solving cubic equation for density (P,T)
*
      COFD2 = 0.4D0
      QF3   = (DCC*DCC/(3.D0*COFD2))**3
      RFACT = 0.5D0*DCC*DCC/(COFD2*RCON)
*
* Calculate parameters for near-critical saturation density equations
      DSLC = DSATL (TNRC1-1.D-05)
      DSVC = DSATV (TNRC1-1.D-05)
      TDEL = TNRC1 - TNRC2
*
* Logarithmic slope of the saturation line (typically about 7)
*
      CALL PSFT (PP, DPDT, TNRC1)
      THETA = TNRC1*DPDT/PP
*
* Reference data used by iterative routines
*
      CALL EOS (CPPRP, DCC, TCC)
      CALL EOS (TPLPRP, DTPL, TTP)
      CALL EOS (TPVPRP, DTPV, TTP)
C-----Version 3.11, following line
      CALL EOS (HIPROP, (0.004D0*DCC), (MIN(8.D0*TCC, TUL)))
      CALL DERIV (CPPRP)
      CALL DERIV (TPLPRP)
      CALL DERIV (TPVPRP)
      CALL DERIV (HIPROP)
C-----Version 3.20, Set units conversion factors (15 fortran lines):
      DO 426 J = 1, NCF
         CONVF(J) = FIXCVF(J)
  426 CONTINUE
      CONVF (16) = FIXCVF(16) / GMOLWT
      CONVF (17) = FIXCVF(17) / GMOLWT
      CONVF (19) = FIXCVF(19) / GMOLWT
      CONVF (51) = PCC
      CONVF (52) = TCC
      CONVF (53) = DCC
      CONVF (55) = RCON
      CONVF (56) = RCON*TCC
      DO 428 J = 1, 9
         NJ = JPLUS(J)
         CONVF(NJ) = FIXCVF(NJ) * GMOLWT
  428 CONTINUE
*
* Normal return
*
      CLOSE (NCOFIL)
      IDID = 0
C-----Version 3.13 ; set IVALID
      IVALID = 1
      RETURN
*
* File or reading errors:
*
C-----Version 3.1
C-----Version 3.12  NCOFIL closed before return in all 3 cases below.
C-----Version 3.13  Set IVALID
C                
80    CONTINUE
      IDID = -1
      GO TO 99
90    CONTINUE
      IDID = -2
      GO TO 99
95    CONTINUE
      IDID = -3
C     GO TO 99
99    CONTINUE
C-----Version 3.13 ; set IVALID
      IVALID = IDID
      CLOSE (NCOFIL)
      RETURN
      END
C
C
C
      SUBROUTINE GASCON (CONST, GCUNIT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
C-----Version 3.13, following subroutine is new
C-----Version 3.20 Units conversion added.
C
C CONST returns fluid state constants obtained from READCF, converted to 
C     the user's units.  
C     See USER.FOR for a complete listing of units for all parameters.
C
C Const( 0) = 1 for valid constants
C           = 0 prior to initializtion.
C           = (negative number) when READCF failed;
C Const( 1) = the Gas Constant used in the equation of state
C Const( 2) = Molecular Wt.
C Const( 3) = Critical point pressure
C Const( 4) = Critical point temperature
C Const( 5) = Critical point density
C Const( 6) = Triple point pressure
C Const( 7) = Triple point temperature
C Const( 8) = Triple point liquid density
C Const( 9) = Triple point vapor density
C Const(10) = compressibility factor = PV/RT at the critical point
C Const(11) = lower limit of Temperature 
C Const(12) = upper limit of Temperature
C Const(13) = upper limit of Pressure 
C Const(14) = Density at the melting line at upper pressure limit.
C Const(15) = Temperature at the melting line at upper pressure limit.
C Const(16) > 0 indicates that transport properties are available.
C Const(17) > 0 indicates that dielectric properties are available.
C Const(18) > 0 indicates that surface tension constants are available.
C Const(19) = acentric factor, used for scaled surface tension equation.
C Const(20) = GASPAK version number.
C Const(21) = (reserved for future use)
C
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (NCF=65)
C Following 3 lines required in the calling program
      PARAMETER (NA=21)
      CHARACTER*11 GCUNIT, UNITS
      DIMENSION CONST(0:NA), GCUNIT(0:NA)
      DIMENSION JU(0:NA)
      COMMON/INDEXX/ IDX(12)
C     COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/POINTS/ A1(8)
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON,  GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/SRFTEN/ SURF, PSURF, ACENT
      COMMON/VERSON/ VERS
      COMMON/VALID/  IVALID
C-----Version 3.20, following 4 lines added
      COMMON/UCONVF/ CONVF(NCF)
      COMMON/UNITLB/ UNITS (NCF)
      COMMON/UUNITS/ KUNITS(0:31)
      DATA JU /0,0,0,1, 2,3,1,2, 3,3,0,2, 2,1,3,2, 0,0,0,0, 0,0/
C
      CONST(0) = IVALID
      IF (IVALID .NE. 1) THEN
         DO 10 J = 1, NA
10       CONST(J) = 0.D0
         RETURN
      ENDIF
      CKU1 = 1./CONVF(KUNITS(1))
      CKU2 = 1./CONVF(KUNITS(2))
      CKU3 = 1./CONVF(KUNITS(3))
      CKU4 = 1./CONVF(KUNITS(4))
      CONST(1) = RCON*CKU4
      CONST(2) = GMOLWT
      DO 20 J = 1, 8
         CONST(J+2) = A1(J)/CONVF(KUNITS(JU(J+2)))
20    CONTINUE
      CONST(11) = TLL*CKU2
      CONST(12) = TUL*CKU2
      CONST(13) = PUL*CKU1
      CONST(14) = DMM*CKU3
      CONST(15) = TMM*CKU2
      CONST(16) = REAL (IDX(8))
      CONST(17) = REAL (IDX(9))
      CONST(18) = REAL (IDX(10))
      CONST(19) = ACENT
      CONST(20) = VERS
      DO 30 J = 0, NA
         GCUNIT(J) = UNITS(KUNITS((JU(J))))
30    CONTINUE
      GCUNIT(2) = ' [g/mol]'
      END                
C
C
C
      SUBROUTINE EOS (PROP, DD, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* This is the primary equation of state.
* Input:   DD [mol/L] and TT [K]; D and T are dimensionless.
* Output:  Array PROP (1 to 9).  For definitions, see subroutine CALC
*
C-----Version Sept. 21, 1992
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ZERO=0.D0, NDIM=50, DMIN=1.D-10)
C-----Version 3.20: TARRAY dimension changed
      DIMENSION DARRAY(0:24), TARRAY(-50:2)
C-----Version 3.20  DFR added, dimensioned
      DIMENSION PROP(0:31), GASPRP(4), TFR(0:3), DFR(0:1)
c-----Version 3.30 commons DISCOF and INDEXX added, FLCON modified
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
      COMMON/INDEXX/ IDX(12)
      COMMON/FLCON/  RCON,  GMOLWT, XMOLWT, SMOLWT,XDIS
      COMMON/REDUC/  DSTAR, TSTAR, GAMMA,  RCONTC
      COMMON/SIZINX/ NDMAX, NTMIN, NTERMS
      COMMON/ZEROS/  P0, T0, S0, H0, U0
C-----Version 3.12  9/21/92; GG added
C-----Version 3.20  RM and NDF added, next two lines
      COMMON/RCOEF/  RN(NDIM), C(NDIM), GG(NDIM), RM(NDIM)
      COMMON/ICOEF/  M(NDIM), K(NDIM), N(NDIM), NTF(NDIM), NDF(NDIM)
C-----Version 3.20: DFR added to /SAVEOS/
      COMMON/SAVEOS/ DSAVE, TSAVE, TFR, GASPRP, DARRAY, TARRAY, DFR
C-----Version 3.20: TI saved
      SAVE TI
C     COMMON/TEST2/  ITER
C
C     ITER = ITER + 1
*
* Change to dimensionless coordinates.
*
      D = DD/DSTAR
      T = TT/TSTAR
*
* Reset output array to 0 (note, PROP(31) may be input parameter).
C-----Version 3.20 Eliminate this step.  It should always be done
C     by the calling routine, if necessary.  Otherwise old data
C     in PROP will merely be overwritten by subsequent action of
C     the calling routine(s).  
*
C      DO 422 J = 0, 29
C         PROP(J) = 0.D0
C  422 CONTINUE
*
* Update D array
*
      IF (D .NE. DSAVE) THEN
         DSAVE = D
C-----Version 3.20: DFR defined
         DFR(1)= SQRT(D)
         DO 424 J = 1, NDMAX
            DARRAY(J) = D*DARRAY(J-1)
  424    CONTINUE
      ENDIF
*
* Update T-array and ideal gas properties array
* GASPRP units are [J/mol] and [J/mol-K]
*
      IF (T .NE. TSAVE) THEN
         IF (T .LE. ZERO) RETURN
         TSAVE = T
C-----Version 3.20; TI defined here, used 2 places for faster calculations.
         TI = 1.D0/T
C-----Version 3.20: TFR(0) is defined in READCF
         TFR(1) = SQRT (T)
         TFR(2) = SQRT (TFR(1))
         TFR(3) = TFR(2)*TFR(1)
C-----Version 3.20: TARRAY(0) is defined in READCF
         TARRAY(1) = T
         TARRAY(2) = T*T
         DO 426 J = -1, NTMIN, -1
            TARRAY(J) = TARRAY(J+1)*TI
  426    CONTINUE
         CALL DILGAS (GASPRP, TT)
      ENDIF
*
* M is the exponent on D, N is the exponent on T
* HELM is the Helmholz energy, which is not included in the output.
* K=0 terms should come first in the coefficient file.
      HELM   = ZERO
      PRESS  = ZERO
      DPDD   = ZERO
      DPDT   = ZERO
      ENTROP = ZERO
      CV     = ZERO
      GDK    = ZERO
      EGDK   = 1.D0
      RKJ    = ZERO
      RGDK   = ZERO
      KJ     = 0
C-----Version 3.12  9/21/92; GJ defined
      GJ     = 1.D0
      DO 428 J = 1, NTERMS
*
* Read parameters for this term; calculate indexed factors only once.
*
C-----Version 3.12  9/21/92; GJ tested
         IF ((KJ .NE. K(J)) .OR. (GJ .NE. GG(J))) THEN
C-----Version 3.12  9/21/92; GJ defined
            GJ  = GG(J)
            KJ  = K(J)
            RKJ = DBLE (KJ)
C-----Version 3.12  9/21/92; GJ used
            GDK = GJ * GAMMA * DARRAY(KJ) * MIN (1, KJ)
            EGDK= EXP (-GDK)
            RGDK= GDK * RKJ
         ENDIF
         MJ  = M(J)
         RNJ = RN(J)
         NJ  = N(J)
         CJ  = C(J)
* Helmholtz factors
C------Version 3.20: next line permits fractional powers on rho
         DHELM  = DARRAY (MJ) * EGDK * DFR(NDF(J))
         THELM  = TARRAY (NJ) * TFR (NTF(J))
         HELM   = HELM + CJ*DHELM*THELM
C------Version 3.20: next line changed
         ZZZ    = RM(J) - RGDK
* Thermodynamic parameters
         DPRESS = ZZZ*D*DHELM
         PRESS  = PRESS + CJ*DPRESS*THELM
         DPDD   = DPDD + CJ*DHELM*THELM*(ZZZ*(ZZZ+1.D0) - RKJ*RGDK)
         DADT   = RNJ * THELM * TI
         DPDT   = DPDT + CJ*DPRESS*DADT
         SJ     = DADT * CJ * DHELM
         ENTROP = ENTROP - SJ
         CV     = CV + (1.D0 - RNJ)*SJ
  428 CONTINUE
*
* Collect terms into the output array; add ideal gas terms
*
      IF (DD .LE. DMIN) THEN
C-----Version 3.30: DI defined, eliminates need for MAX fn in PROP5      
        RZLOG = RCON * LOG (DMIN*RCON*TT/P0)
        DI    = 1./DMIN
      ELSE
        RZLOG = RCON * LOG (DD*RCON*TT/P0)
        DI    = 1./DD
      ENDIF
C-----Version 3.30 the next 4 statements added
      IF ((IDX(4).EQ.2 .OR. IDX(4).EQ.4) .AND. TT.GT.TUP2) THEN
         PP = (PRESS*RCONTC*DSTAR+RCON*DD*TT)/101.325
         CALL IDGDIS (GASPRP,PP,TT)
      END IF
      HELM    = HELM*RCONTC + TT*RZLOG + GASPRP(3) - TT*(GASPRP(1)+S0)
      PROP(1) = PRESS*RCONTC*DSTAR + RCON*DD*TT
      PROP(2) = TT
      PROP(3) = DD
      PROP(4) = ENTROP*RCONTC/TSTAR - RZLOG + GASPRP(1) + S0
      PROP(6) = HELM + TT*PROP(4) + U0
Cv3.30PROP(5) = PROP(6) + PROP(1)/MAX (DD,DMIN) + H0
      PROP(5) = PROP(6) + PROP(1)*DI + H0
      PROP(7) = DPDD*RCONTC + RCON*TT
      PROP(8) = DPDT*RCONTC*DSTAR/TSTAR + RCON*DD
      PROP(9) = CV*RCONTC/TSTAR + GASPRP(4)
*
      RETURN
      END
C
C
C
      SUBROUTINE DERIV (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
*
* The array PROP is used for both input and output.
* On input, PROPs(1:9) have been calculated by EOS.
* In this routine, derivatives PROPs(10:20) are calculated.
*  PROP(10) = Cp    = specific heat at constant P         [J/mol-K]
*  PROP(11) = Gamma = Cp/Cv                               [-]
*  PROP(12) = Alpha = (T/V)(dV/dT) at constant P          [-]
*  PROP(13) = Grun  = (V/Cv)(dP/dT) at constant V         [-]
*  PROP(14) = kT    = (1/D)(dD/dP) at constant T          [1/KPa]
*  PROP(15) = C     = Velocity of sound                   [m/s]
*  PROP(16) = JT    = Joule-Thomson coefficient           [Kelvin/Pa]
*  PROP(17) = dH/dV*V at constant P                       [J/mol]
*  PROP(18) = dH/dT at constant V                         [J/mol-K]
*  PROP(19) = dS/dT at constant V                         [J/mol-K2]
*  PROP(20) = C2mol = (velocity of sound)^2 * Molwt       [J/kilo-mol]
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      PROP(14) = 1.D0/(PROP(3)*PROP(7))
      PROP(12) = PROP(2)*PROP(8)*PROP(14)
      PROP(13) = PROP(8)/(PROP(3)*PROP(9))
      PROP(11) = 1.D0 + PROP(12)*PROP(13)
      PROP(10) = PROP(9)*PROP(11)
      PROP(20) = PROP(11)*PROP(7)
* Avoid stopping the computer if an invalid sound speed is encountered.
      PROP(15) = SQRT (MAX ((1000.D0*PROP(20)/GMOLWT), 1.D0))
      PROP(16) = (PROP(12)-1.D0)/(PROP(3)*PROP(10))
      PROP(17) = PROP(20)/PROP(13)
      PROP(18) = PROP(9)+PROP(8)/PROP(3)
      PROP(19) = PROP(9)/PROP(2)
      RETURN
      END
C
C
C
      SUBROUTINE DILGAS (GAS0P, T)
* (C) Copyright (1990), Cryodata Inc.; all rights reserved.
*
* Calculates entropy, enthalpy, energy and Cv for the gas at zero P.
* GAS0P(1:4) = S, H, U, Cv
*
*  Output is in J/(MOL*K), for Cv and S,  and J/MOL for H and U.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C-----Version 3.1 2/27/92      
      COMMON/IDGAS/  G(11), GL(11), GM(11), GH(11)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/INDEXX/ IDX(12)
      DIMENSION GAS0P(4)
C-----Version 3.1 2/27/92      
      IF (IDX(4) .EQ. 2) THEN
C Hydrogens are a special case
         IF (T .GT. 140.D0) THEN
            DO 420 J = 1, 11
  420       G(J) = GH(J)
         ELSE IF (T .GT. 40.D0) THEN
            DO 430 J = 1, 11
  430       G(J) = GM(J)
         ELSE
            DO 440 J = 1, 11
  440       G(J) = GL(J)
         ENDIF
      ENDIF
      U      = MIN ((G(9)/T), 80.D0)
      EU     = DEXP (U)
      EUM1   = EU-1.D0
C-----Version 3.1 2/27/92 : several lines     
      EUEUM1 = EU/EUM1
      G8UEM1 = G(8)*U/EUM1
      CP     = G8UEM1*U*EUEUM1
      DLOGT  = DLOG(T)
      ENTROP = G8UEM1 + G(8)* DLOG(EUEUM1)
     1         - ((G(1)/(3.D0*T) + G(2)/2.D0)/T + G(3))/T
     2         + G(4)*DLOGT + T*(G(5) + T*(G(6)/2.D0 + T*G(7)/3.D0))
      ENTROP = ENTROP*RCON + G(11)
      ENTHAL = G8UEM1*T - (G(1)/(2.D0*T) + G(2))/T
     1         + G(3)*DLOGT + T*(G(4) + T*(G(5)/2.D0 + T*(G(6)/3.D0
     2         + T*G(7)/4.D0)))
      ENTHAL  = ENTHAL*RCON + G(10)
      TS =1.D0/T**4
      DO 25 I=1,7
      TS = TS*T
   25 CP = CP + G(I)*TS
      CP = CP*RCON
      GAS0P(1) = ENTROP
      GAS0P(2) = ENTHAL
      GAS0P(3) = ENTHAL - T*RCON
      GAS0P(4) = CP - RCON
      END
C
C
C
C-----Version 3.30: New function, fugacity calculation.
      SUBROUTINE FUGAC (IDID, PROP)
* (C) Copyright (1997), Cryodata Inc.; all rights reserved.
C This function is called (from CALC) after props(1:9) are calculated.
C IDID is also an input parameter. 
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31, 0:2)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/ZEROS/  P0, T0, S0, H0, U0
      COMMON/SAVEOS/ DUM1(6), GASPRP(4), DUM2(80)
      IF ((IDID .EQ. 1) .OR. (IDID .EQ. 3)) THEN
         J=0
      ELSE IF ((IDID .EQ. 2) .OR. (IDID .EQ. 4)) THEN
         J=1
      ELSE
         J=2
      ENDIF
      T = PROP(2,J)
      P = PROP(1,J)
      GIBBS = PROP(5,J) - T*PROP(4,J)
      TARE  = GASPRP(3) + U0 + H0 - T*(GASPRP(1) + S0)
      DGIBB = GIBBS - TARE - RCON*T*DLOG(P/P0)
      FUG = EXP (DGIBB/(RCON*T) - 1.)
      PROP(30,0) = FUG
      PROP(30,1) = FUG
      PROP(30,2) = FUG
      RETURN
      END
C
C
C
      SUBROUTINE FDT (IDID, PROP, DD, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Calculates properties (1:9) as a function of density [mol/L] and T[K].
* Valid for two-phase region as well as single phase.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31, 0:2)
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      D = DD
      T = TT
      IF ((D .LT. DLL) .OR. (D .GT. DMM+0.1D0)) THEN
         IDID = -103
         RETURN
      ELSE IF ((T .LT. TLL) .OR. (T .GT. TUL+0.1)) THEN
         IDID = -102
         RETURN
C-----Version 3.11:  TTP changed to TLL in following line
      ELSE IF ((IBTWNR (TLL, T, TCC) .GT. 0)  .AND.
     +    (IBTWNR (DTPL, D, DTPV) .GT. 0)) THEN
         IF (D .LT. 1.01D0*DSATL(T)) THEN
            IF (D .GT. 0.99D0*DSATV(T)) THEN
               CALL PSFT (P, DPDT, T)
               CALL SLFPT (IDID, PROP(0,1), P, T)
               IF (D .LT. PROP(3,1)) THEN
                  CALL SVFPT (IDID, PROP(0,2), P, T)
                  IF (D .GT. PROP(3,2)) THEN
                     X = QFD (PROP(3,1), D, PROP(3,2))
                     CALL MIXPRP (PROP(0,0), PROP(0,1), PROP(0,2),
     +                     X, DPDT)
                     IDID = 3
                     RETURN
                  ENDIF
               ENDIF
            ENDIF
         ENDIF
      ENDIF
C-----Version 3.11:  TTP changed to TLL in following line
      IF ((IBTWNR (TLL, T, TMM) .GE. 0) .AND. (D .GT. DTPL)) THEN
         CALL MELTFT (IDID, PROP, T)
         IF (D .GT. PROP(3,0)) THEN
            IDID = -110
            RETURN
         ENDIF
      ENDIF
      CALL EOS (PROP, D, T)
      CALL DERIV (PROP)
      IDID = 1
      RETURN
      END
C
C
C
C------------------------------------------ End state properties
C------------------------------------------ Saturation properties
C
C
C
      DOUBLE PRECISION FUNCTION DSATL (TT)
* (C) Copyright (1990), Cryodata Inc.; all rights reserved.
* Density of saturated liquid [mol/L] as a function of temperature [K]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PARAMETER (U1=1.D0, ONE3RD=1.D0/3.D0)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/DENSAT/  AV(14), AL(7)
      T  = TT
      IF (T .GE. TCC) THEN
         DSATL=DCC
      ELSE IF (T .GT. TNRC1) THEN
         DSATL = DCC + (DSLC-DCC)*((TCC-T)/(TCC-TNRC1))**ONE3RD
      ELSE
         X  = (T-TCC)/(TTP-TCC)
         X3 = X**ONE3RD
         Y1 = AL(1)*LOG(X) + AL(2)*(U1-U1/X) + AL(3)*(U1-X3/X)
     A        + AL(4)*(U1-U1/X3) + AL(5)*(U1-X3) + AL(6)*(U1-X3*X3)
     B        + AL(7)*(U1-X)
         DSATL = DCC+ (DTPL-DCC)*EXP(Y1)
      ENDIF
      RETURN
      END
C
C
C
      DOUBLE PRECISION FUNCTION DSATV (TT)
* (C) Copyright (1993), Cryodata Inc.; all rights reserved.
* Calculates density [mol/L] of saturated vapor.
* Input is temp[K], not checked for valid range.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
C-----Version 3.20 modifies the following line:
      PARAMETER (U1=1.D0, ONE3RD=1.D0/3.D0, TWO3RD=2.D0/3.D0)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/DENSAT/  AV(14), AL(7)
C-----Version 3.20 adds the following three lines:
      COMMON/INDEXX/ IDX(12)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      DATA C1, C2, C3 /0.3545937691, 1.639536535, -1.014034759/
      T=TT
      IF (T .GE. TCC) THEN
         DSATV = DCC
         RETURN
      ELSE IF (T .GT. TNRC1) THEN
         DSATV = DCC + (DSVC-DCC)*((TCC-T)/(TCC-TNRC1))**ONE3RD
         RETURN
      ENDIF
C-----Version 3.20 uses IDX(11) to select the equation
C     IDX(11)=0: use the standard equation (further defined by AV(14)).
C            =1: use the standard equation down to T/Tc = 0.5; 
C                use "ideal gas" below that.
C            =2: use "ideal gas" always (for preliminary tests, etc.) 
C-----Version 3.20 modifies the following line:
      IF ((IDX(11) .EQ. 0) .OR. ((IDX(11) .EQ. 1) .AND. 
     +    (T .GE. 0.5*TCC))) THEN
         X  = (TCC-T)/(TCC-TTP)
         X3 = X**ONE3RD
         X2 = X*X
         IF (AV(14) .EQ. 0.D0) THEN
            Y1 = AV(1)*LOG(X) + AV(2)*(U1-U1/X) + AV(3)*(U1-X3/X)
     A           + AV(4)*(U1-U1/X3) + AV(5)*(U1-X3) + AV(6)*(U1-X3*X3)
     B           + AV(7)*(U1-X) + AV(8)*(U1-X*X3)   + AV(9)*(U1-X2/X3)
     C           + AV(10)*(U1-X2)       + AV(11)*(U1-X2*X3)
     D           + AV(12)*(U1-X2*X3*X3) + AV(13)*(U1-X2*X)
            DSATV = DCC + (DTPV-DCC)*EXP(Y1)
         ELSE
*           AV(8) = YN = - LOG(DCC/DTPV)
            Y1 = AV(1)*TCC*X/T + AV(2)*X**AV(7) + X*(AV(3)
     A           + X3*(AV(4) + X3*(AV(5) + X3*AV(6))))
            DSATV = DCC*EXP(AV(8)*Y1)
         ENDIF
         IF ((IDX(11) .EQ. 0) .OR. (T .GE. 0.65D0*TCC)) RETURN
      ENDIF
C-----Version 3.20 adds the following lines (down to RETURN statement).
      Y1 = T/TCC
      X = (1.D0 - Y1) ** TWO3RD
      PVRT = C1 + X*(C2 + X*C3)
      CALL PSFT (PSAT, DPDT, T)
      X = PSAT / (RCON*T*PVRT)
      IF (IDX(11) .EQ. 1) THEN
         DSATV = ((0.65D0-Y1)*X + (Y1-0.5D0)*DSATV) / 0.15D0
      ELSE
         DSATV = X
      ENDIF
      RETURN
      END
C
C
C
      SUBROUTINE MIXPRP (PM, SL, SV, X, DPDTS)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: array PM(0:24) (= Mixprop);
*         averaged pressures in SL and SV
* Input:  arrays SatLiquid & SatVapor, quality (=X), dP/dTsat
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PM(0:31), SL(0:31), SV(0:31)
      OMX   = 1.D0-X
      PM(0) = X
* The pressures should be identical, but, on input, they may not be
* because of inaccuracy of the state equation when near critical.
* It is reasonable to set them equal to the average.
      PAVG  = 0.5D0*(SL(1) + SV(1))
      PM(1) = PAVG
      SL(1) = PAVG
      SV(1) = PAVG
      PM(2) = SL(2)
      PM(3) = SL(3)*SV(3)/(X*SL(3) + OMX*SV(3))
      PM(4) = X*SV(4) + OMX*SL(4)
      PM(5) = X*SV(5) + OMX*SL(5)
      PM(6) = X*SV(6) + OMX*SL(6)
      DO 430 J = 7, 24
         PM(J) = 0.D0
  430 CONTINUE
C-----Version 3.20  Change location of output parameters
      SL(0) = 0.
      SV(0) = 1.D0
      SL(28) = DPDTS
      SV(28) = DPDTS
      PM(28) = DPDTS 
      SL(29) = DPDTS*SV(2)*(1.D0/SV(3) - 1.D0/SL(3))
      SV(29) = SV(5) - SL(5)
      PM(29) = SV(29)*OMX
      END
C
C
C
      SUBROUTINE PSFT (PSAT, DPDT, T)
* (C) Copyright (1990), Cryodata Inc.; all rights reserved.
* Calculates vapor pressure [KPa], dP/dT (saturation line) [KPa/K]
* Input is Temp [K], not checked for valid range.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/VPCOF/  VP(6)
      COMMON/INDEXX/ IDX(12)
      X = 1.D0/(1.D0-TTP/TCC)
      DXDT = (TTP/T**2)*X
      X    = (1.D0-TTP/T)*X
      IF (X .LT. 1.D0) THEN
         SUM0 = VP(5)*(1.D0-X)**VP(6)
         SUM1 = SUM0*(1.D0 - VP(6)*X/(1.D0-X))
      ELSE
         SUM0 = 0.D0
         SUM1 = 0.D0
      ENDIF
      SUM0 = X*(SUM0 + VP(1) + X*(VP(2) + X*(VP(3) + X*VP(4))))
      SUM1 = DXDT*(SUM1 + VP(1) + X*(2.D0*VP(2) + X*(3.D0*VP(3)
     +          + X*4.D0*VP(4))))
      PSAT = PTP * EXP (SUM0)
      DPDT = PSAT * SUM1
      END
C
C
C
      FUNCTION QFD (DL, D, DV)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Calculates Quality from liquid, mixture, and vapor densities
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      IF (DL .EQ. DV) THEN
         QFD = 0.5D0
      ELSE
         QFD = (DL/D - 1.D0) / (DL/DV - 1.D0)
      ENDIF
      END
C
C
C
      SUBROUTINE SATFD (IDID, PROP, RHO)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: EOS and derivative properties in the 1-D array PROP.
*         The calling routine should transfer this to the appropriate
*         2-D array.  IDID = 1 in liquid, 2 in vapor.
* Input:  RHO = saturated fluid density [mol/liter].
C Note: This function relies on the saturation density equation(s).
C If saturation pressure or temperature calculated from this equation is
C subsequently used as (saturation) input to CALC, the then-calculated
C density will differ slightly from RHO due to inaccuracy in the 
C density equation(s).
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/PRECSN/ E(0:15)
      EXTERNAL DSATL, DSATV
      D = RHO
      IF (D .GT. DTPL) THEN
         IDID = -113
         RETURN
      ELSE IF (D .GT. DSLC) THEN
         FF = (DTPL-D)/(DTPL-DSLC) + 1.D-06
         TA = TTP + (TNRC1-TTP)*FF
         TB = TTP + (TNRC1-TTP)*(FF**0.25) + 0.005D0*TCC
C-----Version 3.11; Density limits changed.
         CALL ROOTAZ (TSAT, TTP, TA, TB, TCC, E(5), E(9), E(10), E(11),
     +         DSATL, D, JX)
         IF (JX .GT. 0) THEN
            IDID = 1
         ELSE
            IDID = -203
            RETURN
         ENDIF
      ELSE IF (D .GE. DSVC) THEN
         IF (D .GT. DCC) THEN
            TSAT = TCC + (TNRC1-TCC)*((D-DCC)/(DSLC-DCC))**3
            CALL PSFT (PSAT, DPDTS, TSAT)
            IDID = 1
         ELSE IF (D .EQ. DCC) THEN
            TSAT = TCC
            PSAT = PCC
            IDID = 1
         ELSE
            TSAT = TCC + (TNRC1-TCC)*((DCC-D)/(DCC-DSVC))**3
            CALL PSFT (PSAT, DPDTS, TSAT)
            IDID = 2
         ENDIF
         CALL EOS (PROP, D, TSAT)
         CALL DERIV (PROP)
C-----Version 3.01  Dec. 8, 1991; following line added
         PROP(0) = DPDTS
* Near critical (T > TNRC1): overwrite P(D,T) with Psat(T)
         PROP(1) = PSAT
         RETURN
      ELSE IF (D .GE. DTPV) THEN
         FF = (D-DTPV)/(DSVC-DTPV) + 1.D-06
         TA = TTP + (TNRC1-TTP)*FF
         TB = TTP + (TNRC1-TTP)*(FF**0.25) + 0.005D0*TCC
C-----Version 3.1
C-----Version 3.11  Change error limits on density
         CALL ROOTAZ (TSAT, TTP, TA, TB, TCC, E(5), E(9), E(15), E(10),
     +         DSATV, D, JX)
         IF (JX .GT. 0) THEN
            IDID = 2
         ELSE
            IDID = -203
            RETURN
         ENDIF
      ELSE
         IDID = -113
         RETURN
      ENDIF
C-----Version 3.11 Single iteration replaced by Do loop.
* Iteration to obtain consistency with vapor pressure equation
      DO 456 J = 1, 10
      CALL EOS (PROP, D, TSAT)
      CALL PSFT (PS, DPDTS, TSAT)
      DT = (PS - PROP(1)) / (DPDTS - PROP(8))
      IF (ABS(DT) .LT. E(6)) THEN
         CALL DERIV (PROP)
C-----Version 3.01  Dec. 8, 1991; following line added
         PROP(0) = DPDTS
         RETURN
      ENDIF
      TSAT = TSAT - DT
      IF ((TSAT .LT. TTP-1.D-05) .OR. (TSAT .GT. TCC+1.D-05)) THEN
         IDID = -203
         RETURN
      ENDIF
  456 CONTINUE
      IDID = -203
      END
C
C
C
      SUBROUTINE SLFPT (IDID, PROP, PSAT, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Input: saturation pressure PSAT [Pa]; sat temp TT [K]
* Output: saturated Liquid properties, after iteration on density
*         to obtain P(D,T) = PSAT
*         (iteration is not done very near the critical density)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
C-----Version 3.11:  line added
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      DIMENSION PROP(0:31), PROPC(0:31)
      T = TT
      D = DSATL (T)
C-----Version 3.01, following line added Nov. 14, 1991
      IDID = 1
C-----Version 3.11:  TTP changed to TLL in following line
      IF (IBTWNR (TLL, T, TCC) .LT. 0) THEN
         IDID = -112
         RETURN
      ELSE IF (T .LT. TNRC1) THEN
         CALL EOS (PROP, D, T)
         CALL DJPT (IDID, PROP, D, PSAT, T)
         IF (IDID .LT. 0) THEN
            IDID = -204
            RETURN
         ENDIF
         IF (T .LE. TNRC2) RETURN
      ELSE
         CALL EOS (PROP, D, T)
         RETURN
      ENDIF
      CALL EOS (PROPC, D, T)
      F   = (T - TNRC2) / TDEL
      OMF = 1.D0-F
      DO 10 J = 1, 9
         PROP(J) = F*PROPC(J) + OMF*PROP(J)
10    CONTINUE
      RETURN
      END
C
C
C
      SUBROUTINE SVFPT (IDID, PROP, PSAT, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Input: saturation pressure PSAT [Pa]; sat temp TT [K]
* Output: saturated Vapor properties, after iteration on density
*         to obtain P(D,T) = PSAT
*         (iteration is not done very near the critical density)
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
C-----Version 3.11:  line added
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      DIMENSION PROP(0:31), PROPC(0:31)
      T = TT
      D = DSATV (T)
C-----Version 3.01; following line added Nov. 14, 1991
      IDID = 1
C-----Version 3.11:  TTP changed to TLL in following line
      IF (IBTWNR (TLL, T, TCC) .LT. 0) THEN
C-----Version 3.1  IDID error changed
         IDID = -112
         RETURN
      ELSE IF (T .LT. TNRC1) THEN
         CALL EOS (PROP, D, T)
         CALL DJPT (IDID, PROP, D, PSAT, T)
         IF (IDID .LT. 0) THEN
            IDID = -205
            RETURN
         ENDIF
         IF (T .LE. TNRC2) RETURN
      ELSE
         CALL EOS (PROP, D, T)
         RETURN
      ENDIF
      CALL EOS (PROPC, D, T)
      F   = (T - TNRC2) / TDEL
      OMF = 1.D0-F
      DO 10 J = 1, 9
         PROP(J) = F*PROPC(J) + OMF*PROP(J)
10    CONTINUE
      RETURN
      END
C
C
C
      SUBROUTINE TSIP (T, DPDT, POBS)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Iterates vapor press eqn to find TEMP[K], for input of P[kPa].
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/PRECSN/ E(0:15)
* ! The calling program must insure that PTR <= PP <= PCC !
      P = POBS
      T = MIN (TNRC1, (TTP*((P/PTP)**(1.D0/THETA))))
C-----Version 3.11 Add tolerance for low vapor pressure; change Do limit.
C-----Version 3.20 Change E(10) to E(13)
      PMIN = E(13)*P + 1.D-06
      ODP = 0.D0
      DO 448 I = 1, 30
         CALL PSFT (PNEW, DPDT, T)
         DELP = PNEW - POBS
         IF (ABS (DELP) .LE. PMIN) RETURN
         DT1 = -DELP/DPDT
         IF ((I .EQ. 1) .OR. (ODP .EQ. DELP)) THEN
            DT2 = 0.D0
         ELSE
            FFF = DELP/(ODP-DELP)
            DT2 = FFF*(DT1 + FFF*(OT-T))
            IF (ABS(DT2/DT1) .GT. 0.5D0) DT2 = 0.D0
         ENDIF
         OT  = T
         ODP = DELP
         T   = T + DT1 + DT2
  448 CONTINUE
      END
C
C
C
C--------------------------------------------------  end saturation props
C--------------------------------------------------  Melting line P, T, D
C
      SUBROUTINE MELTFD (IDID, PROP, D)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP (1 to 9) from the EOS
* Input:  liquid density D [mol/L] at the melting line
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ZERO=0.D0)
      DIMENSION PROP(0:31)
      COMMON/MELT/   CMELT(6)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/PRECSN/ E(0:15)
C-----Version 3.01; following line added Nov. 14, 1991
      IDID = 1
C-----Version 3.20 Tolerances on DMM changed, DTPL added
      IF (D .GT. DMM+0.002) THEN
         IDID = -103
         RETURN
      ELSE IF (D .LT. DTPL-0.002) THEN
         IDID = -114
         RETURN
      ENDIF
* Estimate temperature
      T = TTP + (TMM-TTP)*(D-DTPL)/(DMM-DTPL)
      ODY  = ZERO
      DT1  = 1.D+12
      DO 432 J = 1, 8
*        Convergence ususally obtained on the 3rd call to EOS
         CALL EOS (PROP, D, T)
         CALL PMFT (PM, DPDTM, T)
         EPS  = E(9)*PM
         DELY = PM - PROP(1)
         IF ((ABS(DELY) .LT. EPS) .OR. (ABS(DT1) .LT. E(4))) RETURN
         DT1  = DELY / (PROP(8) - DPDTM)
         IF ((J .EQ. 1) .OR. (ODY .EQ. DELY)) THEN
            DT2 = ZERO
         ELSE
            DT2 = DT1*DELY/(ODY-DELY) + (OT-T)*(DELY/(ODY-DELY))**2
            IF (ABS(DT2/DT1) .GT. 0.5D0) DT2 = ZERO
         ENDIF
         OT  = T
         ODY = DELY
         T   = T + DT1 + DT2
  432 CONTINUE
* This should never occur if the input is within the valid range
* of the state equation.
      IDID = -206
      END
C
C
C
      SUBROUTINE MELTFP (IDID, PROP, P)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP (1 to 9) from the EOS
* Input:  Pressure [kPa] at the melting line
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/MELT/   CMELT(6)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
C-----Version 3.20 Limit tolerances
      IF (P .GT. PUL*1.0001D0) THEN
         IDID = -101
         RETURN
      ELSE IF (P .LT. PTP*0.999999D0) THEN
         IDID = -115
         RETURN
      ENDIF
      T = TMFP (P)
* Estimate D
      D = DTPL+ (DMM-DTPL)*(T-TTP)/(TMM-TTP)
      CALL EOS (PROP, D, T)
      CALL DJPT (IDID, PROP, D, P, T)
      IF (IDID .LT. 0) IDID = -207
      RETURN
      END
C
C
C
      SUBROUTINE MELTFT (IDID, PROP, T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP (1 to 9) from the EOS
* Input:  Temperature [K] at the melting line
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/MELT/   CMELT(6)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      IF (T .GT. TMM) THEN
         IDID = -116
         RETURN
      ELSE IF (T .LT. TLL) THEN
         IDID = -102
         RETURN
      ENDIF
      CALL PMFT (P, DPDT, T)
* Estimate D
      D = DTPL+ (DMM-DTPL)*(T-TTP)/(TMM-TTP)
      CALL EOS (PROP, D, T)
      CALL DJPT (IDID, PROP, D, P, T)
      IF (IDID .LT. 0) IDID = -208
      RETURN
      END
C
C
C
      SUBROUTINE PMFT (P, DPDT, T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* The melting pressure equation:
* Computes melting pressure [KPa] and dP/dT [kPa/K].
* Input = temperature [K].
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/MELT/  CMELT(6)
      COMMON/INDEXX/IDX(12)
C-----Version 3.20 Common /Points/ added 
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
C-----Version 3.1   IF loop reorganized; no numerical change.
      IF (IDX(3) .EQ. 4) THEN
         P    = (CMELT(1)+T*(CMELT(2)+T*CMELT(3)))*1.D+3
         DPDT = 1.D+3*(CMELT(2)+2.D0*CMELT(3)*T)
      ELSE IF ((IDX(3) .EQ. 2) .AND. (T .GE. 22.D0)) THEN
         TEX  = CMELT(5)*T**CMELT(6)
         P    = (CMELT(4) + TEX)*1.D+03
         DPDT = 1.D+03*CMELT(6)*TEX/T
      ELSE IF (IDX(3) .EQ. 5) THEN
C-----Version 3.20 IDX(3)=5 loop added (Simon equation).
         TEX  = T - TTP
         P    = PTP
         IF (TEX .GT. 0.D0) THEN
            DPDT= CMELT(1)*TEX**(CMELT(2) - 1.D0)
            P   = TEX*DPDT + P
            DPDT= CMELT(2)*DPDT/TTP
         ELSE
            DPDT= 0.D0
         ENDIF
      ELSE
         TEX  = CMELT(2)*T**CMELT(3)
         P    = (CMELT(1) + TEX)*1.D+03
         DPDT = 1.D+03*CMELT(3)*TEX/T
      ENDIF
      END
C
C
C
      DOUBLE PRECISION FUNCTION TMFP (PKPA)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: Temperature [K] at the melting line
* Input:  Pressure [kPa] at the melting line
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/MELT/  CMELT(6)
      COMMON/INDEXX/IDX(12)
C-----Version 3.20 Common /Points/ added 
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      P = PKPA * 1.D-03
      IF(IDX(3).EQ.4)THEN
C-----Version 3.01; If loop reorganized; no numerical change. Nov. 14, 1991
         F1=1.D0/(CMELT(3)*2.D0)
         F2=CMELT(2)**2-4.D0*CMELT(3)*(CMELT(1)-P)
         TMFP=F1*(DSQRT(F2)-CMELT(2))
C-----Version 3.11: 31.64 Mpa
      ELSE IF ((IDX(3) .EQ. 2) .AND. (P .GE. 31.64D0)) THEN
         TMFP=((P-CMELT(4))/CMELT(5))**(1.D0/CMELT(6))
      ELSE IF (IDX(3) .EQ. 5) THEN
C-----Version 3.20 IDX(3)=5 loop added (Simon equation).
         P = PKPA
         TMFP = TTP
         IF (P .GT. PTP) 
     +      TMFP = TMFP + ((P-PTP)/CMELT(1))**(1.D0/CMELT(2))
      ELSE
         TMFP=((P-CMELT(1))/CMELT(2))**(1.D0/CMELT(3))
      ENDIF
      RETURN
      END
C
C
C--------------------------------------------------  end Melting props
C--------------------------------------------------  Iterations
C
C
      SUBROUTINE DFPT (IDID, PROP, PP, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: single phase PROP(1 to 20 ) from the EOS and DERIV
* Input:  P [kPa], T [K]
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ONE3RD=1.D0/3.D0)
      DIMENSION PROP(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/CUBIC/  COFD2, QF3, RFACT
      T = TT
      P = PP
      IF ((T .LT. TLL) .OR. (T .GT. TUL+0.1)) THEN
         IDID = -102
         RETURN
      ENDIF
C-----Version 3.11  Add tolerance to PUL
      IF ((P .LT. PLL) .OR. (P .GT. PUL+1.0)) THEN
         IDID = -101
         RETURN
      ENDIF
C-----Version 3.13  Test for solid range
      IF ((T .LT. TMM) .AND. (P .GT. PCC)) THEN
         CALL PMFT (PM, PXX, T)
         IF (P .GT. PM) THEN
            IDID = -110
            RETURN
         ENDIF
      ENDIF
      IF ((T .GE. TCC) .OR. (P .LT. PTP)) THEN
         IF ((T .GE. 1.3D0*TCC) .OR. (P .LT. 0.5D0*PCC)) THEN
            RCUBIC = RFACT*P/T
            QR = SQRT (QF3 + RCUBIC**2)
            D  = (QR+RCUBIC)**ONE3RD - (QR-RCUBIC)**ONE3RD
            D  = MIN (D, 2.5D0*DCC)
         ELSE
            PXX  = PCC * ((T/TCC)**THETA)
            IF (P .LT. PXX) THEN
               D = DCC * P / PXX
            ELSE
               D = MIN (DCC*SQRT(P/PXX), 2.5D0*DCC)
            ENDIF
         ENDIF
      ELSE
         CALL PSFT (PS, DUM, T)
         IF (P .LT. PS) THEN
            D = DSATV (T) * 0.999D0
         ELSE
C-----Version 3.11  Logic reorganized; add tolerance to PM
            D = DSATL (T) * 1.001D0
            IF (T .LT. TMM) THEN
               CALL PMFT (PM, DPDTM, T)
               IF (P .GT. PM+1.) THEN
                  IDID = -110
                  RETURN
               ENDIF
               D = D + (DMM-D)*((P-PS)/PM)
            ELSE
               DUM = MAX ((DMM*SQRT(T/TMM)), D)
               D = D + (DUM-D)*(P-PS)/(PUL-PS)
            ENDIF
         ENDIF
      ENDIF
      CALL EOS (PROP, D, T)
      CALL DJPT (IDID, PROP, D, P, T)
      IF (IDID .GT. 0) CALL DERIV (PROP)
      RETURN
      END
C
C
C
      SUBROUTINE DFST (IDID, PROP, SS, TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(1 to 20) from the EOS and DERIV
*         valid for both single phase and two-phase mixtures
* Input:  entropy S [J/mol-K] and temperature T [K];
*
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31,0:2)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1                TPLPRP(0:31), TPVPRP(0:31)
*
      T = TT
      S = SS
      IF ((T .LT. TLL) .OR. (T .GT. TUL+0.1)) THEN
         IDID = -102
      ELSE IF (T .LT. TCC) THEN
         CALL PSFT (PSAT, DPDT, T)
         CALL SVFPT (IDID, PROP, PSAT, T)
         IF (S .GT. PROP(4,0)) THEN
* Single phase vapor
            D  = PROP(3,0) * EXP ((PROP(4,0)-S)/RCON)
            CALL DJST (IDID, PROP, D, S, T)
         ELSE
* Either liquid or l-v mixture
            DO 434 J = 1, 9
               PROP(J,2) = PROP(J,0)
  434       CONTINUE
            CALL SLFPT (IDID, PROP(0,1), PSAT, T)
            IF (IDID .LT. 0) THEN
               IDID = -210
            ELSE IF (S .GE. PROP(4,1)) THEN
* Liquid-vapor mixture
               IF (PROP(4,1) .EQ. PROP(4,2)) THEN
                  X = 0.5D0
               ELSE
                  X = (S-PROP(4,1)) / (PROP(4,2)-PROP(4,1))
               ENDIF
               CALL DERIV (PROP(0,1))
               CALL DERIV (PROP(0,2))
               CALL MIXPRP (PROP(0,0), PROP(0,1), PROP(0,2),
     +                      X, DPDT)
               IDID = 3
               RETURN
            ELSE
* Compressed liquid or solid
C-----Version 3.11  Accept if T > TMM (4 lines, new IF)
               IF (T .GE. TMM) THEN
                  D = PROP(3,1) * 1.01
                  CALL DJST (IDID, PROP, D, S, T)
                  IF (IDID .LT. 0) IDID = -121
               ELSE
                  CALL MELTFT (IDID, PROP, T)
                  IF (IDID .LT. 0) THEN
                     IDID = -210
                  ELSE IF (S .LT. PROP(4,0)) THEN
* Solid phase
                     IDID = -110
                  ELSE
* Compressed liquid.  If the input data is exactly at the triple point
* it is possible that PROP(j,0) and PROP (j,1) are identical.
                     IF (PROP(4,0) .EQ. PROP(4,1)) THEN
                        X = 0.5D0
                     ELSE
                        X = (S-PROP(4,0)) / (PROP(4,1) - PROP(4,0))
                     ENDIF
                     D = (1.D0-X)*PROP(3,0) + X*PROP(3,1)
                     CALL DJST (IDID, PROP, D, S, T)
                  ENDIF
               ENDIF
            ENDIF
         ENDIF
      ELSE
         D = HIPROP(3) * EXP ((HIPROP(4) - S +
     +         HIPROP(9)*LOG(T/HIPROP(2)))/RCON)
C-----Version 3.13: low density limit added
         IF (D .LT. DLL) THEN
            IDID = -121
            RETURN
         ENDIF
C-----Version 3.13: DMM added to MIN arguements
         D = MIN (D, (2.5D0*DCC), DMM)
         CALL DJST (IDID, PROP, D, S, T)
      ENDIF
      IF (IDID .GT. 0) THEN
C-----Version 3.11  Tolerance added to PUL
         IF ((PROP(1,0) .LT. PLL) .OR. (PROP(1,0) .GT. PUL+1.)) THEN
            IDID = -101
         ELSE
            CALL DERIV (PROP)
         ENDIF
      ENDIF
      RETURN
      END
C
C
C
      SUBROUTINE DJPT (IDID, PROP, DD, P, T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(1:9); IDID=1 if successful, -209 if failure occurs
* Input:  fixed values: Pressure [kPa] and Temperature [K], also
*         an initial estimate of density DD [mol/L].
*         The calling program must also call EOS (PROP, DD,T) just before
*         calling this routine!
*         Valid only for single phase fluid.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PARAMETER (FRACT=0.4D0)
      COMMON/PRECSN/ E(0:15)
      DIMENSION PROP(0:31)
      D=DD
C-----Version 3.11;  small tolerance added
      EPS  = E(10)*P + E(10)
      ODP  = 0.D0
      DO 436 J = 1, 21
         PNEW = PROP(1)
         DELP = PNEW - P
         IF (ABS(DELP) .LT. EPS) THEN
            IDID = 1
            RETURN
         ENDIF
* DD1 is the ordinary Newton-Raphson density correction
         DD1  = -DELP/PROP(7)
* Restrict large steps (near critical)
         DD1 = SIGN (MIN (ABS(DD1), ABS(FRACT*D)), DD1)
* DD2 is the inverse-quadratic Newton-Raphson density correction.
         IF ((J .EQ. 1) .OR. (ODP .EQ. DELP)) THEN
            DD2 = 0.D0
         ELSE IF ((J .GE. 8) .AND. ((J/4)*4 .EQ. J)) THEN
            DD1 = 0.5*DD1
            DD2 = 0.D0
         ELSE
            FFF = DELP/(ODP-DELP)
            DD2 = FFF*(DD1 + FFF*(OD-D))
            IF (ABS(DD2/DD1) .GT. 0.5D0) DD2 = 0.D0
         ENDIF
         OD  = D
         ODP = DELP
         D   = D + DD1 + DD2
         CALL EOS (PROP, D, T)
  436 CONTINUE
* This should never occur if the input is within the valid range
* of the state equation.
      IDID = -209
      RETURN
      END
C
C
C
      SUBROUTINE DJST (IDID, PROP, DD, S, T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(1:9); IDID=1 if successful, -211 if failure occurs
* Input:  fixed values: Entropy [J/mol-K] and Temperature [K], also
*         an initial estimate of density DD [mol/L].
*         Valid only for single phase fluid.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PARAMETER (FRACT=0.35D0)
      COMMON/PRECSN/ E(0:15)
C-----Version 3.13: /limits/ added
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      DIMENSION PROP(0:31)
      D=DD
      ODS  = 0.D0
      DO 438 J = 1, 10
         CALL EOS (PROP, D, T)
C-----Version 3.13: high pressure limit added
         IF ((PROP(1) .GT. 1.1*PUL) .AND. (J .GT. 1)) THEN
            IDID = -134
            RETURN
         ENDIF
         SNEW = PROP(4)
         DELS = SNEW - S
         DUM  = PROP(8)/D
C-----Version 3.20 Tolerance tightened E(9) to E(13)
         IF (ABS(DELS) .LT. ABS (E(13)*DUM)) THEN
            IDID = 1
            RETURN
         ENDIF
         DD1 = DELS*D/DUM
* Restrict large steps (near critical)
         DD1 = SIGN (MIN (ABS(DD1), ABS(FRACT*D)), DD1)
         IF ((J .EQ. 1) .OR. (ODS .EQ. DELS)) THEN
            DD2 = 0.D0
         ELSE IF ((J .GE. 8) .AND. ((J/4)*4 .EQ. J)) THEN
            DD1 = 0.5*DD1
            DD2 = 0.D0
         ELSE
            FFF = DELS/(ODS-DELS)
            DD2 = FFF*(DD1 + FFF*(OD-D))
            IF (ABS(DD2/DD1) .GT. 0.5D0) DD2 = 0.D0
         ENDIF
         OD  = D
         ODS = DELS
         D   = D + DD1 + DD2
  438 CONTINUE
* This should never occur if the input is within the valid range
* of the state equation.
      IDID = -211
      RETURN
      END
C
C
C
      SUBROUTINE DTFPY (IDID, PROP, P, Y, JY)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(i,j), i= 1 to 20, and IDID
* Input: Pressure [kPa] and parameter Y, where
*        Y = entropy  (S) if JY=4
*        Y = enthalpy (H) if JY=5
*        Y = energy   (U) if JY=6
*            (! No other values of JF are valid !)
* Range:  valid over the full state equation, including 2-phase region
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (FRACT=0.35D0, ONE3RD=1.D0/3.D0)
      LOGICAL PCNVG, YCNVG
      DIMENSION JDER(4:6), PROP(0:31,0:2)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1                TPLPRP(0:31), TPVPRP(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/CUBIC/  COFD2, QF3, RFACT
      COMMON/PRECSN/ E(0:15)
      DATA JDER /10, 10, 9/
      DATA DRATIO /1.9D0/
      IF ((P .LT. PLL) .OR. (P .GT. PUL*1.001D0)) THEN
         IDID = -101
         RETURN
      ENDIF
* Test "ideal gas" solution
* T is reset to a a positive value when a valid estimate is obtained.
      IF (JY .EQ. 4) THEN
         TA = (Y - HIPROP(4))/HIPROP(10)
C-----Version 3.13: if loop check protects against excessive over-range
         IF (ABS(TA) .GT. 10.D0) THEN
            IDID = -121
            RETURN
         ENDIF
C-----Version 3.13: expression rewritten to use TA
         TA = HIPROP(2) * EXP (TA + RCON*LOG(P/HIPROP(1))/HIPROP(10))
C        TA = HIPROP(2) * EXP
C    +        ((Y - HIPROP(4) + RCON*LOG(P/HIPROP(1)))/HIPROP(10))
      ELSE IF ((JY .EQ. 5) .OR. (JY .EQ. 6)) THEN
         TA = HIPROP(2) + (Y - HIPROP(JY))/HIPROP(JDER(JY))
         IF (JY .EQ. 5) THEN
* Enthalpy correction for high pressure high temperature fluid.
            RCUBIC = RFACT*P/HIPROP(2)
            QR = SQRT (QF3 + RCUBIC**2)
            D  = (QR+RCUBIC)**ONE3RD - (QR-RCUBIC)**ONE3RD
            IF (D .GT. DRATIO*DCC) THEN
               PXC = (1.D0+COFD2*(DRATIO**2))*DRATIO*DCC*RCON*HIPROP(2)
               TA  = TA + (PXC - P)/(DRATIO*DCC*HIPROP(10))
            ENDIF
         ENDIF
C-----Version 3.1 comment out or remove
C     ELSE
C        IDID = -4
C        RETURN
      ENDIF
      T    = -1.D0
* Check for approximate ideal gas
C-----Version 3.1: "if" logic changed in following lines. 
      IF (TA .GE. 1.3D0*TCC) THEN
         RCUBIC = RFACT*P/TA
         QR = SQRT (QF3 + RCUBIC**2)
         D  = (QR+RCUBIC)**ONE3RD - (QR-RCUBIC)**ONE3RD
         D  = MIN (D, 2.5D0*DCC)
         T = TA
      ELSE IF (P .LT. PTP) THEN
         IF (Y .LT. TPLPRP(JY)) THEN
            IDID = -117 - JY
            RETURN
         ENDIF
         T = MAX (TLL, TA)
         D = P/(RCON*T)
      ENDIF
C-----Version 3.1: end of changes. 
      IF (T .LT. 0.D0) THEN
* Not "ideal gas"; do further tests
         IF (P .LT. PCC) THEN
* Compare with saturated vapor
            CALL TSIP (TA, DPDT, P)
            DV = DSATV (TA)
            CALL EOS (PROP(0,2), DV, TA)
            CALL DERIV (PROP(0,2))
* In the following line, the derivative term is incorrect when
* JY=6, but the error is zero in the ideal gas, and only weakly divergent
* at the critical point.  It is satisfactory for an initial estimate.
            DELT = (Y - PROP(JY,2)) / PROP(JDER(JY),2)
            IF (JY .EQ. 4) DELT = DELT * TA
            IF ((DELT .LT. 0.2D0) .AND. (TA .LT. TNRC1)) THEN
               CALL DJPT (IDID, PROP(0,2), DV, P, TA)
               IF (IDID .LT. 0) THEN
                  IDID = -212
                  RETURN
               ENDIF
               CALL DERIV (PROP(0,2))
               DELT = (Y - PROP(JY,2)) / PROP(JDER(JY),2)
               IF (JY .EQ. 4) DELT = DELT * TA
            ENDIF
            IF (DELT .GT. 0.D0) THEN
* Single phase vapor
               T = TA + DELT
               D = DV * TA / T
            ELSE
* Possible two-phase or compressed liquid
* Compare with saturated liquid
               DL = DSATL (TA)
               CALL EOS (PROP(0,1), DL, TA)
               CALL DERIV (PROP(0,1))
               DELT = (Y - PROP(JY,1)) / PROP(JDER(JY),1)
               IF (JY .EQ. 4) DELT = DELT * TA
               IF ((DELT .GT. -0.2D0) .AND. (TA .LT. TNRC1)) THEN
                  CALL DJPT (IDID, PROP(0,1), DL, P, TA)
                  IF (IDID .LT. 0) THEN
                     IDID = -212
                     RETURN
                  ENDIF
                  CALL DERIV (PROP(0,1))
                  DELT = (Y - PROP(JY,1)) / PROP(JDER(JY),1)
                  IF (JY .EQ. 4) DELT = DELT * TA
               ENDIF
               IF (DELT .LT. 0.D0) THEN
* Compressed liquid; check melting line
                  T = TA + DELT
                  CALL MELTFP (JDID, PROP, P)
                  IF (JDID .LT. 0) THEN
                     IDID = -212
                     RETURN
                  ELSE IF (Y .LT. PROP(JY,0)) THEN
                     IDID = -110
                     RETURN
                  ELSE
                     T = MAX (T, PROP(2,0))
                     D = PROP(3,0) + (PROP(2,0)-T)*PROP(8,0)/PROP(7,0)
                  ENDIF
               ELSE
* Liquid-vapor mixture.  It is possible that quality bounds would be
* exceeded slightly if T > TNRC1 (very near the critical point).
* Q is the quality (= vapor mass fraction).
                  IF (PROP(JY,1) .EQ. PROP(JY,2)) THEN
                     Q = 0.5D0
                  ELSE
                     Q = (Y-PROP(JY,1))/(PROP(JY,2)-PROP(JY,1))
                     IF (Q .GT. 1.D0) THEN
                        Q = 1.D0
                     ELSE IF (Q .LT. 0.D0) THEN
                        Q = 0.D0
                     ENDIF
                  ENDIF
                  CALL MIXPRP (PROP(0,0), PROP(0,1), PROP(0,2),
     +                         Q, DPDT)
                  IF (TA .GE. TNRC1) PROP(1,0) = P
                  IDID = 3
                  RETURN
               ENDIF
            ENDIF
         ELSE
* P is >= the critical pressure; TXC is the pseudo-critical temperature
            TXC = TCC*MAX (((MIN(P, 0.7D0*PUL)/PCC)**(1./THETA)),
     +            1.05D0)
            CALL EOS (PROP, DCC, TXC)
            CALL DERIV (PROP)
            DELT = (Y - PROP(JY,0)) / PROP(JDER(JY),0)
            IF (JY .EQ. 4) DELT = DELT * TXC
            T = TXC + DELT
            IF (DELT .GT. 0.D0) THEN
               D = DCC * SQRT (TXC / T)
            ELSE
* Compressed liquid; check melting line
* (After this call, PROP refers to melting rather than pseudo-critical line)
               CALL MELTFP (JDID, PROP, P)
               IF (JDID .LT. 0) THEN
                  IDID = -212
                  RETURN
               ELSE IF (Y .LT. PROP(JY,0)) THEN
                  IDID = -110
                  RETURN
               ELSE
                  T = MAX (T, PROP(2,0))
                  D = PROP(3,0) + (PROP(2,0)-T)*PROP(8,0)/PROP(7,0)
               ENDIF
            ENDIF
         ENDIF
      ENDIF
* Test for estimated temperature exceeding the upper temperature
* limit by an excessive amount.
      IF (T .GT. 1.5D0*TUL) THEN
         IDID = -132
         RETURN
      ENDIF
* Finally, 2-D iteration
      IDID = 1
      DO 60 I = 1, 30
C-----Version 3.13: if loop check for wandering out of range.
      IF (((D .LT. DLL) .OR. (T .LT. TLL-.1)) .AND. (I .GT. 1)) THEN
         IDID = -117 - JY
         RETURN
      ENDIF
      CALL EOS (PROP, D, T)
      CALL DERIV (PROP)
      DELP = P - PROP(1,0)
      DELY = Y - PROP(JY,0)
      IF (JY .EQ. 4) THEN
*------ Pressure, Entropy
         DELD = (DELP - PROP(13,0)*D*T*DELY) / PROP(20,0)
         DELT = (PROP(12,0)*DELP/D + T*DELY) / PROP(10,0)
      ELSE IF (JY .EQ. 5) THEN
*------ Pressure, Enthalpy
         DELD = ((1.D0+PROP(13,0))*DELP - D*PROP(13,0)*DELY)/PROP(20,0)
         DELT = PROP(16,0)*DELP + DELY/PROP(10,0)
      ELSE IF (JY .EQ. 6) THEN
*------ Pressure, Energy
         CKT   = PROP(1,0) * PROP(14,0)
         OMPKS = 1. - PROP(13,0)*CKT/PROP(11,0)
         DELD  = (DELP - PROP(13,0)*D*DELY)/(PROP(20,0)*OMPKS)
         DELT  = ((PROP(12,0)-CKT)*DELP/D + DELY)/(PROP(10,0)*OMPKS)
      ENDIF
      YCNVG = .FALSE.
      DUM   = ABS (DELT/T)
* Restrict large steps
      IF ((ABS(DELD/D) + DUM) .GT. FRACT) THEN
         DELD = SIGN (MIN (ABS(DELD), ABS(FRACT*D)), DELD)
         DELT = SIGN (MIN (ABS(DELT), ABS(FRACT*T)), DELT)
      ELSE IF (DUM .LT. E(10)) THEN
         YCNVG = .TRUE.
      ENDIF
      IF (ABS(DELP) .LT. E(9)*P) THEN
         PCNVG = .TRUE.
      ELSE
         PCNVG = .FALSE.
      ENDIF
      IF (PCNVG .AND. YCNVG) THEN
         IF (PROP(2,0) .GT. TUL+1.) THEN
            IDID = -132
         ELSE IF (PROP(2,0) .LT. TLL) THEN
            IDID = -131
         ENDIF
         RETURN
      ENDIF
* Damp out oscillations which sometimes occur
      IF ((I .GE. 8) .AND. ((I/4)*4 .EQ. I)) THEN
         DELD = DELD*0.5D0
         DELT = DELT*0.5D0
      ENDIF
      D  = D + DELD
      T  = T + DELT
   60 CONTINUE
* This should never occur if the input is within the valid range
* of the state equation.
      IDID = -212
      END
C
C
C
      SUBROUTINE DTFHS (IDID, PROP, HH, SS)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(1:20, j) and IDID
* Input: Enthalpy H [J/mol], and Entropy S [J/mol-K]
* Valid over the full state equation, including the two phase region.
C-----Version 3.11:  Major revision 5/23/92; not marked line by line
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (FRACT=0.35D0, ZERO=0.D0)
      LOGICAL TCNVG, DCNVG, IDV
      DIMENSION PROP(0:31,0:2), PRDTMM(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1                TPLPRP(0:31), TPVPRP(0:31)
      COMMON/SUBRT1/ R(0:31), JR, JK
      COMMON/SUBRT2/ R2(0:31), JR2, JK2
      COMMON/PRECSN/ E(0:15)
      EXTERNAL DTHS
      SAVE SMIN
      S = SS
      H = HH
* Calculate Hmax of sat vapor and Smin, if not previously done
      IF (THVM .NE. HVMPRP(2)) THEN
         DHVM = DSATV (THVM)
         CALL EOS (HVMPRP, DHVM, THVM)
         CALL EOS (PRDTMM, (0.9999D0*DMM), TMM)
         SMIN = MIN ((PRDTMM(4)-0.1), TPLPRP(4)) - 0.0001 
      ENDIF
      IF (S .LT. SMIN) THEN
         IDID = -121
         RETURN
      ENDIF
      IDV = .TRUE.
      IF (H .LT. (HVMPRP(5)+50.)) THEN
         IDV = .FALSE.
         CALL SATFY (IDID, PROP, S, 4)
         IF (IDID .GE. 1) THEN
            DELH = H - PROP(5,0)
            IF (DELH .GT. 0.D0) THEN
               CALL DERIV (PROP)
               T = PROP(2,0)*(1.D0 + PROP(13,0)*DELH/PROP(20,0))
               D = PROP(3,0)
               IF (IDID .EQ. 2) D = D * PROP(2,0) / T
            ELSE
C              Two phase region
               R2(4) = S
               R2(5) = H
               TA = MIN (TCC, (PROP(2,0) + 0.1))
C-----Version 3.11:  TTP changed to TLL in following 2 lines
               T1 = 0.75D0*TA + 0.25D0*TLL
               T2 = 0.25D0*TA + 0.75D0*TLL
               CALL ROOTAZ (T, TA, T1, T2, TTP, E(5), E(11), E(5),
     +               E(11), DTHS, ZERO, JX)
               IF (JX .LT. 0) THEN
* Temperature is below triple pt. in 2-phase region
                  IDID = - 131
                  RETURN
               ENDIF
* Two phase; properties in R have been evaluated in function DTHS
               CALL DERIV (R)
               DO 440 J = 1, 20
                  PROP (J,1) = R(J)
  440          CONTINUE
               P = R(1)
               CALL SVFPT (IDID, PROP(0,2), P, T)
               CALL DERIV (PROP(0,2))
               IF (PROP(5,1) .EQ. PROP(5,2)) THEN
                  X = 0.5D0
               ELSE
                  X  = (H - PROP(5,1)) / (PROP(5,2) - PROP(5,1))
               ENDIF
               CALL MIXPRP (PROP, PROP(0,1), PROP(0,2), X, R2(0))
               IDID = 3
               RETURN
            ENDIF
         ELSE IF (S .LT. TPLPRP(4)) THEN
C compressed liquid, near melting line
            D = 0.5*(DTPL+DMM)
            T = 0.5D0*(TTP + MIN (TMM, TCC))
         ELSE
            IDV = .TRUE.
         ENDIF
      ENDIF
      IF (IDV) THEN
* use ideal gas approximation
         T = MAX (HIPROP(2)+(H-HIPROP(5))/HIPROP(10), TCC+5.D0)
         IF (T .GT. 2.D0*TUL) THEN
C-----Version 3.1; value changed from -122
            IDID = -132
            RETURN
         ENDIF
         T = MIN (T, TUL)
         D = HIPROP(3)*EXP(
     +         (HIPROP(4)-S-HIPROP(9)*LOG(HIPROP(2)/T))/RCON)
         D = MIN (D, 2.5D0*DCC)
      ENDIF
      IDID = 1
      DO 60 I = 1, 23
      CALL EOS (PROP, D, T)
      CALL DERIV (PROP)
      DELH = H - PROP(5,0)
      DELS = S - PROP(4,0)
      DELD = (DELH - (1.D0+PROP(13,0))*T*DELS) * D / PROP(20,0)
C-----Version 3.01; add factor of D multiplying PROP(16,0); Dec. 10, 1991
      DELT = (PROP(13,0)*DELH/PROP(20,0) - D*PROP(16,0)*DELS) * T
      TCNVG = .FALSE.
      DCNVG = .FALSE.
      DUM   = ABS (DELT/T)
* Restrict large steps
      IF ((ABS(DELD/D) + DUM) .GT. FRACT) THEN
         DELD = SIGN (MIN (ABS(DELD), ABS(FRACT*D)), DELD)
         DELT = SIGN (MIN (ABS(DELT), ABS(FRACT*T)), DELT)
C-----Version 3.20 Tighten tolerances: E(10) to E(14); 2 places
      ELSE IF (DUM .LT. E(14)) THEN
         TCNVG = .TRUE.
      ENDIF
      IF (ABS(DELD) .LT. E(14)*D) DCNVG = .TRUE.
      IF (DCNVG .AND. TCNVG) THEN
         IDID = 1
* Check for convergence out of range
C-----Version 3.11: small tolerance added, 4 places in following IF
         IF (PROP(2,0) .GT. TUL+1.) THEN
            IDID = -132
         ELSE IF (PROP(2,0) .LT. TLL-0.01) THEN
            IDID = -131
         ELSE IF (PROP(1,0) .GT. PUL+1.) THEN
            IDID = -134
         ELSE IF (PROP(1,0) .LT. PLL) THEN
            IDID = -133
         ELSE IF ((IBTWNR (TTP, PROP(2,0), TMM) .GE. 0) .AND.
     1            (PROP(3,0) .GE. DTPL)) THEN
            CALL MELTFT (IDID, R, T)
            IF (D .GT. 1.0001D0*R(3)) IDID = -110
         ENDIF
         RETURN
      ENDIF
* Damp out oscillations which sometimes occur
      IF ((I .GE. 8) .AND. ((I/4)*4 .EQ. I)) THEN
         DELD = DELD*0.5D0
         DELT = DELT*0.5D0
      ENDIF
      D  = D + DELD
      T  = T + DELT
   60 CONTINUE
C-----Version 3.1
      IDID = -216
      END
C
C
C
      DOUBLE PRECISION FUNCTION DTHS (T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* External function in DTFHS; it must not be called out-of-range!
* DTHS is the difference in Gibbs energy between the H and S which
* were input to subroutine DTFHS and the Gibbs energy calculated along
* the saturated liquid boundary, all as a function of T (coming from ROOTM).
* In principle, either satL or satV could be used, but residual
* inaccuracy in the state equation makes satL the better choice.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      COMMON/SUBRT1/ R1(0:31), JR1, JK1
      COMMON/SUBRT2/ R2(0:31), JR2, JK2
      T1   = T
      CALL PSFT (PSAT, DPDT, T1)
      CALL SLFPT (IDID, R1, PSAT, T1)
      DTHS = R2(5) - R1(5) - T1*(R2(4) - R1(4))
      R2(0)= DPDT
      RETURN
      END
C
C
C
      SUBROUTINE SATFY (IDID, PROP, Y, JY)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: Saturated fluid properties, PROP(0 to 9)
*         PROP(0) = dP/dT sat
*         The calling routine should transfer this to the appropriate
*         2-D array.
*         IDID=1 in saturated liquid, =2 in vapor
* Input: Y and JY, where:
*        Y = entropy, if JY=4
*        Y = enthalpy if JY=5
*        Y = energy   if JY=6
* (! No other values of JY are valid, but validity not tested !)
C-----Version 3.14  Extensive modification to allow input of sat vapor
C     H or U up to the max (between Tcrit and Thvm)
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SATNC/  TNRC1, TNRC2, TDEL, DSLC, DSVC, THETA, THVM
      COMMON/SUBRT1/ R(0:31), JR, JK
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1                TPLPRP(0:31), TPVPRP(0:31)
      COMMON/PRECSN/ E(0:15)
      EXTERNAL SLFYJ, SVFYJ
      IDID = 1
      JR   = JY
      JK   = 0
      IF (JY .EQ. 4) THEN
         YMAX = TPVPRP(4)
         TMIN = TTP
      ELSE
* Calculate sat vapor at THVM, if not previously done
         IF (THVM .NE. HVMPRP(2)) THEN
            DHVM = DSATV (THVM)
            CALL EOS (HVMPRP, DHVM, THVM)
         ENDIF
         YMAX = HVMPRP(JY)
         TMIN = THVM
      ENDIF
      IF ((Y-TPLPRP(JY))*(Y-YMAX) .GT. 0.D0) THEN
         IDID = -113 - JY
         RETURN
      ENDIF
      IF (Y .LE. CPPRP(JY)) THEN
         DY = CPPRP(JY) - TPLPRP(JY)
         F  = (Y - TPLPRP(JY)) / DY
         TA = (1.D0-F)*TPLPRP(2) + F*CPPRP(2)
         TB = MIN (1.1D0*TA, CPPRP(2))
         TA = MAX (0.99D0*TA, TPLPRP(2))
         CALL ROOTAZ (T, TPLPRP(2), TA, TB, CPPRP(2), E(5), E(9),
     +         (E(11)*DY), E(9), SLFYJ, Y, IX)
         IDID = 1
      ELSE
         DY = CPPRP(JY) - YMAX
         F  = (Y - YMAX) / DY
         TA = (1.D0-F)*TMIN + F*CPPRP(2)
         TB = MIN (1.1D0*TA, CPPRP(2))
         TA = MAX (0.99D0*TA, TMIN)
         CALL ROOTAZ (T, TMIN, TA, TB, CPPRP(2), E(5), E(9),
     +         (-E(11)*DY), E(9), SVFYJ, Y, IX)
         IDID = 2
      ENDIF
      IF (IX .LE. 0) THEN
* This should never occur if the input is within the valid range
* of the state equation.
         IDID = -213
      ELSE
         DO 442 J = 0, 9
            PROP(J) = R(J)
  442    CONTINUE
* Output: IDID = 1 in liquid, = 2 in vapor
      ENDIF
      END
C
C
C
C-----Version 3.01;  "Double precision" added to function; Dec. 9, 1991
      DOUBLE PRECISION FUNCTION SLFYJ (TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* This is an EXTERNAL function in SATFY, called by ROOTAZ.
* It is not of general user interest.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      COMMON/SUBRT1/ R(0:31), JR, JK
      T = TT
      JK= JK + 1
      IF (JK .LE. 2) THEN
         D = DSATL (T)
         CALL EOS (R, D, T)
      ELSE
         CALL PSFT (PS, DPDT, T)
         CALL SLFPT (IDID, R, PS, T)
         R(0) = DPDT
      ENDIF
      SLFYJ = R(JR)
      RETURN
      END
C
C
C
C-----Version 3.01;  "Double precision" added to function; Dec. 9, 1991
      DOUBLE PRECISION FUNCTION SVFYJ (TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* This is an EXTERNAL function in SATFY, called by ROOTAZ.
* It is not of general user interest.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      COMMON/SUBRT1/ R(0:31), JR, JK
      T = TT
      JK= JK + 1
      IF (JK .LE. 2) THEN
         D = DSATV (T)
         CALL EOS (R, D, T)
      ELSE
         CALL PSFT (PS, DPDT, T)
         CALL SVFPT (IDID, R, PS, T)
         R(0) = DPDT
      ENDIF
      SVFYJ = R(JR)
      RETURN
      END
C
C
C
      SUBROUTINE TFDP (IDID, PROP, D, P)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROP(i,j), i=1 to 20, and IDID
* Input:  density [mol/L] and pressure [kPa] (can include two-phase)
C-----Version, 3.01 Nov. 27, 1991: Calls to DERIV added, 5 locations.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION PROP(0:31, 0:2)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/SUBRT1/ RT(0:31), JR, JK
C-----Version 3.13, Pressure test added
C-----Version 3.20, limit tolerances added
      IF ((P .LT. PLL*0.99999D0) .OR. (P .GT. PUL+1.)) THEN
         IDID = -101
         RETURN
      ENDIF
      IF ((D .LT. DLL) .OR. (D .GT. DMM+0.01)) THEN
         IDID = -103
         RETURN
      ENDIF
C-----Version 3.13, Tests for far-out-of-range modified, moved here
      T = P /(RCON*D)
      IF (T .GT. TUL*(1.2D0+0.25D0*((D/DCC)**3))) THEN
         IDID = -132
         RETURN
      ENDIF
      IF ((T .LT. 0.96*TLL) .AND. (P .LT. PTP)) THEN
         IDID = -131
         RETURN
      ENDIF
C-----Version 3.01, Nov. 27, 1992:  2.0 changed to 0.8 in following line.
      IF (D .LT. 0.8D0*DCC) THEN
         IF ((T .GT. 1.2*TCC) .OR. (D .LE. DTPV)) THEN
            CALL TJDY (IDID, PROP, D, T, P, 1)
            IF (IDID .GT. 0) CALL DERIV (PROP)
            RETURN
         ENDIF
      ENDIF
* Not in "ideal gas" range.  Test for 2-phase region
      IF (D .LT. DTPL) THEN
         CALL SATFD (IDID, PROP, D)
         IF (IDID .LT. 0) THEN
            IDID = -214
            RETURN
         ENDIF
         IF (P .LT. PROP(1,0)) THEN
*           Two-phase liquid
            CALL TSIP  (T, DPDTS, P)
            CALL SLFPT (IDID, PROP(0,1), P, T)
            CALL SVFPT (IDID, PROP(0,2), P, T)
            CALL DERIV (PROP(0,1))
            CALL DERIV (PROP(0,2))
            Q = QFD (PROP(3,1), D, PROP(3,2))
            CALL MIXPRP (PROP(0,0), PROP(0,1), PROP(0,2), Q, DPDTS)
            IDID = 3
            RETURN
         ELSE
*           Single phase; estimate distance from saturation line.
            T = PROP(2,0) + (P-PROP(1,0))/PROP(8,0)
            IF (T .GT. (TUL*1.5D0)) THEN
               IDID = -132
               RETURN
            ENDIF
            CALL TJDY (IDID, PROP, D, T, P, 1)
            IF (IDID .GT. 0) CALL DERIV (PROP)
            RETURN
         ENDIF
      ELSE
*        Compressed liquid; start at melting line
         CALL MELTFD (IDID, PROP, D)
         IF (PROP(1,0) .GT. P) THEN
            IDID = -110
            RETURN
         ELSE
            T = PROP(2,0) + (P-PROP(1,0))/PROP(8,0)
            IF (T .GT. (TUL*1.5D0)) THEN
               IDID = -132
               RETURN
            ENDIF
            CALL TJDY (IDID, PROP, D, T, P, 1)
            IF (IDID .GT. 0) CALL DERIV (PROP)
            RETURN
         ENDIF
      ENDIF
      END
C
C
C
      SUBROUTINE TFDY (IDID, PROP, DD, Y, JY)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: PROPS (1:20,3), and IDID
* Input:  Density D [mol/L] and parameter Y, where
*                                         Y = entropy  (S) if JY=4
*                                         Y = enthalpy (H) if JY=5
*                                         Y = energy   (U) if JY=6
* Range:  valid over the full state equation, including 2-phase region
C !! Error will occur if JY is not = 4, 5, or 6 !!
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ZERO=0.D0)
C-----Version 3.30: 3 terms added in next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      COMMON/REFDAT/ HIPROP(0:31), CPPRP(0:31), HVMPRP(0:31),
     1                TPLPRP(0:31), TPVPRP(0:31)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/LIMITS/ XLL, PLL, TLL, DLL, XUL, PUL, TUL, DMM, TMM
      COMMON/SUBRT1/ R1(0:31), JR1, JK1
      COMMON/SUBRT2/ R2(0:31), JR2, JK2
      COMMON/PRECSN/ E(0:15)
      DIMENSION PROP (0:31, 0:2)
      EXTERNAL Y2FDT
*
      D = DD
* Check for ideal gas (if density is not too high)
      T = ZERO
      IF ((D .LT. DLL) .OR. (D .GT. DMM)) THEN
         IDID = -103
         RETURN
      ELSE IF (D .LT. 1.3D0*DCC) THEN
         T = HIPROP(2)
         IF (JY .EQ. 4) THEN
            T = T*EXP ((Y-HIPROP(4)+RCON*LOG(D/HIPROP(3)))/HIPROP(9))
         ELSE IF (JY .EQ. 5) THEN
C-----Version 3.30:  one line changed, no thermodynamics change
C           T = T + (Y-HIPROP(5))/HIPROP(18)
            T = T + (Y-HIPROP(5))/(HIPROP(9)+HIPROP(8)/HIPROP(3))
         ELSE IF (JY .EQ. 6) THEN
            T = T + (Y - HIPROP(6))/HIPROP(9)
C-----Version 3.1
C        ELSE
C           IDID = -5
C           RETURN
         ENDIF
* Check validity; T=0 signifies invalid estimate
         IF (T .GT. (1.5D0*TUL)) THEN
* Temperature is much too high; discontinue
            IDID = -132
            RETURN
         ELSE IF (D .LE. DTPV) THEN
*           Dilute vapor, closed to ideal gas, possibly low temp
            T = MAX (T, TLL)
C-----Version 3.01  Two lines removed; 1.3 changed to 1.4; Dec. 8, 1991
C-----Version 3.11  added delT(=15) to following line
         ELSE IF (T .LT. 1.4D0*TCC+15.D0) THEN
            T = ZERO
         ENDIF
      ENDIF
      IF (T .EQ. ZERO) THEN
         IF (D .LT. DTPL) THEN
*           Possible two-phase.  Find saturation properties at this density.
            CALL SATFD (IDID, PROP, D)
            IF (IDID .LT. 0) THEN
               IDID = -215
               RETURN
            ELSE IF (Y .LT. PROP(JY,0)) THEN
* Two-phase mixture: T will be less than PROP(2)
               JR1 = JY
               JK1 = 0
               TB  = PROP(2,0)
* Unusual location for the density.  This is not overwritten during
* the call to ROOTAZ.
               R1(31) = D
C-----Version 3.1 
C-----Version 3.11:  TTP changed to TLL in following 2 lines
               TA  = 0.5D0*(TB+TLL)
               CALL ROOTAZ (T, TLL, TA, TB, TCC, E(5), E(10), E(4),
     +               E(10), Y2FDT, Y, JN)
               IF (JN .GT. 0) THEN
                  X    = R1(0)
                  DPDT = R2(0)
                  DO 444 JP= 1, 9
                     PROP(JP,1) = R1(JP)
                     PROP(JP,2) = R2(JP)
  444             CONTINUE
                  CALL DERIV (PROP(0,1))
                  CALL DERIV (PROP(0,2))
                  CALL MIXPRP (PROP, PROP(0,1), PROP(0,2), X, DPDT)
                  IDID = 3
               ELSE
                  IDID = -117-JY
               ENDIF
               RETURN
            ENDIF
*           PROP (J,0) now contains sat(D) properties
*           Calculate (single-phase) distance from the saturation line.
            T = PROP (2,0)
            IF (JY .EQ. 4) THEN
               T = T*EXP((Y-PROP(4,0)+RCON*LOG(D/PROP(3,0)))/PROP(9,0))
            ELSE IF (JY .EQ. 5) THEN
               CALL DERIV (PROP)
C-----Version 3.30:  one line canceled, one changed, same thermodynamics
C              T = T + (Y - PROP(5,0))/PROP(18,0)
               T = T + (Y - PROP(5,0))/(PROP(9,0)+PROP(8,0)/PROP(3,0))
            ELSE IF (JY .EQ. 6) THEN
               T = T + (Y - PROP(6,0))/PROP(9,0)
            ENDIF
         ELSE
* Compressed liquid; reference to the melting line
            CALL MELTFD (IDID, PROP, D)
            T = PROP (2,0)
            IF (IDID .LT. 0) THEN
               IDID = -215
               RETURN
            ELSE IF (Y .LT. PROP(JY,0)) THEN
* Solid phase encountered
               IDID = -110
               RETURN
            ENDIF
*           Estimate distance from the melting line
            IF (JY .EQ. 4) THEN
               T = T*EXP((Y-PROP(4,0)+RCON*LOG(D/PROP(3,0)))/PROP(9,0))
            ELSE IF (JY .EQ. 5) THEN
               CALL DERIV (PROP)
               T = T + (Y-PROP(5,0))/PROP(10,0)
            ELSE IF (JY .EQ. 6) THEN
               T = T + (Y-PROP(6,0))/PROP(9,0)
C-----Version 3.1 comment out or remove
C            ELSE
C               IDID = -5
C               RETURN
            ENDIF
         ENDIF
      ENDIF
C-----Version 3.11 Next line
      IF (T .GT. 3.D0*TUL) THEN
         IDID = -132
      ELSE
C-----Version 3.11 Next line
         T = MIN (T, TUL)
         CALL TJDY (IDID, PROP, DD, T, Y, JY)
         IF (IDID .LT. 0) THEN
            IDID = -215
* Check for convergence out of range
C-----Version 3.11: small tolerance added, 3 places; following lines
         ELSE IF (PROP(2,0) .GT. TUL+1.) THEN
            IDID = -132
         ELSE IF (PROP(2,0) .LT. TLL-0.01) THEN
            IDID = -131
         ELSE IF (PROP(1,0) .GT. PUL+1.) THEN
            IDID = -134
         ELSE IF (PROP(1,0) .LT. PLL) THEN
            IDID = -133
         ENDIF
      ENDIF
      RETURN
      END
C
C
C
      SUBROUTINE TJDY (IDID, PROP, D, TT, Y, JY)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Iterate to find T such that PROP(J) = Y from EOS (DD, T)
* Output: PROP(1:20), single phase
* Input:  Single=phase temperature (estimated), TT, and
*         Density D [mol/L] and parameter Y, where
*                                         Y = pressure (P) if JY=1
*                                         Y = entropy  (S) if JY=4
*                                         Y = enthalpy (H) if JY=5
*                                         Y = energy   (U) if JY=6
* (!! No other values of JY are valid, but tests of JY are not made !!)
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ZERO=0.D0, FRACT=0.35D0)
      DIMENSION JDER (6), PROP(0:31)
      COMMON/PRECSN/ E(0:15)
      DATA JDER /8, 0, 0, 19, 18, 9/
      ODY  = ZERO
      DT1  = 1.D+12
      T    = TT
      JDY  = JDER (JY)
      DO 446 J = 1, 15
         CALL EOS (PROP, D, T)
         CALL DERIV (PROP)
         YNEW = PROP(JY)
         DELY = YNEW - Y
         DYDT = PROP(JDY)
         EPS  = DYDT*E(7)
         IF ((ABS(DELY) .LT. EPS) .OR. (ABS(DT1) .LT. E(5))) THEN
            IDID = 1
            RETURN
         ENDIF
         DT1  = -DELY/DYDT
* Restrict large steps (near critical)
         DT1 = SIGN (MIN (ABS(DT1), ABS(FRACT*T)), DT1)
         IF ((J .EQ. 1) .OR. (ODY .EQ. DELY)) THEN
            DT2 = ZERO
         ELSE IF ((J .GE. 8) .AND. ((J/4)*4 .EQ. J)) THEN
            DT1 = 0.5D0*DT1
            DT2 = ZERO
         ELSE
            FFF = DELY/(ODY-DELY)
            DT2 = FFF*(DT1 + FFF*(OT-T))
            IF (ABS(DT2/DT1) .GT. 0.5D0) DT2 = ZERO
         ENDIF
         OT  = T
         ODY = DELY
         T   = T + DT1 + DT2
  446 CONTINUE
* This should never occur if the input is within the valid range
* of the state equation.
      IDID = -214
      END
C
C
C
      DOUBLE PRECISION FUNCTION Y2FDT (TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: is returned in common blocks:
*         Saturated liquid and vapor properties are in R1 and R2.
*         dP/dT(sat) is in R2(0).  Mixture quality is in R1(0)
* Input:  Temperature T,  and from common /SUBRT1/,  average mixture
*         density R1(31) [mol/L], and parameter identifier JR;
*             JR=4 specifies entropy
*             JR=5 specifies enthalpy
*             JR=6 specifies internal energy
* !Tests for out-of-range parameters are not made within this routine!
*  This is an EXTERNAL function in subroutine TFDY.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ZERO=0.D0)
      COMMON/SUBRT1/R1(0:31), JR, JK
      COMMON/SUBRT2/R2(0:31), JR2, JK2
* This subroutine is called by ROOTAZ during iteration on T.
* For the first several calls it is not necessary to obtain highly
* accurate saturation densities.  The calls to DSATL and DSATV are
* less accurate but much faster than the calls to SLFPT and SVFPT.
* JK is used as a counter for the number of calls.
* JK is set to 0 in TFDY, where ROOTAZ is called.
*
      JK= JK + 1
      T = TT
      CALL PSFT (P, DPDT, T)
      IF (JK .LT. 4) THEN
         DL = DSATL (T)
         DV = DSATV (T)
         CALL EOS (R1, DL, T)
         CALL EOS (R2, DV, T)
      ELSE
         CALL SLFPT (IDID, R1, P, T)
         CALL SVFPT (IDID, R2, P, T)
      ENDIF
      IF (R1(3) .EQ. R2(3)) THEN
         X = 0.5D0
      ELSE
*        R1(31) contains the mixture density
         X = (R1(3)/R1(31) - 1.D0) / (R1(3)/R2(3) - 1.D0)
      ENDIF
      Y2FDT = X*R2(JR) + (1.D0 - X)*R1(JR)
      R1(0) = X
      R2(0) = DPDT
      END
C
C
C ================================ End of state properties
C ================================ Viscosity functions
C
C
      SUBROUTINE VISC (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Calculates viscosity = PROP(21) [Pa-s] = [SI units]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP (0:31)
C-----Version 3.30: common DISCOF added
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
      COMMON/INDEXX/IDX(12)
      COMMON/TRAN2/ GMW, EOK, RM, TC, DC, X , PC, SIG
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
* DML = Density [mol/L];  DGC = Density [gm/cm3]
* IDX(2) is the old (NW+1)
      T   = PROP(2)
      DML = PROP(3)
      IF (IDX(8) .EQ. 0) THEN
         PROP(21) = 0.
      ELSE IF (IDX(2).EQ.2) THEN
C-----Version 3.30 factor 10^6 relocated, several places
         PROP(21) = 1.D-06*VISCW1 (DML,T)
      ELSE 
C-----Version 3.30 call to VISDIS added; loop reorganized
         DGC=DML*GMW/1000.D0
         IF((IDX(4).EQ.2 .OR. IDX(4).EQ.4) .AND. T.GE.TUP) THEN
            CALL VISDIS (PROP, T, DGC)
         ELSE
            PROP(21) = 1.D-06*(DILVIS(T) + VFDC(DGC,T) + EXCSV(DGC,T))
         ENDIF
      ENDIF
      RETURN
      END
C
C
C
      DOUBLE PRECISION FUNCTION VISCW1 (DS,TS)
* Viscosity [microPa-s] of NW=1 fluids
* Density is in MOL/L, and temp in K.
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/POINTS/PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      ETA0= VISC0 (TS,DS)
      TRM1= EV(3) + EV(4)/(TS*SQRT(TS))
      TRM2= EV(5) + (EV(6) + EV(7)/TS)/TS
      TRMX= DEXP(EV(1) + EV(2)/TS)
      R1  = DS**0.1D0
      R2  = ((DS-DCC)/DCC)*SQRT(DS)
      VISCW1 = TRMX*(DEXP(TRM1*R1 + TRM2*R2) - 1.D0) + ETA0
      END
C
C
C
      DOUBLE PRECISION FUNCTION VISC0 (TX,DX)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Viscosity through linear term in density in MICRO-PA*S for NW=1 fluids.
* Density is in MOL/L, and temp in K.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      DIMENSION C0(9)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/TRAN10/GMW,EOK,SIG
      DATA C0/-3.0328138281D+00, 1.6918880086D+01,-3.7189364917D+01,
     *         4.1288861858D+01,-2.4615921140D+01, 8.9488430959D+00,
     *        -1.8739245042D+00, 2.0966101390D-01,-9.6570437074D-03/
      TS = TX / EOK
      TY = 1.D0 / TS
      TZ= TS**(1.D0/3.D0)
      E0=0.D0
      DO 200 J=1,9
      E0=E0+C0(J)*TY
  200 TY=TY*TZ
      OM22 = 1.D0 / E0
* ETA0 is viscosity at the low density limit.
      ETA0 = 2.6693D0 *DSQRT(GMW * TX) / (SIG*SIG * OM22)
      ETA1 = FV(1) + FV(2)*(FV(3)-DLOG(TX/FV(4)))**2
      VISC0=(ETA0+ETA1*DX)
      END
C
C
C
      DOUBLE PRECISION FUNCTION DILVIS (T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Dilute gas viscosity [microPa-s] for NW=0 fluids
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      SUM=0.0D0
      TF  = T**(1.D0/3.D0)
      TFF = TF**(-4)
      DO 10 I=1,9
      TFF = TFF*TF
   10 SUM = SUM + GV(I)*TFF
      DILVIS=SUM
      END
C
C
C
      DOUBLE PRECISION FUNCTION VFDC (D,T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* First density correction for viscosity [microPa-s] of NW=0 fluids
* Density D [gm/cm3]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      VFDC = (FV(1)+FV(2)*(FV(3)-DLOG(T/FV(4)))**2) *D
      END
C
C
C
      DOUBLE PRECISION FUNCTION EXCSV (D,T)
* Calculates excess viscosity [microPa-s] for NW=0 fluids
* Density D [gm/cm3]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      R2 = SQRT(D) * ((D-EV(8))/EV(8))
      R  = D**(.1D0)
      X  = EV(1) + EV(2)*R2 + EV(3)*R + (EV(4)*R2/T + EV(5)*R/SQRT(T)
     A     +EV(6) + EV(7)*R2) /T
      X1 = EV(1)+EV(6)/T
      EXCSV = DEXP(X)- DEXP(X1)
      END
C
C ---------------------------------- thermal conductivity functions
C
*  !!! Viscosity must be called first (before conductivity) !!!!
      SUBROUTINE COND (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Returns thermal conductivity in PROP(22) [W/(m*K)].
* Input is density(MOL/L), temp(K).
*
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/INDEXX/IDX(12)
      COMMON/TRAN2/ GMW, EOK, RM, TC, DC, X , PC, SIG
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
C-----Version 3.30; common DISCOF added
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
      T  = PROP(2)
      DML= PROP(3)
      IF (IDX(8) .EQ. 0) THEN
         PROP(22) = 0.
      ELSE IF (IDX(2) .EQ. 1) THEN
*        DGC is density [gm/cm3]
         DGC   = DML*GMW/1000.D0
C-----Version 3.30; call THCDIS added, loop reorganized.
         IF(IDX(4).EQ.2 .AND. T.GE.TUP) THEN
            CALL THCDIS(PROP,T,DGC)
         ELSE
            PROP(22)=DILTHC(T)+TCFDC(DGC,T)+EXCST(DGC,T)+TCCRIT(PROP)
         ENDIF
      ELSE
         TI  = 1.D0/T
         TRM0= ET(1)+ (ET(2) + ET(3)*TI)*TI
         TRM1= ET(4)+ (ET(5) + ET(6)*TI)*TI
         TRM2= ET(7)+ ET(8)*TI
         BACKG = THCON0(T) + (TRM0+TRM1*DML)*DML/(1.D0+TRM2*DML)
         PROP(22) = BACKG + THCRIT (PROP)
      ENDIF
      END
C
C
C
      DOUBLE PRECISION FUNCTION THCON0 (TT)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Dilute gas thermal conductivity for NW=1 fluids
* Thermal conductivity (W/(M*K)), TT(K), for NW=1 fluids.
*  Low density limit.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (R=8.31434D0, CON1=15.D0*R/4.D0, CON2=2.D0*CON1/3.D0)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/TRAN10/GMW,EOK,SIG
C-----Version 3.20: DUM2 dimension changed
      COMMON/SAVEOS/ DUM1(6), GASPRP(4), DUM2(80)
      CP0 = GASPRP(4) + R
* CP0 is specific heat in J/(MOL*K),
*  VISC0 is viscosity in MICRO-PA*S.
      ETA0   = 1.D-06*VISC0 (TT,0.0D0)
      TS     = TT/EOK
      YC     = (GT(1)+ GT(2)/TS)*(CP0- CON2)
      THCON0 = 1000.D0*ETA0*(CON1+ YC)/GMW
      END
C
C
C
      DOUBLE PRECISION FUNCTION THCRIT (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Critical enhancement W/(M*K), input MOL/L and K, for NW=1 fluids.
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      COMMON/POINTS/PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/TRAN11/E1,G1,B1,DE,BK,D1,XZ,ZZ,X1,X2,X3,X4
      COMMON/TRAN10/GMW,EOK,SIG
      DIMENSION PROP(0:31)
      TT = PROP(2)
      DD = PROP(3)
      PC1=PCC*1.D+3
      DELD=DABS(DD-DCC)/DCC
      DELT=DABS(TT-TCC)/TCC
      FACT= X1*DELT**4 + X2*DELD**4
      FACT=MIN(FACT,100.D0)
      DFACT = DEXP(-FACT)
      RSTAR = DD/DCC
* C Viscosity must be called before thermal conductivity; units [Pa-s]
      VIS = PROP(21)
*     DPT units [Pa/K]
      DPT = 1.D+03*PROP(8)
      IF (DELD .EQ. 0.D0) THEN
         BGAM  = XZ**G1/D1*((1.D0+E1)/E1)**((G1-1.D0)/(2.D0*B1))
         CHISTR= BGAM*(DELT)**(-G1)
      ELSE IF (DELD .LE. 0.25D0 .AND. DELT .LT. 0.03D0) THEN
* Critical region
         XX = DELT/DELD**(1.D0/B1)
         Y  = (XX+XZ)/XZ
         YE = E1*Y**(2.D0*B1)
         TOP=DELD**(-G1/B1) * ((1.D0+E1)/(1.D0+YE))**
     1       ((G1-1.D0)/(2.D0*B1))
         DIV=D1*(DE+(Y-1.D0)*(DE-1.D0/B1+YE)/(1.D0+YE))
         CHISTR=TOP/DIV
      ELSE
* Non-critical region; converting KPa to Pa.
         DPD = 1.D+03 * PROP(7)
         CHISTR = PC1*DD/(DCC**2*DPD)
      ENDIF
      CHI=CHISTR**X3
      UPPER=X4*BK/PC1*(TT*DPT/RSTAR)**2*CHI*DFACT
      SSENG=UPPER/(ZZ*6.D0*3.14159D0*VIS)
      THCRIT=SSENG
      END
C
C
C
      DOUBLE PRECISION FUNCTION DILTHC (T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Dilute gas thermal conductivity for NW=0 fluids
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PARAMETER (ONE3RD=1.D0/3.D0)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      TF  = T**ONE3RD
      TFF = TF**(-4)
      SUM = 0.
      DO 20 I=1,9
      TFF = TFF*TF
   20 SUM = SUM + GT(I)*TFF
      DILTHC = SUM
      END
C
C
C
      DOUBLE PRECISION FUNCTION TCFDC (D,T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* First density correction for thermal conductivity of NW=0 fluids
* Density D [gm/cm3]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      TCFDC = (FT(1)+FT(2)*(FT(3)-DLOG(T/FT(4)))**2)*D
      END
C
C
C
      DOUBLE PRECISION FUNCTION EXCST (D,T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Calculates excess thermal conductivity for NW=0 fluids
* Density D [gm/cm3]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN6/ GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      R2 = SQRT(D) * ((D-ET(8))/ET(8))
      R=D**(.1D0)
      X = ET(1) + ET(2)*R2 + ET(3)*R + (ET(4)*R2/T + ET(5)*R/SQRT(T)
     A   +ET(6) + ET(7)*R2) /T
      X1= ET(1) + ET(6)/T
      EXCST= DEXP(X)- DEXP(X1)
      END
C
C
C
      DOUBLE PRECISION FUNCTION TCCRIT (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Calculates critical enhancement for thermal conductivity of NW=0 fluids
*  Input units are G/CC, K,  output is W/(M*K).
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN2/  GMW, EOK, RM, TC, DC, X , PC, SIG
*VDA  DC [gm/cm3], PC[MPa]
      COMMON/TRAN12/ DELD,DELT,DSTAR,TSTAR
      DIMENSION PROP(0:31)
      DATA AV /6.0225D+23/, BK /1.38054D-16/
* DX in MOL/L,  D in G/CM3.
      T = PROP(2)
      DX= PROP(3)
      D = DX*GMW*0.001D0
C-----Version 3.20: next line
      DELD=DMAX1 ((DABS (D-DC)/DC), 1.D-08)
      DELT=DABS (T-TC)/TC
* Calculate distance parameter
      R = SQRT ((RM**5)*D*AV/GMW)
      R = R * X*SQRT (EOK/T)
* General equation
      DPT = PROP(8)
* DPDT in KPa/K.
      DPT=DPT*1.0D+4
* DPDT now in DYNES/(CM2*K)
      DPD = PROP(7)
* DPDD in L*KPa/MOL.
      DPD = DPD*1.0D+7/GMW
* DPDD now in DYNE*CM/G.
      IF( DPD .LE. 0.0D0) DPD=1.D0
*   94 VIS = VISCOS(DX,T)*(1.0D-05)
      VIS = PROP(21)*10.D0
* Viscosity now is G/(CM*S).
      IF ((DELD.GT.0.25D0) .OR. (DELT.GT.0.025D0)) THEN
         COMPRS=1.D0/SQRT(D*DPD)
      ELSE
         COMPRS=SENGR(D,T)
      ENDIF
      EX=BK * COMPRS * (T*DPT)**2
      EXB = R * SQRT (BK*T*D*AV/GMW)
      CRIT=EX/(EXB*6.D0*3.14159D0*VIS)
* Thermal cond, CRIT, is in ERG/(CM*SEC*K)
*  Put in damping factor
      BDD=((D-DC)/DC)**4
      BTT=((T-TC)/TC)**2
      BXX= -18.66D0*BTT - 4.25D0*BDD
      IF(BXX.LT.-1.D+2) BXX= -1.D+2
      FACT= DEXP( BXX )
      DELC=CRIT*FACT
      TCCRIT=DELC/100000.D0
* Thermal cond, TCCRIT, is now in W/(M*K)
      END
*
*
      DOUBLE PRECISION FUNCTION SENGR(D,T)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Scaled equation of state for critical region
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/TRAN2/ GMW, EOK, RM, TC, DC, X , PC, SIG
      COMMON/TRAN5/ BETA,XO,DELTA,E1, E2, AGAM
      COMMON/TRAN12/DELD,DELT,DSTAR,TSTAR
      DSTAR= D/DC
      TSTAR=T/TC
      BETO=1.D0/BETA
      XX=DELT/DELD**BETO
      AG=AGAM-1.D0
      BET2= 2.0D0*BETA
      AGB=AG/BET2
      DEL1=DELTA-1.D0
      AGBB=(AG-BET2)/BET2
      XXO=(XX+ XO)/XO
      XXB=XXO**BET2
      BRAK=1.D0 + E2*XXB
      BRAK1=BRAK**AGB
      H=E1*XXO*BRAK1
      HPRIM=(E1/XO)*BRAK1 + (AG/XO)*E1*E2*(XXB)*(BRAK**AGBB)
      RCOM=(DELD**DEL1)*(DELTA*H - (XX/BETA)*HPRIM  )
      RCOMP=1.D0/(RCOM*DSTAR**2)
      RCM=RCOMP/(PC*1.0D+7)
* RCM in CM2/DYNE, PC in MPA
      SENGR = SQRT (RCM)
      END
C
C
C
      SUBROUTINE DIFFUS (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
C Thermal diffusivity [m2/s] as a function of:
C Conductivity [W/m-K],  Density [mol/L], and Cp [J/mol-K]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31)
      PROP(23) = 1.D-3*PROP(22) / (PROP(3)*PROP(10))
      END
C
C
C
      SUBROUTINE PRNDTL (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
C Prandtl number as a function of:
C Viscosity [Pa-s], Cp [J/mol-K], and Conductivity [W/m-K]
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31)
C-----Version 3.30; 3 terms added to FLCON, next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      PROP(24) = 1.D+3*PROP(21)*PROP(10)/(PROP(22)*GMOLWT)
      END
*
* ============================== End of transport properties
*
*
      SUBROUTINE DIEL (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Dielectric constant= PROP(26).  Input P(MPA), D(MOL/L) and T(K).
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      DIMENSION PROP(0:31)
      COMMON/DIELCF/ BX(6)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/INDEXX/ IDX(12)
      IF (IDX(9) .EQ. 0) RETURN
      P = 1.D-03*PROP(1)
      T = PROP(2)
      D = PROP(3)
      IF (IDX(2).EQ.2) THEN
         CM= BX(1) + D*(BX(2)+D*BX(3)) + BX(4)*DLOG(1.D0+TCC/T)
     A       + BX(5)*P
      ELSE
         CM= BX(1) + D*(BX(2)+D*(BX(3)+D*BX(4))) + BX(5)*P + BX(6)*T
      ENDIF
      PROP(26)=(1.D0+2.D0*D*CM)/(1.D0-D*CM)
      END
C
C
C
      SUBROUTINE SURFTN (PROP)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
* Output: Surface tension [N/m] = PROP(25)
* Input:  Temperature [K]
*
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (A=1.46D0, P=11.D0/9.D0, B=2.D0/3.D0, C=0.3263008D0)
*                C=0.4**P
      DIMENSION PROP(0:31)
      COMMON/INDEXX/ IDX(12)
      COMMON/POINTS/ PCC, TCC, DCC, PTP, TTP, DTPL, DTPV, GPVN
      COMMON/SRFTEN/ SURF, PSURF, ACENT
C-----Version 3.30; 3 terms added to FLCON, next line
      COMMON/FLCON/  RCON, GMOLWT, XMOLWT, SMOLWT, XDIS
      T = PROP(2)
      IF (T .GE. TCC) THEN
         PROP(25) = 0.D0
         RETURN
      ELSE IF (IDX(10) .EQ. 0) THEN
* If IDX(10)=0, a "corresponding states" equation is used.
* See F.B.Sprow and J.M.Prausnitz, Trans. Faraday Soc. v62, 1097 (1966).
* Accuracy for simple fluids is about 5 percent.
* ACENT has been calculated in READCF
         D = 0.001D0*DCC*GMOLWT
         S = A*TCC*(D**B)*(1.D0+1.1D0*ACENT)/C
         E = P
      ELSE
* If IDX(10)> 0, SURF and PSURF are correct input parameters
         S = SURF
         E = PSURF
      ENDIF
      PROP(25) = 0.001D0*S * (1.D0 - T/TCC)**E
      RETURN
      END
*
*========================== End of properties
*
      INTEGER FUNCTION IBTWNR (A, B, C)
      IMPLICIT DOUBLE PRECISION (A-D)
      D = (B-A)*(B-C)
      IF (D .LT. 0.D0) THEN
         IBTWNR = 1
C-----Version 3.01; integer 0 changed to 0.D0 in following line; 11/14/91
      ELSE IF (D .EQ. 0.D0) THEN
         IBTWNR = 0
      ELSE
         IBTWNR = -1
      ENDIF
      END
C
C
C
C-----Version 3.1  
      SUBROUTINE ROOTAZ (R, A, B, C, Z, EXA, EXR, EYA, EYR, F, F0, JX)
* (C) Copyright (1992), Cryodata Inc.; all rights reserved.
*
*-----DESCRIPTION
* This subroutine finds the root, R, such that F(R) = F0,
* where F is an external function, and F0 is a specified constant.
*
*-----EXTERNAL VARIABLES
* This version of ROOTAZ assumes that all external floating-point
* variables are DOUBLE PRECISION
* OUTPUT VARIABLES
*   R           = the desired root,     if JX is positive;
*               = an approximate root,  if JX is zero; 
*               = an invalid root,      if JX is negative.
*   JX          = number of calls to F, if ROOTAZ was successful.   
*                 If JX=0, input error tolerances may be invalid.
*                 If JX = JMAX+4 = 54, iteration has been terminated without
*                 satisfying any convergence criterion.
*               = -1 if input values of A, B, C, and Z are invalid
*               = -2 if input values of EXA, EXR, EYA, or EYR are invalid.
*               = -3 if extrapolation beyond [B, C] is indeterminant.
*               = -4 if extrapolation beyond A or Z was attempted.
*               = -5 if extrapolation of a non-monotonic function
*                     was attempted.
* INPUT VARIABLES
*   A and Z     = limiting values of the independent variable which must
*                 bracket the root R.  A must not equal Z.  ROOTAZ will
*                 not extrapolate beyond these limits. 
*   B and C     = Initial guesses for the root R.  B must not equal C,
*                 and both must be within or at the limits of the
*                 range [A, Z].
*   EXA and EXR = absolute and relative tolerances on the independent
*                 variable.  R is accepted as the root if it is
*                 bracketed within the range EXA + EXR*R
*                 EXA must be > zero.  EXR must be >= zero.
*   EYA and EYR = absolute and relative tolerances on the dependent
*                 variable.  R is accepted as the root if the absolute
*                 value of F(R) - F0 is less than EYA + EYR*F0
*                 EYA must be > zero.  EYR must be >= zero.
*   F0          = the specified value of the function F
C
C Note:  ROOTAZ does not modify any of these input variables
C
*   F           = the external function.  It must be named in an EXTERNAL
*                 statement in the calling program.  It can be non-monotonic
*                 if B and C bracket R, but must be monotonic if 
*                 extrapolation is required..
*-----INTERNAL VARIABLES
* All internal floating point variables are DOUBLE PRECISION
* X(1)...X(4)     are successive approximations to R.
* Y(1)...Y(4)     are successive values of F(X(i)) - F0.
* ESYS            specifies the machine precision of the external variables.  
*                 It must be larger than the smallest number EPS such that
*                 1 + EPS is not equal to 1 in double precision arithmetic.
* JMAX            should be chosen such that 2**JMAX > (1/ESYS)**2 
*                 in order to assure convergence for any F.
* Other variables are used in intermediate steps.
C
*-----VERSION Sept. 13, 1992;  V. Arp
      IMPLICIT DOUBLE PRECISION (A-H, O-Z)
      PARAMETER (ESYS=1.D-08, OME=1.D0-ESYS, JMAX=50)
      LOGICAL LIMIT
      DIMENSION X(4), Y(4)
      EXTERNAL F
C Statement function
      EX(J1,J2) = EXA + (EXR+ESYS)*ABS(X(J1)) + (EXR+ESYS)*ABS(X(J2))
C
C Check for valid parameters
C     
      IF ((A .EQ. Z) .OR. (B .EQ. C) .OR. ((B-A)*(B-Z) .GT. 0.D0) .OR.
     +   ((C-A)*(C-Z) .GT. 0.D0)) THEN
         JX = -1
         RETURN
      ENDIF
      IF ((EXA .LE. 0.D0) .OR. (EXR .LT. 0.D0) .OR. (EYA .LE. 0.D0)
     +      .OR. (EYR .LT. 0.D0)) THEN
         JX = -2
         RETURN
      ENDIF
      LIMIT  = .FALSE.
      EY     = EYA + (ESYS + EYR)*ABS(F0)
C
C First guess         
C
      Y(1) = F(B) - F0
      IF (ABS (Y(1)) .LT. EY) THEN
         JX = 1
         R = B
         RETURN
      ENDIF
C
C Second guess         
C
      Y(2) = F(C) - F0
      IF (ABS (Y(2)) .LT. EY) THEN
         JX = 2
         R = C
         RETURN
      ENDIF
      R21 = Y(2)/Y(1)
C
C Order such that Y(2) is smaller than Y(1).  This is required for
C extrapolation, where monotonic F(x) is assumed.
C If extrapolation is not required, F(x) can be non-monotonic.
C
      IF (ABS(R21) .GT. 1.D0+ESYS) THEN
         R32  = Y(2)
         Y(2) = Y(1)
         Y(1) = R32
         X(1) = C
         X(2) = B
      ELSE IF (R21 .LT. OME) THEN
         X(1) = B
         X(2) = C
      ELSE
C Failure: Constant function F; indeterminant extrapolation.  
         JX = -3
         RETURN
      ENDIF
C
C Third try: linear interpolation or extrapolation
C
      STEP = X(2) - X(1)
      X(3) = Y(2)*STEP/(Y(1)-Y(2)) + X(2)
      IF (R21 .GT. 0.D0) THEN
C
C Extrapolation; check limits.
C First, find which limit applies.
C
         IF (((A-X(2))/STEP) .GE. 0.D0) THEN
            XLIM = A
         ELSE
            XLIM = Z
         ENDIF
         IF (ABS(X(2)-XLIM) .LT. 0.5D0*EX(2,2)) THEN
C Failure: A=B or A=C or Z=B or Z=C; extrapolation is not permitted.
            JX = -4
            RETURN
         ENDIF
C Do not extrapolate beyond XLIM.
         IF ((X(3)-XLIM)*(X(3)-X(1)) .GE. 0.D0) THEN
            X(3) = XLIM
            LIMIT = .TRUE.
         ENDIF
      ELSE IF (ABS(STEP) .LE. EX(1,2)) THEN
C B and C straddle the root.
C Return if their separation is is less than the minimum acceptable
C separation specified by EXA, EXR, and ESYS.   User should check for
C input error.
         JX = 0
         R = X(3)
         RETURN
      ENDIF
      Y(3) = F(X(3)) - F0
      IF (ABS(Y(3)) .LT. EY) THEN
         R = X(3)
         JX = 3
         RETURN
      ENDIF
      JF = 0
      JG = 0
C
C-----Do loop
C
      DO 480 JJ = 0, JMAX
      R21 = Y(2)/Y(1)
      R32 = Y(3)/Y(2)
      IF ((R32 .GT. 0.D0) .AND. (R21 .GT. 0.D0)) THEN
C
C Root not bracketed; extrapolation still required.
C
         IF (LIMIT) THEN
C    Failure; further extrpolation is prohibited.
            JX = -4
            RETURN
         ENDIF
         IF (R32 .GE. OME) THEN
C    Extrapolation failure: constant or non-monotonic function.
            JX = -5
            RETURN
         ENDIF
         XI = Y(3)*(X(3)-X(2))/(Y(2)-Y(3))
         IF (R21 .GT. R32) THEN
C    Use inverse quadratic extrapolation if abs(slope) is decreasing.
            XE = Y(3)*(X(3)-X(1))/(Y(1)-Y(3))
            W  = Y(1)/(Y(1)-Y(2))
            XI = W*XI + (1.D0-W)*XE
         ENDIF
         X(4) = XI + X(3)
         IF (R32 .GT. 0.1D0) THEN
C    If extrapolation convergence is slow, give an artificial boost.          
            IF (JJ .GE. 2) THEN
               X(4) = X(4) + REAL(JJ-1)*STEP
            ELSE
               X(4) = X(4) + REAL(JJ+1)*(X(3) - X(2))
            ENDIF
         ENDIF
C    But do not extrapolate beyond the limit.
         IF ((X(4)-XLIM)*(X(4)-X(1)) .GT. 0.D0) THEN
            X(4) = XLIM
            LIMIT = .TRUE.
         ENDIF
         JF = 0
         JS = 2
      ELSE
C
C Root has been bracketed.
C    Define the geometry.
         IF (R32 .LT. 0.D0) THEN
            JO = 1
            JF = 0
            IF (R21 .LT. 0.D0) THEN
               JS = 2
               JD = 3
               JG = JG + 1
            ELSE
               JS = 3
               JD = 2
               JG = 0
            ENDIF
         ELSE
            JS = 1
            JD = 3
            JO = 2
            JF = JF+1
            JG = 0
         ENDIF
         XJD = X(JD)
         XJS = X(JS)
         XJO = X(JO)
         YJD = Y(JD)
C    Exit if the bracket width is less than EX
         IF (ABS(XJS-XJD) .LE. EX(JS,JD)) THEN
            JX = JJ+3
            R = X(3)
            RETURN
         ENDIF
         IF ((ABS(R32) .GE. OME) .OR. 
     +       ((JG .GE. 2) .AND. (R32 .LT. -0.7D0)))  THEN
C    Not converging: bisect.
            X(4) = 0.5D0*(XJS+XJD)
            JF = 0
         ELSE
C
C    The following approximates quadratic interpolation when converging
C    nicely.  Otherwise it can accelerate the search when large changes
C    in slope and/or slow movement toward a distant root are found.
C
            XI  = YJD*(XJD-XJS)/(Y(JS)-YJD) + XJD
            IF (ABS (Y(JO)-YJD) .LT. EY) THEN
               XE = XJS
            ELSE
               XE  = YJD*(XJD-XJO)/(Y(JO)-YJD) + XJD
               IF ((XE-XJS)*(XE-XJD) .GT. 0.D0) XE = XJS
            ENDIF
            W = (XJD-XJO)/(XJS-XJO)
            IF ((JF .GE. 2) .AND. (W .LT. 0.3D0) .AND.
     +          (R32 .GT. 0.25D0)) THEN
               X(4) = X(3) + (X(1) - X(3))*SQRT(W)
            ELSE
               X(4) = W*XI + (1.D0-W)*XE
            ENDIF
         ENDIF
      ENDIF
C X(4), Y(4) are the new estimate.
      Y(4) = F(X(4)) - F0
      IF (ABS (Y(4)) .LT. EY) THEN
         JX = JJ+4
         R = X(4)
         RETURN
      ENDIF
C Update indices.
      IF (JS .GE. 2) THEN
         X(1) = X(2)
         Y(1) = Y(2)
      ENDIF
      X(2) = X(3)
      X(3) = X(4)
      Y(2) = Y(3)
      Y(3) = Y(4)
  480 CONTINUE
C This statement should never be reached 
      R = X(4)
      JX = JMAX+4
      RETURN
      END
C
C==================================================================
C     Following subroutines by R.D. McCarty, Nov. 1996 
C     Calculation of high temperature and disassociation properties
C==================================================================
C
      SUBROUTINE IDGDIS(GAS0P,P,T)
       IMPLICIT DOUBLE PRECISION (A-H,O-Z)
       DIMENSION GAS0P(4)
C  CALCULATES THE IDEAL GAS PROPERTIES FOR A DISASSOCIATING GAS
C  INPUT UNITS ARE K, JOULES, ATM, AND MOLES
C  HEAT OF DISASSOCIATION USED HERE FOR HYDROGEN
c  IS 103238 CAL/MOLE CONVERTED TO JOULES
C   , FOR NITROGEN IS 941607 JOULES/MOL
C  WRITTEN IN NOVEMBER 1996 BY R.D. MCCARTY,
C EQUATIONS ARE FROM WORK BY HANLEY, MCCARTY AND INTEMANN, NBS J OF
C RESEARCH A. VOL 74A, NO 3 MAY-JUNE 331, 1970, AND RELATED COMPUTER CODE
C   FOLLOWING COMMON STATEMENT MODIFIED TO INCLUDE XMOLWT, 11/21/96 RDM
       COMMON/FLCON/  R, GMOLWT, XMOLWT, SMOLWT,XDIS
       COMMON/DISCOF/A(5),GGV(9),GGT(9),GAV(9),GAT(9),DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
       IF(T.LT.TUP2)RETURN
       X=DIS(P,T)
       XX=X*X
       CPA=2.5
       CPM=(GAS0P(4)+R)/R
       HM=GAS0P(2)+431947.
       BRAC=5.-HM/(T*R)
       CP=2.*X*CPA+(1.-X)*CPM+.5*(1.-XX)*X*BRAC**2
       GAS0P(4) =CP*R-R
       SA=DLOG(T)*2.5*R
       SM=GAS0P(1)
       S=(1.-X)*SM+2.*X*SA+(1.+X)*R*DLOG(1.+X)-(1.-X)*R*DLOG(1.-X)
     1-2.*X*R*DLOG(2.*X)-(1.+X)*R*DLOG(P)
       GAS0P(1)=S
       HA=R*T*2.5
       HM=GAS0P(2)
       H=(1.-X)*HM+2.*X*HA+X*DELE
       GAS0P(2)=H
       GAS0P(3)=H-R*T
       RETURN
       END
C
C
C
      DOUBLE PRECISION FUNCTION DIS(P,TT)
C   CALCULATES THE DISASSOCIATION FRACTION FOR A FLUID AS A FUNCTION OF
C   PRESSURE P, IN ATMOSPHERES AND TEMPERATURE T IN kELVIN.  CALCULATED
C  FROM THE EQUILIBRIUM CONSTANTS K DERIVED FROM CLACULATIONS OUTLINED
C  IN WOLLEY SCOTT AND BRICWEEDE NBS J OF RESSEARCH RP 1932.
C   THERMODYNAMIC PROPERTIES REQUIRED FOR THE CALCULATION ARE FROM
C THE ABOVE REFERENCE FOR H2 AND FROM THE JANAF TABLES FOR N2.
C HEAT OF DISASSOCIATION WAS TAKEN FROM THE NBS REFERENCE FOR H2 AND
C FROM LOFTUS AND KRUPENIEM J PHYS & CHEM REF DATA VOL 6, NO 1 1977.
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION F(5)
       COMMON/FLCON/  R, GMOLWT, XMOLWT, SMOLWT,XDIS
      COMMON/DISCOF/A(5),GGV(9),GGT(9),GAV(9),GAT(9),DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
      T=TT/2000.
      XC=0
      F(1)=1./T**2
      F(2)=1./T
      F(3)=T
      F(4)=T**2
      F(5)=1.
      DO 25 J=1,5
   25 XC=XC+A(J)*F(J)
      FK=DEXP(XC-12.846)
      DIS=(FK/(FK+4.*P))**.5
      XDIS=DIS
      GMOLWT=(1.-DIS)*XMOLWT+DIS*XMOLWT/2.
      SMOLWT=GMOLWT
      RETURN
      END
C
C
C
      SUBROUTINE DISCOEF(II)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION H2X(5),FN2X(5),H2GT(9),FN2GT(9),H2GV(9),FN2GV(9),
     1H2GAT(9),H2GAV(9),FN2GAT(9),FN2GAV(9),H2DISV(4),H2DIST(3)
      COMMON/TRAN6/GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
C  GX ARE THE COEFFIICENTS FOR THE DISASSOCIATION CONSTANT K, GGV ARE
C  THE COEFFICIENTS FOR THE HIGH TEMPERATURE MOLECULAR DILUTE VISCOSITY,
C  GGT ARE THE COEFFICIENTS FOR THE HIGH TEMP THERMAL CONDUTIVE MOLECULE
C  GAV ARE THE COEFFICIENTS FOR THE HIGH TEMP  ATOMIC VISCOSITY
C  GAT ARE THE COEFFICIENTS FOR THE HIGH TEMP ATOMIC THERMAL CONDUCTIVE
C  DISV ARE THE COEFFICIENTS FOR THE DISASSOCIATION CONTRIBUTION TO VIS
C  dIST ARE THE COEFFICIENTS FOR THE DIS.. CONTRIBUTION TO THERMAL CONDUCT
C  fACT IS THE SAME AS DELE IS THE DISASSOCIATION ENERGY IN JOULES/MOLE
C  TUP IS THE TEMPERATURE WHERE THE NEW THC AND VIS BEGIN TO BE USED
C  TUP2 IS THE TEMPERATURE WHERE DISASSOCIATION CALCULATIONS BEGIN
C  FOR UNITS APPLICABLE TO THE ABOVE COEFFICIENTS SEE THE APPROPRIATE
C  ROUTINES.
C  MATRICIES WITH H2 OR FN2 PREFEX ARE FOR THE INDIVIDULA GASES
C  ALL COEFFICIENTS FROM FITS BY R.D. MCCARTY, NOVEMBER 1996
      DATA H2X/ .2118172389866036D-01,-.2690064330724328D+02,
     1.6124841965091452D+0,-.9164575525046011D-1,.2635836618419776D+2/
      DATA FN2X/.5809037464731244D+00,-.5942475386282104D+02,
     1 -.1251522808422072D+0,.2879967206401599D-1,.2622478216356694D+2/
      DATA H2GT/.4867367597991E+05, -.3840897821396E+05,
     1.1305714248552E+05, -.2499936065236E+04,.2951804640444E+03,
     2-.2202843407541E+02,.1015716846958E+01,-.2643479681048E-01,
     3.2975245793745E-03/
      DATA FN2GT/
     1-.9873109114265E+02,.1118599784920E+03,-.5311622278158E+02,
     2.1374697385041E+02,-.2117560946903E+01,.1991617788923E+00,
     3-.1108650576066E-01, .3394960379931E-03,-.4384079739987E-05/
      DATA H2GV/ .3597459516279E+06, -.2907864068495E+06,
     1.1010108273374E+06,-.1969542146586E+05,.2357963788723E+04,
     2-.1773345862550E+03,.8211313268144E+01,-.2133279099683E+00,
     3.2385420317913E-02/
      DATA FN2GV/
     1-.7273900514904E+03,.6498303763133E+03,-.1682623850857E+03
     2,-.4719712643829E+01,.6215560253903E+01,-.3440007984008E+00
     3,.3610595253942E-01,-.7142567643153E-03,.6351722000133E-05/
      DATA H2GAV/ .7791433249432E+06,.3438280949718E+06
     1,-.2794947448131E+06,.6411001537068E+05,-.7280622276270E+04
     2,.4534378845259E+03,-.1509587997811E+02,.2264917802515E+00
     3,-.6957031420486E-03/
      DATA FN2GAV/-.9591616190051E+04,.3074256368958E+04
     1,-.3868087010294E+03,.1157470129907E+02,.2625072267414E+01
     2,.7364067495162E-01,.4598124938740E-02,.2625777067162E-03
     3,-.6013169347841E-05/
      DATA H2GAT/ -.1588880396651E+07,.4572557167411E+06
     1,-.3115214134025E+05,-.2876275701444E+04,.4992723913624E+03
     2,-.1973185064936E+02,-.3544923220675E+00,.4229716599154E-01
     3,-.7920497429793E-03/
      DATA FN2GAT/.9010096227074E+04,-.2387445995155E+04
     1,.1625245033947E+03,.8410890804927E+01,-.1408678078238E+01
     2,.2555481434631E-01,.3367971172971E-02,-.1809924243854E-03
     3,.2668354669540E-05/
      DATA H2DISV/ .1585961783778E-01,-.2080285472410E+02
     1,.3838178512297E-01,-.1227097145662E-01/
      DATA H2DIST/ .3755275903354E-01,.9075743755146E+01
     1,.3958918196034E+06/
      DATA H2FACT/431947./
      DATA FN2FACT/941607./
      DATA H2TUP/400./
      DATA H2TUP2/1000./
      DATA FN2TUP/400./
      DATA FN2TUP2/2500./
       IF(II.EQ.2)THEN
       DO 10 I=1,9
       GGV(I)=H2GV(I)
       GGT(I)=H2GT(I)
       GAT(I)=H2GAT(I)
       GAV(I)=H2GAV(I)
       IF(I.LT.6) GX(I)=H2X(I)
   10 CONTINUE
       DO 11 I=1,4
       DISV(I)=H2DISV(I)
       IF(I.LT.4) DIST(I)=H2DIST(I)
   11 CONTINUE
       DELE=H2FACT
       TUP=H2TUP
       TUP2=H2TUP2
       ELSE
       DO 20 I=1,9
       GGV(I)=FN2GV(I)
       GGT(I)=FN2GT(I)
       GAT(I)=FN2GAT(I)
       GAV(I)=FN2GAV(I)
       IF(I.LT.6) GX(I)=FN2X(I)
   20  CONTINUE
       DO 21 I=1,4
       DISV(I)=H2DISV(I)*2.
       IF(I.LT.4) DIST(I)=H2DIST(I)*2.
   21 CONTINUE
       DELE=FN2FACT
       TUP=FN2TUP
       TUP2=FN2TUP2
       END IF
       RETURN
       END
C
C
C
      SUBROUTINE VISDIS (PROP,T,DGC)
C  THIS ROUTINE IS CALLED FOR H2 AND N2 AT TEMPERATURES ABOVE THE
C  DISASSOCIATION THRESHOLD FOR THAT GAS, INPUT IS THE PROPERTY MATRIX
C  PROP, TEMPERATURE IN KELVIN T, AND DENSITY IN GRAMS/CC DGC.  oUTPUT
C  GOES DIRECTLY INTO THE PROP MATRIX, UNITS FOR THE OUTPUT ARE NATIVE
C  r.d. McCARTY NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION PROP(0:31)
      DIMENSION GGVX(9)
      COMMON/TRAN6/GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2 ,DELE
      DO 10 I=1,9
      GGVX(I)=GV(I)
   10 GV(I)=GGV(I)
      TIN=TUP
        IF(T.GT.TUP2)THEN
        P=PROP(1)/101.325
        CALL DISVIS(P,T,X)
        VISCOS=VFDC(DGC,TIN) + EXCSV(DGC,TIN)+X
        ELSE
         VISCOS = DILVIS(T) + VFDC(DGC,TIN) + EXCSV(DGC,TIN)
        END IF
      PROP(21) =  1.D-06*VISCOS
      DO 20 I=1,9
   20 GV(I)=GGVX(I)
      RETURN
      END
C
C
C
      SUBROUTINE THCDIS (PROP,T,DGC)
C  THIS ROUTINE IS CALLED FOR H2 AND N2 FOR ALL TEMPERATURES ABOVE
C  THE DISASSOCIATION THRESHOLD.  INPUT IS THE PROPERTY MATIRX PROP,
C  TEMPERATURE IN KELVIN T, AND DENSITY IN GRAMS/CC DGC. OUTPUT IS
C  DIRECTLY INTO THE PROP MATIRX IN NATIVE UNITS
C  R.D. McCARTY,NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION PROP(0:31)
      DIMENSION GGTX(9)
      COMMON/TRAN6/GV(9),GT(9),FV(4),FT(4),EV(8),ET(8)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
      DO 10 I=1,9
      GGTX(I)=GT(I)
   10 GT(I)=GGT(I)
      TIN=TUP
        IF(T.GT.TUP2)THEN
        P=PROP(1)/101.325
        CALL DISTHC(P,T,X)
        THCON= TCFDC(DGC,TIN) + EXCST(DGC,TIN)+X
        ELSE
        THCON = DILTHC(T) + TCFDC(DGC,TIN) + EXCST(DGC,TIN)
        END IF
      PROP(22) = THCON
      DO 20 I=1,9
   20 GT(I)=GGTX(I)
      RETURN
      END
C
C
C
      SUBROUTINE DISTHC(PP,TT,PRO)
C  THIS ROUTINE CALCULATES THE DISASSOCIATION CONTRIBUTION TO THE
C  THERMAL CONDUCTIVITY.  INPUT IS PRESSURE PP IN ATMOSPHERES ,
C  TEMPERATURE TT IN kELVIN,. OUTPUT IS THERMAL CONDUCTIVITY PRO IN
C  NATIVE UNITS
C  R.D. McCARTY, NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
       DIMENSION  GAS0P(4),F(3)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
C   FOLLOWING COMMON STATEMENT MODIFIED TO INCLUDE XMOLWT, 11/21/96 RDM
      COMMON/FLCON/  R, GMOLWT, XMOLWT, SMOLWT,XDIS
      P=PP
      T=TT
      TA=DILATHC(T)
      TM=DILTHC(T)
      X=DIS(P,T)
      CALL DILGAS(GAS0P,T)
       HM=GAS0P(2)+FACT
       BRAC=5.-HM/(T*R)
        x1=1.-x
      x2=x
      x11=(1.+x1)**2
      x12=x1*x2
      CALL DILGAS(GAS0P,T)
       HM=GAS0P(2)+FACT
       BRAC=5.-HM/(T*R)
       sum=0.0
       x12=x1*x2
       F(1)=(x1*x2/(1+x1)**2)*BRAC**2
       f(2)=x12/x11
       f(3)=x12/x11/t
       do 10 i=1,3
   10 SUM=SUM+F(I)*DIST(I)
       DISCON=SUM+X1*TM+X2*TA
       PRO=DISCON
      RETURN
      END
C
C
C
      SUBROUTINE DISVIS(PP,TT,PRO)
C  THIS ROUTINE CALCULATES THE CONTRIBUTION TO VISCOSITY PRODUCED BY
C  THE DISASSOCIATION, INPUT IS PRESSURE PP IN ATMOSPHERES, TEMPERATURE
C  TT IN kELVIN.  oUTPUT IS VISCOSITY PRO IN NATIVE UNITS
C VALUES FOR H2 ARE FROM WORK BY HANLEY, MCCARTY AND INTEMANN, NBS J OF
C VALUES FOR N2 ARE SCALED FROM H2 AS NO OTHER DATA EXIST
C RESEARCH A. VOL 74A, NO 3 MAY-JUNE 331, 1970,
C  R.D. McCARTY, NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      DIMENSION  GAS0P(4), F(4)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
C   FOLLOWING COMMON STATEMENT MODIFIED TO INCLUDE XMOLWT, 11/21/96 RDM
       COMMON/FLCON/  R, GMOLWT , XMOLWT, SMOLWT,XDIS
      P=PP
      T=TT
      VA=DILAVIS(T)
      VM=DILVIS(T)
      X=DIS(P,T)
      x1=1.-x
      x2=x
      x11=(1.+x1)**2
      x12=x1*x2
       CALL DILGAS(GAS0P,T)
       HM=GAS0P(2)+FACT
       BRAC=5.-HM/(T*R)
       sum=0.0
       F(1)=(x1*x2/(1+x1)**2)*BRAC**2
       f(2)=x12/x11
       f(3)=T*x12/x11
       F(4)=(1.+X2)*T*X12/x11
       do 10 i=1,4
   10 SUM=SUM+F(I)*DISV(I)
       DISCIS=SUM+X1*VM+X2*VA
       PRO=DISCIS
       RETURN
      END
C
C
C
      DOUBLE PRECISION FUNCTION DILAVIS (T)
* (C) Copyright (1996), Cryodata Inc.; all rights reserved.
* Dilute gas viscosity FOR HIGH TEMPERATURE DISASSOCIATED ATOMIC FLUIDS
C  INPUT IS TEMPERATURE T IN KELVIN, OUTPUTIS VISCOSITY IN NATIVE UNITS
C VALUES FOR H2  ARE FROM WORK BY HANLEY, MCCARTY AND INTEMANN, NBS J OF
C RESEARCH A. VOL 74A, NO 3 MAY-JUNE 331, 1970, VALUES FOR N2 ARE SCALED
C  R.D. McCARTY, NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2 ,DELE
C   FOLLOWING COMMON STATEMENT MODIFIED TO INCLUDE XMOLWT, 11/21/96 RDM
       COMMON/FLCON/  R, GMOLWT, XMOLWT, SMOLWT,XDIS
      SUM=0.0D0
      TF  = T**(1.D0/3.D0)
      TFF = TF**(-4)
      DO 10 I=1,9
      TFF = TFF*TF
   10 SUM = SUM + GAV(I)*TFF
      DILAVIS=SUM
      END
C
C
C
       DOUBLE PRECISION FUNCTION DILATHC (T)
* (C) Copyright (1996), Cryodata Inc.; all rights reserved.
* Dilute gas thermal conductivity for high temperature disassociated
c atomic gases
C VALUES FOR H2 ARE FROM WORK BY HANLEY, MCCARTY AND INTEMANN, NBS J OF
C RESEARCH A. VOL 74A, NO 3 MAY-JUNE 331, 1970, VALUES FOR N2 ARE SCALED
C FROM HYDROGEN AS NO OTHER DATA EXIST
C  R.D. McCARTY, NOVEMBER 1996
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      PARAMETER (ONE3RD=1.D0/3.D0)
      COMMON/DISCOF/GX(5),GGV(9),GGT(9),GAV(9),GAT(9), DISV(4),DIST(3)
     1,FACT,TUP,TUP2,DELE
        TF  = T**(1.D0/3.D0)
      TFF = TF**(-4)
      SUM = 0.
      DO 20 I=1,9
      TFF = TFF*TF
   20 SUM = SUM + GAT(I)*TFF
      DILATHC=SUM
      END
