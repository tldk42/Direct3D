#include "common_pch.h"
#include "Vector.h"
/*
  XMMATRIX : 새로운 행렬을 만들 때 사용 (SIMD 연산 가능)
  XMMatrixSet : 행렬을 만들고 값을 초기화할 때 사용
  XMMatrixIdentity : 단위행렬 생성
  XMFLOAT4X4 : 4*4구조체를 이용해 행렬의 원소들을 접근하면서 사용 가능
  XMMatrixMultiply : 행렬을 곱함
  XMMatrixInverse : 역행렬을 만듬
  XMMatrixTranspose : 전치행렬로 바꿈 (4,1 원소가 1,4가 되게 하는게 전치행렬)
*/

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
