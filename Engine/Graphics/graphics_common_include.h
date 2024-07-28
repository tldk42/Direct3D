#pragma once

#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <DirectXMath.h>

#include "Debug/Assert.h"

using namespace Microsoft::WRL;
using namespace DirectX;

/*
  XMMATRIX : 새로운 행렬을 만들 때 사용 (SIMD 연산 가능)
  XMMatrixSet : 행렬을 만들고 값을 초기화할 때 사용
  XMMatrixIdentity : 단위행렬 생성
  XMFLOAT4X4 : 4*4구조체를 이용해 행렬의 원소들을 접근하면서 사용 가능
  XMMatrixMultiply : 행렬을 곱함
  XMMatrixInverse : 역행렬을 만듬
  XMMatrixTranspose : 전치행렬로 바꿈 (4,1 원소가 1,4가 되게 하는게 전치행렬)
*/


using Matrix = XMMATRIX;

CLASS_PTR(CWorldBuffer)
CLASS_PTR(XShader)
