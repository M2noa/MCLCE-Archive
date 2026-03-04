#pragma once

#include <vector>

#include "./AbstractConstituent.h"
#include "Model/Model.h"
#include "Rendering/Objects/RenderObject.h"
#include "Culling/Objects/CullingObject.h"

namespace Esteem
{
	class MeshRenderer : public AbstractConstituent
	{
	private:
		cgc::strong_ptr<const Model> model;
		CullingObject* cullingObject;

		bool enabled : 1;
		bool dynamic : 1;
				
	public:
		static cgc::strong_ptr<MeshRenderer> Instantiate(const cgc::strong_ptr<Entity>& entity,bool dynamic);

		MeshRenderer(const cgc::strong_ptr<Entity>& entity, bool dynamic)
			: AbstractConstituent(entity)
			, dynamic(dynamic)
		{
			Initialize();
		}

		~MeshRenderer();

		void LoadModel(const std::string& path);
		void SetModel(const cgc::strong_ptr<const Model>& model);
		const cgc::strong_ptr<const Model>& GetModel() const { return model; }

		CullingObject* GetCullingObject() { return cullingObject; }
		const CullingObject* GetRenderObjects() const { return cullingObject; }

		void SetCastingShadows(bool enable);

		void Initialize();

		void DirtyCleanUp();

		bool GetEnabled() const { return enabled; }
		void SetEnabled(bool enable) { enabled = enable; }
	};
}
