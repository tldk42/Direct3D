#pragma once
#include "Core/Entity/Transform/JTransformComponent.h"

class CFBXObj;
struct CFbxMesh;

class JBaseMeshComponent : public JTransformComponent, public IRenderable
{
public:
	JBaseMeshComponent();
	JBaseMeshComponent(JTextView InName);
	~JBaseMeshComponent() override;

public:
	void PreRender() override;
	void Render() override;
	void PostRender() override;

	ELayerType GetLayerType() override;

protected:
	void SetBoneMatrices(CFbxMesh* InMeshData, CFBXObj* AnimMesh);
protected:

	std::vector<Ptr<FbxData>>         mFbxData;
	std::vector<Ptr<struct CFbxMesh>> mMeshData;
};
