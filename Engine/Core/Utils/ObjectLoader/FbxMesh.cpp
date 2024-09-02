#include "common_pch.h"
#include "FbxMesh.h"

void CFbxMesh::AddInfluence(const JText& InText, const FMatrix& InMatrix)
{
	InfluenceNames.push_back(InText);
	BindPoseMap[InText] = InMatrix;
}
