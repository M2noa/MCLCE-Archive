/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#pragma once

#include "Core/Utility/MemoryTracker.h"

#include "CommonTypes.h"
#include "Core/Math/Color.h"
#include "client/renderer/renderer/PrimitiveMode.h"
//#include "Renderer/HAL/Enums/PrimitiveMode.h"
//#include "Renderer/Mesh.h"
//#include "Renderer/VertexFormat.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"
//#include "Renderer/TexturePtr.h"
#include "world/Facing.h"

#ifdef DEBUG_STATS
#define END() end(__FUNCTION__ )
#else
#define END() end()
#endif

class TextureData;
namespace mce {
	class MaterialPtr;
}

class Tessellator : public MemoryTracker {

	static const int MAX_MEMORY_USE = 16 * 1024 * 1024;
	static const int MAX_FLOATS = MAX_MEMORY_USE / 4 / 2;

public:

	Tessellator( MemoryTracker* parent = nullptr );

	static Tessellator instance;

	~Tessellator();

	void init();
	void clear();

	void begin( int maxVertices );
	void begin(mce::PrimitiveMode mode, int maxVertices);
	//void draw(const mce::MaterialPtr& material);
	//void draw(const mce::MaterialPtr& material, const mce::TexturePtr& texture);
	//void draw(const mce::MaterialPtr& material, const mce::TexturePtr& texture1, const mce::TexturePtr& texture2);

	void cancel();
	//mce::Mesh end( const char* function = nullptr, bool dontUpload = false );

	///method useful to allow the engine to optimize the index mFormat
	void beginIndices( int estimateCount = 0 );

	void color( int c );
	void color( int c, int alpha );
	void color( float r, float g, float b, float a = 1.f );
	void color( int r, int g, int b, int a = 255 );
	void color( byte r, byte g, byte b, byte a = 255 );
	void color( const Color& c );

	void colorABGR( int c );

	void normal( float x, float y, float z );
	void normal( const Vec3& n );
	void voidBeginAndEndCalls( bool doVoid );

	void tex( float u, float v );
	void tex(const Vec2& coord);
	void tex1( float u, float v );
	void tex1( const Vec2& coord);
	void tex2(float u, float v);
	void tex2(const Vec2& coord);

	void vertex( float x, float y, float z = 0 );
	void vertexUV( float x, float y, float z, float u, float v );
	void vertexUV( const Vec3& pos, float u, float v );

	void vertex( const Vec3& pos );

	void quadFacing(FacingID facing);

	void quad( Index i1, Index i2, Index i3, Index i4 );
	void quad( Index baseIdx, bool flipped = false );
	void quad( bool flipped = false );

	void triangle( Index i1, Index i2, Index i3 );

	void scale2d( float x, float y );
	void scale3d( float sx, float sy, float sz );
	void resetScale();

	void tilt();
	void resetTilt();

	void noColor();
	void enableColor();

	void trim();

	virtual MemoryStats getStats() const override;

	int getByteSize() const;

	void setOffset(float xo, float yo, float zo);
	void setOffset(const Vec3& v);
	void addOffset(float x, float y, float z);
	void addOffset(const Vec3& v);

	void setRotationOffset(float angle, const Vec3& axis);
	void addRotationOffset(float angle, const Vec3& axis);

	Index getVertexCount() const;

	int getPolygonCount() const;

	int getColor();

	//const mce::VertexFormat& getVertexFormat() const;

	bool isTessellating() const;

	//__inline void beginOverride() {
	//	begin(0);
	//	voidBeginAndEndCalls(true);
	//}

	//__inline void endOverrideAndDraw(const mce::MaterialPtr& mat, const mce::TexturePtr& tex) {
	//	voidBeginAndEndCalls(false);
	//	draw(mat, tex);
	//}

	//__inline bool isOverridden() {
	//	return mVoidBeginEnd;
	//}

	//__inline mce::Mesh endOverride() {
	//	voidBeginAndEndCalls(false);
	//	return end(nullptr);
	//}

	const std::vector<FacingID>& getFaces() const { return mQuadFacingList; }

	TArray<FVector> vertices;
	TArray<int32> indices;
	TArray<FVector> normals;
	TArray<FColor> colors;
	TArray<FVector2D> uv1;
	//TArray<FVector2D> uv2;
	//TArray<FRuntimeMeshTangent> tangents;

private:

	Tessellator(const Tessellator& rhs) = delete;

	Tessellator& operator=(const Tessellator& rhs) = delete;

	//struct CurrentVertexPointers {
	//	void* pos = nullptr;
	//	unsigned int* color = nullptr;
	//	int* normal = nullptr;
	//	VertexUVType* uv[3];

	//	const mce::VertexFormat* mFormat;

	//	CurrentVertexPointers() {
	//	}

	//	CurrentVertexPointers(unsigned char* base, const mce::VertexFormat& mVertexFormat );

	//	void nextVertex();

	//	explicit operator bool() const {
	//		return pos != nullptr;
	//	}

	//};

	//CurrentVertexPointers mCurrentPointers;

	std::vector<byte> mVertexArray;	//BUFFER
	bool mIndexPhase;
	int mIndexSize;
	int mIndexCount;

	//mce::VertexFormat mVertexFormat;
	int mVertexCountMax;

	Vec3 mOffset;
	Vec3 mScale;
	Vec3 mScale2D;

	Vec2 mCoordinates[3];

	unsigned int mColor;
	int mNormal;

	FacingID mQuadFacing = Facing::NOT_DEFINED;
	std::vector<FacingID> mQuadFacingList;
	int mCurQuadVertex = 0;
	int mCurQuadFacingIdx = 0;

	bool mTilted;
	FMatrix mTiltedMatrix;

	bool mNoColor;
	bool mVoidBeginEnd;

	Index mVertexCount;
	Index mCount;

	bool mTessellating;

	mce::PrimitiveMode mMode;

	byte* _allocateIndices(int n);
	void _tex(const Vec2& coord, int unit);

};
