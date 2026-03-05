@ECHO OFF

set RULENAME=%1
cd %RULENAME%

rem set TOOLS=..\..\..\..\..\..\Tools

echo Orbis

echo Building Localisation...
%TOOLS%\NewLocalisationPacker --oldFormat Sony %CD%\Strings_Orbis %CD%\languages.loc

echo Building Game Rules...
%TOOLS%\GameRulesPacker -i %CD%\GameRules.xml -o %CD%\ -c zlibrle

echo Building DLC Pack...
%TOOLS%\DLC_DataCreator2_CL --LittleEndian  %CD%\%RULENAME%.xml

rem Return the new tutorial.
copy %RULENAME%.pck ..\..\%RULENAME%_Orbis.pck

echo Cleaning-up...
rem del %CD%\languages.loc
rem del %CD%\GameRules.grf
rem del %CD%\%RULENAME%.pck

echo Finished.
cd ..