#pragma once

#include "SenClientBoxArt.h"
#include "SenClientCultureBackCompat_SenCore.h"

namespace Sentient
{
#pragma warning(disable:4996)

	__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API.")) 
	__inline HRESULT SenBoxArtDownloadExtraInfo(
		const SenBoxArtInfo &boxArtInfo,
		const SenCultureInfo *culture,
		SenBoxArtExtraInfo *out_boxArtExtraInfo,
		SenHandle *out_senHandle,
		SenSysCompletedCallback userCallback, void *userCallbackData )
	{
		return SenBoxArtDownloadExtraInfo(boxArtInfo, out_boxArtExtraInfo, out_senHandle, userCallback, userCallbackData);
	}

	__declspec(deprecated("This function is deprecated, use SenBoxArtDownloadExtraInfo instead"))  
	__inline HRESULT SenBoxArtXMLGetTitle(
		const SenXML &senXML,
		const SenCultureInfo *culture,
		size_t bufferLengthMax,
		size_t *out_bufferLength,
		wchar_t *out_buffer )
	{
		return SenBoxArtXMLGetTitle(senXML, bufferLengthMax, out_bufferLength, out_buffer);
	}

	__declspec(deprecated("This function is deprecated, use SenBoxArtDownloadExtraInfo instead"))  
	__inline HRESULT SenBoxArtXMLGetDescription(
		const SenXML &senXML,
		const SenCultureInfo *culture,
		size_t bufferLengthMax,
		size_t *out_bufferLength,
		wchar_t *out_buffer )
	{
		return SenBoxArtXMLGetDescription(senXML, bufferLengthMax, out_bufferLength, out_buffer);
	}

#pragma warning(default:4996)
}