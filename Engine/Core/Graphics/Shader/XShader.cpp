#include "common_pch.h"
#include "XShader.h"
#include <d3dcompiler.h>

#include "InputLayouts.h"
#include "Debug/Assert.h"
#include "Core/Graphics/GraphicDevice.h"

XShader::XShader(const JWText& InShaderFile, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: mShaderFile(InShaderFile)
{
	CheckResult(
				LoadVertexShader(
								 G_Context.GetDevice()
								 , mShaderFile,
								 mVertexShader.GetAddressOf()
								 , mVertexShaderBuf.GetAddressOf(),
								 VSEntryPoint
								));
	CheckResult(
				LoadPixelShader(
								G_Context.GetDevice(),
								mShaderFile,
								mPixelShader.GetAddressOf(),
								nullptr,
								PSEntryPoint
							   ));

	HandleLayout();
}


XShader::~XShader()
{
	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
}

void XShader::Update() {}

void XShader::Render()
{
	G_Context.GetImmediateDeviceContext()->IASetInputLayout(mVertexLayout.Get());


	G_Context.GetImmediateDeviceContext()->VSSetShader(mVertexShader.Get(), nullptr, 0);
	G_Context.GetImmediateDeviceContext()->PSSetShader(mPixelShader.Get(), nullptr, 0);
}

void XShader::Release()
{

	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
	mVertexLayout    = nullptr;
}

void XShader::HandleLayout()
{
	uint32_t hash = StringHash(mShaderFile.c_str());

	if (hash == HASH_INPUT_LAYOUT_STATIC_MESH)
	{
		CheckResult(
					G_Context.GetDevice()->CreateInputLayout(
															 InputLayout::ALPHABLEND_LAYOUT,
															 ARRAYSIZE(InputLayout::ALPHABLEND_LAYOUT),
															 mVertexShaderBuf->GetBufferPointer(),
															 mVertexShaderBuf->GetBufferSize(),
															 mVertexLayout.GetAddressOf()
															));
	}
	else if (hash == HASH_INPUT_LAYOUT_SKELETAL_MESH)
	{
		CheckResult(
					G_Context.GetDevice()->CreateInputLayout(
															 InputLayout::FBX_MODEL_LAYOUT,
															 ARRAYSIZE(InputLayout::FBX_MODEL_LAYOUT),
															 mVertexShaderBuf->GetBufferPointer(),
															 mVertexShaderBuf->GetBufferSize(),
															 mVertexLayout.GetAddressOf()
															));
	}


}

HRESULT XShader::LoadVertexShader(ID3D11Device* Device, const JWText& VertexFileName, ID3D11VertexShader** VertexShader,
								  ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(VertexFileName.c_str(), EntryPoint, "vs_5_0",
								   &blob);
	if (FAILED(result))
	{
		return result;
	}

	DWORD   size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(result = Device->CreateVertexShader(data, size, nullptr, VertexShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob = nullptr;
	}
	else
	{
		*OutBlob = blob;
	}
	return result;
}

HRESULT XShader::LoadPixelShader(ID3D11Device* Device, const JWText& PixelFileName, ID3D11PixelShader** pixelShader,
								 ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(PixelFileName.c_str(), EntryPoint, "ps_5_0",
								   &blob);
	if (FAILED(result))
	{
		return result;
	}

	DWORD   size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(result = Device->CreatePixelShader(data, size, nullptr, pixelShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob = nullptr;
	}
	else
	{
		*OutBlob = blob;
	}
	return result;
}

HRESULT XShader::CompileShader(const WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** OutBlob)
{
	HRESULT result;

	DWORD ShaderFlag = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	ShaderFlag |= D3DCOMPILE_DEBUG;
#endif
	ComPtr<ID3DBlob> errorBlob;

	result = D3DCompileFromFile(
								FileName,    // 셰이더 파일명
								nullptr,     // 코드 안 매크로 배열 주소
								nullptr,     // ID3DInclude 인터페이스
								EntryPoint,  // main 진입점
								ShaderModel, // 셰이더 프로필
								ShaderFlag,  // 셰이더 컴파일 플래그
								0,           // 이펙트 옵션 컴파일 플래그
								OutBlob,     // 반환 될 blob
								errorBlob.GetAddressOf()// 컴파일 오류 및 경고 목록 저장
							   );

	if (FAILED(result))
	{
		if (errorBlob.Get())
		{
			OutputDebugStringA(static_cast<char*>(errorBlob->GetBufferPointer()));
		}
	}

	if (errorBlob.Get())
	{
		errorBlob = nullptr;
	}

	return result;
}
