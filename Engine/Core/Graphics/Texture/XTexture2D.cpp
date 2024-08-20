#include "common_pch.h"
#include "XTexture2D.h"
#include "Core/Graphics/GraphicDevice.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Utils/Utils.h"

XTexture2D::XTexture2D(JWTextView InName)
	: mID(StringHash(InName.data())),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{
	mSlot = 0;

	XTexture2D::Initialize();
}

XTexture2D::~XTexture2D() = default;

void XTexture2D::Initialize()
{
	CheckResult(CreateWICTextureFromFile(G_Context.GetDevice(), mTextureName.c_str(),
										 mTextureResource.GetAddressOf(),
										 mShaderResourceView.GetAddressOf()));
	CheckResult(mTextureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												 reinterpret_cast<void**>(mTexture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	mTexture->GetDesc(&mTextureDesc);
}

void XTexture2D::Update(float DeltaTime) {}

void XTexture2D::Release()
{
	mShaderResourceView = nullptr;
	mTextureResource    = nullptr;
	mTexture            = nullptr;
}

void XTexture2D::PreRender()
{
	G_Context.GetImmediateDeviceContext()->PSSetShaderResources(mSlot, 1, mShaderResourceView.GetAddressOf());
}

void XTexture2D::Render() {}
void XTexture2D::PostRender() {}

ELayerType XTexture2D::GetLayerType()
{
	return ELayerType::UI;
}
