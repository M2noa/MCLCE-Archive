#include "GraphicalOverlay.h"

#include <glm/glm.hpp>

#include "Math/Math.h"
#include "Utils/Data.h"
#include "Window/View.h"
#include "Rendering/Objects/RenderCamera.h"

#include "World/World.h"

namespace Esteem
{
	GraphicalOverlay::GraphicalOverlay(const std::string& name)
		: name(name)
		, order(0)
		, enabled(true)
		, visible(true)
	{
		std::vector<ModelVertexDataA> meshVertices;
		std::vector<uint> meshIndices;
	}

	void GraphicalOverlay::AddGraphicObject(const cgc::strong_ptr<GraphicalOverlay::OverlayGraphic>& graphic, cgc::raw_ptr<GraphicalOverlay::OverlayBranch> parent)
	{
		if (graphic)
		{
			GraphicalOverlay::OverlayBranch* parentPtr = parent ? static_cast<GraphicalOverlay::OverlayBranch*>(parent) : &root;
			parentPtr->leaves.emplace_back(graphic);

			RebuildGraphicsMesh(graphic);
			if(graphic->renderObject)
				graphic->renderObject->SetModelMatrix(&graphic->matrix);
			RebuildGraphicsRenderList();
		}
		else
			Debug::LogWarning("GraphicalOverlay::AddGraphicObject(...) given graphic is NULL");
	}
	
	void GraphicalOverlay::RemoveGraphicObject(const cgc::strong_ptr<GraphicalOverlay::OverlayGraphic>& graphic, cgc::raw_ptr<GraphicalOverlay::OverlayBranch> parent)
	{
		if (graphic)
		{
			GraphicalOverlay::OverlayBranch* parentPtr = parent ? static_cast<GraphicalOverlay::OverlayBranch*>(parent) : &root;
			auto found = std::find(parentPtr->leaves.begin(), parentPtr->leaves.end(), graphic);
			if (found != parentPtr->leaves.end())
				parentPtr->leaves.erase(found);
			RebuildGraphicsRenderList();
		}
		else
			Debug::LogWarning("GraphicalOverlay::RemoveGraphicObject(...) given graphic is NULL");
	}

	void GraphicalOverlay::RebuildGraphicsMesh(const cgc::strong_ptr<GraphicalOverlay::OverlayGraphic>& graphic)
	{
		// create a new render object for this graphic
		cgc::strong_ptr<Mesh<OverlayVertexData>::Data> meshData = cgc::construct_new<Mesh<OverlayVertexData>::Data>();
		graphic->CalculateMatrix();
		if (graphic->AddToMesh(meshData->vertices, meshData->indices))
		{

			if (!graphic->renderObject)
			{
				cgc::strong_ptr<Mesh<OverlayVertexData>> mesh = cgc::gcnew<Mesh<OverlayVertexData>>(meshData, false);
				graphic->renderObject = RenderObject::Construct(mesh, meshData, graphic->material);
			}
			else
				graphic->renderObject->GetMesh()->UpdateMeshData(graphic->renderObject->GetMesh(), meshData);
		}
	}

	void GraphicalOverlay::RebuildAllMeshes()
	{
		GraphicalOverlay::OverlayBranch* branch = &root;
		std::deque<OverlayBranch*> stack;
		while (true)
		{
			for (std::size_t i = 0; i < branch->leaves.size(); ++i)
			{
				if (branch->leaves[i])
					RebuildGraphicsMesh(branch->leaves[i]);
			}

			for (std::size_t i = 0; i < branch->branches.size(); ++i)
				stack.push_back(branch->branches[i].ptr());

			if (stack.empty())
				break;

			branch = stack.front();
			stack.pop_front();
		}
	}

	void GraphicalOverlay::RebuildGraphicsRenderList()
	{
		// rebuild this overlay's render list
		renderList.clear();
		GraphicalOverlay::OverlayBranch* branch = &root;
		std::deque<OverlayBranch*> stack;
		while (true)
		{
			for (std::size_t i = 0; i < branch->leaves.size(); ++i)
			{
				if(branch->leaves[i]->visible)
					renderList.push_back(branch->leaves[i]->renderObject.ptr());
			}

			for (std::size_t i = 0; i < branch->branches.size(); ++i)
				stack.push_back(branch->branches[i].ptr());

			if (stack.empty())
				break;

			branch = stack.front();
			stack.pop_front();
		}
	}

	GraphicalOverlay::OverlayGraphic::OverlayGraphic(const cgc::strong_ptr<Material>& material)
		: material(material)
		, matrix(1)
		, position(0)
		, rotation()
		, scale(1)
		, visible(true)
		, color(255)
	{
	}

	GraphicalOverlay::OverlayGraphic::~OverlayGraphic()
	{
	}

	bool GraphicalOverlay::OverlayGraphic::AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices)
	{
		return false;
	}

	void GraphicalOverlay::OverlayGraphic::CalculateMatrix()
	{
		matrix = glm::translate(glm::mat4(1.0f), position);
		matrix *= glm::mat4_cast(rotation);
		matrix = glm::scale(matrix, scale);
	}

	GraphicalOverlay::OverlayQuad::OverlayQuad(const cgc::strong_ptr<Material>& material)
		: GraphicalOverlay::OverlayGraphic(material)
		, uvOffset(0)
		, uvSize(1)
	{

	}

	bool GraphicalOverlay::OverlayQuad::AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices)
	{
		vertices.insert(vertices.end(), {
			{ matrix * glm::vec4(0, 0, 0, 1), color, glm::vec3(1), uvOffset },
			{ matrix * glm::vec4(0, 1, 0, 1), color, glm::vec3(1), glm::vec2(uvOffset.x, uvSize.y) },
			{ matrix * glm::vec4(1, 1, 0, 1), color, glm::vec3(1), uvSize },
			{ matrix * glm::vec4(1, 0, 0, 1), color, glm::vec3(1), glm::vec2(uvSize.x, uvOffset.y) } });

		indices.insert(indices.end(), { 0, 1, 2, 0, 2, 3 });

		return true;
	}
}