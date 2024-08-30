#include "common_pch.h"
#include "FbxObject.h"

#include "FbxUtils.h"
#include "Core/Interface/MManagerInterface.h"


FbxManager* g_fbx_manager = nullptr;

XFBXObj::XFBXObj(const JText& InName)
	: mFileName(InName),
	  mFbxImporter(nullptr),
	  mFbxScene(nullptr) {}


XFBXObj::XFBXObj(const JWText& InName)
	: mFileName(WString2String(InName)),
	  mFbxImporter(nullptr),
	  mFbxScene(nullptr) {}

void XFBXObj::Initialize()
{
	if (!g_fbx_manager)
	{
		g_fbx_manager = FbxManager::Create();
	}
	assert(g_fbx_manager);

	mFbxImporter = FbxImporter::Create(g_fbx_manager, "");
	mFbxScene    = FbxScene::Create(g_fbx_manager, "");

	assert(mFbxImporter);
	assert(mFbxScene);

	bool bSuccess = mFbxImporter->Initialize(mFileName.c_str(), -1, g_fbx_manager->GetIOSettings());
	if (!bSuccess)
	{
		LOG_CORE_ERROR("Failed to initialize fbx importer, {0} {1}", __FILE__, __LINE__);
		return;
	}

	bSuccess = mFbxImporter->Import(mFbxScene);
	if (!bSuccess)
	{
		LOG_CORE_ERROR("Failed to import fbx importer, {0} {1}", __FILE__, __LINE__);
		return;
	}

	// front -> y | right -> x | up -> z (y, z 축이 바뀐 형태) 
	FbxAxisSystem::MayaZUp.ConvertScene(mFbxScene);

	mFbxImporter->Destroy();
	mFbxScene->Destroy();
}

void XFBXObj::Release()
{
	if (mFbxScene)
		mFbxScene->Destroy();
	if (mFbxImporter)
		mFbxImporter->Destroy();
	if (g_fbx_manager)
		g_fbx_manager->Destroy();
}

bool XFBXObj::Load()
{
	Initialize();

	FbxNode* root = mFbxScene->GetRootNode();
	assert(root, "empty scene(node x)");

	FMatrix rootMatrix;

	PreProcess_Recursive(root);
	ParseNode_Recursive(root, rootMatrix);
	ParseAnimation();

	return true;
}

bool XFBXObj::Load(const char* InFilePath)
{
	mFileName = InFilePath;

	Initialize();

	return Load();
}

void XFBXObj::PreProcess_Recursive(FbxNode* InNode)
{
	if (!InNode)
		return;

	FMatrix matrix;

	mFrameMatrix.try_emplace(String2WString(InNode->GetName()), matrix);

	int32_t childNum = InNode->GetChildCount();

	for (int32_t i = 0; i < childNum; ++i)
	{
		PreProcess_Recursive(InNode->GetChild(i));
	}
}

void XFBXObj::ParseNode_Recursive(FbxNode* InNode, const FMatrix& ParentWorldMat)
{
	if (!InNode)
		return;

	// Camera, Light Node 제외 탐색
	if (InNode->GetCamera() || InNode->GetLight())
		return;

	Ptr<FbxData> data = MakePtr<FbxData>();
	// Ptr<

	FMatrix nodeWorldMat = ParseTransform(InNode, ParentWorldMat);
	FMatrix geoMat       = Maya2DXMat(FMat2JMat(GetNodeTransform(InNode)));


}

void XFBXObj::ParseAnimation() {}

void XFBXObj::ParseMesh(FbxNode* InNode, FbxMesh* InMesh)
{
	if (!InMesh)
		return;

#pragma region Layer

	int32_t layerCount = InMesh->GetLayerCount();

	if (layerCount == 0 || !InMesh->GetLayer(0)->GetNormals())
	{
		InMesh->InitNormals();
#if FBXSDK_VERSION_MAJOR >= 2015
		InMesh->GenerateNormals();
#else
		InMesh->ComputeVertexNormals();
#endif
	}

	if (!InMesh->GetLayer(0)->GetTangents())
	{
		InMesh->GenerateTangentsData(0);
	}

	FLayerInfo layerInfo;
	layerInfo.Mesh = InMesh;


	std::vector<FbxLayerElementUV*>          vertexUVSets;
	std::vector<FbxLayerElementVertexColor*> vertexColorSets;
	std::vector<FbxLayerElementNormal*>      vertexNormalSets;
	for (int32_t layerIndex = 0; layerIndex < layerCount; ++layerIndex)
	{
		FbxLayer* curLayer = InMesh->GetLayer(layerIndex);
		FLayer    layer;

		if (FbxLayerElementVertexColor* vertexColor = curLayer->GetVertexColors())
		{
			vertexColorSets.push_back(vertexColor);
			layer.VertexColorSets.push_back(vertexColor);
		}
		if (FbxLayerElementUV* uv = curLayer->GetUVs())
		{
			vertexUVSets.push_back(uv);
			layer.VertexUVSets.push_back(uv);
		}
		if (FbxLayerElementNormal* normal = curLayer->GetNormals())
		{
			vertexNormalSets.push_back(normal);
			layer.VertexNormalSets.push_back(normal);
		}

		layer.Layer = curLayer;
		layerInfo.LayerList.push_back(layer);
	}

	mFbxLayerList.push_back(layerInfo);
#pragma endregion

	FbxAMatrix vertexMat;
	FbxAMatrix normalMat;
	{
		FbxVector4 translation = InNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		FbxVector4 rotation    = InNode->GetGeometricRotation(FbxNode::eSourcePivot);
		FbxVector4 scale       = InNode->GetGeometricScaling(FbxNode::eSourcePivot);

		FbxAMatrix geometryMat;
		geometryMat.SetT(translation);
		geometryMat.SetR(rotation);
		geometryMat.SetS(scale);

		vertexMat = geometryMat;

		normalMat = vertexMat;
		normalMat = normalMat.Inverse();
		normalMat = normalMat.Transpose();
	}

	const int32_t polygonCount = InMesh->GetPolygonCount();			//  삼각 or 사각 poly 
	int32_t       vertexCount  = InMesh->GetControlPointsCount();	// 정점 개수 반환
	int32_t       polygonSize;										// triangle -> 3 | square -> 4
	int32_t       polygonFaceCount;									// triangle -> 1| square -> 2 (triangle * 2)
	int32_t       curPolyIndex = 0;
	FbxVector4*   vertices     = InMesh->GetControlPoints();			// 모든 정점 좌표 반환(메시의 로컬 좌표계)

	for (int32_t polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		polygonSize      = InMesh->GetPolygonSize(polygonIndex);
		polygonFaceCount = polygonSize - 2; // 대부분 1일 것


		for (int32_t triangleIndex = 0; triangleIndex < polygonFaceCount; ++triangleIndex)
		{
			int32_t vertIndex[3] = {0, triangleIndex + 2, triangleIndex + 1};

			// ccw(반시계) -> cw(시계)
			int32_t cornerIndices[3] =
			{
				InMesh->GetPolygonVertex(polygonIndex, 0),
				InMesh->GetPolygonVertex(polygonIndex, triangleIndex + 2),
				InMesh->GetPolygonVertex(polygonIndex, triangleIndex + 1)
			};

			int32_t uv[3] =
			{
				InMesh->GetTextureUVIndex(polygonIndex, 0),
				InMesh->GetTextureUVIndex(polygonIndex, triangleIndex + 2),
				InMesh->GetTextureUVIndex(polygonIndex, triangleIndex + 1)
			};


			// polygon을 삼각형 단위로 시계방향으로 순회
			for (int32_t cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
			{
				const int32_t&   dccIndex = cornerIndices[cornerIndex];
				const FbxVector4 curVert  = vertices[dccIndex];

				// Position
				FbxVector4 finalPosition = vertexMat.MultT(curVert);

				// Normal
				FbxVector4 finalNormal = ReadNormal(InMesh,
													vertexNormalSets.size(),
													vertexNormalSets[0],
													dccIndex,
													curPolyIndex + vertIndex[cornerIndex]);
				finalNormal.mData[3] = 0.f;
				finalNormal          = normalMat.MultT(finalNormal);
				finalNormal.Normalize();

				// Texel
				FbxVector2 finalUV = ReadTexel(InMesh,
											   vertexUVSets.size(),
											   vertexUVSets[0],
											   dccIndex,
											   uv[cornerIndex]
											  );

				// Vertex Color
				FbxColor finalColor = ReadColor(InMesh,
												vertexColorSets.size(),
												vertexColorSets[0],
												dccIndex,
												curPolyIndex + vertIndex[cornerIndex]
											   );

				// 저장될 vertexInfo
				FVertexInfo_Simple vertex;
				{
					vertex.Position.x = static_cast<float>(finalPosition.mData[0]);
					vertex.Position.y = static_cast<float>(finalPosition.mData[2]);
					vertex.Position.z = static_cast<float>(finalPosition.mData[1]);

					vertex.Normal.x = static_cast<float>(finalNormal.mData[0]);
					vertex.Normal.y = static_cast<float>(finalNormal.mData[2]);
					vertex.Normal.z = static_cast<float>(finalNormal.mData[1]);

					vertex.UV.x = static_cast<float>(finalUV.mData[0]);
					vertex.UV.y = 1.f - static_cast<float>(finalUV.mData[1]);

					vertex.Color.x = static_cast<float>(finalColor.mRed);
					vertex.Color.y = static_cast<float>(finalColor.mGreen);
					vertex.Color.z = static_cast<float>(finalColor.mBlue);
					vertex.Color.w = static_cast<float>(finalColor.mAlpha);
				}

			}
		}
		curPolyIndex += polygonSize;
	}
}

FMatrix XFBXObj::ParseTransform(FbxNode* InNode, const FMatrix& ParentWorldMat)
{
	// FbxMatrix geoTransform = GetNodeTransform(InNode);
	// FMatrix geo;
	// geo   = Maya2DXMat(FMat2JMat(geoTransform));

	FbxVector4 translation;
	FbxVector4 rotation;
	FbxVector4 scale;

	// lcl(Transform...).Get()은 부모로 부터의 상대적인 Transform을 반환
	if (InNode->LclTranslation.IsValid())
	{
		translation = InNode->LclTranslation.Get();
	}
	if (InNode->LclRotation.IsValid())
	{
		rotation = InNode->LclRotation.Get();
	}
	if (InNode->LclScaling.IsValid())
	{
		scale = InNode->LclScaling.Get();
	}

	FbxMatrix transform(translation, rotation, scale);
	FMatrix   local = Maya2DXMat(FMat2JMat(transform));
	FMatrix   world = local * ParentWorldMat;

	return world;
}
