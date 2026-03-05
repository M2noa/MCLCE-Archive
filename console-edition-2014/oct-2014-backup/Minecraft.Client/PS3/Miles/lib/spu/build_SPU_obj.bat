spu-lv2-g++.exe -mspurs-task -o mssspurs.elf -g -Xlinker --gc-sections -L%SCE_PS3_ROOT%\target\spu\lib -fno-exceptions -fno-rtti -Xlinker --start-group C:\usr\local\cell\target\spu\lib\libdma.a  mssspu.a binkaspu.a mssspu_spurs.a -Xlinker --end-group
spu_elf-to-ppu_obj --strip-mode=normal mssspurs.elf mssspurs.o
