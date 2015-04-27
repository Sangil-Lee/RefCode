/*
 *  msgqput.c
 *  puts a message into a #defined queue, creating it if user
 *  requested. The message is associated a priority still user
 *  defined
 *  Created by Mij <mij@bitchx.it> on 07/08/05.
 *  Original source file available on http://mij.oltrelinux.com/devel/unixprg/
 *  Edited by leesi <leesi@nfrc.re.kr>
 */

#include <stdio.h>
/* mq_* functions */
#include <mqueue.h>
#include <sys/stat.h>
/* exit() */
#include <stdlib.h>
/* getopt() */
#include <unistd.h>
/* ctime() and time() */
#include <time.h>
/* strlen() */
#include <string.h>


/* name of the POSIX object referencing the queue */
#define MSGQOBJ_NAME    "/labview_1"

/* max length of a message (just for this process) */
#define MAX_MSG_LEN     70


int main(int argc, char *argv[]) 
{
    mqd_t msgq_id;
    unsigned int msgprio = 0;
    pid_t my_pid = getpid();
    /*char msgcontent[MAX_MSG_LEN];*/
    double msgcontent[MAX_MSG_LEN];
    int create_queue = 0;
    char ch;            /* for getopt() */
    
    
    /* accepting "-q" for "create queue", requesting "-p prio" for message priority */
    while ((ch = getopt(argc, argv, "qp:")) != -1) 
	{
        switch (ch) {
            case 'q':   /* create the queue */
                create_queue = 1;
                break;
            case 'p':   /* specify client id */
                printf("I (%d), arg:%s\n", my_pid, optarg);
                msgprio = (unsigned int)strtol(optarg, (char **)NULL, 10);
                printf("I (%d) will use priority %d\n", my_pid, msgprio);
                break;
            default:
                printf("Usage: %s [-q] -p msg_prio\n", argv[0]);
                exit(1);
        };
    };
    
    /* forcing specification of "-i" argument */
    if (msgprio == 0) 
	{
        printf("Usage-1: %s [-q] -p msg_prio\n", argv[0]);
        exit(1);
    };
    
    /* opening the queue        --  mq_open() */
    if (create_queue) {
        /* mq_open() for creating a new queue (using default attributes) */
        msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG, NULL);
    } else {
        /* mq_open() for opening an existing queue */
        msgq_id = mq_open(MSGQOBJ_NAME, O_RDWR);
    }
    if (msgq_id == (mqd_t)-1) {
        perror("In mq_open()");
        exit(1);
    }

    /* producing the message */
/*
    time_t currtime;
    currtime = time(NULL);
    snprintf(msgcontent, MAX_MSG_LEN, "Hello from process %u (at %s).", my_pid, ctime(&currtime));
	double test = 10.0;
	int i = 0;
	for (i = 0; i<MAX_MSG_LEN;i++)
	{
		msgcontent[i] = test+i;
	}
    mq_send(msgq_id, msgcontent, strlen(msgcontent)+1, msgprio);
*/ 
    /* sending the message      --  mq_send() */
	while(1)
	{
		int i;
		for (i=0; i<MAX_MSG_LEN;i++)
		{
			msgcontent[i] = rand()/1000000.3423;
			printf("Send[%d]:%f\n",i,msgcontent[i]);
		};
		printf("%d\n", (mqd_t)msgq_id);
		mq_send(msgq_id, (const char*)msgcontent, sizeof(msgcontent), msgprio);
		sleep(1);
	};
    
    /* closing the queue        -- mq_close() */
    mq_close(msgq_id);
    return 0;
}
