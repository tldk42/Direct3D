#pragma once
#include "common_include.h"
#include "Core/Interface/ICoreInterface.h"
#include "Core/Manager/Manager_Base.h"

enum class EGUIType : uint8_t
{
	Viewport = 0,
	Inspector,
	AssetBrowser,
	Scene,
	Console,
	Settings,
	Max
};

#define Manager_GUI MGUIManager::Get()

CLASS_PTR(GUI_Base)

class MGUIManager : public ICoreInterface, public Manager_Base<GUI_Base, MGUIManager>
{
public:
	void Initialize() override;
	void Update(float_t DeltaTime) override;
	void Release() override;

	void Render();

public:
	void AddGUI(EGUIType InType);
	void HideGUI(EGUIType InType);
	void DeleteGUI(EGUIType InType);
	void ScaleAllSize(float InScale);

private:
	void InitializeStaticGUI();
	void UpdateStaticGUI(float DeltaTime);


#pragma region Singleton Boilerplate

private:
	friend class TSingleton<MGUIManager>;
	friend class MManagerInterface;

	MGUIManager();
	~MGUIManager() override;

public:
	MGUIManager(const MGUIManager&)            = delete;
	MGUIManager& operator=(const MGUIManager&) = delete;

#pragma endregion
};
