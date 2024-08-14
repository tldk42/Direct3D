#include "common_pch.h"
#include "GUIManager.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include "Core/Graphics/GraphicDevice.h"
#include "imgui/GUI_Inspector.h"
#include "imgui/GUI_Themes.h"
#include "imgui/GUI_Viewport.h"
#include "Core/Utils/Utils.h"
#include "Core/Window/Window.h"
#include "imgui/GUI_AssetBrowser.h"

GUIManager::GUIManager()  = default;
GUIManager::~GUIManager() = default;

void GUIManager::Initialize()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::Spectrum::StyleColorsSpectrum();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding              = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(Window::GetWindow()->GetWindowHandle());
	ImGui_ImplDX11_Init(G_Context.GetDevice(), G_Context.GetImmediateDeviceContext());

	InitializeStaticGUI();
}

void GUIManager::InitializeStaticGUI()
{
	// 메모리 예약
	mStaticGUIs.reserve(EnumAsByte(EGUIType::Max));

	mStaticGUIs.emplace_back(std::make_unique<GUI_Viewport>("Editor Viewport")); // Editor Viewport
	mStaticGUIs.emplace_back(std::make_unique<GUI_Inspector>("Editor Inspector")); // Editor Inspector
	mStaticGUIs.emplace_back(std::make_unique<GUI_AssetBrowser>("Asset Browser")); // Editor Asset Browser

	for (GUI_BaseUPtr& gui : mStaticGUIs)
	{
		gui->Initialize();
	}
}

void GUIManager::UpdateStaticGUI()
{
	for (GUI_BaseUPtr& gui : mStaticGUIs)
	{
		gui->Render();
	}
}

void GUIManager::Update(float_t DeltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	UpdateStaticGUI();
}

void GUIManager::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void GUIManager::Render()
{
	ImGui::Render(); // ImGui 렌더링 명령 생성
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // 생성된 명령(GetDrawData)을 GPU에 전달

	// Update and Render additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows(); // 새 뷰포트 생성 | 기존 뷰포트 상태, 위치 업데이트 | 필요없는 뷰포트 파괴 등
		ImGui::RenderPlatformWindowsDefault(); // 각 뷰포트 렌더
	}
}

void GUIManager::AddGUI(EGUIType InType)
{}

void GUIManager::HideGUI(EGUIType InType)
{}

void GUIManager::DeleteGUI(EGUIType InType)
{}
