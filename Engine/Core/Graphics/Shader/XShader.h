#pragma once
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Interface/ICoreInterface.h"

class XShader : public ICoreInterface
{
public:
	explicit XShader(const JWText& InVertexShader);
	explicit XShader(const JWText& InVertexShader, const JWText& InPixelShader);
	~XShader() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Render();
	void Release() override;
#pragma endregion

private:
	static HRESULT LoadVertexShader(ID3D11Device*        Device, const JWText& VertexFileName,
									ID3D11VertexShader** VertexShader,
									ID3DBlob**           OutBlob = nullptr);
	static HRESULT LoadPixelShader(ID3D11Device*       Device, const JWText& PixelFileName,
								   ID3D11PixelShader** pixelShader,
								   ID3DBlob**          OutBlob = nullptr);

	static HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
								 LPCSTR       ShaderModel, ID3DBlob** OutBlob);

private:
	JWText mVertexShaderFile;
	JWText mPixelShaderFile;

#pragma region Shader
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader>  mPixelShader;
	ComPtr<ID3DBlob>           mVertexShaderBuf;
	ComPtr<ID3D11InputLayout>  mVertexLayout;
#pragma endregion
};
