#include "common_pch.h"
#include "JDXObject.h"
#include "JShader.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/Texture/JTexture2D.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/Graphics/DXUtils.h"
#include "Core/Utils/ObjectLoader/FbxObject.h"


JDXObject::JDXObject()
{
	mShader = IManager.ShaderManager.CreateOrLoad(L"alphablend");

	mVertexSize = sizeof(Vertex::FVertexInfo_Base);
	mIndexSize  = sizeof(DWORD);


	mPrimitiveModelData.resize(2, MakePtr<JData<Vertex::FVertexInfo_Base>>());

	float                halfSize     = 1 / 2.0f;
	std::vector<FVector> cubeVertices = {
		{-halfSize, -halfSize, -halfSize},
		{halfSize, -halfSize, -halfSize},
		{halfSize, halfSize, -halfSize},
		{-halfSize, halfSize, -halfSize},
		{-halfSize, -halfSize, halfSize},
		{halfSize, -halfSize, halfSize},
		{halfSize, halfSize, halfSize},
		{-halfSize, halfSize, halfSize}
	};

	for (const auto& pos : cubeVertices)
	{
		Vertex::FVertexInfo_Base vertex;
		vertex.Position = pos * 0.2;
		vertex.Normal   = FVector{0.0f, 0.0f, 1.0f}; // Assuming a flat surface
		vertex.UV       = FVector2{0.0f, 0.0f}; // UV coordinates
		vertex.Color    = FVector4{1.0f, 1.0f, 1.0f, 1.0f}; // White color
		mPrimitiveModelData[1]->VertexArray.push_back(vertex);
	}

	mPrimitiveModelData[1]->IndexArray = {
		0, 1, 2, 0, 2, 3, // Front face
		4, 5, 6, 4, 6, 7, // Back face
		0, 1, 5, 0, 5, 4, // Bottom face
		2, 3, 7, 2, 7, 6, // Top face
		0, 3, 7, 0, 7, 4, // Left face
		1, 2, 6, 1, 6, 5  // Right face
	};
	mVertexNum = mPrimitiveModelData[1]->VertexArray.size();
	mIndexNum  = mPrimitiveModelData[1]->IndexArray.size();

	CreateBuffers();
}

JDXObject::JDXObject(CFBXObj* InFbxObj)
{
	mPrimitiveType      = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	mPrimitiveModelData = InFbxObj->mDataList;
	mPrimitiveMeshData  = InFbxObj->mMeshList;

	uint32_t vertexBufOffset = 0;
	uint32_t indexBufOffset  = 0;

	for (int32_t dataIdx = 0; dataIdx < mPrimitiveModelData.size(); ++dataIdx)
	{
		auto data = mPrimitiveModelData[dataIdx].get();
		auto mesh = mPrimitiveMeshData[dataIdx].get();

		if (!mesh->SubMesh.empty())
		{
			//TODO: handle submesh
		}
		else
		{
			if (data->VertexArray.size() < 3)
				continue;

			mVertexNum  = data->VertexArray.size();
			mVertexSize = sizeof(Vertex::FVertexInfo_Base);

			// TODO: Vertex Buffer Update -> Create
			mVertexBegin = vertexBufOffset;
			vertexBufOffset += mVertexNum;


			mIndexNum  = data->IndexArray.size();
			mIndexSize = sizeof(WORD);

			// TODO: Index Buffer Update -> Create
			mIndexBegin = indexBufOffset;
			indexBufOffset += mIndexNum;
		}
	}

	CreateBuffers();

	mShader = IManager.ShaderManager.CreateOrLoad(L"alphablend");
}

JDXObject::JDXObject(const JWText& InShaderFile)
	: mShaderFile(InShaderFile)
{
	mShader = IManager.ShaderManager.CreateOrLoad(L"alphablend");

	mVertexSize = sizeof(Vertex::FVertexInfo_Base);
	mIndexSize  = sizeof(DWORD);

	CreateBuffers();
}

JDXObject::JDXObject(CFBXObj* InFbxObj, const JWText& InShaderFile)
	: JDXObject(InShaderFile)
{


	CreateBuffers();
}


JDXObject::~JDXObject()
{}

void JDXObject::Release()
{}

void JDXObject::CreateBuffers()
{
	ID3D11Device* device = G_Context.GetDevice();
	assert(device);

	// Vertex 버퍼 생성
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_VERTEX_BUFFER,
							(void**)&mPrimitiveModelData[1]->VertexArray.at(0),
							mVertexSize,
							mVertexNum,
							mVertexBuffer.GetAddressOf());

	// Index 버퍼 생성
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_INDEX_BUFFER,
							(void**)&mPrimitiveModelData[1]->IndexArray.at(0),
							mIndexSize,
							mIndexNum,
							mIndexBuffer.GetAddressOf());

	// 상수 버퍼 생성 (WVP)
	Utils::DX::CreateBuffer(device,
							D3D11_BIND_CONSTANT_BUFFER,
							nullptr,
							sizeof(CBuffer::Space),
							1,
							mConstantBuffer_Space.GetAddressOf());
}

void JDXObject::PreRender()
{
	auto* deviceContext = G_Context.GetImmediateDeviceContext();
	assert(deviceContext);

	CBuffer::Space wvp;
	wvp.Model      = FMatrix::Identity;
	wvp.View       = XMMatrixTranspose(IManager.CameraManager.GetCurrentMainCam()->GetViewMatrix());
	wvp.Projection = XMMatrixTranspose(IManager.CameraManager.GetCurrentMainCam()->GetProjMatrix());

	deviceContext->UpdateSubresource(mConstantBuffer_Space.Get(),
									 0,
									 nullptr,
									 &wvp,
									 0,
									 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetInputLayout(mShader->GetInputLayout());

	uint32_t offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &mVertexSize, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	deviceContext->VSSetConstantBuffers(0, 1, mConstantBuffer_Space.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, mConstantBuffer_Space.GetAddressOf());

	deviceContext->VSSetShader(mShader->GetVertexShader(), nullptr, 0);
	deviceContext->PSSetShader(mShader->GetPixelShader(), nullptr, 0);
	deviceContext->GSSetShader(mShader->GetGeometryShader(), nullptr, 0);
	deviceContext->HSSetShader(mShader->GetHullShader(), nullptr, 0);
	deviceContext->DSSetShader(mShader->GetDomainShader(), nullptr, 0);

	deviceContext->RSSetState(G_Context.GetDXTKCommonStates()->CullClockwise());
	// mTexture->PreRender();
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

void JDXObject::SetShaderFile(JWTextView InFileName)
{
	mShaderFile = InFileName;

	mShader = IManager.ShaderManager.CreateOrLoad(InFileName.data());
}

void JDXObject::SetShaderFile(const JWText& InFileName)
{
	mShaderFile = InFileName;

	mShader = IManager.ShaderManager.CreateOrLoad(InFileName);
}
