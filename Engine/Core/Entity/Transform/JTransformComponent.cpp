#include "common_pch.h"
#include "JTransformComponent.h"

#include "Core/Graphics/Shader/JDXObject.h"

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

void JTransformComponent::PreRender()
{
	mShaderData->PreRender();
}

void JTransformComponent::Render()
{
	PreRender();
	
	mShaderData->Render();
	
	PostRender();
}

void JTransformComponent::PostRender()
{
	UpdateConstantBuffer();
	mShaderData->PostRender();
}

void JTransformComponent::SetMesh(CFBXObj* InFbxObj)
{
	
}
