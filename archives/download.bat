@echo off
REM Download script for all archive files from hazbinhelluvageneral.com
REM All files sourced from: https://hazbinhelluvageneral.com/cubevideojuego/cubegame/

set BASE_URL=https://hazbinhelluvageneral.com/cubevideojuego/

echo Downloading Minecraft Console Edition archives...
echo.

REM Console Edition archives
curl -L -o "mc-console-oct2014.zip" "%BASE_URL%mc-console-oct2014.zip"
curl -L -o "mc-cpp-snapshot.zip" "%BASE_URL%mc-cpp-snapshot.zip"
curl -L -o "minecraft.rar" "%BASE_URL%minecraft.rar"
curl -L -o "MinecraftConsoles-141217.zip" "%BASE_URL%MinecraftConsoles-141217.zip"
curl -L -o "MinecraftPS3Edition.7z" "%BASE_URL%MinecraftPS3Edition.7z"

REM Dungeons and Legends
curl -L -o "minecraft_dungeons.zip" "%BASE_URL%minecraft_dungeons.zip"
curl -L -o "minecraft_legends.zip" "%BASE_URL%minecraft_legends.zip"
curl -L -o "filelist.txt" "%BASE_URL%filelist.txt"
curl -L -o "Narration_NoGeneratedLevel_20170216.zip" "%BASE_URL%Narration_NoGeneratedLevel_20170216.zip"

REM Story Mode beta branches
curl -L -o "enderman.zip" "%BASE_URL%enderman.zip"
curl -L -o "endercon.zip" "%BASE_URL%endercon.zip"
curl -L -o "linux-test.zip" "%BASE_URL%linux-test.zip"
curl -L -o "patch.zip" "%BASE_URL%patch.zip"
curl -L -o "info_storymode.txt" "%BASE_URL%info_storymode.txt"

echo.
echo Download complete!
echo.
echo Generating MD5 and SHA256 hashes...

REM Generate hashes
for %%f in (*.zip *.rar *.7z *.txt) do (
    if exist "%%f" (
        echo %%f:
        certutil -hashfile "%%f" MD5
        certutil -hashfile "%%f" SHA256
        echo.
    )
)

echo Hash generation complete!
