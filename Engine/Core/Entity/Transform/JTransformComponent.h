#pragma once
#include "Core/Entity/JObject.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Interface/IRenderable.h"
#include "Core/Utils/Math/TMatrix.h"

struct CFbxMesh;
class CFBXObj;
/**
 * 위치를 가지는 컴포넌트
 * local axis를 시각화 하기 위해 mesh를 추가
 */
class JTransformComponent : public JObject, public IRenderable
{
public:
	JTransformComponent();

public:
	void         PreRender() override;
	void         Render() override;
	void         PostRender() override;
	ELayerType   GetLayerType() override;
	virtual void SetMesh(CFBXObj* InFbxObj);

public:
	virtual void UpdateConstantBuffer();

private:

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

	std::vector<Ptr<FbxData>>  mDataList;
	std::vector<Ptr<CFbxMesh>> mMeshList;

	ComPtr<ID3D11Buffer>             mBoneBuffer;
	ComPtr<ID3D11ShaderResourceView> mBoneBufferSRV;
};
