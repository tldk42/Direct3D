#include "common_pch.h"
#include "GUI_Viewport.h"

#include "Core/JCamera.h"
#include "Core/Graphics/GraphicDevice.h"
#include "Core/Graphics/Viewport/MViewportManager.h"
#include "Core/Interface/MManagerInterface.h"

JCamera g_EditorMainCamera;

GUI_Viewport::GUI_Viewport(const std::string& InTitle)
	: GUI_Base(InTitle),
	  bIsFocused(false),
	  bIsHovered(false) {}

GUI_Viewport::~GUI_Viewport()
{
	g_EditorMainCamera.Release();
}

void GUI_Viewport::Initialize()
{
	g_EditorMainCamera.Initialize();
}

void GUI_Viewport::Update(float DeltaTime)
{
	ImGui::Begin(mTitle.c_str());
	{
		bIsFocused = ImGui::IsWindowFocused();
		bIsHovered = ImGui::IsWindowHovered();

		ImVec2 curSize = ImGui::GetContentRegionAvail();

		FViewportData* viewportData = IManager.ViewportManager.FetchResource(Name_Editor_Viewport);
		if (!viewportData)
		{
			ImGui::End();
			LOG_CORE_FATAL("Invalid Viewport");
		}

		ImGui::Image(viewportData->SRV.Get(), curSize);

		if (ImGui::IsItemHovered() || ImGui::IsItemFocused())
		{
			g_EditorMainCamera.Update(DeltaTime);
		}

		ImGui::End();
	}
}
