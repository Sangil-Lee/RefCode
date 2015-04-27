/* @(#)send_message_int.c	1.11 10/04/01
*****************************************************************************
FILE: send_message_int.c

Send an single integer message to a server process's input socket.

USAGE: send_message_int HOST PORT MESSAGE

ex. send_message_int host 1234 123

5/20/2000: JRF  remove the PCSUPGRADE macros, leave only the upgrade code.
*****************************************************************************
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>

#include "socketcom.h"

/*
This global variable is provided to hold the mask indicating the type
of processor on which this function is executing.  This variable
is filled with a call to get_processor_type.  This only needs to be
done once when the program is started.  Various socket communication
functions will reference this global variable.
*/
processor_type_mask local_processor_type = 0;

int main(int argc,char **argv)
{
    int port;
    char host[100];
    int message_num;
    int message_length;

    if((argc-1) == 3)
    {
            sscanf(argv[1],"%s",host);
            sscanf(argv[2],"%d",&port);
            sscanf(argv[3],"%d",&message_num);
    }

    else
    {
        printf("Enter host name  --> ");
        scanf("%s",host);
        printf("Enter port number--> ");
        scanf("%d",&port);
        printf("Enter message number--> ");
        scanf("%d",&message_num);
    }

    /*
    send message, no reply expected
    */
    {
        printf("Sending int message: %d to %s on port %d\n",
		message_num,host,port);
	/*
	  Determine the type of processor on which this function is executing.
	  This only needs to be done once when the process is started.
	*/
	get_processor(&local_processor_type);
	/*
	  Send the message to the server.
	*/
        message_length = sizeof(int);
        pcs_send_message((char *)&message_num, /* pointer to the bytes
						  to be transmitted. */
                        message_length,        /* number of bytes in 
						  the message. */
                        host,                  /* string giving the name of 
						  the host on which the 
						  server process is 
						  executing. */
                        (u_short)port,         /* number of the port on which
						  the server process is 
						  listening. */
                        "client"               /* a string that will be used
						  as part of any error 
						  messages issued by the 
						  socket communication 
						  functions. */
			 );
    }
    exit(0);
}                                     
