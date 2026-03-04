#pragma once

#include "stdafx.h"

#include "./AbstractConstituent.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Rendering/Objects/LightObject.h"
#include <cppu/cgc/array.h>

namespace Esteem
{
	class RenderingFactory;

	class Light : public AbstractConstituent
	{
		friend class RenderingFactory;
		friend class cgc::constructor;

	private:
		LightRenderData lightData;
		Quaternion rotation;

		RenderingFactory* factory;
		World* world;

		bool enabled;

		Light(const cgc::strong_ptr<Entity>& entity, const LightRenderData& lightData);
		
	public:

		static cgc::strong_ptr<Light> Instantiate(const cgc::strong_ptr<Entity>& entity);
		~Light();

		void Initialize();
		
		void Update();

		bool GetEnabled() const { return enabled; }
		void SetEnabled(bool enable) { enabled = enable; }

		inline uint32_t GetLightIndex() const { return lightData.index; }
		inline const glm::mat4* GetModelMatrix() const { return lightData.matrix; }

		inline LightData* GetLightData() { return lightData.data; }
		inline const LightData* GetLightData() const { return lightData.data; }

		void SetShadowsEnabled(bool enable);

		void SetType(LightData::LightType lightType);
		LightData::LightType GetType() const;

		void SetDistance(const float distance);
		float GetDistance() const;

		void SetBrightness(const float brightness);
		float GetBrightness() const;

		void SetColor(const Color& color);
		const Color& GetColor() const;

		void DirtyCleanUp();
	};
}