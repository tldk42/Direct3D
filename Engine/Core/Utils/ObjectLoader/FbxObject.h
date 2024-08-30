#pragma once

#include <fbxsdk.h>

#include "FbxUtils.h"
#include "Core/Graphics/ShaderStructs.h"
#include "Core/Utils/Math/TMatrix.h"
#include "Core/Utils/Math/Vector.h"

using namespace Fbx::Utils;

using FbxData         = JData<FVertexInfo_Simple>;
using FbxMeshData     = std::vector<Ptr<FbxData>>;
using FbxMaterialList = std::vector<std::vector<class FbxMaterial>>;
using DxMatrixHash    = std::unordered_map<JWText, FMatrix>;
using PoseMap         = std::unordered_map<FbxNode*, FbxMatrix>;

class XFBXObj
{
public:
	XFBXObj(const JText& InName);
	XFBXObj(const JWText& InName);

public:
	/** fbx sdk 초기화 및 생성 */
	void Initialize();
	/** 존재하는 모든 fbx sdk destroy (프로세스 종료 전에 호출) */
	void Release();

public:
	bool Load();
	bool Load(const char* InFilePath);

private:
	void    PreProcess_Recursive(FbxNode* InNode);
	void    ParseNode_Recursive(FbxNode* InNode, const FMatrix& ParentWorldMat);
	void    ParseAnimation();
	void    ParseMesh(FbxNode* InNode, FbxMesh* InMesh);
	FMatrix ParseTransform(FbxNode* InNode, const FMatrix& ParentWorldMat);

private:
	JText mFileName;

	FbxImporter* mFbxImporter;
	FbxScene*    mFbxScene;

	std::unordered_map<JWText, FMatrix> mFrameMatrix;

	std::vector<FLayerInfo>              mFbxLayerList;
	std::vector<Ptr<JData<JStaticMesh>>> mFbxMeshData;
};
