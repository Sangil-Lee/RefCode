#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mdslib.h>
#include <mdsshr.h>


//const char *MDS_IP_ADDR	= "172.18.54.136:8000";
const char *MDS_IP_ADDR	= "localhost:8000";

/* Useful definition for handling returned status values */
#define statusOk(status) ((status) & 1)

char	MDS_TREE_NAME[256]	= "heating_test";
char	MDS_TAG_NAME[256]	= "\\B0_CH00:FOO";
//int		MDS_SHOT_NUM		= -1;
int		MDS_SHOT_NUM		= 1;
int 	MDS_SIZE 			= 1000;

/* An example of Data readout using the MDSplus Data Access library (mdslib) */

int readExample()
{
	int dtypeFloat = DTYPE_FLOAT; /* We are going to read a signal made of float 
									 values */
	int dtypeShort = DTYPE_SHORT;
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access 
				   operation */
	int socket; /* Will contain the handle to the remote mdsip 
				   server */
	short *data; /* Array of floats used for signal */
	float *timebase; /* Array of floats used for timebase */
	int sigDesc; /* Signal descriptor */
	int timeDesc; /* Timebase descriptor */
	int size; /* Length of signal */
	int i, retLen; /* Returned length */
	int shot = -1; /* Just an example shot number */

	/* Connect to MDSplus */
	socket = MdsConnect((char *)MDS_IP_ADDR);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server.\n");
		return -1;
	}

	/* Open tree */
	shot = MDS_SHOT_NUM;
	status = MdsOpen(MDS_TREE_NAME, &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		return -1;
	}

	size = MDS_SIZE;

	/* use malloc() to allocate memory for the signal */
	data = (short *)malloc(size * sizeof(short));
	if ( !data ) 
	{
		fprintf(stderr,"Error allocating memory for data\n");
		return -1;
	}
	timebase = (float *)malloc(size * sizeof(float));
	if ( !timebase )
	{
		fprintf(stderr,"Error allocating memory for timebase\n");
		free( (void *)data );
		return -1;
	}

	/* create a descriptor for this signal */
	sigDesc = descr(&dtypeShort, data, &size, &null);

	/* create a descriptor for the timebase */
	timeDesc = descr(&dtypeFloat, timebase, &size, &null); 

	/* retrieve signal */
	status = MdsValue(MDS_TAG_NAME, &sigDesc, &null, &retLen );
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal: %s\n", MdsGetMsg(status));
		return -1;
	}

	/* retrieve timebase of signal */
	status = MdsValue("DIM_OF(\\B0_CH00:FOO)", &timeDesc, &null, &retLen);
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving timebase: %s\n", MdsGetMsg(status));
		free( (void *)data );
		free( (void *)timebase );
		return -1;
	}

	/* do whatever with the data */

	/* example: print ALL the data */
	for ( i = 0 ; i < size ; i++ )
		printf("%i X:%f Y:%d\n", i, timebase[i], data[i]);

	/* free the dynamically allocated memory when done */
	free( (void *)data );
	free( (void *)timebase );

	/* done */
	return 0;
} 

int main (int argc, char **argv)
{
	if (argc > 1) {
		strcpy (MDS_TREE_NAME, argv[1]);
		MDS_SHOT_NUM = atoi(argv[2]);
	}

	printf ("Tree(%s) Shot(%d) \n", MDS_TREE_NAME, MDS_SHOT_NUM);

	//writeExample();
	readExample();

	printf ("Tree(%s) Shot(%d) \n", MDS_TREE_NAME, MDS_SHOT_NUM);

	return(0);
}
