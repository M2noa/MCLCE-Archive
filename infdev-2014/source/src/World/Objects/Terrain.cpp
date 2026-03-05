#include "Terrain.h"
#include <glm/gtc/matrix_transform.hpp>

#include "Window/View.h"
#include "Rendering/Renderers/OpenGL/Objects/OpenGLMaterial.h"
#include "World/World.h"

#include "BulletCollision/CollisionShapes/btHeightfieldTerrainShape.cpp"
#include "Physics/Physics.h"

namespace Esteem
{
	Terrain::Terrain(uint outerSize, uint innerSize, uint minQuadSize, uint maxQuadSize, uint startSecondLaneDistance, uint increaseLaneDistance, World* world)
		: world(world)
	{
		Initialize(outerSize, innerSize, minQuadSize, maxQuadSize, startSecondLaneDistance, increaseLaneDistance);
	}

	Terrain::~Terrain()
	{
		delete matrix;
	}

	void Terrain::RenderUpdate()
	{
		RenderCamera* renderCamera = world->GetPrimaryCamera()->GetRenderCameraData().ptr();
		glm::vec3 centerPosition = renderCamera->data.position;
		centerPosition.x = (float)div((int)floor(centerPosition.x), 64).quot * 64.f + 16.f;
		centerPosition.y = 0;
		centerPosition.z = (float)div((int)floor(centerPosition.z), 64).quot * 64.f + 16.f;

		//glm::mat4 terrainMatrix = glm::translate(glm::mat4(), centerPosition);
		//*matrix = terrainMatrix;

		((OpenGL::OpenGLMaterial*)terrainMaterial.ptr())->SetUniformValue<glm::vec3>("centerPosition", centerPosition);
	}

	void Terrain::LateRenderUpdate()
	{

	}

	void Terrain::PhysicsUpdate()
	{

	}

	void Terrain::Initialize(uint outerSize, uint innerSize, uint minQuadSize, uint maxQuadSize, uint startSecondLaneDistance, uint increaseLaneDistance)
	{
		RenderingFactory* renderingFactory = RenderingFactory::Instance();
		terrainMaterial = renderingFactory->LoadMaterial("TERRAIN/terrain");

		std::vector<ModelVertexDataA> data;
		std::vector<uint> indices;

		int innerAmount = innerSize / minQuadSize;
		uint lastMadeAmount = 0;

		uint quadSize = minQuadSize;
		uint terrainSize = 0;
		CreateLane(0.f, 1, (float)quadSize, data, indices, lastMadeAmount);

		int i = 1;
		for (; i < innerAmount; ++i)
			CreateLane((float)i, 1, (float)quadSize, data, indices, lastMadeAmount);

		CreatePatchLane((float)i, 2, (float)quadSize, data, indices, lastMadeAmount);

		terrainSize = i * quadSize;

		int increaseSize = innerAmount / 2 + 1;



		while (terrainSize < outerSize)
		{
			quadSize *= 2;
			bool continueGeneration = false;
			if (quadSize * 2 >= maxQuadSize)
				continueGeneration = true;

			for (i = 0; i < (int)startSecondLaneDistance - 1 || continueGeneration; ++i)
			{
				CreateLane(float(i + increaseSize), 1, (float)quadSize, data, indices, lastMadeAmount);
				terrainSize = (i + increaseSize) * quadSize;
				if (terrainSize > outerSize)
					break;
			}

			if (terrainSize > outerSize)
				break;

			if (terrainSize + quadSize > outerSize)
				CreateLane(float(i + increaseSize), 1, (float)quadSize, data, indices, lastMadeAmount);
			else
			{
				// keep filling until the next quads can lock on
				while (div((int)terrainSize + (int)quadSize, quadSize * 2).rem != 0)
				{
					CreateLane(float(i + increaseSize), 1, (float)quadSize, data, indices, lastMadeAmount);
					terrainSize = (i + increaseSize) * quadSize;
					i++;
				}

				CreatePatchLane(float(i + increaseSize), 2, (float)quadSize, data, indices, lastMadeAmount);
			}

			terrainSize = (i + increaseSize) * quadSize;

			startSecondLaneDistance += increaseLaneDistance;
			increaseSize = terrainSize / (quadSize * 2) + 1;
		}

		mesh = cgc::construct_new<Mesh<ModelVertexDataA>>(data, indices, true);
		renderObject = RenderObject::Construct(mesh, terrainMaterial);
		matrix = new glm::mat4();
		renderObject->SetModelMatrix(GetMatrix());
		//renderObjects.push_back(renderObject);


		// Physics

		// create raw data
		long gridSize = (terrainSize * 2) + 1;
		double d_gridSize = 1.0 / (double)gridSize;

		long bytesPerElement = gridSize * gridSize;

		sf::Image img;
		if (img.loadFromFile("./resources/textures/TERRAIN/heightmap1.tga"))
		{
			terrainData = new float[bytesPerElement];
			assert(terrainData && "out of memory");

			float* cell = terrainData;

			for (double y = 0; y < gridSize; ++y)
			{
				for (double x = 0; x < gridSize; ++x)
				{
					glm::vec2 pixel = glm::vec2(x * d_gridSize * (double)img.getSize().x, y * d_gridSize * (double)img.getSize().y);
					*cell = (double)img.getPixel((uint)pixel.x, (uint)pixel.y).r * (1.0 / 255.0) * 500.0;

					cell++;
				}
			}

			// create shape
			heightFieldShape = new btHeightfieldTerrainShape(gridSize, gridSize, terrainData, 1, -500.f, 500.f, 1, PHY_ScalarType::PHY_FLOAT, false);
			heightFieldShape->setLocalScaling(btVector3(5.5f, 1.f, 5.5f));

			PhysicsFactory::CreateTerrain(physicsObject, heightFieldShape);

			world->Physics().AddPhysicsObject(physicsObject, Collision::StaticFilter, -3);
		}

		//world->AddWorldObject(this);
		//world->AddRenderObject(renderObject);
	}

	void Terrain::CreateLane(float size, int increment, float quadSize, std::vector<ModelVertexDataA>& data, std::vector<uint>& indices, uint& lastMadeAmount)
	{
		bool zTriangle = false;
		bool xTriangle = false;

		int previousVerticesCount = (int)data.size();
		int previousCountCreated = lastMadeAmount;
		lastMadeAmount = 0;

		int width = int(size * 2.f) + 1;

		for (float z = -size; z <= size; z += increment)
		{
			xTriangle = false;
			for (float x = -size; x <= size; x += increment)
			{
				if (abs(z) != size && abs(x) != size)
					continue;

				int vertexIndex = (int)data.size();

				ModelVertexDataA tmp;
				glm::vec3 tmpVec;

				tmpVec.x = x * quadSize;
				tmpVec.y = 0;
				tmpVec.z = z * quadSize;
				tmp.position = tmpVec;

				tmpVec.x = 0;
				tmpVec.y = 1;
				tmpVec.z = 0;
				tmp.normal = tmpVec;

				data.push_back(tmp);

				// indices
				if (vertexIndex != 0 && ((abs(z) != size) != (abs(x) != size)))
				{
					uint index1 = 0;
					uint index2 = 0;
					uint index3 = 0;

					uint index4 = 0;
					uint index5 = 0;
					uint index6 = 0;

					if (z == -size)
					{
						index1 = vertexIndex;
						index2 = xTriangle ? vertexIndex - 1 : vertexIndex - previousCountCreated - 2;
						index3 = vertexIndex - previousCountCreated - 1;

						index4 = xTriangle ? vertexIndex + 1 : vertexIndex - previousCountCreated;
						index5 = vertexIndex;
						index6 = index3;
					}
					else if (z == size)
					{
						index1 = xTriangle ? vertexIndex - 1 : previousVerticesCount - (width - 2) + (int(x + size) - 1) - 1;
						index2 = vertexIndex;
						index3 = previousVerticesCount - (width - 2) + (int(x + size) - 1);

						index4 = vertexIndex;
						index5 = xTriangle ? vertexIndex + 1 : previousVerticesCount - (width - 2) + (int(x + size) - 1) + 1;
						index6 = index3;
					}
					else
					{
						index1 = vertexIndex;
						index2 = vertexIndex - previousCountCreated - width;
						index3 = zTriangle ? vertexIndex - (lastMadeAmount == width ? width : 2) : index2 - (z == -size + 2 && x == -size ? width - 2 : 2);

						index4 = vertexIndex;
						index5 = zTriangle ? vertexIndex + 2 : index2 + (z == size - 2 && x == size ? width - 2 : 2);
						index6 = index2;

						if (lastMadeAmount == width * 3 - 5)
						{
							index2 = previousVerticesCount - 1;
							index5 = zTriangle ? vertexIndex + width : vertexIndex + width;
							index6 = previousVerticesCount - 1;
						}
						else if (lastMadeAmount != width)
						{
							index2 += width - 4;
							index6 += width - 4;
							if (!zTriangle)
							{
								index3 += width - 4;
								index5 += width - 4;
							}
						}

						if (x == size)
						{
							int tempIndex = index1;
							index1 = index2;
							index2 = tempIndex;

							tempIndex = index4;
							index4 = index5;
							index5 = tempIndex;
						}
					}

					indices.push_back(index1);
					indices.push_back(index2);
					indices.push_back(index3);
					indices.push_back(index4);
					indices.push_back(index5);
					indices.push_back(index6);
				}

				xTriangle = !xTriangle;
				++lastMadeAmount;
			}
			zTriangle = !zTriangle;
		}
	}

	void Terrain::CreatePatchLane(float size, int increment, float quadSize, std::vector<ModelVertexDataA>& data, std::vector<uint>& indices, uint& lastMadeAmount)
	{
		int previousVerticesCount = (int)data.size();
		int previousCountCreated = lastMadeAmount;
		lastMadeAmount = 0;

		int width = int(size * 2) + 1;

		for (float z = -size; z <= size; z += increment)
		{
			for (float x = -size; x <= size; x += increment)
			{
				if (abs(z) != size && abs(x) != size)
					continue;

				int vertexIndex = (int)data.size();

				ModelVertexDataA tmp;
				glm::vec3 tmpVec;

				tmpVec.x = x * quadSize;
				tmpVec.y = 0;
				tmpVec.z = z * quadSize;
				tmp.position = tmpVec;

				tmpVec.x = 0;
				tmpVec.y = 1;
				tmpVec.z = 0;
				tmp.normal = tmpVec;

				data.push_back(tmp);

				// indices
				if (vertexIndex != 0)
				{
					uint index1 = 0;
					uint index2 = 0;
					uint index3 = 0;

					uint index4 = 0;
					uint index5 = 0;
					uint index6 = 0;

					uint index7 = 0;
					uint index8 = 0;
					uint index9 = 0;

					if (z == -size && x == size) // top right
					{
						index1 = vertexIndex + 2;
						index2 = vertexIndex;
						index3 = vertexIndex - previousCountCreated + lastMadeAmount - 2;

						index4 = 0;
						index5 = vertexIndex;
						index6 = 0;

						index7 = 0;
						index8 = vertexIndex;
						index9 = 0;
					}
					else if (z == -size) // top
					{
						index1 = vertexIndex + 1;
						index2 = vertexIndex;
						index3 = vertexIndex - previousCountCreated + lastMadeAmount;

						index4 = index1;
						index5 = index3;
						index6 = index3 + 1;

						index7 = index1;
						index8 = index6;
						index9 = index6 + 1;
					}
					else if (z == size && x == -size) // bottom left
					{
						index1 = vertexIndex - 2;
						index2 = vertexIndex;
						index3 = vertexIndex - lastMadeAmount - width + int(x + size) + 2;

						index4 = index1;
						index5 = index3;
						index6 = index3 - 2;

						index7 = index1;
						index8 = index6;
						index9 = index6 - 2;
					}
					else if (z == size) // bottom
					{
						index1 = vertexIndex - 1;
						index2 = vertexIndex;
						index3 = vertexIndex - lastMadeAmount - width + int(x + size);

						index4 = index1;
						index5 = index3;
						index6 = index3 - 1;

						index7 = index1;
						index8 = index6;
						index9 = index6 - 1;
					}
					else if (z == -size + 2 && x == -size) // top left
					{
						index1 = previousVerticesCount;
						index2 = vertexIndex;
						index3 = previousVerticesCount - previousCountCreated;

						index4 = 0;
						index5 = vertexIndex;
						index6 = 0;

						index7 = 0;
						index8 = vertexIndex;
						index9 = 0;
					}
					else if (z == size - 2 && x == size) // bottom right
					{
						index1 = vertexIndex + int(size + 1);
						index2 = vertexIndex;
						index3 = previousVerticesCount - 1;

						index4 = index3 - (width - 2);
						index5 = index3;
						index6 = index2;

						index7 = index6;
						index8 = index3 - width;
						index9 = index4;
					}
					else // middle
					{
						index1 = vertexIndex + (x == -size ? -2 : 0);
						index2 = vertexIndex + (x == -size ? 0 : 2);
						index3 = (previousVerticesCount - previousCountCreated) + (width - 2) + int((z + size) * 2) + (x == -size ? -6 : -1);

						index4 = index1;
						index5 = index3;
						index6 = index3 - 2;

						index7 = index4;
						index8 = index6;
						index9 = z == -(size - 4) && x == -size ? (previousVerticesCount - previousCountCreated) : index6 - 2;

						if (x == size)
						{
							int tempIndex = index1;
							index1 = index2;
							index2 = tempIndex;

							tempIndex = index4;
							index4 = index5;
							index5 = tempIndex;

							tempIndex = index7;
							index7 = index8;
							index8 = tempIndex;
						}
					}

					indices.push_back(index1);
					indices.push_back(index2);
					indices.push_back(index3);


					if (!(z == -size && x >= size - 2) && !(z == size && x == -size + 2))
					{
						indices.push_back(index4);
						indices.push_back(index5);
						indices.push_back(index6);

						indices.push_back(index7);
						indices.push_back(index8);
						indices.push_back(index9);
					}
				}
				++lastMadeAmount;
			}
		}
	}
}