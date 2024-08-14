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

#define Manager_GUI GUIManager::Get()

CLASS_PTR(GUI_Base)

class GUIManager : public ICoreInterface, public TSingleton<GUIManager>
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

private:
	void InitializeStaticGUI();
	void UpdateStaticGUI();

private:
	std::vector<GUI_BaseUPtr> mStaticGUIs; // 기본 엔진 GUI (변경 사항 X) 

#pragma region Singleton Boilerplate

private:
	friend class TSingleton<GUIManager>;

	GUIManager();
	~GUIManager() override;

public:
	GUIManager(const GUIManager&)            = delete;
	GUIManager& operator=(const GUIManager&) = delete;

#pragma endregion
};
