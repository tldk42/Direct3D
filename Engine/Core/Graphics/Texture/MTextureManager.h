#pragma once
#include "XTexture2D.h"
#include "Core/Manager/Manager_Base.h"

#define Manager_Texture MTextureManager::Get()

class MTextureManager : public Manager_Base<XTexture2D, MTextureManager>
{
public:
#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MTextureManager>;
	friend class MManagerInterface;

	MTextureManager()  = default;
	~MTextureManager() = default;

public:
	MTextureManager(const MTextureManager&)            = delete;
	MTextureManager& operator=(const MTextureManager&) = delete;

#pragma endregion
};
