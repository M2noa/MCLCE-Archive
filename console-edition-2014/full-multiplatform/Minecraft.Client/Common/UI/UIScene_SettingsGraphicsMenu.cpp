#include "stdafx.h"
#include "UI.h"
#include "UIScene_SettingsGraphicsMenu.h"

UIScene_SettingsGraphicsMenu::UIScene_SettingsGraphicsMenu(int iPad, void *initData, UILayer *parentLayer) : UIScene(iPad, parentLayer)
{
	// Setup all the Iggy references we need for this scene
	initialiseMovie();
	
	m_bNotInGame=(Minecraft::GetInstance()->level==NULL);

	m_checkboxClouds.init(app.GetString(IDS_CHECKBOX_RENDER_CLOUDS),eControl_Clouds,(app.GetGameSettings(m_iPad,eGameSetting_Clouds)!=0));
	m_checkboxBedrockFog.init(app.GetString(IDS_CHECKBOX_RENDER_BEDROCKFOG),eControl_BedrockFog,(app.GetGameSettings(m_iPad,eGameSetting_BedrockFog)!=0));
	m_checkboxCustomSkinAnim.init(app.GetString(IDS_CHECKBOX_CUSTOM_SKIN_ANIM),eControl_CustomSkinAnim,(app.GetGameSettings(m_iPad,eGameSetting_CustomSkinAnim)!=0));

	m_checkboxShowFPS.init(L"Show FPS",eControl_ShowFPS,(app.GetGameSettings(m_iPad,eGameSetting_ShowFPS)!=0));
	m_checkboxShadows.init(L"Shadows",eControl_Shadows,(app.GetGameSettings(m_iPad,eGameSetting_Shadows)!=0));
	m_checkboxSmoothLighting.init(L"Smooth Lighting",eControl_SmoothLighting,(app.GetGameSettings(m_iPad,eGameSetting_SmoothLighting)!=0));
	m_checkboxEntityShadows.init(L"Entity Shadows",eControl_EntityShadows,(app.GetGameSettings(m_iPad,eGameSetting_EntityShadows)!=0));
	m_checkboxVSync.init(L"VSync",eControl_VSync,(app.GetGameSettings(m_iPad,eGameSetting_VSync)!=0));
	
	WCHAR TempString[256];
	
	swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_GAMMA ),app.GetGameSettings(m_iPad,eGameSetting_Gamma));	
	m_sliderGamma.init(TempString,eControl_Gamma,0,100,app.GetGameSettings(m_iPad,eGameSetting_Gamma));
	
	swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_INTERFACEOPACITY ),app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));	
	m_sliderInterfaceOpacity.init(TempString,eControl_InterfaceOpacity,0,100,app.GetGameSettings(m_iPad,eGameSetting_InterfaceOpacity));

	int renderDist = app.GetGameSettings(m_iPad,eGameSetting_RenderDistance);
	if(renderDist == 0) renderDist = 8;
	swprintf( (WCHAR *)TempString, 256, L"Render Distance: %d chunks", renderDist);	
	m_sliderRenderDistance.init(TempString,eControl_RenderDistance,2,32,renderDist);

	int fpsLimit = app.GetGameSettings(m_iPad,eGameSetting_FPSLimit);
	if(fpsLimit == 0) fpsLimit = 60;
	swprintf( (WCHAR *)TempString, 256, L"FPS Limit: %d", fpsLimit);	
	m_sliderFPSLimit.init(TempString,eControl_FPSLimit,30,240,fpsLimit);

	int particleQuality = app.GetGameSettings(m_iPad,eGameSetting_ParticleQuality);
	swprintf( (WCHAR *)TempString, 256, L"Particles: %d%%", particleQuality);	
	m_sliderParticleQuality.init(TempString,eControl_ParticleQuality,0,100,particleQuality);

	doHorizontalResizeCheck();
	
	bool bInGame=(Minecraft::GetInstance()->level!=NULL);
	bool bIsPrimaryPad=(ProfileManager.GetPrimaryPad()==m_iPad);
	// if we're not in the game, we need to use basescene 0 
	if(bInGame)
	{
		// If the game has started, then you need to be the host to change the in-game gamertags
		if(bIsPrimaryPad)
		{	
			// we are the primary player on this machine, but not the game host
			// are we the game host? If not, we need to remove the bedrockfog setting
			if(!g_NetworkManager.IsHost())
			{
				// hide the in-game bedrock fog setting
				removeControl(&m_checkboxBedrockFog, true);
			}
		}
		else
		{
			// We shouldn't have the bedrock fog option, or the m_CustomSkinAnim option
			removeControl(&m_checkboxBedrockFog, true);
			removeControl(&m_checkboxCustomSkinAnim, true);
		}
	}

	if(app.GetLocalPlayerCount()>1)
	{
#if TO_BE_IMPLEMENTED
		app.AdjustSplitscreenScene(m_hObj,&m_OriginalPosition,m_iPad);
#endif
	}
}

UIScene_SettingsGraphicsMenu::~UIScene_SettingsGraphicsMenu()
{
}

wstring UIScene_SettingsGraphicsMenu::getMoviePath()
{
	if(app.GetLocalPlayerCount() > 1)
	{
		return L"SettingsGraphicsMenuSplit";
	}
	else
	{
		return L"SettingsGraphicsMenu";
	}
}

void UIScene_SettingsGraphicsMenu::updateTooltips()
{
	ui.SetTooltips( m_iPad, IDS_TOOLTIPS_SELECT,IDS_TOOLTIPS_BACK);
}

void UIScene_SettingsGraphicsMenu::updateComponents()
{
	bool bNotInGame=(Minecraft::GetInstance()->level==NULL);
	if(bNotInGame)
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,true);
		m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
	}
	else
	{
		m_parentLayer->showComponent(m_iPad,eUIComponent_Panorama,false);
	
		if( app.GetLocalPlayerCount() == 1 ) m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,true);
		else m_parentLayer->showComponent(m_iPad,eUIComponent_Logo,false);

	}
}

void UIScene_SettingsGraphicsMenu::handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled)
{
	ui.AnimateKeyPress(iPad, key, repeat, pressed, released);
	switch(key)
	{
	case ACTION_MENU_CANCEL:
		if(pressed)
		{
			app.SetGameSettings(m_iPad,eGameSetting_Clouds,m_checkboxClouds.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_BedrockFog,m_checkboxBedrockFog.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_CustomSkinAnim,m_checkboxCustomSkinAnim.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_ShowFPS,m_checkboxShowFPS.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_Shadows,m_checkboxShadows.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_SmoothLighting,m_checkboxSmoothLighting.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_EntityShadows,m_checkboxEntityShadows.IsChecked()?1:0);
			app.SetGameSettings(m_iPad,eGameSetting_VSync,m_checkboxVSync.IsChecked()?1:0);

			navigateBack();
			handled = true;
		}
		break;
	case ACTION_MENU_OK:
#ifdef __ORBIS__
	case ACTION_MENU_TOUCHPAD_PRESS:
#endif
		sendInputToMovie(key, repeat, pressed, released);
		break;
	case ACTION_MENU_UP:
	case ACTION_MENU_DOWN:
	case ACTION_MENU_LEFT:
	case ACTION_MENU_RIGHT:
		sendInputToMovie(key, repeat, pressed, released);
		break;
	}
}

void UIScene_SettingsGraphicsMenu::handleSliderMove(F64 sliderId, F64 currentValue)
{
	WCHAR TempString[256];
	int value = (int)currentValue;
	switch((int)sliderId)
	{
	case eControl_Gamma:
		m_sliderGamma.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_Gamma,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_GAMMA ),value);
		m_sliderGamma.setLabel(TempString);

		break;
	case eControl_InterfaceOpacity:
		m_sliderInterfaceOpacity.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_InterfaceOpacity,value);
		swprintf( (WCHAR *)TempString, 256, L"%ls: %d%%", app.GetString( IDS_SLIDER_INTERFACEOPACITY ),value);	
		m_sliderInterfaceOpacity.setLabel(TempString);

		break;
	case eControl_RenderDistance:
		m_sliderRenderDistance.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_RenderDistance,value);
		swprintf( (WCHAR *)TempString, 256, L"Render Distance: %d chunks",value);
		m_sliderRenderDistance.setLabel(TempString);

		break;
	case eControl_FPSLimit:
		m_sliderFPSLimit.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_FPSLimit,value);
		swprintf( (WCHAR *)TempString, 256, L"FPS Limit: %d",value);
		m_sliderFPSLimit.setLabel(TempString);

		break;
	case eControl_ParticleQuality:
		m_sliderParticleQuality.handleSliderMove(value);
		
		app.SetGameSettings(m_iPad,eGameSetting_ParticleQuality,value);
		swprintf( (WCHAR *)TempString, 256, L"Particles: %d%%",value);
		m_sliderParticleQuality.setLabel(TempString);

		break;
	}
}
