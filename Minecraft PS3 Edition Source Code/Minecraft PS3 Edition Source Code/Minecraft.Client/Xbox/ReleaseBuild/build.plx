#! /usr/bin/perl -w
use warnings;
use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);

open FH, '..\Xbox_BuildVer.h' or die $!;

my $lineno=1;
my $index;

while(<FH>)
{
	$lineno++;

	$index = rindex($_,'#define VER_PRODUCTBUILD ');
	my($line) = $_;
	chomp($line);
	if($index>-1)
	{
		@build = split(/ +/, $line);
		
		$buildnum = sprintf("%4d", $build[2]);
		$buildnum=~ tr/ /0/;
	}
}

my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);

##print 'Build num is Minecraft_', $year-100,'.',$mon+1,'.',$mday,'.',$buildnum;

$buildname=sprintf("Minecraft_%02d.%02d.%02d.%s_Release", $year-100,$mon+1,$mday,$buildnum);
$builddir=sprintf("c:/MinecraftReleaseBuilds/%s",$buildname);


print $builddir;

## now create the build directories

$dir = 'c:/MinecraftReleaseBuilds/';

unless(-d $dir)
{
   mkdir $dir or die;
}

$dir = 'c:/MinecraftReleaseBuilds/pdbs/';

unless(-d $dir)
{
   mkdir $dir or die;
}

$dir = 'c:/MinecraftReleaseBuilds/Zips/';

unless(-d $dir)
{
   mkdir $dir or die;
}

unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftReleaseBuilds/%s/Docs",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/Saves",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

## Now copy the files in 

## docs
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Docs/4J Minecraft XBLA Design Changes & Additions v1.0.docx",$buildname);
fcopy('../Docs/4J Minecraft XBLA Design Changes & Additions v1.0.docx',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011.xls',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011_Asia_SYA111511.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011_Asia_SYA111511.xls',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011_EU.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011_EU.xls',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Docs/Xbox Live Avatar Items - Minecraft.xlsx",$buildname);
fcopy('../Docs/Xbox Live Avatar Items - Minecraft.xlsx',$target) or die $!;

## xex
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/Minecraft_Release.xex",$buildname);
fcopy('../../Release/Minecraft.Client.xex',$target) or die $!;

## game data
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/res/",$buildname);
rcopy('../res/',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/Tutorial/TutorialLevel.mcs",$buildname);
rcopy('../Tutorial/TutorialLevel.mcs',$target) or die $!;
##$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/Trial/TrialLevel.mcs",$buildname);
##rcopy('../Trial/TrialLevel.mcs',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/584111F70AAAAAAA",$buildname);
rcopy('../584111F70AAAAAAA',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/%s/Minecraft360/AvatarAwards",$buildname);
rcopy('../AvatarAwards',$target) or die $!;

## release files
$target=sprintf("c:/MinecraftReleaseBuilds/pdbs/%s/Release/Default.exe",$buildname);
fcopy('../../Release/Minecraft.Client.exe',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/pdbs/%s/Release/Default.pdb",$buildname);
fcopy('../../Release/Minecraft.Client.pdb',$target) or die $!;
$target=sprintf("c:/MinecraftReleaseBuilds/pdbs/%s/Release/Default.xdb",$buildname);
fcopy('../../Release/Minecraft.Client.xdb',$target) or die $!;

# Cheat save
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Cheats/Content/E0000C2E8D782D00/584111F7/00000001/Save2012 11718 748.bin",$buildname);
fcopy('../Cheats/E0000C2E8D782D00/584111F7/00000001/Save2012 11718 748.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Cheats/Content/E0000C2E8D782D00/FFFE07D1/00010000/E0000C2E8D782D00",$buildname);
fcopy('../Cheats/E0000C2E8D782D00/FFFE07D1/00010000/E0000C2E8D782D00',$target) or die $!;

open FH, "+>", 'zipthebuild.cmd' or die $!;

$zipline = sprintf("\"C:\\Program Files (x86)\\winzip\\wzzip\" -p -r -ex c:\\MinecraftReleaseBuilds\\Zips\\%s.zip c:\\MinecraftReleaseBuilds\\%s\\*",$buildname,$buildname);

print FH $zipline;
close FH;
