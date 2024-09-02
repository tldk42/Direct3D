#include "common_pch.h"
#include "JBaseMeshComponent.h"

#include "Core/Graphics/Shader/JDXObject.h"
#include "Core/Utils/ObjectLoader/FbxMesh.h"

JBaseMeshComponent::JBaseMeshComponent() {}
JBaseMeshComponent::JBaseMeshComponent(JTextView InName) {}
JBaseMeshComponent::~JBaseMeshComponent() {}

void JBaseMeshComponent::PreRender()
{
	G_Context.GetImmediateDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mShaderData->PreRender();
}

void JBaseMeshComponent::Render()
{
	const int32_t num = mFbxData.size();

	for (int32_t i = 0; i < num; ++i)
	{
		auto mesh = mMeshData[i].get();
		if (mesh->ClassType == EMeshType::BIPED || mesh->ClassType == EMeshType::BONE)
			continue;

		// C Buffer Update
		FMatrix::D3DXMatrixTranspose(&mConstantBufferData.Model, &mWorldMat);
		UpdateConstantBuffer();

		if (!mesh->SubMesh.empty())
		{
			for (int32_t j = 0; j < mesh->SubMesh.size(); ++j)
			{
				auto subMesh = mesh->SubMesh[j].get();

				if (subMesh->FaceNum > 0)
				{}
			}
		}
	}
}

void       JBaseMeshComponent::PostRender() {}
ELayerType JBaseMeshComponent::GetLayerType()
{
	return ELayerType::End;
}

void JBaseMeshComponent::SetBoneMatrices(CFbxMesh* InMeshData, CFBXObj* AnimMesh)
{
	D3D11_MAPPED_SUBRESOURCE mappedFaceDest;

	// G_Context.GetImmediateDeviceContext()->Map()
}
