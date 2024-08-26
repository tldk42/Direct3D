#pragma once
#include "Core/Graphics/graphics_common_include.h"

class XShader
{
public:
	explicit XShader(const JWText& InShaderFile, LPCSTR VSEntryPoint = "VS", LPCSTR PSEntryPoint = "PS");
	~XShader();

public:
#pragma region Core Interface
	void Update();
	void Render();
	void Release();
#pragma endregion

private:
	void HandleLayout();

	static HRESULT LoadVertexShader(ID3D11Device*        Device, const JWText& VertexFileName,
									ID3D11VertexShader** VertexShader,
									ID3DBlob**           OutBlob = nullptr, LPCSTR EntryPoint = nullptr);
	static HRESULT LoadPixelShader(ID3D11Device*       Device, const JWText& PixelFileName,
								   ID3D11PixelShader** pixelShader,
								   ID3DBlob**          OutBlob = nullptr, LPCSTR EntryPoint = nullptr);

	static HRESULT CompileShader(const WCHAR* FileName, LPCSTR        EntryPoint,
								 LPCSTR       ShaderModel, ID3DBlob** OutBlob);

private:
	JWText mShaderFile;

#pragma region Shader
	ComPtr<ID3D11VertexShader> mVertexShader;
	ComPtr<ID3D11PixelShader>  mPixelShader;
	ComPtr<ID3DBlob>           mVertexShaderBuf;
	ComPtr<ID3D11InputLayout>  mVertexLayout;
#pragma endregion
};
