#ifndef __ECH_POWER_CTRL_H
#define __ECH_POWER_CTRL_H

#include "kutil.h"   
#include <stdio.h>
#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

#include "kutilObj.h"
#include "sfwCommon.h"
#include "RfmHandler.h"
#include "kupaLib.h"


#ifdef __cplusplus
extern "C" {
#endif

// --------------------
// RFM Offset
// --------------------
//
#define    RFM_PCS_SHOT_NO           0x04000030        // PCS Shot Number
#define    RFM_PCS_CURR_TIME         0x03800004        // PCS Current Time
#define    RFM_PSI_VALUE_AXIS        0x4032E58
#define    RFM_PSI_VALUE_BDRY        0x4032E5C
#define    RFM_PSI_VALUE_FIRST       0x4032E60     

#define    RFM_FORCE_FAST_INTERLOCK  0x3800010         // If 0, PCS and CCS Fault Interlock Signal     

#define    ECH_PWR_CTRL_PVNAME_ORG   "ECH_AO_OUTPUT_VALUE_PWR_CH0"
#define    ECH_PWR_CTRL_PVNAME       "HICS_ECH_POWER_CONTROL"

extern void ech_power_ctrl (void *param);

#ifdef __cplusplus
}
#endif

#endif /* __ECH_POWER_CTRL_H */

/*
// operating parameters
#define    TORMAG                   2.0  // HICS_ECH_PWR_CTRL_TORMAG
#define    DTUPDCTRLTIME            0.5  // HICS_ECH_PWR_CTRL_DTTIME

#define    LASTUPDATE               3.0  // modification required...

// target setting
#define    TARGMANUALSET            0    // HICS_ECH_PWR_CTRL_TARGMANUAL
#define    TARGWEIGHT               1.0  // HICS_ECH_PWR_CTRL_TARGTEGAIN

#define    TARGPOS1                 0.05 // HICS_ECH_PWR_CTRL_TARGPOS1       // unit [rho_psi] 
#define    TARGPOS2                 0.15 // HICS_ECH_PWR_CTRL_TARGPOS2
#define    TARGPOS3                 0.25 // HICS_ECH_PWR_CTRL_TARGPOS3
#define    TARGPOS4                 0.35 // HICS_ECH_PWR_CTRL_TARGPOS4
#define    TARGPOS5                 0.60 // HICS_ECH_PWR_CTRL_TARGPOS5

#define    TARGVAL1                 2.70 // HICS_ECH_PWR_CTRL_TARGVAL1       // unit [keV]
#define    TARGVAL2                 2.40 // HICS_ECH_PWR_CTRL_TARGVAL2
#define    TARGVAL3                 1.90 // HICS_ECH_PWR_CTRL_TARGVAL3
#define    TARGVAL4                 1.40 // HICS_ECH_PWR_CTRL_TARGVAL4
#define    TARGVAL5                 0.60 // HICS_ECH_PWR_CTRL_TARGVAL5


// constant coeffs. for prescribed profiles
#define    FLATXEON                 1    // HICS_ECH_PWR_CTRL_FLATXEON
#define    FLATXEVAL                0.6  // HICS_ECH_PWR_CTRL_FLATXEVAL
#define    XECOEFA                  0.4  // HICS_ECH_PWR_CTRL_XECOEFA
#define    XECOEFB                  1.0  // HICS_ECH_PWR_CTRL_XECOEFB
#define    XECOEFC                  6.0  // HICS_ECH_PWR_CTRL_XECOEFC
#define    XECOEFD                  80.0 // HICS_ECH_PWR_CTRL_XECOEFD

#define    FLATNEVAL                2.0  // HICS_ECH_PWR_CTRL_FLATNEVAL     // line aveaged electron density [1E19 m-3]


// assumed  ECH deposition profile, Gaussian shape
#define    ECHPOS                   0.15 // HICS_ECH_PWR_CTRL_ECHPOS
*/

// contants
#define    C                        299792458.0                     // defining physics parameters
#define    H                        6.6261E-34
#define    E                        1.6022E-19
#define    MU0                      1.2566E-06
#define    EPSI0                    8.8542E-12
#define    G                        6.6730E-11
#define    K                        1.3807E-23
#define    ALPHA                    0.0073
#define    ME                       9.1094E-31
#define    MP                       1.6726E-27
#define    UA                       1.6605E-27
#define    AVO                      6.0221E+23
#define    SIGMA                    5.6704E-08


// arrays
#define    NUM                      101                           // the number of radial point
#define    CNUM                     5
#define    NPSI                     33
#define    CHAN                     40

// for Debug mode (Test mode)
#define    DEBUG                    1


// macro functions
#define    SIGN(a)                  (((a) > 0.0) ? (1.0) : (-1.0))
#define    SIGNSVD(a,b)             ((b) >= 0.0 ? fabs(a) : -fabs(a))
#define    SQR(a)                   ((a)*(a))
#define    SQR4(a)                  ((a)*(a)*(a)*(a))
#define    FMAX(a,b)                (((a) > (b)) ? (a) : (b))
#define    FMIN(a,b)                (((a) < (b)) ? (a) : (b))
#define    IMIN(a,b)                (((a) < (b)) ? (a) : (b))


// for SVD, from numerical recipes
#define    NR_END                   1
#define    FREE_ARG                 char*


enum onoff {ON=1,OFF=0}; 


//------------------------ structure elements ---------------------------//

struct data_prof {
    float xa[NUM];     
    float ne[NUM];     
    float heat_xe[NUM];
    float te[CHAN];
    float pos[CHAN];
    float npos[CHAN];
};

struct param_mimo {         
    float actuator;                         // turn on/off sign of actuator
    float interval[2];                      // the time for applying the actuator (start-end)
    float maxpower;                         // allowable maxpower of actuator
    float minpower;
    float weight;                           // weight between actuators..
}; 

struct param_targ {         
    float cont;                             // on/off sign of controlling Te 
    float rho[5];                           // dedicated position of Te profile
    float val[5];                           // dedicated target value of Te
    float weight;                           // weight between Te and q control
};

struct param_equi {
    float vpr[NUM];
    float grho2[NUM];
    float rhomax;
    float vpsiaxis,vpsibdry;
    float vpsi[NPSI];
    float lpsi[NPSI];
    float npsi[NPSI];
    int idxa,idxb;
};

struct contr_coef {
    float coef1[NUM];
    float coef2[NUM];
    float coef3[NUM];
    float coef_fce[NUM];  
};

struct param_act {
    int id;
    float prepower;
    float el_coef[CNUM];
    float weight;
    float maxpower;
    float minpower;
};

struct curr_stat {
    float rho[CNUM];
    float val[CNUM];
    float err[CNUM];
    int   npt;
    float weight;
};


//------------------------ declaration of fucntion and structure ---------------------------//
float *vector(long nl, long nh);
float **matrix(long nrl, long nrh, long ncl, long nch);
void free_vector(float *v, long nl, long nh);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);

float trapz(float* xa,float* ya,int n);
float lint(float* xa,float* ya,int n,float x);
float smooth(float* xa,float* ya,int n,int tn);
float round(float x,int pos);
void  svbksb(float **u,float w[],float **v,int m,int n,float b[],float x[]);
void  svdcmp(float **a,int m,int n,float w[],float **v);
float pythag(float a,float b);

struct param_mimo  setvar_fce(void);       // declaration of param_mimo structure 
struct param_targ  setvar_targ(void);      // declaration of param_targ structure
struct param_equi  setvar_equi(void);
struct data_prof   setdis_prof(struct param_equi);
struct contr_coef  el_coef_fec(struct param_equi,struct data_prof);
struct param_act   actuator_fce(struct param_mimo,struct param_targ,struct contr_coef,struct data_prof);
struct curr_stat   setvar_curr(struct param_targ,struct data_prof);


//------------------------ definition of fucntion and structure ---------------------------//
float *vector(long nl, long nh)
/* allocate a float vector with subscript range v[nl..nh] */
{
    float *v;
    
    v=(float *)malloc((size_t) ((nh-nl+1+NR_END)*sizeof(float)));
    if (!v) printf("allocation failure in vector()");
    return v-nl+NR_END;
}
void free_vector(float *v, long nl, long nh)
/* free a float vector allocated with vector() */
{
    free((FREE_ARG) (v+nl-NR_END));
}
float **matrix(long nrl, long nrh, long ncl, long nch)
/* allocate a float matrix with subscript range m[nrl..nrh][ncl..nch] */
{
    long i, nrow=nrh-nrl+1,ncol=nch-ncl+1;
    float **m;

    /* allocate pointers to rows */
    m=(float **) malloc((size_t)((nrow+NR_END)*sizeof(float*)));
    if (!m) printf("allocation failure 1 in matrix()");
    m += NR_END;
    m -= nrl;

    /* allocate rows and set pointers to them */
    m[nrl]=(float *) malloc((size_t)((nrow*ncol+NR_END)*sizeof(float)));
    if (!m[nrl]) printf("allocation failure 2 in matrix()");
    m[nrl] += NR_END;
    m[nrl] -= ncl;

    for(i=nrl+1;i<=nrh;i++) m[i]=m[i-1]+ncol;

    /* return pointer to array of pointers to rows */
    return m;
}

void free_matrix(float **m, long nrl, long nrh, long ncl, long nch)
/* free a float matrix allocated by matrix() */
{
    free((FREE_ARG) (m[nrl]+ncl-NR_END));
    free((FREE_ARG) (m+nrl-NR_END));
}

float trapz(float* xa,float* ya,int n)
{
    int i;
    float y=0.0;
    if(n!=0){
        for(i=0;i<n;i++) y+=0.5*(ya[i]+ya[i+1])*fabs(xa[i]-xa[i+1]);
    }
    
    return y;
}

float lint(float* xa,float* ya,int n,float x)
{
    int i,ar;
    float diff,diff_temp;
    float mm,nn,y;
    
    //float trunc=1E-5;
    
	//TODO
	ar=0;
    diff=fabs(x-xa[0]);
    for(i=1;i<n;i++){
        diff_temp=fabs(x-xa[i]);
        if(diff_temp<diff){
            diff=diff_temp;
            ar=i;
        }
    }

    if(x<xa[ar]){
        mm=fabs(x-xa[ar-1]);
        nn=fabs(xa[ar]-x);
        y=(nn/(mm+nn))*ya[ar-1]+(mm/(mm+nn))*ya[ar];          // linear interpolation
    }
    else{
        mm=fabs(x-xa[ar]);
        nn=fabs(xa[ar+1]-x);
        y=(nn/(mm+nn))*ya[ar]+(mm/(mm+nn))*ya[ar+1];          // linear interpolation
    }

    return y;      
}

float round(float x,int pos)
{ 
    return pow(10.0,+pos)*floor(x*pow(10.0,-pos)+0.5);
}

float smooth(float* xa,float* ya,int n, int tn){
/* It has to be modified by using the mothod, Monotome Hermite interpolation*/
    float y;
    if(n==0 || n==tn){
        y=ya[0];
    }
    else if(n==1 || n==tn-1){
        y=(ya[n-1]+ya[n]+ya[n+1])/3.0;
    }
    else{
        y=(ya[n-2]+ya[n-1]+ya[n]+ya[n+1]+ya[n+2])/5.0;
    }

    return y;
}

void svbksb(float **u,float w[],float **v,int m,int n,float b[],float x[])
/* Solves A.X = B for a vector X, where A is specified by the arrays u[1..m][1..n], w[1..n],
   v[1..n][1..n] as returned by svdcmp. m and n are the dimensions of a, and will be equal for
   square matrices. b[1..m] is the input right-hand side. x[1..n] is the output solution vector.
   No input quantities are destroyed, so the routine may be called sequentially with different b's. */
{
    int   jj,j,i;
    float s;
    //float tmp[n];
    float *tmp;

    tmp=vector(1,n);
    
    for(j=1;j<=n;j++){
        s=0.0;
        if(w[j]){
            for(i=1;i<=m;i++) s+=u[i][j]*b[i];
            s/=w[j];
        }
        tmp[j]=s;
    }
    for(j=1;j<=n;j++){
        s=0.0;
        for(jj=1;jj<=n;jj++) s+=v[j][jj]*tmp[jj];
        x[j]=s;
    }
    free_vector(tmp,1,n);
}

void svdcmp(float **a,int m,int n,float w[],float **v)
/* Given a matrix a[1..m][1..n], this routine computes its singular value decomposition, A =
   U.W.V_T. The matrix U replaces a on output. The diagonal matrix of singular values W is output
   as a vector w[1..n]. The matrix V (not the transpose V T ) is output as v[1..n][1..n]. */
{
    int flag,i,its,j,jj,k,l,nm;
    float anorm,c,f,g,h,s,scale,x,y,z;
    float *rv1;

    rv1=vector(1,n);

	j=0; l=0;
    
    g=scale=anorm=0.0;
    for(i=1;i<=n;i++){
        l=i+1;
        rv1[i]=scale*g;
        g=s=scale=0.0;
        if(i<=m){
            for(k=1;k<=m;k++) scale+=fabs(a[k][i]);
            if(scale){
                for(k=i;k<=m;k++){
                    a[k][i]/=scale;
                    s+=a[k][i]*a[k][i];
                }
                f=a[i][i];
                g=-SIGNSVD(sqrt(s),f);
                h=f*g-s;
                a[i][i]=f-g;
                for(j=l;j<=n;j++){
                    for(s=0.0,k=1;k<=m;k++) s+=a[k][i]*a[k][j];
                    f=s/h;
                    for(k=i;k<=m;k++) a[k][j]+=f*a[k][i];
                }
                for(k=i;k<=m;k++) a[k][i]*=scale;
            }
        }
        w[i]=scale*g;
        g=s=scale=0.0;
        if(i<=m && i != n){
            for(k=1;k<=n;k++) scale+=fabs(a[i][k]);
            if(scale){
                for(k=l;k<=n;k++){
                    a[i][k]/=scale;
                    s+=a[i][k]*a[i][k];
                }
                f=a[i][l];
                g=-SIGNSVD(sqrt(s),f);
                h=f*g-s;
                a[i][l]=f-g;
                for(k=l;k<=n;k++) rv1[k]=a[j][k]/h;
                for(j=l;j<=m;j++){
                    for(s=0.0,k=l;k<=n;k++) s+=a[j][k]*a[i][k];
                    for(k=l;k<=n;k++) a[j][k]+=s*rv1[k];
                }
                for(k=l;k<=n;k++) a[i][k]*=scale;
            }
        }
        anorm=FMAX(anorm,(fabs(w[i])+fabs(rv1[i])));
    }
    for(i=n;i>=1;i--){
        if(i<n){
            if(g){
                for(j=l;j<=n;j++)
                  v[j][i]=(a[i][j]/a[i][l])/g;
                for(j=l;j<=n;j++){
                    for(s=0.0,k=l;k<=n;k++) s+=a[i][k]*v[k][j];
                    for(k=l;k<=n;k++) v[k][j]+=s*v[k][i];
                }
            }
            for(j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
        }
        v[i][i]=1.0;
        g=rv1[i];
        l=i;
    }
    for(i=IMIN(m,n);i>=1;i--){
        l=i+1;
        g=w[i];
        for(j=l;j<=n;j++) a[i][j]=0.0;
        if(g){
            g=1.0/g;
            for(j=l;j<=n;j++){
                for(s=0.0,k=l;k<=m;k++) s+=a[k][i]*a[k][j];
                f=(s/a[i][i])*g;
                for(k=i;k<=m;k++) a[k][j]+=f*a[k][i];
            }
            for(j=i;j<=m;j++) a[j][i]*=g;
        }else for(j=i;j<=m;j++) a[j][i]=0.0;
        ++a[i][i];
    }
    for(k=n;k>=1;k--){
        for(its=1;its<=30;its++){
            flag=1;
            for(l=k;l>=1;l--){
                nm=l-1;
                if((float)(fabs(rv1[l])+anorm)==anorm){
                flag=0;
                break;
              }
                if((float)(fabs(w[nm])+anorm)==anorm) break;
            }
          if(flag){
              c=0.0;
              s=1.0;
              for(i=l;i<=k;i++){
                  f=s*rv1[i];
                  rv1[i]=c*rv1[i];
                  if((float)(fabs(f)+anorm)==anorm) break;
                  g=w[i];
                  h=pythag(f,g);
                  w[i]=h;
                  h=1.0/h;
                  c=g*h;
                  s=-f*h;
                  for(j=1;j<=m;j++){
                      y=a[j][nm];
                      z=a[j][i];
                      a[j][nm]=y*c+z*s;
                      a[j][i]=z*c-y*s;
                  }
              }
          }
          z=w[k];
          if(l==k){
              if(z<0.0){
                  w[k]=-z;
                  for(j=1;j<=n;j++) v[j][k]=-v[j][k];
              }
              break;
          }
        if(its==30) printf("no convergence in 30 svdcmp iterations");
          x=w[l];
          nm=k-1;
          y=w[nm];
          g=rv1[nm];
          h=rv1[k];
          f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
          g=pythag(f,1.0);
          f=((x-z)*(x+z)+h*((y/(f+SIGNSVD(g,f)))-h))/x;
          c=s=1.0;
          for(j=l;j<=nm;j++){
              i=j+1;
              g=rv1[i];
              y=w[i];
              h=s*g;
              g=c*g;
              z=pythag(f,h);
              rv1[j]=z;
              c=f/z;
              s=h/z;
              f=x*c+g*s;
              g=g*c-x*s;
              h=y*s;
              y*=c;
              for(jj=1;jj<=n;jj++){
                  x=v[jj][j];
                  z=v[jj][i];
                  v[jj][j]=x*c+z*s;
                v[jj][i]=z*c-x*s;
              }
              z=pythag(f,h);
              w[j]=z;
              if(z){
                  z=1.0/z;
                  c=f*z;
                  s=h*z;
              }
              f=c*g+s*y;
              x=c*y-s*g;
              for(jj=1;jj<=m;jj++){
                  y=a[jj][j];
                  z=a[jj][i];
                  a[jj][j]=y*c+z*s;
                  a[jj][i]=z*c-y*s;
              }
          }
          rv1[l]=0.0;
          rv1[k]=f;
          w[k]=x;
      }
  }
  free_vector(rv1,1,n);
}

float pythag(float a,float b)
{
    float absa,absb;
    absa=fabs(a);
    absb=fabs(b);
    if(absa>absb) return absa*sqrt(1.0+SQR(absb/absa));
    else return (absb==0.0 ? 0.0:absb*sqrt(1.0+SQR(absa/absb)));
}

struct data_prof setdis_prof(struct param_equi equi)
{
    struct data_prof prof;
    
    int i,ii,j;
    int loadingcnt;
    int cmparr;
    int eceFreq[CHAN]={0,};

    float eceVal[10][CHAN];

    char  ecePVname[20]="";

    float loadEceValue[CHAN]={0,};
    float eceData[CHAN]     ={0,};
    float ecePos[CHAN]      ={0,};
    float smoothEceData[CHAN]={0,};
    
    int   minNormArr,maxNormArr;
    int   midArr;
    float minNormPos;
    float updateNormPos[CHAN]={0,};
    float updateEceData[CHAN]={0,};

    float xa4[NUM]={0,};

    int nPcsCurrTime;
    float nPcsCurrTimeUnitSec;
	float timeCheck;
	
	float TORMAG, DTUPDCTRLTIME;
    float FLATXEON,FLATXEVAL;
	float XECOEFA, XECOEFB, XECOEFC, XECOEFD;
	float FLATNEVAL;

    char tormag[40], dtupdctrltime[40];
	char flatxeon[40], flatxeval[40];
	char xecoefa[40], xecoefb[40], xecoefc[40], xecoefd[40];
	char flatneval[40];


    //operating parameters 
	sprintf(tormag       ,"HICS_ECH_PWR_CTRL_TORMAG"    );
    TORMAG           =   kupaGetValue((char *)tormag);
    sprintf(dtupdctrltime,"HICS_ECH_PWR_CTRL_DTTIME"    );
    DTUPDCTRLTIME    =   kupaGetValue((char *)dtupdctrltime);


    //constant coeffs. for prescribed profiles 
 	sprintf(flatxeon     ,"HICS_ECH_PWR_CTRL_FLATXEON"  );
    FLATXEON         =   kupaGetValue((char *)flatxeon);
    sprintf(flatxeval    ,"HICS_ECH_PWR_CTRL_FLATXEVAL" );
    FLATXEVAL        =   kupaGetValue((char *)flatxeval);
 	sprintf(xecoefa      ,"HICS_ECH_PWR_CTRL_XECOEFA"   );
    XECOEFA          =   kupaGetValue((char *)xecoefa);
 	sprintf(xecoefb      ,"HICS_ECH_PWR_CTRL_XECOEFB"   );
    XECOEFB          =   kupaGetValue((char *)xecoefb);
 	sprintf(xecoefc      ,"HICS_ECH_PWR_CTRL_XECOEFC"   );
    XECOEFC          =   kupaGetValue((char *)xecoefc);
 	sprintf(xecoefd      ,"HICS_ECH_PWR_CTRL_XECOEFD"   );
    XECOEFD          =   kupaGetValue((char *)xecoefd);
	sprintf(flatneval    ,"HICS_ECH_PWR_CTRL_FLATNEVAL" );
    FLATNEVAL        =   kupaGetValue((char *)flatneval);     // line aveaged electron density [1E19 m-3]

    //interlock (safety) ...other issue is if network dies?
	if (TORMAG==0)    TORMAG=2.0;
	if (DTUPDCTRLTIME==0) DTUPDCTRLTIME=0.5;
	if (FLATXEON==0)  FLATXEON=0.0;
	if (FLATXEVAL==0) FLATXEVAL=1.0;
	if (XECOEFA==0)   XECOEFA=1.0;
	if (XECOEFB==0)   XECOEFB=3.0;
	if (XECOEFC==0)   XECOEFC=6.0;
	if (XECOEFD==0)   XECOEFD=9.0;
	if (FLATNEVAL==0) FLATNEVAL=2.5;
           

    /* 2nd harmonics, calculation of real position (m) of ECE chs.
    Equation: R=0.0100352*I_TF./freq
    freq.(kHz) data of ECE01-
    ECE01 ECE02 ECE03 ECE04 ECE05 ECE06 ECE07 ECE08 ECE09 ECE10
    110   111   112   113   114   115   116   117   118   119
    ECE11 ECE12 ECE13 ECE14 ECE15 ECE16 ECE17 ECE18 ECE19 ECE20
    120   121   123   124   125   126   127   128   129   130
    ECE21 ECE22 ECE23 ECE24 ECE25 ECE26 ECE27 ECE28 ECE29 ECE30
    131   132   133   134   138   139   140   141   142   143
    ECE31 ECE32 ECE33 ECE34 ECE35 ECE36 ECE37 ECE38 ECE39 ECE40
    144   145   146   147   148   149   151   152   153   154
    ECE41 ECE42 ECE43 ECE44 ECE45 ECE46 ECE47 ECE48 ECE49 ECE50
    155   156   157   158   159   160   161   162   164   165
    ECE51 ECE52 ECE53 ECE54 ECE55 
    167   168   169   170   171   
    */
    // ECE ch. freq. (kHz)
    eceFreq[0]=110;  eceFreq[1]=111;  eceFreq[2]=112;  eceFreq[3]=113;  eceFreq[4]=114; 
    eceFreq[5]=115;  eceFreq[6]=116;  eceFreq[7]=117;  eceFreq[8]=118;  eceFreq[9]=119; 
    eceFreq[10]=120; eceFreq[11]=121; eceFreq[12]=123; eceFreq[13]=124; eceFreq[14]=125;
    eceFreq[15]=126; eceFreq[16]=127; eceFreq[17]=128; eceFreq[18]=129; eceFreq[19]=130;
    eceFreq[20]=131; eceFreq[21]=132; eceFreq[22]=133; eceFreq[23]=134; eceFreq[24]=138;
    eceFreq[25]=139; eceFreq[26]=140; eceFreq[27]=141; eceFreq[28]=142; eceFreq[29]=143;
    eceFreq[30]=144; eceFreq[31]=145; eceFreq[32]=146; eceFreq[33]=147; eceFreq[34]=148;
    eceFreq[35]=149; eceFreq[36]=151; eceFreq[37]=152; eceFreq[38]=153; eceFreq[39]=154;

    // ECE Position (m)
    for (i=0;i<CHAN;i++) ecePos[i]=0.0100352*TORMAG*1E4/eceFreq[i];


    if  (DEBUG) {
        // #7072 base ECE calibrated (ev)

        eceData[0]=0.00000;    eceData[1]=0.00000;    eceData[2]=1512.61465; eceData[3]=1452.13735; eceData[4]=1403.89301; 
        eceData[5]=1383.13317; eceData[6]=1324.76173; eceData[7]=982.65927;  eceData[8]=879.51440;  eceData[9]=971.59833; 
        eceData[10]=851.05031; eceData[11]=0.00000;   eceData[12]=689.94980; eceData[13]=515.99416; eceData[14]=677.04778; 
        eceData[15]=711.80759; eceData[16]=570.07300; eceData[17]=682.43757; eceData[18]=638.82386; eceData[19]=612.06314; 
        eceData[20]=574.68920; eceData[21]=546.47201; eceData[22]=530.14344; eceData[23]=584.99068; eceData[24]=634.83987; 
        eceData[25]=605.30840; eceData[26]=590.14762; eceData[27]=0.00000;   eceData[28]=0.00000;   eceData[29]=548.45605; 
        eceData[30]=551.57241; eceData[31]=551.57241; eceData[32]=551.57241; eceData[33]=551.57241; eceData[34]=551.57241; 
        eceData[35]=551.57241; eceData[36]=551.57241; eceData[37]=551.57241; eceData[38]=551.57241; eceData[39]=551.57241; 
    
        //for(i=0;i<NUM;i++) prof.te[i]=(2.0-0.2)*pow(1-pow(prof.xa[i],2.0),2.0)+0.2;            // Te profile, it has to be from rt-ECE data
    }
    else {
        //EPICS CA loading ECE data

        //initialization to zero
        for (i=0;i<10;i++) {
            for (j=0;j<CHAN;j++) eceVal[i][j]=0.0;
        }

        //loading ECE signals
        loadingcnt=0;
        for (i=0;i<CHAN;i++) {
            sprintf(ecePVname,"ECE%02d_PV_NAME",i+1);
            loadEceValue[i] = kupaGetValue ((char *)ecePVname);
        }
        for (i=0;i<CHAN;i++) eceVal[loadingcnt][i]=loadEceValue[i];
        loadingcnt++;

        while (loadingcnt<=10) {
            for (i=0;i<CHAN;i++) {
                sprintf(ecePVname,"ECE%02d_PV_NAME",i+1);
                loadEceValue[i] = kupaGetValue ((char *)ecePVname);
            }
            cmparr=loadingcnt-1;
            if  (eceVal[cmparr][5]!=loadEceValue[5]) {       // comparing data betwwen previous ECE05 and present ECE05
                for (i=0;i<CHAN;i++) eceVal[loadingcnt][i]=loadEceValue[i];
                loadingcnt++;
            }
            if  (kuOK != kstar_rfm_read (&nPcsCurrTime, RFM_PCS_CURR_TIME, sizeof(nPcsCurrTime))) {
                kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
                continue;
            }
            kuDebug (kuMON, "[ech_power_ctrl] nPcsCurrTime(%d)\n", nPcsCurrTime);
            nPcsCurrTimeUnitSec=((float)(nPcsCurrTime)/(float)1E6)-15.0;    // PCS start is at -15.0 sec

            timeCheck=nPcsCurrTimeUnitSec-floor(nPcsCurrTimeUnitSec/DTUPDCTRLTIME)*DTUPDCTRLTIME;
            if((timeCheck)>0.300)
                break;
        }

        // warning!! calibriation required ...


        // time averaging ...
        for (j=0;j<CHAN;j++) {
            for (i=1;i<loadingcnt+1;i++) {
                eceData[j]=eceData[j]+eceVal[i][j];
            }
            eceData[j]=eceData[j]/(float)(loadingcnt-1);
        }
    }

    
    for (i=0;i<CHAN;i++){
        prof.te[i] =eceData[i];
        prof.pos[i]=ecePos[i];
    }
    for (i=0;i<CHAN;i++) prof.npos[i]=lint(equi.lpsi,equi.npsi,NPSI,prof.pos[i]);
    
    //find the min. noramlized position and its array number
    minNormPos=prof.npos[0];
    minNormArr=0;
    for (i=0;i<CHAN;i++) {
        if  (minNormPos>prof.npos[i]) {
            minNormArr=i;
            minNormPos=prof.npos[i];
        }
    }

    //re-positioned the arry
    midArr=round(CHAN/2.0,0);
    if  (minNormArr>midArr) {
        ii=0;
        for (i=minNormArr;i>=0;i--) {
            if  ((prof.npos[i]<1.0) & (prof.te[i]!=0.0)) {
                updateNormPos[ii]=prof.npos[i];
                updateEceData[ii]=prof.te[i];
                ii++;
            }
        }
        maxNormArr=ii;
    }
    else {
        ii=0;
        for (i=minNormArr;i<CHAN;i++) {
            if  ((prof.npos[i]<1.0) & (prof.te[i]!=0.0)) {
                updateNormPos[ii]=prof.npos[i];
                updateEceData[ii]=prof.te[i];
                ii++;
            }
        }
        maxNormArr=ii;
    }

    // now profile smoothing progresses...
    for (j=0;j<2;j++) {                                    // 5 times smoothing proceeded ...
        for (i=0;i<maxNormArr;i++) {
            smoothEceData[i]=smooth(updateNormPos,updateEceData,i,maxNormArr);
        }
    }

    prof.xa[0]=0.0;
    prof.xa[NUM-1]=1.0;
    for (i=1;i<NUM-50;i++) {
        prof.xa[i]=prof.xa[i-1]+0.01;                           // normalized psi, radial grid points
        prof.xa[NUM-1-i]=prof.xa[NUM-1]-0.01*i;
    }

    // ne profile at each raidal grid point [1E19 m-3]
    for (i=0;i<NUM;i++) prof.ne[i]=FLATNEVAL;

    // heat xe profile at each raidial uniformed grid point [m2/s]
    if  (FLATXEON) {
        for (i=0;i<NUM;i++) prof.heat_xe[i]=FLATXEVAL;          
    }
    else {
        // xe = a (b + c*xa**2 + d*xa**20)
        for (i=0;i<NUM;i++) xa4[i]=SQR4(prof.xa[i]);
        for (i=0;i<NUM;i++) prof.heat_xe[i]=XECOEFA*(XECOEFB+XECOEFC*SQR(prof.xa[i])+XECOEFD*SQR4(xa4[i])*SQR4(prof.xa[i]));
    }

    return prof;
}
    

struct param_mimo setvar_fce(void)          // the func. for setting basic param. of fce
{
    struct param_mimo fce;

	int nPcsCurrTime;
	float nPcsCurrTimeUnitSec;
	
	char echInjectStartTimePVname[20]="";
	char echInjectEndTimePVname[20]="";
	float startCtrlTime;
	float endCtrlTime;

    if (kuOK != kstar_rfm_read (&nPcsCurrTime, RFM_PCS_CURR_TIME, sizeof(nPcsCurrTime))) {
        kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
        //continue;
    }
    kuDebug (kuTRACE, "[ech_power_ctrl] nPcsCurrTime(%d)\n", nPcsCurrTime);
    nPcsCurrTimeUnitSec=(float)(nPcsCurrTime)/(float)1E6;

	/* PCSRT5-ECHSEVER  */
    sprintf(echInjectStartTimePVname,"ECH_LTU_P1_SEC0_T0");
    startCtrlTime = kupaGetValue ((char *)echInjectStartTimePVname);
   
    sprintf(echInjectEndTimePVname,"ECH_LTU_P1_SEC0_T1");
    endCtrlTime = kupaGetValue ((char *)echInjectEndTimePVname);


    fce.actuator=ON;                        // actuator turn on(1) or off(0)    
//    fce.interval[0]=STARTCTRLTIME;          // start time for applying the actuator fce
//    fce.interval[1]=ENDCTRLTIME;            // end time for applying the actuator fce
    fce.interval[0]=startCtrlTime;
	fce.interval[1]=endCtrlTime;
    fce.maxpower=0.3*1e6;                   // uppper limit of actuator power (W)
    fce.minpower=0.0*1e6;                   // lower limit of actuator power (W)
    fce.weight=1.0;

    return fce;
}

struct param_targ setvar_targ(void)       // the func. for setting the target value of Te
{
    struct param_targ targ;
    

    float TARGMANUALSET;
	float TARGWEIGHT;
	float TARGPOS1, TARGPOS2, TARGPOS3, TARGPOS4, TARGPOS5;
	float TARGVAL1, TARGVAL2, TARGVAL3, TARGVAL4, TARGVAL5;

	char targmanualset[40], targweight[40];
	char targpos1[40], targpos2[40], targpos3[40], targpos4[40], targpos5[40];
	char targval1[40], targval2[40], targval3[40], targval4[40], targval5[40];

    //target setting 
 	sprintf(targmanualset,"HICS_ECH_PWR_CTRL_TARGMANUAL");
    TARGMANUALSET    =   kupaGetValue((char *)targmanualset);
    sprintf(targweight   ,"HICS_ECH_PWR_CTRL_TARGTEGAIN");
    TARGWEIGHT       =   kupaGetValue((char *)targweight);

	sprintf(targpos1     ,"HICS_ECH_PWR_CTRL_TARGPOS1"  );
    TARGPOS1         =   kupaGetValue((char *)targpos1);       // unit [rho_psi] 
 	sprintf(targpos2     ,"HICS_ECH_PWR_CTRL_TARGPOS2"  );
    TARGPOS2         =   kupaGetValue((char *)targpos2);
 	sprintf(targpos3     ,"HICS_ECH_PWR_CTRL_TARGPOS3"  );
    TARGPOS3         =   kupaGetValue((char *)targpos3);
 	sprintf(targpos4     ,"HICS_ECH_PWR_CTRL_TARGPOS4"  );
    TARGPOS4         =   kupaGetValue((char *)targpos4);
 	sprintf(targpos5     ,"HICS_ECH_PWR_CTRL_TARGPOS5"  );
    TARGPOS5         =   kupaGetValue((char *)targpos5);

    sprintf(targval1     ,"HICS_ECH_PWR_CTRL_TARGVAL1"  );
    TARGVAL1         =   kupaGetValue((char *)targval1);       // unit [keV]
    sprintf(targval2     ,"HICS_ECH_PWR_CTRL_TARGVAL2"  );
    TARGVAL2         =   kupaGetValue((char *)targval2);
    sprintf(targval3     ,"HICS_ECH_PWR_CTRL_TARGVAL3"  );
    TARGVAL3         =   kupaGetValue((char *)targval3);
    sprintf(targval4     ,"HICS_ECH_PWR_CTRL_TARGVAL4"  );
    TARGVAL4         =   kupaGetValue((char *)targval4);
    sprintf(targval5     ,"HICS_ECH_PWR_CTRL_TARGVAL5"  );
    TARGVAL5         =   kupaGetValue((char *)targval5);

    // interlock 
    if (TARGVAL1==0) {
		TARGMANUALSET=1.0;
		TARGWEIGHT=2.0;
	}


    targ.cont=ON;                           // 

    if  (TARGMANUALSET) {
        targ.rho[0]=0.05;                       // the position (psiN)
        targ.rho[1]=0.15;
        targ.rho[2]=0.25;
        targ.rho[3]=0.35;
        targ.rho[4]=0.60;
    
        targ.val[0]=2.70;                       // the target value Te (keV)
        targ.val[1]=2.40;
        targ.val[2]=1.90;
        targ.val[3]=1.40;
        targ.val[4]=0.60;
    }
    else {
        targ.rho[0]=TARGPOS1;                       // the position (psiN)
        targ.rho[1]=TARGPOS2;
        targ.rho[2]=TARGPOS3;
        targ.rho[3]=TARGPOS4;
        targ.rho[4]=TARGPOS5;
    
        targ.val[0]=TARGVAL1;                       // the target value Te (keV)
        targ.val[1]=TARGVAL2;
        targ.val[2]=TARGVAL3;
        targ.val[3]=TARGVAL4;
        targ.val[4]=TARGVAL5;
    }
    
    targ.weight=TARGWEIGHT;

    return targ;
}

/* This structure fucntion has to be connected EFIT-data.
   EFIT data are composed of R(psi)[33x1], psi[33x1], psiaxis[1x1], psibdry[1x1]
   vpr,grho2,and rhomax are assumed...*/
struct param_equi setvar_equi(void)
{
    struct param_equi equi;
    
    int i;    
	float xa[NUM];

    /* prepared the prompt nomalized position 
	  because of conflicting the order btw equi. and prof. structure */
	xa[0]=0.0;
    xa[NUM-1]=1.0;
    for (i=1;i<NUM-50;i++) {
        xa[i]=xa[i-1]+0.01;                           // normalized psi, radial grid points
        xa[NUM-1-i]=xa[NUM-1]-0.01*i;
	}

	/* polynomial fit from the reference discharge, a=0.5m R=1.8m k=1.75 */
    for(i=0;i<NUM;i++){
        equi.vpr[i]  =-25.6096*SQR(xa[i])*xa[i]+20.4594*SQR(xa[i])+41.3778*xa[i]+0.2640;   
        equi.grho2[i]=1.0;               // [101x1]
    }
    equi.rhomax=0.678;


    // aquisition of psi information ... 
    // psi R-position, they are always fixed
    equi.lpsi[0]=1.1000;  equi.lpsi[1]=1.1406;  equi.lpsi[2]=1.1812;  equi.lpsi[3]=1.2219;  equi.lpsi[4]=1.2625;  
    equi.lpsi[5]=1.3031;  equi.lpsi[6]=1.3438;  equi.lpsi[7]=1.3844;  equi.lpsi[8]=1.4250;  equi.lpsi[9]=1.4656;
    equi.lpsi[10]=1.5063; equi.lpsi[11]=1.5469; equi.lpsi[12]=1.5875; equi.lpsi[13]=1.6281; equi.lpsi[14]=1.6688; 
    equi.lpsi[15]=1.7094; equi.lpsi[16]=1.7500; equi.lpsi[17]=1.7906; equi.lpsi[18]=1.8312; equi.lpsi[19]=1.8719; 
    equi.lpsi[20]=1.9125; equi.lpsi[21]=1.9531; equi.lpsi[22]=1.9937; equi.lpsi[23]=2.0344; equi.lpsi[24]=2.0750; 
    equi.lpsi[25]=2.1156; equi.lpsi[26]=2.1563; equi.lpsi[27]=2.1969; equi.lpsi[28]=2.2375; equi.lpsi[29]=2.2781; 
    equi.lpsi[30]=2.3187; equi.lpsi[31]=2.3594; equi.lpsi[32]=2.4000;

    if  (DEBUG) {
        equi.vpsiaxis=-0.2854;     equi.vpsibdry=-0.1710;

        equi.vpsi[0]=-0.0975;  equi.vpsi[1]=-0.1029;  equi.vpsi[2]=-0.1090;  equi.vpsi[3]=-0.1158;  equi.vpsi[4]=-0.1236;  
        equi.vpsi[5]=-0.1325;  equi.vpsi[6]=-0.1423;  equi.vpsi[7]=-0.1531;  equi.vpsi[8]=-0.1658;  equi.vpsi[9]=-0.1816;
        equi.vpsi[10]=-0.1997; equi.vpsi[11]=-0.2189; equi.vpsi[12]=-0.2380; equi.vpsi[13]=-0.2552; equi.vpsi[14]=-0.2692; 
        equi.vpsi[15]=-0.2785; equi.vpsi[16]=-0.2823; equi.vpsi[17]=-0.2803; equi.vpsi[18]=-0.2724; equi.vpsi[19]=-0.2596; 
        equi.vpsi[20]=-0.2430; equi.vpsi[21]=-0.2241; equi.vpsi[22]=-0.2044; equi.vpsi[23]=-0.1856; equi.vpsi[24]=-0.1688; 
        equi.vpsi[25]=-0.1552; equi.vpsi[26]=-0.1442; equi.vpsi[27]=-0.1348; equi.vpsi[28]=-0.1268; equi.vpsi[29]=-0.1200; 
        equi.vpsi[30]=-0.1141; equi.vpsi[31]=-0.1090; equi.vpsi[32]=-0.1045;
    }
    else {
        // PSI read from RFM
        if  (kuOK != kstar_rfm_read(&equi.vpsiaxis,RFM_PSI_VALUE_AXIS,sizeof(equi.vpsiaxis))) {
            kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_AXIS\n");
            //continue;
        }
        if  (kuOK != kstar_rfm_read(&equi.vpsibdry,RFM_PSI_VALUE_BDRY,sizeof(equi.vpsibdry))) {
            kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_BDRY\n");
            //continue;
        }
        if  (kuOK != kstar_rfm_read(&equi.vpsi,RFM_PSI_VALUE_FIRST,sizeof(equi.vpsi))) {
            kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_FIRST\n");
            //continue;
        }
    }
    
    for(i=0;i<NPSI;i++) equi.npsi[i]=fabs((equi.vpsi[i]-equi.vpsiaxis)/(equi.vpsibdry-equi.vpsiaxis));
    for(i=0;i<NPSI;i++){
        if(equi.vpsi[i]>equi.vpsiaxis){
            equi.idxa=i;                             // magnetic axis array index
            continue;
        }
    }
    for(i=0;i<NPSI;i++){
        if(equi.vpsi[i]>equi.vpsibdry){
            equi.idxb=i;                             // magnetic bdry array index
            continue;
        }
    }

    return equi;
}

struct contr_coef el_coef_fce(struct param_equi equi,struct data_prof prof)
{
    struct contr_coef el_fce;
    
    int i;
    float mu,su,cu;
    float max_src_fce_el;
    float normp,ratio;
    float src_fce_el[NUM];
    float coef12[NUM];
    float vprsrc[NUM];
	
	float ECHPOS;

	char echpos[40];

    //assumed  ECH deposition profile, Gaussian shape 
    sprintf(echpos       ,"HICS_ECH_PWR_CTRL_ECHPOS"    );
    ECHPOS           =   kupaGetValue((char *)echpos);


    for(i=0;i<NUM;i++) el_fce.coef1[i]=1.0/(equi.vpr[i]*equi.grho2[i]*prof.ne[i]*1E+19*prof.heat_xe[i])/E;    
    
    mu=ECHPOS; // axis position of gaussian shape
	su=0.02;                                                                                     // FWHM
    for(i=0;i<NUM;i++) {
        cu=(prof.xa[i]-mu)/su;
        src_fce_el[i]=(float)exp(-0.5*(float)pow(cu,2.0))/((float)sqrt(2.0*M_PI)*su);
    }
        
    max_src_fce_el=0.0;
    for(i=0;i<NUM;i++) {
        if(max_src_fce_el < src_fce_el[i]) max_src_fce_el=src_fce_el[i];
    }
    for(i=0;i<NUM;i++) src_fce_el[i]=src_fce_el[i]/max_src_fce_el;
    for(i=0;i<NUM;i++) vprsrc[i]=equi.vpr[i]*src_fce_el[i];
    normp=equi.rhomax*trapz(prof.xa,vprsrc,NUM);
    ratio=1.0/normp;
    for(i=0;i<NUM;i++) src_fce_el[i]=ratio*src_fce_el[i];
    for(i=0;i<NUM;i++) vprsrc[i]=equi.vpr[i]*src_fce_el[i];
    for(i=0;i<NUM;i++) el_fce.coef2[i]=equi.rhomax*trapz(prof.xa,vprsrc,i);
    
    for(i=0;i<NUM;i++) coef12[i]=el_fce.coef1[i]*el_fce.coef2[i];           
    for(i=0;i<NUM;i++) el_fce.coef3[i]=equi.rhomax*trapz(prof.xa,coef12,i);   
    for(i=0;i<NUM;i++) el_fce.coef_fce[i]=-(el_fce.coef3[i]-el_fce.coef3[NUM-1]);

    return el_fce;
}

struct param_act actuator_fce(struct param_mimo par_fce,struct param_targ targ,struct contr_coef el_fce,struct data_prof prof)
{
    struct param_act fce;
    int i;
    
    fce.id=1;
    fce.prepower=0.01*1E6; // [w] from the ECH OPI system??
    for(i=0;i<CNUM;i++) {
		fce.el_coef[i]=lint(prof.xa,el_fce.coef_fce,NUM,targ.rho[i]);
	}
    fce.weight=par_fce.weight;
    fce.maxpower=par_fce.maxpower;
    fce.minpower=par_fce.minpower;

    return fce;
}

struct curr_stat setvar_curr(struct param_targ targ,struct data_prof prof)
{
    struct curr_stat curr_te;
    
    int i;

    for(i=0;i<CNUM;i++) curr_te.rho[i]=targ.rho[i];
    for(i=0;i<CNUM;i++) curr_te.val[i]=lint(prof.npos,prof.te,CHAN,curr_te.rho[i]);
    for(i=0;i<CNUM;i++) curr_te.err[i]=targ.val[i]-curr_te.val[i];
    curr_te.npt=CNUM;
    curr_te.weight=targ.weight;

    return curr_te;
}
