#include "common_pch.h"
#include "GUI_Viewport.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Viewport/XViewport.h"

constexpr JTextView EditorViewport = "Editor Viewport";

GUI_Viewport::GUI_Viewport(const char* InTitle)
	: GUI_Base(InTitle) {}

void GUI_Viewport::Initialize()
{
	G_Viewports.CreateNewViewport(EditorViewport, 1920, 1080);
}

void GUI_Viewport::Render()
{
	ImGui::Begin(mTitle);
	{
		bIsFocused = ImGui::IsWindowFocused();
		bIsHovered = ImGui::IsWindowHovered();

		ImVec2 curSize = ImGui::GetContentRegionAvail();

		ImGui::Image(G_Viewports.GetViewportSRV(EditorViewport), curSize);

		ImGui::End();
	}
}
