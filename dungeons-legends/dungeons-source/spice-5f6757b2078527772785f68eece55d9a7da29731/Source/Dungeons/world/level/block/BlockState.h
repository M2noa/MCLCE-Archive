/********************************************************
*  (c) Mojang.    All rights reserved.                  *
*  (c) Microsoft. All rights reserved.                  *
*********************************************************/
#pragma once

#include "CommonTypes.h"

class BlockState {
public:
	BlockState();

	enum BlockStates {
		AgeBit = 0,
		Age,
		AttachedBit,
		Axis,
		BiteCounter,
		ButtonPressedBit,
		CoveredBit,
		Damage,
		DisarmedBit,
		DoorHingeBit,
		UpperBlockBit,
		Direction,
		EndPortalEyeBit,
		ExplodeBit,
		FacingDirection,
		FillLevel,
		Growth,
		HeadPieceBit,
		Height,
		InWallBit,
		LiquidDepth,
		MoisturizedAmount,
		NoDropBit,
		MappedType,
		OccupiedBit,
		OpenBit,
		OutputSubtractBit,
		OutputLitBit,
		PersistentBit,
		PoweredBit,
		RailDataBit,
		RailDirection,
		RedstoneSignal,
		RepeaterDelay,
		SuspendedBit,
		ToggleBit,
		TopSlotBit,
		TriggeredBit,
		UpdateBit,
		UpsideDownBit,
		VineGrowth,
		NumBlockStates
	};

	void initState(unsigned int& startBit, unsigned int numBits);

	bool isInitialized() const;

	template<typename T>
	void set(DataID& data, const T& value) const {
		DEBUG_ASSERT(mInitialized, "This block state was not added to this block when it was registered.");
		// create and use a mask for the affected bits to overwrite the old values 
		unsigned int mask = ((0xF << (4 - mNumBits)) & 0xF) >> (3 - mStartBit);
		unsigned int val = static_cast<unsigned int>(value) << (mStartBit - mNumBits + 1);
		data = (data & ~mask) | val;
	}

	template <typename T>
	T get(const DataID& data) const {
		DEBUG_ASSERT(mInitialized, "This block state was not added to this block when it was registered.");
		return static_cast<T>((0xF >> (4 - mNumBits)) & (static_cast<unsigned int>(data) >> (mStartBit - mNumBits + 1)));
	}

	// use this for boolean queries!
	bool getBool(const DataID& data) const;

private:
	unsigned int mStartBit;
	unsigned int mNumBits;
	bool mInitialized;
};
