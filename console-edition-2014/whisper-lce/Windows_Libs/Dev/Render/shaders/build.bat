@echo off

set OUT=../
set OPT=/O3 /nologo

for /d %%i in ("C:\Program Files (x86)\Windows Kits\10\bin\10.*") do set FXC="%%i\x64\fxc.exe"

if not defined FXC echo FXC not found && exit /b 1
if not exist %OUT% mkdir %OUT%

%FXC% %OPT% /T vs_4_0 /E main /D COMPRESSED  /Fh %OUT%/VS_Compressed.h                        /Vn g_main_VS_Compressed                        main_VS.hlsl
%FXC% %OPT% /T vs_4_0 /E main                /Fh %OUT%/VS_PF3_TF2_CB4_NB4_XW1.h               /Vn g_main_VS_PF3_TF2_CB4_NB4_XW1               main_VS.hlsl
%FXC% %OPT% /T vs_4_0 /E main /D LIGHTING    /Fh %OUT%/VS_PF3_TF2_CB4_NB4_XW1_LIGHTING.h      /Vn g_main_VS_PF3_TF2_CB4_NB4_XW1_LIGHTING      main_VS.hlsl
%FXC% %OPT% /T vs_4_0 /E main /D TEXGEN      /Fh %OUT%/VS_PF3_TF2_CB4_NB4_XW1_TEXGEN.h        /Vn g_main_VS_PF3_TF2_CB4_NB4_XW1_TEXGEN        main_VS.hlsl
%FXC% %OPT% /T vs_4_0 /E VS_ScreenSpace      /Fh %OUT%/VS_ScreenSpace.h                       /Vn g_main_VS_ScreenSpace                       screen_VS.hlsl
%FXC% %OPT% /T vs_4_0 /E VS_ScreenClear      /Fh %OUT%/VS_ScreenClear.h                       /Vn g_main_VS_ScreenClear                       screen_VS.hlsl
%FXC% %OPT% /T ps_4_0 /E main                /Fh %OUT%/PS_Standard.h                          /Vn g_main_PS_Standard                          main_PS.hlsl
%FXC% %OPT% /T ps_4_0 /E main /D TEXTURE_PROJECTION /Fh %OUT%/PS_TextureProjection.h          /Vn g_main_PS_TextureProjection                 main_PS.hlsl
%FXC% %OPT% /T ps_4_0 /E main /D FORCE_LOD   /Fh %OUT%/PS_ForceLOD.h                          /Vn g_main_PS_ForceLOD                          main_PS.hlsl
%FXC% %OPT% /T ps_4_0 /E PS_ScreenSpace      /Fh %OUT%/PS_ScreenSpace.h                       /Vn g_main_PS_ScreenSpace                       screen_PS.hlsl
%FXC% %OPT% /T ps_4_0 /E PS_ScreenClear      /Fh %OUT%/PS_ScreenClear.h                       /Vn g_main_PS_ScreenClear                       screen_PS.hlsl