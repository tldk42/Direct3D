#include "common_pch.h"
#include "XTexture2D.h"
#include "Graphics/GraphicDevice.h"
#include <directxtk/WICTextureLoader.h>

void XTexture2D::Initialize()
{
	CheckResult(CreateWICTextureFromFile(G_Context.GetDevice(), L"", mTextureResource.GetAddressOf(),
										 mShaderResourceView.GetAddressOf()));
	CheckResult(mTextureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												 reinterpret_cast<void**>(mTexture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	mTexture->GetDesc(&mTextureDesc);

	D3D11_SAMPLER_DESC sampler_desc = {};
	sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampler_desc.MinLOD = 0.0f;
	sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
}

void XTexture2D::Update(float DeltaTime) {}

void XTexture2D::Render()
{
	// G_Context.GetDeviceContext()->PSSetShaderResources(mSlot, 1, mSRV.GetAddressOf());

}

void XTexture2D::Release() {}
