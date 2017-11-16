/*
dcom2_scanner.c

scan for second dcom vulnerability (MS03-039)

by Doke Scott, doke at udel.edu, 10 Sep 2003

based on work by:
 *  buildtheb0x presents : dcom/rpc scanner
 *  ---------------------------------------
 *  by: kid and farp

 and on packet sniffs of MS's dcom2 scanner

*/

#define d_dcom_scan_timeout 5  // max seconds for individual dcom scan

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define null NULL


// for sun spro cc wierdness?  seg faults without this
#define my_inet_ntoa(ip) inet_ntoa( *( (struct in_addr *) &ip ) ) 



static char *program_name;
static int verbose = 0;
int dcom_scan_timeout = d_dcom_scan_timeout;
volatile int timed_out = 0;
volatile int dcomsockfd = 0;

extern char *optarg;
extern int optind, opterr, optopt;





void 
print_hex( unsigned char *data, int len ) { 
    int i, j;
    char alphastr[ 17 ];

    for ( i = 0, j = 0; i < len; i++, j++ ) { 
	if ( j == 0 ) { 
	    alphastr[ j ] = isprint( data[i] ) ? data[i] : '.';
	    printf( "%04x  %02x", i, data[ i ] & 0xff );
	    }
	else if ( j == 15 ) { 
	    alphastr[ j ] = isprint( data[i] ) ? data[i] : '.';
	    alphastr[ j + 1 ] = 0;
	    printf( " %02x   %s\n", data[ i ] & 0xff, alphastr );
	    j = -1;
	    }
	else { 
	    alphastr[ j ] = isprint( data[i] ) ? data[i] : '.';
	    printf( " %02x", data[ i ] & 0xff );
	    }
	}
    if ( j )  { 
	alphastr[ j + 1 ] = 0;
	for ( ; j < 16; j++ ) 
	    printf( "   " ); 
	printf( "   %s\n", alphastr );
	}
    }




/*
 * based on:
 *
 *  buildtheb0x presents : dcom/rpc scanner
 *  ---------------------------------------
 *
 *
 *  by: kid and farp
 *
 *  greets: kajun, phr_, dvdman, Sam, flatline, #nanog, synD, and to all danny'
s waitress's
 *
 */

#define DEST_PORT 135

///////////////////////////
// first request packet, bind request

// from dcom1 exploit code
unsigned char bindstr[]={
    0x05, 0x00, 0x0B, 0x03, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
    0x7F, 0x00, 0x00, 0x00, 0xD0, 0x16, 0xD0, 0x16, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0xa0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5D, 0x88, 0x8A, 0xEB, 0x1C, 0xC9, 0x11,
    0x9F, 0xE8, 0x08, 0x00, 0x2B, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
    };

// from dcom1 dcom_scanz
unsigned char fear1[] = {
    0x05, 0x00, 0x0b, 0x03, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0xd0, 0x16, 0xd0, 0x16, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0xb8, 0x4a, 0x9f, 0x4d,
    0x1c, 0x7d, 0xcf, 0x11, 0x86, 0x1e, 0x00, 0x20, 0xaf, 0x6e, 0x7c, 0x57,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11,
    0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
    };

// sniffed from dcom2 scanner, when scanning patched machine
unsigned char request1[] = {
    0x05, 0x00, 0x0b, 0x03, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xd0, 0x16, 0xd0, 0x16, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xa0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11,
    0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
    };

unsigned char expected1[] = { 
    0x05, 0x00, 0x0c, 0x03, 0x10, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xd0, 0x16, 0xd0, 0x16, 0x28, 0x57, 0x00, 0x00,
    0x04, 0x00, 0x31, 0x33, 0x35, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11,
    0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00,
    };


///////////////////////////
// second request packet 

// from dcom1 exploit code
unsigned char exploit_request1[]={
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6b, 0xac, 0xd8, 0x08, 0x2f, 0x2e, 0x03, 0x48, 0xaa, 0xdc, 0xc1, 0x6a,
    0x62, 0xfb, 0xeb, 0x98, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x91, 0x7b, 0x5a,
    0x00, 0xff, 0xd0, 0x11, 0xa9, 0xb2, 0x00, 0xc0, 0x4f, 0xb6, 0xe6, 0xfc,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x38, 0xff, 0x0a, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x07, 0x00
    };


// from dcom1 dcom_scanz
unsigned char fear2[] = {
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00,
    0x09, 0x00, 0x00, 0x00, 0x66, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x6b, 0xac, 0xd8, 0x08, 0x2f, 0x2e, 0x03, 0x48, 0xaa, 0xdc, 0xc1, 0x6a,
    0x62, 0xfb, 0xeb, 0x98, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x91, 0x7b, 0x5a,
    0x00, 0xff, 0xd0, 0x11, 0xa9, 0xb2, 0x00, 0xc0, 0x4f, 0xb6, 0xe6, 0xfc,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x38, 0xff, 0x0a, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x07, 0x00 
    };

// sniffed from dcom2 scanner, when scanning unpatched machine
unsigned char request2[] = {
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0xb0, 0x03, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x98, 0x03, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xe0, 0xab, 0x14, 0x00, 0x68, 0x03, 0x00, 0x00, 0x68, 0x03, 0x00, 0x00,
    0x4d, 0x45, 0x4f, 0x57, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x38, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0x00, 0x00,
    0x30, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x03, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x01, 0x8d, 0x00, 0xb8, 0x01, 0x8d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0xb9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xab, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0xa5, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0xa6, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xa4, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0xad, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0xaa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x07, 0x00, 0x00, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
    0x58, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x5d, 0x88, 0x9a, 0xeb, 0x1c, 0xc9, 0x11, 0x9f, 0xe8, 0x08, 0x00,
    0x2b, 0x10, 0x48, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xb8, 0x47, 0x0a, 0x00, 0x58, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xcc, 0xcc, 0xcc, 0xcc,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xba, 0x09, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x4d, 0x45, 0x4f, 0x57, 0x04, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x3b, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x67, 0x3c, 0x70, 0x94, 0x13, 0x33, 0xfd, 0x46,
    0x87, 0x24, 0x4d, 0x09, 0x39, 0x88, 0x93, 0x9d, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x48, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x7e, 0x09, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x89, 0x0a, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x73, 0x00, 0x61, 0x00,
    0x6a, 0x00, 0x69, 0x00, 0x61, 0x00, 0x64, 0x00, 0x65, 0x00, 0x76, 0x00,
    0x5f, 0x00, 0x78, 0x00, 0x38, 0x00, 0x36, 0x00, 0x00, 0x00, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc,
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x58, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x5e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x5c, 0x00,
    0x00, 0x00, 0x5c, 0x00, 0x6a, 0x00, 0x69, 0x00, 0x61, 0x00, 0x64, 0x00,
    0x65, 0x00, 0x76, 0x00, 0x5f, 0x00, 0x78, 0x00, 0x00, 0x00, 0x36, 0x00,
    0x5c, 0x00, 0x70, 0x00, 0x75, 0x00, 0x62, 0x00, 0x6c, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x5c, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x15, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x90, 0x5b, 0x09, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x6c, 0x00, 0xc0, 0xdf, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00
    };

// sniffed from dcom2 scanner, when scanning unpatched machine
unsigned char expected2_vulnerable[] = { 
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x08, 0x80,
    };

// sniffed from dcom2 scanner, when scanning patched machine
// you also get this on a machine that doesn't have either dcom patch
unsigned char expected2_patched[] = { 
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x01, 0x04, 0x80,
    };


// this came out of one machine, 128.175.214.151
// 0000  05 00 03 23 10 00 00 00 20 00 00 00 01 00 00 00   ...#.... .......
// 0010  00 00 00 00 00 00 00 00 03 00 01 1c 00 00 00 00   ................
// ethereal says it's a dce fault response
// nmap says the system is Me, Win 2000, or WinXP
// all the following packets were unanswered, or zero length.

unsigned char expected2_dce_fault[] = { 
    0x05, 0x00, 0x03 
    };



///////////////////////////
// third request packet 


// from dcom1 dcom_scanz
unsigned char fear3[] = {
    0x05, 0x00, 0x0b, 0x03, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
    0x65, 0x45, 0x79, 0x65, 0xd0, 0x16, 0xd0, 0x16, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0xb8, 0x4a, 0x9f, 0x4d,
    0x1c, 0x7d, 0xcf, 0x11, 0x86, 0x1e, 0x00, 0x20, 0xaf, 0x6e, 0x7c, 0x57,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11,
    0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00
    };

// sniffed from dcom2 scanner, when scanning patched machine
unsigned char request3[] = {
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0xb0, 0x03, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x98, 0x03, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
    0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x98, 0xe5, 0x14, 0x00, 0x68, 0x03, 0x00, 0x00, 0x68, 0x03, 0x00, 0x00,
    0x4d, 0x45, 0x4f, 0x57, 0x04, 0x00, 0x00, 0x00, 0xa2, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x38, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x38, 0x03, 0x00, 0x00,
    0x30, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0xc8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x30, 0x03, 0x00, 0x00, 0xd8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x18, 0x01, 0x8d, 0x00, 0xb8, 0x01, 0x8d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0xb9, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xab, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0xa5, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0xf6, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xff, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0xad, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0xaa, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0x07, 0x00, 0x00, 0x00,
    0x60, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00, 0x00, 0x90, 0x00, 0x00, 0x00,
    0x58, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00,
    0x30, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x5d, 0x88, 0x9a, 0xeb, 0x1c, 0xc9, 0x11, 0x9f, 0xe8, 0x08, 0x00,
    0x2b, 0x10, 0x48, 0x60, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xb8, 0x47, 0x0a, 0x00, 0x58, 0x00, 0x00, 0x00, 0x05, 0x00, 0x06, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46, 0xcc, 0xcc, 0xcc, 0xcc,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xba, 0x09, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00,
    0x4d, 0x45, 0x4f, 0x57, 0x04, 0x00, 0x00, 0x00, 0xc0, 0x01, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46,
    0x3b, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x46, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00, 0x67, 0x3c, 0x70, 0x94, 0x13, 0x33, 0xfd, 0x46,
    0x87, 0x24, 0x4d, 0x09, 0x39, 0x88, 0x93, 0x9d, 0x02, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x48, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x7e, 0x09, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x89, 0x0a, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x00, 0x73, 0x00, 0x61, 0x00,
    0x6a, 0x00, 0x69, 0x00, 0x61, 0x00, 0x64, 0x00, 0x65, 0x00, 0x76, 0x00,
    0x5f, 0x00, 0x78, 0x00, 0x38, 0x00, 0x36, 0x00, 0x00, 0x00, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc,
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x10, 0x08, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x58, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xc0, 0x5e, 0x0a, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x1b, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x5c, 0x00,
    0x00, 0x00, 0x5c, 0x00, 0x6a, 0x00, 0x69, 0x00, 0x61, 0x00, 0x64, 0x00,
    0x65, 0x00, 0x76, 0x00, 0x5f, 0x00, 0x78, 0x00, 0x00, 0x00, 0x36, 0x00,
    0x5c, 0x00, 0x70, 0x00, 0x75, 0x00, 0x62, 0x00, 0x6c, 0x00, 0x69, 0x00,
    0x63, 0x00, 0x5c, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00, 0x41, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x15, 0x00, 0x01, 0x10, 0x08, 0x00,
    0xcc, 0xcc, 0xcc, 0xcc, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x90, 0x5b, 0x09, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x6c, 0x00, 0xc0, 0xdf, 0x08, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00
    };

// this is what you get when it's doesn't have either patch
unsigned char expected3_vulnerable[] = { 
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x54, 0x01, 0x04, 0x80,
    };

// this is what you get when it's got the first dcom patch
unsigned char expected3_patched1[] = { 
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x08, 0x80,
    };

// this is what you get when it's got the second dcom patch
unsigned char expected3_patched2[] = { 
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x02, 0x40, 0x00, 0x80,
    };



///////////////////////////
// fourth request packet 

// from dcom1 dcom_scanz
unsigned char fear4[] = {
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xae, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x5b, 0x52, 0x65, 0x74, 0x69, 0x6e, 0x61, 0x5d, 0x5b, 0x52, 0x65, 0x74,
    0x69, 0x6e, 0x61, 0x5d, 0x00, 0x00, 0x00, 0x00, 0x65, 0x45, 0x79, 0x65,
    0x32, 0x30, 0x30, 0x33, 0x65, 0x45, 0x79, 0x65, 0x32, 0x30, 0x30, 0x33,
    0x68, 0x0f, 0x0b, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x1e, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x41, 0x00, 0x00, 0x00,
    0x5c, 0x00, 0x00, 0x00, 0x63, 0x00, 0x24, 0x00, 0x5c, 0x00, 0x65, 0x00,
    0x45, 0x00, 0x79, 0x00, 0x65, 0x00, 0x5f, 0x00, 0x32, 0x00, 0x30, 0x00,
    0x30, 0x00, 0x33, 0x00, 0x5f, 0x00, 0x52, 0x00, 0x65, 0x00, 0x74, 0x00,
    0x69, 0x00, 0x6e, 0x00, 0x61, 0x00, 0x2e, 0x00, 0x74, 0x00, 0x78, 0x00,
    0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0xb8, 0xeb, 0x0b, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x07, 0x00 
    };


// sniffed from dcom2 scanner, when scanning vulnerable machine
// only issued at vulnerable machine
// no fourth packet when scanning patched machine
// etheral says it's an "alter context"
unsigned char request4[] = {
    0x05, 0x00, 0x0e, 0x03, 0x10, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0xd0, 0x16, 0xd0, 0x16, 0x28, 0x57, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0xb8, 0x4a, 0x9f, 0x4d,
    0x1c, 0x7d, 0xcf, 0x11, 0x86, 0x1e, 0x00, 0x20, 0xaf, 0x6e, 0x7c, 0x57,
    0x00, 0x00, 0x00, 0x00, 0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11,
    0x9f, 0xe8, 0x08, 0x00, 0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00,
    };

unsigned char expected4[] = {
    0x05, 0x00, 0x0f, 0x03, 0x10, 0x00, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0xd0, 0x16, 0xd0, 0x16, 0x28, 0x57, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x5d, 0x88, 0x8a, 0xeb, 0x1c, 0xc9, 0x11, 0x9f, 0xe8, 0x08, 0x00,
    0x2b, 0x10, 0x48, 0x60, 0x02, 0x00, 0x00, 0x00,
    };



///////////////////////////
// fifth request packet 

// sniffed from dcom2 scanner, when scanning vulnerable machine
unsigned char request5[] = {
    0x05, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 0x00, 0x9a, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x82, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x05, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x95, 0x96, 0x95, 0x2a,
    0x8c, 0xda, 0x6d, 0x4a, 0xb2, 0x36, 0x19, 0xbc, 0xaf, 0x2c, 0x2d, 0xea,
    0x34, 0xeb, 0x98, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x07, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x5c, 0x00, 0x4d, 0x00, 0x45, 0x00,
    0x4f, 0x00, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x5c, 0x00, 0x48, 0x00, 0x5c, 0x00, 0x48, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x58, 0xe9, 0x98, 0x00, 0x01, 0x00, 0x00, 0x00, 0x95, 0x96, 0x95, 0x2a,
    0x8c, 0xda, 0x6d, 0x4a, 0xb2, 0x36, 0x19, 0xbc, 0xaf, 0x2c, 0x2d, 0xea,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x5c, 0x00,
    };

// this is what you get with no dcom patches, 
// unfortunately it's also what you get with the dcom 2 patch
unsigned char expected5_vulnerable_or_patched2[] = {
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x02, 0x00, 0x54, 0x01, 0x04, 0x80,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

// this is what you get from dcom patch 1 
unsigned char expected5_patched1[] = {
    0x05, 0x00, 0x02, 0x03, 0x10, 0x00, 0x00, 0x00, 0x5c, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x02, 0x00, 0x04, 0x00, 0x08, 0x80,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };






void
timeout_handler( int info ) {
    //fprintf( stderr, "timed out\n" );
    if ( dcomsockfd )
	close( dcomsockfd );   // have to close it here to abort the connect
    timed_out = 1;
    }


// send a packet, and get response
// return length of received data, or -1 on error
int
exchange_packets( int pktnum, uint32_t ip, int fd, unsigned char *req, 
	int reqlen, unsigned char *resp, int resplen ) { 
    int len;

    if ( verbose > 1 )
	printf( "Sending packet %d\n", pktnum );

    if(send(dcomsockfd, req, reqlen, 0) < 0) {
	close( dcomsockfd );
	alarm( 0 );
	if ( timed_out )
	    printf( "timed out while sending packet %d to %s\n",
		pktnum, my_inet_ntoa( ip ) );
	else
	    fprintf( stderr, "error sending packet %d to %s\n", 
		pktnum, my_inet_ntoa( ip ) );
	return -1;
	}

    if ( ( len = recv( dcomsockfd, resp, resplen, 0 ) ) < 0 ) {
	close( dcomsockfd );
	alarm( 0 );
	if ( timed_out )
	    printf( "timed out while receiving packet %d from %s\n",
		pktnum, my_inet_ntoa( ip ) );
	else
	    fprintf( stderr, "error receiving packet %d from %s\n", 
		pktnum, my_inet_ntoa( ip ) );
	return -1;
	}

    return len;
    }





// scan remote ip for dcom vulnerability
// normally doesn't print anything, just errors
// verbose = 1 for basic scan result printfs
// verbose > 1 for more verbose stuff
// return 0 if ok, 1 if vulnerable, -1 if can't connect or can't tell
//     0 not vulnerable
//     1 does not accept DCE RPC protocol (connection refused)
//     2 no response (filtering DCOM port, or not there)
//     3 vulnerable to dcom 1 and dcom2
//     4 vulnerable to dcom 2 (but patched for dcom1)
//     255 can't tell for some other reason
int
dcom2_scan( uint32_t ip ) {
    struct sockaddr_in dest_addr;   /* hold dest addy */
    unsigned char resp1[1024];
    unsigned char resp2[1024];
    unsigned char resp3[1024];
    unsigned char resp4[1024];
    unsigned char resp5[1024];
    unsigned char assoc_group[4];
    int len1, len2, len3, len4, len5;
    int vun3 = 0;
    int i;

    if ( verbose > 1 )
	printf( "scanning %s\n", my_inet_ntoa( ip ) );

    timed_out = 0;
    signal( SIGALRM, timeout_handler );
    alarm( dcom_scan_timeout );

    dcomsockfd = 0;
    if((dcomsockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	alarm( 0 );
	if ( timed_out ) { 
	    if ( verbose )
		printf( "%s timed out while getting socket\n", 
		    my_inet_ntoa( ip ) );
	    }
	else
	    fprintf( stderr, "error getting socket: %s\n", strerror( errno ) );
        return 255;
        }

    bzero( &dest_addr, sizeof( struct sockaddr_in ) );
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    dest_addr.sin_addr.s_addr = ip;

    if ( verbose > 1 )
	printf("Connecting to %s\n", my_inet_ntoa( ip ) );
    if( connect( dcomsockfd, (struct sockaddr *) &dest_addr,
	    sizeof(struct sockaddr) ) < 0 ) {
	close( dcomsockfd );
	alarm( 0 );
	if ( timed_out || errno == ETIMEDOUT ) {
	    if ( verbose ) 
		printf( "%s timed out while connecting\n", 
		    my_inet_ntoa( ip ) );
	    return 2;
	    }
	else if ( errno == ECONNREFUSED ) { 
	    if ( verbose )
		printf("%s does not accept DCERPC protocol -- good\n",
		    my_inet_ntoa( ip ) );
	    return 1;   // good, port not open, it's not vulnerable
	    }
	if ( verbose )
	    printf( "%s connect failed: %s\n", my_inet_ntoa( ip ), 
		strerror( errno ) );
	return 255;
	}

#if 0 
    // the ms scanner opens a tcp connections, resets it, then opens a second
    // but it just refused the second connection for me.

    close( dcomsockfd );

    if ( verbose > 1 )
	printf("opening second connection to %s\n", my_inet_ntoa( ip ) );
    if( connect( dcomsockfd, (struct sockaddr *) &dest_addr,
	    sizeof(struct sockaddr) ) < 0 ) {
	close( dcomsockfd );
	alarm( 0 );
	if ( timed_out ) {
	    if ( verbose ) 
		printf( "%s timed out while connecting\n", 
		    my_inet_ntoa( ip ) );
	    return 2;
	    }
	else if ( errno == ECONNREFUSED ) { 
	    if ( verbose ) 
		printf("%s does not accept DCERPC protocol -- good\n",
		    my_inet_ntoa( ip ) );
	    return 1;   // good, it's not vulnerable
	if ( verbose )
	    printf( "%s connect failed: %s\n", my_inet_ntoa( ip ), 
		strerror( errno ) );
	return 255;
	}
#endif


    // the bind request
    len1 = exchange_packets( 1, ip, dcomsockfd, request1, sizeof(request1),
	resp1, sizeof( resp1 ) );
    if ( len1 < 0 ) 
	return 255;
    memcpy( assoc_group, resp1 + 20, 4 );  // need this for packet 4
    //printf( "association group:\n" );
    //print_hex( assoc_group, 4 );




    // packet exchange 2, call_id: 1 opnum: 4 ctx_id: 0
    len2 = exchange_packets( 2, ip, dcomsockfd, request2, sizeof(request2),
	resp2, sizeof( resp2 ) );
    if ( len2 < 0 ) 
	return 255;
    if ( ! memcmp( resp2, expected2_patched, 
	    sizeof( expected2_patched ) ) ) {
	// it's probably either patched for dcom 2, or neigther
	if ( verbose > 1 )
	    printf( "response 2 matches expected patched\n" );
	}
    else if ( ! memcmp( resp2, expected2_vulnerable, 
	    sizeof( expected2_vulnerable ) ) ) {
	// it's probably patched for dcom 1
	if ( verbose > 1 )
	    printf( "response 2 matches expected vulnerable\n" );
	}
    else if ( ! memcmp( resp2, expected2_dce_fault, 
	    sizeof( expected2_dce_fault ) ) ) {
	// It's a DCE fault response.  I don't know what causes this,
	// but it seems to ignore any other packets.  So I can't continue.
	close( dcomsockfd );
	if ( verbose > 1 )
	    printf( "response 2 matches dce fault\n" );
	if ( verbose )
	    printf( "%s returns DCE Fault code -- wierd\n", 
		my_inet_ntoa( ip ) );
	return 255;
	}
    else if ( verbose > 1 ) { 
	printf( "Response 2 expected vunerable contents:\n" );
	print_hex( expected2_vulnerable, sizeof( expected2_vulnerable ) );
	printf( "Response 2 expected patched contents:\n" );
	print_hex( expected2_patched, sizeof( expected2_patched ) );
	printf( "Response 2 received contents:\n" );
	print_hex( resp2, len2 );
	for ( i = 0; i < sizeof( expected2_vulnerable ); i++ ) { 
	    if ( resp2[ i ] != expected2_vulnerable[ i ] ) 
		printf( "    %04x %02x != %02x\n", i, resp2[ i ], 
		    expected2_vulnerable[ i ] );
	    }
	}



    // packet exchange 3, call_id: 2 opnum: 4 ctx_id: 0
    len3 = exchange_packets( 3, ip, dcomsockfd, request3, sizeof(request3),
	resp3, sizeof( resp3 ) );
    if ( len3 < 0 ) 
	return 255;
    if ( ! memcmp( resp3, expected3_vulnerable, 
	    sizeof( expected3_vulnerable ) ) ) {
	if ( verbose > 1 )
	    printf( "response 3 matches no dcom patches\n" );
	vun3 = 1;
	}
    else if ( ! memcmp( resp3, expected3_patched1, 
	    sizeof( expected3_patched1 ) ) ) {
	if ( verbose > 1 )
	    printf( "response 3 matches patched for dcom 1\n" );
	vun3 = 1;
	}
    else if ( ! memcmp( resp3, expected3_patched2, 
	    sizeof( expected3_patched2 ) ) ) {
	if ( verbose > 1 )
	    printf( "response 3 matches patched for dcom 2\n" );

	// MS scanner stops here 
	// but I don't really understand these packets

	//if ( verbose )
	//    printf( "%s has both dcom patchs -- good\n", my_inet_ntoa( ip ) );
	//close( dcomsockfd );
	//return 0;
	}
    else if ( verbose > 1 ) { 
	printf( "response 3 does not match any expected packet\n" );
	printf( "Response 3 received contents:\n" );
	print_hex( resp3, len3 );
	for ( i = 0; i < sizeof( expected3_vulnerable ); i++ ) { 
	    if ( resp3[ i ] != expected3_vulnerable[ i ] ) 
		printf( "    %04x %02x != %02x\n", i, resp3[ i ], 
		    expected3_vulnerable[ i ] );
	    }
	}
 


    // packet exchange 4, Alter context 
    memcpy( request4 + 20, assoc_group, 4 );
    memcpy( expected4 + 20, assoc_group, 4 );
    len4 = exchange_packets( 4, ip, dcomsockfd, request4, sizeof(request4),
	resp4, sizeof( resp4 ) );
    if ( len4 < 0 ) 
	return 255;
    else if ( verbose > 1 ) { 
	if ( memcmp( resp4, expected4, sizeof( expected4 ) ) ) {
	    printf( "Response 4 expected contents:\n" );
	    print_hex( expected4, sizeof( expected4 ) );
	    printf( "Response 4 received contents:\n" );
	    print_hex( resp4, len4 );
	    for ( i = 0; i < sizeof( expected4 ); i++ ) { 
		if ( resp4[ i ] != expected4[ i ] ) 
		    printf( "    %04x %02x != %02x\n", i, resp4[ i ], 
			expected4[ i ] );
		}
	    }
	}



    // packet exchange 5, RemoteActivation 
    len5 = exchange_packets( 5, ip, dcomsockfd, request5, sizeof(request5),
	resp5, sizeof( resp5 ) );
    if ( len5 < 0 ) 
	return 255;
    close(dcomsockfd);
    if ( ! memcmp( resp5, expected5_patched1, 
	    sizeof( expected5_patched1 ) ) ) {
	if ( verbose > 1 )
	    printf( "response 5 matches patched for dcom1 -- BAD\n" );
	if ( verbose )
	    printf( "** %s only has 1st dcom patch -- BAD **\n", 
		my_inet_ntoa( ip ) );
	return 4;
	}
    else if ( ! memcmp( resp5, expected5_vulnerable_or_patched2, 
	    sizeof( expected5_vulnerable_or_patched2 ) ) ) {
	if ( verbose > 1 )
	    printf( "response 5 matches either no dcom patches or patched for dcom2\n" );
	if ( vun3 ) { 
	    if ( verbose )
		printf( "** %s has neither dcom patch -- BAD **\n", 
		    my_inet_ntoa( ip ) );
	    return 3;
	    }
	if ( verbose )
	    printf( "%s has both dcom patchs -- good\n", my_inet_ntoa( ip ) );
	return 0;
	}
    else if ( verbose > 1 ) { 
	printf( "Response 5 expected contents:\n" );
	print_hex( expected5_vulnerable_or_patched2, 
	    sizeof( expected5_vulnerable_or_patched2 ) );
	printf( "Response 5 received contents:\n" );
	print_hex( resp5, len5 );
	for ( i = 0; i < sizeof( expected5_vulnerable_or_patched2 ); i++ ) { 
	    if ( resp5[ i ] != expected5_vulnerable_or_patched2[ i ] ) 
		printf( "    %04x %02x != %02x\n", i, resp5[ i ], 
		    expected5_vulnerable_or_patched2[ i ] );
	    }
	}


    else if ( verbose > 1 ) { 
	printf( "%s contains unidentified signature,\n", 
	    my_inet_ntoa( ip ) );
	printf( "Please report if vulnerable.\n" );
	}
    return 255;
    }





void
usage( int rc ) {
    fprintf( stderr, "Usage: %s [-vqh] [ -t timeout ] <ip address>\n"
	"       %s [-vqh] [ -t timeout ] <ip address>/<cidr-bits>\n" 
	"    -v    increase verbosity\n" 
	"    -q    quiet, no output, just exit status\n" 
	"    -t n  set scan timeout to n seconds, default %d\n"
	"    -h    this help\n" 
	"    when scanning one ip, exits with:\n"
	"        0 not vulnerable\n"
	"        1 does not accept DCE RPC protocol (connection refused)\n"
	"        2 no response (filtering DCOM port, or not there)\n"
	"        3 vulnerable to dcom 1 and dcom2\n"
	"        4 vulnerable to dcom 2 (but patched for dcom1)\n"
	"        255 can't tell for some other reason\n"
	"    when scanning an ip range, exits with:\n"
	"        0 nothing was vulnerable\n"
	"        4 one or more were vunerable\n",
	program_name, program_name, d_dcom_scan_timeout );
    exit( rc );
    }





int 
main( int argc, char **argv )  {
    int a, b, c, d, bits; 
    unsigned int mask, low, high, ip, netip;
    int rc = 0, r;

    program_name = argv[0];

    verbose = 1;  // turn on basic prints in scan function 
    dcom_scan_timeout = d_dcom_scan_timeout;

    while ( ( c = getopt( argc, argv, "vqt:h" ) ) >= 0 ) {
	switch ( c ) {
	    case 'v':
		verbose++;
		break;
	    case 'q':
		verbose = 0;
		break;
	    case 't': 
		dcom_scan_timeout = atoi( optarg );
		break;
	    case 'h': 
		usage( 0 );
		break;
	    default:
		usage( -1 );
		break;
	    }
	}

    if ( optind >= argc || ! argv[ optind ] ) 
	usage( -1 );
 
    rc = sscanf( argv[ optind ], "%d.%d.%d.%d/%d", &a, &b, &c, &d, &bits );
    if ( rc == 5 )  { 
	// scan range
	if ( bits < 0 || 32 < bits )
	    usage( -1 );
	rc = 0;
	mask = 0xffffffff << ( 32 - bits );
	low = ( a << 24 | b << 16 | c << 8 | d ) & mask;
	high = low | ~ mask;
	for ( ip = low + 1; ip < high; ip++ ) { 
	    netip = htonl( ip );
	    r = dcom2_scan( netip );
	    if ( r == 3 || r == 4 ) 
		rc = 4;
	    }
	}
    else if ( rc == 4 ) { 
	// scan 1 ip
	inet_pton( AF_INET, argv[ optind ], (struct in_addr *) &netip );
	rc = dcom2_scan( netip );
	}
    else 
	usage( -1 );

    return rc;
    }

// milw0rm.com [2003-09-12]