#pragma once
#include <windows.h>
#include <commctrl.h>
#include <string>

using std::wstring;

class Options;
class Minecraft;

class DebugMenu
{
public:
    DebugMenu();
    ~DebugMenu();
    
    void init(HINSTANCE hInstance, Minecraft* mc);
    void show();
    void hide();
    void update();
    bool isVisible() const { return m_visible; }
    
    bool m_swapStickButtons;
    unsigned int m_debugMask;
    
private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void createControls();
    void updateControlValues();
    void onCommand(WPARAM wParam);
    void onHScroll(WPARAM wParam, LPARAM lParam);
    
    HWND m_hwnd;
    HINSTANCE m_hInstance;
    Minecraft* m_minecraft;
    bool m_visible;
    
    HWND m_musicSlider;
    HWND m_musicEdit;
    HWND m_soundSlider;
    HWND m_soundEdit;
    HWND m_sensitivitySlider;
    HWND m_sensitivityEdit;
    HWND m_fovSlider;
    HWND m_fovEdit;
    HWND m_gammaSlider;
    HWND m_gammaEdit;
    
    HWND m_invertMouseCheck;
    HWND m_bobViewCheck;
    HWND m_anaglyph3dCheck;
    HWND m_advancedOpenglCheck;
    HWND m_fancyGraphicsCheck;
    HWND m_ambientOcclusionCheck;
    HWND m_renderCloudsCheck;
    
    HWND m_viewDistanceCombo;
    HWND m_difficultyCombo;
    HWND m_guiScaleCombo;
    HWND m_particlesCombo;
    HWND m_framerateLimitCombo;
    
    HWND m_vsyncCheck;
    HWND m_unlockFpsCheck;
    HWND m_maxFpsEdit;
    HWND m_fpsLabel;
    
    HWND m_swapStickButtonsCheck;
    
    HWND m_loadSavesFromDiskCheck;
    HWND m_writeSavesToDiskCheck;
    HWND m_freezePlayersCheck;
    HWND m_safeareaCheck;
    HWND m_mobsDontAttackCheck;
    HWND m_freezeTimeCheck;
    HWND m_disableWeatherCheck;
    HWND m_craftAnythingCheck;
    HWND m_useDpadForDebugCheck;
    HWND m_mobsDontTickCheck;
    HWND m_instantMineCheck;
    HWND m_showUIConsoleCheck;
    HWND m_distributableSaveCheck;
    HWND m_debugLeaderboardsCheck;
    HWND m_heightWaterBiomeCheck;
    HWND m_superflatNetherCheck;
    HWND m_moreLightningCheck;
    HWND m_goToNetherCheck;
    HWND m_goToOverworldCheck;
    HWND m_unlockAllDLCCheck;
    HWND m_showMarketingGuideCheck;
    
    HWND m_commandInput;
    HWND m_sendButton;
    HWND m_commandHistory;
    
    bool m_updating;
    bool m_vsyncEnabled;
    bool m_unlockFps;
    int m_maxFps;
    
    void executeCommand(const wstring& command);
    void addCommandToHistory(const wstring& command);
};
