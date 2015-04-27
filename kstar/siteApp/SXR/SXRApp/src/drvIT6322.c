#include "sfwCommon.h"
#include "sfwMDSplus.h"
#include <unistd.h> /* UNIX standard function definitions */
#include <fcntl.h> /* File control definitions */
#include <errno.h> /* Error number definitions */
#include <termios.h>
#include <stdlib.h>



#include "drvIT6322.h"
#include <NIDAQmx.h>



static long drvIT6322_io_report(int level);
static long drvIT6322_init_driver();




struct {
        long            number;
        DRVSUPFUN       report;
        DRVSUPFUN       init;
} drvIT6322 = {
       2,
       drvIT6322_io_report,
       drvIT6322_init_driver
};

epicsExportAddress(drvet, drvIT6322);

ST_IT6322_dev* create_IT6322_taskConfig(char *arg, char *arg1, char *arg2)
{

	ST_IT6322_dev *pIT6322 = NULL;

        pIT6322 = (ST_IT6322_dev*) malloc(sizeof(ST_IT6322_dev));
        if(!pIT6322) return pIT6322;
	

/************************************* for NI-scope */
        pIT6322->myStatusDev = 0;
		pIT6322->baud_rate = 4800;
		pIT6322->change_option = NULL;
				
/************************************* for MDSplus */
		pIT6322->SENSOR_BLOCK1_MAX_TEMP = 40.0;
		pIT6322->SENSOR_BLOCK2_MAX_TEMP = 40.0;
		pIT6322->SENSOR_BLOCK3_MAX_TEMP = 40.0;
		pIT6322->SENSOR_BLOCK4_MAX_TEMP = 40.0;				

        return pIT6322;

}


static long drvIT6322_io_report(int level)
{

        return 0;
}
static long drvIT6322_init_driver(void)
{
        return 0;

}
void threadFunc_Power_Supply_IT6322(void *param)
{
	int n, m;
	int fd1, fd2, dev_fd; /* File descriptor for the port */
	int res1, res2;
	char buf1[255], buf2[255], write_buf1[255], write_buf2[255], conn_buf1[30], conn_buf2[30];
	
	ST_STD_device *pSTDdev = (ST_STD_device*) param;
	ST_IT6322_dev *pIT6322 = pSTDdev->pUser;
		
	struct termios options1;
	struct termios options2;

	int   DC1_Comm_status = 0;
	int   DC2_Comm_status = 0;

	int DC1_Comm_cnt = 0;
	int DC2_Comm_cnt = 0;

	int DC1_Conn_cnt = 0;
	int DC2_Conn_cnt = 0;
	float tmp_val = 0;
	
	fd1 = NULL;
	fd2 = NULL;

	while(1)
		{

//		system("ls /dev/ | grep ttyUSB > dev_list.txt");
		epicsThreadSleep(0.2);
/*		memset(conn_buf1, 0, sizeof(conn_buf1));
		memset(conn_buf2, 0, sizeof(conn_buf2));
		dev_fd = fopen("dev_list.txt", "rt+");
		sprintf(conn_buf1, "%s", "/dev/");
		sprintf(conn_buf2, "%s", "/dev/");
		fscanf(dev_fd, "%s", &conn_buf1[5]);
		fscanf(dev_fd, "%s", &conn_buf2[5]);
		if((strcmp(conn_buf1, "/dev/") == 0) || (strcmp(conn_buf2, "/dev/") == 0))
		printlog("Error Logging Lost DC Power Supply %s, %s\n", conn_buf1, conn_buf2 );
		close(dev_fd);
		fflush(dev_fd);

*/
/* ========================================================== */				
//		Power Supply Configure
/* ========================================================== */				
			while(pIT6322->change_option)
				{
/*

								system("ls /dev/ | grep ttyUSB > dev_list.txt");
								epicsThreadSleep(0.5);
								dev_fd = fopen("dev_list.txt", "rt+");
								sprintf(conn_buf1, "%s", "/dev/");
								sprintf(conn_buf2, "%s", "/dev/");
								fscanf(dev_fd, "%s", &conn_buf1[5]);
								fscanf(dev_fd, "%s", &conn_buf2[5]);
								epicsPrintf("%s %s \n", conn_buf1, conn_buf2);
		close(dev_fd);
		fflush(dev_fd);
*/
								
				
								fd1 = open("/dev/ttyS0", O_RDWR | O_NOCTTY);
				
								if (fd1 == -1)
									{
										epicsPrintf("open_port: Unable to open /dev/ttyUSB0 ");
										fd1 = NULL;
										break;
									}
				
								fd2 = open("/dev/ttyS1", O_RDWR | O_NOCTTY);
				
								if (fd2 == -1)
									{
										epicsPrintf("open_port: Unable to open /dev/ttyUSB1 ");
										if(fd1)
											{
												close(fd1);
												fd1 = NULL;
											}
										fd2 = NULL;
										break;
									}
									
								memset(&options1, 0, sizeof(struct termios));
								memset(&options2, 0, sizeof(struct termios));
							
								cfsetispeed(&options1, B4800);
								cfsetospeed(&options1, B4800);
								cfsetispeed(&options2, B4800);
								cfsetospeed(&options2, B4800);

								options1.c_cflag |= ( CLOCAL | CREAD);
							        options1.c_cc[VTIME]=20;
							        options1.c_cc[VMIN]=7;								
							
								options2.c_cflag |= ( CLOCAL | CREAD);
							        options2.c_cc[VTIME]=20;
							        options2.c_cc[VMIN]=7;
	
								cfmakeraw(&options1);
								cfmakeraw(&options2);
								tcflush (fd1, TCIFLUSH);
								tcsetattr(fd1, TCSANOW, &options1);
								tcflush (fd2, TCIFLUSH);
								tcsetattr(fd2, TCSANOW, &options2);
/*				do {

					res1 = 0;
					res1 = 0;
					epicsThreadSleep(0.5);
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					epicsPrintf("%s\n", buf1);
					memset(buf1, 0, sizeof(buf1));
					fflush(stdout);
					fflush(stdin);					
					
					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					epicsPrintf("%s\n", buf2);
					memset(buf2, 0, sizeof(buf2));
					fflush(stdout);					
					fflush(stdin);										
				}while((res1 > 0) || (res2 > 0));

*/
/********************************/				
//	           DC1_CH1_VOLT               //
/********************************/

				if(pIT6322->change_option & CHANGE_DC1_CH1_VOLT) {

					n = write(fd1, "INST FIR\r", 9);
					fflush(stdout);
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}
					;
					memset(buf1, 0, sizeof(buf1));
	
					n = write(fd1, "INST?\r", 6);
					fflush(stdout);	
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
								break;
						}
					epicsThreadSleep(0.5);
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);

					buf1[res1]= 0;

	
					if(strcmp(&buf1[0], "F") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
	
					if(DC1_Comm_cnt == 3)
						{
				//			epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;

							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));

					sprintf(write_buf1, "%s", "VOLT ");
					gcvt(pIT6322->DC1_CH1_Voltage, 3, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);

					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout);					
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					fflush(stdout);

					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));

					n = write(fd1, "VOLT?\r", 6);
					fflush(stdout);
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
								break;
						}
					epicsThreadSleep(0.5);
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);

					if(res1 > 0)
						buf1[res1] = 0;

					tmp_val = atof(buf1);
				
					epicsPrintf	("%d %s %f %f\n", res1, buf1, pIT6322->DC1_CH1_Voltage, tmp_val);
					fflush(stdout);
					if(tmp_val == pIT6322->DC1_CH1_Voltage)
						{

							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH1_VOLT;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
							

					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
	
					if(DC1_Comm_cnt == 3)
						{
				//			epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;

							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
							break;
						}
					memset(buf1, 0, sizeof(buf1));

					}

/********************************/				
//		  DC1_CH2_VOLT    		  //
/********************************/
					
				if(pIT6322->change_option & CHANGE_DC1_CH2_VOLT) {

					
					n = write(fd1, "INST SECO\r", 10);
					fflush(stdout);
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf1, 0, sizeof(buf1));
				
					n = write(fd1, "INST?\r", 6);
					fflush(stdout);				
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
								break;
						}
					epicsThreadSleep(0.5);					 				
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
				
					buf1[res1]= 0;
				
					if(strcmp(&buf1[0], "S") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
				
					if(DC1_Comm_cnt == 3)
						{
				//			epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
				
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
				
					sprintf(write_buf1, "%s", "VOLT ");
					gcvt(pIT6322->DC1_CH2_Voltage, 3, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);
					
					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout);					
					if (n < 0)
						{
				//			fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					fflush(stdout);
				
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
				
					n = write(fd1, "VOLT?\r", 6);
					fflush(stdout);				
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					tmp_val = atof(buf1);
					
					epicsPrintf ("%d %s %f %f\n", res1, buf1, pIT6322->DC1_CH2_Voltage, tmp_val);
					fflush(stdout);
					if(tmp_val == pIT6322->DC1_CH2_Voltage)
						{
				
							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH2_VOLT;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
				
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
				
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
				
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					memset(buf1, 0, sizeof(buf1));

				}

/********************************/				
//		  DC1_CH3_VOLT		       //
/********************************/

/* Not   */
					
				if(pIT6322->change_option & CHANGE_DC1_CH3_VOLT) {

					
					n = write(fd1, "INST THI\r", 9);
					fflush(stdout);								
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
							
					n = write(fd1, "INST?\r", 6);
					fflush(stdout);				
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					
					buf1[res1]= 0;
				
					if(strcmp(&buf1[0], "T") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					sprintf(write_buf1, "%s", "VOLT ");
					gcvt(pIT6322->DC1_CH3_Voltage, 3, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);
								
					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout);								
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					fflush(stdout);
							
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					n = write(fd1, "VOLT?\r", 6);
					fflush(stdout);							
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					tmp_val = atof(buf1);
								
					epicsPrintf ("%d %s %f %f\n", res1, buf1, pIT6322->DC1_CH3_Voltage, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC1_CH3_Voltage)
						{
							
							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH3_VOLT;
							pIT6322->DC1_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

							
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf1, 0, sizeof(buf1));

				}
/********************************/				
//	           DC2_CH1_VOLT               //
/********************************/

				if(pIT6322->change_option & CHANGE_DC2_CH1_VOLT) {

					m = write(fd2, "INST FIR\r", 9);
					fflush(stdout);

					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf2, 0, sizeof(buf1));
	
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);	
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);

					buf2[res2]= 0;

	
					if(strcmp(&buf2[0], "F") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
	
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;

							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));

					sprintf(write_buf2, "%s", "VOLT ");
					gcvt(pIT6322->DC2_CH1_Voltage, 3, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);

					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);						
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					fflush(stdout);

					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));

					m = write(fd2, "VOLT?\r", 6);
					fflush(stdout);	
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
								break;
						}
					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = atof(buf2);
				
					epicsPrintf	("%d %s %f %f\n", res2, buf2, pIT6322->DC2_CH1_Voltage, tmp_val);
					fflush(stdout);
					if(tmp_val == pIT6322->DC2_CH1_Voltage)
						{

							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH1_VOLT;
							pIT6322->DC2_Comm_status = 1;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			

							

					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
	
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;

							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
							break;
						}
					memset(buf2, 0, sizeof(buf2));


					}

/********************************/				
//		  DC2_CH2_VOLT    		  //
/********************************/
					
				if(pIT6322->change_option & CHANGE_DC2_CH2_VOLT) {

					
					m = write(fd2, "INST SECO\r", 10);
					fflush(stdout);						
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					memset(buf2, 0, sizeof(buf2));
				
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}
				
					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
				
					buf2[res2]= 0;
				
					if(strcmp(&buf2[0], "S") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
				
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
				
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
				
					sprintf(write_buf2, "%s", "VOLT ");
					gcvt(pIT6322->DC2_CH2_Voltage, 3, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);
					
					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);						
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					fflush(stdout);
				
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
				
					m = write(fd2, "VOLT?\r", 6);
					fflush(stdout);					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = atof(buf2);
					
					epicsPrintf ("%d %s %f %f\n", res2, buf2, pIT6322->DC2_CH2_Voltage, tmp_val);
					fflush(stdout);
					if(tmp_val == pIT6322->DC2_CH2_Voltage)
						{
				
							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH2_VOLT;
							pIT6322->DC2_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
				
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
				
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
				
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					memset(buf2, 0, sizeof(buf2));

					}

/********************************/				
//		  DC2_CH3_VOLT			  //
/********************************/
					
				if(pIT6322->change_option & CHANGE_DC2_CH3_VOLT) {

					
					m = write(fd2, "INST THI\r", 9);
					fflush(stdout);									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf2, 0, sizeof(buf2));
							
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 					 						
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					
					buf2[res2]= 0;
				
					if(strcmp(&buf2[0], "T") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					sprintf(write_buf2, "%s", "VOLT ");
					gcvt(pIT6322->DC2_CH3_Voltage, 3, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);
								
					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;

					 									}
					fflush(stdout);
							
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					m = write(fd2, "VOLT?\r", 6);
					fflush(stdout);								
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = atof(buf2);
								
					epicsPrintf ("%d %s %f %f\n", res2, buf2, pIT6322->DC2_CH3_Voltage, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC2_CH3_Voltage)
						{
							
							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH3_VOLT;
							pIT6322->DC2_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf2, 0, sizeof(buf2));

					}

/********************************/				
//		  DC1_CH1_ON_OFF		  //
/********************************/
					
				if(pIT6322->change_option & CHANGE_DC1_CH1_ON_OFF) {

					
					n = write(fd1, "INST FIR\r", 9);
					fflush(stdout);									
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}
					 
					memset(buf1, 0, sizeof(buf1));
					
									
					n = write(fd1, "INST?\r", 6);
					fflush(stdout); 												
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
								
					epicsThreadSleep(0.5);										 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
							
					buf1[res1]= 0;
							
					if(strcmp(&buf1[0], "F") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					sprintf(write_buf1, "%s", "OUTP ");
					gcvt(pIT6322->DC1_CH1_ON_OFF, 1, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);
								
					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout); 																
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
					
					 
					fflush(stdout);
							
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					n = write(fd1, "OUTP?\r", 6);
					fflush(stdout); 															
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);										 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					tmp_val = (int)buf1[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf1[0], res1, buf1, pIT6322->DC1_CH1_ON_OFF, tmp_val);
					fflush(stdout);
					
					if(tmp_val == pIT6322->DC1_CH1_ON_OFF)
						{
								
							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH1_ON_OFF;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf1, 0, sizeof(buf1));

					}				
/**********************************/				
//		  DC1_CH2_ON_OFF		  //
/********************************/



				if(pIT6322->change_option & CHANGE_DC1_CH2_ON_OFF) {

					
					n = write(fd1, "INST SECO\r", 10);
					fflush(stdout);																	
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf1, 0, sizeof(buf1));
							
					n = write(fd1, "INST?\r", 6);
					fflush(stdout);													
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
						
					epicsThreadSleep(0.5);					 					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					
					buf1[res1]= 0;
				
					if(strcmp(&buf1[0], "S") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					sprintf(write_buf1, "%s", "OUTP ");
					gcvt(pIT6322->DC1_CH2_ON_OFF, 1, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);
								
					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout);																	
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					fflush(stdout);
							
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					n = write(fd1, "OUTP?\r", 6);
					fflush(stdout);																
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					tmp_val = (int)buf1[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf1[0], res1, buf1, pIT6322->DC1_CH2_ON_OFF, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC1_CH2_ON_OFF)
						{
							
							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH2_ON_OFF;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf1, 0, sizeof(buf1));

					}				

/********************************/				
//		  DC1_CH3_ON_OFF		  //
/********************************/


				if(pIT6322->change_option & (CHANGE_DC1_CH3_ON_OFF)) {

					
					n = write(fd1, "INST THI\r", 9);
					fflush(stdout);																	
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 					
					memset(buf1, 0, sizeof(buf1));
							
					n = write(fd1, "INST?\r", 6);
					fflush(stdout);													
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 						
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					
					buf1[res1]= 0;
				
					if(strcmp(&buf1[0], "T") == 0)
						{
							DC1_Comm_cnt = 0;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					sprintf(write_buf1, "%s", "OUTP ");
					gcvt(pIT6322->DC1_CH3_ON_OFF, 1, &write_buf1[5]);
					write_buf1[strlen(write_buf1)] = 13;
					epicsPrintf("%s\n", write_buf1);
					fflush(stdout);
								
					n = write(fd1, write_buf1, strlen(write_buf1));
					fflush(stdout);																	
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
									}

					 
					fflush(stdout);
							
					memset(buf1, 0, sizeof(buf1));
					memset(write_buf1, 0, sizeof(write_buf1));
							
					n = write(fd1, "OUTP?\r", 6);
					fflush(stdout);																
					if (n < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd1, F_SETFL, FNDELAY);
					res1 = read(fd1,buf1,255);
					tmp_val = (int)buf1[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf1[0], res1, buf1, pIT6322->DC1_CH1_ON_OFF, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC1_CH3_ON_OFF)
						{
							
							DC1_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC1_CH3_ON_OFF;
							pIT6322->DC1_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
							
					} else {
							DC1_Comm_cnt++;
							pIT6322->DC1_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC1_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf1, 0, sizeof(buf1));

					}				


/********************************/				
//		  DC2_CH1_ON_OFF		  //
/********************************/


				if(pIT6322->change_option & CHANGE_DC2_CH1_ON_OFF) {

					
					m = write(fd2, "INST FIR\r", 9);
					fflush(stdout);																	
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf2, 0, sizeof(buf2));
							
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);																					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
						
					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					
					buf2[res2]= 0;
				
					if(strcmp(&buf2[0], "F") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;							
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					sprintf(write_buf2, "%s", "OUTP ");
					gcvt(pIT6322->DC2_CH1_ON_OFF, 1, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);
								
					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);																									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
									}

					 
					fflush(stdout);
							
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					m = write(fd2, "OUTP?\r", 6);
					fflush(stdout);																								
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = (int)buf2[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf2[0], res2, buf2, pIT6322->DC2_CH1_ON_OFF, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC2_CH1_ON_OFF)
						{
							
							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH1_ON_OFF;
							pIT6322->DC2_Comm_status = 1;														
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf2, 0, sizeof(buf2));

					}				


/********************************/				
//		  DC2_CH2_ON_OFF		  //
/********************************/


				if(pIT6322->change_option & CHANGE_DC2_CH2_ON_OFF) {

					
					m = write(fd2, "INST SECO\r", 10);
					fflush(stdout);																									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf2, 0, sizeof(buf2));
							
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);																					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 					 						
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					
					buf2[res2]= 0;
				
					if(strcmp(&buf2[0], "S") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;														
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					sprintf(write_buf2, "%s", "OUTP ");
					gcvt(pIT6322->DC2_CH2_ON_OFF, 1, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);
								
					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);																									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					fflush(stdout);
							
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					m = write(fd2, "OUTP?\r", 6);
					fflush(stdout);																								
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}

					epicsThreadSleep(0.5);					 					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = (int)buf2[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf2[0], res2, buf2, pIT6322->DC2_CH2_ON_OFF, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC2_CH2_ON_OFF)
						{
							
							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH2_ON_OFF;
							pIT6322->DC2_Comm_status = 1;														
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf2, 0, sizeof(buf2));

					}				

/********************************/				
//		  DC2_CH3_ON_OFF		  //
/********************************/



				if(pIT6322->change_option & CHANGE_DC2_CH3_ON_OFF) {

					
					m = write(fd2, "INST THI\r", 9);
					fflush(stdout);																									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
		
							close(fd2);
							fd2 = NULL;
							break;
						}

					 
					memset(buf2, 0, sizeof(buf2));
							
					m = write(fd2, "INST?\r", 6);
					fflush(stdout);																					
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					
					buf2[res2]= 0;
				
					if(strcmp(&buf2[0], "T") == 0)
						{
							DC2_Comm_cnt = 0;
							pIT6322->DC2_Comm_status = 1;														
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
							
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
						}
					fflush(stdout);
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					sprintf(write_buf2, "%s", "OUTP ");
					gcvt(pIT6322->DC2_CH3_ON_OFF, 1, &write_buf2[5]);
					write_buf2[strlen(write_buf2)] = 13;
					epicsPrintf("%s\n", write_buf2);
					fflush(stdout);
								
					m = write(fd2, write_buf2, strlen(write_buf2));
					fflush(stdout);																									
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
								
							close(fd2);
							fd2 = NULL;
							break;
						}

					fflush(stdout);
							
					memset(buf2, 0, sizeof(buf2));
					memset(write_buf2, 0, sizeof(write_buf2));
							
					m = write(fd2, "OUTP?\r", 6);
					fflush(stdout);																								
					if (m < 0)
						{
//							fputs("write() failed!\n", stderr);
							close(fd1);
							fd1 = NULL;
									
							close(fd2);
							fd2 = NULL;
							break;
						}
					epicsThreadSleep(0.5);					 					
					fcntl(fd2, F_SETFL, FNDELAY);
					res2 = read(fd2,buf2,255);
					tmp_val = (int)buf2[0] - 48;
								
					epicsPrintf ("%c %d %s %d %d\n", buf2[0], res2, buf2, pIT6322->DC2_CH3_ON_OFF, tmp_val);
					fflush(stdout);

					if(tmp_val == pIT6322->DC2_CH3_ON_OFF)
						{
							
							DC2_Comm_cnt = 0;
							pIT6322->change_option &= ~CHANGE_DC2_CH3_ON_OFF;
							pIT6322->DC2_Comm_status = 1;														
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 										
							
					} else {
							DC2_Comm_cnt++;
							pIT6322->DC2_Comm_status = 0;
							scanIoRequest(pSTDdev->ioScanPvt_userCall); 			
					}
							
					if(DC2_Comm_cnt == 3)
						{
//							epicsPrintf("Power_Supply Connection Fail.. Try again.. \n");
							close(fd1);
							fd1 = NULL;
						
							close(fd2);
							fd2 = NULL;
							DC1_Comm_cnt = 0;
							DC2_Comm_cnt = 0;
							break;
					}
					memset(buf2, 0, sizeof(buf2));

					}				
				close(fd1);
				close(fd2);				

				}

		}
		
}


