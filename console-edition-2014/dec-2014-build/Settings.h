#pragma once
class File;
using namespace std;

class Settings
{
//    public static Logger logger = Logger.getLogger("Minecraft");
//    private Properties properties = new Properties();
private:
	unordered_map<wstring,wstring> properties;	// 4J - TODO was Properties type, will need to implement something we can serialise/deserialise too
	//File *file;

public:
	Settings(File *file);
    void generateNewProperties();
    void saveProperties();
    wstring getString(const wstring& key, const wstring& defaultValue);
    int getInt(const wstring& key, int defaultValue);
    bool getBoolean(const wstring& key, bool defaultValue);
    void setBooleanAndSave(const wstring& key, bool value);
};
