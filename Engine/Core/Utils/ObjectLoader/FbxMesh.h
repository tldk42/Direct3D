#pragma once
#include "common_include.h"
#include "Core/Utils/Math/TMatrix.h"

enum class EMeshType : uint8_t
{
	GEOM = 0,
	BONE,
	DUMMY,
	BIPED
};


struct JMesh
{
	JText     Name;
	int32_t   Index;
	EMeshType ClassType;

	JMesh* ParentMesh;

	int32_t MaterialRefNum;
	int32_t FaceNum;

	FMatrix XFormToWorldMat;
	FMatrix WorldMat;
	FMatrix InverseMat;
	FMatrix WorldTransMat;
	FMatrix CalculationMat;
	FMatrix WorldRotateMat;
	FMatrix WorldScaleMat;

	std::vector<FMatrix>    MatrixList;
	std::vector<Ptr<JMesh>> SubMesh;
	std::vector<JMesh*>     ChildMesh;

	int32_t DiffuseTex;

	std::vector<JText>                 InfluenceNames;
	std::unordered_map<JText, FMatrix> BindPoseMap;

	JMesh()  = default;
	~JMesh() = default;

	void AddInfluence(const JText& InText, const FMatrix& InMatrix);
};
