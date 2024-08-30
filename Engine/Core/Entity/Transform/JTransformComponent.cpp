#include "common_pch.h"
#include "JTransformComponent.h"

#include "Core/Graphics/Shader/XShader.h"

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
