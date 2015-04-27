#include <stdio.h>
#include <string.h>
#include <mdslib.h>
#include <treeshr.h>
#include <mdsdescrip.h>

//#include <mdsdescrip.h>
//#include <mdsshr.h>

//#define EXIT_FAILURE -1

/* Define a macro to check status of MDSplus functions */
#define status_ok(status) (((status) & 1) == 1)

#define	SERVER_ADDR	"172.17.102.120:8000"
#define	TREE_NAME	"image"
#define	SHOT_NUM	9999

char	TAG_NAME1[256]	= "\\RBACH01:FOO";
char	TAG_NAME2[256]	= "\\RBACH02:FOO";

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
		fprintf(stderr,"Error opening %s tree for shot %l.\n", TREE_NAME, shot);
		return EXIT_FAILURE;
	}

	status = MdsPut(signal, "BUILD_SIGNAL(sin(6.28*[0..999]/1000.),,[0..999]/1000.)", &null);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error writing signal\n");
		return EXIT_FAILURE;
	}

	/* done */
	return 0;
}

int put_signal_array (char *signal)
{
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;

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

	status = MdsPut(signal, "BUILD_SIGNAL([0..9],,[0..9]/1.)", &null);
	if ( !status_ok(status) ) {
		/* error */
		fprintf(stderr,"Error writing signal\n");
		return EXIT_FAILURE;
	}

	/* done */
	return 0;
}

// context per thread
int context_access (char *signal)
{
	int shot = SHOT_NUM;
	int status;
	int socket;

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

	// Use not MdsOpen but TreeOpen to get the id of node (nid) not MdsOpen

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

#if 0
struct descriptor make_desc (int dtype, void *data)
{
	struct descriptor dsc;

	dsc.dtype = dtype;
	dsc.dtype = DTYPE_NATIVE_FLOAT;
	dsc.class = CLASS_S;
	dsc.pointer = data;
	dsc.length = sizeof(struct descriptor);

	return (dsc);
}

struct descriptor_a make_adesc (int dtype, void *data, int ndim)
{
	struct descriptor_a dsc;

	dsc.dtype = dtype;
	dsc.dtype = DTYPE_NATIVE_FLOAT;
	dsc.class = CLASS_A;
	dsc.pointer = data;
	dsc.length = sizeof(struct descriptor_a);

	dsc.scale = 0;
	dsc.digits = 0;
	dsc.dimct = ndim;
	dsc.arsize = sizeof(float)*ndim;
	dsc.aflags.binscale = 0;
	dsc.aflags.redim = 1;
	dsc.aflags.column = 1;
	dsc.aflags.coeff = 1;
	dsc.aflags.bounds = 0;

	return (dsc);
}
#else
struct descriptor make_desc (int dtype, void *data)
{
	struct descriptor dsc;

	dsc.dtype = dtype;
	dsc.dtype = DTYPE_NATIVE_FLOAT;
	dsc.class = CLASS_S;
	dsc.pointer = data;
	dsc.length = sizeof(float);

	return (dsc);
}

struct descriptor_a make_adesc (int dtype, void *data, int ndim)
{
	struct descriptor_a dsc;

	dsc.dtype = dtype;
	dsc.dtype = DTYPE_NATIVE_FLOAT;
	dsc.class = CLASS_A;
	dsc.pointer = data;
	//dsc.length = dtype_length(&dsc);
	dsc.length = sizeof(float);

	dsc.arsize = dsc.length*ndim;
	dsc.dimct = 1;
	dsc.scale = 0;
	dsc.digits = 0;
	dsc.aflags.binscale = 0;
	dsc.aflags.redim = 1;
	dsc.aflags.column = 1;
	dsc.aflags.coeff = 0;
	dsc.aflags.bounds = 0;

	return (dsc);
}
#endif

// not ok
int put_signal_segment_tree (char *signal)
{
	int dtype_Float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	int sigdesc;     /* signal descriptor */
	int timedesc;    /* descriptor for timebase */
	int size;        /* length of signal */
	int i, j, len;
	float data[10] = {1,2,3,4,5,6,7,8,9,10};

	float start = 0;
	float end	= 10.0;
	float rate	= 1;
	int	  cnt	= 10;
#if 0
	int startDesc	= descr(&dtype_Float, &start, &null); // T0 - Blip
	int endDesc		= descr(&dtype_Float, &end, &null);
	int dimDesc		= descr(&dtype_Float, &rate, &null);
	int valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);

	// descrs is a global variable for descriptor
	// extern struct descriptor *descrs;
	// struct descriptor   *startDescSt    = descrs[startDesc-1];
#else
	struct descriptor   startDesc	= make_desc(dtype_Float, &start);
	struct descriptor   endDesc		= make_desc(dtype_Float, &end);
	struct descriptor   rateDesc	= make_desc(dtype_Float, &rate);
	//struct descriptor   dimDesc		= make_desc(dtype_Float, &rate);
	//struct descriptor   dimDesc		= make_desc(dtype_Float, &rate);
	struct descriptor_a valueDesc	= make_adesc(dtype_Float, (void *)data, cnt);
	struct descriptor   *pdimDesc		= NULL;

	//DESCRIPTOR_SLOPE (slopeDesc, 1);
	DESCRIPTOR_WINDOW (winDesc, &startDesc, &endDesc, &rateDesc);
	DESCRIPTOR_RANGE (rangeDesc, &startDesc, &endDesc, &rateDesc);
	DESCRIPTOR_DIMENSION (dimDesc, &winDesc, &rangeDesc);
#endif
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

	for (i = 0; i < 5; i++) {
		start	= i * 10;
		end		= start + 10 - 1;
		rate	= 1;
		cnt		= 10;

		for (j = 0; j < 10; j++) {
			data[j]	= start + j;
		}

		startDesc	= make_desc(dtype_Float, &start);
		endDesc		= make_desc(dtype_Float, &end);
		//dimDesc		= make_desc(dtype_Float, &rate);
		pdimDesc	= (struct descriptor *)&dimDesc;
		valueDesc	= make_adesc(dtype_Float, (void *)data, cnt);

		printf ("[TreeBeginSegment] %d : start(%f) end(%f) rate(%f) cnt(%d) \n", i, start, end, rate, cnt);

		//status = TreeBeginSegment (nid, &startDesc, &endDesc, &dimDesc, &valueDesc, -1);
		status = TreeBeginSegment (nid, &startDesc, &endDesc, pdimDesc, &valueDesc, -1);
		if ( !status_ok(status) ) {
			fprintf(stderr,"Error BeginSegment : %s\n", MdsGetMsg(status));
			return EXIT_FAILURE;
		}

		printf ("[TreePutSegment  ] %d : start(%f) end(%f) rate(%f) cnt(%d) \n", i, start, end, rate, cnt);

		status = TreePutSegment (nid, -1, &valueDesc);
		if ( !status_ok(status) ) {
			fprintf(stderr,"Error TreePutSegment signal\n");
			return EXIT_FAILURE;
		}
	}

	return 0;
}

int put_signal_segment_tree2 (char *signal)
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

#if 1
	int startDesc	= descr(&dtype_Float, &start, &null); // T0 - Blip
	int endDesc		= descr(&dtype_Float, &end, &null);
	int dimDesc		= descr(&dtype_Float, &rate, &null);
	int valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);

	// descrs is a global variable for descriptor
	// extern struct descriptor *descrs;
	// struct descriptor   *startDescSt    = descrs[startDesc-1];
#else
	struct descriptor   startDesc	= make_desc(dtype_Float, &start);
	struct descriptor   endDesc		= make_desc(dtype_Float, &end);
	struct descriptor   dimDesc		= make_desc(dtype_Float, &rate);
	struct descriptor_a valueDesc	= make_adesc(dtype_Float, (void *)data, cnt);
#endif
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
		fprintf(stderr,"Error BeginSegment : %s\n", MdsGetMsg(status));
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

int put_signal_segment (char *signal)
{
	int dtype_Float = DTYPE_FLOAT;
	int null = 0;
	int shot = SHOT_NUM;
	int status;
	int socket;
	int len;
	float data[10] = {1,2,3,4,5,6,7,8,9,10};

	float start = 0.0;
	float end	= 9.0;
	float rate	= 1.0;
	int	  cnt	= 10;
	int startDesc	= descr(&dtype_Float, &start, &null); // T0 - Blip
	int endDesc		= descr(&dtype_Float, &end, &null);
	int dimDesc		= descr(&dtype_Float, &rate, &null);
	int valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);

	int bufsize = 1024;
	char buf[1024];
	int tstat;
	int dtype_Long = DTYPE_LONG;
	int dtype_cstring = DTYPE_CSTRING;
	int idesc = descr(&dtype_Long, &tstat, &null);
	int sdesc = descr(&dtype_cstring, buf, &null, &bufsize);
	int nid;
	int i, j;
	char eventName[] = "SEGMENT";

	/* Connect to MDSplus */
	socket = MdsConnect (SERVER_ADDR);
	if ( socket == -1 ) {
		fprintf(stderr,"Error connecting to Atlas.\n");
		return EXIT_FAILURE;
	}

	/* Open tree */
	status = MdsOpen (TREE_NAME, &shot);
	if ( !status_ok(status) ) {
		fprintf(stderr,"Error opening %s tree for shot %d.\n", TREE_NAME, shot);
		return EXIT_FAILURE;
	}

	for (i = 0; i < 5; i ++) {
		start	= i * 10;
		end		= start + 10 - 1;
		rate	= 1;
		cnt		= 10;

		for (j = 0; j < 10; j++) {
			data[j]	= start + j;
		}

		startDesc	= descr(&dtype_Float, &start, &null);
		endDesc		= descr(&dtype_Float, &end, &null);
		dimDesc		= descr(&dtype_Float, &rate, &null);
		valueDesc	= descr(&dtype_Float, (void *)data, &cnt, &null);

		//NOK
		//sprintf (buf, "BeginSegment(%s,$,$,$,$,-1)", signal);
		//OK
		sprintf (buf, "BeginSegment(%s,$,$,MAKE_DIM(MAKE_WINDOW(0,$,$),MAKE_SLOPE(MAKE_WITH_UNITS($,\"s\"))),$,-1)", signal);
		status = MdsValue (buf, &startDesc, &endDesc, &endDesc, &startDesc, &dimDesc, &valueDesc, &idesc, &null, &len);

		if ( !status_ok(status) ) {
			fprintf(stderr,"Error BeginSegment : %s\n", MdsGetMsg(status));
			return EXIT_FAILURE;
		}
		printf ("%s\n", buf);

#if 0
		sprintf (buf, "PutSegment(%s,-1,$)", signal);
		status = MdsValue (buf, &valueDesc, &idesc, &null, &len);

		if ( !status_ok(status) ) {
			fprintf(stderr,"Error PutSegment : %s\n", MdsGetMsg(status));
			return EXIT_FAILURE;
		}
		printf ("%s\n", buf);
#endif

		sprintf(buf, "TCL(\"setevent %s\")", eventName);
		status = MdsValue (buf, &idesc, &null, &len);

		if ( !status_ok(status) ) {
			fprintf(stderr,"Error TCL command: %s.\n", MdsGetMsg(status));
			return EXIT_FAILURE;
		}
		printf ("setevent %s\n", eventName);

		sleep (5);
	}

	MdsClose (TREE_NAME, &shot);

	/* done */
	return 0;
}

int main (int argc, char *argv[])
{
	//tcl_show_def ();
	//context_access (TAG_NAME1);

#if 0
	put_signal_sine (TAG_NAME1);
	get_signal (TAG_NAME1);

	put_signal_segment (TAG_NAME2);
	get_signal (TAG_NAME2);
#endif

	//put_signal_array (TAG_NAME1);
	//put_signal_segment_tree (TAG_NAME1);
	put_signal_segment (TAG_NAME1);

	return (0);
}
