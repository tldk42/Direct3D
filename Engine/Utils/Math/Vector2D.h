#pragma once

#include "MathUtility.h"

namespace Math
{
	template <typename T>
	struct TVector2
	{
		static_assert(std::is_floating_point_v<T>, "T must be floating point!!");

	public:
		using FReal = T;

		union
		{
			struct
			{
				T X;
				T Y;
			};

			T XY[2];
		};

		static const TVector2 ZeroVector;
		static const TVector2 UnitVector;
		static const TVector2 Unit45Deg;

		static inline TVector2 Zero() { return ZeroVector; }
		static inline TVector2 One() { return UnitVector; }
		static inline TVector2 UnitX() { return TVector2(1, 0); }
		static inline TVector2 UnitY() { return TVector2(0, 1); }

	public:
		TVector2() = default;
		TVector2(T InX, T InY);
		explicit TVector2(T InF);
		// explicit TVector2(const TVector<T>& V);

		inline TVector2 operator+(const TVector2& V) const;
		inline TVector2 operator-(const TVector2& V) const;
		inline TVector2 operator*(const TVector2& V) const;
		inline T        operator|(const TVector2& V) const;
		inline T        operator^(const TVector2& V) const;
		TVector2        operator/(const TVector2& V) const;

		inline TVector2 operator+(T A) const;
		inline TVector2 operator-(T A) const;
		inline TVector2 operator*(T Scale) const;
		TVector2        operator/(T Scale) const;

		inline bool operator==(const TVector2& V) const;
		inline bool operator!=(const TVector2& V) const;

		inline bool operator<(const TVector2& Other) const;
		inline bool operator<=(const TVector2& Other) const;
		inline bool operator>(const TVector2& Other) const;
		inline bool operator>=(const TVector2& Other) const;

		inline TVector2 operator+=(const TVector2& Other);
		inline TVector2 operator-=(const TVector2& Other);
		inline TVector2 operator*=(const TVector2& Other);
		TVector2        operator/=(const TVector2& Other);

		inline TVector2 operator*=(T Scale);
		TVector2        operator/=(T V);

	public:
		inline static T Distance(const TVector2& A, const TVector2& B);
		inline static T DistanceSquared(const TVector2& A, const TVector2& B);

		inline static T DotProduct(const TVector2& A, const TVector2& B);
		inline static T CrossProduct(const TVector2& A, const TVector2& B);

		inline static TVector2 Min(const TVector2& A, const TVector2& B);
		inline static TVector2 Max(const TVector2& A, const TVector2& B);

		T GetMax() const;
		T GetMax_Abs() const;

		T GetMin() const;
		T GetMin_Abs() const;

		inline T GetLength() const;
		inline T GetLengthSquared() const;

		bool     Normalize(T Tolerance = M_SMALL_NUMBER);
		TVector2 GetSafeNormal(T Tolerance = M_SMALL_NUMBER) const;

		bool IsNearlyZero(T Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsNearlyEqual(const TVector2<T>& Other, T Tolerance = M_KINDA_SMALL_NUMBER) const;
		bool IsZero() const;

		inline std::string ToString() const;
	};

	template <typename T>
	TVector2<T>::TVector2(T InX, T InY)
		: X(InX),
		  Y(InY) {}

	template <typename T>
	TVector2<T>::TVector2(T InF)
		: X(InF),
		  Y(InF) {}

	// template <typename T>
	// TVector2<T>::TVector2(const TVector<T>& V)
	// 	: X(V.X),
	// 	  Y(V.Y)
	//
	// {
	// }

	template <typename T>
	TVector2<T> TVector2<T>::operator+(const TVector2& V) const
	{
		return TVector2(X + V.X, Y + V.Y);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator-(const TVector2& V) const
	{
		return TVector2(X - V.X, Y - V.Y);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator*(const TVector2& V) const
	{
		return TVector2(X * V.X, Y * V.Y);
	}

	template <typename T>
	T TVector2<T>::operator|(const TVector2& V) const
	{
		return X * V.X + Y * V.Y;
	}

	template <typename T>
	T TVector2<T>::operator^(const TVector2& V) const
	{
		return X * V.Y - Y * V.X;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator/(const TVector2& V) const
	{
		return TVector2(X / V.X, Y / V.Y);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator+(T A) const
	{
		return TVector2(X + A, Y + A);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator-(T A) const
	{
		return TVector2(X - A, Y - A);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator*(T Scale) const
	{
		return TVector2(X * Scale, Y * Scale);
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator/(T Scale) const
	{
		return TVector2(X / Scale, Y / Scale);
	}

	template <typename T>
	bool TVector2<T>::operator==(const TVector2& V) const
	{
		return X == V.X && Y == V.Y;
	}

	template <typename T>
	bool TVector2<T>::operator!=(const TVector2& V) const
	{
		return X != V.X && Y != V.Y;
	}

	template <typename T>
	bool TVector2<T>::operator<(const TVector2& Other) const
	{
		return X < Other.X && Y < Other.Y;
	}

	template <typename T>
	bool TVector2<T>::operator<=(const TVector2& Other) const
	{
		return X <= Other.X && Y <= Other.Y;
	}

	template <typename T>
	bool TVector2<T>::operator>(const TVector2& Other) const
	{
		return X > Other.X && Y > Other.Y;
	}

	template <typename T>
	bool TVector2<T>::operator>=(const TVector2& Other) const
	{
		return X >= Other.X && Y >= Other.Y;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator+=(const TVector2& Other)
	{
		X += Other.X;
		Y += Other.Y;
		return *this;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator-=(const TVector2& Other)
	{
		X -= Other.X;
		Y -= Other.Y;
		return *this;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator*=(const TVector2& Other)
	{
		X *= Other.X;
		Y *= Other.Y;
		return *this;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator/=(const TVector2& Other)
	{
		X /= Other.X;
		Y /= Other.Y;
		return *this;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator*=(T Scale)
	{
		X += Scale;
		Y += Scale;
		return *this;
	}

	template <typename T>
	TVector2<T> TVector2<T>::operator/=(T V)
	{
		const T RV = 1.f / V;
		X *= RV;
		Y *= RV;
		return *this;
	}

	template <typename T>
	T TVector2<T>::Distance(const TVector2& A, const TVector2& B)
	{
		return sqrt(DistanceSquared(A, B));
	}

	template <typename T>
	T TVector2<T>::DistanceSquared(const TVector2& A, const TVector2& B)
	{
		return pow((B.X - A.X), 2) + pow((B.Y - A.Y), 2);
	}

	template <typename T>
	T TVector2<T>::DotProduct(const TVector2& A, const TVector2& B)
	{
		return A | B;
	}

	template <typename T>
	T TVector2<T>::CrossProduct(const TVector2& A, const TVector2& B)
	{
		return A ^ B;
	}

	template <typename T>
	TVector2<T> TVector2<T>::Min(const TVector2& A, const TVector2& B)
	{
		return TVector2();
	}

	template <typename T>
	TVector2<T> TVector2<T>::Max(const TVector2& A, const TVector2& B)
	{
		return X;
	}

	template <typename T>
	T TVector2<T>::GetMax() const
	{
		return X > Y ? X : Y;
	}

	template <typename T>
	T TVector2<T>::GetMax_Abs() const
	{
		return abs(X) > abs(Y) ? X : Y;
	}

	template <typename T>
	T TVector2<T>::GetMin() const
	{
		return X < Y ? X : Y;
	}

	template <typename T>
	T TVector2<T>::GetMin_Abs() const
	{
		return abs(X) < abs(Y) ? X : Y;
	}

	template <typename T>
	T TVector2<T>::GetLength() const
	{
		return sqrt(X * X + Y * Y);
	}

	template <typename T>
	T TVector2<T>::GetLengthSquared() const
	{
		return X * X + Y * Y;
	}

	template <typename T>
	bool TVector2<T>::Normalize(T Tolerance)
	{
		const T squareSum = GetLengthSquared();

		if (squareSum > Tolerance)
		{
			const T Scale = 1.f / sqrt(squareSum);
			X *= Scale;
			Y *= Scale;

			return true;
		}

		X = 0.f;
		Y = 0.f;

		return false;
	}

	template <typename T>
	TVector2<T> TVector2<T>::GetSafeNormal(T Tolerance) const
	{
		const T squareSum = GetLength();

		if (squareSum > Tolerance)
		{
			const T Scale = 1.f / squareSum;

			return TVector2(X * Scale, Y * Scale);
		}

		return ZeroVector;
	}

	template <typename T>
	bool TVector2<T>::IsNearlyZero(T Tolerance) const
	{
		return abs(X) <= Tolerance && abs(Y) <= Tolerance;
	}

	template <typename T>
	bool TVector2<T>::IsNearlyEqual(const TVector2<T>& Other, T Tolerance) const
	{

		return operator-(Other).IsNearlyZero(Tolerance);
	}

	template <typename T>
	bool TVector2<T>::IsZero() const
	{
		return X == 0.f && Y == 0.f;
	}

	template <typename T>
	std::string TVector2<T>::ToString() const
	{
		return std::format("X={0:3.3f} Y={1:3.3f}\n", X, Y);
	}
}
