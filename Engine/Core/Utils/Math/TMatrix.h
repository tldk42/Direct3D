#pragma once
#include <DirectXMath.h>

namespace JMath
{
	using namespace DirectX;

	struct TMatrix : public XMFLOAT4X4
	{
	public:
		TMatrix() noexcept;

		constexpr TMatrix(float m00, float m01, float m02, float m03,
						  float m10, float m11, float m12, float m13,
						  float m20, float m21, float m22, float m23,
						  float m30, float m31, float m32, float m33);

		explicit TMatrix(const FVector& r0, const FVector& r1, const FVector& r2);
		explicit TMatrix(const FVector4 r0, const FVector4& r1, const FVector4& r2, const FVector4& r3);

		TMatrix(const XMFLOAT4X4& M);
		TMatrix(const XMFLOAT3X3& M);
		TMatrix(const XMFLOAT4X3& M);

		explicit TMatrix(_In_reads_(16) const float* pArray);
		TMatrix(CXMMATRIX M);

		TMatrix(const TMatrix&) = default;
		TMatrix& operator=(const TMatrix&) = default;

		TMatrix(TMatrix&&) = default;
		TMatrix& operator=(TMatrix&&) = default;

		operator XMFLOAT4X4() const { return *this; }

	};
}
