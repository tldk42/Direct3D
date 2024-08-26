#pragma once

#include <fbxsdk.h>

#include "Core/Graphics/ShaderStructs.h"
#include "Core/Graphics/ConstantBuffer/CConstantBuffer.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Core/Utils/Math/Vector.h"
#include "Core/Utils/Math/Vector4.h"

class XTexture2D;

struct FFBX_DataAnimationTrack
{
	uint32_t    Frame;
	FMatrix     MatTrack;
	FVector     Scale;
	FQuaternion Rotation;
	FVector     Transform;
};

struct FFBX_DataWeight
{
	std::vector<int32_t> Index;
	std::vector<float>   Weight;

	FFBX_DataWeight()
	{
		Index.resize(8);
		Weight.resize(8);
	}

	void AddWeight(int32_t BoneIndex, float BoneWeight)
	{
		for (int32_t i = 0; i < Index.size(); ++i)
		{
			if (BoneWeight > Weight[i])
			{
				for (int32_t j = Index.size() - 1; j >= 0; --j)
				{
					Index[j]  = Index[j - 1];
					Weight[j] = Weight[j - 1];
				}

				Index[i]  = BoneIndex;
				Weight[i] = BoneWeight;

				break;
			}
		}
	}

};

struct FFBX_DataVertex
{
	FVector  Position;
	FVector  Normal;
	FVector4 Color;
	FVector2 Texel;

	FFBX_DataVertex() = default;

	FFBX_DataVertex(FVector  InPosition,
					FVector  InNormal,
					FVector4 InColor,
					FVector2 InTexel)
		: Position(InPosition),
		  Normal(InNormal),
		  Color(InColor),
		  Texel(InTexel)
	{}

};

class FFBX_DataModel
{
public:
	FFBX_DataModel()  = default;
	~FFBX_DataModel() = default;

public:
	FbxNode* ParentNode = nullptr;
	FbxNode* Node       = nullptr;

	Ptr<FFBX_DataModel> ParentObj;

	std::vector<FFBX_DataVertex> mVertexList;

	int32_t Index = -1;
	JWText  Name;
	bool    bSkinned = false;
	FMatrix LocalMatrix;
	FMatrix BoneWorld[255];

public:
	void Initialize();
	void Create();

public:
	virtual void SetVertexData();
	virtual void CreateVertexBuffer();
	virtual void SetIndexData();
	virtual void CreateInputLayout();
	virtual void PostRender();
	virtual void Release();
	virtual void GenAABB();
};

class XFBXObj
{
public:
	XFBXObj(const JWText& InName);

public:
	void ClearData();
	bool Load(FbxNode* InRootNode);

private:
	bool LoadMesh_Recursive(FbxNode* InNode);
	bool LoadUV(FbxMesh* InMesh, FVertexInfo_Simple* vertices, WORD* indices);

private:
	void             PrintNode(FbxNode* InNode);
	void             PrintNodeAttribute(FbxNodeAttribute* InNodeAttribute) const;
	static FbxString GetAttributeTypeName(FbxNodeAttribute::EType type);

private:
	JWText mFileName;

	std::vector<FMeshData> mMeshData;
};


class MFBXLoader : public Manager_Base<XFBXObj, MFBXLoader>
{
public:
	void Initialize();
	void Release();

	void Load(JWTextView InFileName);
	void Load_Internal();

protected:
	void CreateConstantBuffer();

private:
	void PreProcess(FbxNode* InFbxNode, Ptr<FFBX_DataModel> InModel);
	void ParseAnimation();
	void ParseMesh(Ptr<FFBX_DataModel> InModel);

	void ReadTextureCoord(FbxLayerElementUV* InUvSet, int32_t InVertexIndex, int32_t InUVIndex,
						  FbxVector2& InUv);
	FbxColor ReadColor(size_t InSize, FbxLayerElementVertexColor* InFbxLayerElementVertexColor,
					   int32_t InDccIndex, int32_t InVertexIndex);
	FbxVector4 ReadNormal(FbxMesh* InMesh, int32_t InControlPointIndex, int32_t InVertexCounter);
	FbxVector4 ReadNormal(FbxMesh* InMesh, size_t InVertexNormalCount, FbxLayerElementNormal* InVertexNormalSets,
						  int32_t  IncontrolPointIndex, int32_t InVertexIndex);

private:
	FbxManager*  mManager;
	FbxImporter* mImporter;
	FbxScene*    mScene;
	FbxNode*     mRootNode;


	std::vector<Ptr<FFBX_DataModel>> mDrawList;
	std::vector<Ptr<FFBX_DataModel>> mTreeList;

	std::map<FbxNode*, int32_t>           mFbxNodeMap;
	std::map<JWText, Ptr<FFBX_DataModel>> mFbxModelMap;
	ComPtr<ID3D11Buffer>                  mBone;

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MFBXLoader>;
	friend class MManagerInterface;

	MFBXLoader();

public:
	MFBXLoader(const MFBXLoader&)            = delete;
	MFBXLoader& operator=(const MFBXLoader&) = delete;

#pragma endregion
};

class ModelLoader
{};
