#include "common_pch.h"
#include "CViewBuffer.h"

#include "Graphics/GraphicDevice.h"


void CViewBuffer::Initialize() {}

void CViewBuffer::Update(float DeltaTime)
{
	G_Context.GetImmediateDeviceContext()->UpdateSubresource(mConstantBuffer.Get(), 0, nullptr, &mViewMatrix, 0, 0);
}

void CViewBuffer::Render()
{
	G_Context.GetImmediateDeviceContext()->VSSetConstantBuffers(1, 1, mConstantBuffer.GetAddressOf());
}

void CViewBuffer::Release()
{
	mConstantBuffer = nullptr;
}

void CViewBuffer::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC constantBufferDesc{};
	{
		constantBufferDesc.ByteWidth      = sizeof(Matrix);
		constantBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = 0;
	}


	CheckResult(G_Context.GetDevice()->CreateBuffer(&constantBufferDesc, nullptr, mConstantBuffer.GetAddressOf()));
}
