#include "Dungeons.h"

#include "util/FilePickerSettings.h"

FilePickerSettings::FilePickerSettings() 
	: mPickerType(PickerType::Invalid)
	, onCancel([](FilePickerSettings&) { })
	, onPick([](FilePickerSettings&, const std::string&) { })
	, mDefaultFileExtensionIndex(-1)
	, onOperationComplete([](bool) { } ) {

}

void FilePickerSettings::addFileDescription(const std::string& extension, const std::string& name, bool isDefaultExtension /*= false*/) {
	mFileDescriptions.push_back({ extension, name });
	if (mDefaultFileExtensionIndex == -1 || isDefaultExtension) {
		mDefaultFileExtensionIndex = mFileDescriptions.size() - 1;
	}
}

FilePickerSettings::PickerType FilePickerSettings::getPickerType() const {
	return mPickerType;
}

void FilePickerSettings::setPickerType(PickerType PickerType) {
	mPickerType = PickerType;
}

const std::vector<FilePickerSettings::FileDescription>& FilePickerSettings::getFileDescriptions() const {
	return mFileDescriptions;
}

const FilePickerSettings::FileDescription& FilePickerSettings::getDefaultFileDescription() const {
	DEBUG_ASSERT(mDefaultFileExtensionIndex != -1, "No default file description provided");
	return mFileDescriptions[mDefaultFileExtensionIndex];
}

void FilePickerSettings::setDefaultFileName(const std::string& fileName) {
	mDefaultFileName = fileName;
}

const std::string& FilePickerSettings::getDefaultFileName() const {
	return mDefaultFileName;
}

std::string FilePickerSettings::getPickerTitle() const {
	return mFilePickerTitle;
}

void FilePickerSettings::setPickerTitle(std::string FilePickerTitle) {
	mFilePickerTitle = FilePickerTitle;
}

