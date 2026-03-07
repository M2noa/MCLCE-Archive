@echo off
setlocal enabledelayedexpansion
REM Hash generation script for archive files

echo ================================================
echo Archive File Hash Generator
echo ================================================
echo.

set OUTPUT=hashes.txt
echo # Archive File Hashes > %OUTPUT%
echo # Generated: %DATE% %TIME% >> %OUTPUT%
echo # Source: https://hazbinhelluvageneral.com/cubevideojuego/cubegame/ >> %OUTPUT%
echo. >> %OUTPUT%
echo ^| File ^| MD5 ^| SHA256 ^| >> %OUTPUT%
echo ^|------|-----|--------^| >> %OUTPUT%

for %%f in (*.zip *.rar *.7z) do (
    if exist "%%f" (
        echo Processing: %%f
        
        REM Get MD5
        for /f "skip=1 tokens=*" %%a in ('certutil -hashfile "%%f" MD5 ^| findstr /v "CertUtil"') do (
            set "hash=%%a"
            set "hash=!hash: =!"
            if not "!hash!"=="" (
                set "MD5=!hash!"
                goto :gotmd5_%%f
            )
        )
        :gotmd5_%%f
        
        REM Get SHA256
        for /f "skip=1 tokens=*" %%a in ('certutil -hashfile "%%f" SHA256 ^| findstr /v "CertUtil"') do (
            set "hash=%%a"
            set "hash=!hash: =!"
            if not "!hash!"=="" (
                set "SHA256=!hash!"
                goto :gotsha_%%f
            )
        )
        :gotsha_%%f
        
        echo ^| `%%f` ^| `!MD5!` ^| `!SHA256!` ^| >> %OUTPUT%
    )
)

echo. >> %OUTPUT%
echo.
echo ================================================
echo Hash generation complete!
echo Results saved to: %OUTPUT%
echo ================================================
