/********************************************************
*   (c) Mojang. All rights reserved                     *
*   (c) Microsoft. All rights reserved.                 *
*********************************************************/

#pragma once

struct Tick {
	static const Tick MAX;

	explicit Tick(uint64_t v) :
		tickID(v) {
	}

	Tick() :
		Tick(0) {
	}

	Tick(const Tick& t) {
		*this = t;
	}

	const Tick& operator = (const Tick& t) {
		tickID = t.tickID;
		return *this;
	}

	void operator++() {
		++tickID;
	}

	Tick operator+(int o) const {
		return Tick(tickID + o);
	}

	uint64_t operator%(int mod) const {
		return tickID % mod;
	}

	bool operator<(const Tick& other) const {
		return tickID < other.tickID;
	}

	bool operator>(const Tick& other) const {
		return tickID > other.tickID;
	}

	bool operator<=(const Tick& other) const {
		return tickID <= other.tickID;
	}

	bool operator==(const Tick& other) const {
		return tickID == other.tickID;
	}

	uint64_t getTimeStamp() const {
		return tickID;
	}

private:

	uint64_t tickID;
};
