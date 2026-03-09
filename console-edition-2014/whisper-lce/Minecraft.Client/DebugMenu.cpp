#include "stdafx.h"
#include "DebugMenu.h"
#include "Minecraft.h"
#include "Options.h"
#include "LevelRenderer.h"
#include "Textures.h"
#include "Common/Audio/SoundEngine.h"
#include <windowsx.h>

#pragma comment(lib, "comctl32.lib")

#define ID_MUSIC_SLIDER 1001
#define ID_MUSIC_EDIT 1002
#define ID_SOUND_SLIDER 1003
#define ID_SOUND_EDIT 1004
#define ID_SENSITIVITY_SLIDER 1005
#define ID_SENSITIVITY_EDIT 1006
#define ID_FOV_SLIDER 1007
#define ID_FOV_EDIT 1008
#define ID_GAMMA_SLIDER 1009
#define ID_GAMMA_EDIT 1010

#define ID_INVERT_MOUSE 1020
#define ID_BOB_VIEW 1021
#define ID_ANAGLYPH 1022
#define ID_ADVANCED_OPENGL 1023
#define ID_FANCY_GRAPHICS 1024
#define ID_AMBIENT_OCCLUSION 1025
#define ID_RENDER_CLOUDS 1026

#define ID_VIEW_DISTANCE 1030
#define ID_DIFFICULTY 1031
#define ID_GUI_SCALE 1032
#define ID_PARTICLES 1033
#define ID_FRAMERATE_LIMIT 1034
#define ID_VSYNC 1035
#define ID_UNLOCK_FPS 1036
#define ID_MAX_FPS 1037
#define ID_SWAP_STICK_BUTTONS 1038

#define ID_LOAD_SAVES_FROM_DISK 1100
#define ID_WRITE_SAVES_TO_DISK 1101
#define ID_FREEZE_PLAYERS 1102
#define ID_SAFEAREA 1103
#define ID_MOBS_DONT_ATTACK 1104
#define ID_FREEZE_TIME 1105
#define ID_DISABLE_WEATHER 1106
#define ID_CRAFT_ANYTHING 1107
#define ID_USE_DPAD_FOR_DEBUG 1108
#define ID_MOBS_DONT_TICK 1109
#define ID_INSTANT_MINE 1110
#define ID_SHOW_UI_CONSOLE 1111
#define ID_DISTRIBUTABLE_SAVE 1112
#define ID_DEBUG_LEADERBOARDS 1113
#define ID_HEIGHT_WATER_BIOME 1114
#define ID_SUPERFLAT_NETHER 1115
#define ID_MORE_LIGHTNING 1116
#define ID_GO_TO_NETHER 1117
#define ID_GO_TO_OVERWORLD 1118
#define ID_UNLOCK_ALL_DLC 1119
#define ID_SHOW_MARKETING_GUIDE 1120
#define ID_COMMAND_INPUT 1121
#define ID_SEND_BUTTON 1122
#define ID_COMMAND_HISTORY 1123

DebugMenu::DebugMenu()
    : m_hwnd(NULL)
    , m_hInstance(NULL)
    , m_minecraft(NULL)
    , m_visible(false)
    , m_updating(false)
    , m_vsyncEnabled(true)
    , m_unlockFps(false)
    , m_maxFps(300)
    , m_swapStickButtons(false)
    , m_debugMask(0)
    , m_commandInput(NULL)
    , m_sendButton(NULL)
    , m_commandHistory(NULL)
{
}

DebugMenu::~DebugMenu()
{
    if (m_hwnd)
    {
        DestroyWindow(m_hwnd);
    }
}

void DebugMenu::init(HINSTANCE hInstance, Minecraft* mc)
{
    m_hInstance = hInstance;
    m_minecraft = mc;
    
    InitCommonControls();
    
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"MinecraftDebugMenu";
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassExW(&wc);
    
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"MinecraftDebugMenu",
        L"Minecraft Debug Settings & Commands",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_VSCROLL,
        50, 50, 600, 900,
        NULL, NULL, hInstance, this
    );
    
    SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
    
    createControls();
    m_visible = true;
}

void DebugMenu::createControls()
{
    int y = 10;
    int labelWidth = 120;
    int sliderWidth = 200;
    int editWidth = 60;
    int x = 10;
    
    CreateWindowW(L"STATIC", L"Music Volume:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_musicSlider = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, x + labelWidth, y, sliderWidth, 25, m_hwnd, (HMENU)ID_MUSIC_SLIDER, m_hInstance, NULL);
    m_musicEdit = CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + labelWidth + sliderWidth + 5, y, editWidth, 20, m_hwnd, (HMENU)ID_MUSIC_EDIT, m_hInstance, NULL);
    SendMessage(m_musicSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    y += 35;
    
    CreateWindowW(L"STATIC", L"Sound Volume:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_soundSlider = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, x + labelWidth, y, sliderWidth, 25, m_hwnd, (HMENU)ID_SOUND_SLIDER, m_hInstance, NULL);
    m_soundEdit = CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + labelWidth + sliderWidth + 5, y, editWidth, 20, m_hwnd, (HMENU)ID_SOUND_EDIT, m_hInstance, NULL);
    SendMessage(m_soundSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    y += 35;
    
    CreateWindowW(L"STATIC", L"Sensitivity:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_sensitivitySlider = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, x + labelWidth, y, sliderWidth, 25, m_hwnd, (HMENU)ID_SENSITIVITY_SLIDER, m_hInstance, NULL);
    m_sensitivityEdit = CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + labelWidth + sliderWidth + 5, y, editWidth, 20, m_hwnd, (HMENU)ID_SENSITIVITY_EDIT, m_hInstance, NULL);
    SendMessage(m_sensitivitySlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    y += 35;
    
    CreateWindowW(L"STATIC", L"FOV (70-110):", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_fovSlider = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, x + labelWidth, y, sliderWidth, 25, m_hwnd, (HMENU)ID_FOV_SLIDER, m_hInstance, NULL);
    m_fovEdit = CreateWindowW(L"EDIT", L"70", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + labelWidth + sliderWidth + 5, y, editWidth, 20, m_hwnd, (HMENU)ID_FOV_EDIT, m_hInstance, NULL);
    SendMessage(m_fovSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    y += 35;
    
    CreateWindowW(L"STATIC", L"Gamma:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_gammaSlider = CreateWindowW(TRACKBAR_CLASSW, NULL, WS_VISIBLE | WS_CHILD | TBS_HORZ | TBS_AUTOTICKS, x + labelWidth, y, sliderWidth, 25, m_hwnd, (HMENU)ID_GAMMA_SLIDER, m_hInstance, NULL);
    m_gammaEdit = CreateWindowW(L"EDIT", L"0.0", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + labelWidth + sliderWidth + 5, y, editWidth, 20, m_hwnd, (HMENU)ID_GAMMA_EDIT, m_hInstance, NULL);
    SendMessage(m_gammaSlider, TBM_SETRANGE, TRUE, MAKELONG(0, 100));
    y += 45;
    
    CreateWindowW(L"STATIC", L"Boolean Options:", WS_VISIBLE | WS_CHILD | SS_LEFT, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_invertMouseCheck = CreateWindowW(L"BUTTON", L"Invert Mouse", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_INVERT_MOUSE, m_hInstance, NULL);
    y += 25;
    m_bobViewCheck = CreateWindowW(L"BUTTON", L"View Bobbing", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_BOB_VIEW, m_hInstance, NULL);
    y += 25;
    m_anaglyph3dCheck = CreateWindowW(L"BUTTON", L"Anaglyph 3D", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_ANAGLYPH, m_hInstance, NULL);
    y += 25;
    m_advancedOpenglCheck = CreateWindowW(L"BUTTON", L"Advanced OpenGL", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_ADVANCED_OPENGL, m_hInstance, NULL);
    y += 25;
    m_fancyGraphicsCheck = CreateWindowW(L"BUTTON", L"Fancy Graphics", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_FANCY_GRAPHICS, m_hInstance, NULL);
    y += 25;
    m_ambientOcclusionCheck = CreateWindowW(L"BUTTON", L"Ambient Occlusion", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_AMBIENT_OCCLUSION, m_hInstance, NULL);
    y += 25;
    m_renderCloudsCheck = CreateWindowW(L"BUTTON", L"Render Clouds", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 200, 20, m_hwnd, (HMENU)ID_RENDER_CLOUDS, m_hInstance, NULL);
    y += 35;
    
    CreateWindowW(L"STATIC", L"View Distance:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_viewDistanceCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x + labelWidth, y, 150, 200, m_hwnd, (HMENU)ID_VIEW_DISTANCE, m_hInstance, NULL);
    SendMessage(m_viewDistanceCombo, CB_ADDSTRING, 0, (LPARAM)L"Far");
    SendMessage(m_viewDistanceCombo, CB_ADDSTRING, 0, (LPARAM)L"Normal");
    SendMessage(m_viewDistanceCombo, CB_ADDSTRING, 0, (LPARAM)L"Short");
    SendMessage(m_viewDistanceCombo, CB_ADDSTRING, 0, (LPARAM)L"Tiny");
    y += 30;
    
    CreateWindowW(L"STATIC", L"Difficulty:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_difficultyCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x + labelWidth, y, 150, 200, m_hwnd, (HMENU)ID_DIFFICULTY, m_hInstance, NULL);
    SendMessage(m_difficultyCombo, CB_ADDSTRING, 0, (LPARAM)L"Peaceful");
    SendMessage(m_difficultyCombo, CB_ADDSTRING, 0, (LPARAM)L"Easy");
    SendMessage(m_difficultyCombo, CB_ADDSTRING, 0, (LPARAM)L"Normal");
    SendMessage(m_difficultyCombo, CB_ADDSTRING, 0, (LPARAM)L"Hard");
    y += 30;
    
    CreateWindowW(L"STATIC", L"GUI Scale:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_guiScaleCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x + labelWidth, y, 150, 200, m_hwnd, (HMENU)ID_GUI_SCALE, m_hInstance, NULL);
    SendMessage(m_guiScaleCombo, CB_ADDSTRING, 0, (LPARAM)L"Auto");
    SendMessage(m_guiScaleCombo, CB_ADDSTRING, 0, (LPARAM)L"Small");
    SendMessage(m_guiScaleCombo, CB_ADDSTRING, 0, (LPARAM)L"Normal");
    SendMessage(m_guiScaleCombo, CB_ADDSTRING, 0, (LPARAM)L"Large");
    y += 30;
    
    CreateWindowW(L"STATIC", L"Particles:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_particlesCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x + labelWidth, y, 150, 200, m_hwnd, (HMENU)ID_PARTICLES, m_hInstance, NULL);
    SendMessage(m_particlesCombo, CB_ADDSTRING, 0, (LPARAM)L"All");
    SendMessage(m_particlesCombo, CB_ADDSTRING, 0, (LPARAM)L"Decreased");
    SendMessage(m_particlesCombo, CB_ADDSTRING, 0, (LPARAM)L"Minimal");
    y += 30;
    
    CreateWindowW(L"STATIC", L"Framerate Limit:", WS_VISIBLE | WS_CHILD, x, y, labelWidth, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_framerateLimitCombo = CreateWindowW(L"COMBOBOX", NULL, WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST, x + labelWidth, y, 150, 200, m_hwnd, (HMENU)ID_FRAMERATE_LIMIT, m_hInstance, NULL);
    SendMessage(m_framerateLimitCombo, CB_ADDSTRING, 0, (LPARAM)L"Max");
    SendMessage(m_framerateLimitCombo, CB_ADDSTRING, 0, (LPARAM)L"Balanced");
    SendMessage(m_framerateLimitCombo, CB_ADDSTRING, 0, (LPARAM)L"Power Saver");
    y += 35;
    
    CreateWindowW(L"STATIC", L"Performance Options:", WS_VISIBLE | WS_CHILD | SS_LEFT, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_vsyncCheck = CreateWindowW(L"BUTTON", L"VSync (restart required)", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 250, 20, m_hwnd, (HMENU)ID_VSYNC, m_hInstance, NULL);
    SendMessage(m_vsyncCheck, BM_SETCHECK, BST_CHECKED, 0);
    y += 25;
    
    m_unlockFpsCheck = CreateWindowW(L"BUTTON", L"Unlock FPS (experimental)", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 250, 20, m_hwnd, (HMENU)ID_UNLOCK_FPS, m_hInstance, NULL);
    y += 25;
    
    CreateWindowW(L"STATIC", L"Max FPS:", WS_VISIBLE | WS_CHILD, x, y, 80, 20, m_hwnd, NULL, m_hInstance, NULL);
    m_maxFpsEdit = CreateWindowW(L"EDIT", L"300", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER, x + 80, y, 60, 20, m_hwnd, (HMENU)ID_MAX_FPS, m_hInstance, NULL);
    y += 30;
    
    m_fpsLabel = CreateWindowW(L"STATIC", L"Current FPS: 0", WS_VISIBLE | WS_CHILD, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 35;
    
    CreateWindowW(L"STATIC", L"Controller Options:", WS_VISIBLE | WS_CHILD | SS_LEFT, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_swapStickButtonsCheck = CreateWindowW(L"BUTTON", L"L3=Sprint, R3=Camera (restart)", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_SWAP_STICK_BUTTONS, m_hInstance, NULL);
    y += 35;
    
    CreateWindowW(L"STATIC", L"Debug Cheats & Options:", WS_VISIBLE | WS_CHILD | SS_LEFT, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_loadSavesFromDiskCheck = CreateWindowW(L"BUTTON", L"Load Saves From Local Folder", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_LOAD_SAVES_FROM_DISK, m_hInstance, NULL);
    y += 25;
    m_writeSavesToDiskCheck = CreateWindowW(L"BUTTON", L"Write Saves To Local Folder", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_WRITE_SAVES_TO_DISK, m_hInstance, NULL);
    y += 25;
    m_freezePlayersCheck = CreateWindowW(L"BUTTON", L"Freeze Players", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_FREEZE_PLAYERS, m_hInstance, NULL);
    y += 25;
    m_safeareaCheck = CreateWindowW(L"BUTTON", L"Display Safe Area", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_SAFEAREA, m_hInstance, NULL);
    y += 25;
    m_mobsDontAttackCheck = CreateWindowW(L"BUTTON", L"Mobs Don't Attack", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_MOBS_DONT_ATTACK, m_hInstance, NULL);
    y += 25;
    m_freezeTimeCheck = CreateWindowW(L"BUTTON", L"Freeze Time", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_FREEZE_TIME, m_hInstance, NULL);
    y += 25;
    m_disableWeatherCheck = CreateWindowW(L"BUTTON", L"Disable Weather", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_DISABLE_WEATHER, m_hInstance, NULL);
    y += 25;
    m_craftAnythingCheck = CreateWindowW(L"BUTTON", L"Craft Anything", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_CRAFT_ANYTHING, m_hInstance, NULL);
    y += 25;
    m_useDpadForDebugCheck = CreateWindowW(L"BUTTON", L"Use DPad for Debug", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_USE_DPAD_FOR_DEBUG, m_hInstance, NULL);
    y += 25;
    m_mobsDontTickCheck = CreateWindowW(L"BUTTON", L"Mobs Don't Tick", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_MOBS_DONT_TICK, m_hInstance, NULL);
    y += 25;
    m_instantMineCheck = CreateWindowW(L"BUTTON", L"Instant Mine", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_INSTANT_MINE, m_hInstance, NULL);
    y += 25;
    m_showUIConsoleCheck = CreateWindowW(L"BUTTON", L"Show UI Console", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_SHOW_UI_CONSOLE, m_hInstance, NULL);
    y += 25;
    m_distributableSaveCheck = CreateWindowW(L"BUTTON", L"Distributable Save", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_DISTRIBUTABLE_SAVE, m_hInstance, NULL);
    y += 25;
    m_debugLeaderboardsCheck = CreateWindowW(L"BUTTON", L"Debug Leaderboards", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_DEBUG_LEADERBOARDS, m_hInstance, NULL);
    y += 25;
    m_heightWaterBiomeCheck = CreateWindowW(L"BUTTON", L"Height-Water-Biome Maps", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_HEIGHT_WATER_BIOME, m_hInstance, NULL);
    y += 25;
    m_superflatNetherCheck = CreateWindowW(L"BUTTON", L"Superflat Nether", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_SUPERFLAT_NETHER, m_hInstance, NULL);
    y += 25;
    m_moreLightningCheck = CreateWindowW(L"BUTTON", L"More Lightning When Thundering", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_MORE_LIGHTNING, m_hInstance, NULL);
    y += 25;
    m_goToNetherCheck = CreateWindowW(L"BUTTON", L"Go To Nether", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_GO_TO_NETHER, m_hInstance, NULL);
    y += 25;
    m_goToOverworldCheck = CreateWindowW(L"BUTTON", L"Go To Overworld", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_GO_TO_OVERWORLD, m_hInstance, NULL);
    y += 25;
    m_unlockAllDLCCheck = CreateWindowW(L"BUTTON", L"Unlock All DLC", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_UNLOCK_ALL_DLC, m_hInstance, NULL);
    y += 25;
    m_showMarketingGuideCheck = CreateWindowW(L"BUTTON", L"Show Marketing Guide", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX, x, y, 300, 20, m_hwnd, (HMENU)ID_SHOW_MARKETING_GUIDE, m_hInstance, NULL);
    y += 45;
    
    CreateWindowW(L"STATIC", L"Command Console:", WS_VISIBLE | WS_CHILD | SS_LEFT, x, y, 200, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_commandHistory = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        x, y, 560, 150, m_hwnd, (HMENU)ID_COMMAND_HISTORY, m_hInstance, NULL);
    y += 160;
    
    CreateWindowW(L"STATIC", L"Enter Command:", WS_VISIBLE | WS_CHILD, x, y, 120, 20, m_hwnd, NULL, m_hInstance, NULL);
    y += 25;
    
    m_commandInput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", 
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL,
        x, y, 450, 25, m_hwnd, (HMENU)ID_COMMAND_INPUT, m_hInstance, NULL);
    
    m_sendButton = CreateWindowW(L"BUTTON", L"Send", WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        x + 460, y, 100, 25, m_hwnd, (HMENU)ID_SEND_BUTTON, m_hInstance, NULL);
    
    updateControlValues();
}

void DebugMenu::updateControlValues()
{
    if (!m_minecraft || !m_minecraft->options) return;
    
    m_updating = true;
    Options* opt = m_minecraft->options;
    
    SendMessage(m_musicSlider, TBM_SETPOS, TRUE, (LPARAM)(opt->music * 100));
    SetWindowTextW(m_musicEdit, std::to_wstring(opt->music).c_str());
    
    SendMessage(m_soundSlider, TBM_SETPOS, TRUE, (LPARAM)(opt->sound * 100));
    SetWindowTextW(m_soundEdit, std::to_wstring(opt->sound).c_str());
    
    SendMessage(m_sensitivitySlider, TBM_SETPOS, TRUE, (LPARAM)(opt->sensitivity * 100));
    SetWindowTextW(m_sensitivityEdit, std::to_wstring(opt->sensitivity).c_str());
    
    SendMessage(m_fovSlider, TBM_SETPOS, TRUE, (LPARAM)(opt->fov * 100));
    int fovVal = (int)(70 + opt->fov * 50);
    SetWindowTextW(m_fovEdit, std::to_wstring(fovVal).c_str());
    
    SendMessage(m_gammaSlider, TBM_SETPOS, TRUE, (LPARAM)(opt->gamma * 100));
    SetWindowTextW(m_gammaEdit, std::to_wstring(opt->gamma).c_str());
    
    SendMessage(m_invertMouseCheck, BM_SETCHECK, opt->invertYMouse ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_bobViewCheck, BM_SETCHECK, opt->bobView ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_anaglyph3dCheck, BM_SETCHECK, opt->anaglyph3d ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_advancedOpenglCheck, BM_SETCHECK, opt->advancedOpengl ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_fancyGraphicsCheck, BM_SETCHECK, opt->fancyGraphics ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_ambientOcclusionCheck, BM_SETCHECK, opt->ambientOcclusion ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_renderCloudsCheck, BM_SETCHECK, opt->renderClouds ? BST_CHECKED : BST_UNCHECKED, 0);
    
    if (m_viewDistanceCombo && IsWindow(m_viewDistanceCombo))
        SendMessage(m_viewDistanceCombo, CB_SETCURSEL, opt->viewDistance, 0);
    if (m_difficultyCombo && IsWindow(m_difficultyCombo))
        SendMessage(m_difficultyCombo, CB_SETCURSEL, opt->difficulty, 0);
    if (m_guiScaleCombo && IsWindow(m_guiScaleCombo))
        SendMessage(m_guiScaleCombo, CB_SETCURSEL, opt->guiScale, 0);
    if (m_particlesCombo && IsWindow(m_particlesCombo))
        SendMessage(m_particlesCombo, CB_SETCURSEL, opt->particles, 0);
    if (m_framerateLimitCombo && IsWindow(m_framerateLimitCombo))
        SendMessage(m_framerateLimitCombo, CB_SETCURSEL, opt->framerateLimit, 0);
    
    SendMessage(m_loadSavesFromDiskCheck, BM_SETCHECK, (m_debugMask & (1 << 0)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_writeSavesToDiskCheck, BM_SETCHECK, (m_debugMask & (1 << 1)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_freezePlayersCheck, BM_SETCHECK, (m_debugMask & (1 << 2)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_safeareaCheck, BM_SETCHECK, (m_debugMask & (1 << 3)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_mobsDontAttackCheck, BM_SETCHECK, (m_debugMask & (1 << 4)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_freezeTimeCheck, BM_SETCHECK, (m_debugMask & (1 << 5)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_disableWeatherCheck, BM_SETCHECK, (m_debugMask & (1 << 6)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_craftAnythingCheck, BM_SETCHECK, (m_debugMask & (1 << 7)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_useDpadForDebugCheck, BM_SETCHECK, (m_debugMask & (1 << 8)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_mobsDontTickCheck, BM_SETCHECK, (m_debugMask & (1 << 9)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_instantMineCheck, BM_SETCHECK, (m_debugMask & (1 << 10)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_showUIConsoleCheck, BM_SETCHECK, (m_debugMask & (1 << 11)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_distributableSaveCheck, BM_SETCHECK, (m_debugMask & (1 << 12)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_debugLeaderboardsCheck, BM_SETCHECK, (m_debugMask & (1 << 13)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_heightWaterBiomeCheck, BM_SETCHECK, (m_debugMask & (1 << 14)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_superflatNetherCheck, BM_SETCHECK, (m_debugMask & (1 << 15)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_moreLightningCheck, BM_SETCHECK, (m_debugMask & (1 << 16)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_goToNetherCheck, BM_SETCHECK, (m_debugMask & (1 << 17)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_goToOverworldCheck, BM_SETCHECK, (m_debugMask & (1 << 18)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_unlockAllDLCCheck, BM_SETCHECK, (m_debugMask & (1 << 19)) ? BST_CHECKED : BST_UNCHECKED, 0);
    SendMessage(m_showMarketingGuideCheck, BM_SETCHECK, (m_debugMask & (1 << 20)) ? BST_CHECKED : BST_UNCHECKED, 0);
    
    m_updating = false;
}

void DebugMenu::show()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_SHOW);
        m_visible = true;
    }
}

void DebugMenu::hide()
{
    if (m_hwnd)
    {
        ShowWindow(m_hwnd, SW_HIDE);
        m_visible = false;
    }
}

void DebugMenu::update()
{
    if (!m_visible || !m_hwnd) return;
    
    MSG msg;
    while (PeekMessage(&msg, m_hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK DebugMenu::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DebugMenu* menu = (DebugMenu*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    
    switch (uMsg)
    {
    case WM_COMMAND:
        if (menu) menu->onCommand(wParam);
        return 0;
        
    case WM_HSCROLL:
        if (menu) menu->onHScroll(wParam, lParam);
        return 0;
        
    case WM_VSCROLL:
        {
            SCROLLINFO si = { sizeof(SCROLLINFO), SIF_ALL };
            GetScrollInfo(hwnd, SB_VERT, &si);
            int yPos = si.nPos;
            
            switch (LOWORD(wParam))
            {
            case SB_LINEUP: yPos -= 20; break;
            case SB_LINEDOWN: yPos += 20; break;
            case SB_PAGEUP: yPos -= si.nPage; break;
            case SB_PAGEDOWN: yPos += si.nPage; break;
            case SB_THUMBTRACK: yPos = si.nTrackPos; break;
            }
            
            yPos = max(0, min(yPos, si.nMax - (int)si.nPage + 1));
            if (yPos != si.nPos)
            {
                ScrollWindow(hwnd, 0, si.nPos - yPos, NULL, NULL);
                si.nPos = yPos;
                SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                UpdateWindow(hwnd);
            }
        }
        return 0;
        
    case WM_SIZE:
        {
            SCROLLINFO si = { sizeof(SCROLLINFO), SIF_RANGE | SIF_PAGE };
            si.nMin = 0;
            si.nMax = 1400;
            si.nPage = HIWORD(lParam);
            SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
        }
        return 0;
        
    case WM_CLOSE:
        if (menu) menu->hide();
        return 0;
        
    case WM_DESTROY:
        return 0;
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void DebugMenu::onCommand(WPARAM wParam)
{
    if (m_updating || !m_minecraft || !m_minecraft->options) return;
    
    Options* opt = m_minecraft->options;
    int id = LOWORD(wParam);
    int notif = HIWORD(wParam);
    
    if (notif == EN_CHANGE)
    {
        wchar_t buffer[64];
        float val;
        
        switch (id)
        {
        case ID_MUSIC_EDIT:
            {
                GetWindowTextW(m_musicEdit, buffer, 64);
                val = (float)_wtof(buffer);
                if (val >= 0 && val <= 1) {
                    opt->music = val;
                    SendMessage(m_musicSlider, TBM_SETPOS, TRUE, (LPARAM)(val * 100));
                    if (m_minecraft->soundEngine) m_minecraft->soundEngine->updateMusicVolume(val);
                }
            }
            break;
        case ID_SOUND_EDIT:
            {
                GetWindowTextW(m_soundEdit, buffer, 64);
                val = (float)_wtof(buffer);
                if (val >= 0 && val <= 1) {
                    opt->sound = val;
                    SendMessage(m_soundSlider, TBM_SETPOS, TRUE, (LPARAM)(val * 100));
                    if (m_minecraft->soundEngine) m_minecraft->soundEngine->updateSoundEffectVolume(val);
                }
            }
            break;
        case ID_SENSITIVITY_EDIT:
            {
                GetWindowTextW(m_sensitivityEdit, buffer, 64);
                val = (float)_wtof(buffer);
                if (val >= 0 && val <= 1) {
                    opt->sensitivity = val;
                    SendMessage(m_sensitivitySlider, TBM_SETPOS, TRUE, (LPARAM)(val * 100));
                }
            }
            break;
        case ID_FOV_EDIT:
            {
                GetWindowTextW(m_fovEdit, buffer, 64);
                int fovVal = _wtoi(buffer);
                if (fovVal >= 70 && fovVal <= 120) {
                    opt->fov = (fovVal - 70) / 50.0f;
                    SendMessage(m_fovSlider, TBM_SETPOS, TRUE, (LPARAM)(opt->fov * 100));
                }
            }
            break;
        case ID_GAMMA_EDIT:
            {
                GetWindowTextW(m_gammaEdit, buffer, 64);
                val = (float)_wtof(buffer);
                if (val >= 0 && val <= 1) {
                    opt->gamma = val;
                    SendMessage(m_gammaSlider, TBM_SETPOS, TRUE, (LPARAM)(val * 100));
                }
            }
            break;
        case ID_MAX_FPS:
            {
                GetWindowTextW(m_maxFpsEdit, buffer, 64);
                int fps = _wtoi(buffer);
                if (fps > 0 && fps <= 1000) {
                    m_maxFps = fps;
                }
            }
            break;
        }
    }
    else if (notif == BN_CLICKED)
    {
        switch (id)
        {
        case ID_INVERT_MOUSE:
            opt->invertYMouse = !opt->invertYMouse;
            break;
        case ID_BOB_VIEW:
            opt->bobView = !opt->bobView;
            break;
        case ID_ANAGLYPH:
            opt->anaglyph3d = !opt->anaglyph3d;
            if (m_minecraft->textures) m_minecraft->textures->reloadAll();
            break;
        case ID_ADVANCED_OPENGL:
            opt->advancedOpengl = !opt->advancedOpengl;
            if (m_minecraft->levelRenderer) m_minecraft->levelRenderer->allChanged();
            break;
        case ID_FANCY_GRAPHICS:
            opt->fancyGraphics = !opt->fancyGraphics;
            if (m_minecraft->levelRenderer) m_minecraft->levelRenderer->allChanged();
            break;
        case ID_AMBIENT_OCCLUSION:
            opt->ambientOcclusion = !opt->ambientOcclusion;
            if (m_minecraft->levelRenderer) m_minecraft->levelRenderer->allChanged();
            break;
        case ID_RENDER_CLOUDS:
            opt->renderClouds = !opt->renderClouds;
            break;
        case ID_VSYNC:
            m_vsyncEnabled = !m_vsyncEnabled;
            break;
        case ID_UNLOCK_FPS:
            m_unlockFps = !m_unlockFps;
            break;
        case ID_SWAP_STICK_BUTTONS:
            m_swapStickButtons = !m_swapStickButtons;
            break;
        case ID_LOAD_SAVES_FROM_DISK:
            m_debugMask ^= (1 << 0);
            break;
        case ID_WRITE_SAVES_TO_DISK:
            m_debugMask ^= (1 << 1);
            break;
        case ID_FREEZE_PLAYERS:
            m_debugMask ^= (1 << 2);
            break;
        case ID_SAFEAREA:
            m_debugMask ^= (1 << 3);
            break;
        case ID_MOBS_DONT_ATTACK:
            m_debugMask ^= (1 << 4);
            break;
        case ID_FREEZE_TIME:
            m_debugMask ^= (1 << 5);
            break;
        case ID_DISABLE_WEATHER:
            m_debugMask ^= (1 << 6);
            break;
        case ID_CRAFT_ANYTHING:
            m_debugMask ^= (1 << 7);
            break;
        case ID_USE_DPAD_FOR_DEBUG:
            m_debugMask ^= (1 << 8);
            break;
        case ID_MOBS_DONT_TICK:
            m_debugMask ^= (1 << 9);
            break;
        case ID_INSTANT_MINE:
            m_debugMask ^= (1 << 10);
            break;
        case ID_SHOW_UI_CONSOLE:
            m_debugMask ^= (1 << 11);
            break;
        case ID_DISTRIBUTABLE_SAVE:
            m_debugMask ^= (1 << 12);
            break;
        case ID_DEBUG_LEADERBOARDS:
            m_debugMask ^= (1 << 13);
            break;
        case ID_HEIGHT_WATER_BIOME:
            m_debugMask ^= (1 << 14);
            break;
        case ID_SUPERFLAT_NETHER:
            m_debugMask ^= (1 << 15);
            break;
        case ID_MORE_LIGHTNING:
            m_debugMask ^= (1 << 16);
            break;
        case ID_GO_TO_NETHER:
            m_debugMask ^= (1 << 17);
            break;
        case ID_GO_TO_OVERWORLD:
            m_debugMask ^= (1 << 18);
            break;
        case ID_UNLOCK_ALL_DLC:
            m_debugMask ^= (1 << 19);
            break;
        case ID_SHOW_MARKETING_GUIDE:
            m_debugMask ^= (1 << 20);
            break;
        case ID_SEND_BUTTON:
            {
                wchar_t buffer[512];
                GetWindowTextW(m_commandInput, buffer, 512);
                wstring command = buffer;
                if (!command.empty())
                {
                    executeCommand(command);
                    SetWindowTextW(m_commandInput, L"");
                }
            }
            break;
        }
    }
    else if (notif == CBN_SELCHANGE)
    {
        int sel;
        switch (id)
        {
        case ID_VIEW_DISTANCE:
            sel = (int)SendMessage(m_viewDistanceCombo, CB_GETCURSEL, 0, 0);
            opt->viewDistance = sel;
            break;
        case ID_DIFFICULTY:
            sel = (int)SendMessage(m_difficultyCombo, CB_GETCURSEL, 0, 0);
            opt->difficulty = sel;
            break;
        case ID_GUI_SCALE:
            sel = (int)SendMessage(m_guiScaleCombo, CB_GETCURSEL, 0, 0);
            opt->guiScale = sel;
            break;
        case ID_PARTICLES:
            sel = (int)SendMessage(m_particlesCombo, CB_GETCURSEL, 0, 0);
            opt->particles = sel;
            break;
        case ID_FRAMERATE_LIMIT:
            sel = (int)SendMessage(m_framerateLimitCombo, CB_GETCURSEL, 0, 0);
            opt->framerateLimit = sel;
            break;
        }
    }
}

void DebugMenu::onHScroll(WPARAM wParam, LPARAM lParam)
{
    if (m_updating || !m_minecraft || !m_minecraft->options) return;
    
    Options* opt = m_minecraft->options;
    HWND slider = (HWND)lParam;
    int pos = (int)SendMessage(slider, TBM_GETPOS, 0, 0);
    float val = pos / 100.0f;
    
    if (slider == m_musicSlider)
    {
        opt->music = val;
        SetWindowTextW(m_musicEdit, std::to_wstring(val).c_str());
        if (m_minecraft->soundEngine) m_minecraft->soundEngine->updateMusicVolume(val);
    }
    else if (slider == m_soundSlider)
    {
        opt->sound = val;
        SetWindowTextW(m_soundEdit, std::to_wstring(val).c_str());
        if (m_minecraft->soundEngine) m_minecraft->soundEngine->updateSoundEffectVolume(val);
    }
    else if (slider == m_sensitivitySlider)
    {
        opt->sensitivity = val;
        SetWindowTextW(m_sensitivityEdit, std::to_wstring(val).c_str());
    }
    else if (slider == m_fovSlider)
    {
        opt->fov = val;
        int fovVal = (int)(70 + val * 50);
        SetWindowTextW(m_fovEdit, std::to_wstring(fovVal).c_str());
    }
    else if (slider == m_gammaSlider)
    {
        opt->gamma = val;
        SetWindowTextW(m_gammaEdit, std::to_wstring(val).c_str());
    }
}

void DebugMenu::executeCommand(const wstring& command)
{
    if (!m_minecraft) return;
    
    addCommandToHistory(L"> " + command);
    
    if (m_minecraft->handleClientSideCommand(L"/" + command))
    {
        addCommandToHistory(L"Command executed successfully.");
    }
    else
    {
        addCommandToHistory(L"Unknown command or invalid syntax.");
    }
}

void DebugMenu::addCommandToHistory(const wstring& text)
{
    if (!m_commandHistory) return;
    
    int len = GetWindowTextLengthW(m_commandHistory);
    SendMessageW(m_commandHistory, EM_SETSEL, len, len);
    SendMessageW(m_commandHistory, EM_REPLACESEL, FALSE, (LPARAM)(text + L"\r\n").c_str());
    SendMessageW(m_commandHistory, EM_SCROLLCARET, 0, 0);
}
