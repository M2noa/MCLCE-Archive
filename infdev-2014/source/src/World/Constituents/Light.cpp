#include "Light.h"
#include "../Objects/Entity.h"
#include "Utils/Data.h"
#include "Rendering/RenderingFactory.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Math/Math.h"

#include "World/World.h"


namespace Esteem
{
	Light::Light(const cgc::strong_ptr<Entity>& entity, const LightRenderData& lightData)
		: AbstractConstituent(entity)
		, lightData(lightData)
		, world(entity->GetWorld())
	{
	}

	cgc::strong_ptr<Light> Light::Instantiate(const cgc::strong_ptr<Entity>& entity)
	{
		World* world = entity->GetWorld();

		RenderingFactory* factory = Data::GetRenderingFactory();
		auto lightData = factory->LoadLight();

		world->AddLight(lightData);
		auto ptr = world->GetWorldConstituents().lights.Create(entity, lightData);

		ptr->Initialize();

		return ptr;
	}

#define entity GetEntity()
	Light::~Light()
	{
		lightData.data->type = LightData::NONE_REUSE;

		auto& lights = world->GetLights();
		auto it = std::find_if(lights.begin(), lights.end(), [this](const LightRenderData& light) { return light.index == this->lightData.index; });
		if (it != lights.end())
			lights.erase(it);
	}

	void Light::Initialize()
	{
		Light::DirtyCleanUp();
	}

	void Light::Update()
	{

	}

	void Light::DirtyCleanUp()
	{
		lightData.data->position = entity->GetPosition();
		lightData.data->forward = entity->GetRotation() * Vector3::forward;
		*lightData.matrix = glm::transpose(Matrix4x4(entity->GetRotation(), glm::vec3(lightData.data->distance), lightData.data->position));
	}

	void Light::SetShadowsEnabled(bool enable)
	{
		lightData.data->shadowInfoIndex = enable ? 0 : -1;
		entity->SetDirty();
	}

	void Light::SetType(LightData::LightType lightType)
	{
		lightData.data->type = lightType;
	}

	LightData::LightType Light::GetType() const
	{
		return (LightData::LightType)lightData.data->type;
	}

	void Light::SetDistance(const float distance)
	{
		lightData.data->distance = distance;
	}

	float Light::GetDistance() const
	{
		return lightData.data->distance;
	}

	void Light::SetBrightness(const float brightness)
	{
		lightData.data->color.a = brightness;
	}

	float Light::GetBrightness() const
	{
		return lightData.data->color.a;
	}

	void Light::SetColor(const Color& color)
	{
		lightData.data->color.r = color.r;
		lightData.data->color.g = color.g;
		lightData.data->color.b = color.b;
	}

	const Color& Light::GetColor() const
	{
		return lightData.data->color;
	}
}

#undef entity