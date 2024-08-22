#pragma once
#include "Vector2D.h"
#include <DirectXMath.h>

namespace JMath
{
	template <typename T>
	struct TVector
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating point!");

	public:
		using FReal = T;

		union
		{
			struct
			{
				T X;
				T Y;
				T Z;
			};

			T XYZ[3];
		};

		static const TVector ZeroVector;
		static const TVector OneVector;

		static const TVector UpVector;
		static const TVector DownVector;
		static const TVector ForwardVector;
		static const TVector BackwardVector;
		static const TVector RightVector;
		static const TVector LeftVector;

		static const TVector XAxisVector;
		static const TVector YAxisVector;
		static const TVector ZAxisVector;

		static inline TVector Zero() { return ZeroVector; }
		static inline TVector One() { return OneVector; }
		static inline TVector UnitX() { return XAxisVector; }
		static inline TVector UnitY() { return YAxisVector; }
		static inline TVector UnitZ() { return ZAxisVector; }

		inline TVector operator+(const TVector& V) const;
		inline TVector operator-(const TVector& V) const;
		inline TVector operator*(const TVector& V) const;
		inline TVector operator/(const TVector& V) const;
		inline TVector operator|(const TVector& V) const;
		inline TVector operator^(const TVector& V) const;

		inline TVector operator*(T Scale) const;
		inline TVector operator*(TVector2<T> InVector2) const;

		// ------------------------ 비교 연산자 ----------------------------
		bool operator==(const TVector&) const;
		bool operator!=(const TVector&) const;

		// ------------------------ 연산자 ----------------------------
		TVector& operator+=(const TVector&);
		TVector& operator-=(const TVector&);
		TVector& operator*=(T Scale);
		TVector& operator*=(const TVector&);
		TVector& operator/=(T Scale);

		// ------------------------ 단항 연산자 ----------------------------
		TVector& operator++();
		TVector& operator--();

		bool IsNearlyZero(T Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsNearlyEqual(const TVector& Other, T Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsZero() const;

	public:
		// ------------------------ 생성자 ----------------------------
		TVector() noexcept;
		constexpr TVector(T InF);
		constexpr TVector(T InX, T InY, T InZ);

		explicit TVector(const TVector2<T> V, T InZ);
		explicit TVector(const DirectX::XMFLOAT3& InVector);
		explicit TVector(const DirectX::XMVECTORF32& InVector);

		// ------------------------ 복사,이동 생성자 ----------------------------
		TVector(const TVector&) = default;
		TVector(TVector&&)      = default;

		// ------------------------ 대입 연산자 ----------------------------
		TVector& operator=(const DirectX::XMVECTORF32& InVector);
		TVector& operator=(const TVector&) = default;
		TVector& operator=(TVector&&)      = default;

		// operator DirectX::XMVECTOR() const;
	};

	template <typename T> TVector<T> TVector<T>::operator+(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator-(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator*(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator/(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator|(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator^(const TVector& V) const {}

	template <typename T> TVector<T> TVector<T>::operator*(T Scale) const
	{
		return TVector(X * Scale, Y * Scale, Z * Scale);
	}

	template <typename T> TVector<T> TVector<T>::operator*(TVector2<T> InVector2) const
	{
		return TVector(X * InVector2.X, Y * InVector2.Y, Z);
	}

	template <typename T> bool TVector<T>::operator==(const TVector& Other) const
	{
		return X == Other.X && Y == Other.Y && Z == Other.Z;
	}

	template <typename T> bool TVector<T>::operator!=(const TVector&) const
	{}

	template <typename T> TVector<T>& TVector<T>::operator+=(const TVector&) {}
	template <typename T> TVector<T>& TVector<T>::operator-=(const TVector&) {}
	template <typename T> TVector<T>& TVector<T>::operator*=(T Scale) {}
	template <typename T> TVector<T>& TVector<T>::operator*=(const TVector&) {}
	template <typename T> TVector<T>& TVector<T>::operator/=(T Scale) {}
	template <typename T> TVector<T>& TVector<T>::operator++() {}
	template <typename T> TVector<T>& TVector<T>::operator--() {}

	template <typename T> bool TVector<T>::IsNearlyZero(T Tolerance) const
	{
		return abs(X) <= Tolerance && abs(Y) <= Tolerance && abs(Z) <= Tolerance;
	}

	template <typename T> bool TVector<T>::IsNearlyEqual(const TVector& Other, T Tolerance) const
	{
		return operator-(Other).IsNearlyZero(Tolerance);
	}

	template <typename T> bool TVector<T>::IsZero() const
	{
		return X == 0.f && Y == 0.f && Z == 0.f;
	}

	template <typename T>
	TVector<T>::TVector() noexcept
		: X(0), Y(0), Z(0), XYZ{0} {}

	template <typename T>
	constexpr TVector<T>::TVector(T InF)
		: X(InF),
		  Y(InF),
		  Z(InF)
	{}

	template <typename T>
	constexpr TVector<T>::TVector(T InX, T InY, T InZ)
		: X(InX),
		  Y(InY),
		  Z(InZ)
	{}

	template <typename T>
	TVector<T>::TVector(const TVector2<T> V, T InZ)
	{}

	template <typename T> TVector<T>::TVector(const DirectX::XMFLOAT3& InVector)
		: X(InVector.x),
		  Y(InVector.y),
		  Z(InVector.z) {}

	template <typename T> TVector<T>::TVector(const DirectX::XMVECTORF32& InVector)
		: X(InVector.f[0]),
		  Y(InVector.f[1]),
		  Z(InVector.f[2]) {}

	template <typename T> TVector<T>& TVector<T>::operator=(const DirectX::XMVECTORF32& InVector)
	{
		X = InVector.f[0];
		Y = InVector.f[1];
		Z = InVector.f[2];

		return *this;
	}

	// template <typename T> TVector<T>::operator __m128() const
	// {
	// 	return DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)this);
	// }
}
