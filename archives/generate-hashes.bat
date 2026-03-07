@echo off
REM Hash generation script for archive files
REM Generates MD5 and SHA256 hashes for all archive files

echo ================================================
echo Archive File Hash Generator
echo ================================================
echo.

set OUTPUT=hashes.txt
echo # Archive File Hashes > %OUTPUT%
echo # Generated: %DATE% %TIME% >> %OUTPUT%
echo # Source: https://hazbinhelluvageneral.com/cubevideojuego/cubegame/ >> %OUTPUT%
echo. >> %OUTPUT%

echo File,MD5,SHA256 >> %OUTPUT%

for %%f in (*.zip *.rar *.7z *.txt) do (
    if exist "%%f" (
        echo Processing: %%f
        
        REM Get MD5
        for /f "tokens=3" %%a in ('certutil -hashfile "%%f" MD5 ^| findstr /v "certutil" ^| findstr /v "MD5" ^| findstr /v "====="') do set MD5=%%a
        
        REM Get SHA256
        for /f "tokens=3" %%a in ('certutil -hashfile "%%f" SHA256 ^| findstr /v "certutil" ^| findstr /v "SHA256" ^| findstr /v "====="') do set SHA256=%%a
        
        echo "%%f",%MD5%,%SHA256% >> %OUTPUT%
    )
)

echo.
echo ================================================
echo Hash generation complete!
echo Results saved to: %OUTPUT%
echo ================================================
echo.
type %OUTPUT%
