#include "common_pch.h"
#include "MFBXManager.h"

#include "FbxUtils.h"
#include "Core/Graphics/GraphicDevice.h"
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

XFBXObj::XFBXObj(const JText& InName)
	: mFileName(InName) {}


XFBXObj::XFBXObj(const JWText& InName)
	: mFileName(WString2String(InName)) {}

void XFBXObj::Initialize()
{
	FbxManager* manager = IManager.FbxManager.mFbxManager;

	assert(manager);

	mFbxImporter = FbxImporter::Create(manager, "");
	mFbxScene    = FbxScene::Create(manager, "");

	assert(mFbxImporter);
	assert(mFbxScene);

	bool bSuccess = false;

	bSuccess = mFbxImporter->Initialize(mFileName.c_str(), -1, manager->GetIOSettings());
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

void XFBXObj::ClearData() {}

bool XFBXObj::Load(FbxNode* InRootNode)
{
	if (!InRootNode)
		return false;

	int32_t nodeNum = InRootNode->GetChildCount() + 1;
	mMeshData.clear();
	mMeshData.reserve(nodeNum);

	LoadMesh_Recursive(InRootNode);

	return true;
}

bool XFBXObj::Load()
{
	FbxNode* root = mFbxScene->GetRootNode();
	assert(root, "empty scene(node x)");

	PreProcess(root);


}

void XFBXObj::PreProcess(FbxNode* InNode)
{
	int32_t childNum = InNode->GetChildCount();

	for (int32_t i = 0; i < childNum; ++i)
	{
		FbxNode* childNode = InNode->GetChild(i);

		// if (childNode)
	}
}

void XFBXObj::ParseNode(FbxNode* InNode, const FMatrix& ParentWorldMat)
{
	if (!InNode)
		return;

	if (InNode->GetCamera() || InNode->GetLight())
		return;

	FMatrix nodeWorldMat = ParseTransform(InNode, ParentWorldMat);
	FMatrix geoMat       = Maya2DXMat(FMat2JMat(GetNodeTransform(InNode)));


}

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

	for (int32_t layerIndex = 0; layerIndex < layerCount; ++layerIndex)
	{
		FbxLayer* curLayer = InMesh->GetLayer(layerIndex);
		FLayer    layer;

		if (FbxLayerElementVertexColor* vertexColor = curLayer->GetVertexColors())
		{
			layer.VertexColorSets.push_back(vertexColor);
		}
		if (FbxLayerElementUV* uv = curLayer->GetUVs())
		{
			layer.VertexUVSets.push_back(uv);
		}
		if (FbxLayerElementNormal* normal = curLayer->GetNormals())
		{
			layer.VertexNormalSets.push_back(normal);
		}
		if (FbxLayerElementTangent* tangent = curLayer->GetTangents())
		{
			layer.VertexTangentSets.push_back(tangent);
		}
		if (FbxLayerElementMaterial* material = curLayer->GetMaterials())
		{
			layer.VertexMaterialSets.push_back(material);
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

	const int32_t polygonCount = InMesh->GetPolygonCount();
	int32_t       vertexCount  = InMesh->GetControlPointsCount(); // 정점 개수 반환
	FbxVector4*   vertices     = InMesh->GetControlPoints(); // 모든 정점 반환

	for (int32_t polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
	{
		int32_t polygonSize   = InMesh->GetPolygonSize(polygonIndex); // triangle -> 3 | square -> 4
		int32_t polyFaceCount = polygonSize - 2; // triangle -> 1| square -> 2 (triangle * 2)

		int32_t cornerIndices[3];

		for (int32_t triangleIndex = 0; triangleIndex < polyFaceCount; ++triangleIndex)
		{
			cornerIndices[0] = InMesh->GetPolygonVertex(polygonIndex, 0);
			cornerIndices[1] = InMesh->GetPolygonVertex(polygonIndex, triangleIndex + 2);
			cornerIndices[2] = InMesh->GetPolygonVertex(polygonIndex, triangleIndex + 1);

			int32_t u[3];
			u[0] = InMesh->GetTextureUVIndex(polygonIndex, 0);
			u[1] = InMesh->GetTextureUVIndex(polygonIndex, triangleIndex + 2);
			u[2] = InMesh->GetTextureUVIndex(polygonIndex, triangleIndex + 1);

			for (int32_t cornerIndex = 0; cornerIndex < 3; ++cornerIndex)
			{
				const int32_t& dccIndex = cornerIndices[cornerIndex];
				FbxVector4     finalPos = vertexMat.MultT(vertices[dccIndex]);
			}
		}
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

bool XFBXObj::LoadMesh_Recursive(FbxNode* InNode)
{
	unsigned int numPolygons    = 0;
	unsigned int numVertices    = 0;
	unsigned int numIndices     = 0;
	unsigned int numPolygonVert = 0;

	if (InNode->GetNodeAttribute() != nullptr &&
		InNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* mesh = InNode->GetMesh();

		numPolygons = mesh->GetPolygonCount();
		numIndices  = mesh->GetPolygonVertexCount();
		numVertices = mesh->GetControlPointsCount();
		// numVertices = numIndices;

		FbxVector4* controlPoints = mesh->GetControlPoints();

		std::vector<FVertexInfo_Simple> vertex(numVertices);
		std::vector<WORD>               indice(numIndices);

		int* indiceArr = mesh->GetPolygonVertices();

		for (unsigned int i = 0; i < numIndices; ++i)
		{
			indice[i] = indiceArr[i];

			vertex[i].Position.x = static_cast<float>(mesh->GetControlPointAt(indice[i]).mData[0]) / 10000.0f;
			vertex[i].Position.y = static_cast<float>(mesh->GetControlPointAt(indice[i]).mData[1]) / 10000.0f;
			vertex[i].Position.z = static_cast<float>(mesh->GetControlPointAt(indice[i]).mData[2]) / 10000.0f;
		}

		LoadUV(mesh, vertex.data(), indice.data());
	}
	return true;
}

bool XFBXObj::LoadUV(FbxMesh* InMesh, FVertexInfo_Simple* vertices, WORD* indices)
{
	if (!InMesh)
		return false;

	FbxStringList uvNameList;

	InMesh->GetUVSetNames(uvNameList);

	for (int uvIndex = 0; uvIndex < uvNameList.GetCount(); ++uvIndex)
	{
		const char*                 name      = uvNameList.GetStringAt(uvIndex);
		const FbxGeometryElementUV* uvElement = InMesh->GetElementUV(name);

		if (!uvElement)
			continue;

		const int polyCount = InMesh->GetPolygonCount();


		switch (uvElement->GetMappingMode())
		{
		case FbxLayerElement::eNone:
			break;
		case FbxLayerElement::eByControlPoint:
			for (int32_t polyIndex = 0; polyIndex < polyCount; ++polyIndex)
			{
				const uint32_t polySize = InMesh->GetPolygonSize(polyIndex);

				for (int32_t vertexIndex = 0; vertexIndex < polySize; ++vertexIndex)
				{
					FbxVector2 uvValue;

					int32_t polyVertexIndex = InMesh->GetPolygonVertex(polyIndex, vertexIndex);
					// int32_t uv
				}
			}
			break;
		case FbxLayerElement::eByPolygonVertex:
			break;
		case FbxLayerElement::eByPolygon:
			break;
		case FbxLayerElement::eByEdge:
			break;
		case FbxLayerElement::eAllSame:
			break;
		}
	}


	return true;
}

void XFBXObj::PrintNode(FbxNode* InNode)
{
	const char* nodeName    = InNode->GetName();
	FbxDouble3  translation = InNode->LclTranslation.Get();
	FbxDouble3  rotation    = InNode->LclRotation.Get();
	FbxDouble3  scaling     = InNode->LclScaling.Get();

	LOG_CORE_INFO("Position: {0}, {1}, {2}\n", translation[0], translation[1], translation[2]);
	LOG_CORE_INFO("Rotation: {0}, {1}, {2}\n", rotation[0], rotation[1], rotation[2]);
	LOG_CORE_INFO("Scaling: {0}, {1}, {2}\n", scaling[0], scaling[1], scaling[2]);

	for (int i = 0; i < InNode->GetNodeAttributeCount(); i++)
	{
		PrintNodeAttribute(InNode->GetNodeAttributeByIndex(i));
	}

	for (int i = 0; i < InNode->GetChildCount(); i++)
	{
		PrintNode(InNode->GetChild(i));
	}
}

void XFBXObj::PrintNodeAttribute(FbxNodeAttribute* InNodeAttribute) const
{
	if (!InNodeAttribute)
	{
		return;
	}

	FbxString typeName = GetAttributeTypeName(InNodeAttribute->GetAttributeType());
	JText     attrName = InNodeAttribute->GetName();

	LOG_CORE_INFO("Attribute: {}\n", typeName.Buffer());
	LOG_CORE_INFO("Attribute Name: {}\n", attrName);
}

FbxString XFBXObj::GetAttributeTypeName(FbxNodeAttribute::EType type)
{
	switch (type)
	{
	case FbxNodeAttribute::eUnknown:
		return "unidentified";
	case FbxNodeAttribute::eNull:
		return "null";
	case FbxNodeAttribute::eMarker:
		return "marker";
	case FbxNodeAttribute::eSkeleton:
		return "skeleton";
	case FbxNodeAttribute::eMesh:
		return "mesh";
	case FbxNodeAttribute::eNurbs:
		return "nurbs";
	case FbxNodeAttribute::ePatch:
		return "patch";
	case FbxNodeAttribute::eCamera:
		return "camera";
	case FbxNodeAttribute::eCameraStereo:
		return "stereo";
	case FbxNodeAttribute::eCameraSwitcher:
		return "camera switcher";
	case FbxNodeAttribute::eLight:
		return "light";
	case FbxNodeAttribute::eOpticalReference:
		return "optical reference";
	case FbxNodeAttribute::eOpticalMarker:
		return "marker";
	case FbxNodeAttribute::eNurbsCurve:
		return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface:
		return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary:
		return "boundary";
	case FbxNodeAttribute::eNurbsSurface:
		return "nurbs surface";
	case FbxNodeAttribute::eShape:
		return "shape";
	case FbxNodeAttribute::eLODGroup:
		return "lodgroup";
	case FbxNodeAttribute::eSubDiv:
		return "subdiv";
	default:
		return "unknown";
	}
}


// -------------------------------------- Loader -----------------------------

void MFBXManager::Initialize()
{
	mFbxManager = FbxManager::Create();
	auto* ios   = FbxIOSettings::Create(mFbxManager, "");
	mFbxManager->SetIOSettings(ios);
}

void MFBXManager::Release()
{

	mFbxManager->Destroy();
}

void MFBXManager::Load(JWTextView InFileName)
{
	assert(mFbxManager != nullptr, "Invalid Manager Initialization");

	JText fileName = WString2String(InFileName.data());
	
	// TODO: VS, PS
	// XShader* shader = IManager.ShaderManager.CreateOrLoad<XShader>(L"Shader/Character.hlsl");
}

void MFBXManager::Load_Internal()
{
	assert(mFbxManager != nullptr, "Invalid Manager Initialization");
	

	std::string fileName = "Game/Model/CyberPunk_A.fbx";

	int32_t fileMajor, fileMinor, fileRevision;
	int32_t SDKMajor,  SDKMinor,  SDKRevision;
	FbxManager::GetFileFormatVersion(
									 SDKMajor, SDKMinor, SDKRevision);
	ParseAnimation();

	for (int32_t i = 0; i < mDrawList.size(); ++i)
	{
		ParseMesh(mDrawList[i]);
	}

	Release();

}

void MFBXManager::CreateConstantBuffer()
{
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.ByteWidth = sizeof(FMatrix) * 255;
	bufferDesc.Usage     = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	CheckResult(G_Context.GetDevice()->CreateBuffer(
													&bufferDesc, 0, mBone.GetAddressOf()));

}

void MFBXManager::PreProcess(FbxNode* InFbxNode, Ptr<FFBX_DataModel> InModel)
{
	Ptr<FFBX_DataModel> fbxModel = nullptr;

	if (InFbxNode)
	{
		fbxModel             = MakePtr<FFBX_DataModel>();
		fbxModel->ParentNode = InFbxNode->GetParent();
		fbxModel->Node       = InFbxNode;
		fbxModel->Name       = String2WString(InFbxNode->GetName());
		fbxModel->ParentObj  = InModel;
		fbxModel->Index      = mTreeList.size();

		mTreeList.push_back(fbxModel);
		mFbxNodeMap.insert(std::make_pair(InFbxNode, fbxModel->Index));
		mFbxModelMap.insert(std::make_pair(fbxModel->Name, fbxModel));
	}

	FbxMesh* mesh = InFbxNode->GetMesh();
	if (mesh)
	{
		// GetTextureNames(mesh);
		mDrawList.push_back(fbxModel);
	}

	int32_t childNum = InFbxNode->GetChildCount();
	for (int32_t i = 0; i < childNum; ++i)
	{
		FbxNode* child = InFbxNode->GetChild(i);
		PreProcess(child, fbxModel);
	}
}

void MFBXManager::ParseAnimation()
{}

void MFBXManager::ParseMesh(Ptr<FFBX_DataModel> InModel)
{
	FbxMesh* mesh = InModel->Node->GetMesh();

	// InModel->bSkinned = ParseMeshSkinning(mesh, InModel);

	FbxAMatrix geo;
	FbxVector4 trans = InModel->Node->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rot   = InModel->Node->GetGeometricRotation(FbxNode::eSourcePivot);
	FbxVector4 scale = InModel->Node->GetGeometricScaling(FbxNode::eSourcePivot);

	geo.SetT(trans);
	geo.SetR(rot);
	geo.SetS(scale);

	FbxAMatrix normalMat = geo;
	normalMat            = normalMat.Inverse();
	normalMat            = normalMat.Transpose();

	int32_t layerCount = mesh->GetLayerCount();
	bool    bFlag      = false;

	std::vector<FbxLayerElementUV*>          vertexUVSet;
	std::vector<FbxLayerElementVertexColor*> vertexColorSet;
	std::vector<FbxLayerElementTangent*>     vertexTangentSet;
	std::vector<FbxLayerElementMaterial*>    materialSet;
	std::vector<FbxLayerElementNormal*>      vertexNormalSets;

	if (layerCount == 0 || !mesh->GetLayer(0)->GetNormals())
	{
		mesh->InitNormals();
	}

	if (!mesh->GetLayer(0)->GetNormals())
	{
		bFlag = mesh->GenerateTangentsData(0);
	}

	for (int32_t i = 0; i < layerCount; ++i)
	{
		FbxLayer* layer = mesh->GetLayer(i);

		if (layer->GetUVs())
		{
			vertexUVSet.push_back(layer->GetUVs());
		}
		if (layer->GetVertexColors())
		{
			vertexColorSet.push_back(layer->GetVertexColors());
		}
		if (layer->GetTangents())
		{
			vertexTangentSet.push_back(layer->GetTangents());
		}
		if (layer->GetNormals())
		{
			vertexNormalSets.push_back(layer->GetNormals());
		}
		if (layer->GetMaterials())
		{
			materialSet.push_back(layer->GetMaterials());
		}
	}

	// 머티리얼
	// int32_t materialCount = InModel->Node->GetMaterialCount();
	// for (int32_t i = 0; i < materialCount; ++i)
	// {
	// 	FbxSurfaceMaterial* material = InModel->Node->GetMaterial(i);
	//
	// 	if (material)
	// 	{
	// 		JText name = ParseMaterial();
	// 		
	// 	}
	// }

	// InModel->m_pSubVertexList.resize(1);
	// InModel->m_pSubIWVertexList.resize(1);

	int32_t polyIndex = 0;
	int32_t polyCount = mesh->GetPolygonCount();
	int32_t faceNum   = 0;

	FbxVector4* vertexPositions = mesh->GetControlPoints();

	// 폴리건 수만큼 순회
	for (int32_t i = 0; i < polyCount; ++i)
	{
		int32_t polySize = mesh->GetPolygonSize(i);

		faceNum = polySize - 2;

		for (int32_t face = 0; face < faceNum; ++face)
		{
			int32_t vertexIndex[3] = {0, face + 2, face + 1};
			int32_t cornerIndex[3] = {
				mesh->GetPolygonVertex(i, 0),
				mesh->GetPolygonVertex(i, face + 2),
				mesh->GetPolygonVertex(i, face + 1),
			};
			int32_t uvIndex[3] = {
				mesh->GetTextureUVIndex(i, 0),
				mesh->GetTextureUVIndex(i, face + 2),
				mesh->GetTextureUVIndex(i, face + 1),
			};

			for (int32_t index = 0; index < 3; ++index)
			{
				int32_t         dccIndex = cornerIndex[index];
				FFBX_DataVertex vertex;
				FbxVector4      v = geo.MultT(vertexPositions[dccIndex]);

				// ------------------------- Pos -----------------------------
				vertex.Position.x = v.mData[0];
				vertex.Position.y = v.mData[2];
				vertex.Position.z = v.mData[1];

				// ------------------------- UV Tex -----------------------------
				if (!vertexUVSet.empty())
				{
					FbxLayerElementUV* uvSet = vertexUVSet[0];
					FbxVector2         uv;
					ReadTextureCoord(
									 uvSet,
									 dccIndex,
									 uvIndex[index],
									 uv);
					vertex.Texel.x = uv.mData[0];
					vertex.Texel.y = 1.0f - uv.mData[1];
				}

				// ------------------------- Col -----------------------------
				FbxColor color = FbxColor(1, 1, 1, 1);
				if (!vertexColorSet.empty())
				{
					color = ReadColor(vertexColorSet.size(),
									  vertexColorSet[0],
									  dccIndex,
									  polyIndex + vertexIndex[index]);
				}
				vertex.Color.x = color.mRed;
				vertex.Color.y = color.mGreen;
				vertex.Color.z = color.mBlue;
				vertex.Color.w = InModel->Index;

				// ------------------------- Normal -----------------------------
				if (vertexNormalSets.empty())
				{
					FbxVector4 normal = ReadNormal(
												   mesh,
												   dccIndex,
												   polyIndex + vertexIndex[index]);
					normal = normalMat.MultT(normal);
					normal.Normalize();
					vertex.Normal.x = normal.mData[0]; // x
					vertex.Normal.y = normal.mData[2]; // z
					vertex.Normal.z = normal.mData[1]; // y
					vertex.Normal.Normalize();
				}
				else
				{
					// Store vertex normal
					FbxVector4 finalNorm = ReadNormal(
													  mesh,
													  vertexNormalSets.size(),
													  vertexNormalSets[0],
													  dccIndex,
													  polyIndex + vertexIndex[
														  index]);// dwTriangleIndex * 3 + dwCornerIndex);
					finalNorm.mData[3] = 0.0;
					finalNorm          = normalMat.MultT(finalNorm);
					finalNorm.Normalize();
					vertex.Normal.x = finalNorm.mData[0];
					vertex.Normal.y = finalNorm.mData[2];
					vertex.Normal.z = finalNorm.mData[1];
					vertex.Normal.Normalize();
				}

				// ------------------------- Weight -----------------------------
				// TVertexIW iwVertex;
				//
				// FbxGeometryElementTangent* vertexTangent = pFbxMesh->GetElementTangent(0);
				// if (vertexTangent != nullptr)
				// {
				// 	FbxVector4 tangent = ReadTangent(pFbxMesh,
				// 									 VertexTangentSet.size(),
				// 									 vertexTangent,
				// 									 DCCIndex,
				// 									 iBasePolyIndex + VertexIndex[iIndex]);
				// 	iwVertex.tan.x = (FLOAT)tangent.mData[0];
				// 	iwVertex.tan.y = (FLOAT)tangent.mData[2];
				// 	iwVertex.tan.z = (FLOAT)tangent.mData[1];
				// }
				// else
				// {
				// 	iwVertex.tan = {0, 0, 0};
				// }

				// ------------------------- Skinned -----------------------------
				// if (InModel->bSkinned)
				// {
				// 	FFBX_DataWeight* weight = &InModel->VertexBufWeightList[dccIndex];
				// 	for (int i = 0; i < 4; i++)
				// 	{
				// 		iwVertex.i[i] = weight->Index[i];
				// 		iwVertex.w[i] = weight->Weight[i];
				// 	}
				// }
				// else
				// {
				// 	// 일반오브젝트 에니메이션을 스키닝 케릭터 화 작업.
				// 	iwVertex.i[0] = pObject->m_iIndex;
				// 	iwVertex.w[0] = 1.0f;
				// }
				InModel->mVertexList.push_back(vertex);//36
				// InModel->m_pSubVertexList[iSubMtrl].push_back(vertex);
				// InModel->m_pSubIWVertexList[iSubMtrl].push_back(iwVertex);
			}

		}
	}
}

void MFBXManager::ReadTextureCoord(FbxLayerElementUV* InUvSet, int32_t InVertexIndex, int32_t InUVIndex,
								   FbxVector2&        InUv)
{
	FbxLayerElementUV* fbxLayerElementUV = InUvSet;
	if (!fbxLayerElementUV)
	{
		return;
	}

	switch (fbxLayerElementUV->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
		{
			switch (fbxLayerElementUV->GetReferenceMode())
			{
			case FbxLayerElementUV::eDirect:
				{
					FbxVector2 fbxUv = fbxLayerElementUV->GetDirectArray().GetAt(InVertexIndex);
					InUv.mData[0]    = fbxUv.mData[0];
					InUv.mData[1]    = fbxUv.mData[1];
					break;
				}
			case FbxLayerElementUV::eIndexToDirect:
				{
					int        id    = fbxLayerElementUV->GetIndexArray().GetAt(InVertexIndex);
					FbxVector2 fbxUv = fbxLayerElementUV->GetDirectArray().GetAt(id);
					InUv.mData[0]    = fbxUv.mData[0];
					InUv.mData[1]    = fbxUv.mData[1];
					break;
				}
			}
			break;
		}
	case FbxLayerElementUV::eByPolygonVertex:
		{
			switch (fbxLayerElementUV->GetReferenceMode())
			{
			// Always enters this part for the example model
			case FbxLayerElementUV::eDirect: // fbx 5.0 이하일 때
			case FbxLayerElementUV::eIndexToDirect:
				{
					InUv.mData[0] = fbxLayerElementUV->GetDirectArray().GetAt(InUVIndex).mData[0];
					InUv.mData[1] = fbxLayerElementUV->GetDirectArray().GetAt(InUVIndex).mData[1];
					break;
				}
			}
			break;
		}
	}
}

FbxColor MFBXManager::ReadColor(size_t  InSize, FbxLayerElementVertexColor* InFbxLayerElementVertexColor,
								int32_t InDccIndex,
								int32_t InVertexIndex)
{
	FbxColor value(1, 1, 1, 1);
	if (InSize > 0 && InFbxLayerElementVertexColor != NULL)
	{
		// Crack apart the FBX dereferencing system for Color coordinates		
		switch (InFbxLayerElementVertexColor->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			switch (InFbxLayerElementVertexColor->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				{
					value = InFbxLayerElementVertexColor->GetDirectArray().GetAt(InDccIndex);
				}
				break;
			case FbxLayerElement::eIndexToDirect:
				{
					int iColorIndex = InFbxLayerElementVertexColor->GetIndexArray().GetAt(InDccIndex);
					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
				}
				break;
			}
		case FbxLayerElement::eByPolygonVertex:
			switch (InFbxLayerElementVertexColor->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				{
					int iColorIndex = InVertexIndex;
					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
				}
				break;
			case FbxLayerElement::eIndexToDirect:
				{
					int iColorIndex = InFbxLayerElementVertexColor->GetIndexArray().GetAt(InVertexIndex);
					value           = InFbxLayerElementVertexColor->GetDirectArray().GetAt(iColorIndex);
				}
				break;
			}
			break;
		}
	}
	return value;
}

FbxVector4 MFBXManager::ReadNormal(FbxMesh* InMesh, int32_t InControlPointIndex, int32_t InVertexCounter)
{
	if (InMesh->GetElementNormalCount() < 1) {}

	const FbxGeometryElementNormal* vertexNormal = InMesh->GetElementNormal(0);
	// 노말 획득 
	FbxVector4 result;
	// 노말 벡터를 저장할 벡터 
	switch (vertexNormal->GetMappingMode()) 	// 매핑 모드 
	{
	// 제어점 마다 1개의 매핑 좌표가 있다.
	case FbxGeometryElement::eByControlPoint:
		{
			// control point mapping 
			switch (vertexNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				{
					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[0]);
					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[1]);
					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InControlPointIndex).mData[2]);
				}
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt(InControlPointIndex);
					// 인덱스를 얻어온다. 
					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
				}
				break;
			}
			break;
		}
		break;
	// 정점 마다 1개의 매핑 좌표가 있다.
	case FbxGeometryElement::eByPolygonVertex:
		{
			switch (vertexNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				{
					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[0]);
					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[1]);
					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(InVertexCounter).mData[2]);
				}
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int index = vertexNormal->GetIndexArray().GetAt(InVertexCounter);
					// 인덱스를 얻어온다. 
					result[0] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
					result[1] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
					result[2] = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
				}
				break;
			}
		}
		break;
	}
	return result;
}

FbxVector4 MFBXManager::ReadNormal(FbxMesh*               InMesh, size_t InVertexNormalCount,
								   FbxLayerElementNormal* InVertexNormalSets,
								   int32_t                IncontrolPointIndex, int32_t InVertexIndex)
{
	FbxVector4 result(0, 0, 0);
	if (InVertexNormalCount < 1)
	{
		return result;
	}
	int iVertexNormalLayer = InMesh->GetElementNormalCount();

	const FbxGeometryElementNormal* vertexNormal = InMesh->GetElementNormal(0);
	// 노말 획득 

	// 노말 벡터를 저장할 벡터 
	switch (InVertexNormalSets->GetMappingMode()) 	// 매핑 모드 
	{
	// 제어점 마다 1개의 매핑 좌표가 있다.
	case FbxGeometryElement::eByControlPoint:
		{
			// control point mapping 
			switch (InVertexNormalSets->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				{
					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
																	   mData[
						0]);
					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
																	   mData[
						1]);
					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(IncontrolPointIndex).
																	   mData[
						2]);
				}
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int index = InVertexNormalSets->GetIndexArray().GetAt(IncontrolPointIndex);
					// 인덱스를 얻어온다. 
					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
				}
				break;
			}
			break;
		}
		break;
	// 정점 마다 1개의 매핑 좌표가 있다.
	case FbxGeometryElement::eByPolygonVertex:
		{
			switch (vertexNormal->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				{
					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[0]);
					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[1]);
					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(InVertexIndex).mData[2]);
				}
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					int index = InVertexNormalSets->GetIndexArray().GetAt(InVertexIndex);
					// 인덱스를 얻어온다. 
					result[0] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
					result[1] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
					result[2] = static_cast<float>(InVertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
				}
				break;
			}
		}
		break;
	}
	return result;
}
