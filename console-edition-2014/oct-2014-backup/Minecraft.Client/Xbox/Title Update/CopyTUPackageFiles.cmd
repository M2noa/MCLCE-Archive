mkdir .\Update\Package
mkdir .\Update\Package\res
mkdir .\Update\Package\res\font
mkdir .\Update\Package\res\armor
mkdir .\Update\Package\res\misc
mkdir .\Update\Package\res\mob
mkdir .\Update\Package\res\GameRules


copy ..\Minecraft.Client\ContentPackage\default.xex .\Update\Package\default.xex
copy ..\Minecraft.Client\Xbox\res\armor\power.png .\Update\Package\res\armor\power.png
copy ..\Minecraft.Client\Xbox\res\misc\explosion.png .\Update\Package\res\misc\explosion.png
copy ..\Minecraft.Client\Xbox\res\mob\enderman_eyes.png .\Update\Package\res\mob\enderman_eyes.png
copy ..\Minecraft.Client\Xbox\res\font\Mojangles_7.png .\Update\Package\res\font\Mojangles_7.png
copy ..\Minecraft.Client\Xbox\res\font\Mojangles_11.png .\Update\Package\res\font\Mojangles_11.png
copy ..\Minecraft.Client\Xbox\res\GameRules\Tutorial.pck .\Update\Package\res\GameRules\Tutorial.pck


pause
