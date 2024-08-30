#pragma once
#include "GraphicDevice.h"
#include "Core/Graphics/graphics_common_include.h"
#include "Core/Utils/Math/Vector4.h"

struct FConstantBuffer_WVP
{
	FMatrix Model;
	FMatrix View;
	FMatrix Projection;
};

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
};

typedef JData<FVertexInfo_Simple> FbxData;
