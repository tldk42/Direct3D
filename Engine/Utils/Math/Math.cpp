#include "common_pch.h"
#include "Vector.h"


template <> const FVector FVector::ZeroVector(0, 0, 0);
template <> const FVector FVector::OneVector(1, 1, 1);
template <> const FVector FVector::UpVector(0, 0, 1);
template <> const FVector FVector::DownVector(0, 0, -1);
template <> const FVector FVector::ForwardVector(1, 0, 0);
template <> const FVector FVector::BackwardVector(-1, 0, 0);
template <> const FVector FVector::RightVector(0, 1, 0);
template <> const FVector FVector::LeftVector(0, -1, 0);
template <> const FVector FVector::XAxisVector(1, 0, 0);
template <> const FVector FVector::YAxisVector(0, 1, 0);
template <> const FVector FVector::ZAxisVector(0, 0, 1);


template <> const FVector2D FVector2D::ZeroVector(0.f, 0.f);
template <> const FVector2D FVector2D::UnitVector(1.f, 1.f);
template <> const FVector2D FVector2D::Unit45Deg(M_INVERSE_SQRT_2, M_INVERSE_SQRT_2);
