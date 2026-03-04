#! /usr/bin/perl -w
use warnings;
use File::Copy::Recursive qw(fcopy rcopy dircopy fmove rmove dirmove);

open FH, '..\..\Common\BuildVer.h' or die $!;

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

$buildname=sprintf("Minecraft_%02d.%02d.%02d.%s_TU12", $year-100,$mon+1,$mday,$buildnum);

open FH, "+>", 'BuildName.txt' or die $!;

$configline = sprintf("MINECRAFTVIT%s,",$buildnum); ##build package name
print FH $configline;

close FH;
