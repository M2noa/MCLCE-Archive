/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once



template<typename INDEX, INDEX STEP = 1>
class Range {
	const INDEX mBeginIDX, mEndIDX;
public:

	class iterator {
		INDEX mIndex;
public:

		iterator(INDEX i) :
			mIndex(i) {
		}

		void operator++() {
			mIndex += STEP;
		}

		INDEX operator*() const {
			return mIndex;
		}

		bool operator != (const iterator& other) const {
			return mIndex != other.mIndex;
		}

	};

	Range(INDEX beginIDX, INDEX endIDX) :
		mBeginIDX(beginIDX)
		, mEndIDX(endIDX) {
		static_assert(STEP != 0, "Invalid step size");
		DEBUG_ASSERT((STEP > 0 && beginIDX <= endIDX) || (STEP < 0 && beginIDX >= endIDX), "Malformed iterator");
	}

	const iterator begin() const {
		return iterator(mBeginIDX);
	}

	const iterator end() const {
		return iterator(mEndIDX);
	}

};

template<typename INDEX, INDEX STEP = 1>
Range<INDEX, STEP> range(INDEX begin, INDEX end) {
	return Range<INDEX, STEP>(begin, end);
}

template<typename INDEX, INDEX STEP = 1>
Range<INDEX, STEP> range(INDEX count) {
	return Range<INDEX, STEP>(0, count);
}

template<typename INDEX, INDEX STEP = 1>
Range<INDEX, STEP> range_incl(INDEX begin, INDEX end) {
	return Range<INDEX, STEP>(begin, end + STEP);
}

template<typename INDEX, INDEX STEP = 1>
Range<INDEX, STEP> range_incl(INDEX count) {
	return Range<INDEX, STEP>(0, count + STEP);
}

template<int STEP = 1>
Range<int, STEP> rangef(float begin, float end) {
	return Range<int, STEP>((int)begin, (int)end);
}

template<int STEP = 1>
Range<int, STEP> rangef_incl(float begin, float end) {
	return Range<int, STEP>((int)begin, (int)end + 1);
}

template<typename INDEX, int STEP = -1>
Range<int, STEP> reverse_range(INDEX begin, INDEX end) {
	return range<int, STEP>(((int)end) + STEP, ((int)begin) + STEP);
}

template<typename INDEX, int STEP = -1>
Range<int, STEP> reverse_range(INDEX count) {
	return reverse_range<INDEX, STEP>(0, count);
}
