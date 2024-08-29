#include "common_pch.h"
#include "MFBXManager.h"

#include "FbxUtils.h"
#include "Core/Interface/MManagerInterface.h"

void FFBX_DataModel::Initialize()
{}

void FFBX_DataModel::Create() {}
void FFBX_DataModel::SetVertexData() {}

void FFBX_DataModel::CreateVertexBuffer() {}

void FFBX_DataModel::SetIndexData() {}

void FFBX_DataModel::CreateInputLayout() {}

void FFBX_DataModel::PostRender() {}

void FFBX_DataModel::Release() {}

void FFBX_DataModel::GenAABB() {}

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

	bool bSuccess;

	bSuccess = mFbxImporter->Initialize(mFileName.c_str(), -1, g_fbx_manager->GetIOSettings());
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


// -------------------------------------- Loader -----------------------------
//
// void MFBXManager::Initialize()
// {
// 	mFbxManager = FbxManager::Create();
// 	auto* ios   = FbxIOSettings::Create(mFbxManager, "");
// 	mFbxManager->SetIOSettings(ios);
// }
//
// void MFBXManager::Release()
// {
//
// 	mFbxManager->Destroy();
// }
//
// void MFBXManager::Load(JWTextView InFileName)
// {
// 	assert(mFbxManager != nullptr, "Invalid Manager Initialization");
//
// 	JText fileName = WString2String(InFileName.data());
//
// 	// TODO: VS, PS
// 	// XShader* shader = IManager.ShaderManager.CreateOrLoad<XShader>(L"Shader/Character.hlsl");
// }
//
// void MFBXManager::Load_Internal()
// {
// 	assert(mFbxManager != nullptr, "Invalid Manager Initialization");
//
//
// 	std::string fileName = "Game/Model/CyberPunk_A.fbx";
//
// 	int32_t fileMajor, fileMinor, fileRevision;
// 	int32_t SDKMajor,  SDKMinor,  SDKRevision;
// 	FbxManager::GetFileFormatVersion(
// 									 SDKMajor,
// 									 SDKMinor,
// 									 SDKRevision);
// 	ParseAnimation();
//
// 	for (int32_t i = 0; i < mDrawList.size(); ++i)
// 	{
// 		ParseMesh(mDrawList[i]);
// 	}
//
// 	Release();
//
// }
//
// void MFBXManager::CreateConstantBuffer()
// {
// 	D3D11_BUFFER_DESC bufferDesc;
// 	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
// 	bufferDesc.ByteWidth = sizeof(FMatrix) * 255;
// 	bufferDesc.Usage     = D3D11_USAGE_DEFAULT;
// 	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//
// 	CheckResult(G_Context.GetDevice()->CreateBuffer(
// 													&bufferDesc,
// 													0,
// 													mBone.GetAddressOf()));
//
// }
//
// void MFBXManager::PreProcess_Recursive(FbxNode* InFbxNode, Ptr<FFBX_DataModel> InModel)
// {
// 	Ptr<FFBX_DataModel> fbxModel = nullptr;
//
// 	if (InFbxNode)
// 	{
// 		fbxModel             = MakePtr<FFBX_DataModel>();
// 		fbxModel->ParentNode = InFbxNode->GetParent();
// 		fbxModel->Node       = InFbxNode;
// 		fbxModel->Name       = String2WString(InFbxNode->GetName());
// 		fbxModel->ParentObj  = InModel;
// 		fbxModel->Index      = mTreeList.size();
//
// 		mTreeList.push_back(fbxModel);
// 		mFbxNodeMap.insert(std::make_pair(InFbxNode, fbxModel->Index));
// 		mFbxModelMap.insert(std::make_pair(fbxModel->Name, fbxModel));
// 	}
//
// 	FbxMesh* mesh = InFbxNode->GetMesh();
// 	if (mesh)
// 	{
// 		// GetTextureNames(mesh);
// 		mDrawList.push_back(fbxModel);
// 	}
//
// 	int32_t childNum = InFbxNode->GetChildCount();
// 	for (int32_t i = 0; i < childNum; ++i)
// 	{
// 		FbxNode* child = InFbxNode->GetChild(i);
// 		PreProcess_Recursive(child, fbxModel);
// 	}
// }
//
// void MFBXManager::ParseAnimation()
// {}
//
// void MFBXManager::ParseMesh(Ptr<FFBX_DataModel> InModel)
// {
// 	FbxMesh* mesh = InModel->Node->GetMesh();
//
// 	// InModel->bSkinned = ParseMeshSkinning(mesh, InModel);
//
// 	FbxAMatrix geo;
// 	FbxVector4 trans = InModel->Node->GetGeometricTranslation(FbxNode::eSourcePivot);
// 	FbxVector4 rot   = InModel->Node->GetGeometricRotation(FbxNode::eSourcePivot);
// 	FbxVector4 scale = InModel->Node->GetGeometricScaling(FbxNode::eSourcePivot);
//
// 	geo.SetT(trans);
// 	geo.SetR(rot);
// 	geo.SetS(scale);
//
// 	FbxAMatrix normalMat = geo;
// 	normalMat            = normalMat.Inverse();
// 	normalMat            = normalMat.Transpose();
//
// 	int32_t layerCount = mesh->GetLayerCount();
// 	bool    bFlag      = false;
//
// 	std::vector<FbxLayerElementUV*>          vertexUVSet;
// 	std::vector<FbxLayerElementVertexColor*> vertexColorSet;
// 	std::vector<FbxLayerElementTangent*>     vertexTangentSet;
// 	std::vector<FbxLayerElementMaterial*>    materialSet;
// 	std::vector<FbxLayerElementNormal*>      vertexNormalSets;
//
// 	if (layerCount == 0 || !mesh->GetLayer(0)->GetNormals())
// 	{
// 		mesh->InitNormals();
// 	}
//
// 	if (!mesh->GetLayer(0)->GetNormals())
// 	{
// 		bFlag = mesh->GenerateTangentsData(0);
// 	}
//
// 	for (int32_t i = 0; i < layerCount; ++i)
// 	{
// 		FbxLayer* layer = mesh->GetLayer(i);
//
// 		if (layer->GetUVs())
// 		{
// 			vertexUVSet.push_back(layer->GetUVs());
// 		}
// 		if (layer->GetVertexColors())
// 		{
// 			vertexColorSet.push_back(layer->GetVertexColors());
// 		}
// 		if (layer->GetTangents())
// 		{
// 			vertexTangentSet.push_back(layer->GetTangents());
// 		}
// 		if (layer->GetNormals())
// 		{
// 			vertexNormalSets.push_back(layer->GetNormals());
// 		}
// 		if (layer->GetMaterials())
// 		{
// 			materialSet.push_back(layer->GetMaterials());
// 		}
// 	}
//
// 	// 머티리얼
// 	// int32_t materialCount = InModel->Node->GetMaterialCount();
// 	// for (int32_t i = 0; i < materialCount; ++i)
// 	// {
// 	// 	FbxSurfaceMaterial* material = InModel->Node->GetMaterial(i);
// 	//
// 	// 	if (material)
// 	// 	{
// 	// 		JText name = ParseMaterial();
// 	// 		
// 	// 	}
// 	// }
//
// 	// InModel->m_pSubVertexList.resize(1);
// 	// InModel->m_pSubIWVertexList.resize(1);
//
// 	int32_t polyIndex = 0;
// 	int32_t polyCount = mesh->GetPolygonCount();
// 	int32_t faceNum   = 0;
//
// 	FbxVector4* vertexPositions = mesh->GetControlPoints();
//
// 	// 폴리건 수만큼 순회
// 	for (int32_t i = 0; i < polyCount; ++i)
// 	{
// 		int32_t polySize = mesh->GetPolygonSize(i);
//
// 		faceNum = polySize - 2;
//
// 		for (int32_t face = 0; face < faceNum; ++face)
// 		{
// 			int32_t vertexIndex[3] = {0, face + 2, face + 1};
// 			int32_t cornerIndex[3] = {
// 				mesh->GetPolygonVertex(i, 0),
// 				mesh->GetPolygonVertex(i, face + 2),
// 				mesh->GetPolygonVertex(i, face + 1),
// 			};
// 			int32_t uvIndex[3] = {
// 				mesh->GetTextureUVIndex(i, 0),
// 				mesh->GetTextureUVIndex(i, face + 2),
// 				mesh->GetTextureUVIndex(i, face + 1),
// 			};
//
// 			for (int32_t index = 0; index < 3; ++index)
// 			{
// 				int32_t         dccIndex = cornerIndex[index];
// 				FFBX_DataVertex vertex;
// 				FbxVector4      v = geo.MultT(vertexPositions[dccIndex]);
//
// 				// ------------------------- Pos -----------------------------
// 				vertex.Position.x = v.mData[0];
// 				vertex.Position.y = v.mData[2];
// 				vertex.Position.z = v.mData[1];
//
// 				// ------------------------- UV Tex -----------------------------
// 				if (!vertexUVSet.empty())
// 				{
// 					FbxLayerElementUV* uvSet = vertexUVSet[0];
// 					FbxVector2         uv;
// 					ReadTextureCoord(
// 									 uvSet,
// 									 dccIndex,
// 									 uvIndex[index],
// 									 uv);
// 					vertex.Texel.x = uv.mData[0];
// 					vertex.Texel.y = 1.0f - uv.mData[1];
// 				}
//
// 				// ------------------------- Col -----------------------------
// 				FbxColor color = FbxColor(1, 1, 1, 1);
// 				if (!vertexColorSet.empty())
// 				{
// 					color = ReadColor(vertexColorSet.size(),
// 									  vertexColorSet[0],
// 									  dccIndex,
// 									  polyIndex + vertexIndex[index]);
// 				}
// 				vertex.Color.x = color.mRed;
// 				vertex.Color.y = color.mGreen;
// 				vertex.Color.z = color.mBlue;
// 				vertex.Color.w = InModel->Index;
//
// 				// ------------------------- Normal -----------------------------
// 				if (vertexNormalSets.empty())
// 				{
// 					FbxVector4 normal = ReadNormal(
// 												   mesh,
// 												   dccIndex,
// 												   polyIndex + vertexIndex[index]);
// 					normal = normalMat.MultT(normal);
// 					normal.Normalize();
// 					vertex.Normal.x = normal.mData[0]; // x
// 					vertex.Normal.y = normal.mData[2]; // z
// 					vertex.Normal.z = normal.mData[1]; // y
// 					vertex.Normal.Normalize();
// 				}
// 				else
// 				{
// 					// Store vertex normal
// 					FbxVector4 finalNorm = ReadNormal(
// 													  mesh,
// 													  vertexNormalSets.size(),
// 													  vertexNormalSets[0],
// 													  dccIndex,
// 													  polyIndex + vertexIndex[
// 														  index]);// dwTriangleIndex * 3 + dwCornerIndex);
// 					finalNorm.mData[3] = 0.0;
// 					finalNorm          = normalMat.MultT(finalNorm);
// 					finalNorm.Normalize();
// 					vertex.Normal.x = finalNorm.mData[0];
// 					vertex.Normal.y = finalNorm.mData[2];
// 					vertex.Normal.z = finalNorm.mData[1];
// 					vertex.Normal.Normalize();
// 				}
//
// 				// ------------------------- Weight -----------------------------
// 				// TVertexIW iwVertex;
// 				//
// 				// FbxGeometryElementTangent* vertexTangent = pFbxMesh->GetElementTangent(0);
// 				// if (vertexTangent != nullptr)
// 				// {
// 				// 	FbxVector4 tangent = ReadTangent(pFbxMesh,
// 				// 									 VertexTangentSet.size(),
// 				// 									 vertexTangent,
// 				// 									 DCCIndex,
// 				// 									 iBasePolyIndex + VertexIndex[iIndex]);
// 				// 	iwVertex.tan.x = (FLOAT)tangent.mData[0];
// 				// 	iwVertex.tan.y = (FLOAT)tangent.mData[2];
// 				// 	iwVertex.tan.z = (FLOAT)tangent.mData[1];
// 				// }
// 				// else
// 				// {
// 				// 	iwVertex.tan = {0, 0, 0};
// 				// }
//
// 				// ------------------------- Skinned -----------------------------
// 				// if (InModel->bSkinned)
// 				// {
// 				// 	FFBX_DataWeight* weight = &InModel->VertexBufWeightList[dccIndex];
// 				// 	for (int i = 0; i < 4; i++)
// 				// 	{
// 				// 		iwVertex.i[i] = weight->Index[i];
// 				// 		iwVertex.w[i] = weight->Weight[i];
// 				// 	}
// 				// }
// 				// else
// 				// {
// 				// 	// 일반오브젝트 에니메이션을 스키닝 케릭터 화 작업.
// 				// 	iwVertex.i[0] = pObject->m_iIndex;
// 				// 	iwVertex.w[0] = 1.0f;
// 				// }
// 				InModel->mVertexList.push_back(vertex);//36
// 				// InModel->m_pSubVertexList[iSubMtrl].push_back(vertex);
// 				// InModel->m_pSubIWVertexList[iSubMtrl].push_back(iwVertex);
// 			}
//
// 		}
// 	}
// }
//
// void MFBXManager::ReadTextureCoord(FbxLayerElementUV* InUvSet, int32_t InVertexIndex, int32_t InUVIndex,
// 								   FbxVector2&        InUv)
// {
// 	FbxLayerElementUV* fbxLayerElementUV = InUvSet;
// 	if (!fbxLayerElementUV)
// 	{
// 		return;
// 	}
//
// 	switch (fbxLayerElementUV->GetMappingMode())
// 	{
// 	case FbxLayerElementUV::eByControlPoint:
// 		{
// 			switch (fbxLayerElementUV->GetReferenceMode())
// 			{
// 			case FbxLayerElementUV::eDirect:
// 				{
// 					FbxVector2 fbxUv = fbxLayerElementUV->GetDirectArray().GetAt(InVertexIndex);
// 					InUv.mData[0]    = fbxUv.mData[0];
// 					InUv.mData[1]    = fbxUv.mData[1];
// 					break;
// 				}
// 			case FbxLayerElementUV::eIndexToDirect:
// 				{
// 					int        id    = fbxLayerElementUV->GetIndexArray().GetAt(InVertexIndex);
// 					FbxVector2 fbxUv = fbxLayerElementUV->GetDirectArray().GetAt(id);
// 					InUv.mData[0]    = fbxUv.mData[0];
// 					InUv.mData[1]    = fbxUv.mData[1];
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	case FbxLayerElementUV::eByPolygonVertex:
// 		{
// 			switch (fbxLayerElementUV->GetReferenceMode())
// 			{
// 			// Always enters this part for the example model
// 			case FbxLayerElementUV::eDirect: // fbx 5.0 이하일 때
// 			case FbxLayerElementUV::eIndexToDirect:
// 				{
// 					InUv.mData[0] = fbxLayerElementUV->GetDirectArray().GetAt(InUVIndex).mData[0];
// 					InUv.mData[1] = fbxLayerElementUV->GetDirectArray().GetAt(InUVIndex).mData[1];
// 					break;
// 				}
// 			}
// 			break;
// 		}
// 	}
// }
//
// FbxColor MFBXManager::ReadColor(size_t  InSize, FbxLayerElementVertexColor* InFbxLayerElementVertexColor,
// 								int32_t InDccIndex,
// 								int32_t InVertexIndex)
// {
// 	FbxColor value(1, 1, 1, 1);
// 	if (InSize > 0 && InFbxLayerElementVertexColor != NULL)
// 	{
// 		// Crack apart the FBX dereferencing system for Color coordinates		
// 		switch (InFbxLayerElementVertexColor->GetMappingMode())
// 		{
// 		case FbxLayerElement::eByControlPoint:
// 			switch (InFbxLayerElementVertexColor->GetReferenceMode())
// 			{
// 			case FbxLayerElement::eDirect:
// 				{
// 					value = InFbxLayerElementVertexColor->GetDirectArray().GetAt(InDccIndex);
// 				}
// 				break;
// 			case FbxLayerElement::eIndexToDirect:
// 				{
// 					int iColorIndex = InFbxLayerElementVertexColor->GetIndexArray().GetAt(InDccIndex);
// 					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
// 				}
// 				break;
// 			}
// 		case FbxLayerElement::eByPolygonVertex:
// 			switch (InFbxLayerElementVertexColor->GetReferenceMode())
// 			{
// 			case FbxLayerElement::eDirect:
// 				{
// 					int iColorIndex = InVertexIndex;
// 					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
// 				}
// 				break;
// 			case FbxLayerElement::eIndexToDirect:
// 				{
// 					int iColorIndex = InFbxLayerElementVertexColor->GetIndexArray().GetAt(InVertexIndex);
// 					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
// 				}
// 				break;
// 			}
// 			break;
// 		}
// 	}
// 	return value;
// }
//
// FbxVector4 MFBXManager::ReadNormal(FbxMesh* InMesh, int32_t InControlPointIndex, int32_t InVertexCounter)
// {
// 	if (InMesh->GetElementNormalCount() < 1) {}
//
// 	const FbxGeometryElementNormal* vertexNormal = InMesh->GetElementNormal(0);
// 	// 노말 획득 
// 	FbxVector4 result;
// 	// 노말 벡터를 저장할 벡터 
// 	switch (vertexNormal->GetMappingMode()) 	// 매핑 모드 
// 	{
// 	// 제어점 마다 1개의 매핑 좌표가 있다.
// 	case FbxGeometryElement::eByControlPoint:
// 		{
// 			// control point mapping 
// 			switch (vertexNormal->GetReferenceMode())
// 			{
// 			case FbxGeometryElement::eDirect:
// 				{
// 					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[0]);
// 					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[1]);
// 					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[2]);
// 				}
// 				break;
// 			case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int index = vertexNormal->GetIndexArray().GetAt(InControlPointIndex);
// 					// 인덱스를 얻어온다. 
// 					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
// 					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
// 					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
// 				}
// 				break;
// 			}
// 			break;
// 		}
// 		break;
// 	// 정점 마다 1개의 매핑 좌표가 있다.
// 	case FbxGeometryElement::eByPolygonVertex:
// 		{
// 			switch (vertexNormal->GetReferenceMode())
// 			{
// 			case FbxGeometryElement::eDirect:
// 				{
// 					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[0]);
// 					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[1]);
// 					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[2]);
// 				}
// 				break;
// 			case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int index = vertexNormal->GetIndexArray().GetAt(InVertexCounter);
// 					// 인덱스를 얻어온다. 
// 					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
// 					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
// 					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
// 				}
// 				break;
// 			}
// 		}
// 		break;
// 	}
// 	return result;
// }
//
// FbxVector4 MFBXManager::ReadNormal(FbxMesh*               InMesh, size_t InVertexNormalCount,
// 								   FbxLayerElementNormal* InVertexNormalSets,
// 								   int32_t                IncontrolPointIndex, int32_t InVertexIndex)
// {
// 	FbxVector4 result(0, 0, 0);
// 	if (InVertexNormalCount < 1)
// 	{
// 		return result;
// 	}
// 	int iVertexNormalLayer = InMesh->GetElementNormalCount();
//
// 	const FbxGeometryElementNormal* vertexNormal = InMesh->GetElementNormal(0);
// 	// 노말 획득 
//
// 	// 노말 벡터를 저장할 벡터 
// 	switch (InVertexNormalSets->GetMappingMode()) 	// 매핑 모드 
// 	{
// 	// 제어점 마다 1개의 매핑 좌표가 있다.
// 	case FbxGeometryElement::eByControlPoint:
// 		{
// 			// control point mapping 
// 			switch (InVertexNormalSets->GetReferenceMode())
// 			{
// 			case FbxGeometryElement::eDirect:
// 				{
// 					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
// 																	   mData[
// 						0]);
// 					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
// 																	   mData[
// 						1]);
// 					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
// 																	   mData[
// 						2]);
// 				}
// 				break;
// 			case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int index = InVertexNormalSets->GetIndexArray().GetAt(IncontrolPointIndex);
// 					// 인덱스를 얻어온다. 
// 					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
// 					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
// 					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
// 				}
// 				break;
// 			}
// 			break;
// 		}
// 		break;
// 	// 정점 마다 1개의 매핑 좌표가 있다.
// 	case FbxGeometryElement::eByPolygonVertex:
// 		{
// 			switch (vertexNormal->GetReferenceMode())
// 			{
// 			case FbxGeometryElement::eDirect:
// 				{
// 					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[0]);
// 					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[1]);
// 					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[2]);
// 				}
// 				break;
// 			case FbxGeometryElement::eIndexToDirect:
// 				{
// 					int index = InVertexNormalSets->GetIndexArray().GetAt(InVertexIndex);
// 					// 인덱스를 얻어온다. 
// 					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
// 					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
// 					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
// 				}
// 				break;
// 			}
// 		}
// 		break;
// 	}
// 	return result;
// }
