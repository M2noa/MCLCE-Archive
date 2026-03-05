#pragma once

#include "SenClientCultureBackCompat_SenCore.h"
#include "SenClientFame.h"
#include "SenClientAvatar.h"

namespace Sentient
{
	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenGetAwardMessage(
		int userIndex, 
		const SenCultureInfo *culture,
		SenAwardMessageData *out_awardData )
	{
		return SenGetAwardMessage(userIndex, out_awardData);
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenGetVIPLevelName(
		const SenCultureInfo *culture,
		unsigned int vipLevel,
		size_t maxNameLength,
		wchar_t *out_name)
	{
		return SenGetVIPLevelName(vipLevel, maxNameLength, out_name);
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenGetFameDisplayData(
		int userIndex, 
		const SenCultureInfo *culture,
		size_t startIndex, 
		size_t maxDisplayDataCount, 
		size_t *out_dataCount, 
		SenFameDisplayData *out_displayData )
	{
		return SenGetFameDisplayData(userIndex, startIndex, maxDisplayDataCount, out_dataCount, out_displayData);
	}


	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenAvatarDownloadExtraInfo(
		const SenAvatarInfo &avatarInfo,
		bool male,
		const SenCultureInfo *culture,
		SenAvatarExtraInfo *out_avatarExtraInfo,
		SenSysCompletedCallback userCallback, void *userCallbackData )
	{
		return SenAvatarDownloadExtraInfo(
			avatarInfo,
			male,
			culture,
			out_avatarExtraInfo,
			userCallback, userCallbackData );
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenAvatarXMLGetTitle(
		const SenXML &senXML,
		const SenCultureInfo *culture, // optional
		size_t bufferLengthMax,
		size_t *out_bufferLength,  // optional
		wchar_t *out_buffer )
	{
		return SenAvatarXMLGetTitle(senXML, bufferLengthMax, out_bufferLength, out_buffer);
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenAvatarXMLGetName(
		const SenXML &senXML,
		const SenCultureInfo *culture,
		size_t bufferLengthMax,
		size_t *out_bufferLength,  // optional
		wchar_t *out_buffer )
	{
		return SenAvatarXMLGetName(senXML, bufferLengthMax, out_bufferLength, out_buffer);
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenAvatarXMLGetDescription(
		const SenXML &senXML,
		const SenCultureInfo *culture, // optional
		size_t bufferLengthMax,
		size_t *out_bufferLength,  // optional
		wchar_t *out_buffer )
	{
		return SenAvatarXMLGetDescription(senXML, bufferLengthMax, out_bufferLength, out_buffer);
	}

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
	__inline HRESULT SenAvatarXMLGetPaletteName(
		const SenXML &senXML,
		int paletteIndex,
		const SenCultureInfo *culture, // optional
		size_t bufferLengthMax,
		size_t *out_bufferLength,  // optional
		wchar_t *out_buffer )
	{
		return SenAvatarXMLGetPaletteName(senXML, paletteIndex, bufferLengthMax, out_bufferLength, out_buffer);
	}
}
