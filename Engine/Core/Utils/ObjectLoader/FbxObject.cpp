#include "common_pch.h"
#include "FbxObject.h"

#include "FbxMaterial.h"
#include "FbxMesh.h"
#include "FbxUtils.h"
#include "Core/Interface/MManagerInterface.h"


FbxManager* g_fbx_manager = nullptr;

CFBXObj::CFBXObj(const JText& InName)
	: mFileName(InName),
	  mFbxImporter(nullptr),
	  mFbxScene(nullptr) {}


CFBXObj::CFBXObj(const JWText& InName)
	: mFileName(WString2String(InName)),
	  mFbxImporter(nullptr),
	  mFbxScene(nullptr) {}

void CFBXObj::Initialize()
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

	// mFbxImporter->Destroy();
	// mFbxScene->Destroy();
}

void CFBXObj::Release()
{
	if (mFbxScene)
		mFbxScene->Destroy();
	if (mFbxImporter)
		mFbxImporter->Destroy();
	if (g_fbx_manager)
		g_fbx_manager->Destroy();
}

void CFBXObj::Render()
{
	int32_t objNum = mDataList.size();
	for (int32_t i = 0; i < objNum; ++i)
	{
		auto mesh = mMeshList[i].get();

		if (mesh->ClassType == EMeshType::BIPED || mesh->ClassType == EMeshType::BONE)
			continue;


	}
}

bool CFBXObj::Load()
{
	Initialize();

	FbxNode* root = mFbxScene->GetRootNode();
	assert(root, "empty scene(node x)");

	FMatrix rootMatrix;

	ParseNode(root, FbxNodeAttribute::eSkeleton);
	ParseNode(root, FbxNodeAttribute::eMesh);

	PreProcess_Recursive(root);
	ParseNode_Recursive(root, nullptr, rootMatrix);
	ParseAnimation();

	Convert();

	return true;
}

bool CFBXObj::Load(const char* InFilePath)
{
	mFileName = InFilePath;

	Initialize();

	return Load();
}

bool CFBXObj::Convert()
{
	for (int32_t i = 0; i < mDataList.size(); ++i)
	{
		auto mesh = mMeshList[i].get();
		auto data = mDataList[i].get();

		// mesh->MatrixList.resize()

		mesh->Index   = i;
		mesh->FaceNum = data->FaceCount;
		mesh->Name    = mesh->Name;

		if (mesh->ParentMesh)
		{
			mesh->ParentMesh->ChildMesh.push_back(mesh);
		}

		if (mesh->ClassType == EMeshType::GEOM)
		{
			auto pMtrl = mFbxMaterialList[mesh->MaterialRefNum];
			if (mesh->FaceNum > 0 && pMtrl.empty() == false)
			{
				if (pMtrl.size() == 1)
				{
					CFbxMaterial* subMaterial = pMtrl[0];
					// 2번 인자값=-1 이면 Face Count(_countof )를 계산하지 않는다.
					data->SetUniqueBuffer(data->TriList, -1, 0);
					mesh->DiffuseTex = -1;

					if (subMaterial->mParams.size() > 0)
					{
						JWText name;
						name = String2WString(subMaterial->mParams[0].StringValue);

						// mesh->DiffuseTex = I_Texture.Add(
						// 								 g_pd3dDevice,
						// 								 name.c_str(),
						// 								 m_szDirName.c_str());
					}
					mNumVertex += data->VertexArray.size();
					mNumIndex += data->IndexArray.size();
				}
				else
				{
					int iAddCount = 0;
					for (int iSub = 0; iSub < pMtrl.size(); iSub++)
					{
						auto pSubMesh = mesh->SubMesh[iSub].get();
						auto pSubData = data->SubMesh[iSub].get();

						// 2번 인자값=-1 이면 Face Count(_countof )를 계산하지 않는다.
						pSubData->SetUniqueBuffer(pSubData->TriList, -1, 0);
						pSubMesh->DiffuseTex = -1;

						CFbxMaterial* pSubMtrl = pMtrl[iSub];
						if (pSubMtrl->mParams.size() > 0)
						{
							JWText name;
							name = String2WString(pSubMtrl->mParams[0].StringValue);
							// pSubMesh->DiffuseTex = I_Texture.Add(
							// 									 g_pd3dDevice,
							// 									 name.c_str(),
							// 									 m_szDirName.c_str());
						}
						mNumVertex += pSubData->VertexArray.size();
						mNumIndex += pSubData->IndexArray.size();

						pSubData->FaceCount = pSubData->IndexArray.size() / 3;
						pSubMesh->FaceNum   = pSubData->IndexArray.size() / 3;
					}
				}
			}
		}
	}
	return true;
}

void CFBXObj::ParseNode(FbxNode* InNode, FbxNodeAttribute::EType NodeAttribute)
{
	if (!InNode)
		return;
	FbxNodeAttribute* attribute = InNode->GetNodeAttribute();
	if (!attribute || attribute->GetAttributeType() != NodeAttribute)
		return;


	Ptr<FbxData>  data = MakePtr<FbxData>();
	Ptr<CFbxMesh> mesh = MakePtr<CFbxMesh>();

	switch (NodeAttribute)
	{
	case FbxNodeAttribute::eSkeleton:
		break;
	case FbxNodeAttribute::eMesh:
		break;
	default:
		break;
	}
}

void CFBXObj::PreProcess_Recursive(FbxNode* InNode)
{
	if (!InNode)
		return;

	mFrameMatrix.try_emplace(String2WString(InNode->GetName()), FMatrix::Identity);

	int32_t childNum = InNode->GetChildCount();

	for (int32_t i = 0; i < childNum; ++i)
	{
		PreProcess_Recursive(InNode->GetChild(i));
	}
}

void CFBXObj::ParseNode_Recursive(FbxNode* InNode, CFbxMesh* ParentMesh, const FMatrix& ParentWorldMat)
{
	if (!InNode)
		return;

	// Camera, Light Node 제외 탐색
	if (InNode->GetCamera() || InNode->GetLight())
		return;

	Ptr<FbxData>  data = MakePtr<FbxData>();
	Ptr<CFbxMesh> mesh = MakePtr<CFbxMesh>();

	FMatrix nodeWorldMat = ParseTransform(InNode, ParentWorldMat);
	// Fbx Transform -> FTransform(현재 엔진의 행렬로 변환) -> DirectX Axis 변환
	FMatrix geoMat = Maya2DXMat(FMat2JMat(GetNodeTransform(InNode)));

	mesh->Name            = InNode->GetName();
	mesh->ParentMesh      = ParentMesh;
	mesh->XFormToWorldMat = geoMat;
	mesh->MaterialRefNum  = -1;
	mesh->FaceNum         = 0;

	mMeshList.push_back(mesh);
	mDataList.push_back(data);

	mMeshHash.try_emplace(InNode, mesh.get());

	// Skeletal ? or Static
	if (InNode->GetMesh())
	{
		mesh->ClassType = EMeshType::GEOM;
		ParseMesh(InNode, InNode->GetMesh(), mesh.get(), data.get());
	}
	else
	{
		mesh->ClassType = EMeshType::BONE;
	}
	
	for (int32_t i = 0; i < InNode->GetChildCount(); ++i)
	{
		ParseNode_Recursive(InNode->GetChild(i), mesh.get(), nodeWorldMat);
	}

}

void CFBXObj::ParseAnimation() {}

void CFBXObj::ParseMesh(FbxNode* InNode, FbxMesh* InMesh, CFbxMesh* InMeshData, FbxData* InFbxData)
{
	if (!InMesh)
		return;

#pragma region Layer

	int32_t layerCount = InMesh->GetLayerCount();

	// Normal없으면 Normal 데이터 생성
	if (layerCount == 0 || !InMesh->GetLayer(0)->GetNormals())
	{
		InMesh->InitNormals();
#if FBXSDK_VERSION_MAJOR >= 2015
		InMesh->GenerateNormals();
#else
		InMesh->ComputeVertexNormals();
#endif
	}

	// Tangent(Normal에 orthogonal)한 경사 일듯? 마찬가지로 없으면 생성
	if (!InMesh->GetLayer(0)->GetTangents())
	{
		InMesh->GenerateTangentsData(0);
	}

	FLayerInfo layerInfo;
	layerInfo.Mesh = InMesh;

	std::vector<CFbxMaterial*> materials;

	std::vector<FbxLayerElementUV*>          vertexUVSets;
	std::vector<FbxLayerElementVertexColor*> vertexColorSets;
	std::vector<FbxLayerElementNormal*>      vertexNormalSets;
	std::vector<FbxLayerElementMaterial*>    vertexMaterialSets;
	std::vector<FbxLayerElementTangent*>     vertexTangentSets;

	// 레이어별로 Normal, Tangent, Color, UV, 머티리얼(정점에 다수의 텍스처가 매핑 되어있을 경우) 있으면 정보를 넣어놓는다.
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
		if (FbxLayerElementTangent* tangent = curLayer->GetTangents())
		{
			vertexTangentSets.push_back(tangent);
			layer.VertexTangentSets.push_back(tangent);
		}
		if (FbxLayerElementMaterial* material = curLayer->GetMaterials())
		{
			vertexMaterialSets.push_back(material);
			layer.VertexMaterialSets.push_back(material);

			const int32_t matCount = material->mDirectArray->GetCount();

			if (matCount > 1)
			{
				for (int32_t i = 0; i < matCount; ++i)
				{
					CFbxMaterial* fbxMat = ParseMaterialInLayer(InMesh, curLayer, i);
					materials.push_back(fbxMat);

					auto subData = MakePtr<FbxData>();
					auto subMesh = MakePtr<CFbxMesh>();

					InMeshData->SubMesh.push_back(subMesh);
					InFbxData->SubMesh.push_back(subData);
				}
			}
			else
			{
				CFbxMaterial* fbxMat = ParseMaterialInLayer(InMesh, curLayer, 0);
				materials.push_back(fbxMat);
			}
		}

		layer.Layer = curLayer;
		layerInfo.LayerList.push_back(layer);
	}

	mFbxLayerList.push_back(layerInfo);

	InMeshData->MaterialRefNum = mFbxMaterialList.size();
	mFbxMaterialList.push_back(materials);
#pragma endregion

	InMeshData->AddInfluence(InMeshData->Name, FMatrix::Identity);

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

		int32_t materialIndex = 0;
		if (!vertexMaterialSets.empty())
		{
			switch (vertexMaterialSets[0]->GetMappingMode())
			{
			case FbxLayerElement::eByPolygon:
				switch (vertexMaterialSets[0]->GetReferenceMode())
				{
				case FbxLayerElement::eDirect:
					materialIndex = polygonIndex;
					break;
				case FbxLayerElement::eIndex:
				case FbxLayerElement::eIndexToDirect:
					materialIndex = vertexMaterialSets[0]->GetIndexArray().GetAt(polygonIndex);
					break;
				}
				break;
			default:
				break;
			}
		}


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

			FTri<FVertexInfo_Simple> tri;
			tri.SubIndex = materialIndex;


			// polygon을 삼각형 단위로 시계방향으로 순회
			for (int32_t cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
			{
				const int32_t&   dccIndex = cornerIndices[cornerIndex];
				const FbxVector4 curVert  = vertices[dccIndex];

				FbxVector4 finalPosition;
				FbxVector4 finalNormal;
				FbxVector2 finalUV;
				FbxColor   finalColor(1, 1, 1, 1);

				// Position
				finalPosition = vertexMat.MultT(curVert);

				// Normal
				if (!vertexNormalSets.empty())
					finalNormal = ReadNormal(InMesh,
											 vertexNormalSets.size(),
											 vertexNormalSets[0],
											 dccIndex,
											 curPolyIndex + vertIndex[cornerIndex]);
				finalNormal.mData[3] = 0.f;
				finalNormal          = normalMat.MultT(finalNormal);
				finalNormal.Normalize();

				// Texel
				if (!vertexUVSets.empty())
					finalUV = ReadTexel(InMesh,
										vertexUVSets.size(),
										vertexUVSets[0],
										dccIndex,
										uv[cornerIndex]
									   );

				// Vertex Color
				if (!vertexColorSets.empty())
					finalColor = ReadColor(InMesh,
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
				tri.Vertex[cornerIndex] = vertex;
			}
			if (auto& subMeshes = InFbxData->SubMesh; !subMeshes.empty())
			{
				subMeshes[materialIndex]->TriList.push_back(tri);
				subMeshes[materialIndex]->FaceCount++;
			}
			else
			{
				InFbxData->TriList.push_back(tri);
			}
			InFbxData->FaceCount++;
		}

		curPolyIndex += polygonSize;
	}
}

CFbxMaterial* CFBXObj::ParseMaterialInLayer(FbxMesh* Mesh, FbxLayer* Layer, int32_t MaterialIndex)
{
	FbxLayerElementMaterial* layerMaterial = Layer->GetMaterials();
	FbxSurfaceMaterial*      fbxMaterial   = Mesh->GetNode()->GetMaterial(MaterialIndex);

	CFbxMaterial* material = new CFbxMaterial(fbxMaterial->GetName());

	struct FTextureParam
	{
		const char*              FbxPropertyName;
		const char*              ParamName;
		int32_t                  PostOperations;
		EMaterialExportParamFlag ParamFlags;
	};

	FTextureParam extractionList[] =
	{
		{FbxSurfaceMaterial::sTransparentColor, "AlphaTexture", 1, EMaterialExportParamFlag::AlphaChannel},
		{FbxSurfaceMaterial::sDiffuse, "DiffuseTexture", 0, EMaterialExportParamFlag::DiffuseMap},
		{FbxSurfaceMaterial::sBump, "NormalMapTexture", 0, EMaterialExportParamFlag::None},
		{FbxSurfaceMaterial::sNormalMap, "NormalMapTexture", 0, EMaterialExportParamFlag::NormalMap},
		{FbxSurfaceMaterial::sSpecular, "SpecularMapTexture", 0, EMaterialExportParamFlag::SpecularMap},
		{FbxSurfaceMaterial::sEmissive, "EmissiveMapTexture", 0, EMaterialExportParamFlag::None},
		{FbxSurfaceMaterial::sTransparencyFactor, "MaskTexture", 1, EMaterialExportParamFlag::None},
	};

	for (int32_t i = 0; i < ARRAYSIZE(extractionList); ++i)
	{
		const FTextureParam& textureParam = extractionList[i];
		FbxProperty          property     = fbxMaterial->FindProperty(textureParam.FbxPropertyName);

		if (property.IsValid())
		{
			if (ExtractTextures(property, textureParam.ParamName, material, textureParam.ParamFlags))
			{
				if (textureParam.PostOperations & 1)
				{
					material->SetTransparent(true);
				}
			}
		}
	}
	return material;
}

bool CFBXObj::ExtractTextures(FbxProperty&             Property, const char* ParamName, CFbxMaterial* Material,
							  EMaterialExportParamFlag ParamFlags)
{
	bool          bResult             = false;
	const int32_t layeredTextureCount = Property.GetSrcObjectCount<FbxLayeredTexture>();

	// 텍스처 레이어가 여러개 일 경우
	if (layeredTextureCount > 0)
	{
		int32_t textureIndex = 0;

		for (int32_t i = 0; i < layeredTextureCount; ++i)
		{
			FbxLayeredTexture* fbxLayeredTexture = Property.GetSrcObject<FbxLayeredTexture>(i);
			int32_t            textureCount      = fbxLayeredTexture->GetSrcObjectCount<FbxTexture>();

			for (int32_t j = 0; j < textureCount; ++j)
			{
				if (Property.GetSrcObject<FbxTexture>(j))
				{
					const FbxFileTexture* fileTexture = Property.GetSrcObject<FbxFileTexture>(j);
					AddTextureParam(Material, ParamName, textureIndex++, fileTexture->GetFileName(), ParamFlags);
					bResult = true;
				}
			}
		}
	}
	else
	{
		const int32_t textureCount = Property.GetSrcObjectCount<FbxTexture>();

		for (int32_t i = 0; i < textureCount; ++i)
		{
			if (Property.GetSrcObject<FbxTexture>(i))
			{
				const FbxFileTexture* fileTexture = Property.GetSrcObject<FbxFileTexture>(i);
				AddTextureParam(Material, ParamName, i, fileTexture->GetFileName(), ParamFlags);
				bResult = true;
			}
		}
	}

	return bResult;
}

void CFBXObj::AddTextureParam(CFbxMaterial* Material, const char* ParamName, int32_t Index, const char* FileName,
							  EMaterialExportParamFlag ParamFlags)
{
	FMaterialExportParams OutParam;
	if (Index == 0)
	{
		OutParam.Name = ParamName;
	}
	else
	{
		OutParam.Name = std::format("{0}{1}", ParamName, Index);
	}

	OutParam.ParamType      = EMaterialExportParamType::Texture2D;
	OutParam.StringValue    = FileName;
	OutParam.bInstanceParam = true;
	OutParam.Flags          = ParamFlags;

	Material->AddParam(OutParam);
}


FMatrix CFBXObj::ParseTransform(FbxNode* InNode, const FMatrix& ParentWorldMat)
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
