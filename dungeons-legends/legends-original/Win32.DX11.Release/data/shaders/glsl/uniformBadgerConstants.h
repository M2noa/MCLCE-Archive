#ifndef _UNIFORM_BADGER_CONSTANTS_H
#define _UNIFORM_BADGER_CONSTANTS_H

#include "shaders/uniformMacro.h"

#ifdef MCPE_PLATFORM_NX
#extension GL_ARB_enhanced_layouts : enable
uniform BadgerConstants {
#endif
UNIFORM int NUM_STRUCTURES;
UNIFORM vec4 STRUCTURE_DATA[8];
UNIFORM vec4 STRUCTURE_COLOR[8];
END_UNIFORM_BLOCK

#endif
