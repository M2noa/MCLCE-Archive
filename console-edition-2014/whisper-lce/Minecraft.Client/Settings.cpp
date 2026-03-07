#include "stdafx.h"
#include "Settings.h"
#include "..\Minecraft.World\File.h"
#include "..\Minecraft.World\StringHelpers.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>

namespace
{
	string toAscii(const wstring& value)
	{
		string output;
		output.reserve(value.size());
		for (size_t i = 0; i < value.size(); ++i)
		{
			const wchar_t c = value[i];
			output.push_back((c >= 0 && c <= 127) ? (char)c : '?');
		}
		return output;
	}
}

Settings::Settings(File *file)
{
	this->file = file;
	loadProperties();
}

Settings::~Settings()
{
	delete file;
	file = NULL;
}

void Settings::loadProperties()
{
	if (file == NULL)
	{
		return;
	}

	properties.clear();

	if (!file->exists())
	{
		generateNewProperties();
		return;
	}

	const string filePath = toAscii(file->getPath());
	ifstream in(filePath.c_str(), ios::in);
	if (!in.is_open())
	{
		return;
	}

	string line;
	while (getline(in, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
		{
			line.erase(line.size() - 1);
		}

		wstring lineWide = trimString(convStringToWstring(line));
		if (lineWide.empty() || lineWide[0] == L'#' || lineWide[0] == L'!')
		{
			continue;
		}

		size_t splitPos = lineWide.find_first_of(L"=:");
		wstring key;
		wstring value;
		if (splitPos == wstring::npos)
		{
			key = trimString(lineWide);
			value = L"";
		}
		else
		{
			key = trimString(lineWide.substr(0, splitPos));
			value = trimString(lineWide.substr(splitPos + 1));
		}

		if (!key.empty())
		{
			properties[key] = value;
		}
	}
}

bool Settings::parseBoolValue(const wstring& value, bool defaultValue)
{
	const wstring lower = toLower(trimString(value));
	if (lower == L"true" || lower == L"1" || lower == L"yes" || lower == L"on")
	{
		return true;
	}
	if (lower == L"false" || lower == L"0" || lower == L"no" || lower == L"off")
	{
		return false;
	}
	return defaultValue;
}

void Settings::generateNewProperties()
{
	saveProperties();
}

void Settings::saveProperties()
{
	if (file == NULL)
	{
		return;
	}

	const string filePath = toAscii(file->getPath());
	ofstream out(filePath.c_str(), ios::out | ios::trunc);
	if (!out.is_open())
	{
		return;
	}

	vector<wstring> keys;
	keys.reserve(properties.size());
	for (AUTO_VAR(it, properties.begin()); it != properties.end(); ++it)
	{
		keys.push_back(it->first);
	}
	sort(keys.begin(), keys.end());

	for (size_t i = 0; i < keys.size(); ++i)
	{
		const wstring& key = keys[i];
		out << toAscii(key) << "=" << toAscii(properties[key]) << "\n";
	}
}

wstring Settings::getString(const wstring& key, const wstring& defaultValue)
{
	if(properties.find(key) == properties.end())
	{
		properties[key] = defaultValue;
		saveProperties();
	}
	return properties[key];
}

int Settings::getInt(const wstring& key, int defaultValue)
{
	if(properties.find(key) == properties.end())
	{
		properties[key] = _toString<int>(defaultValue);
		saveProperties();
	}

	const wstring value = trimString(properties[key]);
	wistringstream stream(value);
	int parsed = defaultValue;
	stream >> parsed;
	if (stream.fail() || !stream.eof())
	{
		properties[key] = _toString<int>(defaultValue);
		saveProperties();
		return defaultValue;
	}

	return parsed;
}

bool Settings::getBoolean(const wstring& key, bool defaultValue)
{
	if(properties.find(key) == properties.end())
	{
		properties[key] = defaultValue ? L"true" : L"false";
		saveProperties();
	}
	MemSect(35);
	bool retval = parseBoolValue(properties[key], defaultValue);
	MemSect(0);
	return retval;
}

void Settings::setBooleanAndSave(const wstring& key, bool value)
{
	properties[key] = value ? L"true" : L"false";
	saveProperties();
}
