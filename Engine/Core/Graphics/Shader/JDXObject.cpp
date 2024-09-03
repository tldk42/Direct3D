#include "common_pch.h"
#include "JDXObject.h"
#include "JShader.h"
#include "Debug/Assert.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/Texture/JTexture2D.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"


JDXObject::JDXObject(const JWText& InShaderFile)
	: mShaderFile(InShaderFile)
{
	mVertexSize = sizeof(FVertexInfo_Simple);
	mIndexSize  = sizeof(DWORD);

	// 성공적으로 로드되면 Vertex, Pixel, InputLayout은 보장 (이후 셰이더는 필요시 로드)
	mShader = IManager.ShaderManager.CreateOrLoad(InShaderFile);
	
	Utils::DX::CreateBuffer(G_Context.GetDevice(), D3D11_BIND_CONSTANT_BUFFER, nullptr, sizeof(CBuffer::Space), 1, mConstantBuffer.GetAddressOf());
}


JDXObject::~JDXObject()
{}

void JDXObject::Release()
{}

void JDXObject::PreRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);

	deviceContext->IASetPrimitiveTopology(mPrimitiveType);

	deviceContext->IASetInputLayout(mShader->GetInputLayout());

	uint32_t offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &mVertexSize, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, mConstantBuffer.GetAddressOf());

	deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
	deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
	deviceContext->GSSetShader(mShader->GetGeometryShader(), nullptr, 0);
	deviceContext->HSSetShader(mShader->GetHullShader(), nullptr, 0);
	deviceContext->DSSetShader(mShader->GetDomainShader(), nullptr, 0);

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

void JDXObject::UpdateVertexData(void*          InSrcData, const uint32_t InVertexNum, const uint32_t InVertexSize,
								 const uint32_t InVertexBegin)
{
	mVertexNum   = InVertexNum;
	mVertexSize  = InVertexSize;
	mVertexBegin = InVertexBegin;

	Utils::DX::UpdateBuffer(G_Context.GetImmediateDeviceContext(), mVertexBuffer.Get(), InSrcData);

}

void JDXObject::UpdateIndexData(void* InSrcData, const uint32_t InIndexNum, const uint32_t InIndexBegin)
{
	mIndexNum   = InIndexNum;
	mIndexBegin = InIndexBegin;

	Utils::DX::UpdateBuffer(G_Context.GetImmediateDeviceContext(), mIndexBuffer.Get(), InSrcData);

}
