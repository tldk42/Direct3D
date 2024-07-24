#include "common_pch.h"
#include "CProjectionBuffer.h"

#include "Graphics/GraphicDevice.h"

void CProjectionBuffer::Initialize()
{}

void CProjectionBuffer::Update(float DeltaTime)
{
	G_Context.GetDeviceContext()->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &mProjectionMatrix, 0, 0);
}

void CProjectionBuffer::Render()
{
	G_Context.GetDeviceContext()->VSSetConstantBuffers(2, 1, mConstantBuffer.GetAddressOf());
}

void CProjectionBuffer::Release()
{
	mConstantBuffer = nullptr;
}

void CProjectionBuffer::CreateConstantBuffer()
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
