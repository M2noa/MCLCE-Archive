#pragma once

#include "SenClientCultureBackCompat_SenCore.h"
#include "SenClientNews.h"

namespace Sentient
{
__declspec(deprecated("This function is deprecated. See the function body for an example of using the new API."))   
__inline HRESULT SenGetTickerMessage(
	int userIndex, 
	const SenCultureInfo *culture,
	SenTickerData *out_tickerData )
{
	return SenGetTickerMessage(userIndex, out_tickerData);
}
}