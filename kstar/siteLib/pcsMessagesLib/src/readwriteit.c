/* @(#)readwriteit.c	1.6 08/03/01
******************************************************************************
FILE: readwriteit.c

Read and write primitives for communication through the socket.
If interrupted before the desired read or write is finished, these
routines restart.

------------------------------------------------------------------------------
Modifications:
2/6/93: created
9/9/94: handle interrupted system call properly so that this code
        will work under both SYSV and SunOS
5/4/00: JRF add a test in readit for a read returning 0 bytes.
******************************************************************************
*/
#define DEBUGLINES 0

#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/*
******************************************************************************
SUBROUTINE: readit

Read a specified number of bytes from the socket. If the number of bytes
requested is 0, this function simply returns.

If the read is interrupted before finishing, then it is restarted.

If the read returns 0 bytes too often, then give up.  (This is a problem
that has been observed but which is not understood. 
It could perhaps happen if the
transmitting process goes away before sending the correct number of bytes.)

******************************************************************************
*/
int readit(int msgsock, char *message, int length)
{
   int rval,remaining,count;
   char *buffer;

   remaining = length;
   buffer = message;
   count = 0;

   while(remaining != 0){
      errno = 0;
      rval = read(msgsock, buffer, remaining);
      if( (rval < 0) && (errno != EINTR) ){
	 return rval;
      }
      else if(rval > 0){
	 buffer = buffer + rval;
	 remaining = remaining - rval;
      }
      else if(rval == 0){
	 count = count + 1;
	 /*
	   The first bunch of times that the number of bytes read is 0,
	   simply try again.  After that, wait awhile between tries.  Increase
	   the wait time as the number of tries increases.  Eventually,
	   give up and
	   return the number of bytes that have been received successfully.
	 */
	 if(count > 20)
	    return length - remaining;
	 else if(count > 10)
	    sleep(count - 10);
      }
   }
   return length;
}

/*
******************************************************************************
SUBROUTINE: writeit

Write a specified number of bytes through the socket.  If the write 
is interrupted before finishing, then restart it.
******************************************************************************
*/
int writeit(int msgsock, char *message, int length)
{
   int i,remaining;
   char *buffer;

   remaining = length;
   buffer = message;

   while(remaining != 0)
   {
      errno = 0;
      i = write(msgsock, buffer, remaining);

      if( (i < 0) && (errno != EINTR) ){
	 return i;
      }
      else if(i >= 0){
#if DEBUGLINES
	 if(i != remaining)
	    fprintf(stderr,
		    "readit: ok interrupted read, errno = %d.\n",errno);
#endif
	 buffer = buffer + i;
	 remaining = remaining - i;
      }
#if DEBUGLINES
      if(errno == EINTR)
	 fprintf(stderr,"readit: handled interrupted read.\n");
#endif
   }
   return length;
}
