#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mdslib.h>
#include <mdsshr.h>


//const char *MDS_IP_ADDR	= "172.18.54.136:8000";
const char *MDS_IP_ADDR	= "localhost:8000";

/* Useful definition for handling returned status values */
#define statusOk(status) ((status) & 1)
/*Internal function prototypes */
static int getSize(char *sigName);


/* An example of Data readout using the MDSplus Data Access library (mdslib) */

int readExample()
{
	int dtypeFloat = DTYPE_FLOAT; /* We are going to read a signal made of float 
									 values */
	int null = 0; /* Used to mark the end of the argument list */
	int status; /* Will contain the status of the data access 
				   operation */
	int socket; /* Will contain the handle to the remote mdsip 
				   server */
	float *data; /* Array of floats used for signal */
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
	status = MdsOpen("my_tree", &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot, MdsGetMsg(status));
		return -1;
	}

	/* use getSize to get size of signal */
	size = getSize(":SIGNAL1");
	if ( size < 1 )
	{
		/* error */
		fprintf(stderr,"Error retrieving length of signal\n");
		return -1;
	}

	/* use malloc() to allocate memory for the signal */
	data = (float *)malloc(size * sizeof(float));
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
	sigDesc = descr(&dtypeFloat, data, &size, &null);

	/* create a descriptor for the timebase */
	timeDesc = descr(&dtypeFloat, timebase, &size, &null); 

	/* retrieve signal */
	status = MdsValue(":SIGNAL1", &sigDesc, &null, &retLen );
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal: %s\n", MdsGetMsg(status));
		return -1;
	}

	/* retrieve timebase of signal */
	status = MdsValue("DIM_OF(:SIGNAL1)", &timeDesc, &null, &retLen);
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
		printf("%i X:%f Y:%f\n", i, timebase[i], data[i]);

	/* free the dynamically allocated memory when done */
	free( (void *)data );
	free( (void *)timebase );

	/* done */
	return 0;
} 

/* Get the size (number of elements) of the node whose name is passed as argument */
/* It will use TDI function SIZE(), returning the number of elements of its argument */
static int getSize(char *sigName)
{
	int dtypeLong = DTYPE_LONG; /* The returned value of SIZE() is an
								   integer */
	int retSize; /* Will containg the returned size */
	int lenDescr; /* Descriptor for returned size */
	int null = 0; /* Used to mark the end of the argument
					 list */ 
	int status; /* Returned function status */
	char expression[1024]; /* A buffer large enough to contain the
							  expression SIZE(<signal>) */

	/* Build the expression SIZE(<signal>) */
	sprintf(expression, "SIZE(%s)", sigName);

	/* Create the descriptor */
	lenDescr = descr(&dtypeLong, &retSize, &null);

	/* Evaluate the expression */
	status = MdsValue(expression, &lenDescr, &null, 0);
	/* Check status */ 
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error getting the size of %s.\n", sigName);
		return -1;
	}
	return retSize;
}


/* An example of data insertion using the MDSplus Data Access library (mdslib) */
int writeExample()
{
	int dtypeFloat = DTYPE_FLOAT; /* We are going to write a signal made of 
									 float values */
	int null = 0; /* Used to mark the end of the argument
					 list */
	int status; /* Will contain the status of the data
				   access operation */
	int socket; /* Will contain the handle to the remote
				   mdsip server */
	float data[1000]; /* Array of floats used for signal */
	float timebase[1000]; /* Array of floats used for timebase */
	int dataDesc; /* Data descriptor */
	int timeDesc; /* Timebase descriptor */
	int shot = -1; /* Just an example shot number */
	int len; /* The dimension of X and Y arrays */
	int i; 

	/* Connect to MDSplus */
	socket = MdsConnect((char *)MDS_IP_ADDR);
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to mdsip server.\n");
		return -1;
	}

	/* Open tree */
	status = MdsOpen("my_tree", &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error opening tree for shot %d: %s.\n",shot,
				MdsGetMsg(status));
		return -1;
	}

	/* Build a sample Y and X array */
	for(i = 0; i < 1000; i++)
	{
		data[i] = sin(i / 300.) * cos(i / 20.);
		timebase[i] = i/1000.; /*Build a signal which lasts 1 sec */
	}

	/* Build data descriptor */
	len = 1000; /* 1000 samples */ 
	dataDesc = descr(&dtypeFloat, data, &len, &null);

	/* Build timebase descriptor */
	timeDesc = descr(&dtypeFloat, timebase, &len, &null);

	/* Write The signal */
	status = MdsPut(":SIGNAL1", "BUILD_SIGNAL($1,,$2)", &dataDesc, &timeDesc,
			&null);
	if ( !statusOk(status) )
	{
		/* error */
		fprintf(stderr,"Error writing signal: %s\n", MdsGetMsg(status));
		return -1;
	}

	/* Close tree */
	status = MdsClose("my_tree", &shot);
	if ( !statusOk(status) )
	{
		fprintf(stderr,"Error closing tree for shot %d: %s.\n",shot, 
				MdsGetMsg(status));
		return -1;
	}
	/* Done */
	return 0;
}

int main (int argc, char **argv)
{
	writeExample();
	readExample();

	return(0);
}
