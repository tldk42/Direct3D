#include "common_pch.h"
#include "XShader.h"
#include <d3dcompiler.h>

#include "InputLayouts.h"
#include "Debug/Assert.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Texture/XTexture2D.h"

HRESULT LoadVertexShader(ID3D11Device* Device, const JWText& VertexFileName, ID3D11VertexShader** VertexShader,
						 ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(VertexFileName.c_str(),
								   EntryPoint,
								   "vs_5_0",
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

HRESULT LoadPixelShader(ID3D11Device* Device, const JWText& PixelFileName, ID3D11PixelShader** pixelShader,
						ID3DBlob**    OutBlob, LPCSTR       EntryPoint)
{
	ID3DBlob* blob;
	HRESULT   result = CompileShader(PixelFileName.c_str(),
								   EntryPoint,
								   "ps_5_0",
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

HRESULT LoadGeometryShader(ID3D11Device* Device, const JWText& ShaderFile, ID3D11GeometryShader** geometryShader,
						   ID3DBlob**    OutBlob, LPCSTR       FuncName)
{
	ID3DBlob* blob;

	HRESULT result = CompileShader(ShaderFile.c_str(),
								   FuncName ? FuncName : "GS",
								   "gs_5_0",
								   &blob);

	size_t  size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(Device->CreateGeometryShader(data, size, nullptr, geometryShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob->Release();
	}
	else
	{
		*OutBlob = blob;
	}

	return result;
}

HRESULT LoadHullShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11HullShader** hullShader,
						   ID3DBlob**    OutBlob, LPCSTR       FuncName)
{
	ID3DBlob* blob;

	HRESULT result = CompileShader(ShaderFile.c_str(),
								   FuncName ? FuncName : "HS",
								   "hs_5_0",
								   &blob);

	size_t  size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(Device->CreateHullShader(data, size, nullptr, hullShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob->Release();
	}
	else
	{
		*OutBlob = blob;
	}

	return result;
}

HRESULT LoadDomainShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11DomainShader** domainShader,
							 ID3DBlob**    OutBlob, LPCSTR       FuncName)
{
	ID3DBlob* blob;

	HRESULT result = CompileShader(ShaderFile.c_str(),
								   FuncName ? FuncName : "DS",
								   "ds_5_0",
								   &blob);

	size_t  size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(Device->CreateDomainShader(data, size, nullptr, domainShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob->Release();
	}
	else
	{
		*OutBlob = blob;
	}

	return result;
}

HRESULT LoadComputeShaderFile(ID3D11Device* Device, const JWText& ShaderFile, ID3D11ComputeShader** computeShader,
							  ID3DBlob**    OutBlob, LPCSTR       FuncName)
{
	ID3DBlob* blob;

	HRESULT result = CompileShader(ShaderFile.c_str(),
								   FuncName ? FuncName : "CS",
								   "cs_5_0",
								   &blob);

	size_t  size = blob->GetBufferSize();
	LPCVOID data = blob->GetBufferPointer();

	if (FAILED(Device->CreateComputeShader(data, size, nullptr, computeShader)))
	{
		blob = nullptr;
		return result;
	}

	if (!OutBlob)
	{
		blob->Release();
	}
	else
	{
		*OutBlob = blob;
	}

	return result;
}


HRESULT CompileShader(const WCHAR* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** OutBlob)
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

XShader::XShader(const JWText& InShaderFile, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
	: mShaderFile(InShaderFile)
{
	CheckResult(
				LoadVertexShader(
								 G_Context.GetDevice(),
								 mShaderFile,
								 mVertexShader.GetAddressOf(),
								 mVertexShaderBuf.GetAddressOf(),
								 VSEntryPoint
								));
	CheckResult(
				LoadPixelShader(
								G_Context.GetDevice(),
								mShaderFile,
								mPixelShader.GetAddressOf(),
								mPixelShaderBuf.GetAddressOf(),
								PSEntryPoint
							   ));

	CheckResult(
				LoadGeometryShader(
								   G_Context.GetDevice(),
								   mShaderFile,
								   mGeometryShader.GetAddressOf(),
								   mGeometryShaderBuf.GetAddressOf()));

	CheckResult(
				LoadHullShaderFile(
								   G_Context.GetDevice(),
								   mShaderFile,
								   mHullShader.GetAddressOf(),
								   mHullShaderBuf.GetAddressOf()));

	CheckResult(
				LoadDomainShaderFile(
									 G_Context.GetDevice(),
									 mShaderFile,
									 mDomainShader.GetAddressOf(),
									 mDomainShaderBuf.GetAddressOf()));

	CheckResult(
				LoadComputeShaderFile(
									  G_Context.GetDevice(),
									  mShaderFile,
									  mComputeShader.GetAddressOf(),
									  mComputeShaderBuf.GetAddressOf()));

	HandleLayout();
}


XShader::~XShader()
{
	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
}

void XShader::Release()
{

	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
	mInputLayout     = nullptr;
}

void XShader::PreRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();

	deviceContext->IASetInputLayout(mInputLayout.Get());

	// deviceContext->IASetVertexBuffers(0, 1, mVertexShaderBuf.GetAddressOf(), )
	// deviceContext->IASetIndexBuffer();
	// deviceContext->VSSetConstantBuffers();
	// deviceContext->PSSetConstantBuffers();

	deviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
	deviceContext->GSSetShader(mGeometryShader.Get(), nullptr, 0);
	deviceContext->HSSetShader(mHullShader.Get(), nullptr, 0);
	deviceContext->DSSetShader(mDomainShader.Get(), nullptr, 0);

	// deviceContext->PSSetShaderResources(0, 1, mTexture->GetSRV());

}

void XShader::Render()
{
	PreRender();
	PostRender();
}

void XShader::PostRender() {}

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
															 mInputLayout.GetAddressOf()
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
															 mInputLayout.GetAddressOf()
															));
	}


}
