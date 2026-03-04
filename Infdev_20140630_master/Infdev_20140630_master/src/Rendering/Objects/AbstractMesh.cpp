#include "AbstractMesh.h"
#include "Rendering/RenderingFactory.h"

namespace Esteem
{

	void AbstractMesh::UnloadFromCPU()
	{
		meshData = cgc::strong_ptr<IMeshData>();
	}

	bool AbstractMesh::AvailableOnCPU() const
	{
		return (bool)meshData;
	}

	void AbstractMesh::SetMeshData(const cgc::strong_ptr<IMeshData>& meshData)
	{
		this->meshData = meshData;
		this->meshData->version = ++version;
	}

	void AbstractMesh::UpdateMeshData(const cgc::strong_ptr<AbstractMesh>& mesh, const cgc::strong_ptr<IMeshData>& meshData)
	{
		mesh->SetMeshData(meshData);
		RenderingFactory::Instance()->UpdateMesh(mesh, meshData);
	}
}