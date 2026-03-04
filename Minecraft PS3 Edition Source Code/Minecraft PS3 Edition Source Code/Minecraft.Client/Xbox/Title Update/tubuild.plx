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

$buildname=sprintf("Minecraft_%02d.%02d.%02d.%s_TU18", $year-100,$mon+1,$mday,$buildnum);
$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s",$buildname);


print $builddir;

## now create the build directories

## -----------------------------------------
##
## FULL BUILD
## 
## -----------------------------------------

$dir = 'c:/MinecraftSubmissionBuilds/';

unless(-d $dir)
{
   mkdir $dir or die;
}

unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Package",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Test",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Cheats",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}
$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Cheats/Content",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Release",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

## Now copy the files in 

## xlast & gameconfig
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Minecraft.gameconfig",$buildname);
fcopy('../GameConfig/Minecraft.gameconfig',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/MinecraftContentPackage.xlast",$buildname);
fcopy('../ContentPackageBuild/MinecraftContentPackage.xlast',$target) or die $!;

## docs
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft XBLA Design Changes & Additions v1.0.docx",$buildname);
fcopy('../Docs/4J Minecraft XBLA Design Changes & Additions v1.0.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011.xls',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011_Asia_SYA111511.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011_Asia_SYA111511.xls',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/Minecraft_XBLA_TIS_Nov2011_EU.xls",$buildname);
fcopy('../Docs/Minecraft_XBLA_TIS_Nov2011_EU.xls',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/Xbox Live Avatar Items - Minecraft.xlsx",$buildname);
fcopy('../Docs/Xbox Live Avatar Items - Minecraft.xlsx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/Ratings_Submission_Form.doc",$buildname);
fcopy('../Docs/Ratings_Submission_Form.doc',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/PDLC_Request_Form_Nov2010-IronTheme.xls",$buildname);
fcopy('../Docs/PDLC_Request_Form_Nov2010-IronTheme.xls',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/GameOverview_Minecraft.doc",$buildname);
fcopy('../Docs/GameOverview_Minecraft.doc',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft XBLA Sentient Usage v1.0.docx",$buildname);
fcopy('../Docs/4J Minecraft XBLA Sentient Usage v1.0.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft XBLA TMS.docx",$buildname);
fcopy('../Docs/4J Minecraft XBLA TMS.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft 1.7.3 update.docx",$buildname);
fcopy('../Docs/4J Minecraft 1.7.3 update.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft 1.8.2 update.docx",$buildname);
fcopy('../Docs/4J Minecraft 1.8.2 update.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft 1.0.1 update.docx",$buildname);
fcopy('../Docs/4J Minecraft 1.0.1 update.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft 1.2.3 update.docx",$buildname);
fcopy('../Docs/4J Minecraft 1.2.3 update.docx',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Docs/4J Minecraft 1.3.1 update.docx",$buildname);
fcopy('../Docs/4J Minecraft 1.3.1 update.docx',$target) or die $!;



## Achievements
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/01.png",$buildname);
fcopy('../GameConfig/01.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/02.png",$buildname);
fcopy('../GameConfig/02.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/03.png",$buildname);
fcopy('../GameConfig/03.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/04.png",$buildname);
fcopy('../GameConfig/04.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/05.png",$buildname);
fcopy('../GameConfig/05.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/06.png",$buildname);
fcopy('../GameConfig/06.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/07.png",$buildname);
fcopy('../GameConfig/07.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/08.png",$buildname);
fcopy('../GameConfig/08.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/09.png",$buildname);
fcopy('../GameConfig/09.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/10.png",$buildname);
fcopy('../GameConfig/10.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/11.png",$buildname);
fcopy('../GameConfig/11.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/12.png",$buildname);
fcopy('../GameConfig/12.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/13.png",$buildname);
fcopy('../GameConfig/13.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/14.png",$buildname);
fcopy('../GameConfig/14.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/15.png",$buildname);
fcopy('../GameConfig/15.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/16.png",$buildname);
fcopy('../GameConfig/16.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/17.png",$buildname);
fcopy('../GameConfig/17.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/18.png",$buildname);
fcopy('../GameConfig/18.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/19.png",$buildname);
fcopy('../GameConfig/19.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/20.png",$buildname);
fcopy('../GameConfig/20.png',$target) or die $!;

##background
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Minecraft_BKGND.png",$buildname);
fcopy('../GameConfig/Minecraft_BKGND.png',$target) or die $!;

##gamerpics
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/32_584111F70002000100010001.png",$buildname);
fcopy('../GameConfig/32_584111F70002000100010001.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/32_584111F70002000200010002.png",$buildname);
fcopy('../GameConfig/32_584111F70002000200010002.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/64_584111F70002000100010001.png",$buildname);
fcopy('../GameConfig/64_584111F70002000100010001.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/64_584111F70002000200010002.png",$buildname);
fcopy('../GameConfig/64_584111F70002000200010002.png',$target) or die $!;

##boxart
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Minecraft_BOXART.png",$buildname);
fcopy('../GameConfig/Minecraft_BOXART.png',$target) or die $!;

##banner
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/MinecraftMarketplace.png",$buildname);
fcopy('../GameConfig/MinecraftMarketplace.png',$target) or die $!;

## game icon
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/MinecraftIcon.png",$buildname);
fcopy('../GameConfig/MinecraftIcon.png',$target) or die $!;

## avatar icons
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward1_F_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward1_F_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward1_F_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward1_F_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward1_M_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward1_M_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward1_M_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward1_M_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward2_F_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward2_F_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward2_F_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward2_F_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward2_M_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward2_M_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward2_M_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward2_M_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward3_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward3_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/TitleAward3_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward3_icon-128.png',$target) or die $!;

## avatar bin files
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Tshirt5_Porkchop_Female.bin",$buildname);
fcopy('../GameConfig/Tshirt5_Porkchop_Female.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Tshirt5_Porkchop_Male.bin",$buildname);
fcopy('../GameConfig/Tshirt5_Porkchop_Male.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/MineCraft_Watch_Female.bin",$buildname);
fcopy('../GameConfig/MineCraft_Watch_Female.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/MineCraft_Watch_Male.bin",$buildname);
fcopy('../GameConfig/MineCraft_Watch_Male.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Creeper_Cap.bin",$buildname);
fcopy('../GameConfig/Creeper_Cap.bin',$target) or die $!;


## PACKAGE DIR

## Achievements
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/01.png",$buildname);
fcopy('../GameConfig/01.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/02.png",$buildname);
fcopy('../GameConfig/02.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/03.png",$buildname);
fcopy('../GameConfig/03.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/04.png",$buildname);
fcopy('../GameConfig/04.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/05.png",$buildname);
fcopy('../GameConfig/05.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/06.png",$buildname);
fcopy('../GameConfig/06.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/07.png",$buildname);
fcopy('../GameConfig/07.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/08.png",$buildname);
fcopy('../GameConfig/08.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/09.png",$buildname);
fcopy('../GameConfig/09.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/10.png",$buildname);
fcopy('../GameConfig/10.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/11.png",$buildname);
fcopy('../GameConfig/11.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/12.png",$buildname);
fcopy('../GameConfig/12.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/13.png",$buildname);
fcopy('../GameConfig/13.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/14.png",$buildname);
fcopy('../GameConfig/14.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/15.png",$buildname);
fcopy('../GameConfig/15.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/16.png",$buildname);
fcopy('../GameConfig/16.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/17.png",$buildname);
fcopy('../GameConfig/17.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/18.png",$buildname);
fcopy('../GameConfig/18.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/19.png",$buildname);
fcopy('../GameConfig/19.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/20.png",$buildname);
fcopy('../GameConfig/20.png',$target) or die $!;

## arcadeinfo
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/ArcadeInfo.xml",$buildname);
fcopy('../GameConfig/ArcadeInfo.xml',$target) or die $!;

## game icon
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/MinecraftIcon.png",$buildname);
fcopy('../GameConfig/MinecraftIcon.png',$target) or die $!;

##gamerpics
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/32_584111F70002000100010001.png",$buildname);
fcopy('../GameConfig/32_584111F70002000100010001.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/32_584111F70002000200010002.png",$buildname);
fcopy('../GameConfig/32_584111F70002000200010002.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/64_584111F70002000100010001.png",$buildname);
fcopy('../GameConfig/64_584111F70002000100010001.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/64_584111F70002000200010002.png",$buildname);
fcopy('../GameConfig/64_584111F70002000200010002.png',$target) or die $!;

## Avatar Awards
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/AvatarAwards",$buildname);
fcopy('../GameConfig/AvatarPackages/AvatarAwards',$target) or die $!;

## avatar icons
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward1_F_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward1_F_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward1_F_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward1_F_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward1_M_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward1_M_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward1_M_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward1_M_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward2_F_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward2_F_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward2_F_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward2_F_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward2_M_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward2_M_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward2_M_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward2_M_icon-128.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward3_icon-64.png",$buildname);
fcopy('../GameConfig/TitleAward3_icon-64.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/TitleAward3_icon-128.png",$buildname);
fcopy('../GameConfig/TitleAward3_icon-128.png',$target) or die $!;

## avatar bin files
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/Tshirt5_Porkchop_Female.bin",$buildname);
fcopy('../GameConfig/Tshirt5_Porkchop_Female.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/Tshirt5_Porkchop_Male.bin",$buildname);
fcopy('../GameConfig/Tshirt5_Porkchop_Male.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/MineCraft_Watch_Female.bin",$buildname);
fcopy('../GameConfig/MineCraft_Watch_Female.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/MineCraft_Watch_Male.bin",$buildname);
fcopy('../GameConfig/MineCraft_Watch_Male.bin',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/Creeper_Cap.bin",$buildname);
fcopy('../GameConfig/Creeper_Cap.bin',$target) or die $!;

## banner
# 4J Stu - This is not reference in the XLAST, but seems like an online thing rather than in the package anyway?
# 4J-PB - It's referenced in the xlast in the content offer
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/MinecraftMarketplace.png",$buildname);
fcopy('../GameConfig/MinecraftMarketplace.png',$target) or die $!;

## xex
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/Default.xex",$buildname);
fcopy('../../ContentPackage_NO_TU/Default.xex',$target) or die $!;

## game data
## can't copy all res - there's tutorial build files in there

$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/1_2_2",$buildname);
rcopy('../../Common/res/1_2_2/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/achievement",$buildname);
rcopy('../../Common/res/achievement/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/armor",$buildname);
rcopy('../../Common/res/armor/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/art",$buildname);
rcopy('../../Common/res/art/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/audio",$buildname);
rcopy('../../Common/res/audio/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/environment",$buildname);
rcopy('../../Common/res/environment/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/font",$buildname);
rcopy('../../Common/res/font/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/gui",$buildname);
rcopy('../../Common/res/gui/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/item",$buildname);
rcopy('../../Common/res/item/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/misc",$buildname);
rcopy('../../Common/res/misc/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/mob",$buildname);
rcopy('../../Common/res/mob/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/terrain",$buildname);
rcopy('../../Common/res/terrain/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/title",$buildname);
rcopy('../../Common/res/title/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/pack.png",$buildname);
fcopy('../../Common/res/pack.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/particles.png",$buildname);
fcopy('../../Common/res/particles.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/terrain.png",$buildname);
fcopy('../../Common/res/terrain.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/tutorialDiff",$buildname);
fcopy('../../Common/res/TitleUpdate/tutorialDiff',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/art",$buildname);
rcopy('../../Common/res/TitleUpdate/res/art/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/armor",$buildname);
rcopy('../../Common/res/TitleUpdate/res/armor/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/font",$buildname);
rcopy('../../Common/res/TitleUpdate/res/font/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/item",$buildname);
rcopy('../../Common/res/TitleUpdate/res/item/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/misc",$buildname);
rcopy('../../Common/res/TitleUpdate/res/misc/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/mob",$buildname);
rcopy('../../Common/res/TitleUpdate/res/mob/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/textures",$buildname);
rcopy('../../Common/res/TitleUpdate/res/textures/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/items.png",$buildname);
fcopy('../../Common/res/TitleUpdate/res/items.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/terrain.png",$buildname);
fcopy('../../Common/res/TitleUpdate/res/terrain.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/terrainMipMapLevel2.png",$buildname);
fcopy('../../Common/res/TitleUpdate/res/terrainMipMapLevel2.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/terrainMipMapLevel3.png",$buildname);
fcopy('../../Common/res/TitleUpdate/res/terrainMipMapLevel3.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/GameRules/Tutorial.pck",$buildname);
fcopy('../../Common/res/TitleUpdate/GameRules/Tutorial.pck',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/audio",$buildname);
rcopy('../../Common/res/TitleUpdate/audio/',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/TMS/TMSFiles.xzp",$buildname);
fcopy('../../XboxMedia/XZP/TMSFiles.xzp',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/colours.col",$buildname);
fcopy('../../Common/res/TitleUpdate/res/colours.col',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/particles.png",$buildname);
fcopy('../../Common/res/TitleUpdate/res/particles.png',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/res/TitleUpdate/DLC",$buildname);
rcopy('../../Common/res/TitleUpdate/DLC/',$target) or die $!;


## Bill Brown wants the Package directory to be a full build of the current game, not a TU - this was for the disc version




$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Release/Default.exe",$buildname);
fcopy('../../ContentPackage/Default.exe',$target) or die $!;


$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/Tutorial/Tutorial",$buildname);
rcopy('../../Common/Tutorial/Tutorial',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/584111F70AAAAAAA",$buildname);
rcopy('../../XboxMedia/584111F70AAAAAAA',$target) or die $!;


## nui speech
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Package/",$buildname);
rcopy('../kinect/speech/',$target) or die $!;

## test/debug - Not doing debug
##$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Debug/Default.exe",$buildname);
##fcopy('../../Debug/Minecraft.Client.exe',$target) or die $!;
##$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Debug/Default.pdb",$buildname);
##fcopy('../../Debug/Minecraft.Client.pdb',$target) or die $!;
##$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Debug/Default.xdb",$buildname);
##fcopy('../../Debug/Minecraft.Client.xdb',$target) or die $!;

## test/release
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Release/Default.exe",$buildname);
fcopy('../../ContentPackage/Default.exe',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Release/Default.pdb",$buildname);
fcopy('../../ContentPackage/Default.pdb',$target) or die $!;
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Release/Default.xdb",$buildname);
fcopy('../../ContentPackage/Default.xdb',$target) or die $!;

## test/cheats
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/Test/Cheats/Content/",$buildname);
rcopy('../Cheats/',$target) or die $!;


# need to change the attributes on the game config
$target=sprintf("c:/MinecraftSubmissionBuilds/%s/LiveFiles/Minecraft.gameconfig",$buildname);
chmod 0755, $target;

## -----------------------------------------
##
## TITLE UPDATE BUILD
## 
## -----------------------------------------

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Docs",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Package",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}


$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Package/res",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Package/res/TMS",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Package/res/font",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

$builddir=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Released",$buildname);
unless(-d $builddir)
{
   mkdir $builddir or die;
}

## Now copy the files in 

## Original xex and changed files are assumed to be in the TitleUpdate directory

$target=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Minecraft_TU18.xlast",$buildname);
fcopy('./TitleUpdate/Minecraft_TU18.xlast',$target) or die $!;

$target=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Docs",$buildname);
rcopy('./TitleUpdate/Docs',$target) or die $!;

$target=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Released",$buildname);
rcopy('./TitleUpdate/Released',$target) or die $!;

$target=sprintf("c:/MinecraftSubmissionBuilds/%s/TitleUpdate/Package",$buildname);
rcopy('./TitleUpdate/Package',$target) or die $!;

open FH, "+>", 'zipthebuild.cmd' or die $!;

$zipline = sprintf("\"C:\\Program Files (x86)\\winzip\\wzzip\" -p -r -ex c:\\MinecraftSubmissionBuilds\\Zips\\%s.zip c:\\MinecraftSubmissionBuilds\\%s\\*",$buildname,$buildname);

print FH $zipline;
close FH;
