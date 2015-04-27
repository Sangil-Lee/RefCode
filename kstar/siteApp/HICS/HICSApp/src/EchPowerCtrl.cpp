#include "kutilObj.h"
#include "sfwCommon.h"
#include "RfmHandler.h"

#include <math.h>
#include "EchPowerCtrl.h"


void ech_power_ctrl (void *param)
{

	// operation variables
    int              nShotNumber;
    kuFloat64        dCtrlVolt = 0;
    char             szValue[64];
    
    int nPcsCurrTime;
    float nPcsCurrTimeUnitSec;
    float nPcsSubTime;
    float timeCheck;
    float timeSubCheck;
    int sw;

    int forceFastInterLock;

	// controller variables
    float el_coef0_fce[CNUM];
    float curr_te_err0[CNUM];
    
    int maxiter =2;
    int contiter=1;
    int i,j,ii;
    int m,n;

    float CO;              // control output from SVD
    float addpower_fce,addpower_fce_new,newpower_fce;
    float satur_up,satur_dn;
    float nmult;
    float quant_fce;

    float wmax,wmin;
    float **a,**u,*w,**v,*b,*x;

    int nsat_up=0;
    int nsat_dn=0;

    float vpsiaxis,vpsibdry;
    float vpsi[33];

    int step=0;

    float loadEceValue;
    char  ecePVname[20]="";
    
    char  echInjectStartTimePVname[20]="";
    char  echInjectEndTimePVname[20]="";
    float startCtrlTime;
    float endCtrlTime;

	char  dtupdctrltime[40];
	float DTUPDCTRLTIME;


    kuDebug (kuTRACE, "[ech_power_ctrl] started ...\n");

    while (kuTRUE) {
        //epicsThreadSleep (0.001);
        epicsThreadSleep (1);

        // for test
        if (kuOK != kstar_rfm_read (&nShotNumber, RFM_PCS_SHOT_NO, sizeof(nShotNumber))) {
            kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_SHOT_NO\n");
            continue;
        }
        kuDebug (kuTRACE, "[ech_power_ctrl] nShotNumber(%d)\n", nShotNumber);

        if (kuOK != kstar_rfm_read (&nPcsCurrTime, RFM_PCS_CURR_TIME, sizeof(nPcsCurrTime))) {
            kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
            continue;
        }
        kuDebug (kuTRACE, "[ech_power_ctrl] nPcsCurrTime(%d)\n", nPcsCurrTime);
        nPcsCurrTimeUnitSec=((float)(nPcsCurrTime)/(float)1E6)-15.0;    // PCS start is at -15.0 sec

        //operating parameters 
        sprintf(dtupdctrltime,"HICS_ECH_PWR_CTRL_DTIME"    );
        DTUPDCTRLTIME    =   kupaGetValue((char *)dtupdctrltime);


#if 1
        // for test
        //dCtrlVolt++;
		
        
        /* PCS-PCSRT5-ECHSEVER-ECHSOURCE test 
        sprintf(echInjectStartTimePVname,"ECH_LTU_P1_SEC0_T0");
        startCtrlTime = kupaGetValue ((char *)echInjectStartTimePVname);
        startCtrlTime = startCtrlTime-16.0;    // LTU start at -16.0 sec

        sprintf(echInjectEndTimePVname,"ECH_LTU_P1_SEC0_T1");
        endCtrlTime = kupaGetValue ((char *)echInjectEndTimePVname);
		endCtrlTime = endCtrlTime-16.0;        // LTU start at -16.0 sec

        //printf("startCtrlTime= %3.3f, EndCtrlTime= %3.3f\n",startCtrlTime,endCtrlTime);

        if (((nPcsCurrTimeUnitSec)>=(startCtrlTime)) & ((nPcsCurrTimeUnitSec)<=(endCtrlTime-0.1))) {
            printf("CurrTime= %3.5f, startCtrlTime= %3.3f, EndCtrlTime= %3.3f\n",nPcsCurrTimeUnitSec,startCtrlTime,endCtrlTime);
            dCtrlVolt=55.0;
        }
        else {
            dCtrlVolt=0.0;
        }
        */
        
		/*  */
        struct param_mimo  par_fce;
        struct param_targ  targ   ;
        struct param_equi  equi   ;
        struct data_prof   prof   ;
        struct contr_coef  el_fce ;
        struct param_act   act_fce;
        struct curr_stat   curr_te;

        
        par_fce=setvar_fce();
        targ   =setvar_targ();
        equi   =setvar_equi();
        prof   =setdis_prof(equi);
        el_fce =el_coef_fce(equi,prof);
        act_fce=actuator_fce(par_fce,targ,el_fce,prof);
        curr_te=setvar_curr(targ,prof);
        


        /* PCS-PCSRT5-ECHSERVER test
        if ((int)(step/2)*2==step){
            dCtrlVolt=10.0;
        }
        else
        {
            dCtrlVolt=20.0;
        }
        step++;
        printf("dCtrlVolt= %f, step=%d\n",dCtrlVolt,step);
        printf("shot time!! = %f\n",(float)(nPcsCurrTime)/1000000);
        */


        /* PCS-PCSRT5 test.. psi data loading test 
        if (nPcsCurrTime>15000000){
            // PSI read from RFM
            if(kuOK != kstar_rfm_read(&vpsiaxis,RFM_PSI_VALUE_AXIS,sizeof(vpsiaxis))){
                kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_AXIS\n");

            }
            if(kuOK != kstar_rfm_read(&vpsibdry,RFM_PSI_VALUE_BDRY,sizeof(vpsibdry))){
                kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_BDRY\n");
            }
            if(kuOK != kstar_rfm_read(&vpsi,RFM_PSI_VALUE_FIRST,sizeof(vpsi))){
                kuDebug (kuFATAL,"[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PSI_VALUE_FIRST\n");
            }
    
            printf("vpsiaxis= %f\n",vpsiaxis);
            printf("vpsibdry= %f\n",vpsibdry);
            for(i=0;i<33;i++) printf("%d vpsi    = %f\n",i,vpsi[i]);
            sleep(1);    // 1 second sleep
        }
        else
        {
            printf("before shot starting...[Time: %3.4u] [shot: %d]\n",nPcsCurrTime,nShotNumber);
            sleep(1);
        }
        */


        /* ECESER0VER-PCSRT5 test.. ECE data loading test
        loadEceValue=100;
        printf("loadEceValue initial: %f\n",loadEceValue);
        sleep(2);

        for (i=0;i<CHAN;i++) {
            sprintf(ecePVname,"ECEHR_READ_DATA_CH%02d",i);
            loadEceValue = kupaGetValue ((char *)ecePVname);
            printf("%d CH, ECE Val=%f\n",i+1,loadEceValue);
        }
        */




#else
        //TODO : add your algorithm here
            
        // This is a real-time ative controller for electron temperature profile.
        // The controller has been developed by Sun Hee Kim. (Nucl. Fusion 52 (2012) 074002)
        // For the KSTAR, it is modified/implemented by Hyun-Seok Kim. 


        //---------------------------------- main function ----------------------------------//

        timeCheck=nPcsCurrTimeUnitSec-floor(nPcsCurrTimeUnitSec/DTUPDCTRLTIME)*DTUPDCTRLTIME;
        if(timeCheck<=0.35){
            sw=1;
        }  
        else if((timeCheck>0.35) & (timeCheck<=0.49)){
            sw=2;
        }
        else{
            sw=3;
        }


        struct param_mimo  par_fce;
        struct param_targ  targ   ;
        struct param_equi  equi   ;
        struct data_prof   prof   ;
        struct contr_coef  el_fce ;
        struct param_act   act_fce;
        struct curr_stat   curr_te;


        switch(sw)
        {
            case 1:
            {    
                
                par_fce=setvar_fce();
                targ   =setvar_targ();
                equi   =setvar_equi();
                prof   =setdis_prof(equi);
                el_fce =el_coef_fce(equi,prof);
                act_fce=actuator_fce(par_fce,targ,el_fce,prof);
                curr_te=setvar_curr(targ,prof);
                

                nPcsSubTime=0.0;
                do
                {
                    if(kuOK != kstar_rfm_read (&nPcsSubTime, RFM_PCS_CURR_TIME, sizeof(nPcsSubTime))){
                        kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
                        continue;
                    }
                    kuDebug (kuMON, "[ech_power_ctrl] nPcsSubTime(%d)\n", nPcsSubTime);
                
                    nPcsSubTime=nPcsSubTime*1E-6;    // [s]
                    timeSubCheck=nPcsSubTime-floor(nPcsSubTime/DTUPDCTRLTIME)*DTUPDCTRLTIME;
                }while(timeSubCheck<=0.01);          // each 10 ms interval    

                break;
            }

    
            case 2:
            {    
                for(i=0;i<CNUM;i++) el_coef0_fce[i]=act_fce.el_coef[i]; 
                for(i=0;i<CNUM;i++) curr_te_err0[i]=curr_te.err[i];
            
                ii=0;
                addpower_fce_new=0.0;

                while(contiter & (ii<maxiter))                                            // continue interation until contiter==0
                {
                    ii++;

                    m=CNUM;           // target array 
                    n=1;              // only one actuator fce

                    a=matrix(1,m,1,n);
                    u=matrix(1,m,1,n);
                    v=matrix(1,n,1,n);
                    w=vector(1,n);
                    b=vector(1,m);
                    x=vector(1,n);

                    for(i=0;i<m;i++) a[i+1][1]=act_fce.el_coef[i];
                    for(i=0;i<m;i++) b[i+1]=curr_te.err[i];
                    for(i=1;i<=m;i++)
                        for(j=1;j<=n;j++)
                            u[i][j]=a[i][j];

                    svdcmp(u,m,n,w,v);
                    wmax=0.0;
                    for(j=1;j<=n;j++) if(w[j]>wmax) wmax=w[j];
                    wmin=wmax*1.0e-6;
                    for(j=1;j<=n;j++) if(w[j]<wmin) w[j]=0.0;
                    svbksb(u,w,v,m,n,b,x);
                    CO=*x;

                    quant_fce=0.05*1e6;
                    for(i=0;i<CNUM;i++) act_fce.el_coef[i]=el_coef0_fce[i];
                    if(isnan(CO)) CO=0.0;
                    if(fabs(CO)<1e3) CO=0.0;

                    addpower_fce=CO*act_fce.weight;
                    newpower_fce=act_fce.prepower+addpower_fce;
        
                    satur_up=(newpower_fce>act_fce.maxpower);
                    satur_dn=(newpower_fce<act_fce.minpower);
                    if((satur_up==0) & (satur_dn==0))
                    {
                        nmult=round(addpower_fce/quant_fce,0);
                        addpower_fce_new=addpower_fce_new+nmult*SIGN(addpower_fce)*quant_fce;
                        addpower_fce=addpower_fce_new;
                        newpower_fce=act_fce.prepower+addpower_fce;

                        for(i=0;i<CNUM;i++) curr_te.err[i]=curr_te_err0[i]-addpower_fce*el_coef0_fce[i];
                        if(nmult>=1) maxiter=MAX(i+1,maxiter);
                    }
                    satur_up=(newpower_fce>act_fce.maxpower);
                    satur_dn=(newpower_fce<act_fce.minpower);
                    if(satur_up) nsat_up++;
                    if(satur_dn) nsat_dn++;
                    if(nsat_up+nsat_dn==0) break;
                    else{
                        for(i=0;i<CNUM;i++) act_fce.el_coef[i]=0.0*el_coef0_fce[i];
                    }
                    free_matrix(a,1,m,1,n);
                    free_matrix(u,1,m,1,n);
                    free_matrix(v,1,n,1,n);
                    free_vector(w,1,n);
                    free_vector(b,1,m);
                    free_vector(x,1,n);

                }
                newpower_fce=MIN(act_fce.maxpower,MAX(act_fce.minpower,newpower_fce));

                nPcsSubTime=0.0;
                do
                {
                    if(kuOK != kstar_rfm_read (&nPcsSubTime, RFM_PCS_CURR_TIME, sizeof(nPcsSubTime))){
                        kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
                        continue;
                    }
                    kuDebug (kuMON, "[ech_power_ctrl] nPcsSubTime(%d)\n", nPcsSubTime);
                
                    nPcsSubTime=nPcsSubTime*1E-6;    // [s]
                    timeSubCheck=nPcsSubTime-floor(nPcsSubTime/DTUPDCTRLTIME)*DTUPDCTRLTIME;
                }while(timeSubCheck<=0.49);          // each 10 ms interval    

                break;
            }


            case 3:
            {

                nPcsSubTime=0.0;
                do
                {
                    if(kuOK != kstar_rfm_read (&nPcsSubTime, RFM_PCS_CURR_TIME, sizeof(nPcsSubTime))){
                        kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
                        continue;
                    }
                    kuDebug (kuMON, "[ech_power_ctrl] nPcsSubTime(%d)\n", nPcsSubTime);
                
                    nPcsSubTime=nPcsSubTime*1E-6;     // [s]
                    timeSubCheck=nPcsSubTime-floor(nPcsSubTime/DTUPDCTRLTIME)*DTUPDCTRLTIME;
                }while(timeSubCheck<=0.499);          // each 10 ms interval    


                dCtrlVolt=newpower_fce;               // power -> V equation required..

                break;
            }

            default:
                continue;

          

        }
    
    
    
#endif

        // ---------------------------------
        // update value
        // ---------------------------------


        if (kuOK != kstar_rfm_read (&nPcsCurrTime, RFM_PCS_CURR_TIME, sizeof(nPcsCurrTime))) {
            kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_PCS_CURR_TIME\n");
            continue;
        }
        kuDebug (kuTRACE, "[ech_power_ctrl] nPcsCurrTime(%d)\n", nPcsCurrTime);
        nPcsCurrTimeUnitSec=((float)(nPcsCurrTime)/(float)1E6)-15.0;

        if (((nPcsCurrTimeUnitSec)>=(startCtrlTime)) & ((nPcsCurrTimeUnitSec)<=(startCtrlTime+0.2))) {
           printf("db_put out time= %3.5f\n",nPcsCurrTimeUnitSec);
		}


        if (kuOK != kstar_rfm_read (&forceFastInterLock, RFM_FORCE_FAST_INTERLOCK, sizeof(forceFastInterLock))) {
            kuDebug (kuFATAL, "[kstar_rfm_handler] kstar_rfm_read() failed for RFM_FORCE_FAST_INTERLOCK\n");
            continue;
        }
        
		if (forceFastInterLock==0) 	dCtrlVolt=0;        // PCS and CCS fault interlock. 
	

		sprintf (szValue, "%f", dCtrlVolt);
        db_put (ECH_PWR_CTRL_PVNAME, szValue);
    }
}
