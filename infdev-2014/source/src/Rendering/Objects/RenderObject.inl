#pragma once

#include "./RenderObject.h"

namespace Esteem
{
	inline RenderObject::RenderObject(const cgc::strong_ptr<RenderObject>& copy)
		: material(copy->material)
		, mesh(copy->mesh)
		, boneMatrices(copy->boneMatrices)
		, renderOrder(copy->renderOrder)
		, state(copy->state)
		, instances(copy->instances)
		, modelMatrix(copy->modelMatrix)
		, customBuffers(copy->customBuffers)
	{
	}
	
	inline RenderObject::RenderObject(const cgc::strong_ptr<Material>& material, const cgc::strong_ptr<AbstractMesh>& mesh)
		: material(material)
		, mesh(mesh)
		, boneMatrices()
		, renderOrder(RenderOrder::OPAQUE_)
		, state(0)
		, instances(1)
		, modelMatrix(nullptr)
		, customBuffers(nullptr)
	{
	}

	inline RenderObject::~RenderObject()
	{
		delete customBuffers;
	}

	inline void RenderObject::SetModelMatrix(const glm::mat4* modelMatrix)
	{
		this->modelMatrix = modelMatrix;
	}

	inline const glm::mat4* RenderObject::GetModelMatrix() const
	{
		return modelMatrix;
	}

	inline const cgc::strong_ptr<Material>& RenderObject::GetMaterial() const
	{
		return material;
	}

	inline void RenderObject::SetMaterial(const cgc::strong_ptr<Material>& material)
	{
		this->material = material;
	}

	inline const glm::vec3& RenderObject::GetPosition() const
	{
		return reinterpret_cast<const glm::vec3&>((*this->modelMatrix)[3][0]);
	}

	inline uint16 RenderObject::GetInstanceCount() const
	{
		return instances;
	}
	
	inline void RenderObject::SetInstanceCount(uint16 instances)
	{
		this->instances = instances;
	}

	inline void RenderObject::SetRenderOrder(RenderOrder renderOrder)
	{
		assert(renderOrder < RenderOrder::COUNT);
		this->renderOrder = renderOrder;
	}

	inline RenderObject::RenderOrder RenderObject::GetRenderOrder() const
	{
		return renderOrder;
	}

	inline bool RenderObject::IsInitialized() const
	{
		return initialized;
	}

	inline bool RenderObject::IsStatic() const
	{
		return isStatic;
	}

	inline bool RenderObject::IsTranslucent() const
	{
		return isTranslucent;
	}

	inline void RenderObject::SetCastingShadows(bool enable)
	{
		castsShadows = enable;
	}

	inline bool RenderObject::IsCastingShadows() const
	{
		return castsShadows;
	}

	inline void RenderObject::SetCustomBuffer(Material::Buffers* buffer)
	{
		customBuffers = buffer;
	}

	inline Material::Buffers* RenderObject::GetCustomBuffer() const
	{
		return customBuffers;
	}

	inline cgc::strong_ptr<RenderObject> RenderObject::Construct(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType, TRIANGLE_TYPE triangleType)
	{
		return RenderingFactory::Instance()->LoadRenderObject(mesh, material, uploadType, triangleType);
	}

	inline cgc::strong_ptr<RenderObject> RenderObject::Construct(cgc::strong_ptr<AbstractMesh> mesh, const cgc::strong_ptr<IMeshData>& meshData, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType, TRIANGLE_TYPE triangleType)
	{
		mesh->SetMeshData(meshData);
		return RenderingFactory::Instance()->LoadRenderObject(mesh, material, uploadType, triangleType);
	}

	inline cgc::strong_ptr<RenderObject> RenderObject::Construct(const cgc::strong_ptr<RenderObject>& copy)
	{
		return RenderingFactory::Instance()->LoadRenderObject(copy);
	}
}