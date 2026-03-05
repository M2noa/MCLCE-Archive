/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#include "Dungeons.h"

#include "util/ClassID.h"

ClassID::IDType ClassID::getNextID() {
	static IDType lastID = 0;
	return ++lastID;
}
