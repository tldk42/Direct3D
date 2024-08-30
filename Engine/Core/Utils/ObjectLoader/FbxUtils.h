#pragma once
#include <fbxsdk.h>

#include "Core/Utils/Logger.h"
#include "Core/Utils/Math/TMatrix.h"

namespace Fbx::Utils
{
	struct FLayer
	{
		FbxLayer*                                Layer;
		std::vector<FbxLayerElementUV*>          VertexUVSets;
		std::vector<FbxLayerElementVertexColor*> VertexColorSets;
		std::vector<FbxLayerElementNormal*>      VertexNormalSets;

		// UnComment When needeed
		// std::vector<FbxLayerElementMaterial*>    VertexMaterialSets;
		// std::vector<FbxLayerElementTangent*>     VertexTangentSets;
	};

	struct FLayerInfo
	{
		FbxMesh*            Mesh;
		std::vector<FLayer> LayerList;
	};


	[[nodiscard]] inline FbxMatrix GetNodeTransform(const FbxNode* InNode)
	{
		if (!InNode)
		{
			throw std::exception("empty node");
		}

		const FbxVector4 transform = InNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 rotation  = InNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 scale     = InNode->GetGeometricScaling(FbxNode::eSourcePivot);

		return FbxMatrix(transform, rotation, scale);
	}

	/** FBX SDK Matrix -> Jacob Engine Matrix */
	[[nodiscard]] inline FMatrix FMat2JMat(const FbxMatrix& InMatrix)
	{
		FMatrix resultMatrix;

		float*        dest = reinterpret_cast<float*>(&resultMatrix);
		const double* src  = reinterpret_cast<const double*>(&InMatrix);

		for (int32_t i = 0; i < 16; ++i)
		{
			dest[i] = static_cast<float>(src[i]);
		}

		return resultMatrix;
	}

	/** Maya (z-up) axis -> Directx axis*/
	[[nodiscard]] inline FMatrix Maya2DXMat(const FMatrix& InMatrix)
	{
		FMatrix returnMatrix;

		returnMatrix._11 = InMatrix._11;
		returnMatrix._12 = InMatrix._13;
		returnMatrix._13 = InMatrix._12;
		returnMatrix._21 = InMatrix._31;
		returnMatrix._22 = InMatrix._33;
		returnMatrix._23 = InMatrix._32;
		returnMatrix._31 = InMatrix._21;
		returnMatrix._32 = InMatrix._23;
		returnMatrix._33 = InMatrix._22;
		returnMatrix._41 = InMatrix._41;
		returnMatrix._42 = InMatrix._43;
		returnMatrix._43 = InMatrix._42;

		returnMatrix._14 = returnMatrix._24 = returnMatrix._34 = 0.0f;
		returnMatrix._44 = 1.0f;
		return returnMatrix;
	}

#pragma region Read Node Data

	[[nodiscard]] inline FbxString GetAttributeTypeName(FbxNodeAttribute::EType type)
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
	};

	inline void PrintNodeAttribute(FbxNodeAttribute* InNodeAttribute)
	{
		if (!InNodeAttribute)
		{
			return;
		}

		FbxString typeName = GetAttributeTypeName(InNodeAttribute->GetAttributeType());
		JText     attrName = InNodeAttribute->GetName();

		LOG_CORE_INFO("Attribute: {}\n", typeName.Buffer());
		LOG_CORE_INFO("Attribute Name: {}\n", attrName);
	};

	inline void PrintNode(FbxNode* InNode)
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

	[[nodiscard]] inline FbxVector4 ReadNormal(const FbxMesh*         InMesh, int32_t VertexNormalCount,
											   FbxLayerElementNormal* VertexNormalSets,
											   int32_t                ControlPointIndex, int32_t VertexIndex)
	{
		FbxVector4 resultNormal(0, 0, 0);

		if (VertexNormalCount < 1)
			return resultNormal;

		const bool bRefModeDirect = VertexNormalSets->GetReferenceMode() == FbxGeometryElement::eDirect;
		const int32_t vertexNormalLayer = InMesh->GetElementNormalCount();
		const FbxGeometryElementNormal* vertexNormal = InMesh->GetElementNormal(0);

		int32_t index;

		switch (VertexNormalSets->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint: // Control Points to 1(uv coord) 
			index = VertexNormalSets->GetIndexArray().GetAt(ControlPointIndex);
			break;
		case FbxLayerElement::eByPolygonVertex: // Vertex to 1(uv coord)
			index = VertexNormalSets->GetIndexArray().GetAt(VertexIndex);
			break;
		default:
			return resultNormal;
		}

		resultNormal[0] = static_cast<float>(
			VertexNormalSets->GetDirectArray().GetAt(bRefModeDirect ? ControlPointIndex : index).mData[0]);
		resultNormal[1] = static_cast<float>(
			VertexNormalSets->GetDirectArray().GetAt(bRefModeDirect ? ControlPointIndex : index).mData[1]);
		resultNormal[2] = static_cast<float>(
			VertexNormalSets->GetDirectArray().GetAt(bRefModeDirect ? ControlPointIndex : index).mData[2]);

		return resultNormal;
	}

	[[nodiscard]] inline FbxVector2 ReadTexel(const FbxMesh*     InMesh, int32_t VertexTextureCount,
											  FbxLayerElementUV* VertexTextureSets,
											  int32_t            UVIndex, int32_t VertexIndex)
	{
		FbxVector2 resultUV(0, 0);

		if (VertexTextureCount < 1 || !VertexTextureSets)
			return resultUV;

		const int32_t            uvCount = InMesh->GetElementUVCount();
		const FbxLayerElementUV* uv      = InMesh->GetElementUV(0);

		const bool bDirectMode = VertexTextureSets->GetDirectArray().GetAt(VertexIndex);
		FbxVector2 fbxUV;

		switch (VertexTextureSets->GetMappingMode())
		{
		case FbxLayerElementUV::eByControlPoint:
			switch (VertexTextureSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				fbxUV = VertexTextureSets->GetDirectArray().GetAt(VertexIndex);
				resultUV.mData[0] = fbxUV.mData[0];
				resultUV.mData[1] = fbxUV.mData[1];
				break;
			case FbxLayerElementUV::eIndexToDirect:
				fbxUV = VertexTextureSets->GetDirectArray().GetAt(VertexTextureSets->GetIndexArray().GetAt(VertexIndex));
				resultUV.mData[0] = fbxUV.mData[0];
				resultUV.mData[1] = fbxUV.mData[1];
				break;
			case FbxLayerElement::eIndex:
				break;
			}
			break;
		case FbxLayerElementUV::eByPolygonVertex:
			resultUV.mData[0] = VertexTextureSets->GetDirectArray().GetAt(UVIndex).mData[0];
			resultUV.mData[1] = VertexTextureSets->GetDirectArray().GetAt(UVIndex).mData[1];
			break;
		default:
			return resultUV;
		}

		return resultUV;
	}

	[[nodiscard]] inline FbxColor ReadColor(const FbxMesh*              InMesh, int32_t VertexColorCount,
											FbxLayerElementVertexColor* VertexColorSets,
											int32_t                     ColorIndex, int32_t VertexIndex)
	{
		FbxColor returnColor(1, 1, 1, 1);
		if (VertexColorCount < 1 || !VertexColorSets)
			return returnColor;

		const int32_t                        vertexColorCount = InMesh->GetElementVertexColorCount();
		const FbxGeometryElementVertexColor* vertexColor      = InMesh->GetElementVertexColor(0);

		int32_t index = -1;

		switch (VertexColorSets->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
			switch (VertexColorSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				return VertexColorSets->GetDirectArray().GetAt(ColorIndex);
			case FbxLayerElement::eIndexToDirect:
				index = VertexColorSets->GetIndexArray().GetAt(ColorIndex);
				break;
			default:
				return returnColor;
			}
		case FbxLayerElement::eByPolygonVertex:
			switch (VertexColorSets->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
				index = VertexIndex;
				break;
			case FbxLayerElement::eIndexToDirect:
				index = VertexColorSets->GetIndexArray().GetAt(VertexIndex);
				break;
			default:
				return returnColor;
			}
			break;
		default:
			return returnColor;
		}

		return VertexColorSets->GetDirectArray().GetAt(index);
	}
#pragma endregion

}
