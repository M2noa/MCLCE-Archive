#pragma once

#include "UIScene.h"

class UIScene_SettingsGraphicsMenu : public UIScene
{
private:
	enum EControls
	{
		eControl_Clouds,
		eControl_BedrockFog,
		eControl_CustomSkinAnim,
		eControl_Gamma,
		eControl_InterfaceOpacity,
		eControl_ShowFPS,
		eControl_RenderDistance,
		eControl_FPSLimit,
		eControl_Shadows,
		eControl_ParticleQuality,
		eControl_SmoothLighting,
		eControl_EntityShadows,
		eControl_VSync
	};

	UIControl_CheckBox m_checkboxClouds, m_checkboxBedrockFog, m_checkboxCustomSkinAnim;
	UIControl_CheckBox m_checkboxShowFPS, m_checkboxShadows, m_checkboxSmoothLighting, m_checkboxEntityShadows, m_checkboxVSync;
	UIControl_Slider m_sliderGamma, m_sliderInterfaceOpacity, m_sliderRenderDistance, m_sliderFPSLimit, m_sliderParticleQuality;
	UI_BEGIN_MAP_ELEMENTS_AND_NAMES(UIScene)
		UI_MAP_ELEMENT( m_checkboxClouds, "Clouds")
		UI_MAP_ELEMENT( m_checkboxBedrockFog, "BedrockFog")
		UI_MAP_ELEMENT( m_checkboxCustomSkinAnim, "CustomSkinAnim")
		UI_MAP_ELEMENT( m_sliderGamma, "Gamma")
		UI_MAP_ELEMENT( m_sliderInterfaceOpacity, "InterfaceOpacity")
		UI_MAP_ELEMENT( m_checkboxShowFPS, "ShowFPS")
		UI_MAP_ELEMENT( m_sliderRenderDistance, "RenderDistance")
		UI_MAP_ELEMENT( m_sliderFPSLimit, "FPSLimit")
		UI_MAP_ELEMENT( m_checkboxShadows, "Shadows")
		UI_MAP_ELEMENT( m_sliderParticleQuality, "ParticleQuality")
		UI_MAP_ELEMENT( m_checkboxSmoothLighting, "SmoothLighting")
		UI_MAP_ELEMENT( m_checkboxEntityShadows, "EntityShadows")
		UI_MAP_ELEMENT( m_checkboxVSync, "VSync")
	UI_END_MAP_ELEMENTS_AND_NAMES()

	bool m_bNotInGame;
public:
	UIScene_SettingsGraphicsMenu(int iPad, void *initData, UILayer *parentLayer);
	virtual ~UIScene_SettingsGraphicsMenu();

	virtual EUIScene getSceneType() { return eUIScene_SettingsGraphicsMenu;}
	
	virtual void updateTooltips();
	virtual void updateComponents();

protected:
	// TODO: This should be pure virtual in this class
	virtual wstring getMoviePath();

public:
	// INPUT
	virtual void handleInput(int iPad, int key, bool repeat, bool pressed, bool released, bool &handled);

	virtual void handleSliderMove(F64 sliderId, F64 currentValue);
};