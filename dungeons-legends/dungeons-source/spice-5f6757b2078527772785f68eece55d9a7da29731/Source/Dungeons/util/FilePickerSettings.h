#pragma once

#include <string>
#include <vector>
#include <functional>

class FilePickerSettings {
public:
	FilePickerSettings();

	struct FileDescription {
		// This should *not* include the dot '.'
		// Example: "mcworld"
		std::string Extension;

		// Name of the file extension, used for labels/dropdowns
		// Example: "Minecraft World File"
		std::string Name;
	};

	enum class PickerType {
		Invalid,

		Open,
		Save
	};

	void addFileDescription(const std::string& extension, const std::string& name, bool isDefaultExtension = false);

	PickerType getPickerType() const;
	void setPickerType(PickerType PickerType);
	
	std::function<void(FilePickerSettings&)> onCancel;
	// Used as a callback to the platform for when your operation is
	//	complete with the file the user have picked
	std::function<void(bool)> onOperationComplete;
	std::function<void(FilePickerSettings&, const std::string&)> onPick;

	const std::vector<FileDescription>& getFileDescriptions() const;
	const FileDescription& getDefaultFileDescription() const;

	void setDefaultFileName(const std::string& fileName);
	const std::string& getDefaultFileName() const;

	std::string getPickerTitle() const;
	void setPickerTitle(std::string FilePickerTitle);
private:
	std::vector<FileDescription> mFileDescriptions;
	size_t mDefaultFileExtensionIndex;

	PickerType mPickerType = PickerType::Invalid;

	// Does not need to contain the extension
	std::string mDefaultFileName;

	// Title of the picker window
	std::string mFilePickerTitle;
};