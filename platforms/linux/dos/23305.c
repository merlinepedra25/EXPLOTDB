/*
source: http://www.securityfocus.com/bid/8906/info

A vulnerability has been reported in thttpd that may allow a remote attacker to execute arbitrary code on vulnerable host. The issue is reported to exist due to a lack of bounds checking by software, leading to a buffer overflow condition. The problem is reported to exist in the defang() function in libhttpd.c.

This issue may allow an attacker to gain unauthorized access to a vulnerable host. Successful exploitation of this issue may allow an attacker to execute arbitrary code in the context of the web server in order to gain unauthorized access to a vulnerable system.

thttpd versions 2.21 to 2.23b1 have been reported to be prone to this issue, however other versions may be affected as well. 
*/

static void
defang( char* str, char* dfstr, int dfsize )
{
    char* cp1;
    char* cp2;

    for ( cp1 = str, cp2 = dfstr;
   *cp1 != '\0' && cp2 - dfstr < dfsize - 1;
   ++cp1, ++cp2 )
     {
     switch ( *cp1 )
         {
         case '<':
         *cp2++ = '&';
         *cp2++ = 'l';
         *cp2++ = 't';
         *cp2 = ';';
         break;
         case '>':
         *cp2++ = '&';
         *cp2++ = 'g';
         *cp2++ = 't';
         *cp2 = ';';
         break;
         default:
         *cp2 = *cp1;
         break;
         }
     }
    *cp2 = '\0';
}