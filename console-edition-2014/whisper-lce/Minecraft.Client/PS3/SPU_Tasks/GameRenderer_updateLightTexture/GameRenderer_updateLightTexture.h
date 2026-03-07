#pragma once

static const int LEVEL_MAX_BRIGHTNESS = 15;
class GameRenderer_updateLightTexture_dataIn
{
public:
	class LevelData
	{
	public:
		bool m_valid;
		float m_skyDarken;
		float m_brightnessRamp[LEVEL_MAX_BRIGHTNESS + 1];
		int m_lightningBoltTime;
		int m_dimensionID;
		int* m_lightPixelsOutput;
	};

	LevelData m_levelData[3];
	float blr;
};
