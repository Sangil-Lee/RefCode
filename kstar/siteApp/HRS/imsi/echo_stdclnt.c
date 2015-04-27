/* echo_stdclnt.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFSIZE 1024
void error_handling(char *message);

int main(int argc, char **argv)
{
	int sock;
	FILE* readFP;
	FILE* writeFP;
	char message[BUFSIZE];

	struct sockaddr_in serv_addr;

	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock == -1)
		error_handling("socket() error");

	/* file pointer conversion from file discripter */
	readFP = fdopen(sock, "r");
	writeFP=fdopen(sock, "w");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error!");

	while(1)
	{
		/* message input, transferring */
		fputs("Enter messages. (q to quit) : ", stdout);
		fgets(message, BUFSIZE, stdin);

		if(!strcmp(message,"q\n")) break;
		fputs(message, writeFP);
		fflush(writeFP);
	
		/* message receive, output */
		fgets(message, BUFSIZE, readFP);
		printf("Received message: %s \n", message);
	}

	fclose(writeFP);
	fclose(readFP);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
