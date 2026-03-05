ECHO OFF

set RULENAME=%1
cd %RULENAME%
rem set TOOLS=..\..\..\..\..\..\Tools

ECHO "Building Localisation"
%TOOLS%\NewLocalisationPacker.exe --oldFormat Microsoft %CD%\Strings_Xbox360 %CD%\languages.loc

ECHO "Building Game Rules"
%TOOLS%\GameRulesPacker -i %CD%\GameRules.xml -o %CD%\ -c lzxrle

ECHO "Building DLC Pack"
%TOOLS%\DLC_DataCreator2_CL %CD%\%RULENAME%.xml

rem Return the new tutorial.
copy %RULENAME%.pck ..\..\%RULENAME%.pck

cd ..