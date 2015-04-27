/* @(#)socketbasic.c	1.13  08/03/01
*******************************************************************************
FILE: socketbasic.c

Low level routines  used to build routines for interprocess 
communication.

establish_socket
make_connection
transmit_the_message
get_the_reply
close_connection
socket_error
------------------------------------------------------------------------------
Modifications:
5/17/00:  JRF, Created from routines originally in socketcom.c
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
#include <strings.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "socketbasic.h"
#include "MemDebug.h"

extern processor_type_mask local_processor_type;

/*
*******************************************************************************
SUBROUTINE: establish_socket

Connect to a server.   Return the socket number.
Note that if this function executes successfully, then a file has
been opened as a result. This file must be closed eventually.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
*/
int establish_socket(
		     char *host,
		     u_short port,
		     char *text,
		     int *sock_out,
		     struct sockaddr_in *server)
{
   int sock;
   struct hostent *hp;
#if defined(SOLARIS_2)
   struct hostent *hp_result;
   char *buffer;
   int buffer_length,h_error;
#endif
/*
Get the host address of the server.
*/
#if defined(SOLARIS_2)
   buffer_length = 2000;          /* This is just a guess at the required
				     length of this buffer.  The man pages
				     give no information on how long 
				     it must be. */
   buffer = (char *)malloc(buffer_length);
   hp_result = (struct hostent *)malloc(sizeof(struct hostent));

   hp = gethostbyname_r(host,hp_result,buffer,buffer_length,&h_error);
   if(hp == (struct hostent *)NULL)
   {
      socket_error(text,host," (establish_connection): unknown host");
      return 1;
   }
#else
   hp = gethostbyname(host);
   if(hp == (struct hostent *)NULL)
   {
      socket_error(text,host," (establish_connection): unknown host");
      return 1;
   }
#endif
/* Create the socket. */

   sock = socket(AF_INET, SOCK_STREAM, 0);
   if(sock < 0 )
   {
      socket_error(text,NULL," (establish_connection) opening stream socket");
      return 1;
   }

/* Set the server structure and the host name. */

   server->sin_family = AF_INET;
   bcopy((char *)hp->h_addr, (char *)&server->sin_addr, hp->h_length);
   server->sin_port = htons(port);

   *sock_out = sock;

#if defined(SOLARIS_2)
   free(buffer);
   free(hp_result);
#endif
   return 0;
}
/*
*******************************************************************************
SUBROUTINE: make_connection

Connect to a specified server.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
 */
int make_connection(int sock,
		    struct sockaddr_in *server,
		    char *text)
{
   int count_timeout, count_repeat, error, tryit;

   count_repeat = 0;
   tryit = 1;

   while(tryit == 1){
/*
Make the connection to the server.

The connect function will time out at some point if the connection isn't
made.  This could happen if too many clients are simultaneously attempting
to connect to the server.  So, if the connect fails with a timeout error,
we try again.  However, we only try a few times.

The connect function could fail for many other reasons.  Some of these
reasons are not really failures.  In these cases the connect will continue and
will complete asynchronously.  If this happens we use the select function
to confirm that the socket is ready for writing.  However, we put a timeout
on the select function.  If the connection doesn't complete within a reasonable
time, we try the connect function again.  However, this is only repeated
a few times before we give up.
*/
      count_timeout = 0;
      errno = ETIMEDOUT;
      while(errno == ETIMEDOUT) {
	 errno = 0;
	 error = connect(sock,(struct sockaddr *)server, sizeof *server);
	 if( (error < 0) && (errno == ETIMEDOUT) ) {
	    count_timeout = count_timeout + 1;
	    if(count_timeout >= 5){
	       socket_error(text,NULL,
			    " (make_connection) connecting stream socket");
	       return 1;
	    }
	 }
	 else if(error < 0){
	    if( (errno != EALREADY)  &&
		(errno != EINPROGRESS)  &&
		(errno != EINTR) ) {
	       socket_error(text,NULL,
			    " (make_connection) connecting stream socket");
	       return 1;
	    }
	 }
      }

      if(error >= 0){
	 /*
	   The connection was made without error.
	 */
	 tryit = 0;
      }
      else{
	 /*
	   The call to "connect" returned with one of these errors:
	   EALREADY, EINPROGRESS, EINTR.
	   In this case, the connection might complete, so here
	   we check to see if the connection has completed before trying again.
	 */
	 fd_set ready;
	 struct timeval to;
	 /*
	   Check to see if the socket is ready for writing.
	   Give it 2 seconds to become ready.
	 */
	 FD_ZERO(&ready);
	 FD_SET(sock, &ready);
	 to.tv_sec = 2;
	 to.tv_usec = 0;

/* SOLARIS_1 = Solaris version 1.x */
/* SOLARIS_2 = Solaris version 2.x */

#if defined(SOLARIS_1) || defined(SOLARIS_2) || defined(REALIX) || \
                                        defined(LINUXOS) || defined(HPUX_10)
         if(select(sock + 1, (fd_set *)0, &ready, (fd_set *)0, &to) < 0)
#endif
#ifdef HPUX_9                     /* HP unix version 9.x */
         if(select(sock + 1, (int *)&ready, (int *)0,    (int *)0,    &to) < 0)
#endif
         {
	    socket_error(text,NULL," (make_connection), select");
	    count_repeat = count_repeat + 1;
	    if(count_repeat >= 5){
	       socket_error(text," (make_connection) repeat limit reached",
			    NULL);
	       return 1;
	    }
         }
	 else{ /* Call to select() returned without error. */
	    /*
	      If the socket is ready, then  we are done.  
	      If not, then try to connect again.
	      */
	    if(FD_ISSET(sock, &ready)) {
	       tryit = 0;
	    }
	    else{
	       count_repeat = count_repeat + 1;
	       if(count_repeat >= 5){
		  socket_error(text," (make_connection) repeat limit reached",
			       NULL);
		  return 1;
	       }
	    }
	 } /* call to select() */
      } /* if(error >= 0){ */
   } /* while(tryit == 1){ */
   return 0;
}
/*
*******************************************************************************
SUBROUTINE: transmit_the_message_header

Send the message header to the server. The message header consists of
the processor type and the number of bytes in the remainder of the message.

This routine assumes that the global variable "local_processor_type"
contains the processor type mask for the processor on which this 
routine is running.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
*/
int transmit_the_message_header(
			 int message_length,
			 int sock,
			 char *text)
{
   processor_type_mask processor;
   message_length_value length;
/*
Send out the host_type and length of the message.
*/
   processor = local_processor_type;
   hton_processor_type(processor);
   if( writeit(sock, (char *)&processor, sizeof(processor_type_mask) ) < 0)
   {
      socket_error(text,NULL,
		   " (transmit_the_message_header), sending host type");
      return 1;
   }
   length = message_length;
   hton_message_length(length);
   if( writeit(sock, (char *)&length, sizeof(message_length_value) ) < 0)
   {
      socket_error(text,NULL,
		   " (transmit_the_message_header), sending message length");
      return 1;
   }
   return 0;
}
/*
*******************************************************************************
SUBROUTINE: transmit_the_message_body

Send the  body of the message.  This routine assumes that the message
header has already been transmitted.

This routine can be called multiple times to send a message in pieces.
When the message header was transmitted, the message length sent there
should have been the length of the complete message body.  Here, the function
argument "length" gives the length of the piece to the transmitted by
one call to this routine.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
*/
int transmit_the_message_body(
			      char *message,
			      int length,
			      int sock,
			      char *text)
{
   if( writeit(sock, message, length) < 0)
   {
      socket_error(text,NULL,
		   " (transmit_the_message_body), sending message body");
      return 1;
   }

   return 0;
}
/*
*******************************************************************************
SUBROUTINE: get_the_reply

Receive the reply from the server. If the reply length is 0, return a
NULL pointer.  Otherwise, return a pointer to the reply data in memory
obtained with malloc.  The calling routine must free this memory.  The
calling routine can get away with attempting to free memory in the case
where the reply length is 0 because free(NULL) is allowed.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
*/
int get_the_reply(
		  void **reply_out,
		  int *reply_length,
		  int sock,
		  char *text)
{
   int rval;
   void *reply;
   message_length_value length;

   *reply_out = NULL; /* Defaults if there is an error. */
   *reply_length = 0;
/*
Get the reply length followed by the body of the reply.
Allocate sufficient memory for the reply.
*/
   if((rval = readit(sock, (char *)&length, sizeof(message_length_value))
                    ) < sizeof(message_length_value))
   {
      socket_error(text,NULL," (get_the_reply), reading reply length");
      return 1;
   }
   ntoh_message_length(length);
   *reply_length = length;

   if(*reply_length > 0){
      reply = malloc(*reply_length);
      if((rval = readit(sock, (char *)reply, *reply_length)) < *reply_length){
	 socket_error(text,NULL," (get_the_reply), reading reply");
	 free(reply);
	 *reply_out = NULL;
	 *reply_length = 0;
	 return 1;
      }
      *reply_out = reply;
   }
   else{
      *reply_out = NULL;
   }
   return 0;
}
/*
*******************************************************************************
SUBROUTINE: close_connection

Terminate the socket connection to the server process.

If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.
In the event of an error, 1 is returned and errno is left to the appropriate 
error use by the calling routine.
*******************************************************************************
*/
int close_connection(
		     int sock,
		     char *text)
{
  int count = 100;
/*
All done.  Close the socket. Try at most 100 times.
*/
   errno = EINTR;
   while(errno == EINTR) {
      errno = 0;
      if(close(sock) < 0){
	 if(errno != EINTR) {
	    socket_error(text,NULL," (Close_connection): closing socket");
            return 1;
	 }
	 else {
	   count = count - 1;
	   if(count == 0){
	       socket_error(text,"(close_connection): "
			    "failed to close connection after 100 attempts."
			 " Encountered 100 interruptions of close function.\n",
			    NULL);
	     return 1;
	   }
	 }
      }
   }
   return 0;
}
/*
*******************************************************************************
SUBROUTINE: socket_error

Issue an error message indicating the cause of a socket routine problem.
If text = NULL, no error messages are printed.  Otherwise, the string
indicated by text is used as part of the error message.

The value of errno is preserved at the same value it had when this function
was entered.

Both text1 and source are optional strings.  If either is NULL then the
corresponding line in the error message isn't printed.
*******************************************************************************
*/
void socket_error(char *text,char *text1,char *source)
{
   int temp;
   if(text == NULL)
      return;

   temp = errno;
#ifdef REALIX
#else
   fprintf(stderr,"%s\n",text);
   if(source != NULL)
      perror(source);
   if(text1 != NULL)
      fprintf(stderr,"%s\n",text1);
#endif
   errno = temp;
   return;
}
