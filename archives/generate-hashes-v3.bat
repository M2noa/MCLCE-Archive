@echo off
setlocal enabledelayedexpansion
REM Hash generation script for archive files

echo ================================================
echo Archive File Hash Generator
echo ================================================
echo.

set OUTPUT=hashes.md
echo # Archive File Hashes > %OUTPUT%
echo. >> %OUTPUT%
echo Generated: %DATE% %TIME% >> %OUTPUT%
echo. >> %OUTPUT%
echo Source: https://hazbinhelluvageneral.com/cubevideojuego/cubegame/ >> %OUTPUT%
echo. >> %OUTPUT%

for %%f in (*.zip *.rar *.7z) do (
    if exist "%%f" (
        echo Processing: %%f
        echo. >> %OUTPUT%
        echo ### %%f >> %OUTPUT%
        
        REM Get MD5
        echo **MD5:** >> %OUTPUT%
        certutil -hashfile "%%f" MD5 | findstr /v "CertUtil" | findstr /v "hash" >> %OUTPUT%
        
        REM Get SHA256
        echo **SHA256:** >> %OUTPUT%
        certutil -hashfile "%%f" SHA256 | findstr /v "CertUtil" | findstr /v "hash" >> %OUTPUT%
    )
)

echo. >> %OUTPUT%
echo.
echo ================================================
echo Hash generation complete!
echo Results saved to: %OUTPUT%
echo ================================================
type %OUTPUT%
