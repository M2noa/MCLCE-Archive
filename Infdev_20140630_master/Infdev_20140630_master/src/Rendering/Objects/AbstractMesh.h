#pragma once

#include <cppu/cgc/pointers.h>

#include "Rendering/Objects/IVBO.h"
#include "Rendering/Objects/IEBO.h"
#include "Memory/array_view.h"

namespace Esteem
{
	struct IMeshData
	{
		uint8 version;

		IMeshData()
			: version(0)
		{ }

		virtual mm::array_view GetVertexMemInfo() const = 0;
		virtual mm::array_view GetIndexMemInfo() const = 0;

		virtual ~IMeshData() { };
	};
	
	class AbstractMesh
	{
	private:
		bool keepOnCPU;
		cgc::strong_ptr<IMeshData> meshData;
		
	public:
		uint8 version;

		AbstractMesh(bool keepOnCPU)
			: keepOnCPU(keepOnCPU)
			, version(0)
		{}

		AbstractMesh(bool keepOnCPU, const cgc::strong_ptr<IMeshData>& meshData)
			: keepOnCPU(keepOnCPU)
			, meshData(meshData)
			, version(0)
		{}

		virtual const cgc::strong_ptr<IVBO>& GetVBO() const = 0;
		virtual void SetVBO(const cgc::strong_ptr<IVBO>& vbo) = 0;

		virtual const cgc::strong_ptr<IEBO>& GetEBO() const = 0;
		virtual void SetEBO(const cgc::strong_ptr<IEBO>& ebo) = 0;

		inline void KeepOnCPU(bool keepOnCPU) { this->keepOnCPU = keepOnCPU; };
		inline bool KeepOnCPU() { return keepOnCPU; };
		
		const cgc::strong_ptr<IMeshData>& GetMeshData() const { return  meshData; }
		void SetMeshData(const cgc::strong_ptr<IMeshData>& meshData);

		bool AvailableOnCPU() const;
		void UnloadFromCPU();

		static void UpdateMeshData(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData);
	};
}