#pragma once
#include "Graphics/graphics_common_include.h"

class XTexture2D : public ICoreInterface
{
public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

private:
	ComPtr<ID3D11Resource>           mTextureResource;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	ComPtr<ID3D11Texture2D>          mTexture;
	ComPtr<ID3D11RenderTargetView>   mRenderTargetView;
	ComPtr<ID3D11DepthStencilView>   mDepthStencilView;

	D3D11_SHADER_RESOURCE_VIEW_DESC mSRVDesc;
	D3D11_TEXTURE2D_DESC            mTextureDesc;
};
