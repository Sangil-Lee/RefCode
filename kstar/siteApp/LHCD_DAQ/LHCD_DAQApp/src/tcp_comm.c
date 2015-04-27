#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/select.h>
#include <sys/time.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "tcp_comm.h"

int set_up_tcp( char *ip, int port )
{
    struct sockaddr_in server;
    int connect_stat;
    int sock;

    sock = socket( PF_INET, SOCK_STREAM, 0 );

    server.sin_family = AF_INET;
    server.sin_port = htons( port );
    server.sin_addr.s_addr = inet_addr(ip);

    if( sock >= 0 )
	{
		connect_stat = connect( sock, (struct sockaddr *)&server, sizeof(struct sockaddr_in) );
	    if( connect_stat < 0 )
	    {
		    close( sock );
        }
	}

    return sock;
}

int recv_data_get( int sock, char *temp, int count)
{
    int ret = 0;
    int ind = 0;

    while(1) {
        ret = recv(sock, &temp[ind], count-ind, 0);
        if(ret <= 0) {
            printf("*** Data get error... ***\n");
            ind = -1;
            break;
        }
        ind += ret;
        if(ind >= count) break;
    }

    return ind;
}
