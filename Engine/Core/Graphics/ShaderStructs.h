#pragma once
#include "GraphicDevice.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Math/Vector4.h"

namespace CBuffer
{
	/**
	 * Model, View, Projection
	 */
	struct Space
	{
		FMatrix Model;
		FMatrix View;
		FMatrix Projection;
	};

	struct Light
	{
		FVector4 LightPos;
	};

	struct Camera
	{
		FVector4 CamPos;
	};
}

struct FVertexInfo_Simple
{
	FVector  Position;
	FVector  Normal;
	FVector2 UV;
	FVector4 Color;
};

class JActorS
{
public:
	std::vector<Ptr<struct JStaticMesh>> mMesh;
	FMatrix                              mWorldMat;

};

class JStaticMesh : public JActorS
{
public:
	JStaticMesh* mParentMesh;

};


template <typename T>
struct FTri
{
	int32_t SubIndex = -1;
	T       Vertex[3];
	FVector Normal;

public:
	FTri() = default;

	FTri(int32_t InIndex)
		: SubIndex(InIndex) {};
};

template <typename T>
struct IsTriangleSame
{
	FTri<T> Tri;

	IsTriangleSame(FTri<T> Data)
		: Tri(Data) {}

	bool operator()(FTri<T>& Value)
	{
		return Value.SubIndex == Tri.SubIndex;
	}
};

template <typename T>
struct JData
{
	int32_t                 FaceCount = 0;
	std::vector<T>          VertexArray;
	std::vector<WORD>       IndexArray;
	std::vector<FTri<T>>    TriList;
	std::vector<Ptr<JData>> SubMesh;
	std::vector<JData*>     ChildMesh;
	T*                      DrawVertex = nullptr;
	FMatrix                 InverseMatrix;

	int32_t SetUniqueBuffer(std::vector<FTri<T>>& TriList, int32_t Material, int32_t StartTri)
	{
		int32_t faceNum = TriList.size();

		VertexArray.reserve(faceNum * 3);
		IndexArray.reserve(faceNum * 3);

		FTri<T> triSame(Material);
		if (Material >= 0)
		{
			faceNum = std::count_if(TriList.begin(), TriList.end(), IsTriangleSame<T>(triSame));
		}

		for (int32_t face = 0; face < faceNum; ++face)
		{
			for (int32_t i = 0; i < 3; ++i)
			{
				VertexArray.push_back(TriList[StartTri + face].Vertex[i]);
				IndexArray.push_back(VertexArray.size() - 1);
			}
		}
		return faceNum;
	}
};

typedef JData<FVertexInfo_Simple> FbxData;
