#pragma once
#include "Core/Entity/Actor/JActor.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Utils/Math/TMatrix.h"

class JTransformComponent : public JObject
{
public:

	virtual void UpdateConstantBuffer();


protected:
	JTransformComponent* mParentTransformComp;
	XShader*             mShaderData;

	FConstantBuffer_WVP mConstantBufferData;

	FMatrix mXFromWorldMat;
	FMatrix mWorldMat;
	FMatrix mWorldMat_Inverse;
	FMatrix mWorldTransformMat;
	FMatrix mWorldRotationMat;
	FMatrix mWorldScaleMat;
};
