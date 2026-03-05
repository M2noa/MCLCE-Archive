SET rootPath=..\

SET TOOLS_PATH="C:\Program Files (x86)\Microsoft Xbox 360 SDK\bin\win32"

REM overwrite the edb files with the ones with no localisation data
copy blank_edbs\*.* .\

if not exist XUR mkdir XUR
if not exist RESX mkdir RESX

del /Q XUR\*.*
del /Q RESX\*.*
del /Q .\Minecraft_all.resx

if not exist .\lcl\de-DE mkdir .\lcl\de-DE
if not exist .\lcl\es-ES mkdir .\lcl\es-ES
if not exist .\lcl\fr-FR mkdir .\lcl\fr-FR
if not exist .\lcl\it-IT mkdir .\lcl\it-IT
if not exist .\lcl\ja-JP mkdir .\lcl\ja-JP
if not exist .\lcl\ko-KR mkdir .\lcl\ko-KR
if not exist .\lcl\pt-BR mkdir .\lcl\pt-BR
if not exist .\lcl\pt-PT mkdir .\lcl\pt-PT
if not exist .\lcl\zh-CHT mkdir .\lcl\zh-CHT

if not exist .\lcl\cs-CZ mkdir .\lcl\cs-CZ
if not exist .\lcl\da-DK mkdir .\lcl\da-DK
if not exist .\lcl\el-GR mkdir .\lcl\el-GR
if not exist .\lcl\en-GB mkdir .\lcl\en-GB
if not exist .\lcl\es-MX mkdir .\lcl\es-MX
if not exist .\lcl\fi-FI mkdir .\lcl\fi-FI
if not exist .\lcl\nb-NO mkdir .\lcl\nb-NO
if not exist .\lcl\nl-BE mkdir .\lcl\nl-BE
if not exist .\lcl\pl-PL mkdir .\lcl\pl-PL
if not exist .\lcl\ru-RU mkdir .\lcl\ru-RU
if not exist .\lcl\sk-SK mkdir .\lcl\sk-SK
if not exist .\lcl\sv-SE mkdir .\lcl\sv-SE
if not exist .\lcl\zh-CHS mkdir .\lcl\zh-CHS

for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui" %TOOLS_PATH%\xui2bin.exe  /nologo  ..\%%A.xui

for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui" %TOOLS_PATH%\xui2resx.exe  /nologo ..\%%A.xui

for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui" copy ..\%%A.xur .\XUR\
for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui" copy ..\%%A.resx .\RESX\
for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui"  del ..\%%A.xur 
for /F "tokens=1,2 delims=." %%A in (..\xuis.txt) DO IF "%%B"=="xui"  del ..\%%A.resx 


copy ..\..\Common\media\strings.resx .\RESX\
copy ..\4JLibs\Media\4J_strings.resx .\RESX\

REM Game rules
copy ..\..\Common\res\TitleUpdate\GameRules\BuildOnly\Tutorial\Strings\en-EN.lang .\RESX\Tutorialstrings.resx

%TOOLS_PATH%\resxloc /locstudio .\RESX\*.resx .\Minecraft_all.resx

REM Make sure the loc people haven't made the files read only
attrib -R .\lcl\de-DE\Minecraft_all.resx
attrib -R .\lcl\es-ES\Minecraft_all.resx
attrib -R .\lcl\fr-FR\Minecraft_all.resx
attrib -R .\lcl\it-IT\Minecraft_all.resx
attrib -R .\lcl\ja-JP\Minecraft_all.resx
attrib -R .\lcl\ko-KR\Minecraft_all.resx
attrib -R .\lcl\pt-BR\Minecraft_all.resx
attrib -R .\lcl\pt-PT\Minecraft_all.resx
attrib -R .\lcl\zh-CHT\Minecraft_all.resx

attrib -R .\lcl\cs-CZ\Minecraft_all.resx
attrib -R .\lcl\da-DK\Minecraft_all.resx
attrib -R .\lcl\el-GR\Minecraft_all.resx
attrib -R .\lcl\en-GB\Minecraft_all.resx
attrib -R .\lcl\es-MX\Minecraft_all.resx
attrib -R .\lcl\fi-FI\Minecraft_all.resx
attrib -R .\lcl\nb-NO\Minecraft_all.resx
attrib -R .\lcl\nl-BE\Minecraft_all.resx
attrib -R .\lcl\pl-PL\Minecraft_all.resx
attrib -R .\lcl\ru-RU\Minecraft_all.resx
attrib -R .\lcl\sk-SK\Minecraft_all.resx
attrib -R .\lcl\sv-SE\Minecraft_all.resx
attrib -R .\lcl\zh-CHS\Minecraft_all.resx

pause

