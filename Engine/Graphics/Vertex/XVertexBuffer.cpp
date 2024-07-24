#include "common_pch.h"
#include "XVertexBuffer.h"
#include "Graphics/GraphicDevice.h"
#include "Graphics/ConstantBuffer/CWorldBuffer.h"
#include "Graphics/Shader/ShaderManager.h"

XVertexBuffer::XVertexBuffer()
	: mPrimitiveType(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST),
	  mTransform(XMMatrixIdentity())
{
	mDrawColor = FLinearColor::Black;

	mWorldBuffer = std::make_unique<CWorldBuffer>();

	mVertexBufferArray.reserve(3);
}

XVertexBuffer::~XVertexBuffer()
{
	XVertexBuffer::Release();
}

void XVertexBuffer::Initialize()
{
	mWorldBuffer->Initialize();

	CreateVertexArray();
	CreateVertexBuffer();

	CreateIndexArray();
	CreateIndexBuffer();

	LoadShader(L"Shader/alphablend.hlsl");
}

void XVertexBuffer::Update(float DeltaTime)
{
	mWorldBuffer->SetWorldMatrix(mTransform);
	mWorldBuffer->Update(DeltaTime);
}

void XVertexBuffer::Render()
{
	constexpr UINT stride = sizeof(FVertexInfo);
	constexpr UINT offset = 0;

	mWorldBuffer->Render();
	mShader->Render();

	G_Context.GetDeviceContext()->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	G_Context.GetDeviceContext()->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	G_Context.GetDeviceContext()->IASetPrimitiveTopology(mPrimitiveType);

	// G_Context.GetDeviceContext()->Draw(3, 0);
	G_Context.GetDeviceContext()->DrawIndexed(mIndices.size(), 0, 0);

	
}

void XVertexBuffer::Release()
{
	mIndexBuffer  = nullptr;
	mVertexBuffer = nullptr;
	mShader       = nullptr;
	mWorldBuffer  = nullptr;
}

void XVertexBuffer::CreateVertexBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	{
		bufferDesc.ByteWidth      = std::size(mVertexBufferArray) * sizeof(FVertexInfo); // 버퍼크기
		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;	// 버퍼의 읽기/쓰기 방법 지정
		bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 파이프라인에 바인딩될 방법
		bufferDesc.CPUAccessFlags = 0; // 생성될 버퍼에 CPU가 접근하는 유형 (DX 성능에 매우 중요)
		bufferDesc.MiscFlags      = 0; // 추가적인 옵션 플래그
	}

	D3D11_SUBRESOURCE_DATA initData;
	{
		initData.pSysMem = mVertexBufferArray.data(); // 초기화 데이터 포인터 (정점 배열의 주소를 넘겨준다)
		// InitData.SysMemPitch (텍스처 리소스의 한줄의 크기)
		// InitData.SysMemSlicePitch (3차원 텍스처의 깊이 간격)
	}

	CheckResult(
		G_Context.GetDevice()->CreateBuffer(
			&bufferDesc,
			&initData,
			mVertexBuffer.GetAddressOf()
		));
}

void XVertexBuffer::CreateIndexBuffer()
{
	const UINT numIndex = mIndices.size();

	D3D11_BUFFER_DESC initData;
	{
		initData.ByteWidth      = numIndex * sizeof(WORD);
		initData.Usage          = D3D11_USAGE_DEFAULT;
		initData.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		initData.CPUAccessFlags = 0;
		initData.MiscFlags      = 0;
	}

	D3D11_SUBRESOURCE_DATA ibInitData;
	ZeroMemory(&ibInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	ibInitData.pSysMem = mIndices.data();

	CheckResult(
		G_Context.GetDevice()->CreateBuffer(
			&initData,
			&ibInitData,
			mIndexBuffer.GetAddressOf()
		));
}

void XVertexBuffer::CreateVertexArray()
{
	mVertexBufferArray =
	{
		FVertexInfo{FVector(-.5f, -.5f, .5f), mDrawColor, {0, 0}},
		FVertexInfo{FVector(.0f, .5f, .5f), mDrawColor, {1, 0}},
		FVertexInfo{FVector(.5f, -.5f, .5f), mDrawColor, {1, 1}},
	};
}

void XVertexBuffer::CreateIndexArray()
{
	mIndices =
	{
		0, 1, 2
	};
}

void XVertexBuffer::LoadShader(WTextView InShaderFileName)
{
	//TODO: Load Shader
	mShader = Manager_Shader.CreateOrLoad<XShader>(L"Shader/alphablend.hlsl");
}
