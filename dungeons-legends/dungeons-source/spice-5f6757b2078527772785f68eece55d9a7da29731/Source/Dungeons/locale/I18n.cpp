/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "locale/I18n.h"
//#include "locale/Localization.h"
//
//#include "platform/AppPlatform.h"
//#include "Core/Resource/ResourceHelper.h"
//#include "Core/Resource/ResourceUtil.h"
//#include "resources/ResourcePackManager.h"

//const Localization* I18n::mCurrentLanguage = nullptr;
//std::vector<std::unique_ptr<Localization>> I18n::mLanguages;
//
//std::unordered_map<std::string, std::vector<std::string>> I18n::mFormatDictionary;
//
//bool I18n::mLanguageSupportsHypenSplitting = true;
//
//void I18n::loadLanguages(ResourcePackManager& resourcePackManager) {
//
//	//locales are explicitly ordered because when choosing one based on the language
//	//the first (most likely) one must be choosen when a language has more than one locale
//	Json::Value root;
//	Json::Reader reader;
//
//	//this is using a vector because we need to make sure that the order is preserved. New languages must be appended at the bottom so the first one remains the first
//	std::vector<std::string> languageCodes;
//
//	auto versions = resourcePackManager.loadAllVersionsOf(ResourceLocation("texts/languages.json"));
//
//	//find all supported language codes in any pack
//	for (auto& version : versions) {
//		if (reader.parse(version, root)) {
//			for (auto& element : root) {
//				auto code = element.asString();
//
//				//make sure each language is only added once
//				if (std::find(languageCodes.begin(), languageCodes.end(), code) == languageCodes.end()) {
//					languageCodes.push_back(std::move(code));
//				}
//			}
//		}
//	}
//
//	for (auto&& code : languageCodes) {
//		mLanguages.emplace_back(std::make_unique<Localization>(code, resourcePackManager));
//	}
//
//	_initFormatDictionary();
//}
//
//void I18n::chooseLanguage(const Localization& chosen) {
//	mCurrentLanguage = &chosen;
//	_setLanguageSupportsHypenSplitting(chosen);
//}
//
//void I18n::chooseLanguage(const std::string& code) {
//	chooseLanguage(getLocaleFor(code));
//}
//
//const std::vector<std::unique_ptr<Localization>>& I18n::getSupportedLocales() {
//	return mLanguages;
//}
//
//const Localization& I18n::getLocaleFor(const std::string& code) {
//
//	//check if there is a perfect match
//	for (auto& lang : mLanguages) {
//		if (lang->getFullLanguageCode() == code) {
//			return *lang;
//		}
//	}
//
//	//try to match only the language
//	auto languageCode = Localization::getLanguageCode(code);
//
//	for (auto& lang : mLanguages) {
//		if (lang->getLanguageCode() == languageCode) {
//			return *lang;
//		}
//	}
//
//	//no luck, use the default locale
//	return *mLanguages[0];
//}

std::string I18n::get(const std::string& id, const std::vector<std::string>& params) {
	//std::string s;
	//return mCurrentLanguage ? (mCurrentLanguage->get(id, s, params) ? s : id) : id;

	return id;
}

std::string I18n::get(const std::string& langString) {
	//look for format strings
	std::vector<std::string> params;

	//_getFormatParmas(langString, params);

	return get(langString, params);
}

//optional_ref<const Localization> I18n::getCurrentLanguage() {
//	if (mCurrentLanguage) {
//		return *mCurrentLanguage;
//	}
//	return{};
//}
//
//bool I18n::languageSupportsHypenSplitting() {
//	return mLanguageSupportsHypenSplitting;
//}
//
//void I18n::_initFormatDictionary() {
//	//used to specify the params for a lang string
//	//add strings to the params to use as the format when localizing the strings
//
//	//std::vector<std::string> params;
//	//
//	//params.emplace_back("\n\n");
//	//_addFormatToDictionary("createWorldScreen.gameMode.desc", params);
//	//params.clear();
//}
//
//void I18n::_addFormatToDictionary(const std::string& key, const std::vector<std::string>& params) {
//	DEBUG_ASSERT(_getKeyExists(key), "String does not exist in lang file");
//	mFormatDictionary[key] = params;
//}
//
//void I18n::_getFormatParmas(const std::string& key, std::vector<std::string>& parmas) {
//	auto it = mFormatDictionary.find(key);
//	if (it != mFormatDictionary.end()) {
//		parmas = it->second;
//	}
//}
//
//bool I18n::_getKeyExists(const std::string& key) {
//	return mLanguages[0]->getStringIdExists(key);
//}
//
//void I18n::_setLanguageSupportsHypenSplitting(const Localization& localization) {
//	const std::string languageCode = localization.getFullLanguageCode();
//
//	if (languageCode == "ja_JP" || languageCode == "ko_KR" || languageCode == "zh_CN" || languageCode == "zh_TW") {
//		mLanguageSupportsHypenSplitting = false;
//	}
//	else {
//		mLanguageSupportsHypenSplitting = true;
//	}
//}