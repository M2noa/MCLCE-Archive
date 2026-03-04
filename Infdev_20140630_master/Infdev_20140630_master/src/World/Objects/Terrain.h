#pragma once

#include "stdafx.h"
#include <vector>

#include "Rendering/Objects/Mesh.h"
#include "Rendering/Objects/Texture2D.h"
#include "Rendering/Objects/IShader.h"
#include "Rendering/Objects/RenderObject.h"
#include "Rendering/Objects/Material.h"

#include "./IWorldObject.h"

#include "Physics/Physics.h"
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>
//#include <BulletCollision/CollisionShapes/btTriangleBuffer.h>

namespace Esteem
{
	class Terrain : public IWorldObject
	{
	private:
		World* world;
		btCollisionObject physicsObject;
		cgc::strong_ptr<RenderObject> renderObject;

		uint terrainVBO;
		cgc::strong_ptr<Texture2D> heightMapTexture;
		cgc::strong_ptr<Texture2D> detailMapTexture;
		cgc::strong_ptr<Texture2D> splatMapTexture;
		cgc::strong_ptr<Material> terrainMaterial;
		cgc::strong_ptr<IShader> terrainShader;

		uint outerSize;
		uint innerSize;
		uint minQuadSize;
		uint maxQuadSize;

		cgc::strong_ptr<Mesh<ModelVertexDataA>> mesh;
		glm::mat4* matrix;
		inline glm::mat4* GetMatrix() { return matrix; }

		void Initialize(uint outerSize, uint innerSize, uint minQuadSize, uint maxQuadSize, uint startSecondLaneDistance, uint increaseLaneDistance);
		void CreateLane(float size, int increment, float quadSize, std::vector<ModelVertexDataA>& data, std::vector<uint>& indices, uint& lastMadeAmount);
		void CreatePatchLane(float size, int increment, float quadSize, std::vector<ModelVertexDataA>& data, std::vector<uint>& indices, uint& lastMadeAmount);

		// physics data

		int s_gridSize;  // must be (2^N) + 1
		float s_gridSpacing;
		float s_gridHeightScale;

		float* terrainData;
		btHeightfieldTerrainShape* heightFieldShape;

		World* worldData;
		
	public:
		Terrain(uint outerSize, uint innerSize, uint minQuadSize, uint maxQuadSize, uint startSecondLaneDistance, uint increaseLaneDistance, World* world);
		virtual ~Terrain();

		virtual void RenderUpdate();

		virtual void LateRenderUpdate();

		virtual void PhysicsUpdate();

		virtual void DirtyCleanUp() {}

		virtual World* GetWorld() { return this->world; }
	};
}