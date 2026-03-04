
BuildConsole.exe "..\..\..\MinecraftConsoles.sln" /build /showagent /openmonitor /nowait /cfg="ContentPackage_NO_TU|Xbox 360"
pause
BuildConsole.exe "..\..\..\MinecraftConsoles.sln" /build /showagent /openmonitor /nowait /cfg="ContentPackage|Xbox 360"
pause

REM "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\MinecraftConsoles.sln" /Clean "ContentPackage|Xbox 360" /ProjectConfig "ContentPackage|Xbox 360" /Out clean.txt /NoLogo
REM "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\MinecraftConsoles.sln" /Build "ContentPackage|Xbox 360" /ProjectConfig "ContentPackage|Xbox 360" /Out build.txt /NoLogo

REM "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\Minecraft360.sln" /Clean ContentPackageNoTU /ProjectConfig ContentPackageNoTU /Out cleanNoTU.txt /NoLogo
REM "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" "..\..\..\Minecraft360.sln" /Build ContentPackageNoTU /ProjectConfig ContentPackageNoTU /Out buildNoTU.txt /NoLogo

mkdir .\TitleUpdate\Package
mkdir .\TitleUpdate\Package\res
mkdir .\TitleUpdate\Package\res\TMS
mkdir .\TitleUpdate\Package\res\font
mkdir .\TitleUpdate\Package\res\audio
mkdir .\TitleUpdate\Package\res\armor
mkdir .\TitleUpdate\Package\res\art
mkdir .\TitleUpdate\Package\res\misc
mkdir .\TitleUpdate\Package\res\item
mkdir .\TitleUpdate\Package\res\gui
mkdir .\TitleUpdate\Package\res\mob
mkdir .\TitleUpdate\Package\res\mob\villager
mkdir .\TitleUpdate\Package\res\mob\enderdragon
mkdir .\TitleUpdate\Package\res\GameRules
mkdir .\TitleUpdate\Package\res\textures
mkdir .\TitleUpdate\Package\res\textures\blocks
mkdir .\TitleUpdate\Package\res\textures\items
mkdir .\TitleUpdate\Docs
mkdir .\TitleUpdate\Released

copy .\Minecraft_12.03.30.0062\Package\default.xex 						.\TitleUpdate\Released\default.xex
copy ..\..\ContentPackage\default.xex 									.\TitleUpdate\Package\default.xex
copy ..\..\Common\res\font\Mojangles_7.png 								.\TitleUpdate\Package\res\font\Mojangles_7.png
copy ..\..\Common\res\font\Mojangles_11.png 							.\TitleUpdate\Package\res\font\Mojangles_11.png
copy ..\..\Common\res\TitleUpdate\res\terrain.png						.\TitleUpdate\Package\res\terrain.png 
copy ..\..\Common\res\TitleUpdate\res\terrainMipmapLevel2.png			.\TitleUpdate\Package\res\terrainMipmapLevel2.png 
copy ..\..\Common\res\TitleUpdate\res\terrainMipmapLevel3.png			.\TitleUpdate\Package\res\terrainMipmapLevel3.png
copy ..\..\Common\res\TitleUpdate\res\items.png							.\TitleUpdate\Package\res\items.png 
copy ..\..\Common\res\TitleUpdate\res\armor\power.png 					.\TitleUpdate\Package\res\armor\power.png
copy ..\..\Common\res\TitleUpdate\res\armor\cloth_1.png 				.\TitleUpdate\Package\res\armor\cloth_1.png
copy ..\..\Common\res\TitleUpdate\res\armor\cloth_1_b.png 				.\TitleUpdate\Package\res\armor\cloth_1_b.png
copy ..\..\Common\res\TitleUpdate\res\armor\cloth_2.png 				.\TitleUpdate\Package\res\armor\cloth_2.png
copy ..\..\Common\res\TitleUpdate\res\armor\cloth_2_b.png 				.\TitleUpdate\Package\res\armor\cloth_2_b.png
copy ..\..\Common\res\TitleUpdate\res\item\book.png 					.\TitleUpdate\Package\res\item\book.png
copy ..\..\Common\res\TitleUpdate\res\misc\explosion.png 				.\TitleUpdate\Package\res\misc\explosion.png
copy ..\..\Common\res\TitleUpdate\res\misc\footprint.png 				.\TitleUpdate\Package\res\misc\footprint.png
copy ..\..\Common\res\TitleUpdate\res\misc\glint.png 					.\TitleUpdate\Package\res\misc\glint.png
copy ..\..\Common\res\TitleUpdate\res\misc\mapicons.png					.\TitleUpdate\Package\res\misc\mapicons.png 
copy ..\..\Common\res\TitleUpdate\res\misc\particlefield.png 			.\TitleUpdate\Package\res\misc\particlefield.png
copy ..\..\Common\res\TitleUpdate\res\misc\tunnel.png 					.\TitleUpdate\Package\res\misc\tunnel.png
copy ..\..\Common\res\TitleUpdate\res\mob\enderman_eyes.png 			.\TitleUpdate\Package\res\mob\enderman_eyes.png
copy ..\..\Common\res\TitleUpdate\res\mob\redcow.png 					.\TitleUpdate\Package\res\mob\redcow.png
copy ..\..\Common\res\TitleUpdate\res\mob\snowman.png 					.\TitleUpdate\Package\res\mob\snowman.png
copy ..\..\Common\res\TitleUpdate\res\mob\zombie.png 					.\TitleUpdate\Package\res\mob\zombie.png
copy ..\..\Common\res\TitleUpdate\res\mob\skeleton_wither.png 			.\TitleUpdate\Package\res\mob\skeleton_wither.png
copy ..\..\Common\res\TitleUpdate\res\mob\enderdragon\beam.png 			.\TitleUpdate\Package\res\mob\enderdragon\beam.png
copy ..\..\Common\res\TitleUpdate\res\mob\enderdragon\ender.png 		.\TitleUpdate\Package\res\mob\enderdragon\ender.png
copy ..\..\Common\res\TitleUpdate\res\mob\enderdragon\ender_eyes.png 	.\TitleUpdate\Package\res\mob\enderdragon\ender_eyes.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\butcher.png 			.\TitleUpdate\Package\res\mob\villager\butcher.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\farmer.png 			.\TitleUpdate\Package\res\mob\villager\farmer.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\librarian.png 		.\TitleUpdate\Package\res\mob\villager\librarian.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\priest.png 			.\TitleUpdate\Package\res\mob\villager\priest.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\smith.png 			.\TitleUpdate\Package\res\mob\villager\smith.png
copy ..\..\Common\res\TitleUpdate\res\mob\villager\villager.png 		.\TitleUpdate\Package\res\mob\villager\villager.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\fire_0.png 		.\TitleUpdate\Package\res\textures\blocks\fire_0.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\fire_0.txt 		.\TitleUpdate\Package\res\textures\blocks\fire_0.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\fire_1.png 		.\TitleUpdate\Package\res\textures\blocks\fire_1.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\fire_1.txt 		.\TitleUpdate\Package\res\textures\blocks\fire_1.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\lava.png 			.\TitleUpdate\Package\res\textures\blocks\lava.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\lava.txt 			.\TitleUpdate\Package\res\textures\blocks\lava.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\lava_flow.png		.\TitleUpdate\Package\res\textures\blocks\lava_flow.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\lava_flow.txt		.\TitleUpdate\Package\res\textures\blocks\lava_flow.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\portal.png 		.\TitleUpdate\Package\res\textures\blocks\portal.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\portal.txt 		.\TitleUpdate\Package\res\textures\blocks\portal.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\water.png 		.\TitleUpdate\Package\res\textures\blocks\water.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\water.txt 		.\TitleUpdate\Package\res\textures\blocks\water.txt
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\water_flow.png	.\TitleUpdate\Package\res\textures\blocks\water_flow.png
copy ..\..\Common\res\TitleUpdate\res\textures\blocks\water_flow.txt	.\TitleUpdate\Package\res\textures\blocks\water_flow.txt
copy ..\..\Common\res\TitleUpdate\res\textures\items\clock.png			.\TitleUpdate\Package\res\textures\items\clock.png
copy ..\..\Common\res\TitleUpdate\res\textures\items\clock.txt			.\TitleUpdate\Package\res\textures\items\clock.txt
copy ..\..\Common\res\TitleUpdate\res\textures\items\compass.png		.\TitleUpdate\Package\res\textures\items\compass.png
copy ..\..\Common\res\TitleUpdate\res\textures\items\compass.txt		.\TitleUpdate\Package\res\textures\items\compass.txt
copy ..\..\Common\res\TitleUpdate\GameRules\Tutorial.pck 				.\TitleUpdate\Package\res\GameRules\Tutorial.pck
copy ..\..\XboxMedia\XZP\TMSFiles.xzp  									.\TitleUpdate\Package\res\TMS\TMSFiles.xzp
copy ..\..\Common\res\TitleUpdate\audio\AdditionalMusic.xwb  			.\TitleUpdate\Package\res\audio\AdditionalMusic.xwb
copy ..\..\Common\res\TitleUpdate\audio\additional.xsb  				.\TitleUpdate\Package\res\audio\additional.xsb
copy ..\..\Common\res\TitleUpdate\audio\additional.xwb  				.\TitleUpdate\Package\res\audio\additional.xwb
copy ..\..\Common\res\TitleUpdate\audio\Minecraft.xgs  					.\TitleUpdate\Package\res\audio\Minecraft.xgs
copy ..\..\Common\res\TitleUpdate\audio\minecraft.xsb  					.\TitleUpdate\Package\res\audio\minecraft.xsb
copy ..\..\Common\res\TitleUpdate\res\colours.col  						.\TitleUpdate\Package\res\colours.col

REM TU14
copy ..\..\Common\res\TitleUpdate\res\item\enderchest.png 				.\TitleUpdate\Package\res\item\enderchest.png
copy ..\..\Common\res\TitleUpdate\res\art\kz.png 						.\TitleUpdate\Package\res\art\kz.png
copy ..\..\Common\res\TitleUpdate\res\mob\wolf_collar.png 				.\TitleUpdate\Package\res\mob\wolf_collar.png
copy ..\..\Common\res\TitleUpdate\res\mob\wolf_tame.png 				.\TitleUpdate\Package\res\mob\wolf_tame.png
copy ..\..\Common\res\TitleUpdate\res\particles.png 					.\TitleUpdate\Package\res\particles.png
copy ..\..\Common\res\TitleUpdate\res\mob\zombie_villager.png 			.\TitleUpdate\Package\res\mob\zombie_villager.png
copy "..\..\Common\res\TitleUpdate\res\items.png"						".\TitleUpdate\Package\res\items.png"
copy "..\..\Common\res\TitleUpdate\res\terrain.png"						".\TitleUpdate\Package\res\terrain.png"
copy "..\..\Common\res\TitleUpdate\res\terrainMipMapLevel2.png"			".\TitleUpdate\Package\res\terrainMipMapLevel2.png"
copy "..\..\Common\res\TitleUpdate\res\terrainMipMapLevel3.png"			".\TitleUpdate\Package\res\terrainMipMapLevel3.png"
xcopy "..\..\Common\res\TitleUpdate\DLC"								".\TitleUpdate\Package\res\DLC" /i /e /y

REM TU17
copy ..\..\Common\res\TitleUpdate\res\font\Default.png 					.\TitleUpdate\Package\res\font\Default.png
copy ..\..\Common\res\TitleUpdate\res\font\Mojangles_7.png 				.\TitleUpdate\Package\res\font\Mojangles_7.png
copy ..\..\Common\res\TitleUpdate\res\font\Mojangles_11.png 			.\TitleUpdate\Package\res\font\Mojangles_11.png

REM TU17
copy ..\..\Common\res\TitleUpdate\res\font\Default.png  				.\TitleUpdate\Package\res\font\Default.png 

REM copy Minecraft_response_doc.xls 									.\TitleUpdate\Docs


c:\perl64\bin\perl tubuild.plx

REM blast Minecraft_Day1TU.xlast /L:3

REM zip it

zipthebuild.cmd

pause