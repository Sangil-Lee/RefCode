/*******************************************************************
* SMOOTHING AN ARRAY OF N ORDINATES Y's (ASCENDING ORDER ABCISSAS) *
* ---------------------------------------------------------------- *
* Description:                                                     *
* This program uses the procedure SMOOFT for smoothing an array of *
* given ordinates (y's) that are in order of increasing abscissas  *
* (x's), but without using the abscissas themselves supposed to be *
* equally spaced. It first removes any linear trend, then uses a   *
* Fast Fourier Transform procedure (REALFT) to low-pass filter the *
* data. The linear trend is reinserted at the end. One user-speci- *
* fied parameter, EPS, enters "the amount of smoothing", given as  *
* the number of points over which the data should be smoothed.     *
* ---------------------------------------------------------------- *
* SAMPLE RUN:                                                      *
* Input data file contains:                                        *
* 1024                                                             *
* 0.00000000000000E+0000 7.50000000000000E-0001                    *
* 9.21288168207468E-0003 7.77368637910513E-0001                    *
* 1.84257633641494E-0002 8.34466556277221E-0001                    *
* 2.76386450462383E-0002 9.03071871110114E-0001                    *
* 3.68515267282987E-0002 9.92958153417021E-0001                    *
* 4.60644084103592E-0002 1.09195646826811E+0000                    *
* 5.52772900924197E-0002 1.15230452277865E+0000                    *
* 6.44901717745370E-0002 1.06763022290215E+0000                    *
* 7.37030534565974E-0002 1.34541171127239E+0000                    *
* 8.29159351386579E-0002 1.48611048393104E+0000                    *
* 9.21288168207184E-0002 1.09349703210864E+0000                    *
* 1.01341698502779E-0001 1.72386602840743E+0000                    *
* 1.10554580184839E-0001 1.14317464708984E+0000                    *
* ---------------------- ----------------------                    *
* 9.37871355209791E+0000 2.43969819122867E+0001                    *
* 9.38792643377383E+0000 2.42468007203424E+0001                    *
* 9.39713931544975E+0000 2.42436619192304E+0001                    *
* 9.40635219712567E+0000 2.42829449073179E+0001                    *
* 9.41556507880159E+0000 2.42980085689633E+0001                    *
* 9.42477796047751E+0000 2.43119449022633E+0001                    *
*                                                                  *
* Output file contains (here EPS=30):                              *
*                                                                  *
*       Time          Y        Smoothed Y                          *
* ----------------------------------------                         *
*     0.000000     0.750000     0.788488                           *
*     0.009213     0.777369     0.816559                           *
*     0.018426     0.834467     0.846302                           *
*     0.027639     0.903072     0.877999                           *
*     0.036852     0.992958     0.911780                           *
*     0.046064     1.091956     0.947587                           *
*     0.055277     1.152305     0.985167                           *
*     0.064490     1.067630     1.024100                           *
*     0.073703     1.345412     1.063859                           *
*     0.082916     1.486110     1.103896                           *
*     0.092129     1.093497     1.143733                           *
*     0.101342     1.723866     1.183046                           *
*     0.110555     1.143175     1.221738                           *
*     --------     --------     --------                           *
*     9.378709    24.396982    24.223680                           *
*     9.387921    24.246801    24.246965                           *
*     9.397134    24.243662    24.271065                           *
*     9.406346    24.282946    24.295721                           *
*     9.415559    24.298008    24.320887                           *
*     9.424771    24.311945    24.346704                           *
*                                                                  *
* ---------------------------------------------------------------- *
* Reference:  "Numerical Recipes By W.H. Press, B. P. Flannery,    *
*              S.A. Teukolsky and W.T. Vetterling, Cambridge       *
*              University Press, 1986" [BIBLI 08].                 *
*                                                                  *
*                              C++ Release By J-P Moreau, Paris.   *
*******************************************************************/
#include <stdio.h>
#include <math.h>

#define  NMAX  2048

int   i, ndata;
float signal[NMAX], ysave[NMAX];
float dt, t, tbegin, temp, tend;

FILE  *fp_in, *fp_out;

   void realft(float *data, int n, int isign);


float MAX(float a, float b) {
  if (a>=b) return a;
  else return b;
}


void SMOOFT(float *Y, int N, int PTS) {
/*--------------------------------------------------------------
  Smooths an array Y of length N, with a window whose full width
  is of order PTS neighboring points, a user supplied value. 
  Array Y is modified.
--------------------------------------------------------------*/
//Label: e1
  int   J,K,M,MO2,NMIN;
  float CNST,FAC,RN1,Y1,YN;
  float ONE=1.0;
  float QUART=0.25;
  float ZERO=0.0;
  
  M=2;
  NMIN=N + 2*PTS;
e1:if (M < NMIN) {
    M=2*M;
    goto e1;
   }
  if (M > NMAX) {
    printf(" Sample too big.\n");
    return;
  }
  CNST=(ONE*PTS/M)*(ONE*PTS/M);
  Y1=Y[1];
  YN=Y[N];
  RN1=ONE/(N-1);
  for (J=1; J<=N; J++)      //Remove linear trend
    Y[J]=Y[J]-RN1*(Y1*(N-J)+YN*(J-1));
  if (N+1 <= M)
    for (J=N+1; J<=M; J++)  Y[J]=0.0;
  MO2=M/2;
  realft(Y,MO2,1);      //Fourier transform
  Y[1]=Y[1]/MO2;
  FAC=1.0;
  for (J=1; J<MO2; J++) {
    K=2*J+1;
    if (FAC != 0.0) {
      FAC=MAX(ZERO,(ONE-CNST*J*J)/MO2);
      Y[K]=FAC*Y[K];
      Y[K+1]=FAC*Y[K+1];
    }
    else {
      Y[K]=0.0;
      Y[K+1]=0.0;
    }
  }
  FAC=MAX(ZERO,(ONE-QUART*PTS*PTS)/MO2);  //Last point
  Y[2]=FAC*Y[2];
  realft(Y,MO2,-1);       //Inverse Fourier transform
  for (J=1; J<=N; J++)    //Restore linear trend
    Y[J]=RN1*(Y1*(N-J)+YN*(J-1))+Y[J];
}


void four1(float *data, int nn, int isign) { 
/*------------------------------------------------------------------------------------------- 
 Replaces data(1:2*nn) by its discrete Fourier transform, if isign is input as 1; or replaces 
 data(1:2*nn) by nn times its inverse discrete Fourier transform, if isign is input as -1. 
 data is a complex array of length nn or, equivalently, a real array of length 2*nn. nn 
 MUST be an integer power of 2 (this is not checked for here). 
--------------------------------------------------------------------------------------------*/
//Labels: e1, e2
  int    i,istep,j,m,mmax,n; 
  float  tempi,tempr; 
  double theta,wi,wpi,wpr,wr,wtemp;  //Double precision for the trigonometric recurrences
     
  n=2*nn;
  j=1;
  i=1;
  while (i<=n) {  //This is the bit-reversal section of the routine
	if (j > i) {
      tempr=data[j];        //Exchange the two complex numbers
      tempi=data[j+1];
      data[j]=data[i];
      data[j+1]=data[i+1];
      data[i]=tempr;
      data[i+1]=tempi;
    } 
    m=nn;
e1: if (m >=2 && j > m) {
      j=j-m;
      m=m/2;
      goto e1; 
    } 
    j=j+m;
    i += 2;
  } 
  mmax=2;                     //Here begins the Danielson-Lanczos section of the routine
e2:if (n > mmax) {                                   //Outer loop executed log2 nn times
    istep=2*mmax;
    theta=6.28318530717959/(isign*mmax);   //Initialize for the trigonometric recurrence
    wpr=-2.0*sin(0.5*theta)*sin(0.5*theta);
    wpi=sin(theta);
    wr=1.0;
    wi=0.0;
    m=1;
    while (m<=mmax) {                              //Here are the two nested inner loops
      i=m;
      while (i<=n) {
        j=i+mmax;                                //This is the Danielson-Lanczos formula:
        tempr=wr*data[j]-wi*data[j+1];
        tempi=wr*data[j+1]+wi*data[j];
        data[j]=data[i]-tempr;
        data[j+1]=data[i+1]-tempi;
        data[i]=data[i]+tempr;
        data[i+1]=data[i+1]+tempi;
        i += istep;
      }
      wtemp=wr;  //Trigonometric recurrence
      wr=wr*wpr-wi*wpi+wr;
      wi=wi*wpr+wtemp*wpi+wi;
      m += 2;
    } 
    mmax=istep;
    goto e2;  //Not yet done
   }           //All done 
} 

void realft(float *data, int n, int isign) {
/*-------------------------------------------------------------------------------------------
 USES four1
 Calculates the Fourier transform of a set of n real-valued data points. Replaces this data 
 (which is stored in array data(1:n)) by the positive frequency half of its complex Fourier 
 transform. The real-valued first and last components of the complex transform are returned 
 as elements data(1) and data(2), respectively. n must be a power of 2. This routine 
 also calculates the inverse transform of a complex data array if it is the transform of real 
 data. (Result in this case must be multiplied by 2/n.) 
-------------------------------------------------------------------------------------------*/
  int    i,i1,i2,i3,i4,n2p3; 
  float  c1,c2,h1i,h1r,h2i,h2r,wis,wrs;
  double theta,wi,wpi,wpr,wr,wtemp;    //Double precision for the trigonometric recurrences
  double PI=4.0*atan(1.0);   
  
  theta=PI/(n/2);                      //Initialize the recurrence
  c1=0.5;
  if (isign == 1) {
    c2=-0.5;
    four1(data,n/2,1);                 //The forward transform is here
  }   
  else {
    c2=0.5;                            //Otherwise set up for an inverse transform
    theta=-theta;
  }
  wpr=-2.0*sin(0.5*theta)*sin(0.5*theta);
  wpi=sin(theta);
  wr=1.0+wpr;
  wi=wpi;
  n2p3=n+3;
  for (i=2; i<= n/4; i++) {            //Case i=1 done separately below
    i1=2*i-1;
    i2=i1+1;
    i3=n2p3-i2;
    i4=i3+1;
    wrs=wr;
    wis=wi;
    h1r=c1*(data[i1]+data[i3]);    //The two separate transforms are separated out of data
    h1i=c1*(data[i2]-data[i4]);
    h2r=-c2*(data[i2]+data[i4]);
    h2i=c2*(data[i1]-data[i3]);
    data[i1]=h1r+wrs*h2r-wis*h2i;    //Here they are recombined to form the true transform
                                      //of the original real data
    data[i2]=h1i+wrs*h2i+wis*h2r;
    data[i3]=h1r-wrs*h2r+wis*h2i;
    data[i4]=-h1i+wrs*h2i+wis*h2r;
    wtemp=wr;                         //The recurrence
    wr=wr*wpr-wi*wpi+wr;
    wi=wi*wpr+wtemp*wpi+wi;
  }

  if (isign == 1) {
    h1r=data[1];
    data[1]=h1r+data[2];
    data[2]=h1r-data[2];         //Squeeze the first and last data together to get
  }                              //them all within the original array
  else {
    h1r=data[1];
    data[1]=c1*(h1r+data[2]);
    data[2]=c1*(h1r-data[2]);
    four1(data,n/2,-1);          //This is the inverse transform for the case isign-1
  } 
} 


void main()  {

  //open input and output files
  fp_in = fopen("smooth.dat","r");
  fp_out = fopen("smooth.lst","w");

  //read number of input signal points in input file
  fscanf(fp_in, "%d", &ndata);

  //take nearest power of two
  if (ndata > 2048) ndata=2048;
  if (ndata<2048 && ndata>1023) ndata=1024;
  if (ndata<1024 && ndata>511) ndata=512;
  if (ndata<512 && ndata>255) ndata=256;
  if (ndata<256 && ndata>127) ndata=128;
  if (ndata<128 && ndata>63) ndata=64;
  if (ndata<64 && ndata>31) ndata=32;
  if  (ndata<32) {
    fprintf(fp_out," Error: number of points too small (<32) !\n");
    fclose(fp_out);
    printf(" Results in file smooth.lst (error).\n");
    return;
  }

  //read ndata couples T(i), Y(i) in input data file.
  for (i=1; i<=ndata; i++) {
    fscanf(fp_in, "%f %f", &temp, &signal[i]);
    if (i == 1) tbegin=temp;
    if (i == ndata) tend=temp;
  }
  fclose(fp_in);

  for (i=1; i<=ndata; i++) ysave[i]=signal[i];

  SMOOFT(signal,ndata,30);

  dt=(tend-tbegin)/(ndata-1);
  t=tbegin-dt;
  fprintf(fp_out,"      Time          Y        Smoothed Y \n");
  fprintf(fp_out,"----------------------------------------\n");
  for (i=1; i<=ndata; i++) {
    t += dt;
    fprintf(fp_out,"   %10.6f   %10.6f   %10.6f\n", t, ysave[i], signal[i]);
  }

  fclose(fp_out);
  printf("\n Results in file smooth.lst.\n\n");
 
}

// end of file smooth.cpp

