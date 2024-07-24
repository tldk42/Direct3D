#pragma once
#include "Vector2D.h"

namespace Math
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

		inline TVector operator*(T Scale) const;
		inline TVector operator*(TVector2<T> InVector2) const;

	public:
		TVector();
		TVector(T InF);
		TVector(T InX, T InY, T InZ);

		explicit TVector(const TVector2<T> V, T InZ);
	};

	template <typename T> TVector<T> TVector<T>::operator*(T Scale) const
	{
		return TVector(X * Scale, Y * Scale, Z * Scale);
	}

	template <typename T> TVector<T> TVector<T>::operator*(TVector2<T> InVector2) const
	{
		return TVector(X * InVector2.X, Y * InVector2.Y, Z);
	}

	template <typename T>
	TVector<T>::TVector()
	{}

	template <typename T>
	TVector<T>::TVector(T InF)
		: X(InF),
		  Y(InF)
	{}

	template <typename T>
	TVector<T>::TVector(T InX, T InY, T InZ)
		: X(InX),
		  Y(InY),
		  Z(InZ)
	{}

	template <typename T>
	TVector<T>::TVector(const TVector2<T> V, T InZ)
	{}
}
