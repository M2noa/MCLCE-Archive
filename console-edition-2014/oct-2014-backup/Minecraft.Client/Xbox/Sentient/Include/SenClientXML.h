/********************************************************
*                                                       *
* Copyright (C) Microsoft. All rights reserved.         *
*                                                       *
********************************************************/

// Sentient Client XML API
//
// Include this to get access to all XML Sentient features.

#pragma once

#include "SenClientTypes.h"


namespace Sentient
{
	//======================//
	//                      //
	//    XML Data Types    //
	//                      //
	//======================//

	/// @brief           An easy-to-access pre-parsed XML.
	///
	/// @details         This holds some easily-traversible, easily-searchible representations
	///                  of the content of a raw UTF-8 XML buffer.
	///
	/// @related         SenXMLParse()
	/// @related         SenXMLDestroy()
	///
	struct SenXML
	{
		void *reserved;
	};


	//=====================//
	//                     //
	//    XML Functions    //
	//                     //
	//=====================//

	/// @brief           Parse a raw UTF-8 XML buffer into a more useful format.
	///
	/// @param[in]       source
	///                  The raw UTF-8 XML buffer to be parsed.
	///
	/// @param[out]      out_senXML
	///                  The (private) object to hold the parsed data.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  E_POINTER: Bad pointer to the raw XML buffer.
	///                  E_OUTOFMEMORY: Couldn't parse due to lack of memory.
	///                  E_FAIL: Failed to parse for unspecified reason.
	///                  S_OK: Parsed successfully.
	///
	/// @details         It is expensive to parse an XML file directly for each attribute fetch.
	///                  Thus, we pre-parse the whole thing once after (down)loading it and then
	///                  pass the parsed data to the fetch routines.
	///
	/// @related         SenXMLDestroy()
	///
	HRESULT SenXMLParse( 
		const char *source, 
		SenXML *out_senXML );

	/// @brief           Free the resources/memory associated with a parsed XML file.
	///
	/// @param[in]       senXML
	///                  The (private) object holding the parsed data to be freed.
	///
	/// @return          Check SUCCEEDED( hresult ) or FAILED( hresult ) to determine success.  Specific values include:
	///                  E_INVALIDARG: senXML is not a parsed XML.
	///                  S_OK: Destroyed/freed successfully.
	///
	/// @details         A parsed XML takes up some extra memory.  Once done with its contents,
	///                  you should call this routine to free it up.
	///
	/// @related         SenXMLParse()
	///
	HRESULT SenXMLDestroy( 
		SenXML &senXML );

} // namespace Sentient
