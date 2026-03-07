@echo off
setlocal

set "EXE_PATH="
for %%P in (
	"%~dp0..\..\x64_Release\Minecraft.Client.exe"
	"%~dp0..\..\x64\Release\Minecraft.Client.exe"
	"%~dp0..\..\Release\Minecraft.Client.exe"
) do (
	if exist "%%~fP" (
		set "EXE_PATH=%%~fP"
		goto :found
	)
)

echo Could not find Minecraft.Client.exe.
echo Build with configuration "Server|Windows64" first.
exit /b 1

:found
echo Starting dedicated server from "%EXE_PATH%".
"%EXE_PATH%" -server %*

