/* @(#)socketbasic.h	1.10 01/10/02
******************************************************************************
FILE: socketbasic.h

Definitions used by standard PCS socket communication functions.
------------------------------------------------------------------------------
Modifications:
5/17/2000: created by J. Ferron
******************************************************************************
*/
#include <sys/types.h>
#if defined(SOLARIS_2)
#include <sys/inttypes.h>
#else
#include <inttypes.h>
#endif
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#if 1
#include "processor_type.h"
#include "swap_functions.h"
#endif

#ifndef _SOCKETBASIC_
#define _SOCKETBASIC_
/*
------------------------------------------------------------------------------
STRUCTURE: data_queue_entry
MACROS: DATA_LIST_SETUP, ADD_TO_DATA_LIST

Structure to define a queue of data blocks to be transmitted by the
routine send_data_list.
------------------------------------------------------------------------------
*/

struct data_queue_entry{
   char *data;  /* Pointer to the data to be transmitted. */
   int length;  /* Count of bytes to be transmitted. */
   struct data_queue_entry *next; /* Pointer to the next entry in the queue.*/
};
/*
A convenient macro to set up the array of pointers to this type of structure
that is required for calling the function add_to_data_list         .
*/
#define DATA_LIST_SETUP struct data_queue_entry *data_queue[2] = {NULL,NULL};
/*
An abbreviated way to call the function add_to_data_list.  This macro assumes
that the variables client_type and data_queue exist in the function where this
macro is used.
*/
#define ADD_TO_DATA_LIST(descriptor,data) \
   add_to_data_list(descriptor, data, client_type, data_queue);
/*
------------------------------------------------------------------------------
MACROS: ntoh_processor_type, hton_processor_type
        ntoh_message_length, hton_message_length
TYPEDEF: message_length_value

The first 2 portions of a message sent to a server from a client are the
processor type of the client and the number of bytes in the remainder of
the message.  These values are of type processor_type_mask and
message_length_value.  The data types for these values must be chosen to be
types with identical length on each of the possible types of processor.

These macros convert a value from network to host format or from
host to network format. The network format is big endian.

Here we define the type for a message length value
to be an integer. We assume that type int has length of 4 bytes.
A similar definition for processor_type_mask is made in processor_type.h.

These macros also assume the existence of a global variable called
local_processor_type which holds the type mask for the processor on which
the current process is executing.
------------------------------------------------------------------------------
*/
typedef int message_length_value;

#define ntoh_processor_type(type) \
   if( (local_processor_type & LITTLEENDIAN) != 0)\
      swap_i4((char *)&type,4);

#define hton_processor_type(type) \
   if( (local_processor_type & LITTLEENDIAN) != 0)\
      swap_i4((char *)&type,4);

#define ntoh_message_length(type) \
   if( (local_processor_type & LITTLEENDIAN) != 0)\
      swap_i4((char *)&type,4);

#define hton_message_length(type) \
   if( (local_processor_type & LITTLEENDIAN) != 0)\
      swap_i4((char *)&type,4);
/*
-----------------------------------------------------------
Function prototypes
-----------------------------------------------------------
*/
extern int make_connection(int sock,
			   struct sockaddr_in *server,
			   char *text);
extern int establish_socket(
			    char *host,
			    u_short port,
			    char *text,
			    int *sock_out,
			    struct sockaddr_in *server);
extern int transmit_the_message_header(
				       int message_length,
				       int sock,
				       char *text);
extern int transmit_the_message_body(
				     char *message,
				     int length,
				     int sock,
				     char *text);
extern int get_the_reply(
			 void **reply_out,
			 int *reply_length,
			 int sock,
			 char *text);
extern int close_connection(
			    int sock,
			    char *text);
extern void socket_error(char *text,char *text1,char *source);
extern int writeit(int, char *, int);
extern int readit(int, char *, int);

#endif /* end of #ifndef _SOCKETBASIC_ */
