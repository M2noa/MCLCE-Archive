/********************************************************
*   (c) Mojang. All rights reserved                       *
*   (c) Microsoft. All rights reserved.                   *
*********************************************************/

#include "Dungeons.h"

#include "client/renderer/renderer/Tessellator.h"

//#include "client/renderer/texture/TextureData.h"
#include "Core/Debug/Log.h"
#include "Core/Math/Color.h"
//#include "Renderer/HAL/Interface/RenderContext.h"
//#include "Renderer/HAL/Interface/RenderContextImmediate.h"
//#include "Renderer/HAL/Interface/RenderDevice.h"
//#include "Renderer/MaterialPtr.h"
//#include "Renderer/MatrixStack.h"
//#include "Renderer/RenderMaterial.h"
//#include "Renderer/HAL/Enums/RenderFeature.h"

#include "util/Math.h"
#include "util/StringUtils.h"
#include "world/phys/Vec2.h"
#include "world/phys/Vec3.h"

Tessellator Tessellator::instance;

template<typename T>
T packNormalized(float x) {
	return (T)std::ceil(x * std::numeric_limits<T>::max());
}

//Tessellator::CurrentVertexPointers::CurrentVertexPointers(unsigned char* base, const mce::VertexFormat& mVertexFormat)
//	: mFormat(&mVertexFormat) {
//
//	pos = (void*)(base + (intptr_t)mVertexFormat.getFieldOffset(mce::VertexField::Position));
//
//	if (mVertexFormat.hasField(mce::VertexField::Color)) {
//		color = (unsigned int*)(base + (intptr_t)mVertexFormat.getFieldOffset(mce::VertexField::Color));
//	}
//
//	if (mVertexFormat.hasField(mce::VertexField::Normal)) {
//		normal = (int*)(base + (intptr_t)mVertexFormat.getFieldOffset(mce::VertexField::Normal));
//	}
//
//	const int numUVs = 3;	//we should derive this from the vert mFormat
//
//	for (int i = 0; i < numUVs; ++i) {
//		auto field = (mce::VertexField)((int)mce::VertexField::UV0 + i);
//		if (mVertexFormat.hasField(field)) {
//			uv[i] = (VertexUVType*)(base + (intptr_t)mVertexFormat.getFieldOffset(field));
//		}
//		else {
//			uv[i] = nullptr;
//		}
//	}
//}
//
//void Tessellator::CurrentVertexPointers::nextVertex() {
//	pos = (void*)((char*)pos + mFormat->getVertexSize());
//	color = color ? (unsigned int*)((char*)color + mFormat->getVertexSize()) : nullptr;
//	normal = normal ? (int*)((char*)normal + mFormat->getVertexSize()) : nullptr;
//
//	const int numUVs = 3;	//we should derive this from the vert mFormat
//
//	for (int i = 0; i < numUVs; ++i) {
//		uv[i] = uv[i] ? (VertexUVType*)((char*)uv[i] + mFormat->getVertexSize()) : nullptr;
//	}
//}

Tessellator::Tessellator(MemoryTracker* parent)
	: MemoryTracker("Tessellator", parent)
	//, mCurrentPointers()
	, mIndexPhase(false)
	, mIndexSize(0)
	, mIndexCount(0)
	//, mVertexFormat()
	, mVertexCountMax(0)
	, mOffset()
	, mScale(1.f)
	, mScale2D(1.f)
	, mColor(0)
	, mNormal(0)
	, mTilted(false)
	, mTiltedMatrix(FMatrix())
	, mNoColor(false)
	, mVoidBeginEnd(false)
	, mVertexCount(0)
	, mCount(0)
	, mTessellating(false)
	, mMode(mce::PrimitiveMode::None) {

	// TODO: rherlitz fix
	//mTiltedMatrix = glm::rotate(glm::mat4(1.0f), 180 + 30.0f, glm::vec3(1, 0, 0));
	//mTiltedMatrix = glm::rotate(mTiltedMatrix, 45.0f, glm::vec3(0, 1, 0));
}

Tessellator::~Tessellator() {

}

void Tessellator::init() {

}

void Tessellator::clear() {
	vertices.Empty();
	indices.Empty();
	colors.Empty();
	normals.Empty();
	uv1.Empty();
	//uv2.Empty();
	//tangents.Empty();

	mCount = 0;
	mIndexCount = 0;
	mVertexCount = 0;
	mVoidBeginEnd = false;
	mTessellating = false;
	mIndexPhase = false;

	mVertexArray.clear();
	//mCurrentPointers = CurrentVertexPointers();

	//mVertexFormat = mce::VertexFormat::EMPTY;
}

void Tessellator::cancel() {
	mTessellating = false;
}

void Tessellator::begin(mce::PrimitiveMode mode, int maxVertices) {
	if (mTessellating || mVoidBeginEnd) {
		if (mTessellating && !mVoidBeginEnd) {
			DEBUG_FAIL("already mTessellating!\n");
		}
		return;
	}
	//if (mTessellating) {
	//    throw /*new*/ IllegalStateException("Already mTessellating!");
	//}
	clear();
	mMode = mode;
	mNoColor = false;

	mTessellating = true;

// 	mVertexFormat.enableField(mce::VertexField::Position);	//position is always enabled

	mIndexSize = 0;	//let the user decide
	mIndexCount = 0;
	mVertexCountMax = maxVertices;
	mCurQuadFacingIdx = 0;
	mCurQuadVertex = 0;
	mQuadFacingList.clear();
}

void Tessellator::begin(int maxVertices = 0) {
	begin(mce::PrimitiveMode::QuadList, maxVertices);
}

void Tessellator::_tex(const Vec2& coord, int unit) {
	DEBUG_ASSERT(coord.x >= 0.f && coord.x <= 1.f && coord.y >= 0.f && coord.y <= 1.f, "Invalid texture coordinates");

	mCoordinates[unit] = coord;

	//const mce::VertexField field = static_cast<mce::VertexField>(enum_cast(mce::VertexField::UV0) + unit);
	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(field);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(field), "Can't add UV coordinates at this point");
	//}
}

void Tessellator::tex(const Vec2& v) {
	_tex(v, 0);
}

void Tessellator::tex1(const Vec2& v) {
	_tex(v, 1);
}

void Tessellator::tex2(const Vec2& v) {
	_tex(v, 2);
}

void Tessellator::tex(float u, float v) {
	tex(Vec2(u, v));
}

void Tessellator::tex1(float u, float v) {
	tex1(Vec2(u, v));
}

void Tessellator::tex2(float u, float v) {
	tex2(Vec2(u, v));
}

int Tessellator::getColor() {
	return mColor;
}

//const mce::VertexFormat& Tessellator::getVertexFormat() const {
//	return mVertexFormat;
//}

bool Tessellator::isTessellating() const {
	return mTessellating;
}

void Tessellator::color(const Color& c) {
	color((byte)(c.r * 255), (byte)(c.g * 255), (byte)(c.b * 255), (byte)(c.a * 255));
}

void Tessellator::color(float r, float g, float b, float a) {
	color(Color(r, g, b, a));
}

void Tessellator::color(byte r, byte g, byte b, byte a) {
	if (mNoColor) {
		return;
	}

// 	mColor = (a << 24) | (b << 16) | (g << 8) | (r);
	mColor = (a << 24) | (r << 16) | (g << 8) | (b);

	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(mce::VertexField::Color);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(mce::VertexField::Color), "Can't add Vertex Color at this point");
	//}
}

void Tessellator::color(int r, int g, int b, int a) {
	color(
		(byte)Math::clamp(r, 0, 255),
		(byte)Math::clamp(g, 0, 255),
		(byte)Math::clamp(b, 0, 255),
		(byte)Math::clamp(a, 0, 255));
}

void Tessellator::color(int c) {
	int r = ((c >> 16) & 255);
	int g = ((c >> 8) & 255);
	int b = ((c) & 255);
	color(r, g, b);
}

//@note: doesn't care about endianess
void Tessellator::colorABGR(int c) {
	if (mNoColor) {
		return;
	}
	mColor = c;

	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(mce::VertexField::Color);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(mce::VertexField::Color), "Can't add Vertex Color at this point");
	//}
}

void Tessellator::color(int c, int alpha) {
	int r = ((c >> 16) & 255);
	int g = ((c >> 8) & 255);
	int b = ((c) & 255);
	color(r, g, b, alpha);
}

void Tessellator::vertexUV(float x, float y, float z, float u, float v) {
	tex(u, v);
	vertex(x, y, z);
}

void Tessellator::vertexUV(const Vec3& pos, float u, float v) {
	tex(u, v);
	vertex(pos);
}

void Tessellator::scale2d(float sx, float sy) {
	mScale2D.x *= sx;
	mScale2D.y *= sy;
}

void Tessellator::scale3d(float sx, float sy, float sz) {
	mScale.x *= sx;
	mScale.y *= sy;
	mScale.z *= sz;
}

void Tessellator::resetScale() {
	mScale2D.x = mScale2D.y = 1;
	mScale.x = 1;
	mScale.y = 1;
	mScale.z = 1;
}

void Tessellator::beginIndices(int extimateCount) {
	DEBUG_ASSERT(mTessellating && mIndexPhase == false, "invalid state!");
	mIndexPhase = true;

	mIndexSize = (getVertexCount() <= std::numeric_limits<unsigned short>::max()) ? sizeof(unsigned short) : sizeof(unsigned int);

	if (extimateCount == 0 && mMode == mce::PrimitiveMode::QuadList) {
		extimateCount = (getVertexCount() / 4 * 6 * mIndexSize);
	}

	mVertexArray.reserve(mVertexArray.size() + extimateCount * mIndexSize);
}

byte* Tessellator::_allocateIndices(int n) {
	int sz = n * mIndexSize;
	mVertexArray.resize(mVertexArray.size() + sz);
	return &mVertexArray[mVertexArray.size() - sz];
}

template<typename I>
void _triangle(void* ptr, Index i1, Index i2, Index i3) {
	auto base = (I*)ptr;

	base[0] = (I)i1;
	base[1] = (I)i2;
	base[2] = (I)i3;
}

void Tessellator::triangle(Index i1, Index i2, Index i3) {
	DEBUG_ASSERT(mIndexPhase, "Can't draw indices now");
	DEBUG_ASSERT(i1 < getVertexCount(), "Vertex outside mesh");
	DEBUG_ASSERT(i2 < getVertexCount(), "Vertex outside mesh");
	DEBUG_ASSERT(i3 < getVertexCount(), "Vertex outside mesh");

	auto ptr = _allocateIndices(3);

	switch (mIndexSize) {
		case 1:
			_triangle<byte>(ptr, i1, i2, i3);
			break;
		case 2:
			_triangle<unsigned short>(ptr, i1, i2, i3);
			break;
		case 4:
			_triangle<unsigned int>(ptr, i1, i2, i3);
			break;
		default:
			DEBUG_FAIL("Index size not supported");
	}

	mIndexCount += 3;
}

void Tessellator::quad(Index i1, Index i2, Index i3, Index i4) {
	triangle(i1, i2, i3);
	triangle(i4, i1, i3);
}

void Tessellator::quad(Index baseIdx, bool flipped) {

	if (!flipped) {
		quad(
			baseIdx,
			baseIdx + 1,
			baseIdx + 2,
			baseIdx + 3);
	}
	else {
		quad(
			baseIdx + 3,
			baseIdx + 2,
			baseIdx + 1,
			baseIdx);
	}
}

void Tessellator::quad(bool flipped) {
	quad(mCount - 4, flipped);
}

void Tessellator::vertex(float x, float y, float z) {

	//if (mCount == mce::RenderDevice::getInstance().getMaxVertexCount()) {	//filled up :( stop adding vertices
	if (mCount >= 1000000) {	//filled up :( stop adding vertices
		return;
	}

	mCount++;

// 	void* oldMem = mVertexArray.data();

	//if the user specified a max vertex mCount, apply here
	//if (mVertexCountMax) {
	//	mVertexArray.reserve(mVertexCountMax * mVertexFormat.getVertexSize());
	//	mVertexCountMax = 0;	//stop applying
	//}

	//create a new empty vertex
	//size_t size = (mVertexCount + 1) * mVertexFormat.getVertexSize();
	//mVertexArray.resize(size);

	//if (!mCurrentPointers || oldMem != mVertexArray.data()) {//the memory or the mFormat changed, need to update the
	//														// pointers
	//	mCurrentPointers = CurrentVertexPointers(mVertexArray.data() + mVertexFormat.getVertexSize() * mVertexCount, mVertexFormat);
	//}
	//else {
	//	mCurrentPointers.nextVertex();
	//}

	// TODO: rherlitz fix
	//if (mTilted) {
	//	glm::vec4 rotatePos = glm::vec4(x, y, z, 1.0f);
	//	rotatePos = mTiltedMatrix * rotatePos;
	//	x = rotatePos[0];
	//	y = rotatePos[1];
	//	z = rotatePos[2];
	//}

	x = float(mScale2D.x * ((x * mScale.x) + mOffset.x));
	y = float(mScale2D.y * ((y * mScale.x) + mOffset.y));
	z = float((z * mScale.x) + mOffset.z);

	vertices.Add(FVector(x, z, y));
	uv1.Add(FVector2D(mCoordinates[0].x, mCoordinates[0].y));
	//uv2.Add(FVector2D(uvx, uvy));
	colors.Add(FColor(mColor));
	//tangents.Add(FRuntimeMeshTangent(0, 1, 0));

	++mVertexCount;

	if (mCurQuadVertex == 3) {
		mCurQuadVertex = 0;
		mQuadFacingList.push_back(mQuadFacing);
		mQuadFacing = Facing::NOT_DEFINED;
		mCurQuadFacingIdx++;

		auto v1 = mVertexCount - 4;
		auto v2 = mVertexCount - 3;
		auto v3 = mVertexCount - 2;
		auto v4 = mVertexCount - 1;

		auto& vert1 = vertices[v1];
		auto& vert2 = vertices[v2];
		auto& vert3 = vertices[v3];

		FVector normal = FVector::CrossProduct(vert3 - vert1, vert2 - vert1);
		normal.Normalize();

		normals.Add(normal);
		normals.Add(normal);
		normals.Add(normal);
		normals.Add(normal);

		indices.Add(v1);
		indices.Add(v2);
		indices.Add(v3);
		indices.Add(v3);
		indices.Add(v4);
		indices.Add(v1);
	}
	else {
		mCurQuadVertex++;
	}


//	if (mce::RenderDevice::getInstance().checkFeatureSupport(mce::RenderFeature::HalfFloats)) {
//		glm::half* pos = (glm::half*)mCurrentPointers.pos;
//		pos[0] = glm::half(mScale2D.x * ((x * mScale.x) + mOffset.x));
//		pos[1] = glm::half(mScale2D.y * ((y * mScale.x) + mOffset.y));
//		pos[2] = glm::half((z * mScale.x) + mOffset.z);
//		pos[3] = glm::half(1.0f);
//	}
//	else {
//		float* pos = (float*)mCurrentPointers.pos;
//		pos[0] = float(mScale2D.x * ((x * mScale.x) + mOffset.x));
//		pos[1] = float(mScale2D.y * ((y * mScale.x) + mOffset.y));
//		pos[2] = float((z * mScale.x) + mOffset.z);
//	}
//
//	const int numUVs = 3;	//should be derived from vert mFormat
//
//	for (int i = 0; i < numUVs; ++i) {
//		if (mCurrentPointers.uv[i]) {
//
//#if defined(GFX_API_DX11)
//			if (mce::RenderContextImmediate::getAsConst().getDevice()->getDeviceFeatureLevel() < D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0) {
//
//				mCurrentPointers.uv[i][0] = packNormalized<short>(mCoordinates[i].x);
//				mCurrentPointers.uv[i][1] = packNormalized<short>(mCoordinates[i].y);
//			}
//			else
//#endif
//			{
//				mCurrentPointers.uv[i][0] = packNormalized<unsigned short>(mCoordinates[i].x);
//				mCurrentPointers.uv[i][1] = packNormalized<unsigned short>(mCoordinates[i].y);
//			}
//		}
//	}
//
//	if (mCurrentPointers.color) {
//		*mCurrentPointers.color = mColor;
//	}
//
//	if (mCurrentPointers.normal) {
//		*mCurrentPointers.normal = mNormal;
//	}
//
//	++mVertexCount;
//
//	if (mCurQuadVertex == 3) {
//		mCurQuadVertex = 0;
//		mQuadFacingList.push_back(mQuadFacing);
//		mQuadFacing = Facing::NOT_DEFINED;
//		mCurQuadFacingIdx++;
//	}
//	else {
//		mCurQuadVertex++;
//	}
}

void Tessellator::quadFacing(FacingID facing) {
	if (mCurQuadVertex == 0) {
		mQuadFacing = facing;
	}
}

void Tessellator::vertex(const Vec3& pos) {
	vertex(pos.x, pos.y, pos.z);
}

void Tessellator::noColor() {
	mNoColor = true;
}

void Tessellator::normal(float x, float y, float z) {
	auto byte = (signed char*)(&mNormal);
#if defined(GFX_API_DX11)
	if (mce::RenderContextImmediate::getAsConst().getDevice()->getDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0) {
		// move from -1 to 1, to 0 to 1, undo transform in shader
		x = x * 0.5f + 0.5f;
		y = y * 0.5f + 0.5f;
		z = z * 0.5f + 0.5f;

		byte[0] = packNormalized<unsigned char>(x);
		byte[1] = packNormalized<unsigned char>(y);
		byte[2] = packNormalized<unsigned char>(z);
		byte[3] = 0;
	}
	else {
		byte[0] = packNormalized<signed char>(x);
		byte[1] = packNormalized<signed char>(y);
		byte[2] = packNormalized<signed char>(z);
		byte[3] = 0;
	}
#else
	byte[0] = packNormalized<signed char>(x);
	byte[1] = packNormalized<signed char>(y);
	byte[2] = packNormalized<signed char>(z);
	byte[3] = 0;
#endif

	//if (!mCurrentPointers) {
	//	mVertexFormat.enableField(mce::VertexField::Normal);
	//}
	//else {
	//	DEBUG_ASSERT(mVertexFormat.hasField(mce::VertexField::Normal), "Can't add normals at this point");
	//}
}

void Tessellator::normal(const Vec3& n) {
	normal(n.x, n.y, n.z);
}

void Tessellator::setOffset(float xo, float yo, float zo) {
	mOffset.x = xo;
	mOffset.y = yo;
	mOffset.z = zo;
}

void Tessellator::addOffset(float x, float y, float z) {
	mOffset.x += x;
	mOffset.y += y;
	mOffset.z += z;
}

void Tessellator::setOffset(const Vec3& v) {
	mOffset = v;
}

void Tessellator::addOffset(const Vec3& v) {
	mOffset += v;
}

// TODO: rherlitz fix
void Tessellator::setRotationOffset(float angle, const Vec3& axis) {
	//mTiltedMatrix = glm::rotate(glm::mat4(1.f), angle, (const glm::vec3&)axis);
}

void Tessellator::addRotationOffset(float angle, const Vec3& axis) {
	//mTiltedMatrix = glm::rotate(glm::mat4(1.f), angle, (const glm::vec3&)axis) * mTiltedMatrix;
}

Index Tessellator::getVertexCount() const {
	return mVertexCount;
}

//mce::Mesh Tessellator::end(const char* function, bool dontUpload) {
//
//	if (!mTessellating) {
//		LOGE("not mTessellating!\n");
//		return mce::Mesh();
//	}
//
//	if (mVoidBeginEnd) {
//		return mce::Mesh();
//	}
//
//	if (getVertexCount() == 0 || !mCurrentPointers) {
//		mTessellating = false;
//		return mce::Mesh();
//	}
//
//	mce::Mesh result(
//		mVertexFormat,
//		getVertexCount(),
//		mIndexCount,
//		mIndexSize,
//		mMode,
//		mVertexArray.data(),
//		dontUpload);
//
//	if (!dontUpload) {
//		clear();
//	}
//
//	return result;
//}
//
//void Tessellator::draw(const mce::MaterialPtr& material) {
//	if (!mTessellating || mVoidBeginEnd) {
//		return;
//	}
//
//	if (getVertexCount() > 0) {
//		end(__FUNCTION__, true).render(material);
//	}
//	clear();
//}


//void Tessellator::draw(const mce::MaterialPtr& material, const mce::TexturePtr& texture) {
//	if (!mTessellating || mVoidBeginEnd) {
//		return;
//	}
//
//	if (getVertexCount() > 0) {
//		end(__FUNCTION__, true).render(material, texture);
//	}
//	clear();
//}
//
//void Tessellator::draw(const mce::MaterialPtr& material, const mce::TexturePtr& texture1, const mce::TexturePtr& texture2) {
//	if (!mTessellating || mVoidBeginEnd) {
//		return;
//	}
//
//	if (getVertexCount() > 0) {
//		end(__FUNCTION__, true).render(material, texture1, texture2);
//	}
//	clear();
//}

void Tessellator::voidBeginAndEndCalls(bool doVoid) {
	mVoidBeginEnd = doVoid;
}

void Tessellator::enableColor() {
	mNoColor = false;
}

void Tessellator::tilt() {
	mTilted = true;
}

void Tessellator::resetTilt() {
	mTilted = false;

	// TODO: rherlitz fix
	//mTiltedMatrix = glm::rotate(glm::mat4(1.0f), 180 + 30.0f, glm::vec3(1, 0, 0));
	//mTiltedMatrix = glm::rotate(mTiltedMatrix, 45.0f, glm::vec3(0, 1, 0));
}

MemoryStats Tessellator::getStats() const {
	return MemoryStats(sizeof(*this) + mVertexArray.capacity(), 1);
}

int Tessellator::getByteSize() const {
	return mVertexArray.size();
}

void Tessellator::trim() {
	Util::freeVectorMemory(mVertexArray);
}

int Tessellator::getPolygonCount() const {
	int n = mIndexCount ? mIndexCount : getVertexCount();	//get the actual number of primitives

	switch (mMode) {
		case mce::PrimitiveMode::QuadList:
			return (n / 4) * 2;
		case mce::PrimitiveMode::TriangleList:
			return n / 3;
		case mce::PrimitiveMode::TriangleStrip:
			return n - 2;
		case mce::PrimitiveMode::LineList:
			return n / 2;
		case mce::PrimitiveMode::LineStrip:
			return n - 1;
		default:
			return 0;
	}
}
