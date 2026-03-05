#pragma once
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;

typedef Vector4 XMVECTOR;
typedef Matrix4 XMMATRIX;
typedef Vector4 XMFLOAT4;

XMMATRIX XMMatrixMultiply(XMMATRIX a, XMMATRIX b);
XMVECTOR XMMatrixDeterminant(XMMATRIX a);
XMMATRIX XMMatrixInverse(Vector4 *a, XMMATRIX b);