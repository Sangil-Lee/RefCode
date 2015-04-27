/* @(#)socketcom.h	1.6 10/04/01
******************************************************************************
FILE: socketcom.h

Definitions used by standard PCS socket communication functions.
------------------------------------------------------------------------------
Modifications:
5/16/2000: created by J. Ferron
******************************************************************************
*/
#include <sys/types.h>
#include <sys/time.h>
#include "socketbasic.h"
#include "descriptors.h"

#ifndef _SOCKETCOM_
#define _SOCKETCOM_
/*
-----------------------------------------------------------
Function prototypes for socket routines.
-----------------------------------------------------------
*/
/*
Functions used by a client to communicate with a server.
*/
extern void *exchange_message(char *, int, int *,
			      char *, u_short, char *);
extern int pcs_send_message(char *, int, char *, u_short, char *);
/*
Functions for a server.
*/
extern int wait_for_input_connection(int sock, struct timeval *to, char *text,
				     int *msgsock);
extern int server_get_message(int msgsock, 
			      processor_type_mask *client_type_output,
			      char **message_output, 
			      int *message_length_output,
			      char *error_string);
extern int server_get_message_type(char *message_storage,
				   processor_type_mask client_type);
/*
Functions used by a server to reply to a client.
*/
extern int send_data_list(int msgsock, 
			  struct data_queue_entry **data_queue_in);
extern void add_to_data_list(STRUCT_DESCRIPTORS *descriptor,  char *data_in,
			     int target,
			     struct data_queue_entry **data_queue_in);
#endif /* end of #ifndef _SOCKETCOM_ */
