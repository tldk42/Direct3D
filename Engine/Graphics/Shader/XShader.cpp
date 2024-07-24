#include "common_pch.h"
#include "XShader.h"
#include <d3dcompiler.h>

#include "InputLayouts.h"
#include "Debug/Assert.h"
#include "Graphics/GraphicDevice.h"

XShader::XShader(const WText& InVertexShader)
	: mVertexShaderFile(InVertexShader),
	  mPixelShaderFile(InVertexShader)
{
	XShader::Initialize();
}

XShader::XShader(const WText& InVertexShader, const WText& InPixelShader)
	: mVertexShaderFile(InVertexShader),
	  mPixelShaderFile(InPixelShader)
{
	XShader::Initialize();

}

XShader::~XShader()
{
	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
}

void XShader::Initialize()
{
	CheckResult(
		LoadVertexShader(
			G_Context.GetDevice()
			, mVertexShaderFile,
			mVertexShader.GetAddressOf()
			, mVertexShaderBuf.GetAddressOf()
		));
	CheckResult(
		LoadPixelShader(
			G_Context.GetDevice(),
			mPixelShaderFile,
			mPixelShader.GetAddressOf()
		));

	CheckResult(
		G_Context.GetDevice()->CreateInputLayout(
			ALPHABLEND_LAYOUT,
			ARRAYSIZE(ALPHABLEND_LAYOUT),
			mVertexShaderBuf->GetBufferPointer(),
			mVertexShaderBuf->GetBufferSize(),
			mVertexLayout.GetAddressOf()
		));
}

void XShader::Update(float_t DeltaTime) {}

void XShader::Render()
{
	G_Context.GetDeviceContext()->IASetInputLayout(mVertexLayout.Get());


	G_Context.GetDeviceContext()->VSSetShader(mVertexShader.Get(), nullptr, 0);
	G_Context.GetDeviceContext()->PSSetShader(mPixelShader.Get(), nullptr, 0);
}

void XShader::Release()
{

	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
	mVertexLayout    = nullptr;
}

HRESULT XShader::LoadVertexShader(ID3D11Device* Device, const WText& VertexFileName, ID3D11VertexShader** VertexShader,
								  ID3DBlob**    OutBlob)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(VertexFileName.c_str(), "vs", "vs_5_0",
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

HRESULT XShader::LoadPixelShader(ID3D11Device* Device, const WText& PixelFileName, ID3D11PixelShader** pixelShader,
								 ID3DBlob**    OutBlob)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(PixelFileName.c_str(), "ps", "ps_5_0",
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
