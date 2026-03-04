#include "RenderCamera.h"
#include "Utils/Data.h"
#include "Rendering/Objects/GraphicalOverlay.h"

namespace Esteem
{
	void RenderCamera::AddGraphicalOverlay(cgc::raw_ptr<GraphicalOverlay> overlay)
	{
		auto iterator = std::upper_bound(graphicalOverlays.begin(), graphicalOverlays.end(), overlay,
			[](const GraphicalOverlay* newOverlay, const GraphicalOverlay* elem)
			{ return newOverlay->GetOrder() < elem->GetOrder(); });

		graphicalOverlays.insert(iterator, overlay);
	}

	void RenderCamera::RemoveGraphicalOverlay(cgc::raw_ptr<GraphicalOverlay> overlay)
	{
		(void)std::remove(graphicalOverlays.begin(), graphicalOverlays.end(), overlay);
	}

	Rml::ElementDocument* RenderCamera::LoadHTMLDocument(const std::string& path)
	{
		if (Data::AssetExists(path))
			return guiContext->LoadDocumentFromMemory(Data::ReadAsset(path), path);

		return nullptr;
	}

	void RenderCamera::RemoveHTMLDocument(Rml::ElementDocument* document)
	{
		guiContext->UnloadDocument(document);
	}
}