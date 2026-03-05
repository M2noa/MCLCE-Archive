/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/
#pragma once

#include "common_header.h"

#include "world/level/BlockPos.h"
#include "world/Pos.h"
#include "world/level/LevelConstants.h"

#include "util/Bounds.h"

template<typename PTR>
struct ptr_move {
	ptr_move(PTR& dest, PTR& src) {
		dest = std::move(src);
	}

};

template<typename PTR>
struct ptr_move<PTR*>{
	ptr_move(PTR*& dest, PTR*& src) {
		dest = src;
		src = nullptr;
	}

};

template<class Elem>
class GridArea {

public:

	typedef std::vector<Elem> ChunkList;
	typedef typename ChunkList::iterator Iterator;
	typedef typename ChunkList::const_iterator ConstIterator;
	typedef std::function<void (Elem&)> DestroyCallback;
	typedef DestroyCallback AddCallback;
	typedef std::function<Elem(const Pos&)> GenerateCallback;

	struct MoveStats {
		unsigned short removed = 0, added = 0, kept = 0;
	};

	GridArea() 
	: mBounds()
	, generate(GenerateCallback())
	, add(AddCallback())
	, destroy(DestroyCallback())
	, mCircle(false) {
	}

	GridArea(
		const BlockPos& min,
		const BlockPos& max,
		int chunkSide,
		const GenerateCallback& generate = GenerateCallback(),
		const DestroyCallback& destroy = DestroyCallback(),
		bool circle = false,
		const AddCallback& add = AddCallback()) :
		mBounds(min, max, chunkSide)
		, generate(generate)
		, destroy(destroy)
		, mCircle(circle)
		, add(add) {

		mChunks.resize(mBounds.mVolume);

		if (generate) {
			_fill();
		}
	}

	GridArea(
		const BlockPos& pos,
		int blockRadius,
		int chunkSide,
		const GenerateCallback& generate = GenerateCallback(),
		const DestroyCallback& destroy = DestroyCallback(),
		bool circle = false,
		const AddCallback& add = AddCallback()) :
		GridArea(BlockPos(pos.x - blockRadius, pos.y >= LEVEL_SPAWN_HEIGHT ? 0 : pos.y - blockRadius, pos.z - blockRadius),
				BlockPos(pos.x + blockRadius, pos.y >= LEVEL_SPAWN_HEIGHT ? blockRadius : pos.y + blockRadius, pos.z + blockRadius),
				chunkSide, generate, destroy, circle, add) {

	}

	GridArea(GridArea&& rhs) {
		_move(std::move(rhs));
	}

	GridArea& operator=(GridArea&& rhs) {
		clear();
		_move(std::move(rhs));

		return *this;
	}

	~GridArea() {
		clear();
	}

	void clear() {
		if (destroy) {
			for (auto& c : mChunks) {//tell the callback everything will be destroyed
				if (c) {
					destroy(c);
				}
			}
		}
		mChunks.clear();
		mBounds = Bounds();
	}

	Elem& at(const Pos& pos) {
		return mChunks[mBounds.idxFromWorldPos(pos)];
	}

	Elem& at(const BlockPos& p) {
		return mChunks[mBounds.idxFromWorldPos(p)];
	}

	const Elem& at(const Pos& pos) const {
		return mChunks[mBounds.idxFromWorldPos(pos)];
	}

	const Elem& at(const BlockPos& p) const {
		return mChunks[mBounds.idxFromWorldPos(p)];
	}

	bool contains(const BlockPos& p) const {
		return mBounds.contains(p);
	}

	bool contains(const Pos& pos) const {
		return mBounds.contains(pos);
	}

	bool contains(const Bounds& area) const {
		return mBounds.contains(area);
	}

	bool isInCircle(const BlockPos& p)  {
		Pos chunkPos = mBounds.gridToChunk(p);
		return isChunkInCircle(chunkPos);
	}

	bool isChunkInCircle(const Pos& p){
		const Vec3 from = mBounds.getCenter();
		const Vec3 to((float)p.x, (float)p.y, (float)p.z);

		// Check to see if the chunk is within the radius of the bounds.
		// increase the radius by the length of the chunk diagonal to ensure all corners of the chunk are included in the check 
		const float CHUNK_DIAGONAL_LENGTH = SQRT_3;
		return from.distanceToSqr(to) < powf(mBounds.mDim.x / 2.f + CHUNK_DIAGONAL_LENGTH, 2);
	}

	const Bounds& getBounds() const {
		return mBounds;
	}

	///moves this area destroying the unneeded chunks and creating new ones
	/** returns the range of new elements
	 */
	void move( const BlockPos& blockMin, const BlockPos& blockMax ) {
		Bounds newBounds( blockMin, blockMax, mBounds.mSide );
		if (mBounds == newBounds) {
			return;
		}

		//prepare the new grid buffer
		mNewChunks.resize(newBounds.mVolume);

		//for each chunk in the old buffer, try to see if it belongs to the new buffer
		_moveOldChunks(newBounds);

		if (destroy) {
			//for each chunk remaining in the old buffer, call the destroy callback as it will be destroyed in the move
			_trim();
		}

		//move over - destroy old chunks and set the new bounds
		mBounds = newBounds;
		std::swap(mChunks, mNewChunks);
		mNewChunks.clear();

		//fill in still empty chunks
		if (generate) {
			_fill();
		}
	}

	///moves this area destroying the unneeded chunks and creating new ones
	/** returns the range of new elements
	 */
	void move(const BlockPos& pos, int blockRadius ) {
		move(pos - blockRadius, pos + blockRadius);
	}

	Iterator begin() {
		return mChunks.begin();
	}

	Iterator end() {
		return mChunks.end();
	}

	ConstIterator begin() const {
		return mChunks.begin();
	}

	ConstIterator end() const {
		return mChunks.end();
	}

	size_t size() const {
		return mChunks.size();
	}

	bool empty() const {
		return mChunks.empty();
	}

	explicit operator bool() const {
		return mBounds.mSide != 0;
	}

protected:

	DestroyCallback destroy;
	GenerateCallback generate;
	AddCallback add;

	Bounds mBounds;

	ChunkList mChunks, mNewChunks;

	bool mCircle;

	void _move(GridArea&& rhs) {
		generate = std::move(rhs.generate);
		destroy = std::move(rhs.destroy);
		add = std::move(rhs.add);
		mBounds = rhs.mBounds;
		mCircle = rhs.mCircle;

		mChunks = std::move(rhs.mChunks);	//grab the chunks from that & let it die
		mNewChunks = std::move(rhs.mNewChunks);
	}

	void _trim() {
		//remove all the remaining chunks
		for (auto&& chunk : mChunks) {
			if (chunk) {
				destroy(chunk);
			}
		}
	}

	void _fill() {
		auto itr = mBounds.begin(), endItr = mBounds.end();
		if (mCircle) {
			for (; itr != endItr; ++itr) {

				auto& old = mChunks[itr.index()];
				if (!old) {	//if this cell was empty, use the callback to generate a new one
					if(isChunkInCircle(itr)){
						old = generate(itr);
						if(add) {
							add(old);
						}
					}
				}
			}
		}
		else{
			for (; itr != endItr; ++itr) {
				auto& old = mChunks[itr.index()];
				if (!old) {	//if this cell was empty, use the callback to generate a new one
					old = generate(itr);
					if(add) {
						add(old);
					}
				}
			}
		}
	}

	void _moveOldChunks( const Bounds& newBounds ) {
		if (!mCircle) {
			auto itr = mBounds.begin(), endItr = mBounds.end();

			for (; itr != endItr; ++itr) {
				auto& c = mChunks[itr.index()];
				if (c && newBounds.contains(itr)) {
					const int newIdx = newBounds.idxFromWorldPos(itr);
					ptr_move<Elem>(mNewChunks[newIdx], c);
				}
			}
		}
		else {	//more expensive loop: we need to not move stuff that would end outside the circle
			auto itr = mBounds.begin(), endItr = mBounds.end();

			for (; itr != endItr; ++itr) {
				auto& c = mChunks[itr.index()];
				if (c && newBounds.contains(itr)) {
					if (isChunkInCircle(itr)) {
						const int newIdx = newBounds.idxFromWorldPos(itr);
						ptr_move<Elem>(mNewChunks[newIdx], c);
					}
				}
			}
		}
	}

};
