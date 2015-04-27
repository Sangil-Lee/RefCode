#include "EtosServer.h"
#include <unistd.h>        // write(), close()
#include <fcntl.h>         // O_WRONLY


#include <stdio.h>
#include <sys/stat.h>
#include <arpa/inet.h>

 

#define MAXBUF 256

int ssock, csock, i;
struct sockaddr_in client_addr, server_addr;
 

int main()

{

	int  i;

	int clen;
//	struct sockaddr_in client_addr, server_addr;

//	char buf[MAXBUF] = "Im server connecting success.";
	
	int    iSize = 0;
	int 	iRead = 0;
	
	//EPICS TAG SEARCH
	SearchTag();

      //make server socket
	if((ssock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{      
		perror("Failed to make socket discriptor");
		exit(1);

	}
printf("bind ok...\n");
	clen = sizeof(client_addr);

	//setting address structure
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	//server_addr.sin_addr.s_addr = htonl("172.17.101.251");
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(2222);
       

	//binding

	if(bind(ssock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		perror("Failed to binding server socket");
		exit(1);
	}

	//wait for client
	if(listen(ssock, 8) <0 )
	{
		perror("listen error : ");
		exit(1);
        }
	char readbuf[MAXBUF];
	memset(&readbuf, 0, MAXBUF);

	i=0;
	while(1)
	{
		iSize = 0;
		csock = accept(ssock, (struct sockaddr *)&client_addr, &clen);
		i++;
		//if(write(csock, buf, MAXBUF) <= 0)
		//	perror("Writing error : ");
                		
		VacDataRecv();
		close(csock);

		iRead = readtag(T_CIP_REAL, VacTagDatachid[0]);
		if(iRead == 0)
		{
			printf("ETOS1 EPICS TAG RE SEARCH!!!!\n");
			SearchTag();
		}
        }
	return 0;

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
	//printf("ETOS1 Writetag istatus : %d\n", istatus);
}

int readtag(cUINT utype, chid searchID)
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
	if(istatus != 1)	return 0;
	return 1;
}

int VacDataRecv()
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
			//iSize = recv(skLabVIEW,buf,sizeof(buf),0);
			iSize = read(csock, buf, sizeof(buf));
			printf("recv : %s              %d\n",buf, iSize);

			for (i=0; i<iSize; i++)
			{

				if ( bDataStart )
				{
					//#체크EtherNetClientTest_LabView
					//if( buf[i] == 0x03 )
					if( buf[i] == 0x03 && i == 89)
					{
						bDataStart = false;
						bDataEnd = true;
					//printf("bDataEnd check : %d\n", bDataEnd);
						dataBuf[iIdx] = buf[i];
						iIdx = 0;
						
						VacpacketParsing();

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
					if( buf[i] == 0x02 )	
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

int VacpacketParsing()
{
	bool bDataChk = true;
	float dVac[6];

	char cAIVal[11];	
	int iFor = 0;
	int iFor2 = 0;
	int iVac[4];
	int iTmpStartCnt = 49;
	int iVionStartCnt = iTmpStartCnt + 16;
//	char opr[1];
	int iopr = 0;
	char amp[1];
	int iamp = 0;
//	char temp;
	cUINT  type;
	int iCount=0;

//printf(" dataBuf : %s        Length : %d       iTmpStartCnt : %d\n",dataBuf, strlen(dataBuf), iTmpStartCnt);

	//데이터가 정상적인지 여부와 데이터 자르기
	if (dataBuf[0] != 0x02) bDataChk = false;	//첫번째 데이터가 stx인지 확인
	if ((0x30 > dataBuf[1]) || (0x39 < dataBuf[1])) bDataChk = false;	
	if (dataBuf[2] != 0x2E) bDataChk = false;
	for ( iFor = 3; iFor < 6; iFor++ )
	{
		if (0x2E != dataBuf[iFor])
		{
			if ((0x30 > dataBuf[iFor]) || (0x39 < dataBuf[iFor])) bDataChk = false; 
		}
	}
	//if ((dataBuf[6] != 'E') && (dataBuf[6] != 'e'))  bDataChk = false;
	if (dataBuf[6] != 'E')  bDataChk = false;		
	for ( iFor = 0; iFor < 4; iFor++ )
	{
		iCount = (iFor * 8) + 6;
		if (dataBuf[iCount+1] != 0x2D) bDataChk = false;	
		if ((0x30 > dataBuf[iCount+2]) || (0x39 < dataBuf[iCount+2])) bDataChk = false;
		if ((0x30 > dataBuf[iCount+3]) || (0x39 < dataBuf[iCount+3])) bDataChk = false;
		if (dataBuf[iCount+4] != 0x2E) bDataChk = false;	
		if ((0x30 > dataBuf[iCount+5]) || (0x39 < dataBuf[iCount+5])) bDataChk = false;
		if ((0x30 > dataBuf[iCount+6]) || (0x39 < dataBuf[iCount+6])) bDataChk = false;
		if ((0x30 > dataBuf[iCount+7]) || (0x39 < dataBuf[iCount+7])) bDataChk = false;
		//if ((dataBuf[iCount+8] != 'E') && (dataBuf[iCount+8] != 'e')) bDataChk = false;
		if (dataBuf[iCount+8] != 'E') bDataChk = false;	
	}
	if (dataBuf[39] != 0x2D) bDataChk = false;
	if (dataBuf[40] != 0x32) bDataChk = false;
	if (bDataChk == false)
	{
		printf("Data Fault \n");
		//return 0;
	}
	//Data 분리
	memset(dVac, 0x00, sizeof(dVac));
	type   = DBR_FLOAT;

	for ( iFor = 0; iFor < 6; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 8 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2+1)+(iFor*8)];
		}
		dVac[iFor] = atof(cAIVal);
		fltWriteDATA = dVac[iFor];
		printf("%d   Value : %f\n",i+1, fltWriteDATA);
		writetag(T_CIP_REAL, VacTagDatachid[iFor]);					
	}

	for ( iFor = 0; iFor < 2; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 4 ; iFor2++)
		{			
			switch (iFor2)
			{
				case 0:
					cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt];
					iopr = cAIVal[iFor2];
					if(iopr > 0x19)
					{
						iWriteDATA = cAIVal[iFor2]-0x20;
					}
					else
					{
						iWriteDATA = cAIVal[iFor2];
					}
					printf("iWriteDATA = %d\n", iWriteDATA);
					if(iWriteDATA < 0) iWriteDATA = 0;
					writetag(T_CIP_INT, TmpStsTagDatachid[iFor]);
					break;
				case 1:
					iamp = 0;
					memset(amp, 0x00, sizeof(amp));
					//cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt];
					amp[0] = abs(dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt]);
					//temp = dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt] << 6;
					//amp[0] = temp >> 6;
					printf("AMP : %1x     val : %d\n", amp[0], iamp);
					fltWriteDATA = amp[0]/25.5;//cAIVal[iFor2]/25.5;
					//printf("test :%d\n", dataBuf[50]);
					printf("fltWriteDATA = %f\n", fltWriteDATA);
					//if(fltWriteDATA < 0) 
					//{
					//	fltWriteDATA *= -1.6;	//-값이 나오면 +값으로 변환하기 위하여
					//}
					writetag(T_CIP_REAL, TmpAmpTagDatachid[iFor]);
					break;
				case 2:
					cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt];
					iWriteDATA = cAIVal[iFor2];
					writetag(T_CIP_INT, TmpRpmTagDatachid[iFor]);
					break;
				case 3:
					cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt];
					printf("TMP%d Temperature : %x\n", iFor, dataBuf[(iFor2)+(iFor*4)+iTmpStartCnt]);
					iWriteDATA = cAIVal[iFor2];
					writetag(T_CIP_INT, TmpTempTagDatachid[iFor]);					
					break;				
			}
						
			//printf("TMP value%d : %d\n", iFor2+1, cAIVal[iFor2]);
		}
		iVac[iFor] = atoi(cAIVal);
		//printf("TMP Value : %i\n",iVac[iFor]);
	}

	//VacIon
	for ( iFor = 0; iFor < 2; iFor++ )
	{
		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 0; iFor2 < 7 ; iFor2++)
		{
			cAIVal[iFor2] = dataBuf[(iFor2)+(iFor*12)+iVionStartCnt];
			//printf("VacIon%d Current value(char) : %x\n", iFor+1, cAIVal[iFor2]);
		}
		fltWriteDATA = atof(cAIVal);
		writetag(T_CIP_REAL, VionAmpTagDatachid[iFor]);	
		printf("VacIon%d Current value : %f\n", iFor+1, fltWriteDATA);

		memset(cAIVal,0x00,sizeof(cAIVal));
		for (iFor2 = 7; iFor2 < 12 ; iFor2++)
		{
			cAIVal[iFor2-7] = dataBuf[(iFor2)+(iFor*12)+iVionStartCnt];
			//printf("VacIon%d Fault value(char) : %x\n", iFor+1, cAIVal[iFor2-7]);
		}
		iWriteDATA = atof(cAIVal);
		writetag(T_CIP_INT, VionFltTagDatachid[iFor]);
		printf("VacIon%d Fault value : %d\n", iFor+1, iWriteDATA);
	
	}
	return 1;
}

void SearchTag()
{
	int i = 0;

	//MAXI_GAUGE
	for(i=0;i<6;i++)
	{
		ca_search(VacTagData[i], &VacTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		
	}        

	//TMP1~2
	for(i=0;i<2;i++)
	{
		ca_search(TmpStsTagData[i], &TmpStsTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		

		ca_search(TmpAmpTagData[i], &TmpAmpTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		

		ca_search(TmpRpmTagData[i], &TmpRpmTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);	

		ca_search(TmpTempTagData[i], &TmpTempTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		
	}        

	//VACION1~2
	for(i=0;i<2;i++)
	{
		ca_search(VionAmpTagData[i], &VionAmpTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		

		ca_search(VionFltTagData[i], &VionFltTagDatachid[i]);
		istatus = ca_pend_io(0.1);
		printf("%d : %d\n", i, istatus);		
	}        


}
