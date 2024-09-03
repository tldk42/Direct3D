#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/IRenderable.h"


/**
 * 화면에 뿌려지는 모든 데이터는 이 오브젝트를 컴포넌트로 가지거나 상속받는다
 */
class JDXObject : public IRenderable
{
public:
	explicit JDXObject(const JWText& InShaderFile);
	~JDXObject();

public:
	void Release();

#pragma region Render Interface
	void PreRender() override;
	void Render() override;
	void PostRender() override;
#pragma endregion

public:
	void UpdateVertexData(void*          InSrcData, const uint32_t InVertexNum, const uint32_t InVertexSize,
						  const uint32_t InVertexBegin);
	void UpdateIndexData(void* InSrcData, const uint32_t InIndexNum, const uint32_t InIndexBegin);

public:
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetVertexBuffer() const { return mVertexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetIndexBuffer() const { return mIndexBuffer.Get(); }
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetCBuffer() const { return mConstantBuffer.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return mPrimitiveType; }

private:
	JWText            mShaderFile;
	class JShader*    mShader;
	class JTexture2D* mTexture;

	ComPtr<ID3D11Buffer> mVertexBuffer;
	ComPtr<ID3D11Buffer> mIndexBuffer;
	ComPtr<ID3D11Buffer> mConstantBuffer;

	uint32_t mVertexNum;
	uint32_t mIndexNum;
	uint32_t mVertexSize;
	uint32_t mIndexSize;
	uint32_t mVertexBegin;
	uint32_t mIndexBegin;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType;
};
