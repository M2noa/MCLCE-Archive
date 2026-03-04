SET rootPath=..\

SET TOOLS_PATH="C:\Program Files (x86)\Microsoft Xbox 360 SDK\bin\win32"

if not exist ..\..\Common\Media\de-DE  mkdir ..\..\Common\Media\de-DE 
if not exist ..\..\Common\Media\es-ES  mkdir ..\..\Common\Media\es-ES 
if not exist ..\..\Common\Media\fr-FR  mkdir ..\..\Common\Media\fr-FR 
if not exist ..\..\Common\Media\it-IT  mkdir ..\..\Common\Media\it-IT 
if not exist ..\..\Common\Media\ja-JP  mkdir ..\..\Common\Media\ja-JP 
if not exist ..\..\Common\Media\ko-KR  mkdir ..\..\Common\Media\ko-KR 
if not exist ..\..\Common\Media\pt-BR  mkdir ..\..\Common\Media\pt-BR 
if not exist ..\..\Common\Media\pt-PT  mkdir ..\..\Common\Media\pt-PT 
if not exist ..\..\Common\Media\zh-CHT mkdir ..\..\Common\Media\zh-CHT

if not exist ..\..\Common\Media\cs-CZ mkdir ..\..\Common\Media\cs-CZ
if not exist ..\..\Common\Media\da-DK mkdir ..\..\Common\Media\da-DK
if not exist ..\..\Common\Media\el-GR mkdir ..\..\Common\Media\el-GR
if not exist ..\..\Common\Media\en-GB mkdir ..\..\Common\Media\en-GB
if not exist ..\..\Common\Media\es-MX mkdir ..\..\Common\Media\es-MX
if not exist ..\..\Common\Media\fi-FI mkdir ..\..\Common\Media\fi-FI
if not exist ..\..\Common\Media\nb-NO mkdir ..\..\Common\Media\nb-NO
if not exist ..\..\Common\Media\nl-BE mkdir ..\..\Common\Media\nl-BE
if not exist ..\..\Common\Media\pl-PL mkdir ..\..\Common\Media\pl-PL
if not exist ..\..\Common\Media\ru-RU mkdir ..\..\Common\Media\ru-RU
if not exist ..\..\Common\Media\sk-SK mkdir ..\..\Common\Media\sk-SK
if not exist ..\..\Common\Media\sv-SE mkdir ..\..\Common\Media\sv-SE
if not exist ..\..\Common\Media\zh-CHS mkdir ..\..\Common\Media\zh-CHS

copy .\lcl\de-DE\Minecraft_all.resx ..\..\Common\Media\de-DE
%TOOLS_PATH%\resxloc ..\..\Common\Media\de-DE\Minecraft_all.resx
del ..\..\Common\Media\de-DE\Minecraft_all.resx

copy .\lcl\es-ES\Minecraft_all.resx ..\..\Common\Media\es-ES
%TOOLS_PATH%\resxloc ..\..\Common\Media\es-ES\Minecraft_all.resx
del ..\..\Common\Media\es-ES\Minecraft_all.resx

copy .\lcl\fr-FR\Minecraft_all.resx ..\..\Common\Media\fr-FR
%TOOLS_PATH%\resxloc ..\..\Common\Media\fr-FR\Minecraft_all.resx
del ..\..\Common\Media\fr-FR\Minecraft_all.resx

copy .\lcl\it-IT\Minecraft_all.resx ..\..\Common\Media\it-IT
%TOOLS_PATH%\resxloc ..\..\Common\Media\it-IT\Minecraft_all.resx
del ..\..\Common\Media\it-IT\Minecraft_all.resx

copy .\lcl\ja-JP\Minecraft_all.resx ..\..\Common\Media\ja-JP
%TOOLS_PATH%\resxloc ..\..\Common\Media\ja-JP\Minecraft_all.resx
del ..\..\Common\Media\ja-JP\Minecraft_all.resx

copy .\lcl\ko-KR\Minecraft_all.resx ..\..\Common\Media\ko-KR
%TOOLS_PATH%\resxloc ..\..\Common\Media\ko-KR\Minecraft_all.resx
del ..\..\Common\Media\ko-KR\Minecraft_all.resx

copy .\lcl\pt-BR\Minecraft_all.resx ..\..\Common\Media\pt-BR
%TOOLS_PATH%\resxloc ..\..\Common\Media\pt-BR\Minecraft_all.resx
del ..\..\Common\Media\pt-BR\Minecraft_all.resx

copy .\lcl\pt-PT\Minecraft_all.resx ..\..\Common\Media\pt-PT
%TOOLS_PATH%\resxloc ..\..\Common\Media\pt-PT\Minecraft_all.resx
del ..\..\Common\Media\pt-PT\Minecraft_all.resx

copy .\lcl\zh-CHT\Minecraft_all.resx ..\..\Common\Media\zh-CHT
%TOOLS_PATH%\resxloc ..\..\Common\Media\zh-CHT\Minecraft_all.resx
del ..\..\Common\Media\zh-CHT\Minecraft_all.resx

copy .\lcl\cs-CZ\Minecraft_all.resx ..\..\Common\Media\cs-CZ
copy .\lcl\da-DK\Minecraft_all.resx ..\..\Common\Media\da-DK
copy .\lcl\el-GR\Minecraft_all.resx ..\..\Common\Media\el-GR
copy .\lcl\en-GB\Minecraft_all.resx ..\..\Common\Media\en-GB
copy .\lcl\es-MX\Minecraft_all.resx ..\..\Common\Media\es-MX
copy .\lcl\fi-FI\Minecraft_all.resx ..\..\Common\Media\fi-FI
copy .\lcl\nb-NO\Minecraft_all.resx ..\..\Common\Media\nb-NO
copy .\lcl\nl-BE\Minecraft_all.resx ..\..\Common\Media\nl-BE
copy .\lcl\pl-PL\Minecraft_all.resx ..\..\Common\Media\pl-PL
copy .\lcl\ru-RU\Minecraft_all.resx ..\..\Common\Media\ru-RU
copy .\lcl\sk-SK\Minecraft_all.resx ..\..\Common\Media\sk-SK
copy .\lcl\sv-SE\Minecraft_all.resx ..\..\Common\Media\sv-SE
copy .\lcl\zh-CHS\Minecraft_all.resx ..\..\Common\Media\zh-CHS

%TOOLS_PATH%\resxloc ..\..\Common\Media\cs-CZ\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\da-DK\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\el-GR\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\en-GB\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\es-MX\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\fi-FI\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\nb-NO\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\nl-BE\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\pl-PL\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\ru-RU\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\sk-SK\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\sv-SE\Minecraft_all.resx
%TOOLS_PATH%\resxloc ..\..\Common\Media\zh-CHS\Minecraft_all.resx

del ..\..\Common\Media\cs-CZ\Minecraft_all.resx
del ..\..\Common\Media\da-DK\Minecraft_all.resx
del ..\..\Common\Media\el-GR\Minecraft_all.resx
del ..\..\Common\Media\en-GB\Minecraft_all.resx
del ..\..\Common\Media\es-MX\Minecraft_all.resx
del ..\..\Common\Media\fi-FI\Minecraft_all.resx
del ..\..\Common\Media\nb-NO\Minecraft_all.resx
del ..\..\Common\Media\nl-BE\Minecraft_all.resx
del ..\..\Common\Media\pl-PL\Minecraft_all.resx
del ..\..\Common\Media\ru-RU\Minecraft_all.resx
del ..\..\Common\Media\sk-SK\Minecraft_all.resx
del ..\..\Common\Media\sv-SE\Minecraft_all.resx
del ..\..\Common\Media\zh-CHS\Minecraft_all.resx



if exist ..\..\Common\Media\de-DE\Tutorialstrings.resx move ..\..\Common\Media\de-DE\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\de-DE.lang
if exist ..\..\Common\Media\es-ES\Tutorialstrings.resx move ..\..\Common\Media\es-ES\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\es-ES.lang
if exist ..\..\Common\Media\fr-FR\Tutorialstrings.resx move ..\..\Common\Media\fr-FR\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\fr-FR.lang
if exist ..\..\Common\Media\it-IT\Tutorialstrings.resx move ..\..\Common\Media\it-IT\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\it-IT.lang
if exist ..\..\Common\Media\ja-JP\Tutorialstrings.resx move ..\..\Common\Media\ja-JP\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\ja-JP.lang
if exist ..\..\Common\Media\ko-KR\Tutorialstrings.resx move ..\..\Common\Media\ko-KR\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\ko-KR.lang
if exist ..\..\Common\Media\pt-BR\Tutorialstrings.resx move ..\..\Common\Media\pt-BR\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\pt-BR.lang
if exist ..\..\Common\Media\pt-PT\Tutorialstrings.resx move ..\..\Common\Media\pt-PT\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\pt-PT.lang
if exist ..\..\Common\Media\zh-CHT\Tutorialstrings.resx move ..\..\Common\Media\zh-CHT\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\zh-CHT.lang

if exist ..\..\Common\Media\cs-CZ\Tutorialstrings.resx move ..\..\Common\Media\cs-CZ\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\cs-CZ.lang
if exist ..\..\Common\Media\da-DK\Tutorialstrings.resx move ..\..\Common\Media\da-DK\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\da-DK.lang
if exist ..\..\Common\Media\el-GR\Tutorialstrings.resx move ..\..\Common\Media\el-GR\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\el-GR.lang
if exist ..\..\Common\Media\en-GB\Tutorialstrings.resx move ..\..\Common\Media\en-GB\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\en-GB.lang
if exist ..\..\Common\Media\es-MX\Tutorialstrings.resx move ..\..\Common\Media\es-MX\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\es-MX.lang
if exist ..\..\Common\Media\fi-FI\Tutorialstrings.resx move ..\..\Common\Media\fi-FI\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\fi-FI.lang
if exist ..\..\Common\Media\nb-NO\Tutorialstrings.resx move ..\..\Common\Media\nb-NO\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\nb-NO.lang
if exist ..\..\Common\Media\nl-BE\Tutorialstrings.resx move ..\..\Common\Media\nl-BE\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\nl-BE.lang
if exist ..\..\Common\Media\pl-PL\Tutorialstrings.resx move ..\..\Common\Media\pl-PL\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\pl-PL.lang
if exist ..\..\Common\Media\ru-RU\Tutorialstrings.resx move ..\..\Common\Media\ru-RU\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\ru-RU.lang
if exist ..\..\Common\Media\sk-SK\Tutorialstrings.resx move ..\..\Common\Media\sk-SK\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\sk-SK.lang
if exist ..\..\Common\Media\sv-SE\Tutorialstrings.resx move ..\..\Common\Media\sv-SE\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\sv-SE.lang
if exist ..\..\Common\Media\zh-CHS\Tutorialstrings.resx move ..\..\Common\Media\zh-CHS\Tutorialstrings.resx ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\Microsoft\zh-CHS.lang

pause