#pragma once
#include "Core/Graphics/Layer/XLayerBase.h"
#include "Core/Graphics/Shader/MShaderManager.h"
#include "Core/Graphics/Texture/MTextureManager.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "GUI/MGUIManager.h"

#define IManager MManagerInterface::Get()

class MManagerInterface : public TSingleton<MManagerInterface>
{
public:
	void Initialize();
	void Update(float DeltaTime);
	void Render();
	void Release();

public:
	MTextureManager  TextureManager;	// 텍스처
	MShaderManager   ShaderManager;		// 셰이더
	MGUIManager      GUIManager;		// GUI
	MViewportManager ViewportManager;	// 뷰포트

	MLayerManager LayerManager;		// 레이어


#pragma region Singleton Boilerplate

public:
	friend class TSingleton<MManagerInterface>;

	MManagerInterface();

public:
	MManagerInterface(const MManagerInterface&)            = delete;
	MManagerInterface& operator=(const MManagerInterface&) = delete;

#pragma endregion
};
