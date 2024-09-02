#pragma once
#include "JDXObject.h"
#include "Core/Manager/Manager_Base.h"

#define Manager_Shader MShaderManager::Get()


class MShaderManager : public Manager_Base<JDXObject, MShaderManager>
{
private:
	friend class TSingleton<MShaderManager>;
	friend class MManagerInterface;

	MShaderManager()  = default;
	~MShaderManager() = default;

public:
	MShaderManager(const MShaderManager&)            = delete;
	MShaderManager& operator=(const MShaderManager&) = delete;

};
