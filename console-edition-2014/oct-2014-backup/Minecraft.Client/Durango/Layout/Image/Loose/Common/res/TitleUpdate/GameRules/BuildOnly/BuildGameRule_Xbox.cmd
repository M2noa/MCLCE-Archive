@ECHO OFF

set RULENAME=%1
cd %RULENAME%

rem set TOOLS=..\..\..\..\..\..\Tools

echo Xbox 360

echo Building Localisation...
%TOOLS%\NewLocalisationPacker.exe --oldFormat Microsoft %CD%\Strings_Xbox360 %CD%\languages.loc

echo Building Game Rules...
%TOOLS%\GameRulesPacker -i %CD%\GameRules.xml -o %CD%\ -c lzxrle

echo Building DLC Pack...
%TOOLS%\DLC_DataCreator2_CL %CD%\%RULENAME%.xml

rem Return the new tutorial.
copy %RULENAME%.pck ..\..\%RULENAME%_Xbox.pck

echo Cleaning-up...
rem del %CD%\languages.loc
rem del %CD%\GameRules.grf
rem del %CD%\%RULENAME%.pck

echo Finished.
cd ..