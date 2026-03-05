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

open FH, "+>", 'BuildName.txt' or die $!;

$configline = sprintf("MINECRAFTVIT%s,",$buildnum); ##build package name
print FH $configline;

close FH;

## Copy the symbols file

open FH, "+>", 'move_pkg.cmd' or die $!;

$configline = sprintf("mkdir X:\\Builds\\PSVita\\%s\\\n",$buildnum);
print FH $configline;

$configline = sprintf("move X:\\Builds\\PSVita\\%%TERRITORY%%\\BUILD\\EP4433-PCSB00560_00-MINECRAFTVIT0000.pkg X:\\Builds\\PSVita\\%s\\\n",$buildnum,$buildnum);
print FH $configline;
$configline = sprintf("move X:\\Builds\\PSVita\\%%TERRITORY%%\\BUILD\\psvita-pkg-EP4433-PCSB00560_00-MINECRAFTVIT0000-A0100-V0100-spec.xml X:\\Builds\\PSVita\\%s\\\n",$buildnum,$buildnum);
print FH $configline;
$configline = sprintf("move X:\\Builds\\PSVita\\%%TERRITORY%%\\BUILD\\psvita-pkg-EP4433-PCSB00560_00-MINECRAFTVIT0000-A0100-V0100-submission_materials.zip X:\\Builds\\PSVita\\%s\\\n",$buildnum,$buildnum);
print FH $configline;
$configline = sprintf("move X:\\Builds\\PSVita\\%%TERRITORY%%\\%%TERRITORY%%.self X:\\Builds\\PSVita\\%s\\\n",$buildnum,$buildnum);
print FH $configline;

close FH;
