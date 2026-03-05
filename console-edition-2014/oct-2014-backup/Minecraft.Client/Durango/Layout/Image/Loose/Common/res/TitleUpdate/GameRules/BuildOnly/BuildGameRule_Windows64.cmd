@ECHO OFF

set RULENAME=%1
cd %RULENAME%

rem set TOOLS=..\..\..\..\..\..\Tools

echo Windows 64

echo Building Localisation...
%TOOLS%\NewLocalisationPacker --oldFormat Microsoft %CD%\Strings_Win64 %CD%\languages.loc

echo Building Game Rules...
%TOOLS%\GameRulesPacker -i %CD%\GameRules.xml -o %CD%\ -c zlibrle

echo Building DLC Pack...
%TOOLS%\DLC_DataCreator2_CL --LittleEndian %CD%\%RULENAME%.xml

rem Return the new tutorial.
copy %RULENAME%.pck ..\..\%RULENAME%_Windows64.pck

echo Cleaning-up...
rem del %CD%\languages.loc
rem del %CD%\GameRules.grf
rem del %CD%\%RULENAME%.pck

echo Finished.
cd ..