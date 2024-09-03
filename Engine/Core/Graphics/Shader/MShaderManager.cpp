#include "common_pch.h"
#include "MShaderManager.h"


MShaderManager::MShaderManager()
{
	CreateOrLoad(L"Shader/alphablend.hlsl");
}
