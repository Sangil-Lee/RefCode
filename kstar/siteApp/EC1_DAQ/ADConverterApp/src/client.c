#include "client.h"
#include <unistd.h>        // write(), close()
#include <fcntl.h>         // O_WRONLY

///////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////
void delay_handler() { /** NO PROCEDURE REQUIRED **/ }

void delay(float fsec)
{
    struct itimerval rtm;
    signal(SIGALRM, (void(*)(int))delay_handler);
    rtm.it_value.tv_sec     = (long)fsec;
    rtm.it_value.tv_usec    = 1000000. * (fsec - rtm.it_value.tv_sec);
    rtm.it_interval.tv_sec  = 0;
    rtm.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_REAL,&rtm,0) == -1 ){
        return;
    }
    pause();
}

int ReadRTDFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(RTDTag, 0x00, sizeof(RTDTag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/RTDTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		//puts( buff);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				RTDTag[iTagCnt] = (char*)TagName;				

				ca_search(RTDTag[iTagCnt], &RTDchid[iTagCnt]);
				istatus = ca_pend_io(0.1);
printf("RTDTag[%d] : %s Search[%d] : %d\n", iTagCnt, RTDTag[iTagCnt], iTagCnt, istatus);
				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "RTD Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int ReadFRFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(FRTag, 0x00, sizeof(FRTag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/FlowRateTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				FRTag[iTagCnt] = (char*)TagName;				

ca_search(FRTag[iTagCnt], &FRchid[iTagCnt]);
istatus = ca_pend_io(0.1);
printf("FRTag[%d] : %s Search[%d] : %d\n", iTagCnt, FRTag[iTagCnt], iTagCnt, istatus);

				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "Flow Rate Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int ReadTTFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(TTTag, 0x00, sizeof(TTTag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/TCDeltaTTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				TTTag[iTagCnt] = (char*)TagName;				

ca_search(TTTag[iTagCnt], &TTchid[iTagCnt]);
istatus = ca_pend_io(0.1);
printf("TTTag[%d] : %s Search[%d] : %d\n", iTagCnt, TTTag[iTagCnt], iTagCnt, istatus);

				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "TC/DeltaT Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int ReadSEFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(SETag, 0x00, sizeof(SETag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/SetDataTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				SETag[iTagCnt] = (char*)TagName;				

ca_search(SETag[iTagCnt], &SEchid[iTagCnt]);
istatus = ca_pend_io(0.1);
printf("SETag[%d] : %s Search[%d] : %d\n", iTagCnt, SETag[iTagCnt], iTagCnt, istatus);

				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "SetData Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int ReadEtcFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(EtcTag, 0x00, sizeof(EtcTag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/EtcTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				EtcTag[iTagCnt] = (char*)TagName;				

ca_search(EtcTag[iTagCnt], &Etcchid[iTagCnt]);
istatus = ca_pend_io(0.1);
printf("EtcTag[%d] : %s Search[%d] : %d\n", iTagCnt, EtcTag[iTagCnt], iTagCnt, istatus);

				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "Etc Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int ReadTDFile()
{
	int	i;
	char  buff[BUFF_SIZE];
   	int   fd;
	int	iSize = 0; 
	char	TagName[70];
	int	iTagCnt = 0;
	int 	iBufCnt = 0;

	memset(buff, 0x00, sizeof(buff));
	memset(TDTag, 0x00, sizeof(TDTag));
	memset(TagName, 0x00, sizeof(TagName));

	if ( 0 < ( fd = open( "../ReadTagFile/TlDumLauTag.txt", O_RDONLY)))
	{
		iSize = read( fd, buff, BUFF_SIZE);
		close( fd);

		for(i=0;i<iSize;i++)
		{
			//Enter Check
			if(buff[i] == ',' || i == iSize-1)
			{
				iBufCnt = 0;
				TDTag[iTagCnt] = (char*)TagName;				

ca_search(TDTag[iTagCnt], &TDchid[iTagCnt]);
istatus = ca_pend_io(0.1);
printf("TDTag[%d] : %s Search[%d] : %d\n", iTagCnt, TDTag[iTagCnt], iTagCnt, istatus);

				memset(TagName, 0x00, sizeof(TagName));
				iTagCnt++;
			}
			else
			{
				if (iBufCnt < 70 && buff[i] >= 0x30)
				{
					TagName[iBufCnt] = buff[i];
					iBufCnt++;	
				}
				
			}
		}

	}
	else
	{
		printf( "TL/TC Dummy2 Launcher Data File Read Fail....\n");
		return 0;
	}

	return 1;
}

int main(int argc, char* argv[])
{

	//int i = 0;
	//RTDTag Read from File...
	if (ReadRTDFile() < 1)
	{
		printf("RTD File read fail....\n");
		return 0;
	}

	if (ReadFRFile() < 1)
	{
		printf("FlowRate File read fail....\n");
		return 0;
	}

	if (ReadTTFile() < 1)
	{
		printf("TC/DeltaT File read fail....\n");
		return 0;
	}

	if (ReadSEFile() < 1)
	{
		printf("SetData File read fail....\n");
		return 0;
	}

	if (ReadEtcFile() < 1)
	{
		printf("Etc File read fail....\n");
		return 0;
	}

	if (ReadTDFile() < 1)
	{
		printf("T/L DummyLoad2 Launcher File read fail....\n");
		return 0;
	}
	
	initsocket();
	while(1)
	{
		LabVIEWCheck();
		//delay(0.1);
	}
	return 0;
}

void initsocket()
{
	struct hostent *hpLabVIEW = NULL;
	struct sockaddr_in sinLabVIEW;

	char *hostLabVIEW = "192.168.0.130";

	struct linger ling;
	int opt = 1;
	ling.l_onoff  = 1;
	ling.l_linger = 0;


	//printf("LabView Socket Start!!!\n");
	hpLabVIEW = gethostbyname( hostLabVIEW);
	if( !hpLabVIEW ) 
	{
		fprintf(stderr, "LabViewPC: unknown host: %s\n", hostLabVIEW);
		//exit(EXIT_FAILURE);
	}

	// build address data structure
	bzero( (char*)&sinLabVIEW, sizeof(sinLabVIEW) );
	sinLabVIEW.sin_family = AF_INET;
	bcopy( hpLabVIEW->h_addr, (char*)&sinLabVIEW.sin_addr, hpLabVIEW->h_length );
	sinLabVIEW.sin_port = htons( 6340 );
	
	// active open

	if( ( skLabVIEW = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "Socket Open error...!!" );
		//exit( EXIT_FAILURE );
	}

	if( connect( skLabVIEW, (struct sockaddr*)&sinLabVIEW, sizeof(sinLabVIEW) ) < 0 ){
		perror( "LabView : Not connect!!!" );
		close(skLabVIEW);
		perror( "Connection Close..." );
		//exit(EXIT_FAILURE);
	}
	setsockopt(skLabVIEW, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
	setsockopt(skLabVIEW, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	printf("LabView Connection Success!!!\n");

}

int ReadTag(cUINT utype, chid searchID)
{
	cUINT type;

	switch(utype)
	{
		case T_CIP_INT:
			type = DBR_INT;
			ca_get(type, searchID, &iDATA);
			break;
		case T_CIP_DINT:
			type = DBR_DOUBLE;
			ca_get(type, searchID, &dblDATA);
			break;
		case T_CIP_REAL:
			type = DBR_FLOAT;
			ca_get(type, searchID, &fltDATA);
			break;
		case T_CIP_BITS:
			type = T_CIP_DINT;
			ca_get(type, searchID, &fltDATA);
			break;
	}
	istatus = ca_pend_io(0.1);
	return 1;
}

void writetag(cUINT utype, chid searchID)
{
	cUINT  type;

	switch (utype)
	{
		case T_CIP_INT:
			type   = DBR_INT;
			ca_put(type, searchID, &iWriteDATA);
			break;

		case T_CIP_DINT:
			type   = DBR_DOUBLE;
			ca_put(type, searchID, &dblWriteDATA);
			break;

		case T_CIP_BITS:
			type   = DBR_INT;
			if (bDATA%2==0)
				bDATA=0;
			else
			bDATA=255;
			ca_put(type, searchID, &bWriteDATA);
			break;

		case T_CIP_REAL:
			type   = DBR_FLOAT;
			ca_put(type, searchID, &fltWriteDATA);
			break;
	}//end switch
	istatus = ca_pend_io(0.1);
}

void LabVIEWCheck()
{
	delay(0.5);
	int iChk = -1;	
	int iSize = 0;
	char sendtemp1[8];
	char sendtemp2[8];
	char temp1[5];
	char temp2[4];
	char temp3[5];
	char temp4[4];
	char chktemp[8];
	char senddata[20];
	int iCon = 0; 
	float flt6220T0=0;
	float flt6220T1=0;

	//EC1_DAQ_6220_T0
	fltDATA=0;
	ReadTag(T_CIP_REAL, Etcchid[5]);
	flt6220T0 = fltDATA;
	iCon = fltDATA;
	memset(temp1,0x00, sizeof(temp1));
	memset(temp2,0x00, sizeof(temp2));
	memset(sendtemp1,0x00, sizeof(sendtemp1));
	sprintf(temp1, "%04d", iCon);
	sprintf(chktemp, "%04.3f", flt6220T0);
	iSize = strlen(chktemp);
	sprintf(temp2, "%c%c%c%c", chktemp[iSize-4], chktemp[iSize-3], chktemp[iSize-2], chktemp[iSize-1] );
	sprintf(sendtemp1, "%s%s", temp1, temp2);	
	sprintf(senddata, "SEND%s", sendtemp1);	

	//EC1_DAQ_6220_T1
	fltDATA=0;
	ReadTag(T_CIP_REAL, Etcchid[6]);
	flt6220T1 = fltDATA;
	iCon = fltDATA;
	memset(temp3,0x00, sizeof(temp3));
	memset(temp4,0x00, sizeof(temp4));
	memset(sendtemp2,0x00, sizeof(sendtemp2));
	sprintf(temp3, "%04d", iCon);
	sprintf(chktemp, "%04.3f", flt6220T1);
	iSize = strlen(chktemp);
	sprintf(temp4, "%c%c%c%c", chktemp[iSize-4], chktemp[iSize-3], chktemp[iSize-2], chktemp[iSize-1] );
	sprintf(sendtemp2, "%s%s", temp3, temp4);	
//printf("sendtemp2     %s\n",sendtemp2);
	sprintf(senddata, "%s%s", senddata, sendtemp2);	


//printf("senddata     %s\n",senddata);
	//if (SocketAsk("SEND", 0) > 0)
	if (SocketAsk(senddata, 0) > 0)
	{
		//RTD Data
		iChk = LVDataRecv("RT", 2);
		if (iChk > 0)
		{
//			printf("RTD Socket Return : %d\n", iChk);
		}
		else
		{
//			printf("RTD Socket Return : %d\n", iChk);
		}
		
		//Flow Rate Data
		iChk = LVDataRecv("FR", 2);
		if (iChk > 0)
		{
//			printf("Flow Rate Socket Return : %d\n", iChk);
		}
		else
		{
//			printf("Flow Rate Socket Return : %d\n", iChk);
		}

		//TC, Delta T, DO Data
		iChk = LVDataRecv("TT", 2);
		if (iChk > 0)
		{
//			printf("TC, Delta T, DO Socket Return : %d\n", iChk);
		}
		else
		{
//			printf("TC, Delta T, DO Socket Return : %d\n", iChk);
		}

		//TC, Delta T, DO Data
		iChk = LVDataRecv("TD", 2);
//		printf("T/L TC, Launcher TC, Dummy Load2 Socket Return : %d\n", iChk);
		

		//Setting Data
		iLabRoopCnt += 1;
		ReadTag(T_CIP_INT, Etcchid[1]);
		if (iDATA < 1)
		{
	
			iLabRoopCnt = 0;
			iChk = LVDataRecv("SE", 2);
			if (iChk > 0)
			{
//				printf("Setting Data Socket Return : %d\n", iChk);
			}
			else
			{
//				printf("Setting Data Socket Return : %d\n", iChk);
			}

		}
		else
		{
			//printf("ReadTag EC1_WCS_DATA_SET : %d\n", iDATA);
			if (SocketAsk("SEDT", 1) > 0)
			{
				SendSet();
			}
		}
		
		//EC1_WCS_DT_START
		//Calc Request Check
		ReadTag(T_CIP_INT, Etcchid[2]);
		//printf("EC1_WCS_DT_START : %d\n", iDATA);
		if (iDATA > 0)
		{
			SendINTG();
		}
		
		ReadTag(T_CIP_INT, Etcchid[4]);
		//printf("EC1_WCS_DT_STOP : %d\n", iDATA);
		if (iDATA > 0)
		{
			StopINTG();
		}	
		ichkclock = 0;
	}
	else
	{
		ichkclock += 1; 
		if (ichkclock == 50) 
		{
			close(skLabVIEW);
			perror( "LabView Connection Close..." );
			initsocket();
			ichkclock = 0;
		}
	}
	
}

int LVDataRecv(char *str, int iDataSize)
{
	size_t len = 0;
	int    i = 0;
	bool   bDataStart = false;
	bool   bDataEnd = false;
	bool   bDataChk = false;
	int    iIdx = 0;
	int    iRoopCnt = 0;
	int    iChk = -1;
	int    iSize = 0;

	len = strlen( buf ) + 1;
	send( skLabVIEW, str, iDataSize, 0 );
	memset(dataBuf,0x00, sizeof(dataBuf));

	//while start
	//printf("RTDSocketSR start!!!\n");
	//while (iRoopCnt < 10)
	for (iRoopCnt=0; iRoopCnt<10; iRoopCnt++)
	{	
		if (bDataEnd == false)
		{
			//printf("For Roop Start : %d\n", iRoopCnt);	
			memset(buf, 0x00, sizeof(buf));
			iSize = recv(skLabVIEW,buf,sizeof(buf),0);
			//printf("recv : %s              %d\n",buf, iSize);

			for (i=0; i<iSize; i++)
			{
				if ( bDataStart )
				{
					//#EtherNetClientTest_LabView
					if( buf[i] == 0x23 )
					{
						bDataStart = false;
						bDataEnd = true;
					//printf("bDataEnd check : %d\n", bDataEnd);
						dataBuf[iIdx] = buf[i];
						iIdx = 0;
						
						if (str == "RT") RTDpacketParsing();
						if (str == "FR") FRpacketParsing();
						if (str == "TT") TTpacketParsing();
						if (str == "SE") SEpacketParsing();
						if (str == "TD") TDpacketParsing();

						memset(dataBuf,0x00,sizeof(dataBuf));
						//printf("Data Read Success!!!!!!\n");
						iChk = 1;
						iRoopCnt = 11;		
					}
					else
					{
						dataBuf[iIdx] = buf[i];
						iIdx++;
					}
				}
				else
				{
					//@check
					if( buf[i] == 0x40 )	
					{
						bDataStart = true;
						bDataEnd = false;
						bDataChk = false;
						iIdx = 0;
						dataBuf[iIdx] = buf[i];
					//	printf("DataStart!!!     iIdx = %d    dataBuf[0] : %c\n",iIdx, dataBuf[0]);
						iIdx++;
					}//@check
				
				}//if bDataStart	
			}//for
			//printf("dataBuf : %s        RoopCount : %d\n",dataBuf, iRoopCnt);			
		}//if bDataEnd end	
    }//while
	if (iChk != 1) printf(" Data Read Fail...\n");return 0;
	return iChk;
}

int RTDpacketParsing()
{
	bool bDataChk = true;
	float dRTDTemp[32];

	//float iStat[16];
	bool bStat[16];
	
	char cLenVal[4];
	char cAIVal[11];	
	int iStatStartCnt = 256+PREPACKET;
	int iLenVal = 0;
	char tempchk[3];
	int iFor = 0;
	int iFor2 = 0;

	//chid  searchID;
	
	cUINT  type;

//printf("RTD dataBuf : %s        Length : %d       iStatStartCnt : %d\n",dataBuf, strlen(dataBuf), iStatStartCnt);

	if (dataBuf[0] != 0x40) bDataChk = false;	
	cLenVal[0] = dataBuf[1];
	cLenVal[1] = dataBuf[2];
	cLenVal[2] = dataBuf[3];
	iLenVal = atoi(cLenVal);
	//if ( iLenVal != 333 ) bDataChk = false;			

	tempchk[0] = dataBuf[4];
	tempchk[1] = dataBuf[5];

	if (dataBuf[4] != 'R' || dataBuf[5] != 'T') bDataChk = false;//RTD Data 

	if (bDataChk == false)	return 0;

	//Data 
	memset(dRTDTemp, 0x00, sizeof(dRTDTemp));
	type   = DBR_FLOAT;
		
	for ( iFor = 0; iFor < 32; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+6)+(iFor*8)];
		}
		dRTDTemp[iFor] = atof(cAIVal);
		fltWriteDATA = dRTDTemp[iFor];
		//if (iFor == 0) printf("\n\nRTD 0 Data : %f\n\n", fltWriteDATA);
		writetag(T_CIP_REAL, RTDchid[iFor]);					
	}

	//***********Stat **************//
	type = DBR_INT;
	memset(bStat,true,sizeof(bStat));
	for ( iFor = 0; iFor < 16; iFor++ )
	{
		bStat[iFor] = true;
		if( dataBuf[iFor + iStatStartCnt] == 0x30 )	bStat[iFor] = false;
		iWriteDATA = 0;
		if (bStat[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, RTDchid[iFor+32]);
	}
	return 1;
}

int FRpacketParsing()
{
	bool bDataChk = true;
	float dFRTemp[36];

	//float iStat[16];
	bool bStat[18];
	
	char cLenVal[4];
	char cAIVal[11];	
	int iStatStartCnt = 144+PREPACKET;
	int iLenVal = 0;
	char tempchk[3];
	int iFor = 0;
	int iFor2 = 0;

	cUINT  type;

	if (dataBuf[0] != 0x40) bDataChk = false;	
	cLenVal[0] = dataBuf[1];
	cLenVal[1] = dataBuf[2];
	cLenVal[2] = dataBuf[3];
	iLenVal = atoi(cLenVal);
	//if ( iLenVal != 333 ) bDataChk = false;			

	tempchk[0] = dataBuf[4];
	tempchk[1] = dataBuf[5];

	if (dataBuf[4] != 'F' || dataBuf[5] != 'R') bDataChk = false;//FR Data 		

	if (bDataChk == false)	return 0;

	//Data 
	type = DBR_FLOAT;
	memset(dFRTemp, 0x00, sizeof(dFRTemp));

	for ( iFor = 0; iFor < 18; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+PREPACKET)+(iFor*8)];
		}
		dFRTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dFRTemp[iFor];
		writetag(T_CIP_REAL, FRchid[iFor]);
	}

	//***********Stat **************//
	memset(bStat,true,sizeof(bStat));
	type = DBR_INT;
	for ( iFor = 0; iFor < 18; iFor++ )
	{		
		bStat[iFor] = true;
		if( dataBuf[iFor + iStatStartCnt] == 0x30 )	bStat[iFor] = false;
		iWriteDATA = 0;
		if (bStat[iFor]==true) iWriteDATA = 1;
		iWriteDATA = bStat[iFor];
		writetag(T_CIP_INT, FRchid[iFor+18]);
	}
	return 1;
}

int TTpacketParsing()
{
	bool bDataChk = true;
	float dTTTemp[50];

	//float iStat[16];
	bool bDO[3];
	bool bStat[11];
	char cLenVal[4];
	char cAIVal[11];	
	int iStatStartCnt = 72+PREPACKET;
	int iDeltaTStartCnt = 81+PREPACKET;
	int iDoStartCnt = 321+PREPACKET;
	int iLenVal = 0;
	char tempchk[3];
	int iFor = 0;
	int iFor2 = 0;

	cUINT type;

//printf("TT dataBuf : %s        Length : %d\n",dataBuf, strlen(dataBuf));

	if (dataBuf[0] != 0x40) bDataChk = false;	
	cLenVal[0] = dataBuf[1];
	cLenVal[1] = dataBuf[2];
	cLenVal[2] = dataBuf[3];
	iLenVal = atoi(cLenVal);
	//if ( iLenVal != 333 ) bDataChk = false;			

	tempchk[0] = dataBuf[4];
	tempchk[1] = dataBuf[5];

	if (dataBuf[4] != 'T' || dataBuf[5] != 'T') bDataChk = false;//TT Data 	

	if (bDataChk == false)	return 0;
				
	//Data 
	memset(dTTTemp, 0x00, sizeof(dTTTemp));
	type = DBR_FLOAT;
	//***********TC **************//	
	for ( iFor = 0; iFor < 9; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+PREPACKET)+(iFor*8)];
		}
		dTTTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dTTTemp[iFor];
		writetag(T_CIP_REAL, TTchid[iFor]);
	}

	//***********TC Stat **************//
	memset(bStat,true,sizeof(bStat));
	type = DBR_INT;
	for ( iFor = 0; iFor < 9; iFor++ )
	{
		bStat[iFor] = true;
		if( dataBuf[iFor + iStatStartCnt] == 0x30 )	bStat[iFor] = false;
		
		iWriteDATA = 0;
		if (bStat[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, TTchid[iFor+9]);
	}
//printf("TT dataBuf : %s        Length : %d\n",dataBuf, sizeof(dataBuf));
	//***********Delta T **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 30; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iDeltaTStartCnt];
		}
		dTTTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dTTTemp[iFor];
		writetag(T_CIP_REAL, TTchid[iFor+18]);
	}

	//***********DO **************//
	memset(bDO,true,sizeof(bDO));
	type = DBR_INT;
	for ( iFor = 0; iFor < 3; iFor++ )
	{
		bDO[iFor] = true;
		if( dataBuf[iFor + iDoStartCnt] == 0x30 )	bDO[iFor] = false;
		
		iWriteDATA = 0;
		if (bDO[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, TTchid[iFor+48]);
	}

	return 1;
}

int SEpacketParsing()
{

	bool bDataChk = true;
	float dSETemp[43];

	bool bEnDis[34];
	char cLenVal[4];
	char cAIVal[11];	
	int iRTEnStartCnt = 128+PREPACKET;
	int iFRStartCnt = 144+PREPACKET;	
	int iFREnStartCnt = 288+PREPACKET;
	int iTCStartCnt = 306+PREPACKET;
	int iTCEnStartCnt = 314+PREPACKET;
	int iDeltaTStartCnt = 315+PREPACKET;
	int iTLTCStartCnt = 395+PREPACKET;
	int iTLTCEnStartCnt = 403+PREPACKET;
	int iLauTCStartCnt = 404+PREPACKET;
	int iLauTCEnStartCnt = 412+PREPACKET;
	int iLenVal = 0;
	char tempchk[3];
	int iFor = 0;
	int iFor2 = 0;

	cUINT type;
//printf("SE dataBuf : %s        Length : %d\n",dataBuf, strlen(dataBuf));
//return 0;

	if (dataBuf[0] != 0x40) bDataChk = false;	//
	cLenVal[0] = dataBuf[1];
	cLenVal[1] = dataBuf[2];
	cLenVal[2] = dataBuf[3];
	iLenVal = atoi(cLenVal);
	//if ( iLenVal != 414 ) bDataChk = false;			

	tempchk[0] = dataBuf[4];
	tempchk[1] = dataBuf[5];

	if (dataBuf[4] != 'S' || dataBuf[5] != 'E') bDataChk = false;//SE Data 
	
	if (bDataChk == false)	return 0;

	//Data 
	memset(dSETemp, 0x00, sizeof(dSETemp));
	type = DBR_FLOAT;
//printf("SETest111111\n");
	//***********RTD **************//	
	for ( iFor = 0; iFor < 16; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+PREPACKET)+(iFor*8)];
		}
		dSETemp[iFor] = atof(cAIVal);
		//printf("dSETemp[%d] : %f\n", iFor, dSETemp[iFor]);
		fltWriteDATA = dSETemp[iFor];
		ca_put(type, SEchid[iFor], &fltWriteDATA);
		istatus = ca_pend_io(0.1);
	}
//printf("SETest22222222\n");
	//***********RTD En/Dis **************//
	memset(bEnDis,true,sizeof(bEnDis));
	type = DBR_INT;
	for ( iFor = 0; iFor < 16; iFor++ )
	{
		bEnDis[iFor] = true;
		if( dataBuf[iFor + iRTEnStartCnt] == 0x30 )	bEnDis[iFor] = false;
		iWriteDATA = 0;
		if (bEnDis[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, SEchid[iFor+16]);
	}
//printf("SETest333333333333\n");
	//***********Flow Rate **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 18; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iFRStartCnt];
		}
		dSETemp[iFor] = atof(cAIVal);

		fltWriteDATA = dSETemp[iFor];
		writetag(T_CIP_REAL, SEchid[iFor+32]);
	}
//printf("SETest4444444444\n");
	//***********Flow Rate En/Dis **************//
	memset(bEnDis,true,sizeof(bEnDis));
	type = DBR_INT;
	for ( iFor = 0; iFor < 18; iFor++ )
	{
		bEnDis[iFor] = true;
		if( dataBuf[iFor + iFREnStartCnt] == 0x30 )	bEnDis[iFor] = false;
		
		iWriteDATA = 0;
		if (bEnDis[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, SEchid[iFor+50]);
	}
//printf("SETest55555555\n");
	//**********TC Data ************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 1; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iTCStartCnt];
		}
		dSETemp[0] = atof(cAIVal);

		fltWriteDATA = dSETemp[0];
		writetag(T_CIP_REAL, SEchid[iFor+68]);
	}
//printf("SETest6666666666\n");
	//**********TC En/Dis ************//
	type = DBR_INT;
	bEnDis[0] = true;

	if( dataBuf[iTCEnStartCnt] == 0x30 )	bEnDis[0] = false;
//printf("SETest Collecotr TC STATUS         %c\n", dataBuf[iTCEnStartCnt]);
	if (bEnDis[0] == true)	iWriteDATA = 1;
	writetag(T_CIP_INT, SEchid[69]);

	//***********Delta T **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 10; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iDeltaTStartCnt];
		}
		dSETemp[iFor] = atof(cAIVal);

		fltWriteDATA = dSETemp[iFor];
		writetag(T_CIP_REAL, SEchid[iFor+70]);
	}

	//***********Transmissiion Line TC **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 1; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iTLTCStartCnt];
		}
		dSETemp[iFor] = atof(cAIVal);
		fltWriteDATA = dSETemp[iFor];
		writetag(T_CIP_REAL, SEchid[iFor+80]);
	}

	//**********Transmissiion Line TC En/Dis ************//
	type = DBR_INT;
	bEnDis[0] = true;

	iWriteDATA = 0;
	if( dataBuf[iTLTCEnStartCnt] == 0x30 )	bEnDis[0] = false;
	if (bEnDis[0] == true)	iWriteDATA = 1;
	writetag(T_CIP_INT, SEchid[81]);

	//***********Launcher TC **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 1; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iLauTCStartCnt];
		}
		dSETemp[iFor] = atof(cAIVal);
		fltWriteDATA = dSETemp[iFor];
		writetag(T_CIP_REAL, SEchid[iFor+82]);
	}

	//**********Launcher TC En/Dis ************//
	type = DBR_INT;
	bEnDis[1] = true;

	iWriteDATA = 0;
	if( dataBuf[iLauTCEnStartCnt] == 0x30 )	bEnDis[1] = false;
//printf("SETest Launcher TC STATUS         %c\n", dataBuf[iLauTCEnStartCnt]);
	if (bEnDis[1] == true)	iWriteDATA = 1;
	writetag(T_CIP_INT, SEchid[83]);

	//2012-04-24 by son
	ReadTag(T_CIP_REAL, FRchid[7]);
	fltWriteDATA = fltDATA;
	writetag(T_CIP_REAL, SEchid[75]);

	ReadTag(T_CIP_REAL, FRchid[9]);
	fltWriteDATA = fltDATA;
	writetag(T_CIP_REAL, SEchid[76]);

	ReadTag(T_CIP_REAL, FRchid[5]);
	fltWriteDATA = fltDATA;
	writetag(T_CIP_REAL, SEchid[77]);
	return 1;
}

int TDpacketParsing()
{
	bool bDataChk = true;
	float dTDTemp[41];

	bool bLauStat[5];
	bool bStat[9];
	bool bINTL[2];

	char cLenVal[4];
	char cAIVal[11];	
	int iStatStartCnt = 72+PREPACKET;
	int iLauStartCnt = 81+PREPACKET;
	int iLauStatStartCnt = 121+PREPACKET;
	int iDUM2StartCnt = 126+PREPACKET;
	int iIntlStartCnt = 222+PREPACKET;
	int iLenVal = 0;
	char tempchk[3];
	int iFor = 0;
	int iFor2 = 0;

	cUINT type;

//printf("TD dataBuf : %s        Length : %d\n",dataBuf, strlen(dataBuf));

	if (dataBuf[0] != 0x40) bDataChk = false;	//
	cLenVal[0] = dataBuf[1];
	cLenVal[1] = dataBuf[2];
	cLenVal[2] = dataBuf[3];
	iLenVal = atoi(cLenVal);
	//if ( iLenVal != 333 ) bDataChk = false;			

	tempchk[0] = dataBuf[4];
	tempchk[1] = dataBuf[5];

	if (dataBuf[4] != 'T' || dataBuf[5] != 'D') bDataChk = false;//TT Data check	

	if (bDataChk == false)	return 0;
				
	//Transmission Line TC, Launcher TC, Dummy Load2 TC

	//Data 
	memset(dTDTemp, 0x00, sizeof(dTDTemp));
	type = DBR_FLOAT;
	//***********Transmission Line TC **************//	
	for ( iFor = 0; iFor < 9; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 9 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+PREPACKET)+(iFor*8)];
		}
		dTDTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dTDTemp[iFor];
		writetag(T_CIP_REAL, TDchid[iFor]);
	}

	//***********Transmission Line TC Status**************//
	memset(bStat,true,sizeof(bStat));
	type = DBR_INT;
	for ( iFor = 0; iFor < 9; iFor++ )
	{
		bStat[iFor] = true;
		if( dataBuf[iFor + iStatStartCnt] == 0x30 )	bStat[iFor] = false;
		
		iWriteDATA = 0;
		if (bStat[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, TDchid[iFor+9]);
	}

	//***********Launcher TC **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 5; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iLauStartCnt];
		}
		dTDTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dTDTemp[iFor];
		writetag(T_CIP_REAL, TDchid[iFor+18]);
	}

	//***********Launcher Status**************//
	memset(bLauStat,true,sizeof(bLauStat));
	type = DBR_INT;
	for ( iFor = 0; iFor < 5; iFor++ )
	{
		bLauStat[iFor] = true;
		if( dataBuf[iFor + iLauStatStartCnt] == 0x30 )	bLauStat[iFor] = false;
		
		iWriteDATA = 0;
		if (bLauStat[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, TDchid[iFor+23]);
	}

	//***********Dummy Load2 TC **************//
	type = DBR_FLOAT;
	for ( iFor = 0; iFor < 12; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*8)+iDUM2StartCnt];
		}
		dTDTemp[iFor] = atof(cAIVal);

		fltWriteDATA = dTDTemp[iFor];
		writetag(T_CIP_REAL, TDchid[iFor+28]);
	}

	//*********** INTERLOCK **************//
	memset(bINTL,true,sizeof(bINTL));
	type = DBR_INT;
	for ( iFor = 0; iFor < 2; iFor++ )
	{
		bINTL[iFor] = true;
		if( dataBuf[iFor + iIntlStartCnt] == 0x30 )	bINTL[iFor] = false;
		
		iWriteDATA = 0;
		if (bINTL[iFor] == true)	iWriteDATA = 1;
		writetag(T_CIP_INT, TDchid[iFor+40]);
	}

	return 1;
}


int SocketAsk(char *strsend, int iDiv)
{
	char tempchk[6];
	int iSize = 0;
	
	send(skLabVIEW, strsend, strlen(strsend), 0);
	memset(buf, 0x00, sizeof(buf));
	memset(tempchk, 0x00, sizeof(tempchk));
	iSize = recv(skLabVIEW, buf, sizeof(buf), 0);
	if (iSize>0)
	{
		sprintf(tempchk, buf, iSize);
		//printf("SocketAsk[%s] Receive : %s          0 : %c       1 : %c      2 : %c      3 : %c     4 : %c\n", strsend, tempchk, tempchk[0], tempchk[1], tempchk[2], tempchk[3], tempchk[4]);	
	}

	switch(iDiv)
	{
		case 0:
			//if (tempchk == "ACKND")
			if(tempchk[0]=='A' && tempchk[1]=='C' && tempchk[2]=='K' && tempchk[3]=='N' && tempchk[4]=='D')
			{
				//printf("SEND   Receive Ok!!!\n");
				return 1;
			}
			break;

		case 1:
			if(tempchk[0]=='A' && tempchk[1]=='C' && tempchk[2]=='K' && tempchk[3]=='S' && tempchk[4]=='E')
			{	
				//printf("SEDT   Receive Ok!!!\n");
				return 1;
			}
			break;
	}	

	return 0;	
}


void SendSet(void)
{
	int i = 0;
	int iSize = 0;
	char sendtemp[413];
	char temp1[5];
	char temp2[4];
	char chktemp[8];
	char senddata[420];
	int iCon = 0; 

	char recvtemp[6];
	char chkbit[1];

	memset(sendtemp, 0x00, sizeof(sendtemp));
//	printf("SendSet Start!!\n");
//	sprintf(senddata, "@379DT");
	sprintf(senddata, "@413DT");
	for(i=0;i<84;i++)
	{
		if (i<16 || (i>31 && i<50) || (i==68) || (i>69 && i<80) || i == 80 || i == 82)
		{
			ReadTag(T_CIP_REAL, SEchid[i]);
			fltSetVal[i] = fltDATA;
			iCon = fltSetVal[i];
			sprintf(temp1, "%04d", iCon);
			sprintf(chktemp, "%04.3f", fltSetVal[i]);
			iSize = strlen(chktemp);
			sprintf(temp2, "%c%c%c%c", chktemp[iSize-4], chktemp[iSize-3], chktemp[iSize-2], chktemp[iSize-1] );
			sprintf(sendtemp, "%s%s%s", sendtemp, temp1, temp2);		
		}
		else
		{
			ReadTag(T_CIP_INT, SEchid[i]);
			fltSetVal[i] = iDATA;
			iCon = iDATA;
if (i == 69) printf("SETag[%d] : %d\n", i, iDATA);
			chkbit[0] = '0';
			if (iDATA > 0) chkbit[0] = '1';			
			sprintf(sendtemp, "%s%1c", sendtemp, chkbit[0]);	
		}
		
	}
	sprintf(senddata, "%s%s#", senddata, sendtemp);
//	printf("SendFormat : %s      sendtemp_Size : %d\n", senddata, strlen(senddata));
	send( skLabVIEW, senddata, strlen(senddata), 0 );

	memset(buf, 0x00, sizeof(buf));
	memset(recvtemp, 0x00, sizeof(recvtemp));
	iSize = recv(skLabVIEW,buf,sizeof(buf),0);
	if (iSize>0)
	{
		sprintf(recvtemp, buf, iSize);
		
		if(recvtemp[0]=='A' && recvtemp[1]=='C' && recvtemp[2]=='K' && recvtemp[3]=='D' && recvtemp[4]=='T')
		{
			//printf("DataSet SEND   Receive Ok!!!\n");
			iWriteDATA = 0;
			//writetag(T_CIP_INT, "EC1_WCS_SET_DATA");
			writetag(T_CIP_INT, Etcchid[1]);
		}
	}
	//printf("Dataset End!!!!!\n");
	
}

int SendINTG(void)
{
	char sendtemp[20];
	int iSize = 0;
	float fltPwt=0;
	float fltTs=0;
	char temp1[5];
	char temp2[4];
	char chktemp[9];
	int iCon = 0; 

	char tempset1[5];
	char tempset2[5];

	char recvtemp[6];

	memset(sendtemp, 0x00, sizeof(sendtemp));
	//ReadTag(T_CIP_REAL, "EC1_WCS_DT_RFPWS");
	ReadTag(T_CIP_REAL, Etcchid[0]);
	fltPwt = fltDATA;
	iCon = fltPwt;
	
	memset(chktemp, 0x00, sizeof(chktemp));	
	sprintf(temp1, "%04d", iCon);
	sprintf(chktemp, "%04.3f", fltPwt);
	iSize = strlen(chktemp);
	sprintf(temp2, "%c%c%c%c", chktemp[iSize-4], chktemp[iSize-3], chktemp[iSize-2], chktemp[iSize-1] );

	//ReadTag(T_CIP_REAL, "EC1_DAQ_INTG_SET");
	ReadTag(T_CIP_REAL, Etcchid[3]);

	fltTs = fltDATA;
	iCon = fltTs;

	memset(tempset1, 0x00, sizeof(tempset1));
	memset(tempset2, 0x00, sizeof(tempset2));
	memset(chktemp, 0x00, sizeof(chktemp));

	sprintf(tempset1, "%04d", iCon);
	sprintf(chktemp, "%04.3f", fltTs);
	iSize = strlen(chktemp);
	sprintf(tempset2, "%c%c%c%c", chktemp[iSize-4], chktemp[iSize-3], chktemp[iSize-2], chktemp[iSize-1] );
	sprintf(sendtemp, "INTG%s%s%s%s#", temp1, temp2, tempset1, tempset2);

	send( skLabVIEW, sendtemp, strlen(sendtemp), 0 );
	memset(buf, 0x00, sizeof(buf));
	iSize = recv(skLabVIEW,buf,sizeof(buf),0);
	if (iSize>0)
	{
	
		memset(recvtemp, 0x00, sizeof(recvtemp));	
		sprintf(recvtemp, buf, iSize);
		
		if(recvtemp[0]=='A' && recvtemp[1]=='C' && recvtemp[2]=='K' && recvtemp[3]=='I' && recvtemp[4]=='N')
		{
			printf("INTG Start SEND   Receive Ok!!!\n");
			iWriteDATA = 0;
			//writetag(T_CIP_INT, "EC1_WCS_DT_START");
			writetag(T_CIP_INT, Etcchid[2]);
			return 1;
		}
	}
	return 0;
}

int StopINTG(void)
{
	char sendtemp[5];
	int iSize = 0;
	char recvtemp[6];

	memset(sendtemp, 0x00, sizeof(sendtemp));

	sprintf(sendtemp, "STOP");

	send( skLabVIEW, sendtemp, strlen(sendtemp), 0 );
//printf("sendtemp : %s\n",sendtemp);
	memset(buf, 0x00, sizeof(buf));
	iSize = recv(skLabVIEW,buf,sizeof(buf),0);
	if (iSize>0)
	{
	
		memset(recvtemp, 0x00, sizeof(recvtemp));	
		sprintf(recvtemp, buf, iSize);
		printf("stopintg recvtemp: %s\n", recvtemp);
		if(recvtemp[0]=='A' && recvtemp[1]=='C' && recvtemp[2]=='K' && recvtemp[3]=='S' && recvtemp[4]=='P')
		{
			printf("INTG Stop SEND   Receive Ok!!!\n");
			iWriteDATA = 0;
			//writetag(T_CIP_INT, "EC1_WCS_DT_STOP");
			writetag(T_CIP_INT, Etcchid[4]);
			return 1;
		}
	}
	return 0;

}
