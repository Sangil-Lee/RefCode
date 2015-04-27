#include <stdio.h>
#include <string.h>
#include <mdslib.h>
#include <treeshr.h>

//#include <mdsdescrip.h>
//#include <mdsshr.h>

//#define EXIT_FAILURE -1

/* Define a macro to check status of MDSplus functions */
#define status_ok(status) (((status) & 1) == 1)

#define	SERVER_ADDR	"127.0.0.1:8000"
#define	TREE_NAME	"image"
#define	SHOT_NUM	9999

char	TAG_NAME1[256]	= "\\RBACH01:FOO";
char	TAG_NAME2[256]	= "\\RBACH02:FOO";
char	TAG_PATH1[256]	= "\\IMAGE::RBACH01:FOO";
char	TAG_PATH2[256]	= "\\IMAGE::RBACH02:FOO";
#if 0
char	NODE_PATH1[256]	= "\\IMAGE::TOP.RAB:RBACH01:FOO";
char	NODE_PATH2[256]	= "\\IMAGE::TOP.RAB:RBACH02:FOO";
#else
char	NODE_PATH1[256]	= "\\IMAGE::TOP.RAB:RBACH01";
char	NODE_PATH2[256]	= "\\IMAGE::TOP.RAB:RBACH02";
#endif

int tcl_show_def ()
{
	int dtype_long = DTYPE_LONG;
	int dtype_cstring = DTYPE_CSTRING;
	int status, tstat, len, shot=SHOT_NUM, null=0, socket;
	int bufsize = 1024;
	char buf[1024];
	int idesc = descr(&dtype_long, &tstat, &null);
	int sdesc = descr(&dtype_cstring, buf, &null, &bufsize);

	/* Connect to MDSplus server */
	puts("Connecting to MDSplus server.");
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to MDSplus server.\n");
		return -1;
	}

	/* Open tree */
	puts("Opening tree.");
	status = MdsOpen (TREE_NAME, &shot);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening tree for shot %l.\n",shot);
		return -1;
	}

	/* Demonstrate use of the TDI function TCL() to send TCL commands
	   to the MDSplus server. */

	/* First, send the TCL command to the server. */
	puts("Demonstrating use of TCL() function.");
	status = MdsValue("TCL(\"SHOW DEF\",_output)",&idesc,&null,&len);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error with SHOW DEF command.\n");
		return -1;
	}
	printf("Status of TCL(\"SHOW DEF\") = %i\n",tstat);

	/* If the command was successful, print its output. */
	if ( status_ok(tstat) ) {
		status = MdsValue("_output",&sdesc,&null,&len);
		if ( !status_ok(status) ) {
			fprintf(stderr,"Error getting _output from SHOW DEF command.\n");
			return -1;
		}
		printf("Output of TCL(\"SHOW DEF\") = %s\n",buf);
	}

	return 0;
}

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
	if ( !status_ok(status) ) {
		fprintf(stderr,"Unable to get length of %s.\n",signal);
		return EXIT_FAILURE;
	}

	/* return signal length */
	return size;
}

int get_signal (char *signal)
{
	/* local vars */
	int dtype_float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	float *data;     /* array of floats used for signal */
	float *timebase; /* array of floats used for timebase */
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, len;
	char buf[1024];

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	status = MdsOpen (TREE_NAME, &shot);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening EFIT01 tree for shot %l.\n",shot);
		return EXIT_FAILURE;
	}

	/* use get_signal_length to get size of signal */
	size = get_signal_length (signal);
	if ( size < 1 ) {
		/* error */
		fprintf(stderr,"Error retrieving length of signal\n");
		return EXIT_FAILURE;
	}

	/* use malloc() to allocate memory for the signal */
	data = (float *)malloc(size * sizeof(float));
	if ( !data ) {
		fprintf(stderr,"Error allocating memory for data\n");
		return EXIT_FAILURE;
	}

	/* create a descriptor for this signal */
	sigdesc = descr(&dtype_float, data, &size, &null);

	/* retrieve signal */
	status = MdsValue (signal, &sigdesc, &null, &len );
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error retrieving signal\n");
		free( (void *)data );
		return EXIT_FAILURE;
	}

	/* use malloc() to allocate memory for the timebase */
	timebase = (float *)malloc(size * sizeof(float));
	if ( !timebase ) {
		fprintf(stderr,"Error allocating memory for timebase");
		free( (void *)data );
		return EXIT_FAILURE;
	}

	/* create a descriptor for the timebase */
	timedesc = descr(&dtype_float, timebase, &size, &null);

	/* retrieve timebase of signal */
	snprintf(buf,sizeof(buf)-1,"DIM_OF(%s)", signal);

	status = MdsValue(buf, &timedesc, &null, 0);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error retrieving timebase\n");
		free( (void *)data );
		free( (void *)timebase );
		return EXIT_FAILURE;
	}

	/* do whatever with the data */

	/* example: print first 10 data points */
	for ( i = 0 ; i < size && i < 10 ; i++ ) {
		printf("%i  X:%f  Y:%f\n", i, timebase[i], data[i]);
	}

	/* free the dynamically allocated memory when done */
	free( (void *)data );
	free( (void *)timebase );

	/* done */
	return 0;
}

int put_signal_sine (char *signal)
{
	int dtype_float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, len;

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	status = MdsOpen (TREE_NAME, &shot);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening EFIT01 tree for shot %l.\n",shot);
		return EXIT_FAILURE;
	}

	//status = MdsPut(signal, "BUILD_SIGNAL(sin(6.28*[0..999]/1000.),,[0..999]/1000.)", &null);
	status = MdsPut(signal, "BUILD_SIGNAL(sin(1.28*[0..999]/1000.),,[0..999]/1000.)", &null);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error writing signal\n");
		return EXIT_FAILURE;
	}

	/* done */
	return 0;
}

int context_access (char *signal)
{
	int dtype_Float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, len;
	float data[10] = {1,2,3,4,5,6,7,8,9,10};

	float start = 0;
	float end	= 100.0;
	float rate	= 0.1;
	int	  cnt	= 10;
	int startDesc	= descr(&dtype_Float, &start, &null); // T0 - Blip
	int endIdxDesc	= descr(&dtype_Float, &end, &null);
	int dimDesc		= descr(&dtype_Float, &rate, &null);
	int valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);
	int nid;
	void *ctx	= 0;

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	//status = _TreeOpen (&ctx, TREE_NAME, shot, 0);
	status = TreeOpen (TREE_NAME, shot, 0);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening EFIT01 tree for shot %l.\n",shot);
		return EXIT_FAILURE;
	}

	//status = _TreeFindNode (ctx, signal, &nid);
	status = TreeFindNode (signal, &nid);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error TreeFindNode for [%s]\n", signal);
		return EXIT_FAILURE;
	}
	printf ("nid = %d\n", nid);

	/* done */
	return 0;
}

int get_node_id (int socket, char *signal, char *path)
{
	int		nid;
	void	*dbid = (void *)&socket;

	if ( !status_ok (TreeFindNode (signal, &nid)) ) { printf("Error TreeFindNode [%s]\n", signal); }
	if ( !status_ok (TreeFindNode (path, &nid)) ) { printf("Error TreeFindNode [%s]\n", path); }

	//socket = -1;
	if (0 <= socket) {
		if ( !status_ok (_TreeFindNode (dbid, signal, &nid)) ) { printf("Error _TreeFindNode [%s]\n", signal); }
		if ( !status_ok (_TreeFindNode (dbid, path, &nid)) ) { printf("Error _TreeFindNode [%s]\n", path); }
	}

	return (nid);
}

int put_signal_segment (char *signal)
{
	int dtype_Float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, len;
	float data[10] = {1,2,3,4,5,6,7,8,9,10};

	float start = 0;
	float end	= 10.0;
	float rate	= 1;
	int	  cnt	= 10;
	int startDesc	= descr(&dtype_Float, &start, &null); // T0 - Blip
	int endDesc		= descr(&dtype_Float, &end, &null);
	int dimDesc		= descr(&dtype_Float, &rate, &null);
	int valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);
	int nid;

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	status = TreeOpen (TREE_NAME, shot, 0);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening EFIT01 tree for shot %l.\n",shot);
		return EXIT_FAILURE;
	}

	status = TreeFindNode (signal, &nid);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error TreeFindNode for [%s]\n", signal);
		return EXIT_FAILURE;
	}
	printf ("nid = %d\n", nid);

	status = TreeBeginSegment (nid, 
			(struct descriptor *)&startDesc, 
			(struct descriptor *)&endDesc, 
			(struct descriptor *)&dimDesc, 
			(struct descriptor_a *)&valueDesc,
			-1);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error BeginSegment\n");
		return EXIT_FAILURE;
	}

#if 0
	for (i = 0; i < 10000; i++) {
		status = PutRow(signal, "1000, $, $", timestamp, &null)
		if ( !status_ok(status) ) {
			/* error */
			fprintf(stderr,"Error writing signal\n");
			return EXIT_FAILURE;
		}
	}
#endif

	/* done */
	return 0;
}

int main (int argc, char *argv[])
{
	tcl_show_def ();
	context_access (TAG_NAME1);
	context_access (TAG_NAME2);

#if 1
	put_signal_sine (TAG_NAME1);
	get_signal (TAG_NAME1);

	put_signal_segment (TAG_NAME2);
	get_signal (TAG_NAME2);
#endif

	return (0);
}
