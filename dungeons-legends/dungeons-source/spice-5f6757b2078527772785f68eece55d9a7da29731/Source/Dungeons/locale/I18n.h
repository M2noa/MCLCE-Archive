/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

//#include "Core/Utility/optional_ref.h"
//
//class Localization;
//class ResourcePackManager;

class I18n {
public:

	//static void loadLanguages(ResourcePackManager& resourcePackManager);
	//static void chooseLanguage(const std::string& code);
	//static void chooseLanguage(const Localization& chosen);

	static std::string get(const std::string& id, const std::vector<std::string>& params);
	static std::string get(const std::string& langString);

//	static const std::vector<std::unique_ptr<Localization>>& getSupportedLocales();
//	static const Localization& getLocaleFor(const std::string& code);
//	static optional_ref<const Localization> getCurrentLanguage();
//
//	static bool languageSupportsHypenSplitting();
//
//private:
//
//	static void _initFormatDictionary();
//	static void _addFormatToDictionary(const std::string& key, const std::vector<std::string>& params);
//	static void _getFormatParmas(const std::string& key, std::vector<std::string>& params);
//	static bool _getKeyExists(const std::string& key);
//
//
//	static void _setLanguageSupportsHypenSplitting(const Localization& localization);
//
//	static const Localization* mCurrentLanguage;
//	static std::vector<std::unique_ptr<Localization>> mLanguages;
//
//	static std::unordered_map<std::string, std::vector<std::string>> mFormatDictionary;
//
//	static bool mLanguageSupportsHypenSplitting;
};
