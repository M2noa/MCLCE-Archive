#pragma once

#include "./IModelLoader.h"

#include "Utils/details/zipfilebuffer.h"

#include "./ModelFactory.h"


namespace Esteem
{
	class NativeModelLoader : public IModelLoader
	{
	private:

		bool LoadModel_0_1_0_0(std::streambuf& streamBuffer, Model& model, const std::string& modelTexturesFolder, Model::ModelGenerateSettings settings);

	public:
		NativeModelLoader();
		//~NativeModelLoader();
		
		virtual void LoadModel(const std::string& path, Model& model, const std::string& modelTexturesFolder, Model::ModelGenerateSettings settings);
		virtual cgc::strong_ptr<AnimationCollection> LoadAnimations(const std::string& path);

		bool SaveModel(std::ostream& streamBuffer, Model& model, const std::string& modelTexturesFolder);
		
		/// \brief used by the Data class to enable factory search, put in own logic in here
		virtual bool EqualsSearch(std::string_view search);
	};
}