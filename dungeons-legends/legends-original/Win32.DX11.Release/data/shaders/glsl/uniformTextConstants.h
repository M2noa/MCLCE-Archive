#ifndef _UNIFORM_TEXT_CONSTANTS_H
#define _UNIFORM_TEXT_CONSTANTS_H

#include "shaders/uniformMacro.h"

#ifdef MCPE_PLATFORM_NX
uniform TextConstants {
#endif
// BEGIN_UNIFORM_BLOCK(TextConstants) - unfortunately this macro does not work on old Amazon platforms so using above 3 lines instead
UNIFORM float GLYPH_SMOOTH_RADIUS;
UNIFORM float GLYPH_CUTOFF;
UNIFORM float OUTLINE_CUTOFF;
UNIFORM vec4 OUTLINE_COLOR;
UNIFORM float SHADOW_SMOOTH_RADIUS;
UNIFORM vec4 SHADOW_COLOR;
UNIFORM vec2 SHADOW_OFFSET;
END_UNIFORM_BLOCK

#endif
