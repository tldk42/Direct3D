#pragma once
#include "Graphics/graphics_common_include.h"
#include "Utils/Math/Color.h"
#include "Utils/Math/Vector.h"

struct FVertexInfo
{
	FVector      Pos;
	FLinearColor Color;
	FVector2D    Tex;
};

class XVertexBuffer : public ICoreInterface
{
public:
	XVertexBuffer();
	~XVertexBuffer() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

protected:
	virtual void CreateVertexBuffer();
	virtual void CreateIndexBuffer();
	virtual void CreateVertexArray();
	virtual void CreateIndexArray();
	virtual void LoadShader(WTextView InShaderFileName);

private:
	ComPtr<ID3D11Buffer> mVertexBuffer; /** 정점 배열 */
	ComPtr<ID3D11Buffer> mIndexBuffer;  /** 정점 인덱스 */

	D3D_PRIMITIVE_TOPOLOGY mPrimitiveType; /** 그려질 타입 */
	FLinearColor           mDrawColor;     /** 텍스처를 안쓸때 지정 컬러 */

	std::vector<FVertexInfo> mVertexBufferArray;
	std::vector<WORD>        mIndices;

	XShader*         mShader;
	CWorldBufferUPtr mWorldBuffer;   /** 월드 버퍼 */
	
	Matrix           mTransform;
	
};
