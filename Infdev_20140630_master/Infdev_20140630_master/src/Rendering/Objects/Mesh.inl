#pragma once

#include "./Mesh.h"
#include <cppu/cgc/constructor.h>

namespace Esteem
{
	template<class V, class I>
	Mesh<V, I>::Mesh(bool keepOnCPU)
		: AbstractMesh(keepOnCPU)
	{

	}

	template<class V, class I>
	Mesh<V, I>::Mesh(std::vector<V>& vertices, std::vector<I>& indices, bool keepOnCPU)
		: AbstractMesh(keepOnCPU, cgc::construct_new<Mesh<V, I>::Data>(vertices, indices))
	{ }

	template<class V, class I>
	Mesh<V, I>::Mesh(const cgc::strong_ptr<Data>& meshData, bool keepOnCPU)
		: AbstractMesh(keepOnCPU, meshData)
	{ }

	template<class V, class I>
	inline const cgc::strong_ptr<IVBO>& Mesh<V, I>::GetVBO() const
	{
		return vbo;
	};

	template<class V, class I>
	inline void Mesh<V, I>::SetVBO(const cgc::strong_ptr<IVBO>& vbo)
	{
		this->vbo = vbo;

		if(this->vbo)
			V::SetAttributes(this->vbo.ptr());
	};

	template<class V, class I>
	inline const cgc::strong_ptr<IEBO>& Mesh<V, I>::GetEBO() const
	{
		return ebo;
	};

	template<class V, class I>
	inline void Mesh<V, I>::SetEBO(const cgc::strong_ptr<IEBO>& ebo)
	{
		this->ebo = ebo;
	};

	template<class V, class I>
	inline std::vector<V>& Mesh<V, I>::Data::GetVertexData()
	{
		return vertices;
	};

	template<class V, class I>
	inline const std::vector<V>& Mesh<V, I>::Data::GetVertexData() const
	{
		return vertices;
	};

	template<class V, class I>
	inline std::vector<I>& Mesh<V, I>::Data::GetIndexData()
	{
		return indices;
	};

	template<class V, class I>
	inline const std::vector<I>& Mesh<V, I>::Data::GetIndexData() const
	{
		return indices;
	};

	template<class V, class I>
	mm::array_view Mesh<V, I>::Data::GetVertexMemInfo() const
	{
		return mm::array_view(vertices);
	}

	template<class V, class I>
	mm::array_view Mesh<V, I>::Data::GetIndexMemInfo() const
	{
		return mm::array_view(indices);
	}
	
	template<class V, class I>
	inline bool Mesh<V, I>::AvailableOnGPU() const
	{
		return vbo;
	}
}