/****************************************************************************

Module      : CodacPcsRun.c

Copyright(c): 2011 NFRI. All Rights Reserved.

Revision History:
Author: woong   2011-11-30 assisted by swyun


*****************************************************************************/
#include "CodacPcsRun.h"

#include <sys/mman.h> // Need for mlockall()
 #include <unistd.h> // need for sysconf(int name);
 #include <malloc.h>
 #include <sys/time.h> // need for getrusage
 #include <sys/resource.h> // need for getrusage


CODAC_Controller::CODAC_Controller()
{
	m_cmdU = 0;
//	OffCnt = 0;
//	useOfflineDensity = false;
	m_dtemp = 0.0;
	m_maxRefCnt = INIT_PID_RUN_TIME * INIT_PID_RUN_RATE ;
	m_pRefData = NULL;
	m_pRefPidEnable = NULL;
	m_invDensityScale = INIT_INV_REF_SCALE;
	m_offlineABbit = TYPE_A;
	
	ref_section_cnt = INIT_REF_SECT_CNT;

	m_bliptime = 16;	
	fb_start_time = INIT_START_TIME;
	fb_stop_time = INIT_STOP_TIME;
	fb_period = fb_stop_time - fb_start_time;
	m_kp = m_ki = m_kd = m_h = m_inv_h = m_prev_error = m_error_thresh = m_integral = 0.0;
	m_PID_enable = false;
	op_start_time = (-14.0);
	op_stop_time = 10;
	m_status = 0x0;
	
	pOffline_rawData	= NULL;
	fpDensity	= NULL;
	fpDMean		= NULL;
	fpPIDlog	= NULL;
}

CODAC_Controller::~CODAC_Controller()
{
	if( fpDensity )
		fclose(fpDensity);
	if( fpDMean )
		fclose(fpDMean);
	if( fpPIDlog )
		fclose(fpPIDlog);
	
	if (pOffline_rawData) {
		for( int i=0; i<2; i++) {
			if (pOffline_rawData[i]) {
				free(pOffline_rawData[i]);
			}
		}
		free(pOffline_rawData);
	}

	if (m_pRefData)
		free(m_pRefData);
	if (m_pRefPidEnable)
		free(m_pRefPidEnable);
}

void CODAC_Controller::pid_initialize(double kp, double ki, double kd, double error_thresh, double step_time)
{
	m_kp = kp;
	m_ki = ki;
	m_kd = kd;
	m_error_thresh = error_thresh;
	m_curr_error = 0.0;
	m_curr_ref = 0.0;

	m_h = step_time;
	m_inv_h = 1/step_time;

	m_integral = 0;
	m_started = false;
	m_pidCnt = 0;

	m_status |= STATE_PID_INIT;

}

double CODAC_Controller::pid_update(double error)
{
	double q, deriv;

	if(fabs(error) < m_error_thresh )
		q = 1;
	else 
		q = 0;

	m_integral += m_h*q*error;

	if( !m_started )
	{
		m_started = true;
		deriv = 0;
	}
	else
		deriv = (error - m_prev_error) * m_inv_h;

	m_prev_error = error;

	m_cmdU = m_kp*(error + m_ki*m_integral + m_kd*deriv);

	/*
	have to consider if negative voltage command.
	who is handling minus output, PCS or FUEL ?
	*/

	m_pidCnt++;

	return m_cmdU;

}

void CODAC_Controller::save_log()
{
//	if (fpPIDlog) {
		fprintf(fpPIDlog, "%d %lf %lf %lf   %lf \n", m_pidCnt, m_density_mean, m_curr_ref, m_curr_error, m_cmdU);
//	}
}

void CODAC_Controller::show_internal_value()
{
	printf("\n=========================\n");
	printf("PID command U:     %lf\n", m_cmdU);
	printf("density scale(inv): %.2e \n", m_invDensityScale);
	printf("OP blip time:    %d\n", m_bliptime);	
	printf("System status: 0x%X\n", m_status); 

	if( m_status & STATE_MAKE_PATTERN )
		printf("Make pattern ............\t Done.\n");
	else printf("Make pattern ............\t required!\n"); 

	if( m_status & STATE_PID_INIT )
		printf("PID initialize ..........\t Done.\n");
	else printf("PID  initialize ..........\t required!\n"); 

	if( m_status & STATE_DENSITY_INIT )
		printf("Density initialize ......\t Done.\n");
	else printf("Density  initialize ......\t required!\n"); 

	if( m_PID_enable ) 
		printf("PID .............\t enabled.\n");
	else printf("PID .............\t disabled.\n");

	printf("\tFeedback start time:\t%f\n", fb_start_time);
	printf("\tFeedback stop time:\t%f\n", fb_stop_time);
	printf("\tFeedback period:\t%f\n", fb_period);
	printf("\tNum. of reference:\t%d\n", m_maxRefCnt );

	printf("Section counter:    %d\n", ref_section_cnt);
	printf("Section start:        %.4f   with '0' value\n", op_start_time);
	for( int i=0; i<ref_section_cnt; i++) {
		printf("Section %d:   (%.3f, %.3f), a:%f\n", i, ST_Ref_step[i].x2, ST_Ref_step[i].y2, ST_Ref_step[i].a);
	}

}

void CODAC_Controller::sys_reset()
{
	m_status = 0x0;
	printf("System reset!!\n");
}

/*
	return decreasing scaled ref. data 
*/
double CODAC_Controller::pid_getRefSigVoltage(__u32 cnt)
{
	if( cnt > m_maxRefCnt ) return 0.0;

	m_curr_ref = m_pRefData[cnt];
	
	return m_curr_ref;
}
double CODAC_Controller::pid_getRefSigQuantized(__u32 cnt)
{
	if( cnt > m_maxRefCnt ) return 0.0;

	m_curr_ref = m_pRefData[cnt]*m_invDensityScale; // (1e-19)
	
	return m_curr_ref;
}

/*
ref: PCS target reference signal
H: Plant feedback from MMWI density
*/
double CODAC_Controller::pid_processError(double ref, double H)
{
//	double errorS;

	m_curr_error = ref - H;

//	if( useOfflineDensity ) {
//		fprintf(fpDensity, "%d: %d->%d, (%lf, %lf) V, (%lf, %lf) D, %lf,      %lf\n", dCnt, curCH, nextCh, vltg[0], vltg[1], calcD[0], calcD[1], m_dtemp, calcDensity[i] );
//		fprintf(fpPIDlog, "%d %lf %lf     %lf\n", m_pidCnt, ref, H,  m_curr_error);
//	}

	return m_curr_error;
}


/*
	load full scaled target reference data.  *10E19
*/
int CODAC_Controller::pid_load_refFile(const char *filename, int ab)
{
	FILE *fp;
	double rawD;
	int got;
	int cnt=0;
	char bufLineScan[512];
	double dTime1, vltg1;

	if( ab == TYPE_A) {
		if((fp = fopen(filename, "r"))==NULL) {
			return ERROR;
		}
		while(1) 	{
			if( fgets(bufLineScan, 512, fp) == NULL ) break;
			cnt++;
		}
		fseek(fp, 0, SEEK_SET);
		m_pRefData = (double *)malloc(sizeof(double)*cnt);
		if( m_pRefData == NULL ) {
			printf("pid_load_refFile, malloc error!\n");
			fclose(fp);
			return ERROR;
		}
		cnt = 0;
		while(1) 
		{
			if( fgets(bufLineScan, 512, fp) == NULL ) {
				break;
			}
			sscanf(bufLineScan, "%lf %le",  &dTime1, &vltg1);
			m_pRefData[cnt] = vltg1;
			cnt++;
//			if( cnt > 140000 && cnt <140010 )
//				printf("%d, ref: %lf, %f\n", cnt, vltg1/1.e+10, m_pRefData[cnt-1]);
		}
		fclose(fp);
		
	}
	else 
	{
		if((fp = fopen(filename, "rb"))==NULL) {
			return ERROR;
		}
		while(1) {
			got = fread( &rawD, sizeof(double), 1, fp);
			if( got != 1) {
				break;
			}
			cnt++;
		}
		fseek(fp, 0, SEEK_SET);

		m_pRefData = (double *)malloc(sizeof(double)*cnt);
		if( m_pRefData == NULL ) {
			printf("pid_load_refFile, malloc error!\n");
			fclose(fp);
			return ERROR;
		}
		got = fread( m_pRefData, sizeof(double), cnt, fp );
		if( got != cnt ) {
			printf(	"FAIL <---  (read() error, size:%ld, requested %d bytes, got %d)\n", sizeof(double), cnt, got);
			fclose(fp);
			return ERROR;
		}
		fclose(fp);
		
	}
	
	m_maxRefCnt = cnt;
	printf("your target signal count: %d, malloc size: %ld byte\n", m_maxRefCnt, sizeof(double)*cnt);

	return OK;
}


int CODAC_Controller::open_message_file()
{
	if(fpDensity) fclose(fpDensity);
	if((fpDensity = fopen("myDensity.txt", "w"))==NULL) {
		printf("\"%s\" open error!\n", "myDensity.txt");
		return ERROR;
	}
	
	if(fpDMean) fclose(fpDMean);
	if((fpDMean = fopen("meanDensity.txt", "w"))==NULL) {
		printf("\"%s\" open error!\n", "meanDensity.txt");
		return ERROR;
	}
	
	if(fpPIDlog) fclose(fpPIDlog);
	if((fpPIDlog = fopen("pidLog.txt", "w"))==NULL) {
		printf("\"%s\" open error!\n", "pidLog.txt");
		return ERROR;
	}

//	useOfflineDensity = true;

	return OK;
}

__u32 CODAC_Controller::dnst_check_data_cnt(const char * rawFile,int ab)
{
	__u32 Cnt = 0;
	FILE *fp;
	char bufLineScan[512];
	int got;
	unsigned short rawD[2];

	if( ab == TYPE_A) {
		if((fp = fopen(rawFile, "r"))==NULL) 
			return 0;
		while(1) 
		{
			if( fgets(bufLineScan, 512, fp) == NULL ) {
				Cnt -= 1;
				break;
			}
			Cnt++;
		}
	}
	else 
	{
		if((fp = fopen(rawFile, "rb"))==NULL)
			return 0;
		while(1) {
			got = fread( rawD, sizeof(unsigned short), 2, fp);
			if( got != 2) {
				Cnt -= 1;
				break;
			}
			Cnt++;
		}
	}
	fclose(fp);
	
	printf("your raw datal count: %d\n", Cnt);
	return Cnt;
}

void CODAC_Controller::op_set_dummy_raw()
{
	int toggle = 1;
	int j = 0;
	for(int i=0; i<m_maxRefCnt; i++) {
//		pOffline_rawData[0][i] = i;
		if( toggle ) {
			pOffline_rawData[1][i] = j;
			j++;
			if( j > 10000) {
				toggle = 0;
				j--;
			}
		}
		else {
			pOffline_rawData[1][i] = j;
			j--;
			if( j < 0) {
				toggle = 1;
				j++;
			}
		}
	}
}

double CODAC_Controller::op_get_voltage(__u32 raw)
{
	return (20.0 * ((double)(0xffff & raw) / (double)0xffff) - 10.0);
}
__u32 CODAC_Controller::op_get_maxRefCnt()
{
	return m_maxRefCnt;
}

void CODAC_Controller::op_set_startTime(double time)
{
	op_start_time = time;
}
double CODAC_Controller::op_get_startTime()
{
	return op_start_time;
}

void CODAC_Controller::op_set_stopTime(double time)
{
	op_stop_time = time;
}
double CODAC_Controller::op_get_stopTime()
{
	return op_stop_time;
}


/*
chNo: start with 0, 1, 2, ..., 7
*/
double CODAC_Controller::dnst_average_from_block(__u32 *aiBuf, int EOB, int chNo)
{
	double sum = 0.0;
	int i;
	for (i=0; i<EOB; i++)
	{
		sum += 20.0 * ((double)(0xffff & aiBuf[USED_ADC_CH * i + chNo]) / (double)0xffff) - 10.0;
	}
	return (sum / (double)EOB);
}


int CODAC_Controller::dnst_read_dataFile(const char *rawFile, int ab)
{
	FILE *fp;
	char bufLineScan[512];
	double dTime1, dTime2, vltg1, vltg2;
	unsigned short rawD[2];
	int got;
	__u32 OffCnt = 0;

	if( ab == TYPE_A) {
		if((fp = fopen(rawFile, "r"))==NULL) {
			return ERROR;
		}
		while(1) 
		{
			if( fgets(bufLineScan, 512, fp) == NULL ) {
				OffCnt -= 1;
				break;
			}

			sscanf(bufLineScan, "%lf %lf %lf %lf",  &dTime1, &vltg1, &dTime2, &vltg2);

			pOffline_rawData[0][OffCnt] = (unsigned short)(((vltg1 + 10.0) * 0xffff) / 20.0 );
			pOffline_rawData[1][OffCnt] = (unsigned short)(((vltg2 + 10.0) * 0xffff) / 20.0 );
			offline_rawData[0][OffCnt] = vltg1;
			offline_rawData[1][OffCnt] = vltg2;


			OffCnt++;

		}
		printf("first offline data: %f, %f\n", offline_rawData[0][0], offline_rawData[1][0] );
	}
	else 
	{
		if((fp = fopen(rawFile, "rb"))==NULL) {
			return ERROR;
		}
		while(1) {
			got = fread( rawD, sizeof(unsigned short), 2, fp);
			if( got != 2) {
				OffCnt -= 1;
				break;
			}
			
			pOffline_rawData[0][OffCnt] = rawD[0];
			pOffline_rawData[1][OffCnt] = rawD[1];

			OffCnt++;
		}
	}
	
	fclose(fp);

	m_maxRefCnt = OffCnt;
	printf("your data signal count: %d\n", m_maxRefCnt);

	return OK;
}

int CODAC_Controller::dnst_load_dataFile(const char *rawFile, int ab)
{
	m_offlineABbit = ab;
	
	__u32 dataCnt = dnst_check_data_cnt(rawFile, ab);

	dataCnt += 10;  // just for margin

	if( dataCnt > 500000 ) 
	{
		printf("raw data count overflow! cnt:%d\n", dataCnt);
		return ERROR;
	}

	pOffline_rawData = (__u16 **)malloc( sizeof(unsigned short *) * 2 );  // two channel input from MMWI
	for( int i=0; i<2; i++) {
		pOffline_rawData[i] = (__u16 *)malloc(sizeof(unsigned short)*dataCnt);
		if( pOffline_rawData[i] == NULL ) {
			printf("dnst_initialize, malloc error!\n");
			return ERROR;
		}
	}

	if(dnst_read_dataFile(rawFile, ab)) {
		printf("\"%s\" open error!\n", rawFile);
		for( int i=0; i<2; i++) free(pOffline_rawData[i]);
		free(pOffline_rawData);
		return ERROR;
	}

	printf("\"%s\" has %d count data. malloc size: %ld byte\n", rawFile, m_maxRefCnt, sizeof(unsigned short)*dataCnt*2);

	open_message_file();

	return OK;
}

__u32 CODAC_Controller::dnst_getData_ch0(__u32 cnt)
{
	if (cnt > m_maxRefCnt )
		return ERROR;
	return (__u32)pOffline_rawData[0][cnt];
}
__u32 CODAC_Controller::dnst_getData_ch1(__u32 cnt)
{
	if (cnt > m_maxRefCnt )
		return ERROR;
	return (__u32)pOffline_rawData[1][cnt];
}

double CODAC_Controller::dnst_getRaw_ch0(__u32 cnt)
{
	if (cnt > m_maxRefCnt )
		return 0.;
	return offline_rawData[0][cnt];
}
double CODAC_Controller::dnst_getRaw_ch1(__u32 cnt)
{
	if (cnt > m_maxRefCnt )
		return 0.;
	return offline_rawData[1][cnt];
}


void CODAC_Controller::dnst_initialize(double a1,double b1,double c1,double d1,double a2,double b2,double c2,double d2, double limU, double limL)
{
	m_a1 = a1;
	m_b1 = b1;
	m_c1 = c1;
	m_d1 = d1;
	m_a2 = a2;
	m_b2 = b2;
	m_c2 = c2;
	m_d2 = d2;

	m_limU = limU;
	m_limL = limL;
	m_density = 0.0;
	m_density_mean = 0.0;
	m_dtemp = 0.0;


	curCH = 0;

	isFirst = true;

	m_status |= STATE_DENSITY_INIT;

}

double CODAC_Controller::dnst_process (int elofData, __u32 *aiBuf)
{
	int i;

	double avgNES1, avgNES2;
	double calcDensity[MAX_ACCUM_DATA];
	double calcD[USED_INPUT_CH];
	double vltg[USED_INPUT_CH];
	int nextCh;
	double tempDen=0.0;

	if( elofData > MAX_ACCUM_DATA ) {
		printf("input data count error: limit %d, input %d\n", MAX_ACCUM_DATA, elofData );
		return 0.0 ;
	}

	
#if 1
	if(isFirst )
	{
		avgNES1 = dnst_average_from_block(aiBuf, elofData, 0);
		avgNES2 = dnst_average_from_block(aiBuf, elofData, 1);
		
		if (fabs(avgNES1 - 0.1) < fabs(avgNES2 - 0.1) )
		{
			curCH = 0;
			m_dtemp -= avgNES1;
		} else
		{
			curCH = 1;
			m_dtemp -= avgNES2;
		}
		isFirst = false;
		
	}
#endif
	nextCh = curCH;

	for (i=0; i<elofData; i++)
	{
		vltg[0] =  20.0 * ((double)(0xffff & aiBuf[USED_ADC_CH * i + 0]) / (double)0xffff) - 10.0;
		vltg[1] =  20.0 * ((double)(0xffff & aiBuf[USED_ADC_CH * i + 1]) / (double)0xffff) - 10.0;
		
		calcD[0] = m_a1*((m_b1*(vltg[0]*vltg[0])) + (m_c1 * vltg[0]) + m_d1); 
		calcD[1] = m_a2*((m_b2*(vltg[1]*vltg[1])) + (m_c2 * vltg[1]) + m_d2); 


		if(calcD[curCH] > m_limU)
		{
			nextCh = (curCH==0)?1:0;
			
			m_dtemp = m_dtemp + (FRINGE / 2.);
		}
		if(calcD[curCH]  < m_limL)
		{
			nextCh = (curCH==0)?1:0;
			
			m_dtemp = m_dtemp - (FRINGE / 2.);
		}
		
		calcDensity[i] = m_dtemp + calcD[nextCh];

		tempDen += calcDensity[i];


//		if( useOfflineDensity ) {
//			fprintf(fpDensity, "%d: %d->%d, (%lf, %lf) V, (%lf, %lf) D, %lf,      %lf\n", dCnt, curCH, nextCh, vltg[0], vltg[1], calcD[0], calcD[1], m_dtemp, calcDensity[i] );
//			fprintf(fpDensity, "%lf\n", calcDensity[i] );
//		}

		curCH = nextCh;

	}

	m_density = calcDensity[elofData-1];
	m_density_mean = tempDen / (double)elofData ;

//	if( useOfflineDensity ) {
//		fprintf(fpDMean, "%lf\n", m_density_mean );
//	}
	
	return (m_density_mean);
}


#if 0
double CODAC_Controller::dnst_process (int elofData, __u32 *aiBuf)
{
	int i;
	static unsigned int dCnt = 0;

	double avgNES1, avgNES2;
	double dtemp=0;
	double calcDensity[MAX_ACCUM_DATA];
	double calcD[USED_INPUT_CH];
	double d;
	double d1, d2;
	double vltg[USED_INPUT_CH];
	int ccw;

	if( elofData > MAX_ACCUM_DATA ) {
		printf("input data count error: limit %d, input %d\n", MAX_ACCUM_DATA, elofData );
		return 0.0 ;
	}

	
	dtemp = d0;
	
	ccw = curCH;

	/* 3. Difference from center is smaller than another channel? No, goto #4.  Yes, goto #6  */
	/* 5. set new base density */
	for (i=0; i<elofData; i++)
	{
		vltg[0] =  20.0 * ((double)(0xffff & aiBuf[8 * i + 0]) / (double)0xffff) - 10.0;
		vltg[1] =  20.0 * ((double)(0xffff & aiBuf[8 * i + 1]) / (double)0xffff) - 10.0;
		
		d1 = m_a1*((m_b1*(vltg[0]*vltg[0])) + (m_c1 * vltg[0]) + m_d1); 
		d2 = m_a2*((m_b2*(vltg[1]*vltg[1])) + (m_c2 * vltg[1]) + m_d2); 

		
		if( ccw == 0 ){
			if(d1 > m_limU)
			{
				ccw = 1;
				dtemp = dtemp + (FRINGE / 2.);
			}
			if(d1  < m_limL)
			{
				ccw = 1;
				dtemp = dtemp - (FRINGE / 2.);
			}
			d = d1 + dtemp;
		} 
		else if ( ccw == 1 ) 
		{
			if(d2 > m_limU)
			{
				ccw = 0;
				dtemp = dtemp + (FRINGE / 2.);
			}
			if(d2  < m_limL)
			{
				ccw = 0;
				dtemp = dtemp - (FRINGE / 2.);
			}
			d = d2 + dtemp;
		}

		curCH = ccw;
		d0 = dtemp;

		

		if( useOfflineDensity ) {
			fprintf(fpDensity, "%d: %d,  (%lf, %lf) V, (%lf, %lf) D,      %lf\n", dCnt, ccw, vltg[0], vltg[1], d1, d2, d);
//			fprintf(fpDensity, "%lf\n",  d);
		}

		dCnt++;

	}
	

	m_density = d;
	
	return (m_density);
}
#endif

double CODAC_Controller::dnst_process_test(__u32 cnt)
{
	int i;

	double avgNES1, avgNES2;
	double calcDensity[MAX_ACCUM_DATA];
	double calcD[USED_INPUT_CH];
	double vltg[USED_INPUT_CH];
	int nextCh;
	double tempDen=0.0;
	
#if 0
	if(isFirst )
	{
		double sum1=0.0, sum2 = 0.0;
		for (i=0; i<10; i++)
		{
			sum1 += offline_rawData[0][i];
			sum2 += offline_rawData[1][i];
		}
		avgNES1 = sum1 / 10.0;
		avgNES1 = sum2 / 10.0;
		
		if (fabs(avgNES1 - 0.1) < fabs(avgNES2 - 0.1) )
		{
			curCH = 0;
			m_dtemp -= avgNES1;
		} else
		{
			curCH = 1;
			m_dtemp -= avgNES2;
		}
		isFirst = false;
		
	}
#endif
	nextCh = curCH;

	for (i=0; i<10; i++)
	{
		vltg[0] =  offline_rawData[0][cnt+i];
		vltg[1] =  offline_rawData[1][cnt+i];
		
		calcD[0] = m_a1*((m_b1*(vltg[0]*vltg[0])) + (m_c1 * vltg[0]) + m_d1); 
		calcD[1] = m_a2*((m_b2*(vltg[1]*vltg[1])) + (m_c2 * vltg[1]) + m_d2); 


		if(calcD[curCH] > m_limU)
		{
			nextCh = (curCH==0)?1:0;
			
			m_dtemp = m_dtemp + (FRINGE / 2.);
		}
		if(calcD[curCH]  < m_limL)
		{
			nextCh = (curCH==0)?1:0;
			
			m_dtemp = m_dtemp - (FRINGE / 2.);
		}
		
		calcDensity[i] = m_dtemp + calcD[nextCh];

		tempDen += calcDensity[i];


//		if( useOfflineDensity ) {
//			fprintf(fpDensity, "%d: %d->%d, (%lf, %lf) V, (%lf, %lf) D, %lf,      %lf\n", dCnt, curCH, nextCh, vltg[0], vltg[1], calcD[0], calcD[1], m_dtemp, calcDensity[i] );
//			fprintf(fpDensity, "%lf\n", calcDensity[i] );
//		}

		curCH = nextCh;
		
	}

	m_density = calcDensity[0];
	m_density_mean = tempDen / 10.0;

//	if( useOfflineDensity ) {
//		fprintf(fpDMean, "%lf\n", m_density_mean );
//	}
	
	return (m_density_mean);
}

void CODAC_Controller::op_memoryLock(int size)
{
	int i, page_size;
	char* buffer;
	struct rusage usage;
		
	// Now lock all current and future pages from preventing of being paged
	if (mlockall(MCL_CURRENT | MCL_FUTURE ))
	{
		perror("mlockall failed:");
	}
	
	// Turn off malloc trimming.
	mallopt (M_TRIM_THRESHOLD, -1);
		   
	// Turn off mmap usage.
	mallopt (M_MMAP_MAX, 0);

	page_size = sysconf(_SC_PAGESIZE);
	buffer = (char *)malloc(size);

	printf("request size: %d byte, page size: %d\n", size, page_size);

	// Touch each page in this piece of memory to get it mapped into RAM
	for (i=0; i < size; i+=page_size)
	{
		// Each write to this buffer will generate a pagefault.
		// Once the pagefault is handled a page will be locked in memory and never
		// given back to the system.
		buffer[i] = 0;
		// print the number of major and minor pagefaults this application has triggered
		getrusage(RUSAGE_SELF, &usage);
		printf("Major-pagefaults:%ld, Minor Pagefaults:%ld\n", usage.ru_majflt, usage.ru_minflt);
	}
	free(buffer);


}

void CODAC_Controller::op_set_bliptime(int blip)
{
	m_bliptime = blip;
}

int CODAC_Controller::op_get_bliptime()
{
	return m_bliptime;
}

void CODAC_Controller::pid_set_startTime(double time)
{
	double prev_start = fb_start_time;
	fb_start_time = time;
//	printf(">>fb start time, prev: %lf, curr: %lf\n", prev_start, fb_start_time);
}

void CODAC_Controller::pid_set_stopTime(double time)
{
	double prev_stop = fb_stop_time;
		
	fb_stop_time = time;
	fb_period = fb_stop_time - fb_start_time;
//	printf(">>fb stop time, prev: %lf, curr: %lf, period: %lf\n", prev_stop, fb_stop_time, fb_period);
}

int CODAC_Controller::pid_set_sectEndTime(int sect, double time)
{
	if( sect > MAX_REF_SECTION) {
		printf("section limit overflow! request:%d\n", sect);
		return ERROR;
	}
	ST_Ref_step[sect].x2 = time;
//	ref_step_time[sect] = time;
	return OK;
}

int CODAC_Controller::pid_set_sectEndDensity(int sect, double density)
{
	if( sect > MAX_REF_SECTION) {
		printf("section limit overflow! request:%d\n", sect);
		return ERROR;
	}
	ST_Ref_step[sect].y2 = density;
//	ref_step_density[sect] = density;
	return OK;
}

int CODAC_Controller::pid_set_sectCounter(__u32 cnt)
{
	if( cnt > MAX_REF_SECTION) {
		printf("section limit overflow! request:%d\n", cnt);
		return ERROR;
	}
	ref_section_cnt = cnt;
	return OK;
}
double CODAC_Controller::pid_get_mapping_time(__u32 cnt)
{
	// first input index is zero, but it means just one setp passed. 2012. 6. 18
	
//	double tempT = cnt * m_h + m_h; 
	double tempT = cnt * m_h; 
	return ( op_start_time + tempT );
}


void CODAC_Controller::pid_set_enable(bool enable)
{
	if( m_PID_enable ) 
		printf("Previous PID enabled!\n");
	else printf("Previous PID disabled!\n");
	
	m_PID_enable = enable;
	printf(" Current PID is '%d'\n", m_PID_enable);
}

bool CODAC_Controller::pid_get_enable()
{
	return m_PID_enable;
}

bool CODAC_Controller::pid_get_enable_cnt(__u32 cnt)
{
	if( cnt > m_maxRefCnt ) return 0.0;

	return m_pRefPidEnable[cnt];
}


int CODAC_Controller::pid_make_refPattern()
{
	__u32 i;
	int sect, prev_sect;
	double total_period;

	FILE *fp;
	if((fp = fopen("make_pattern.txt", "w"))==NULL) {
		printf("\"%s\" open error!\n", "ref_data.txt");
		return ERROR;
	}
//	total_period = fb_stop_time - op_start_time;
	total_period = op_stop_time - op_start_time;
	if( m_inv_h < 1) {
		printf("Need right frequency! (%f)\n", m_inv_h );
		return ERROR;
	}
	m_maxRefCnt = (__u32)(total_period * m_inv_h); /* sec x freq */

//	printf("period: %d, freq:%d, refCnt: %d\n", fb_period, (int)m_inv_h, m_maxRefCnt);
	
	if( m_pRefData != NULL ) free(m_pRefData);
	m_pRefData = (double *)malloc(sizeof(double)*m_maxRefCnt);
	if( m_pRefData == NULL ) {
		printf("pid_make m_pRefData, malloc error!Total cnt: %d\n", m_maxRefCnt);
		return ERROR;
	}

	if( m_pRefPidEnable != NULL ) free(m_pRefPidEnable);
	m_pRefPidEnable = (bool *)malloc(sizeof(bool)*m_maxRefCnt);
	if( m_pRefPidEnable == NULL ) {
		printf("pid_make m_pRefPidEnable, malloc error!Total cnt: %d\n", m_maxRefCnt);
		return ERROR;
	}

	for( i=0; i < ref_section_cnt; i++) 
	{
		ST_Ref_step[i].matchingCnt = (__u32)((ST_Ref_step[i].x2 - (double)op_start_time) * m_inv_h +0.5);
		printf("matching count: %d, for sect: %d\n", ST_Ref_step[i].matchingCnt, i);
	}


//	ST_Ref_step[0].a = (ST_Ref_step[0].y2 - 0.0 ) / (ST_Ref_step[0].x2 - 0.0 ) ;
	ST_Ref_step[0].a = 0.0;
	for(i=1; i<ref_section_cnt; i++)
		ST_Ref_step[i].a = (ST_Ref_step[i].y2 - ST_Ref_step[i-1].y2 ) / (ST_Ref_step[i].x2 - ST_Ref_step[i-1].x2 ) ;

	m_pRefData[0] = 0.0;
	m_pRefPidEnable[0] = false;
	prev_sect = 0;

	for(i=0; i<m_maxRefCnt; i++)
	{
		sect = pid_find_section( i );

		if( sect == 0 ) {
			m_pRefData[i] = 0.0; // fill zero until meet second section.
			m_pRefPidEnable[i] = false;				
			fprintf(fp, "%d sect:%d, %lf \n", i, sect, m_pRefData[i]);
		} else {
			m_pRefData[i] = ST_Ref_step[sect].a * m_h + m_pRefData[i-1];
			// margine for floating point... 
			if( (fb_start_time - 0.000001) <= pid_get_mapping_time(i) && pid_get_mapping_time(i) <= (fb_stop_time + 0.000001) )
				m_pRefPidEnable[i] = true;
			else
				m_pRefPidEnable[i] = false;

			fprintf(fp, "%d sect:%d, %lf : mapping:%lf, on/off: %d\n", i, sect, m_pRefData[i], pid_get_mapping_time(i),m_pRefPidEnable[i] );
/*
			if( prev_sect != sect )
			{
				m_pRefData[i] = ST_Ref_step[sect].y2;
			}
			prev_sect = sect;
*/
		}
	}


#if 0
	FILE *fp;
    if((fp = fopen("make_pattern.txt", "w"))==NULL) {
        printf("\"%s\" open error!\n", "ref_data.txt");
        return ERROR;
    }
	for(i=0; i<m_maxRefCnt; i++)
		fprintf(fp, "%d   %lf\n", i, m_pRefData[i]);

	fclose(fp);
#endif

	m_status |= STATE_MAKE_PATTERN;


	return OK;
}
#if 0
int CODAC_Controller::pid_find_section(__u32 inCnt)
{
	int sect = 0;
	__u32 i;
/*
	if( inCnt <10 )
		printf("inCnt: %d: %f, %d,  %f, pid_find_section: %f\n", inCnt, ST_Ref_step[0].x2, fb_start_time, m_inv_h,
		((double)(ST_Ref_step[0].x2 - fb_start_time) * m_inv_h));
*/
/*	if( inCnt < ((ST_Ref_step[0].x2 - fb_start_time) * m_inv_h) )
	{
		return (-1);
	}
*/
	for( i=0; i < ref_section_cnt; i++) 
	{
		if( inCnt < (__u32)((ST_Ref_step[i].x2 - (double)fb_start_time) * m_inv_h +0.5) ) 
		{ 
			sect = (int)i;
			break;
		}
	}
	return sect;
}
#endif

int CODAC_Controller::pid_find_section(__u32 inCnt)
{
	int sect = 0;
	__u32 i;
/*	
	for( i=0; i < ref_section_cnt; i++) 
	{
		if( inCnt <= (__u32)((ST_Ref_step[i].x2 - (double)op_start_time) * m_inv_h +0.5) ) 
		{ 
			sect = (int)i;
			break;
		}
	}
*/
	for( i=0; i < ref_section_cnt; i++) 
	{
		if( inCnt <= ST_Ref_step[i].matchingCnt  )
		{
			sect = (int)i;
			break;
		}
	}
	
	return sect;
}

void CODAC_Controller::save_ref_data()
{
	FILE *fp;
	__u32 i;

	if((fp = fopen("ref_data.txt", "w"))==NULL) {
		printf("\"%s\" open error!\n", "ref_data.txt");
		return ;
	}

	for(i=0; i<ref_section_cnt; i++)
		fprintf(fp, "sect:%d a: %lf, (%lf, %lf) / %d \n", i, ST_Ref_step[i].a, ST_Ref_step[i].x2, ST_Ref_step[i].y2, (int)((ST_Ref_step[i].x2 - (double)fb_start_time) * m_inv_h +0.5) ); 
		
		

	for( i=0; i<m_maxRefCnt; i++)
		fprintf(fp, "%d %lf %d, %f \n", i, m_pRefData[i], m_pRefPidEnable[i], pid_get_mapping_time(i));

	fclose(fp);

}


