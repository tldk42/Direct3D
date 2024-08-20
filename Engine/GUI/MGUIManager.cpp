#include "common_pch.h"
#include "MGUIManager.h"

#include <ranges>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx11.h>

#include "Core/Graphics/GraphicDevice.h"
#include "Core/Interface/MManagerInterface.h"
#include "imgui/GUI_Inspector.h"
#include "imgui/GUI_Themes.h"
#include "imgui/GUI_Viewport.h"
#include "Core/Window/Window.h"
#include "imgui/GUI_AssetBrowser.h"

constexpr char Name_Viewport[]     = "Editor Viewport";
constexpr char Name_Inspector[]    = "Editor Inspector";
constexpr char Name_AssetBrowser[] = "Asset Browser";

MGUIManager::MGUIManager()  = default;
MGUIManager::~MGUIManager() = default;

void MGUIManager::Initialize()
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
	ImGui::GetIO().FontGlobalScale = 1.5f; // 기본 글자 크기보다 1.5배로 확대

}

void MGUIManager::InitializeStaticGUI()
{
	CreateOrLoad<GUI_Viewport>(Name_Viewport)->Initialize();
	CreateOrLoad<GUI_Inspector>(Name_Inspector)->Initialize();
	CreateOrLoad<GUI_AssetBrowser>(Name_AssetBrowser)->Initialize();
}

void MGUIManager::UpdateStaticGUI(float DeltaTime)
{
	for (GUI_BaseUPtr& gui : mManagedList | std::ranges::views::values)
	{
		gui->Update(DeltaTime);
	}
}

void MGUIManager::Update(float_t DeltaTime)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	UpdateStaticGUI(DeltaTime);
}

void MGUIManager::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void MGUIManager::Render()
{
	ImGui::Render(); // ImGui 렌더링 명령 생성
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // 생성된 명령(GetDrawData)을 GPU에 전달

	// Update and Present additional Platform Windows
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows(); // 새 뷰포트 생성 | 기존 뷰포트 상태, 위치 업데이트 | 필요없는 뷰포트 파괴 등
		ImGui::RenderPlatformWindowsDefault(); // 각 뷰포트 렌더
	}
}

void MGUIManager::AddGUI(EGUIType InType)
{}

void MGUIManager::HideGUI(EGUIType InType)
{}

void MGUIManager::DeleteGUI(EGUIType InType)
{}

void MGUIManager::ScaleAllSize(float InScale)
{
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(InScale);

}
