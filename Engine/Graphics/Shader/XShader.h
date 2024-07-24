#pragma once
#include "Graphics/graphics_common_include.h"
#include "common_include.h"

class XShader : public ICoreInterface
{
public:
	explicit XShader(const WText& InVertexShader);
	explicit XShader(const WText& InVertexShader, const WText& InPixelShader);
	~XShader() override;

public:
#pragma region Core Interface
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Render() override;
	void Release() override;
#pragma endregion

private:
	static HRESULT LoadVertexShader(ID3D11Device*        Device, const WText& VertexFileName,
									ID3D11VertexShader** VertexShader,
									ID3DBlob**           OutBlob = nullptr);
	static HRESULT LoadPixelShader(ID3D11Device*       Device, const WText& PixelFileName,
								   ID3D11PixelShader** pixelShader,
								   ID3DBlob**          OutBlob = nullptr);

	static HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
								 LPCSTR       ShaderModel, ID3DBlob** OutBlob);

private:
	WText mVertexShaderFile;
	WText mPixelShaderFile;

#pragma region Shader
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader>  mPixelShader;
	ComPtr<ID3DBlob>           mVertexShaderBuf;
	ComPtr<ID3D11InputLayout>  mVertexLayout;
#pragma endregion
};
