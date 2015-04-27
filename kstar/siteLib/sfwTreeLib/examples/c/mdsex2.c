#include <stdio.h>
#include <string.h>
#include <mdslib.h>

#define EXIT_FAILURE -1
#define status_ok(status) (((status) & 1) == 1)

int get_signal_length(const char *signal)
{
	/* local vars */
	int dtype_long = DTYPE_LONG;
	char buf[1024];
	int size;
	int null = 0;
	int idesc = descr(&dtype_long, &size, &null);
	int status;

	/* init buffer */
	memset(buf,0,sizeof(buf));

	/* put SIZE() TDI function around signal name */
	snprintf(buf,sizeof(buf)-1,"SIZE(%s)",signal);

	/* use MdsValue to get the signal length */
	status = MdsValue(buf, &idesc, &null, 0);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Unable to get length of %s.\n",signal);
		return EXIT_FAILURE;
	}

	/* return signal length */
	return size;

}

int main(int argc, char *argv[])
{
	/* local vars */
	int dtype_float = DTYPE_FLOAT;
	int null = 0;
	int shot = 107000;
	int status;
	int socket;
	float *data;     /* array of floats used for signal */
	float *timebase; /* array of floats used for timebase */
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, len;

	/* Connect to MDSplus */
	socket = MdsConnect("atlas.gat.com");
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	status = MdsOpen("EFIT01", &shot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening EFIT01 tree for shot %l.\n",shot);
		return EXIT_FAILURE;
	}

	/* use get_signal_length to get size of signal */
	size = get_signal_length("\\AMINOR");
	if ( size < 1 )
	{
		/* error */
		fprintf(stderr,"Error retrieving length of signal\n");
		return EXIT_FAILURE;
	}

	/* use malloc() to allocate memory for the signal */
	data = (float *)malloc(size * sizeof(float));
	if ( !data )
	{
		fprintf(stderr,"Error allocating memory for data\n");
		return EXIT_FAILURE;
	}

	/* create a descriptor for this signal */
	sigdesc = descr(&dtype_float, data, &size, &null);

	/* retrieve signal */
	status = MdsValue("\\AMINOR",&sigdesc, &null, &len );
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving signal\n");
		free( (void *)data );
		return EXIT_FAILURE;
	}

	/* use malloc() to allocate memory for the timebase */
	timebase = (float *)malloc(size * sizeof(float));
	if ( !timebase )
	{
		fprintf(stderr,"Error allocating memory for timebase");
		free( (void *)data );
		return EXIT_FAILURE;
	}

	/* create a descriptor for the timebase */
	timedesc = descr(&dtype_float, timebase, &size, &null);

	/* retrieve timebase of signal */
	status = MdsValue("DIM_OF(\\AMINOR)", &timedesc, &null, 0);
	if ( !status_ok(status) )
	{
		/* error */
		fprintf(stderr,"Error retrieving timebase\n");
		free( (void *)data );
		free( (void *)timebase );
		return EXIT_FAILURE;
	}

	/* do whatever with the data */

	/* example: print first 10 data points */
	for ( i = 0 ; i < size && i < 10 ; i++ )
		printf("%i  X:%f  Y:%f\n", i, timebase[i], data[i]);

	/* free the dynamically allocated memory when done */
	free( (void *)data );
	free( (void *)timebase );

	/* done */
	return 0;
}


#include <stdio.h>
#include <mdslib.h>

/* Define a macro to check status of MDSplus functions */
#define status_ok(status) (((status) & 1) == 1)

int main()
{

	int dtype_long = DTYPE_LONG;
	int dtype_cstring = DTYPE_CSTRING;
	int status, tstat, len, shot=100, null=0, socket;
	int bufsize = 1024;
	char buf[1024];
	int idesc = descr(&dtype_long, &tstat, &null);
	int sdesc = descr(&dtype_cstring, buf, &null, &bufsize);

	/* Connect to MDSplus server */
	puts("Connecting to MDSplus server.");
	socket = MdsConnect("atlas.gat.com");
	if ( socket == -1 )
	{
		fprintf(stderr,"Error connecting to MDSplus server.\n");
		return -1;
	}

	/* Open tree */
	puts("Opening tree.");
	status = MdsOpen("MYTREE", &shot);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error opening tree for shot %l.\n",shot);
		return -1;
	}

	/* Demonstrate use of the TDI function TCL() to send TCL commands
	   to the MDSplus server. */

	/* First, send the TCL command to the server. */
	puts("Demonstrating use of TCL() function.");
	status = MdsValue("TCL(\"SHOW DEF\",_output)",&idesc,&null,&len);
	if ( !status_ok(status) )
	{
		fprintf(stderr,"Error with SHOW DEF command.\n");
		return -1;
	}
	printf("Status of TCL(\"SHOW DEF\") = %i\n",tstat);

	/* If the command was successful, print its output. */
	if ( status_ok(tstat) )
	{
		status = MdsValue("_output",&sdesc,&null,&len);
		if ( !status_ok(status) )
		{
			fprintf(stderr,"Error getting _output from SHOW DEF command.\n");
			return -1;
		}
		printf("Output of TCL(\"SHOW DEF\") = %s\n",buf);
	}

	return 0;
}
