#pragma once
#include "Utils/Math/Vector2D.h"

inline Matrix TranslationMatrix(float InX, float InY, float InZ = 0.f)
{
	return XMMatrixTranslation(InX, InY, InZ);
}

inline Matrix RotationMatrix(float InDegree)
{
	float radians = XMConvertToRadians(InDegree);
	return XMMatrixRotationZ(radians);
}

inline Matrix ScaleMatrix(float InX, float InY)
{
	return XMMatrixScaling(InX, InY, 1.0f);
}

/** 4x4 행렬 -> 위치벡터 반환 */
inline FVector Mat2LocVector(const Matrix& InMatrix)
{
	XMFLOAT4X4 locationVec;
	XMStoreFloat4x4(&locationVec, InMatrix);
	return {locationVec._41, locationVec._42, locationVec._43};
}

/** 4x4 행렬 -> 회전 각 반환 */
inline float Mat2RotDegree(const Matrix& InMatrix)
{
	XMFLOAT4X4 matrixValues;
	XMStoreFloat4x4(&matrixValues, InMatrix);

	return 0;
}

/** 4x4 행렬 -> 크기벡터 반환 */
inline FVector Mat2ScaleVector(const Matrix& InMatrix)
{
	XMFLOAT4X4 matrixValues;
	XMStoreFloat4x4(&matrixValues, InMatrix);

	const float scaleX = sqrtf(matrixValues._11 * matrixValues._11 + matrixValues._12 * matrixValues._12);
	const float scaleY = sqrtf(matrixValues._21 * matrixValues._21 + matrixValues._22 * matrixValues._22);
	const float scaleZ = sqrtf(matrixValues._31 * matrixValues._31 + matrixValues._32 * matrixValues._32);

	return FVector{scaleX, scaleY, scaleZ};
}
