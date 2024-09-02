#pragma once

#include <fbxsdk.h>

#include "FbxMaterial.h"
#include "FbxMesh.h"
#include "FbxUtils.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Core/Utils/Math/Vector.h"

using namespace Fbx::Utils;

using FbxData      = JData<FVertexInfo_Simple>;
using FbxMeshData  = std::vector<Ptr<FbxData>>;
using DxMatrixHash = std::unordered_map<JWText, FMatrix>;
using PoseHash     = std::unordered_map<FbxNode*, FbxMatrix>;
using MeshHash     = std::unordered_map<FbxNode*, CFbxMesh*>;

class CFBXObj
{
public:
	CFBXObj(const JText& InName);
	CFBXObj(const JWText& InName);

public:
	/** fbx sdk 초기화 및 생성 */
	void Initialize();
	/** 존재하는 모든 fbx sdk destroy (프로세스 종료 전에 호출) */
	void Release();

	void Render();

public:
	bool Load();
	bool Load(const char* InFilePath);
	bool Convert();

private:
	void          PreProcess_Recursive(FbxNode* InNode);
	void          ParseNode_Recursive(FbxNode* InNode, CFbxMesh* ParentMesh, const FMatrix& ParentWorldMat);
	void          ParseAnimation();
	void          ParseMesh(FbxNode* InNode, FbxMesh* InMesh, CFbxMesh* InMeshData, FbxData* InFbxData);
	CFbxMaterial* ParseMaterialInLayer(FbxMesh* Mesh, FbxLayer* Layer, int32_t MaterialIndex);
	bool          ExtractTextures(FbxProperty&    Property, const char* ParamName, CFbxMaterial* Material,
						 EMaterialExportParamFlag ParamFlags);
	void AddTextureParam(CFbxMaterial*            Material, const char* ParamName, int32_t Index, const char* FileName,
						 EMaterialExportParamFlag ParamFlags);
	FMatrix ParseTransform(FbxNode* InNode, const FMatrix& ParentWorldMat);

private:
	JText mFileName;

	FbxImporter* mFbxImporter;
	FbxScene*    mFbxScene;

	std::unordered_map<JWText, FMatrix> mFrameMatrix;

	std::vector<Ptr<FbxData>>            mDataList;
	std::vector<Ptr<CFbxMesh>>           mMeshList;
	std::vector<FLayerInfo>              mFbxLayerList;
	std::vector<Ptr<JData<JStaticMesh>>> mFbxMeshData;

	uint32_t mNumVertex;
	uint32_t mNumIndex;

	MeshHash        mMeshHash;
	FbxMaterialList mFbxMaterialList;
};
