#include <vxWorks.h> 
#include <time.h> 
#include <timers.h> 
#include <socket.h> 
#include <sockLib.h> 
#include <in.h> 
#include <stdio.h> 
#include <string.h> 
#include <hostLib.h> 
#include <ioLib.h> 


static char const rcsid[] = "$Id: rdate.c,v 1.1 1995/09/27 04:03:35 
mwette Exp $"; 


#define IPPORT_TIMECLIENT 37 


extern char sysBootParams; 


int rdate1(void) 
{ 
  int blen = 256; 
  struct sockaddr_in sockAddr; 
  struct timespec tp; 
  int sock, n, size, status; 
  unsigned long t; 
  char *bufr; 
  static char *BOOT_HOST = &sysBootParams + 20; 
  char *name; 


  sock = socket(AF_INET, SOCK_STREAM, 0); 
  if (sock == -1) { 
    perror("rdate: socket() failed"); 
    return -1; 
  } 


  memset(&sockAddr, '\0', sizeof(sockAddr)); 
  sockAddr.sin_family = AF_INET; 
  sockAddr.sin_port = htons(IPPORT_TIMECLIENT); 
  sockAddr.sin_addr.s_addr = hostGetByName("172.17.100.119"); 
  printf ("%d \n", sockAddr.sin_addr.s_addr); 


  if (bind (sock, (struct sockaddr *)&sockAddr, sizeof (sockAddr)) == 
-1) 
  { 
         perror("rdate: bind() failed"); 
         return -1; 
  } 


  /*status = connect(sock, (struct sockaddr*)&sockaddr, 
sizeof(sockaddr)); */ 
  if (connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == 
-1) { 
    close(sock); 
    perror("rdate: connect() failed"); 
    return -1; 
  } 


  bufr = (char *)&t; 
  size = sizeof(t); 
  while (size) { 
    n = fioRead(sock, bufr, size); 
    if (n < 0) { 
      close(sock); 
      perror("rdate: read() failed"); 
      return -1; 
    } 
    size -= n; 
    bufr += n; 
  } 
  close (sock); 


  /* Adjust for 70 difference (1900->1970) and 1 sec delay. */ 
  tp.tv_sec = (ntohl(t) - 2208988800L) + 1; 
  tp.tv_nsec = 0; 
  clock_settime(CLOCK_REALTIME, &tp); 
  return 0; 



} 

