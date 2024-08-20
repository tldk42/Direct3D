#pragma once
#include "Core/Interface/ICoreInterface.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/IRenderable.h"

class XTexture2D : public ICoreInterface, public IRenderable
{
public:
	explicit XTexture2D(JWTextView InName);
	~XTexture2D();

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Release() override;
#pragma endregion

#pragma region Render Interface
	void            PreRender() override;
	void            Render() override;
	void            PostRender() override;
	ELayerType GetLayerType() override;
#pragma endregion

public:
	[[nodiscard]] FORCEINLINE ID3D11ShaderResourceView* GetSRV() const { return mShaderResourceView.Get(); }

private:
	uint32_t mSlot;
	uint32_t mID;
	JWText   mTextureName;

	ComPtr<ID3D11Resource>           mTextureResource;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	ComPtr<ID3D11Texture2D>          mTexture;

	D3D11_SHADER_RESOURCE_VIEW_DESC mSRVDesc;
	D3D11_TEXTURE2D_DESC            mTextureDesc;
};
