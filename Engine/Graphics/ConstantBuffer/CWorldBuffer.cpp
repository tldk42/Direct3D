#include "common_pch.h"
#include "CWorldBuffer.h"
#include "Graphics/GraphicDevice.h"


void CWorldBuffer::Initialize()
{
	CreateConstantBuffer();
}

void CWorldBuffer::Update(float DeltaTime)
{
	G_Context.GetDeviceContext()->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &mWorldMatrix, 0, 0);
}

void CWorldBuffer::Render()
{
	G_Context.GetDeviceContext()->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
}

void CWorldBuffer::Release()
{
	mConstantBuffer = nullptr;
}

void CWorldBuffer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantBufferDesc{};
	{
		constantBufferDesc.ByteWidth      = sizeof(XMMATRIX);
		constantBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = 0;
	}

	CheckResult(G_Context.GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBuffer.GetAddressOf()));
}
