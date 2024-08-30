#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Graphics/ShaderStructs.h"

class JStaticMeshComponent : public JObject
{
public:
	JStaticMeshComponent();
	JStaticMeshComponent(JTextView InName);
	~JStaticMeshComponent();

private:
	FConstantBuffer_WVP mConstantBufferData;
	XShader*            mShaderData;
};
