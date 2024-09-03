#include "common_pch.h"
#include "JTexture2D.h"
#include "Core/Graphics/GraphicDevice.h"
#include <directxtk/WICTextureLoader.h>

#include "Core/Utils/Utils.h"

JTexture2D::JTexture2D(JWTextView InName)
	: mID(StringHash(InName.data())),
	  mTextureName(InName),
	  mSRVDesc(),
	  mTextureDesc()
{
	mSlot = 0;
	LoadFromFile();
}

void JTexture2D::PreRender()
{
	G_Context.GetImmediateDeviceContext()->PSSetShaderResources(mSlot, 1, mShaderResourceView.GetAddressOf());
}

void JTexture2D::Render() {}
void JTexture2D::PostRender() {}

void JTexture2D::LoadFromFile()
{
	ComPtr<ID3D11Resource>  textureResource;
	ComPtr<ID3D11Texture2D> texture;


	CheckResult(CreateWICTextureFromFile(G_Context.GetDevice(),
										 mTextureName.c_str(),
										 textureResource.GetAddressOf(),
										 mShaderResourceView.GetAddressOf()));

	CheckResult(textureResource->QueryInterface(__uuidof(ID3D11Texture2D),
												reinterpret_cast<void**>(texture.GetAddressOf())));

	mShaderResourceView->GetDesc(&mSRVDesc);
	texture->GetDesc(&mTextureDesc);

	textureResource = nullptr;
	texture         = nullptr;
}
