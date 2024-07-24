#pragma once
#include "XShader.h"
#include "Manager/Manager_Base.h"

#define Manager_Shader ShaderManager::Get()


class ShaderManager : public Manager_Base<XShader, ShaderManager>
{
private:
	friend class TSingleton<ShaderManager>;
	ShaderManager()  = default;
	~ShaderManager() = default;

public:
	ShaderManager(const ShaderManager&)            = delete;
	ShaderManager& operator=(const ShaderManager&) = delete;

};
