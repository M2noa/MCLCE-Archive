#pragma once

namespace mce {
	// TODO: this was in common_header.h, need to remove the dup or remove this
	typedef uint16_t VertexUVType;
}

//Types defined globally
typedef uint8_t byte;

#ifdef PUBLISH
#define GSL_UNENFORCED_ON_CONTRACT_VIOLATION
#endif

#include <gsl/string_span>

//define our own string span that is immutable
using string_span = gsl::basic_string_span<const char>;

#include "Core/Utility/string_span_operators.h"

