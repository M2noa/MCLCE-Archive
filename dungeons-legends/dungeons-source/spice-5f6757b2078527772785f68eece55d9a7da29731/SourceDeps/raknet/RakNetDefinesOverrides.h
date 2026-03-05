/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// USER EDITABLE FILE

#define RAKNET_SUPPORT_IPV6 1

// Redefine RAKNET DEBUG PRINTING to use a callback that can be set by a consuming project.
#define RAKNET_DEBUG_PRINTF(...) if(RakNet::rakDebugLogCallback) RakNet::rakDebugLogCallback(__VA_ARGS__)

#include "RakDebug.h"