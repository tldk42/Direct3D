#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"


/**
 * 화면에 뿌려지는 모든 데이터는 이 오브젝트를 컴포넌트로 가지거나 상속받는다
 */
class JDXObject : public IRenderable
{
public:
	explicit JDXObject();
	explicit JDXObject(class CFBXObj* InFbxObj);
	explicit JDXObject(const JWText& InShaderFile);
	explicit JDXObject(CFBXObj* InFbxObj, const JWText& InShaderFile);
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
	[[nodiscard]] FORCEINLINE ID3D11Buffer*            GetCBuffer() const { return mConstantBuffer_Space.Get(); }
	[[nodiscard]] FORCEINLINE D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveType() const { return mPrimitiveType; }

	void SetShaderFile(JWTextView InFileName);
	void SetShaderFile(const JWText& InFileName);

private:
	void CreateBuffers();

private:
	JWText            mShaderFile;
	class JShader*    mShader;
	class JTexture2D* mTexture;


	// -------------------------------- Buffers --------------------------------------
	ComPtr<ID3D11Buffer> mVertexBuffer;			// Vertex 
	ComPtr<ID3D11Buffer> mIndexBuffer;			// Index
	ComPtr<ID3D11Buffer> mConstantBuffer_Space; // World, View, Projection
	ComPtr<ID3D11Buffer> mConstantBuffer_Light; // Light Direction

	// ----------------------------- Model Primitive Data -----------------------------
	std::vector<Ptr<JData<Vertex::FVertexInfo_Base>>> mPrimitiveModelData;
	std::vector<Ptr<class JMesh>>                     mPrimitiveMeshData;

	uint32_t mVertexNum;
	uint32_t mIndexNum;
	uint32_t mVertexSize;
	uint32_t mIndexSize;
	uint32_t mVertexBegin;
	uint32_t mIndexBegin;

	D3D11_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};
