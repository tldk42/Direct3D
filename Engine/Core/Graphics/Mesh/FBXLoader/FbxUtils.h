#pragma once
#include <fbxsdk.h>
#include "Core/Utils/Math/TMatrix.h"

namespace Fbx::Utils
{
	struct FLayer
	{
		FbxLayer*                                Layer;
		std::vector<FbxLayerElementUV*>          VertexUVSets;
		std::vector<FbxLayerElementMaterial*>    VertexMaterialSets;
		std::vector<FbxLayerElementVertexColor*> VertexColorSets;
		std::vector<FbxLayerElementNormal*>      VertexNormalSets;
		std::vector<FbxLayerElementTangent*>     VertexTangentSets;
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
}
