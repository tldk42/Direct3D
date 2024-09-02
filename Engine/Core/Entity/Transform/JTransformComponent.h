#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"

class CFBXObj;
/**
 * 위치를 가지는 컴포넌트
 * local axis를 시각화 하기 위해 mesh를 추가
 */
class JTransformComponent : public JObject, public IRenderable
{
public:
	virtual void UpdateConstantBuffer();

public:
	void PreRender() override;
	void Render() override;
	void PostRender() override;

private:
	virtual void SetMesh(CFBXObj* InFbxObj);

protected:
	JTransformComponent* mParentTransformComp;
	JDXObject*           mShaderData;

	CBuffer::Space mConstantBufferData;

	FMatrix mXFromWorldMat;
	FMatrix mWorldMat;
	FMatrix mWorldMat_Inverse;
	FMatrix mWorldTransformMat;
	FMatrix mWorldRotationMat;
	FMatrix mWorldScaleMat;
};
