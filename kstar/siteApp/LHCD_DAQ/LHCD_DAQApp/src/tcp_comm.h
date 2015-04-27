#ifndef __TCP_COMM__
#define __TCP_COMM__

int set_up_tcp(char *ip, int port);
int recv_data_get(int sock, char *temp, int count);

#endif
