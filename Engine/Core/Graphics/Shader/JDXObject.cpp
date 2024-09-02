#include "common_pch.h"
#include "JDXObject.h"
#include <d3dcompiler.h>

#include "InputLayouts.h"
#include "Debug/Assert.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/ShaderStructs.h"
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

void CreateVertexBuffer(void* InVertices, uint32_t InVertexNum, uint32_t InSize, ID3D11Buffer** OutVertexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc;
	{
		bufferDesc.ByteWidth      = InVertexNum * InSize; // 버퍼크기
		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;	// 버퍼의 읽기/쓰기 방법 지정
		bufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER; // 파이프라인에 바인딩될 방법
		bufferDesc.CPUAccessFlags = 0; // 생성될 버퍼에 CPU가 접근하는 유형 (DX 성능에 매우 중요)
		bufferDesc.MiscFlags      = 0; // 추가적인 옵션 플래그
	}

	D3D11_SUBRESOURCE_DATA vertexData;
	{
		vertexData.pSysMem = InVertices; // 초기화 데이터 포인터 (정점 배열의 주소를 넘겨준다)
		// InitData.SysMemPitch (텍스처 리소스의 한줄의 크기)
		// InitData.SysMemSlicePitch (3차원 텍스처의 깊이 간격)
	}

	CheckResult(
				G_Context.GetDevice()->CreateBuffer(
													&bufferDesc,
													&vertexData,
													OutVertexBuffer
												   ));
}

void CreateIndexBuffer(void* InIndices, uint32_t InIndexNum, uint32_t InSize, ID3D11Buffer** OutIndexBuffer)
{
	D3D11_BUFFER_DESC bufferDesc;
	{
		bufferDesc.ByteWidth      = InIndexNum * InSize;
		bufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags      = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags      = 0;
	}

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexData.pSysMem = InIndices;

	CheckResult(
				G_Context.GetDevice()->CreateBuffer(
													&bufferDesc,
													&indexData,
													OutIndexBuffer
												   ));
}

void CreateConstantBuffer(void* InData, uint32_t InIndexNum, uint32_t InSize, ID3D11Buffer** OutConstantBuffer)
{
	D3D11_BUFFER_DESC constantBufferDesc{};
	{
		constantBufferDesc.ByteWidth      = InIndexNum * InSize;
		constantBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		constantBufferDesc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA constantData;
	ZeroMemory(&constantData, sizeof(D3D11_SUBRESOURCE_DATA));
	constantData.pSysMem = InData;

	CheckResult(G_Context.GetDevice()->CreateBuffer(&constantBufferDesc, &constantData, OutConstantBuffer));
}

JDXObject::JDXObject(const JWText& InShaderFile, LPCSTR VSEntryPoint, LPCSTR PSEntryPoint)
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


	mVertexSize = sizeof(FVertexInfo_Simple);
	mIndexSize  = sizeof(DWORD);
	// mVertexNum = 
	// mIndexNum = 
	HandleLayout();

	void** vertexData = nullptr;
	vertexData = (void**)

	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateConstantBuffer();
}


JDXObject::~JDXObject()
{
	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
}

void JDXObject::Release()
{
	mVertexShader    = nullptr;
	mPixelShader     = nullptr;
	mVertexShaderBuf = nullptr;
	mInputLayout     = nullptr;
}

void JDXObject::PreRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);

	deviceContext->IASetPrimitiveTopology(mPrimitiveType);

	deviceContext->IASetInputLayout(mInputLayout.Get());

	uint32_t offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &mVertexSize, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	
	deviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());

	deviceContext->VSSetShader(mVertexShader.Get(), nullptr, 0);
	deviceContext->PSSetShader(mPixelShader.Get(), nullptr, 0);
	deviceContext->GSSetShader(mGeometryShader.Get(), nullptr, 0);
	deviceContext->HSSetShader(mHullShader.Get(), nullptr, 0);
	deviceContext->DSSetShader(mDomainShader.Get(), nullptr, 0);

	mTexture->PreRender();
}

void JDXObject::Render()
{}

void JDXObject::PostRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();

	if (mIndexNum != 0)
	{
		deviceContext->DrawIndexed(mIndexNum, 0, 0);
	}
	else
	{
		deviceContext->Draw(mVertexNum, 0);
	}
}

ELayerType JDXObject::GetLayerType()
{
	return ELayerType::End;
}

void JDXObject::SetVertexShader(JWTextView InFile, LPCSTR FuncName)
{
	CheckResult(
				LoadVertexShader(
								 G_Context.GetDevice(),
								 InFile.data(),
								 mVertexShader.GetAddressOf(),
								 mVertexShaderBuf.GetAddressOf(),
								 FuncName
								));
}

void JDXObject::SetPixelShader(JWTextView InFile, LPCSTR FuncName)
{
	CheckResult(
				LoadPixelShader(
								G_Context.GetDevice(),
								InFile.data(),
								mPixelShader.GetAddressOf(),
								mPixelShaderBuf.GetAddressOf(),
								FuncName
							   ));
}

void JDXObject::SetGeometryShader(JWTextView InFile, LPCSTR FuncName)
{
	CheckResult(
				LoadGeometryShader(
								   G_Context.GetDevice(),
								   InFile.data(),
								   mGeometryShader.GetAddressOf(),
								   mGeometryShaderBuf.GetAddressOf(),
								   FuncName));
}

void JDXObject::SetHullShader(JWTextView InFile, LPCSTR FuncName)
{
	CheckResult(
				LoadHullShaderFile(
								   G_Context.GetDevice(),
								   InFile.data(),
								   mHullShader.GetAddressOf(),
								   mHullShaderBuf.GetAddressOf(),
								   FuncName));
}

void JDXObject::SetDomainShader(JWTextView InFile, LPCSTR FuncName)
{

	CheckResult(
				LoadDomainShaderFile(
									 G_Context.GetDevice(),
									 InFile.data(),
									 mDomainShader.GetAddressOf(),
									 mDomainShaderBuf.GetAddressOf(),
									 FuncName));
}

void JDXObject::SetComputeShader(JWTextView InFile, LPCSTR FuncName)
{
	CheckResult(
				LoadComputeShaderFile(
									  G_Context.GetDevice(),
									  InFile.data(),
									  mComputeShader.GetAddressOf(),
									  mComputeShaderBuf.GetAddressOf(),
									  FuncName));
}

void JDXObject::HandleLayout()
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
