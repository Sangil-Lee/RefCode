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
int ReadDG645File()
{
	return 1;
}


int main(int argc, char* argv[])
{

	int i = 0;
	for(i=0;i<24;i++)
	{
		ca_search(DG645Tag[i], &DG645chid[i]);
		istatus = ca_pend_io(0.1);
	}

	initsocket();
	while(1)
	{
		DG645Check();
	}
	return 0;
}

void initsocket()
{
	struct hostent *hpDG645 = NULL;
	struct sockaddr_in sinDG645;

	char *hostDG645 = "172.17.121.252";

	struct linger ling;
	int opt = 1;
	ling.l_onoff  = 1;
	ling.l_linger = 0;

	printf("DG645 Socket Start!!!\n");
	hpDG645 = gethostbyname( hostDG645);
	if( !hpDG645 ) 
	{
		fprintf(stderr, "DG645PC: unknown host: %s\n", hostDG645);
		//exit(EXIT_FAILURE);
	}

	// build address data structure
	bzero( (char*)&sinDG645, sizeof(sinDG645) );
	sinDG645.sin_family = AF_INET;
	bcopy( hpDG645->h_addr, (char*)&sinDG645.sin_addr, hpDG645->h_length );
	sinDG645.sin_port = htons( 5024 );
	
	// active open
	if( ( skDG645 = socket( PF_INET, SOCK_STREAM, 0 ) ) < 0 )
	{
		perror( "simplex-talk: socket" );
		//exit( EXIT_FAILURE );
	}

	if( connect( skDG645, (struct sockaddr*)&sinDG645, sizeof(sinDG645) ) < 0 ){
		perror( "dg645 : Not connect!!!\n" );
		close(skDG645);
		//exit(EXIT_FAILURE);
	}

	dg_write("lcal \n");
	sleep(1);
	dg_read();
	sleep(1);				
	setsockopt(skDG645, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling));
	setsockopt(skDG645, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	printf("DG645 Connection Success!!!\n");	
}

void dg_write(char *str)
{
	// Write string to connection
	int result;
	result = send(skDG645,str,(int)strlen(str),0);
//	printf("dg_write   Command : %s / result : %d\n", str, result);
}

int dg_read()
{
	int recv_size = 0;
	int i = 0;

	recv_size = recv(skDG645, bufDG645, sizeof(bufDG645), 0);

	if (recv_size == 0) return 0;

	char tempbuf[recv_size];

	for (i=0; i<recv_size; i++)
	{
		tempbuf[i] = bufDG645[i];
	}
	iRead = atoi(tempbuf);
//	printf("dg_read   recv : %s     size : %d      fltRead: %f\n", bufDG645, recv_size, fltRead);
	return 1;
}


void DG645Check()
{
	
	int iDGMode = 0;	// OPI에서 지시가 내려온 내용

	//MODE check
	if (ReadTag(T_CIP_INT, DG645chid[0]))
	{
		iDGMode = iDATA;
		iErr = 0;	//Error Code Initialize
		if (iDGMode>0)
		{
			printf("EC1_DG_SET_NO : %d        iDATA : %d\n", iDGMode, iDATA);
			DG645SubCheck(iDGMode);
		}
		//ReadTS();
	}

	ReadTS();

//printf("ichkclockdg645 : %d\n", ichkclockdg645);
	if (ichkclockdg645 > 10) 
	{
		initsocket();
		ichkclockdg645 = 0;
	}

}

void DG645SubCheck(int iDGMode)
{
	int iFor = 0;

	Set_Error();	//Error Code Initialize

	//Trigger Mode
	if (iDGMode < 4 && iDGMode > 0 )
		TriggerMode(iDGMode);

	//Single-shot\n
	if (iDGMode == 4 )
		SingleShot();

	//Repetition Rate
	if (iDGMode == 5)
		Reprate();

	//t0 ch1 ~ t1 ch4 Time set
	if (iDGMode == 6)
	{
		for(iFor=0;iFor<8;iFor++)
		{
			if (TimeSet_chk(iFor)==0)
			{
				iDGMode = 0;
			}
		}
			
	}
	Set_Zero();
}

void TriggerMode(int iDGMode)
{
	int rtnVal = 0;
	int iModeChk = 0;
	iModeChk = ReadTag(T_CIP_INT, DG645chid[iDGMode]);
	if (iModeChk > 0)
	{
		switch (iDGMode)
		{
			case 1:		// Internal
				rtnVal = TriggerModeSet(0);
				break;

			case 2:		// External
				rtnVal = TriggerModeSet(1);
				break;

			case 3:		// Single Shot
				rtnVal = TriggerModeSet(5);
				break;
		}
		if (rtnVal > 0)
		{
			printf("DG645 Trigger Mode Internal Error Code : %d\n", iErr);
			iDGMode = 0;
			Set_Error();
		}
	}
}

int TriggerModeSet(int iTgMode)
{
	char senddata[10];

	sprintf(senddata, "tsrc %d\n", iTgMode);
	dg_write(senddata); 
	iErr = DG645_ErrChk();
	if (iErr > 0) return 0;
	
	dg_write("tsrc?\n");
	dg_read();
	Trigger_DB_Set(iRead);
	return 1;
}

void Trigger_DB_Set(int iDiv)
{
	int iTMI = 0;		// Trigger Mode Internal
	int iTME = 0;		// Trigger Mode External
	int iTMS = 0;		// Trigger Mode Single Shot
	switch (iDiv)
	{
		//Internal
		case 0:
			iTMI = 1;
			iTME = 0;
			iTMS = 0;
			break;

		//External
		case 1:	
			iTMI = 0;
			iTME = 1;
			iTMS = 0;
			break;

		//Single-Shot
		case 5:
			iTMI = 0;
			iTME = 0;
			iTMS = 1;
			break;

		default:		
			iTMI = 0;
			iTME = 0;
			iTMS = 0;
			break;
	}
	iWriteDATA = iTMI;
	writetag(T_CIP_INT, DG645chid[1]);

	iWriteDATA = iTME;
	writetag(T_CIP_INT, DG645chid[2]);

	iWriteDATA = iTMS;
	writetag(T_CIP_INT, DG645chid[3]);
}

void SingleShot()
{
	dg_write("*trg\n"); 
	iErr = DG645_ErrChk();
	if (iErr > 0)
	{
		printf("DG645 Single-Shot Error Code : %d\n", iErr);
		Set_Error();
	}
	iWriteDATA = 0;
	writetag(T_CIP_INT, DG645chid[4]);
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


void Set_Error()
{
	iWriteDATA = iErr;
	writetag(T_CIP_INT, DG645chid[7]);

}

void Set_Zero()
{
	iWriteDATA = 0;
	writetag(T_CIP_INT, DG645chid[0]);
}

int TimeSet_chk(int iDiv)
{
	const char *time_ch[8] = {"t0 ch1", "t1 ch1", "t0 ch2", "t1 ch2", "t0 ch3", "t1 ch3", "t0 ch4", "t1 ch4"};
	char temp[30];
	int  ich0, ich1, ichno;

	ich0 = iDiv + 2;
	if (ich0 == 2 || ich0 == 4 || ich0 == 6 || ich0 == 8)
		ich1 = 0;
	else
		ich1 = ich0 - 1;

	ichno = 2*iDiv + 9;
	ReadTag(T_CIP_REAL, DG645chid[ichno]);
	sprintf(temp, "dlay %d,%d,%.3fe-0\n",ich0, ich1, fltDATA);
	dg_write(temp);
	iErr = DG645_ErrChk();
	if (iErr > 0) 
	{	
		printf("DG645 %s Error Code : %d\n", time_ch[iDiv], iErr);
		Set_Error();
		return 0;
	}
	else
		return 1;
}

int ReadTS()
{
	int  i = 0;
	char temp[30];
	int idgTM = 0;
	int idgRR = 0;	

	for(i=0; i<8; i++)
	{
		sprintf(temp, "dlay? %d\n", i+2);
		dg_write(temp);
		idgTM = dgTM_read();
		if(idgTM > 0)
		{
			ichkclockdg645 = 0;
		}
		else
		{
			ichkclockdg645 += 1;
//			printf("ichk1 : %d\n", ichkclockdg645);
		}
		fltWriteDATA = fltRead;
		writetag(T_CIP_REAL, DG645chid[(i*2)+8]);	
	}

	dg_write("trat? \n");
	idgRR = dgRR_read();
//	printf("idgRR : %d\n", idgRR);
	if(idgRR > 0)
	{
		ichkclockdg645 = 0;
	}
	else
	{
		ichkclockdg645 += 1;
//		printf("ichk2 : %d\n", ichkclockdg645);
	}
	fltWriteDATA = fltRead;
	writetag(T_CIP_REAL, DG645chid[5]);

	return 1;
}

int DG645_ErrChk(void)
{
	int i = 0;
	int recv_size = 0;
	char dg_buf[recv_size];
	
	dg_write("lerr?\n");
	//sleep(1);
	delay(0.1);
	recv_size = recv(skDG645, bufDG645, sizeof(bufDG645), 0);

	if (recv_size == 0) return 0;

	for (i=0; i<recv_size; i++)
	{
		dg_buf[i] = bufDG645[i];
	}
	return atoi(dg_buf);
}

int dgTM_read()
{
	int recv_size = 0;
	int i = 0;
	
	recv_size = recv(skDG645,bufDG645,sizeof(bufDG645),0);
	if (recv_size < 1) return 0;

	char tempbuf[recv_size-2];

	for (i=2; i<recv_size; i++)
	{
		tempbuf[i-2] = bufDG645[i];
	}
	fltRead = atof(tempbuf);
//printf("dgTM_read   recv : %s     size : %d      fltRead: %f\n", bufDG645, recv_size, fltRead);
	return 1;
}

int dgRR_read()
{
	int recv_size = 0;
	int i = 0;
	
	memset(bufDG645, 0x00, sizeof(bufDG645));
	recv_size = recv(skDG645,bufDG645,sizeof(bufDG645),0);

	if (recv_size < 1) return 0;

	char tempbuf[recv_size-2];

	for (i=0; i<recv_size; i++)
	{
		tempbuf[i] = bufDG645[i];
//		printf("Repetition rate  tempbuf[%d] = %c \n", i, tempbuf[i]);		
	}
	fltRead = atof(tempbuf);
//printf("Repetition rate    recv : %s     size : %d      fltRead: %f\n", bufDG645, recv_size, fltRead);
	return 1;
}

int Reprate(void)
{
	char temp[30];
	ReadTag(T_CIP_REAL, DG645chid[6]); 
	memset(temp, 0x00, sizeof(temp));
	//sprintf(temp, "trat %.1f\n", fltDATA);
	sprintf(temp, "trat %.4f\n", fltDATA);	//2011-07-19 4f update by son
	dg_write(temp);
	iErr = DG645_ErrChk();
	if (iErr > 0) return 0;

	return 1;
}
