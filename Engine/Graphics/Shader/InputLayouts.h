#pragma once
#include <d3d11.h>

constexpr D3D11_INPUT_ELEMENT_DESC Position =
{
	"POSITION",                  // 셰이더 입력 서명에서 이 요소와 연결된 의미체계 
	0,                           // 의미상 인덱스
	DXGI_FORMAT_R32G32B32_FLOAT, // 데이터 형식 (float2)
	0,                           // 입력 어셈블러 식별정수
	0,                           // 요소 사이 오프셋 
	D3D11_INPUT_PER_VERTEX_DATA, // 단일 입력 슬롯 입력 데이터 클래스
	0                            // 정점 버퍼에서 렌더링 되는 인스턴스의 수 (D3D11_INPUT_PER_VERTEX_DATA -> 0)
};
constexpr D3D11_INPUT_ELEMENT_DESC Color =
{
	"COLOR",
	0,
	DXGI_FORMAT_R32G32B32A32_FLOAT,
	0,
	D3D11_APPEND_ALIGNED_ELEMENT,
	D3D11_INPUT_PER_VERTEX_DATA,
	0
};

constexpr D3D11_INPUT_ELEMENT_DESC Texture =
{
	"TEX",
	0,
	DXGI_FORMAT_R32G32_FLOAT,
	0,
	D3D11_APPEND_ALIGNED_ELEMENT,
	D3D11_INPUT_PER_VERTEX_DATA,
	0
};

constexpr D3D11_INPUT_ELEMENT_DESC ALPHABLEND_LAYOUT[] =
{
	Position,
	Color,
	Texture
};


