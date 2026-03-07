@ECHO OFF

set RULENAME=%1
cd %RULENAME%

rem set TOOLS=..\..\..\..\..\..\Tools

echo Play Station 3

echo Building Localisation...
%TOOLS%\NewLocalisationPacker --oldFormat Sony %CD%\Strings_PS3 %CD%\languages.loc

echo Building Game Rules...
%TOOLS%\GameRulesPacker -i %CD%\GameRules.xml -o %CD%\ -c edgezlibrle

echo Building DLC Pack...
%TOOLS%\DLC_DataCreator2_CL %CD%\%RULENAME%.xml

rem Return the new tutorial.
copy %RULENAME%.pck ..\..\%RULENAME%_PS3.pck

echo Cleaning-up...
rem del %CD%\languages.loc
rem del %CD%\GameRules.grf
rem del %CD%\%RULENAME%.pck

echo Finished.
cd ..