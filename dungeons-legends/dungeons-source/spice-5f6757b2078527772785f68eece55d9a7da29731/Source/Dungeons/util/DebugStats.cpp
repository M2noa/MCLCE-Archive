#include "Dungeons.h"

#include "util/DebugStats.h"
//#include "Renderer/RendererSettings.h"
//#include "platform/AppPlatform.h"

#if defined(DEBUG_STATS) 
#if !defined(MCPE_HEADLESS)
#include "Renderer/HAL/Interface/RenderDevice.h"
#endif

template<typename T>
static int64_t bytesToMegabytes(const T used) {
	return used / (T)1024 / (T)1024;
}

DebugStats DebugStats::instance;

void DebugStats::nextAVCStat(){
	mAVC = (AVCStats)(((int)mAVC + 1) % (int)AVCStats::_Count);
}

std::string DebugStats::getOSString() {
	auto& hardwareInfo = AppPlatform::singleton().getHardwareInformation();
	return hardwareInfo.mOSVersion;
}

std::string DebugStats::getSystemMemString() {
	return Util::format("%03d", bytesToMegabytes(AppPlatform::singleton().getTotalMemory()));
}

std::string DebugStats::getCPUString() {
	auto& hardwareInfo = AppPlatform::singleton().getHardwareInformation();
	return Util::format("%s (%s)", hardwareInfo.mCPUName.c_str(), hardwareInfo.mCPUType.c_str());
}

std::string DebugStats::getResolutionString() {
	return Util::format("%dx%d", mDisplayWidth, mDisplayHeight);
}

std::string DebugStats::getGPUString() {
#if !defined(MCPE_HEADLESS)
	const mce::AdapterDescription& adapterDescription = mce::RenderDevice::getInstance().getDeviceInformation().getAdapterDescription();
	return adapterDescription.mDescription;
#else
	return "";
#endif
}

std::string DebugStats::getMSAALevelString() {
#if !defined(MCPE_HEADLESS)
	return Util::format("%d", mce::RendererSettings::getInstance().mBackBufferSampleDescription.count);
#else
	return "";
#endif
}

std::string DebugStats::getGPUMemoryString() {
#if !defined(MCPE_HEADLESS)
	const mce::AdapterDescription& adapterDescription = mce::RenderDevice::getInstance().getDeviceInformation().getAdapterDescription();
	uint64_t gpuMB = bytesToMegabytes(adapterDescription.mDedicatedVideoMemory);
	return Util::format("%u", gpuMB);
#else
	return "";
#endif
}

void DebugStats::fillSystemInfo(std::vector<std::string> &outputInfo) {

	// Note: All memory values are virtual ram values
	const int64_t availableMemory = AppPlatform::singleton().getAvailableMemory();
	const int64_t totalMemory = AppPlatform::singleton().getTotalMemory();
	const int64_t usedMemory = totalMemory - availableMemory;

	outputInfo.push_back(Util::format("OS: %s", getOSString().c_str()));
	outputInfo.push_back(Util::format("Mem: %2d%% %03d/%03dMB", static_cast<int>(usedMemory * 100 / totalMemory), static_cast<int>(bytesToMegabytes(usedMemory)), static_cast<int>(bytesToMegabytes(totalMemory))));
	outputInfo.push_back(Util::format("CPU: %s", getCPUString().c_str()));
	outputInfo.push_back("");
	outputInfo.push_back(Util::format("Display: %s", getResolutionString().c_str()));
	outputInfo.push_back(Util::format("GPU: %s", getGPUString().c_str()));
	outputInfo.push_back(Util::format("GPU Dedicated Memory: %u", getGPUMemoryString().c_str()));
	outputInfo.push_back(Util::format("MSAA: %s", getMSAALevelString().c_str()));
	outputInfo.push_back(Util::format("Render Distance: %d", mViewDistance));
	/* Graphics Debug Info - Not implemented yet
	Util::format("Display: %dx%d (%s)",
	hardwareInfo.mScreenWidth,
	hardwareInfo.mScreenHeight,
	hardwareInfo.mGraphicsVendor.c_str(),
	hardwareInfo.mGraphicsRenderer.c_str(),
	hardwareInfo.mGraphicsVersion.c_str())
	*/
}

#endif
