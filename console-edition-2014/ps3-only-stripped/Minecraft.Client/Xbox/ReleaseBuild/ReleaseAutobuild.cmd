
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\Minecraft360.sln" /Clean Release /ProjectConfig Release /Out cleanr.txt /NoLogo
"C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\Minecraft360.sln" /Build Release /ProjectConfig Release /Out buildr.txt /NoLogo

perl build.plx

REM zip it

zipthebuild.cmd

pause