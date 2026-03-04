/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client News API
//
// Include this to get access to all News-related Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{

	// Maximum number of items that will be returned from the service
	static const size_t MAX_NEWS_ITEM_COUNT = 20;

	// Default news cache expire time
	//  News cache expire time controls the duration between service synchronization.
	//  This duration is measured in seconds.  Default value is 6 minutes
	static const int CACHE_EXPIRE = 5 * 60 * 1000;

	// Extern linkage for News Item Count
	extern size_t newsItemCount;
	class NewsItemCountClass
	{
		public: NewsItemCountClass() { newsItemCount = MAX_NEWS_ITEM_COUNT; };
	};
	static NewsItemCountClass newsItemCountClass;

	// Extern linkage for cache expire
	extern int cacheExpire;
	class CacheExpireTimeoutClass
	{
		public: CacheExpireTimeoutClass() { cacheExpire = CACHE_EXPIRE; };
	};
	static CacheExpireTimeoutClass cacheExpireTimeoutClass;

	// Default characters per second
	//  Used for displaying local news messages through the PostLocalNews API
	//  Used for displaying default news messages through the SetDefaultNewsStrings API
	static const float DEFAULT_CHARS_PER_SEC = 2.0f;

	// Max size for news ticker messages
	static const size_t MAX_NEWS_TICKER_MESSAGE_SIZE = 256;

	/**************************
	 ***** News Types *****
	 **************************/

	enum SenTickerMessageType
	{
		SenTickerMessageType_Text = 0,
		SenTickerMessageType_MarketplaceOffer,
		SenTickerMessageType_Max
	};

	struct SenTickerData
	{
		wchar_t					message[MAX_NEWS_TICKER_MESSAGE_SIZE];
		SenTickerMessageType	messageType;
		INT64					data1;
		INT64					data2;
		float					charPerSec;
	};

	/**************************
	 ***** News Functions *****
	 **************************/

	// Ticker messages need to be pulled whenever the ticker is displayed. Call this function to get the next message to display

	/// @brief			public 
	///
	/// @param[in\out]	userIndex
	/// @param[in\out]	out_tickerData
	///
	/// @return			HRESULT
	///
	/// @details		<Insert detailed method documentation>
	///
	/// @related		<Related API>
	HRESULT SenGetTickerMessage(
		int userIndex, 
		SenTickerData *out_tickerData );

} // namespace Sentient