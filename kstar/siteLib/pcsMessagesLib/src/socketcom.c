/* @(#)socketcom.c	1.21 05/12/05
*******************************************************************************
FILE: socketcom.c

A set of routines that a C language client process can use to communicate 
with a server process. (see talktoserver.c for routines for an IDL language
client process to use)

------------------------------------------------------------------------------
Modifications:
1/9/96: on Solaris2 systems, don't use bcopy
2/1/98: where appropriate, check for interrupted system service and try again.
11/04/99: BGP changes to support endian differences.
3/27/2000: JRF changes made for the PCS upgrade 32/64 bit compatibility.
           Also, rearrange the functions to be more modular.
	   Remove the swap functions into their own file.
5/20/2000: JRF  remove the PCSUPGRADE macros, leave only the upgrade code.
*******************************************************************************
*/

/*
Includes
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
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

#if 1
#include "serverdefs.h"
#include "MemDebug.h"
#endif

/*
misc.
*/
extern processor_type_mask local_processor_type;
/*
*******************************************************************************
Routines for exchanging a message with a server or for simply transmitting
a message to a server.  The main routines are exchange_message and
pcs_send_message.
*******************************************************************************
*/
/*
*******************************************************************************
SUBROUTINE: exchange_message

Exchange a message with a server.  A pointer to the reply
message is returned. The reply message is placed into memory obtained with
malloc.  The calling function is responsible for freeing this memory.

If there are problems in the message exchange, return NULL and set
*reply_length to -1. A valid result of this routine is that the reply
message has length 0.  So, to test for errors in this function the calling
function should test for *reply_length = -1.
*******************************************************************************
*/
void *exchange_message(
		       char *message,
		       int message_length,
		       int *reply_length,
		       char *host,
		       u_short port,
		       char *text)
{
   int sock,error,type;
   void *reply;
   struct sockaddr_in server;

   *reply_length = -1; /* Default in case of error. */

/* 
Create the socket. 
*/
   error = establish_socket(host,port,text,&sock,&server);
   if(error)
      return(NULL);
/*
At this point the socket has been opened.  This counts against the
maximum number of files, so if there is an error after this it is
necessary to be careful to close the file.
*/
/*
Make the connection between the socket and the server.
*/
   error = make_connection(sock, &server, text);
   if(error) {
      close_connection(sock,text);
      return(NULL);
   }
/*
Send the message.
*/
   error = transmit_the_message_header(message_length,sock,text);
   if(error) {
      close_connection(sock,text);
      return(NULL);
   }
   error = transmit_the_message_body(message,message_length,sock,text);
   if(error) {
      close_connection(sock,text);
      return(NULL);
   }
/*
Check message types that do not return a reply.
Just return.
*/
   type = ((int*)message)[0];
   if(type == REMOVE_USER_INFO) return 0;
   if(type == SHUTDOWN_WAVESERVER) return 0;
/*
Get the reply.

In case of error, the function sets *reply_length to 0 and takes care of
freeing any memory it allocated for the reply.
*/
   error = get_the_reply(&reply,reply_length,sock,text);
   if(error) {
      close_connection(sock,text);
      *reply_length = -1;
      return(NULL);
   }
/*
All done.  Close the socket and return the pointer to the reply data.
*/
   error = close_connection(sock,text);
   if(error) {
      *reply_length = -1;
      return(NULL);
   }

   return reply;
}
/*
*******************************************************************************
SUBROUTINE: pcs_send_message

Send a message to a server.  No reply is expected.
Return 0 on success, 1 on failure.
*******************************************************************************
*/
int pcs_send_message(
	char *message,
	int message_length,
	char *host,
	u_short port,
	char *text)
{
   int sock,error;
   struct sockaddr_in server;
/* 
Create the socket. 
*/
   error = establish_socket(host,port,text,&sock,&server);
   if(error)
      return 1;
/*
At this point the socket has been opened.  This counts against the
maximum number of files, so if there is an error after this it is
necessary to be careful to close the file.
*/

/*
Make the connection between the socket and the server.
*/
   error = make_connection(sock, &server, text);
   if(error) {
      close_connection(sock,text);
      return 1;
   }
/*
Send the message.
*/
   error = transmit_the_message_header(message_length,sock,text);
   if(error) {
      close_connection(sock,text);
      return 1;
   }
   error = transmit_the_message_body(message,message_length,sock,text);
   if(error) {
      close_connection(sock,text);
      return 1;
   }
/*
All done.  Close the socket.
*/
   error = close_connection(sock,text);
   if(error)
     return 1;

   return 0;
}
