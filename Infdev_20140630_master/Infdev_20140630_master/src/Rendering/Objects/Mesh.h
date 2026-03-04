#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <cppu/cgc/pointers.h>

#include "./AbstractMesh.h"

namespace Esteem
{
	template<class V, class I = uint>
	class Mesh : public AbstractMesh
	{
	public:
		struct Data : IMeshData
		{
			std::vector<V> vertices;
			std::vector<I> indices;

			std::vector<V>& GetVertexData();
			const std::vector<V>& GetVertexData() const;
			std::vector<I>& GetIndexData();
			const std::vector<I>& GetIndexData() const;

			Data(std::vector<V>& vertices, std::vector<I>& indices)
				: vertices(vertices)
				, indices(indices)
			{ }

			Data()
				: vertices()
				, indices()
			{ }

			virtual mm::array_view GetVertexMemInfo() const;
			virtual mm::array_view GetIndexMemInfo() const;

			virtual ~Data() {}
		};

	private:
		cgc::strong_ptr<IVBO> vbo;
		cgc::strong_ptr<IEBO> ebo;

	public:
		Mesh(bool keepOnCPU);
		Mesh(std::vector<V>& vertexData, std::vector<I>& indices, bool keepOnCPU);
		Mesh(const cgc::strong_ptr<Data>& meshData, bool keepOnCPU);

		virtual const cgc::strong_ptr<IVBO>& GetVBO() const;
		virtual void SetVBO(const cgc::strong_ptr<IVBO>& vbo);

		virtual const cgc::strong_ptr<IEBO>& GetEBO() const;
		virtual void SetEBO(const cgc::strong_ptr<IEBO>& ebo);
		
		bool AvailableOnGPU() const;
	};
}

#include "./Mesh.inl"