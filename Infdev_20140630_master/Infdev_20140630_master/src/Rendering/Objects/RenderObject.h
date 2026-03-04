#pragma once

#include "Rendering/Objects/Material.h"
#include "Rendering/Objects/VertexData/ModelVertexDataA.h"
#include "Rendering/Objects/BoneMatrices.h"
#include "Rendering/RenderingFactory.h"
#include "AbstractMesh.h"

#undef OPAQUE

namespace Esteem
{
	class RenderObject
	{
		friend class cgc::constructor;
	public:
		enum RenderOrder : uint8
		{
			OPAQUE_ = 0, // FU windows for the #define OPAQUE
			OPAQUE_DOUBLE_SIDED,
			OPAQUE_PARTICLES,
			TRANSLUCENT,
			TRANSLUCENT_PARTICLES,
			OVERLAY,

			COUNT
		};

	protected:
		cgc::strong_ptr<Material> material;
		cgc::strong_ptr<AbstractMesh> mesh;
		const glm::mat4* modelMatrix;
		Material::Buffers* customBuffers; // TODO: might want to move this in some kind of intermediate custom material

		union
		{
			struct
			{
				bool initialized : 1;
				bool isStatic : 1;
				bool isTranslucent : 1;
				bool castsShadows : 1;
			};
			uint8 state;
		};

		RenderOrder renderOrder;

		uint16 instances;

		RenderObject(const cgc::strong_ptr<Material>& material, const cgc::strong_ptr<AbstractMesh>& mesh);

	public:
		RenderObject(const cgc::strong_ptr<RenderObject>& copy);
		~RenderObject();

		cgc::strong_ptr<BoneMatrices> boneMatrices;

		void SetModelMatrix(const glm::mat4* modelMatrix);
		const glm::mat4* GetModelMatrix() const;
		
		const cgc::strong_ptr<Material>& GetMaterial() const;
		void SetMaterial(const cgc::strong_ptr<Material>& material);

		const glm::vec3& GetPosition() const;

		const cgc::strong_ptr<AbstractMesh>& GetMesh() const { return mesh; }

		uint16 GetInstanceCount() const;
		void SetInstanceCount(uint16 instances);

		void SetRenderOrder(RenderOrder renderOrder);
		RenderOrder GetRenderOrder() const;

		bool IsInitialized() const;
		bool IsStatic() const;
		bool IsTranslucent() const;

		void SetCastingShadows(bool enable);
		bool IsCastingShadows() const;
		
		void SetCustomBuffer(Material::Buffers* buffer);
		Material::Buffers* GetCustomBuffer() const;

		static cgc::strong_ptr<RenderObject> Construct(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType = UPLOAD_TYPE::STATIC, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES);
		static cgc::strong_ptr<RenderObject> Construct(cgc::strong_ptr<AbstractMesh> mesh, const cgc::strong_ptr<IMeshData>& meshData, const cgc::strong_ptr<Material>& material, UPLOAD_TYPE uploadType = UPLOAD_TYPE::STATIC, TRIANGLE_TYPE triangleType = TRIANGLE_TYPE::TRIANGLES);
		static cgc::strong_ptr<RenderObject> Construct(const cgc::strong_ptr<RenderObject>& copy);
	};
}

#include "./RenderObject.inl"