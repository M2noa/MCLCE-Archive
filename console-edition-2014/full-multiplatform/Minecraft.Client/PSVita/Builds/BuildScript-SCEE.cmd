REM @echo off

SET TERRITORY=SCEE
SET TARGETDIR=X:\Builds\PSVita
SET CONTENTID=EP4433-PCSB00560
SET KLICENSEE=0x8EC5D170888C9865D100F04EB42198BF
SET PASSCODE=8SQ7qhCBbrqizUufLmzaSf5Qd7l2VvhV
SET DRMTYPE=Free
SET PACKAGETYPE=Demo
SET CONTENTTYPE=GameExec
SET PACKAGEVERSION=01.00

SET CURDIR=%CD%

BuildConsole.exe "..\..\..\MinecraftConsoles.sln" /build /showagent /openmonitor /nowait /cfg="ContentPackage|PSVita"

if not exist %TARGETDIR% mkdir %TARGETDIR%
xcopy /I /Y "%SCE_PSP2_SDK_DIR%\target\sce_module" "%TARGETDIR%\sce_module\"

::Copy the data files
call CopyDataFiles.cmd %TARGETDIR% %TERRITORY% 

:: Create conf
c:\perl64\bin\perl CreatePSVitaConfig_%TERRITORY%.plx

set PACKAGENAME=MINECRAFTVIT0000

cd /D %TARGETDIR%\%TERRITORY%

if exist ..\%CONTENTID%_00-%PACKAGENAME%.pkg del ..\%CONTENTID%_00-%PACKAGENAME%.pkg
 
psp2pubcmd.exe gp4p_attr_set --content_id %CONTENTID%_00-%PACKAGENAME% --pub_ver %PACKAGEVERSION% --drm_type %DRMTYPE% --capacity gc2_gcrm --passcode %PASSCODE% Minecraft.Client_%TERRITORY%.gp4p

:: Create package

if exist .\BUILD del -Q -S .\BUILD\
if not exist .\BUILD mkdir .\BUILD

psp2pubcmd.exe pkg_create --oformat all Minecraft.Client_%TERRITORY%.gp4p .\BUILD > PackageBuildOutput.txt

cd /D %CURDIR%

call move_pkg.cmd

pause
GOTO :EOF


