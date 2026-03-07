mkdir .\Package
copy ..\..\ContentPackage\default.xex .\Package\default.xex

REM Copy files from the Gameconfig

copy ..\GameConfig\01.png							.\Package\
copy ..\GameConfig\02.png							.\Package\
copy ..\GameConfig\03.png							.\Package\
copy ..\GameConfig\04.png							.\Package\
copy ..\GameConfig\05.png							.\Package\
copy ..\GameConfig\06.png							.\Package\
copy ..\GameConfig\07.png							.\Package\
copy ..\GameConfig\08.png							.\Package\
copy ..\GameConfig\09.png							.\Package\
copy ..\GameConfig\10.png							.\Package\
copy ..\GameConfig\11.png							.\Package\
copy ..\GameConfig\12.png							.\Package\
copy ..\GameConfig\13.png							.\Package\
copy ..\GameConfig\14.png							.\Package\
copy ..\GameConfig\15.png							.\Package\
copy ..\GameConfig\16.png							.\Package\
copy ..\GameConfig\17.png							.\Package\
copy ..\GameConfig\18.png							.\Package\
copy ..\GameConfig\19.png							.\Package\
copy ..\GameConfig\20.png							.\Package\
copy ..\GameConfig\32_584111F70002000100010001.png				.\Package\
copy ..\GameConfig\32_584111F70002000200010002.png				.\Package\
copy ..\GameConfig\64_584111F70002000100010001.png				.\Package\
copy ..\GameConfig\64_584111F70002000200010002.png				.\Package\
copy ..\GameConfig\ArcadeInfo.xml						.\Package\
copy ..\GameConfig\Minecraft_BOXART.png						.\Package\
copy ..\GameConfig\Minecraft_BKGND.png						.\Package\
copy ..\GameConfig\MinecraftIcon.png						.\Package\
copy ..\GameConfig\MinecraftMarketplace.png					.\Package\
copy ..\GameConfig\TitleAward1_F_icon-64.png					.\Package\
copy ..\GameConfig\TitleAward1_F_icon-128.png					.\Package\
copy ..\GameConfig\TitleAward1_M_icon-64.png					.\Package\
copy ..\GameConfig\TitleAward1_M_icon-128.png					.\Package\
copy ..\GameConfig\TitleAward2_F_icon-64.png					.\Package\
copy ..\GameConfig\TitleAward2_F_icon-128.png					.\Package\
copy ..\GameConfig\TitleAward2_M_icon-64.png					.\Package\
copy ..\GameConfig\TitleAward2_M_icon-128.png					.\Package\
copy ..\GameConfig\TitleAward3_icon-64.png					.\Package\
copy ..\GameConfig\TitleAward3_icon-128.png					.\Package\
copy ..\GameConfig\TitleAward3_icon-128.png					.\Package\
copy ..\GameConfig\Tshirt5_Porkchop_Female.bin					.\Package\
copy ..\GameConfig\Tshirt5_Porkchop_Male.bin					.\Package\
copy ..\GameConfig\MineCraft_Watch_Female.bin					.\Package\
copy ..\GameConfig\MineCraft_Watch_Male.bin					.\Package\
copy ..\GameConfig\Creeper_Cap.bin						.\Package\
copy ..\AvatarAwards								.\Package\
copy ..\584111F70AAAAAAA							.\Package\

xcopy /Y /S /I ..\res\*.* .\Package\res\
xcopy /Y /S /I ..\kinect\speech\*.* .\Package\
mkdir .\Package\Tutorial\
copy ..\Tutorial\Tutorial .\Package\Tutorial\Tutorial

pause
