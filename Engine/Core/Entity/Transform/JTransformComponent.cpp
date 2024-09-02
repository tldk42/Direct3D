#include "common_pch.h"
#include "JTransformComponent.h"

#include "Core/Graphics/Shader/JDXObject.h"
#include "Core/Interface/MManagerInterface.h"
#include "Core/Utils/ObjectLoader/FbxObject.h"


JTransformComponent::JTransformComponent()
{
	mShaderData = IManager.ShaderManager.CreateOrLoad(L"Shader/alphablend.hlsl");
}

void JTransformComponent::PreRender()
{
	mShaderData->PreRender();
}

void JTransformComponent::Render()
{
	PreRender();

	mShaderData->Render();

	auto* context = G_Context.GetImmediateDeviceContext();
	assert(context);

	{
		int32_t drawObjectNum = mDataList.size();

		for (int32_t i = 0; i < drawObjectNum; ++i)
		{
			auto* mesh = mMeshList[i].get();

			if (mesh->ClassType == EMeshType::BIPED || mesh->ClassType == EMeshType::BONE)
				continue;

			UpdateConstantBuffer();

			{
				FMatrix*                 matrix;
				D3D11_MAPPED_SUBRESOURCE mappedFaceDest;

				CheckResult(context->Map(mBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedFaceDest));

				matrix = static_cast<FMatrix*>(mappedFaceDest.pData);
				for (int32_t j = 0; j < mesh->InfluenceNames.size(); ++j)
				{
					matrix[j] = mesh->BindPoseMap[mesh->InfluenceNames[j]] * mesh->MatrixList[j];
				}

				context->Unmap(mBoneBuffer.Get(), 0);
			}

			context->VSSetShaderResources(1, 1, mBoneBufferSRV.GetAddressOf());

			if (!mesh->SubMesh.empty())
			{}
			else
			{
				if (mesh->FaceNum < 1)
					continue;
				// context->PSSetShaderResources(0, 1, );
			}
		}
	}

	PostRender();
}

void JTransformComponent::PostRender()
{
	UpdateConstantBuffer();
	mShaderData->PostRender();
}

ELayerType JTransformComponent::GetLayerType()
{
	return ELayerType::End;
}


void JTransformComponent::UpdateConstantBuffer()
{
	if (mParentTransformComp && mParentTransformComp->mShaderData->GetCBuffer())
	{
		G_Context.GetImmediateDeviceContext()->UpdateSubresource(mParentTransformComp->mShaderData->GetCBuffer(),
																 0,
																 nullptr,
																 &mParentTransformComp->mConstantBufferData,
																 0,
																 0);
	}
	else if (mShaderData->GetCBuffer())
	{
		G_Context.GetImmediateDeviceContext()->UpdateSubresource(mShaderData->GetCBuffer(),
																 0,
																 nullptr,
																 &mConstantBufferData,
																 0,
																 0);
	}
}

void JTransformComponent::SetMesh(CFBXObj* InFbxObj)
{
	// Create a bone matrix buffer
	// It will be updated more than once per frame (in a typical game) so make it dynamic
	D3D11_BUFFER_DESC vbdesc =
	{
		255 * sizeof(FMatrix),
		D3D11_USAGE_DYNAMIC,
		D3D11_BIND_SHADER_RESOURCE,
		D3D11_CPU_ACCESS_WRITE,
		0
	};
	CheckResult(G_Context.GetDevice()->CreateBuffer(&vbdesc, nullptr, mBoneBuffer.GetAddressOf()));

	// Again, we need a resource view to use it in the shader
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format               = DXGI_FORMAT_R32G32B32A32_FLOAT;
	SRVDesc.ViewDimension        = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.ElementOffset = 0;
	SRVDesc.Buffer.ElementWidth  = 255 * 4;
	CheckResult(G_Context.GetDevice()->
						  CreateShaderResourceView(mBoneBuffer.Get(), &SRVDesc, mBoneBufferSRV.GetAddressOf()));

	mMeshList = InFbxObj->mMeshList;
	mDataList = InFbxObj->mDataList;

	for (int32_t i = 0; i < mDataList.size(); ++i)
	{
		auto data = mDataList[i].get();
		auto mesh = mMeshList[i].get();

		if (!mesh->SubMesh.empty())
		{
			for (int32_t subMeshIdx = 0; subMeshIdx < mesh->SubMesh.size(); ++subMeshIdx)
			{
				auto subData = data->SubMesh[subMeshIdx].get();
				auto subMesh = mesh->SubMesh[subMeshIdx].get();

				if (subData->VertexArray.size() < 3)
					continue;

				// subMesh.
			}
		}
	}
}
