#include "MeshRenderer.h"

#include "../Objects/Entity.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Math/Math.h"
#include "Utils/Data.h"
#include "Model/ModelFactory.h"
#include "Window/View.h"

#include "World/World.h"

namespace Esteem
{
	cgc::strong_ptr<MeshRenderer> MeshRenderer::Instantiate(const cgc::strong_ptr<Entity>& entity, bool dynamic)
	{
		World* worldData = entity->GetWorld();

		cgc::strong_ptr<MeshRenderer> meshRenderer = worldData->GetWorldConstituents().meshRenderers.Create(entity, dynamic);
		meshRenderer->Initialize();

		return meshRenderer;
	}

#define entity GetEntity()

	MeshRenderer::~MeshRenderer()
	{
		delete cullingObject;
	}

	void MeshRenderer::Initialize()
	{
		entity->UpdateMatrix();
	}

	void MeshRenderer::LoadModel(const std::string& path)
	{
		uint dotPosition = path.find_last_of('.');
		std::string extensionLess = path.substr(0, dotPosition);

		ModelFactory& modelFactory = Data::GetModelFactory();
		model = modelFactory.LoadModel(path, extensionLess, Model::ModelGenerateSettings::GENERATE_NORMALS);

		SetModel(model);
	}
	
	void MeshRenderer::SetModel(const cgc::strong_ptr<const Model>& model)
	{
		if (this->model = model)
		{
			const std::vector<cgc::strong_ptr<Mesh<ModelVertexDataA>>>& meshes = model->meshes;
			const std::vector<cgc::strong_ptr<Material>>& materials = model->materials;

			std::vector<cgc::strong_ptr<RenderObject>> renderObjects;
			renderObjects.reserve(meshes.size());

			for (uint i = 0; i < meshes.size(); ++i)
			{
				cgc::strong_ptr<RenderObject> renderObject = RenderObject::Construct(cgc::static_pointer_cast<AbstractMesh>(meshes[i]), materials[i]);
				renderObject->SetModelMatrix(&entity->GetMatrix());
				renderObjects.push_back(renderObject);
			}

			CullingObject::Type type = dynamic ? CullingObject::Type::DYNAMIC : CullingObject::Type::STATIC;
			cullingObject = new CullingObject(std::move(renderObjects), model->center, model->volume, entity.ptr(), type);

			entity->GetWorld()->AddCullingObject(cullingObject);
		}
		else
			Debug::LogError("MeshRenderer::SetCustomModel() given model is NULL");
	}

	void MeshRenderer::SetCastingShadows(bool enable)
	{
		for (auto& renderObject : cullingObject->GetRenderObjects())
			renderObject->SetCastingShadows(enable);
	}

	void MeshRenderer::DirtyCleanUp()
	{
		if(model != nullptr)
		{
			/*cullingObject->SetHalfVolume(entity->GetRotation() * (entity->GetScale() * model->volume));
			cullingObject->SetCenter(entity->GetPosition() + (entity->GetRotation() * (model->center * entity->GetScale())));*/
			entity->UpdateMatrix();

			cullingObject->SetAABBFromTransform(model->center, model->volume, entity.ptr());
			cullingObject->TransformUpdate();
		}
	}
}

#undef entity