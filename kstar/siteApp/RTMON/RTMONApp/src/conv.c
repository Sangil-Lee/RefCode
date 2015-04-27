#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>

/* variables	**************************************************************/

static	int		isTarget		= 1;

static int _parse_args(int argc, char** argv)
{
	int		errs	= 0;
	int		i;

	printf("USAGE: conv \"file name\" <-r> <-d>\n");
	printf("   -r   PCS target reference file.\n");
	printf("   -d   MMWI raw data file\n");

	for (i = 2; i < argc; i++)
	{
		if (strcmp(argv[i], "-r") == 0)
		{
			isTarget	= 1;
			continue;
		}

		if (strcmp(argv[i], "-d") == 0)
		{
			isTarget	= 0;
			continue;
		}

		if (isdigit(argv[i][0]) == 0)
		{
			errs	= 1;
			printf("ERROR: invalid argument: %s\n", argv[i]);
			break;
		}

				
		errs	= 1;
		printf("ERROR: invalid argument: %s\n", argv[i]);
		break;
	}

	return(errs);
}


int main(int argc, char **argv)
{
	FILE *fp, *fpDes;
	char bufLineScan[512];
	double dTime1, dTime2, vltg1, vltg2;
	unsigned short ch1, ch2;
	char fileName[256];
	char targetName[32];
	int len;
	char c;
	int		errs;
	int isFirst = 1, i;
	unsigned short u16Temp[128];
	short s16Temp[128];
	signed short s16Tmp;
	float s32Tmp;
	int int32Tmp[96];
	int u32Tmp;
	unsigned short u16Tmp;
	int cnt=0;


	if( argc < 2 ) {
		printf("Need file name!\n");
		exit(1);
	}
//	sscanf(&argv[1], "%s", fileName);
	sprintf(fileName, argv[1]);

//	len = strlen(fileName);
//	strncat(targetName, fileName, len-4);
	strcpy(targetName, fileName);
	strcat(targetName, ".txt");


	printf("read file: '%s' -> '%s'\n", fileName, targetName);

//	errs	= _parse_args(argc, argv);

	
	fp = fopen(fileName, "rb");
	if(fp == NULL)
	{
		perror("raw file open error!");
		exit(1);
	}
	fpDes = fopen(targetName, "w");
	if(fpDes == NULL)
	{
		perror("coversion file open error!");
		fclose(fp);
		exit(1);
	}


#if 1	
	while((len=fread(u16Temp, 256, 1, fp) ) > 0 )
	{
//		if( u16Temp[16] & 0x8000) int32Tmp =  u16Temp[16] - 0xffff ;
//		else int32Tmp = u16Temp[16];

		for(i=0; i<96; i++)
		int32Tmp[i] = ( u16Temp[i] & 0x8000 ) ? u16Temp[i] - 0xffff : u16Temp[i];

/*		fprintf(fpDes, "%d %04x %fv  %04x %fv\n", cnt,
			 int32Tmp[0], (float)int32Tmp[0]*10./32767.,
			 int32Tmp[16], (float)int32Tmp[16]*10./32767.
			); */
		fprintf(fpDes, "%d  (%x  %x)  %f   %f\n", cnt,
				u16Temp[0], u16Temp[16],
                         (float)int32Tmp[0]*10./32767.,
                         (float)int32Tmp[16]*10./32767.
                        );
		cnt++;
	}
#endif
#if 0
	while((len=fread(s16Temp, 256, 1, fp) ) > 0 )
        {
                fprintf(fpDes, "%d %f (%04x),  %f (%04x)\n", cnt,
                         s16Temp[0], s16Temp[0],
                        s16Temp[16], s16Temp[16]
                        );
                cnt++;
        }
#endif


#if 0	
	} 
	else 
	{
		while(1) 
		{
			if( fgets(bufLineScan, 512, fp) == NULL ) break;

			sscanf(bufLineScan, "%lf %lf %lf %lf",  &dTime1, &vltg1, &dTime2, &vltg2);

			ch1 = (unsigned short)(((vltg1 + 10.0) * 0xffff) / 20.0 );
			ch2 = (unsigned short)(((vltg2 + 10.0) * 0xffff) / 20.0 );

			fwrite( &ch1, sizeof(unsigned short), 1, fpDes);
			fwrite( &ch2, sizeof(unsigned short), 1, fpDes);
			if( isFirst) {
				printf("prev: %.8f, %.8f\n", vltg1, vltg2);
				vltg1 = (20.0 * ((double)(0xffff & ch1) / (double)0xffff) - 10.0);
				vltg2 = (20.0 * ((double)(0xffff & ch2) / (double)0xffff) - 10.0);
				printf("converted: %.8f, %.8f\n", vltg1, vltg2);
				isFirst = 0;
			}
		}
	}
#endif
	fclose(fp);
	fclose(fpDes);

	return 0;
	
}



