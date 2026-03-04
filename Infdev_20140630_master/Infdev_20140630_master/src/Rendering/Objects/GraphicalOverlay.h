#pragma once

#include <vector>
#include <cppu/cgc/pointers.h>

#include "Rendering/Objects/Mesh.h"
#include "Rendering/Objects/VertexData/OverlayVertexData.h"
#include "Rendering/Objects/RenderObject.h"

namespace Esteem
{
	class RenderingFactory;
	class RenderCamera;
	class Material;
	class World;

	class GraphicalOverlay
	{
	friend class cgc::constructor;
	template<class T, class... Args> friend cgc::strong_ptr<T> cgc::construct_new(Args&& ...);
	public:
		enum class Projection
		{
			NONE = 0,
			ORTHOGRAPHIC = 1,
			PERSPECTIVE = 2,
		};

		class OverlayGraphic
		{
		public:
			cgc::strong_ptr<RenderObject> renderObject;
			cgc::strong_ptr<Material> material;
			
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;

			bool visible;
			glm::mat4 matrix;
			Color8u color;

			OverlayGraphic(const cgc::strong_ptr<Material>& material);
			~OverlayGraphic();

			void CalculateMatrix();
			
			inline Color8u GetColor() const { return color; }
			inline void SetColor(Color8u color) { this->color = color; }
			
			virtual bool AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices);
		};

		class OverlayQuad : public OverlayGraphic
		{
		public:
			glm::vec2 uvOffset;
			glm::vec2 uvSize;
			
			OverlayQuad(const cgc::strong_ptr<Material>& material);

			bool AddToMesh(std::vector<OverlayVertexData>& vertices, std::vector<uint>& indices) override;
		};

		class OverlayBranch
		{
		public:
			std::vector<cgc::strong_ptr<OverlayBranch>> branches;
			std::vector<cgc::strong_ptr<OverlayGraphic>> leaves;
		};

	private:
		bool enabled;
		bool visible;
		std::string name;
		int order;

		OverlayBranch root;
		cgc::strong_ptr<RenderCamera> renderCamera;

		std::vector<RenderObject*> renderList;

		GraphicalOverlay(const std::string& name);
		
	public:

		void AddGraphicObject(const cgc::strong_ptr<OverlayGraphic>& graphic, cgc::raw_ptr<OverlayBranch> parent = nullptr);
		void RemoveGraphicObject(const cgc::strong_ptr<OverlayGraphic>& graphic, cgc::raw_ptr<OverlayBranch> parent = nullptr);
		
		void RebuildAllMeshes();
		void RebuildGraphicsMesh(const cgc::strong_ptr<OverlayGraphic>& graphic);
		void RebuildGraphicsRenderList();

		inline void SetRenderCamera(const cgc::strong_ptr<RenderCamera>& renderCamera) { this->renderCamera = renderCamera; }
		inline const cgc::strong_ptr<RenderCamera>& GetRenderCamera() { return renderCamera; }

		inline bool IsEnabled() const { return enabled; }
		inline void SetEnabled(bool enable) { this->enabled = enable; }

		inline bool IsVisible() const { return visible; }
		inline void SetVisible(bool visible) { this->visible = visible; }

		inline int GetOrder() const { return order; }
		inline void SetOrder(int order) { this->order = order; }
		
		inline const std::vector<RenderObject*>& GetRenderList() const { return renderList; }

	};
}
